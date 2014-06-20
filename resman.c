/*
 * Freescale Management Complex (MC) resource manager tool
 *
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 * Author: German Rivera <German.Rivera@freescale.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include "fsl_mc_io_wrapper.h"
#include "fsl_mc_cmd_wrappers.h"
#include "fsl_mc_ioctl.h"
#include "utils.h"

/**
 * Bit masks for command-line options:
 */
#define OPT_HELP_MASK		    ONE_BIT_MASK(OPT_HELP)
#define OPT_VERSION_MASK	    ONE_BIT_MASK(OPT_VERSION)
#define OPT_RESOURCES_MASK	    ONE_BIT_MASK(OPT_RESOURCES)
#define OPT_CONTAINER_MASK	    ONE_BIT_MASK(OPT_CONTAINER)
#define OPT_SOURCE_CONTAINER_MASK   ONE_BIT_MASK(OPT_SOURCE_CONTAINER)
#define OPT_DEST_CONTAINER_MASK	    ONE_BIT_MASK(OPT_DEST_CONTAINER)

/**
 * Maximum level of nesting of DPRCs
 */
#define MAX_DPRC_NESTING	    16

/**
 * Command line option indices for getopt_long()
 */
enum cmd_line_options {
	OPT_HELP = 0,
	OPT_VERSION,
	OPT_RESOURCES,
	OPT_CONTAINER,
	OPT_SOURCE_CONTAINER,
	OPT_DEST_CONTAINER,
	/*
	 * NOTE: New entries must be added above this entry.
	 */
	NUM_CMD_LINE_OPTIONS
};

C_ASSERT(NUM_CMD_LINE_OPTIONS <= 32);

/**
 * Global state of the resman tool
 */
struct resman {
	/**
	 * Bit mask of command-line options not consumed yet
	 */
	uint32_t cmd_line_options_mask;

	/**
	 * Array of option arguments for options found in the command line,
	 * that have arguments. One entry per option.
	 */
	const char *cmd_line_option_arg[NUM_CMD_LINE_OPTIONS];

	/**
	 * resman command found in the command line
	 */
	const char *cmd_name;

	/**
	 * Number of arguments specified for the command found in the command
	 * line
	 */
	int num_cmd_args;

	/**
	 * Array of arguments for the resman command found in the command line
	 */
	char *const *cmd_args;

	/**
	 * MC I/O portal
	 */
	struct mc_portal_wrapper mc_portal;

	/**
	 * Id for the root DPRC in the system
	 */
	uint16_t root_dprc_id;

	/**
	 * Handle for the root DPRC in the system
	 */
	uint16_t root_dprc_handle;
};

typedef int resman_cmd_func_t(void);

struct resman_command {
	char *name;
	resman_cmd_func_t *func;
};

static struct resman resman = {
	.cmd_line_options_mask = 0x0,
	.cmd_name = NULL,
	.num_cmd_args = 0,
};

static const char resman_version[] = "0.1";

static struct option getopt_long_options[] = {
	[OPT_HELP] = {
		.name =  "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 'h',
	},

	[OPT_VERSION] = {
		.name = "version",
		.has_arg = 0,
		.flag = NULL,
		.val = 'v',
	},

	[OPT_RESOURCES] = {
		.name = "resources",
		.has_arg = 0,
		.flag = NULL,
		.val = 'r',
	},

	[OPT_CONTAINER] = {
		.name = "container",
		.has_arg = 1,
		.flag = NULL,
		.val = 'c',
	},

	[OPT_SOURCE_CONTAINER] = {
		.name = "source",
		.has_arg = 1,
		.flag = NULL,
		.val = 's',
	},

	[OPT_DEST_CONTAINER] = {
		.name = "dest",
		.has_arg = 1,
		.flag = NULL,
		.val = 'd',
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(getopt_long_options) == NUM_CMD_LINE_OPTIONS + 1);

static void print_unexpected_options_error(uint32_t options_mask)
{
#	define PRINT_OPTION(_opt_mask, _opt_index) \
	do {								\
		if (options_mask & (_opt_mask)) {			\
			fprintf(stderr, "\t%c, %s\n",			\
				getopt_long_options[_opt_index].val,	\
				getopt_long_options[_opt_index].name);  \
		}							\
	} while (0)

	ERROR_PRINTF("Invalid options:\n");
	PRINT_OPTION(OPT_HELP_MASK, OPT_HELP);
	PRINT_OPTION(OPT_VERSION_MASK, OPT_VERSION);
	PRINT_OPTION(OPT_RESOURCES_MASK, OPT_RESOURCES);
	PRINT_OPTION(OPT_CONTAINER_MASK, OPT_CONTAINER);
	PRINT_OPTION(OPT_SOURCE_CONTAINER_MASK, OPT_SOURCE_CONTAINER);
	PRINT_OPTION(OPT_DEST_CONTAINER_MASK, OPT_DEST_CONTAINER);

#	undef PRINT_OPTION
}

static void print_usage(void)
{
	static const char usage_msg[] =
		"resman [OPTION]... <command> [ARG]...\n"
		"\n"
		"General options:\n"
		"-h, --help\tPrint this message\n"
		"-v, --version\tPrint version of the resman tool\n"
		"\n"
		"Commands:\n"
		"list\tList all containers (DPRC objects) in the system.\n"
		"show [-r] <container>\n"
		"\tDisplay the contents of a DPRC/container\n"
		"\tOptions:\n"
		"\t-r, --resources\n"
		"\t\tDisplay resources instead of objects\n"
		"\tNOTE: Use 0.dprc for the global container.\n"
		"info <object>\n"
		"\tShow general info about an MC object.\n"
		"create <object type> [-c]\n"
		"\tCreate a new MC object of the given type.\n"
		"\tOptions:\n"
		"\t-c <container>, --container=<container>\n"
		"\t\tContainer in which the object is to be created\n"
		"destroy <object>\n"
		"\tDestroy an MC object.\n"
		"move <object> -s <source container> -d <destination container>\n"
		"\tMove a non-DPRC MC object from one container to another.\n"
		"\tOptions:\n"
		"\t-s <container>, --source=<container>\n"
		"\t\tContainer in which the object currently exists (source)\n"
		"\t-d <container>, --dest=<container>\n"
		"\t\tContainer to which object is to be moved (destination)\n"
		"\tNOTE: source and destination containers must have parent-child relationship.\n"
		"\n";

	printf(usage_msg);

	resman.cmd_line_options_mask &= ~OPT_HELP_MASK;
	if (resman.cmd_line_options_mask != 0)
		ERROR_PRINTF("Extra options ignored\n");
}

static void print_version(void)
{
	printf("Freescale MC resman tool version %s\n", resman_version);

	resman.cmd_line_options_mask &= ~OPT_VERSION_MASK;
	if (resman.cmd_line_options_mask != 0)
		ERROR_PRINTF("Extra options ignored\n");
}

/**
 * Lists nested DPRCs inside a given DPRC, recursively
 */
static int list_dprc(uint32_t dprc_id, uint16_t dprc_handle,
		     int nesting_level, bool show_non_dprc_objects)
{
	uint32_t num_child_devices;
	int error = 0;

	assert(nesting_level <= MAX_DPRC_NESTING);

	for (int i = 0; i < nesting_level; i++)
		printf("  ");

	printf("%u.dprc\n", dprc_id);

	error = dprc_get_device_count(&resman.mc_portal,
				      dprc_handle,
				      &num_child_devices);
	if (error < 0) {
		ERROR_PRINTF("dprc_get_device_count() failed with error %d\n",
			     error);
		goto out;
	}

	for (uint32_t i = 0; i < num_child_devices; i++) {
		struct dprc_dev_desc dev_desc;
		uint16_t child_dprc_handle;
		int error2;

		error = dprc_get_device(
				&resman.mc_portal,
				dprc_handle,
				i,
				&dev_desc);
		if (error < 0) {
			ERROR_PRINTF(
				"dprc_get_device(%u) failed with error %d\n",
				i, error);
			goto out;
		}

		if (strcmp(dev_desc.type, "dprc") != 0) {
			if (show_non_dprc_objects) {
				for (int i = 0; i < nesting_level + 1; i++)
					printf("  ");

				printf("%u.%s\n", dev_desc.id, dev_desc.type);
			}

			continue;
		}

		error = dprc_open(&resman.mc_portal,
				  dev_desc.id,
				  &child_dprc_handle);
		if (error < 0) {
			ERROR_PRINTF(
				"dprc_open() failed for %u.dprc with error %d\n",
				dev_desc.id, error);
			goto out;
		}

		error = list_dprc(dev_desc.id, child_dprc_handle,
				  nesting_level + 1, show_non_dprc_objects);

		error2 = dprc_close(&resman.mc_portal, child_dprc_handle);
		if (error2 < 0) {
			ERROR_PRINTF("dprc_close() failed with error %d\n",
				     error2);
			if (error == 0)
				error = error2;

			goto out;
		}
	}

out:
	return error;
}

static int cmd_list_containers(void)
{
	int error;

	if (resman.num_cmd_args != 0) {
		ERROR_PRINTF("Unexpected arguments\n");
		error = -EINVAL;
		goto out;
	}

	if (resman.cmd_line_options_mask != 0) {
		print_unexpected_options_error(
			resman.cmd_line_options_mask);
		error = -EINVAL;
		goto out;
	}

	error = list_dprc(resman.root_dprc_id, resman.root_dprc_handle, 0, false);
out:
	return error;
}

/**
 * List resources of all types found in the container specified by dprc_handle
 */
static int list_mc_resources(uint16_t dprc_handle)
{
	static const char *res_names[] = {
		"mcp",
		"swp",
		"bp",
		"swpch",
		"fq",
		"qpr",
		"qd",
		"cg",
		"cqch",
		"rplr",
		"ifp.wr0",
		/*"port.dpsw0",*/
		"kp.wr0.ctlue",
		"kp.wr0.ctlui",
		"kp.aiop0.ctlu",
		"kp.aiop0.mflu",
		/*"plp",*/
		"prp.wr0.ctlue",
		"prp.wr0.ctlui",
		"prp.aiop0.ctlu",
		"prp.aiop0.mflu",
		/*"pp",*/
		"plcy.wr0.ctlui",
		"plcye.wr0.ctlui",
		"dpni",
		"dpcon",
		"dpci",
		"dpseci",
		"dpio",
		"dpbp",
		"dpsw",
		"dpdmux",
		"dpmac",
		"dprc",
	};

	int error;

	if (resman.cmd_line_options_mask != 0) {
		print_unexpected_options_error(resman.cmd_line_options_mask);
		error = -EINVAL;
		goto out;
	}

	for (unsigned int i = 0; i < ARRAY_SIZE(res_names); i++) {
	}

	/*TODO: implement this: */
	DEBUG_PRINTF("%s not implemented yet %d\n", __func__, dprc_handle);
	error = ENOTSUP;
out:
	return error;
}

static int cmd_show_container(void)
{
	int n;
	uint32_t num_child_devices;
	uint32_t dprc_id;
	uint16_t dprc_handle;
	const char *dprc_name;
	int error = 0;
	bool dprc_opened = false;

	if (resman.num_cmd_args == 0) {
		ERROR_PRINTF("<container> argument missing\n");
		error = -EINVAL;
		goto out;
	}

	if (resman.num_cmd_args > 1) {
		ERROR_PRINTF("Invalid number of arguments: %d\n",
			     resman.num_cmd_args);
		error = -EINVAL;
		goto out;
	}

	dprc_name = resman.cmd_args[0];
	n = sscanf(dprc_name, "%u.dprc", &dprc_id);
	if (n != 1) {
		ERROR_PRINTF("Invalid DPRC object name: %s\n",
			     resman.cmd_args[0]);
		error = -EINVAL;
		goto out;
	}

	if (dprc_id != resman.root_dprc_id) {
		error = dprc_open(&resman.mc_portal,
				  dprc_id,
				  &dprc_handle);
		if (error < 0) {
			ERROR_PRINTF(
				"dprc_open() failed for %u.dprc with error %d\n",
				dprc_id, error);
			goto out;
		}

		dprc_opened = true;
	} else {
		dprc_handle = resman.root_dprc_handle;
	}

	if (resman.cmd_line_options_mask & OPT_RESOURCES_MASK) {
		resman.cmd_line_options_mask &= ~OPT_RESOURCES_MASK;
		return list_mc_resources(dprc_handle);
	} else {
		if (resman.cmd_line_options_mask != 0) {
			print_unexpected_options_error(
				resman.cmd_line_options_mask);
			error = -EINVAL;
			goto out;
		}
	}

	error = dprc_get_device_count(&resman.mc_portal,
				      dprc_handle,
				      &num_child_devices);
	if (error < 0) {
		ERROR_PRINTF("dprc_get_device_count() failed with error %d\n",
			     error);
		goto out;
	}

	printf("%s contains %u objects%c\n", dprc_name, num_child_devices,
	       num_child_devices == 0 ? '.' : ':');

	for (uint32_t i = 0; i < num_child_devices; i++) {
		struct dprc_dev_desc dev_desc;

		error = dprc_get_device(
				&resman.mc_portal,
				dprc_handle,
				i,
				&dev_desc);
		if (error < 0) {
			ERROR_PRINTF(
				"dprc_get_device(%u) failed with error %d\n",
				i, error);
			goto out;
		}

		printf("%u.%s\n", dev_desc.id, dev_desc.type);
	}

	error = 0;
out:
	if (dprc_opened) {
		int error2;

		error2 = dprc_close(&resman.mc_portal, dprc_handle);
		if (error2 < 0) {
			ERROR_PRINTF("dprc_close() failed with error %d\n",
				     error2);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int cmd_info_object(void)
{
	ERROR_PRINTF("command '\%s\' not implemented yet\n", resman.cmd_name);
	return ENOTSUP;
}

static int cmd_create_object(void)
{
	ERROR_PRINTF("command '\%s\' not implemented yet\n", resman.cmd_name);
	return ENOTSUP;
}

static int cmd_destroy_object(void)
{
	ERROR_PRINTF("command '\%s\' not implemented yet\n", resman.cmd_name);
	return ENOTSUP;
}

static int cmd_move_object(void)
{
	ERROR_PRINTF("command '\%s\' not implemented yet\n", resman.cmd_name);
	return ENOTSUP;
}

static const struct resman_command resman_commands[] = {
	{ "list", cmd_list_containers },
	{ "show", cmd_show_container },
	{ "info", cmd_info_object },
	{ "create", cmd_create_object },
	{ "destroy", cmd_destroy_object },
	{ "move", cmd_move_object },
};

static int parse_cmd_line(int argc, char *argv[])
{
	int c;
	int error = 0;
	resman_cmd_func_t *cmd_func = NULL;

	for (;;) {
		c = getopt_long(argc, argv, "hvrc:s:d:", getopt_long_options, NULL);
		if (c == -1)
			break;

		switch (c) {
		case 'h':
			resman.cmd_line_options_mask |= OPT_HELP_MASK;
			break;

		case 'v':
			resman.cmd_line_options_mask |= OPT_VERSION_MASK;
			break;

		case 'r':
			resman.cmd_line_options_mask |= OPT_RESOURCES_MASK;
			break;

		case 'c':
			resman.cmd_line_options_mask |= OPT_CONTAINER_MASK;
			resman.cmd_line_option_arg[OPT_CONTAINER] = optarg;
			break;

		case 's':
			resman.cmd_line_options_mask |= OPT_SOURCE_CONTAINER_MASK;
			resman.cmd_line_option_arg[OPT_SOURCE_CONTAINER] = optarg;
			break;

		case 'd':
			resman.cmd_line_options_mask |= OPT_DEST_CONTAINER_MASK;
			resman.cmd_line_option_arg[OPT_DEST_CONTAINER] = optarg;
			break;

		case '?':
			error = -EINVAL;
			goto out;
	       default:
			assert(false);
		}
	}

	if (resman.cmd_line_options_mask & OPT_HELP_MASK) {
		print_usage();
		goto out;
	}

	if (resman.cmd_line_options_mask & OPT_VERSION_MASK) {
		print_version();
		goto out;
	}


	if (optind == argc) {
		ERROR_PRINTF("resman command missing\n");
		error = -EINVAL;
		goto out;
	}

	assert(optind < argc);
	resman.cmd_name = argv[optind];
	resman.num_cmd_args = argc - (optind + 1);
	if (resman.num_cmd_args != 0)
		resman.cmd_args = &argv[optind + 1];

	//???
	DEBUG_PRINTF("cmd: %s\n", resman.cmd_name);
	for (int i = 0; i < resman.num_cmd_args; i++)
		DEBUG_PRINTF("cmd arg[%d]: %s\n", i, resman.cmd_args[i]);
	//???

	/*
	 * Lookup command:
	 */
	for (unsigned int i = 0; i < ARRAY_SIZE(resman_commands); i++) {
		if (strcmp(resman.cmd_name, resman_commands[i].name) == 0) {
			cmd_func = resman_commands[i].func;
			break;
		}
	}

	if (cmd_func == NULL) {
		ERROR_PRINTF("Invalid command \'%s\'\n", resman.cmd_name);
		error = -EINVAL;
		goto out;
	}

	/*
	 * Execute command:
	 */
	error = cmd_func();
out:
	return error;
}

int main(int argc, char *argv[])
{
	int error;
	bool mc_portal_initialized = false;
	struct ioctl_dprc_info root_dprc_info = { 0 };

	DEBUG_PRINTF("resman built on " __DATE__ " " __TIME__ "\n");
	error = mc_portal_wrapper_init(&resman.mc_portal);
	if (error != 0)
		goto out;

	mc_portal_initialized = true;
	DEBUG_PRINTF("mc_portal regs: %p\n", resman.mc_portal.mmio_regs);
	DEBUG_PRINTF("calling ioctl\n");
	error = ioctl(resman.mc_portal.fd, RESMAN_GET_ROOT_DPRC_INFO,
		      &root_dprc_info);
	if (error == -1) {
		error = -errno;
		goto out;
	}

	DEBUG_PRINTF("ioctl returned dprc_id: %#x, dprc_handle: %#x\n",
		     root_dprc_info.dprc_id,
		     root_dprc_info.dprc_handle);

	resman.root_dprc_id = root_dprc_info.dprc_id;
	resman.root_dprc_handle = root_dprc_info.dprc_handle;

	error = parse_cmd_line(argc, argv);
out:
	if (mc_portal_initialized)
		mc_portal_wrapper_cleanup(&resman.mc_portal);

	return error;
}


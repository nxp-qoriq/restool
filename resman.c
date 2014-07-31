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
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include "utils.h"
#include "fsl_mc_io.h"
#include "fsl_mc_cmd.h"
#include "fsl_mc_ioctl.h"
#include "fsl_dprc.h"
#include "fsl_dpmng.h"

/**
 * Bit masks for command-line options:
 */
#define OPT_HELP_MASK		    ONE_BIT_MASK(OPT_HELP)
#define OPT_VERSION_MASK	    ONE_BIT_MASK(OPT_VERSION)
#define OPT_RESOURCES_MASK	    ONE_BIT_MASK(OPT_RESOURCES)
#define OPT_VERBOSE_MASK	    ONE_BIT_MASK(OPT_VERBOSE)
#define OPT_TYPE_MASK		    ONE_BIT_MASK(OPT_TYPE)
#define OPT_CONTAINER_MASK	    ONE_BIT_MASK(OPT_CONTAINER)
#define OPT_SOURCE_CONTAINER_MASK   ONE_BIT_MASK(OPT_SOURCE_CONTAINER)
#define OPT_DEST_CONTAINER_MASK	    ONE_BIT_MASK(OPT_DEST_CONTAINER)

#define ALL_DPRC_OPTS (			\
	DPRC_CFG_OPT_SPAWN_ALLOWED |		\
	DPRC_CFG_OPT_ALLOC_ALLOWED |		\
	DPRC_CFG_OPT_OBJ_CREATE_ALLOWED |	\
	DPRC_CFG_OPT_TOPOLOGY_CHANGES_ALLOWED |	\
	DPRC_CFG_OPT_IOMMU_BYPASS |		\
	DPRC_CFG_OPT_AIOP)

/**
 * Maximum level of nesting of DPRCs
 */
#define MAX_DPRC_NESTING	    16

/**
 * MC object type string max length (without including the null terminator)
 */
#define OBJ_TYPE_MAX_LENGTH	4

/**
 * MC resource type string max length (without including the null terminator)
 */
#define RES_TYPE_MAX_LENGTH	15

/*
 * TODO: Obtain the following constants from the fsl-mc bus driver via an ioctl
 */
#define MC_PORTALS_BASE_PADDR	((phys_addr_t)0x00080C000000ULL)
#define MC_PORTAL_STRIDE	0x10000
#define MC_PORTAL_SIZE		64
#define MAX_MC_PORTALS		512

#define MC_PORTAL_PADDR_TO_PORTAL_ID(_portal_paddr) \
	(((_portal_paddr) - MC_PORTALS_BASE_PADDR) / MC_PORTAL_STRIDE)

/**
 * Command line option indices for getopt_long()
 */
enum cmd_line_options {
	OPT_HELP = 0,
	OPT_VERSION,
	OPT_RESOURCES,
	OPT_VERBOSE,
	OPT_TYPE,
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
	uint32_t option_mask;

	/**
	 * Array of option arguments for options found in the command line,
	 * that have arguments. One entry per option.
	 */
	const char *option_arg[NUM_CMD_LINE_OPTIONS];

	/**
	 * resman command found in the command line
	 */
	const char *cmd_name;

	/**
	 * Number of non-option arguments found in the command line,
	 * excluding the command (e.g. list, show, info) itself.
	 */
	int num_non_option_arg;

	/**
	 * Array of non-option arguments found in the command line,
	 * excluding the command (e.g. list, show, info) itself.
	 */
	char *const *non_option_arg;

	/**
	 * MC I/O portal
	 */
	struct mc_io mc_io;

	/**
	 * MC firmware version
	 */
	struct mc_version mc_fw_version;

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
	.option_mask = 0x0,
	.cmd_name = NULL,
	.num_non_option_arg = 0,
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

	[OPT_VERBOSE] = {
		.name = "verbose",
		.has_arg = 0,
		.flag = NULL,
		.val = 'x',
	},

	[OPT_TYPE] = {
		.name = "type",
		.has_arg = 1,
		.flag = NULL,
		.val = 't',
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

static void print_unexpected_options_error(uint32_t option_mask)
{
#	define PRINT_OPTION(_opt_mask, _opt_index) \
	do {								\
		if (option_mask & (_opt_mask)) {			\
			fprintf(stderr, "\t-%c, --%s\n",		\
				getopt_long_options[_opt_index].val,	\
				getopt_long_options[_opt_index].name);  \
		}							\
	} while (0)

	ERROR_PRINTF("Invalid options:\n");
	PRINT_OPTION(OPT_HELP_MASK, OPT_HELP);
	PRINT_OPTION(OPT_VERSION_MASK, OPT_VERSION);
	PRINT_OPTION(OPT_RESOURCES_MASK, OPT_RESOURCES);
	PRINT_OPTION(OPT_VERBOSE_MASK, OPT_VERBOSE);
	PRINT_OPTION(OPT_TYPE_MASK, OPT_TYPE);
	PRINT_OPTION(OPT_CONTAINER_MASK, OPT_CONTAINER);
	PRINT_OPTION(OPT_SOURCE_CONTAINER_MASK, OPT_SOURCE_CONTAINER);
	PRINT_OPTION(OPT_DEST_CONTAINER_MASK, OPT_DEST_CONTAINER);

#	undef PRINT_OPTION
}

static int parse_object_name(const char *obj_name, char *expected_obj_type,
			     char *obj_type, uint32_t *obj_id)
{
	int n;

	n = sscanf(obj_name, "%" STRINGIFY(OBJ_TYPE_MAX_LENGTH) "s.%u",
		   obj_type, obj_id);
	if (n != 2) {
		ERROR_PRINTF("Invalid MC object name: %s\n", obj_name);
		return -EINVAL;
	}

	if (expected_obj_type != NULL && strcmp(obj_type, expected_obj_type) != 0) {
		ERROR_PRINTF("Expected \'%s\' object type\n", expected_obj_type);
		return -EINVAL;
	}

	return 0;
}


static void print_usage(void)
{
	static const char usage_msg[] =
		"usage: resman <command> [OPTION] [ARG]\n"
		"\n"
		"General options:\n"
		"  -h, --help\tPrint this message\n"
		"\te.g. resman --help\n"
		"  -v, --version\tPrint version of the resman tool\n"
		"\te.g. resman --version\n"
		"\n"
		"Commands:\n"
		"  list\tList all containers (DPRC objects) in the system.\n"
		"  show <container> [-r] [-t] <resouce-type>\n"
		"\tDisplay the contents of a DPRC/container\n"
		"\tnon-option: display the objects of DPRC/container\n"
		"\t\te.g. show dprc.2\n"
		"\tOptions:\n"
		"\t-r, --resources\n"
		"\t\tDisplay general count of each resource\n"
		"\t\te.g. show dprc.2 -r\n"
		"\t-t, --type\n"
		"\t\tDisplay detailed resource of the specified type\n"
		"\t\te.g. show dprc.2 -t mcp\n"
		"\tNOTE: Use dprc.0 for the global container.\n"
		"  info <object> [--verbose]\n"
		"\tnon-option: show general info about an MC object.\n"
		"\t\te.g. info dprc.2\n"
		"\t\te.g. info dpni.7\n"
		"\tOption: -x, --verbose\n"
		"\tShow general and verbose info about an MC object.\n"
		"\t\te.g. info dprc.2 --verbose\n"
		"\t\te.g. info dpni.7 --verbose\n"
		/*"  create <object type> [-c]\n"
		"\tCreate a new MC object of the given type.\n"
		"\tOptions:\n"
		"\t-c <container>, --container=<container>\n"
		"\t\tContainer in which the object is to be created\n"
		"  destroy <object>\n"
		"\tDestroy an MC object.\n"
		"  move <object> -s <source container> -d <destination container>\n"
		"\tMove a non-DPRC MC object from one container to another.\n"
		"\tOptions:\n"
		"\t-s <container>, --source=<container>\n"
		"\t\tContainer in which the object currently exists (source)\n"
		"\t-d <container>, --dest=<container>\n"
		"\t\tContainer to which object is to be moved (destination)\n"
		"\tNOTE: source and destination containers must have parent-child relationship.\n"*/
		"\n";

	printf(usage_msg);

	resman.option_mask &= ~OPT_HELP_MASK;
	if (resman.option_mask != 0)
		ERROR_PRINTF("Extra options ignored\n");
}

static void print_version(void)
{
	printf("Freescale MC resman tool version %s\n", resman_version);
	printf("MC firmware version: %u.%u.%u\n",
	       resman.mc_fw_version.major,
	       resman.mc_fw_version.minor,
	       resman.mc_fw_version.revision);

	resman.option_mask &= ~OPT_VERSION_MASK;
	if (resman.option_mask != 0)
		ERROR_PRINTF("Extra options ignored\n");
}

static int open_dprc(uint32_t dprc_id, uint16_t *dprc_handle)
{
	int error;

	error = dprc_open(&resman.mc_io,
			  dprc_id,
			  dprc_handle);
	if (error < 0) {
		ERROR_PRINTF(
			"dprc_open() failed for dprc.%u with error %d\n",
			dprc_id, error);
		goto out;
	}

	if (*dprc_handle == 0) {
		ERROR_PRINTF("dprc_open() returned invalid handle (auth 0) for "
			     "dprc.%u\n", dprc_id);

		(void)dprc_close(&resman.mc_io, *dprc_handle);
		error = -ENOENT;
		goto out;
	}

	error = 0;
out:
	return error;
}

/**
 * Lists nested DPRCs inside a given DPRC, recursively
 */
static int list_dprc(uint32_t dprc_id, uint16_t dprc_handle,
		     int nesting_level, bool show_non_dprc_objects)
{
	int num_child_devices;
	int error = 0;

	assert(nesting_level <= MAX_DPRC_NESTING);

	for (int i = 0; i < nesting_level; i++)
		printf("  ");

	printf("dprc.%u\n", dprc_id);

	error = dprc_get_obj_count(&resman.mc_io,
				   dprc_handle,
				   &num_child_devices);
	if (error < 0) {
		ERROR_PRINTF("dprc_get_object_count() failed with error %d\n",
			     error);
		goto out;
	}

	for (int i = 0; i < num_child_devices; i++) {
		struct dprc_obj_desc obj_desc;
		uint16_t child_dprc_handle;
		int error2;

		error = dprc_get_obj(
				&resman.mc_io,
				dprc_handle,
				i,
				&obj_desc);
		if (error < 0) {
			ERROR_PRINTF(
				"dprc_get_object(%u) failed with error %d\n",
				i, error);
			goto out;
		}

		if (strcmp(obj_desc.type, "dprc") != 0) {
			if (show_non_dprc_objects) {
				for (int i = 0; i < nesting_level + 1; i++)
					printf("  ");

				printf("%s.%u\n", obj_desc.type, obj_desc.id);
			}

			continue;
		}

		error = open_dprc(obj_desc.id, &child_dprc_handle);
		if (error < 0)
			goto out;

		error = list_dprc(obj_desc.id, child_dprc_handle,
				  nesting_level + 1, show_non_dprc_objects);

		error2 = dprc_close(&resman.mc_io, child_dprc_handle);
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

	if (resman.num_non_option_arg != 0) {
		ERROR_PRINTF("Unexpected arguments\n");
		error = -EINVAL;
		goto out;
	}

	if (resman.option_mask != 0) {
		print_unexpected_options_error(
			resman.option_mask);
		error = -EINVAL;
		goto out;
	}

	error = list_dprc(resman.root_dprc_id, resman.root_dprc_handle, 0, false);
out:
	return error;
}


static int show_one_resource_type(uint16_t dprc_handle,
				      const char *mc_res_type)
{
	int res_count;
	int res_discovered_count;
	struct dprc_res_ids_range_desc range_desc;
	int error;

	error = dprc_get_res_count(&resman.mc_io, dprc_handle,
				   (char *)mc_res_type, &res_count);
	if (error < 0) {
		ERROR_PRINTF("dprc_get_res_count() failed: %d\n", error);
		goto out;
	}

	if (res_count == 0) {
		printf("Don't have any %s resource\n", mc_res_type);
		goto out;
	}

	memset(&range_desc, 0, sizeof(struct dprc_res_ids_range_desc));
	res_discovered_count = 0;
	do {
		int id;

		error = dprc_get_res_ids(&resman.mc_io, dprc_handle,
					 (char *)mc_res_type, &range_desc);
		if (error < 0) {
			ERROR_PRINTF("dprc_get_res_ids() failed: %d\n", error);
			goto out;
		}

		for (id = range_desc.base_id; id <= range_desc.last_id; id++) {
			printf("%s.%d\n", mc_res_type, id);
			res_discovered_count++;
		}
	} while (res_discovered_count < res_count &&
		 range_desc.iter_status != DPRC_ITER_STATUS_LAST);
out:
	return error;
}

static int show_one_resource_type_count(uint16_t dprc_handle,
				      const char *mc_res_type)
{
	int res_count;
	int error;

	error = dprc_get_res_count(&resman.mc_io, dprc_handle,
				   (char *)mc_res_type, &res_count);
	if (error < 0) {
		ERROR_PRINTF("dprc_get_res_count() failed: %d\n", error);
		goto out;
	}

	if (res_count == 0)
		goto out;

	printf("%s: %d\n",mc_res_type, res_count);

out:
	return error;
}

/**
 * List resources of all types found in the container specified by dprc_handle
 */
static int show_mc_resources(uint16_t dprc_handle)
{
	int pool_count;
	char res_type[RES_TYPE_MAX_LENGTH + 1];
	int error;

	error = dprc_get_pool_count(&resman.mc_io, dprc_handle,
				    &pool_count);
	if (error < 0) {
		ERROR_PRINTF("dprc_get_pool_count() failed: %d\n", error);
		goto out;
	}

	assert(pool_count > 0);
	for (int i = 0; i < pool_count; i++) {
		res_type[sizeof(res_type) - 1] = '\0';
		error = dprc_get_pool(&resman.mc_io, dprc_handle,
				      i, res_type);

		assert(res_type[sizeof(res_type) - 1] == '\0');
		error = show_one_resource_type_count(dprc_handle, res_type);
		if (error < 0)
			goto out;
	}
out:
	return error;
}

static int show_mc_objects(uint16_t dprc_handle, char *dprc_name)
{
	int num_child_devices;
	int error;

	error = dprc_get_obj_count(&resman.mc_io,
				   dprc_handle,
				   &num_child_devices);
	if (error < 0) {
		ERROR_PRINTF("dprc_get_object_count() failed with error %d\n",
			     error);
		goto out;
	}

	printf("%s contains %u objects%c\n", dprc_name, num_child_devices,
	       num_child_devices == 0 ? '.' : ':');

	for (int i = 0; i < num_child_devices; i++) {
		struct dprc_obj_desc obj_desc;

		error = dprc_get_obj(&resman.mc_io,
				     dprc_handle,
				     i,
				     &obj_desc);
		if (error < 0) {
			ERROR_PRINTF(
				"dprc_get_object(%u) failed with error %d\n",
				i, error);
			goto out;
		}

		printf("%s.%u\n", obj_desc.type, obj_desc.id);
	}

	error = 0;
out:
	return error;
}

static int cmd_show_container(void)
{
	uint32_t dprc_id;
	uint16_t dprc_handle;
	char *dprc_name;
	int error = 0;
	bool dprc_opened = false;
	char obj_type[OBJ_TYPE_MAX_LENGTH + 1];
	const char *res_type;

	if (resman.num_non_option_arg == 0) {
		ERROR_PRINTF("<container> argument missing\n");
		error = -EINVAL;
		goto out;
	}

	if (resman.num_non_option_arg > 1) {
		ERROR_PRINTF("Invalid number of non-option arguments: %d\n",
			     resman.num_non_option_arg);
		error = -EINVAL;
		goto out;
	}

	dprc_name = resman.non_option_arg[0];
	error = parse_object_name(dprc_name, "dprc", obj_type, &dprc_id);
	if (error < 0)
		goto out;

	if (dprc_id != resman.root_dprc_id) {
		error = open_dprc(dprc_id, &dprc_handle);
		if (error < 0)
			goto out;

		dprc_opened = true;
	} else {
		dprc_handle = resman.root_dprc_handle;
	}

	if (resman.option_mask == 0) {
		error = show_mc_objects(dprc_handle, dprc_name);
		goto out;
	}

	if (resman.option_mask & OPT_RESOURCES_MASK) {
		resman.option_mask &= ~OPT_RESOURCES_MASK;
		error = show_mc_resources(dprc_handle);
		if (error < 0)
			goto out;
	}

	if (resman.option_mask & OPT_TYPE_MASK) {
		assert(resman.option_arg[OPT_TYPE] != NULL);
		res_type = resman.option_arg[OPT_TYPE];
		resman.option_mask &= ~OPT_TYPE_MASK;
		error = show_one_resource_type(dprc_handle, res_type);
		if (error < 0)
			goto out;
	}

	if (resman.option_mask != 0) {
		print_unexpected_options_error(
			resman.option_mask);
		error = -EINVAL;
		goto out;
	}

out:
	if (dprc_opened) {
		int error2;

		error2 = dprc_close(&resman.mc_io, dprc_handle);
		if (error2 < 0) {
			ERROR_PRINTF("dprc_close() failed with error %d\n",
				     error2);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static void print_dprc_options(uint64_t options)
{
	if (options & ~ALL_DPRC_OPTS) {
		printf("\tUnrecognized options found...\n");
		return;
	}

	if (options & DPRC_CFG_OPT_SPAWN_ALLOWED)
		printf("\tDPRC_CFG_OPT_SPAWN_ALLOWED\n");

	if (options & DPRC_CFG_OPT_ALLOC_ALLOWED)
		printf("\tDPRC_CFG_OPT_ALLOC_ALLOWED\n");

	if (options & DPRC_CFG_OPT_OBJ_CREATE_ALLOWED)
		printf("\tDPRC_CFG_OPT_OBJ_CREATE_ALLOWED\n");

	if (options & DPRC_CFG_OPT_TOPOLOGY_CHANGES_ALLOWED)
		printf("\tDPRC_CFG_OPT_TOPOLOGY_CHANGES_ALLOWED\n");

	if (options & DPRC_CFG_OPT_IOMMU_BYPASS)
		printf("\tDPRC_CFG_OPT_IOMMU_BYPASS\n");

	if (options & DPRC_CFG_OPT_AIOP)
		printf("\tDPRC_CFG_OPT_AIOP\n");
}

static int print_dprc_attr(uint32_t dprc_id)
{
	uint16_t dprc_handle;
	int error;
	struct dprc_attributes dprc_attr;
	bool dprc_opened = false;

	if (dprc_id != resman.root_dprc_id) {
		error = open_dprc(dprc_id, &dprc_handle);
		if (error < 0)
			goto out;

		dprc_opened = true;
	} else {
		dprc_handle = resman.root_dprc_handle;
	}

	memset(&dprc_attr, 0, sizeof(dprc_attr));
	error = dprc_get_attributes(&resman.mc_io, dprc_handle, &dprc_attr);
	if (error < 0) {
		ERROR_PRINTF("dprc_get_attributes() failed: %d\n", error);
		goto out;
	}

	assert(dprc_id == (uint32_t)dprc_attr.container_id);
	printf(
		"container id: %d\n"
		"icid: %u\n"
		"portal id: %d\n"
		"version: %u.%u\n"
		"dprc options: %#llx\n",
		dprc_attr.container_id,
		dprc_attr.icid,
		dprc_attr.portal_id,
		dprc_attr.version.major,
		dprc_attr.version.minor,
		(unsigned long long)dprc_attr.options);
	print_dprc_options(dprc_attr.options);

	error = 0;
out:
	if (dprc_opened) {
		int error2;

		error2 = dprc_close(&resman.mc_io, dprc_handle);
		if (error2 < 0) {
			ERROR_PRINTF("dprc_close() failed with error %d\n",
				     error2);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int print_dprc_verbose(uint16_t dprc_handle,
			     int nesting_level, uint32_t target_id)
{
	int num_child_devices;
	int error = 0;
	uint32_t irq_mask;
	uint32_t irq_status;

	assert(nesting_level <= MAX_DPRC_NESTING);

	error = dprc_get_obj_count(&resman.mc_io,
				   dprc_handle,
				   &num_child_devices);
	if (error < 0) {
		ERROR_PRINTF("dprc_get_object_count() failed with error %d\n",
			     error);
		goto out;
	}

	for (int i = 0; i < num_child_devices; i++) {
		struct dprc_obj_desc obj_desc;
		uint16_t child_dprc_handle;
		int error2;

		error = dprc_get_obj(
				&resman.mc_io,
				dprc_handle,
				i,
				&obj_desc);
		if (error < 0) {
			ERROR_PRINTF(
				"dprc_get_object(%u) failed with error %d\n",
				i, error);
			goto out;
		}

		if (strcmp(obj_desc.type, "dprc") != 0)
			continue;

		if (target_id == (uint32_t)obj_desc.id) {
			printf("number of mappable regions: %u\n",obj_desc.region_count);
			printf("number of interrupts: %u\n", obj_desc.irq_count);

			error = open_dprc(obj_desc.id, &child_dprc_handle);
			if (error < 0)
				goto out;

			for (int j = 0; j < obj_desc.irq_count; j++) {
				dprc_get_irq_mask(&resman.mc_io, child_dprc_handle, j, &irq_mask);
				printf("interrupt %d's mask: %#x\n", j, irq_mask);
				dprc_get_irq_status(&resman.mc_io, child_dprc_handle, j, &irq_status);
				(irq_status == 0) ?
				printf("interrupt %d's status: %#x - no interrupt pending.\n", j, irq_status) :
				(irq_status == 1) ?
				printf("interrupt %d's status: %#x - interrupt pending.\n", j, irq_status) :
				printf("interrupt %d's status: %#x - error status.\n", j, irq_status);
			}

			error2 = dprc_close(&resman.mc_io, child_dprc_handle);
			if (error2 < 0) {
				ERROR_PRINTF("dprc_close() failed with error %d\n",
					     error2);
				if (error == 0)
					error = error2;
			}
			goto out;
		}

		error = open_dprc(obj_desc.id, &child_dprc_handle);
		if (error < 0)
			goto out;

		error = print_dprc_verbose(child_dprc_handle,
					  nesting_level + 1, target_id);

		error2 = dprc_close(&resman.mc_io, child_dprc_handle);
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

static int print_dprc_info(uint32_t dprc_id)
{
	int error;

	error = print_dprc_attr(dprc_id);
	if (error < 0)
		goto out;

	if ((resman.option_mask & OPT_VERBOSE_MASK) &&
		dprc_id == resman.root_dprc_id) {
		resman.option_mask &= ~OPT_VERBOSE_MASK;
		printf("root dprc doesn't have verbose info to display\n");
		goto out;
	}

	if ((resman.option_mask & OPT_VERBOSE_MASK) &&
		dprc_id != resman.root_dprc_id) {
		resman.option_mask &= ~OPT_VERBOSE_MASK;
		error = print_dprc_verbose(resman.root_dprc_handle, 0, dprc_id);
		goto out;
	}
out:
	return error;
}

static int print_dpni_info(uint32_t dpni_id)
{
	printf("coming soon...\n");
	printf("dpni's id is: %u", dpni_id);
	return 0;
}

static int cmd_info_object(void)
{
	int error;
	char *obj_name;
	char obj_type[OBJ_TYPE_MAX_LENGTH + 1];
	uint32_t obj_id;

	if (resman.num_non_option_arg == 0) {
		ERROR_PRINTF("<object> argument missing\n");
		error = -EINVAL;
		goto out;
	}

	if (resman.num_non_option_arg > 1) {
		ERROR_PRINTF("Invalid number of non-option arguments: %d\n",
			     resman.num_non_option_arg);
		error = -EINVAL;
		goto out;
	}

	obj_name = resman.non_option_arg[0];
	error = parse_object_name(obj_name, NULL, obj_type, &obj_id);
	if (error < 0)
		goto out;

	if (strcmp(obj_type, "dprc") == 0) {
		error = print_dprc_info(obj_id);
	} else if (strcmp(obj_type, "dpni") == 0) {
		error = print_dpni_info(obj_id);
	} else {
		ERROR_PRINTF("Unexpected object type \'%s\'\n", obj_type);
		error = -EINVAL;
	}

	if (error < 0)
		goto out;

	if (resman.option_mask != 0) {
		print_unexpected_options_error(resman.option_mask);
		error = -EINVAL;
		goto out;
	}
out:
	return error;
}

/**
 * Create a DPRC object in the MC, as a child of the container
 * referred by 'dprc_handle'.
 */
static int create_dprc(uint16_t dprc_handle)
{
	int error;
	int error2;
	struct dprc_cfg cfg;
	int child_dprc_id;
	uint64_t mc_portal_phys_addr;
	int32_t portal_id;
	bool portal_allocated = false;
	bool child_dprc_created = false;

	assert(dprc_handle != 0);
	error = ioctl(resman.mc_io.fd, RESMAN_ALLOCATE_MC_PORTAL,
		      &portal_id);
	if (error == -1) {
		error = -errno;
		ERROR_PRINTF(
			"ioctl(RESMAN_ALLOCATE_MC_PORTAL) failed with error %d\n",
			error);
		goto error;
	}

	portal_allocated = true;
	DEBUG_PRINTF("ioctl returned portal_id: %u\n", portal_id);

	cfg.icid = DPRC_GET_ICID_FROM_POOL;
	cfg.portal_id = portal_id;
	cfg.options =
		(DPRC_CFG_OPT_SPAWN_ALLOWED | DPRC_CFG_OPT_ALLOC_ALLOWED);

	error = dprc_create_container(
			&resman.mc_io,
			dprc_handle,
			&cfg,
			&child_dprc_id,
			&mc_portal_phys_addr);
	if (error < 0) {
		ERROR_PRINTF(
			"dprc_create_container() failed: %d\n", error);
		goto error;
	}

	child_dprc_created = true;
	printf("dprc.%u object created (using MC portal id %u, portal addr %#llx)\n",
	       child_dprc_id, portal_id, (unsigned long long)mc_portal_phys_addr);

	return 0;
error:
	if (child_dprc_created) {
		error2 = dprc_destroy_container(&resman.mc_io, dprc_handle,
						child_dprc_id);
		if (error2 < 0) {
			ERROR_PRINTF(
			    "dprc_destroy_container() failed with error %d\n",
			    error2);
		}
	}

	if (portal_allocated) {
		error2 = ioctl(resman.mc_io.fd, RESMAN_FREE_MC_PORTAL,
			       portal_id);
		if (error2 == -1) {
			error2 = -errno;
			ERROR_PRINTF(
				"ioctl(RESMAN_FREE_MC_PORTAL) failed with error %d\n",
				error2);
		}
	}

	return error;
}

static int create_dpni(uint16_t dprc_handle)
{
	assert(dprc_handle != 0);
	ERROR_PRINTF("Creation of DPNI objects not implemented yet\n");
	return ENOTSUP;
}

static int cmd_create_object(void)
{
	uint16_t dprc_handle;
	int error;
	char *target_obj_type;
	bool dprc_opened = false;

	if (resman.num_non_option_arg == 0) {
		ERROR_PRINTF("<object type> argument missing\n");
		error = -EINVAL;
		goto out;
	}

	if (resman.num_non_option_arg > 1) {
		ERROR_PRINTF("Invalid number of non-option arguments: %d\n",
			     resman.num_non_option_arg);
		error = -EINVAL;
		goto out;
	}

	target_obj_type = resman.non_option_arg[0];
	if (resman.option_mask & OPT_CONTAINER_MASK) {
		char obj_type[OBJ_TYPE_MAX_LENGTH + 1];
		uint32_t dprc_id;

		assert(resman.option_arg[OPT_CONTAINER] != NULL);
		error = parse_object_name(resman.option_arg[OPT_CONTAINER],
					  "dprc", obj_type, &dprc_id);
		if (error < 0)
			goto out;

		if (dprc_id != resman.root_dprc_id) {
			error = open_dprc(dprc_id, &dprc_handle);
			if (error < 0)
				goto out;

			dprc_opened = true;
		} else {
			dprc_handle = resman.root_dprc_handle;
		}

		resman.option_mask &= ~OPT_CONTAINER_MASK;
	} else {
		dprc_handle = resman.root_dprc_handle;
	}

	if (resman.option_mask != 0) {
		print_unexpected_options_error(resman.option_mask);
		error = -EINVAL;
		goto out;
	}

	if (strcmp(target_obj_type, "dprc") == 0) {
		error = create_dprc(dprc_handle);
	} else if (strcmp(target_obj_type, "dpni") == 0) {
		error = create_dpni(dprc_handle);
	} else {
		ERROR_PRINTF("Unexpected object type \'%s\'\n", target_obj_type);
		error = -EINVAL;
	}
out:
	if (dprc_opened) {
		int error2;

		error2 = dprc_close(&resman.mc_io, dprc_handle);
		if (error2 < 0) {
			ERROR_PRINTF("dprc_close() failed with error %d\n",
				     error2);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int destroy_dprc(uint16_t parent_dprc_handle, int child_dprc_id)
{
	int error;
	uint16_t child_dprc_handle;
	struct dprc_attributes dprc_attr;
	bool dprc_opened = false;

	assert(parent_dprc_handle != 0);

	/*
	 * Before destroying the child container, get its MC portal id.
	 * We need to notify the fsl_mc_resman kernel driver that this
	 * MC portal has become available for reallocation, once we destroy it:
	 */

	error = open_dprc(child_dprc_id, &child_dprc_handle);
	if (error < 0)
		goto error;

	dprc_opened = true;
	memset(&dprc_attr, 0, sizeof(dprc_attr));
	error = dprc_get_attributes(&resman.mc_io, child_dprc_handle, &dprc_attr);
	if (error < 0) {
		ERROR_PRINTF("dprc_get_attributes() failed: %d\n", error);
		goto error;
	}

	assert(child_dprc_id == dprc_attr.container_id);
	dprc_opened = false;
	error = dprc_close(&resman.mc_io, child_dprc_handle);
	if (error < 0) {
		ERROR_PRINTF("dprc_close() failed with error %d\n", error);
		goto error;
	}

	/*
	 * Destroy child container in the MC:
	 */
	error = dprc_destroy_container(&resman.mc_io, parent_dprc_handle,
					child_dprc_id);
	if (error < 0) {
		ERROR_PRINTF(
		    "dprc_destroy_container() failed with error %d\n",
		    error);

		goto error;
	}

	printf("dprc.%u object destroyed\n", child_dprc_id);

	/*
	 * Tell the fsl_mc_resman kernel driver that the
	 * MC portal that was allocated for the destroyed child
	 * container can now be freed.
	 */
	error = ioctl(resman.mc_io.fd, RESMAN_FREE_MC_PORTAL,
		       dprc_attr.portal_id);
	if (error == -1) {
		error = -errno;
		ERROR_PRINTF(
			"ioctl(RESMAN_FREE_MC_PORTAL) failed with error %d\n",
			error);
		goto error;
	}

	DEBUG_PRINTF("Freed MC portal id %u\n", dprc_attr.portal_id);
	return 0;
error:
	if (dprc_opened) {
		int error2;

		error2 = dprc_close(&resman.mc_io, child_dprc_handle);
		if (error2 < 0) {
			ERROR_PRINTF(
				"dprc_close() failed with error %d\n", error2);
		}
	}

	return error;
}

static int cmd_destroy_object(void)
{
	int error;
	char *obj_name;
	char obj_type[OBJ_TYPE_MAX_LENGTH + 1];
	uint32_t obj_id;

	if (resman.num_non_option_arg == 0) {
		ERROR_PRINTF("<object> argument missing\n");
		error = -EINVAL;
		goto out;
	}

	if (resman.num_non_option_arg > 1) {
		ERROR_PRINTF("Invalid number of non-option arguments: %d\n",
			     resman.num_non_option_arg);
		error = -EINVAL;
		goto out;
	}

	if (resman.option_mask != 0) {
		print_unexpected_options_error(resman.option_mask);
		error = -EINVAL;
		goto out;
	}

	obj_name = resman.non_option_arg[0];
	error = parse_object_name(obj_name, NULL, obj_type, &obj_id);
	if (error < 0)
		goto out;

	if (strcmp(obj_type, "dprc") == 0) {
		error = destroy_dprc(resman.root_dprc_handle, obj_id);
	} else {
		ERROR_PRINTF("Unexpected object type \'%s\'\n", obj_type);
		error = -EINVAL;
	}
out:
	return error;
}

static int cmd_move_object(void)
{
	uint32_t obj_id;
	uint32_t src_dprc_id;
	uint32_t dest_dprc_id;
	char *obj_name;
	struct dprc_res_req res_req;
	char obj_type[OBJ_TYPE_MAX_LENGTH + 1];
	int error;

	if (resman.num_non_option_arg == 0) {
		ERROR_PRINTF("<object> argument missing\n");
		error = -EINVAL;
		goto error;
	}

	if (resman.num_non_option_arg > 1) {
		ERROR_PRINTF("Invalid number of non-option arguments: %d\n",
			     resman.num_non_option_arg);
		error = -EINVAL;
		goto error;
	}

	obj_name = resman.non_option_arg[0];
	error = parse_object_name(obj_name, NULL, obj_type, &obj_id);
	if (error < 0)
		goto error;

	if (strcmp(obj_type, "dprc") == 0) {
		ERROR_PRINTF("Objects of type \'dprc\' cannot be moved\n");
		error = -EINVAL;
		goto error;
	}

	if (resman.option_mask !=
	    (OPT_SOURCE_CONTAINER_MASK | OPT_DEST_CONTAINER)) {
		print_unexpected_options_error(
			resman.option_mask);
		error = -EINVAL;
		goto error;
	}

	resman.option_mask &=
		~(OPT_SOURCE_CONTAINER_MASK | OPT_DEST_CONTAINER);

	assert(resman.option_arg[OPT_SOURCE_CONTAINER] != NULL);
	assert(resman.option_arg[OPT_DEST_CONTAINER] != NULL);

	error = parse_object_name(
			resman.option_arg[OPT_SOURCE_CONTAINER],
			"dprc", obj_type, &src_dprc_id);
	if (error < 0)
		goto error;

	error = parse_object_name(
			resman.option_arg[OPT_DEST_CONTAINER],
			"dprc", obj_type, &dest_dprc_id);
	if (error < 0)
		goto error;

	if (dest_dprc_id == src_dprc_id) {
		ERROR_PRINTF("Source and destination containers must be different\n");
		error = -EINVAL;
		goto error;
	}

	strcpy(res_req.type, obj_type);
	res_req.num = 1;
	res_req.options = DPRC_RES_REQ_OPT_EXPLICIT;
	res_req.id_base_align = obj_id;

	if (src_dprc_id == resman.root_dprc_id) {
		/*
		 * Move object from root container to child container:
		 */
		error = dprc_assign(&resman.mc_io,
 				    resman.root_dprc_handle,
 				    dest_dprc_id,
				    &res_req);
		if (error < 0) {
			ERROR_PRINTF(
				"dprc_assign() failed: %d\n", error);
			goto error;
		}
	} else if (dest_dprc_id == resman.root_dprc_id) {
		/*
		 * Move object from child container to root container:
		 */
		error = dprc_unassign(&resman.mc_io,
 				      resman.root_dprc_handle,
				      src_dprc_id,
				      &res_req);
		if (error < 0) {
			ERROR_PRINTF(
				"dprc_unassign() failed: %d\n", error);
			goto error;
		}
	} else {
		/*
		 * TODO: The limitation below should be relaxed to require only that
		 * there must be a parent-child relationship between the source and
		 * destination containers.
		 */
		ERROR_PRINTF("Either the source or the destination container must be root container\n");
		error = -EINVAL;
		goto error;
	}

	printf("%s.%u moved from dprc.%u to dprc.%u\n",
	       obj_type, obj_id, src_dprc_id, dest_dprc_id);

	return 0;
error:
	return error;
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
		c = getopt_long(argc, argv, "hvrxt:c:s:d:", getopt_long_options, NULL);
		if (c == -1)
			break;

		switch (c) {
		case 'h':
			resman.option_mask |= OPT_HELP_MASK;
			break;

		case 'v':
			resman.option_mask |= OPT_VERSION_MASK;
			break;

		case 'r':
			resman.option_mask |= OPT_RESOURCES_MASK;
			break;

		case 'x':
			resman.option_mask |= OPT_VERBOSE_MASK;
			break;

		case 't':
			resman.option_mask |= OPT_TYPE_MASK;
			resman.option_arg[OPT_TYPE] = optarg;
			break;

		case 'c':
			resman.option_mask |= OPT_CONTAINER_MASK;
			resman.option_arg[OPT_CONTAINER] = optarg;
			break;

		case 's':
			resman.option_mask |= OPT_SOURCE_CONTAINER_MASK;
			resman.option_arg[OPT_SOURCE_CONTAINER] = optarg;
			break;

		case 'd':
			resman.option_mask |= OPT_DEST_CONTAINER_MASK;
			resman.option_arg[OPT_DEST_CONTAINER] = optarg;
			break;

		case '?':
			error = -EINVAL;
			goto out;
	       default:
			assert(false);
		}
	}

	DEBUG_PRINTF("value of optind is: %d\n", optind);
	DEBUG_PRINTF("number of variables are: %d\n", argc);

	if (optind == argc && resman.option_mask & OPT_HELP_MASK) {
		print_usage();
		goto out;
	}

	if (optind == argc && resman.option_mask & OPT_VERSION_MASK) {
		print_version();
		goto out;
	}

	if (optind == argc) {
		ERROR_PRINTF("warning: resman command missing.\n");
		print_usage();
		error = -EINVAL;
		goto out;
	}

	assert(optind < argc);
	resman.cmd_name = argv[optind];
	resman.num_non_option_arg = argc - (optind + 1);
	if (resman.num_non_option_arg != 0)
		resman.non_option_arg = &argv[optind + 1];

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
		ERROR_PRINTF("error: invalid command \'%s\'\n.",
			     resman.cmd_name);
		print_usage();
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
	bool mc_io_initialized = false;
	bool root_dprc_opened = false;
	struct ioctl_dprc_info root_dprc_info = { 0 };

	DEBUG_PRINTF("resman built on " __DATE__ " " __TIME__ "\n");
	error = mc_io_init(&resman.mc_io);
	if (error != 0)
		goto out;

	mc_io_initialized = true;
	DEBUG_PRINTF("resman.mc_io.fd: %d\n", resman.mc_io.fd);

	error = mc_get_version(&resman.mc_io, &resman.mc_fw_version);
	if (error != 0) {
		ERROR_PRINTF("mc_get_version() failed with error %d\n",
			     error);
		goto out;
	}

	DEBUG_PRINTF("MC firmware version: %u.%u.%u\n",
		     resman.mc_fw_version.major,
		     resman.mc_fw_version.minor,
		     resman.mc_fw_version.revision);

	DEBUG_PRINTF("calling ioctl(RESMAN_GET_ROOT_DPRC_INFO)\n");
	error = ioctl(resman.mc_io.fd, RESMAN_GET_ROOT_DPRC_INFO,
		      &root_dprc_info);
	if (error == -1) {
		error = -errno;
		goto out;
	}

	DEBUG_PRINTF("ioctl returned dprc_id: %#x, dprc_handle: %#x\n",
		     root_dprc_info.dprc_id,
		     root_dprc_info.dprc_handle);

	resman.root_dprc_id = root_dprc_info.dprc_id;
	error = open_dprc(resman.root_dprc_id, &resman.root_dprc_handle);
	if (error < 0)
		goto out;

	root_dprc_opened = true;
	error = parse_cmd_line(argc, argv);
out:
	if (root_dprc_opened) {
		int error2;

		error2 = dprc_close(&resman.mc_io, resman.root_dprc_handle);
		if (error2 < 0) {
			ERROR_PRINTF("dprc_close() failed with error %d\n",
				     error2);
			if (error == 0)
				error = error2;
		}
	}

	if (mc_io_initialized)
		mc_io_cleanup(&resman.mc_io);

	return error;
}


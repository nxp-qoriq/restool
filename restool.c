/*
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 * Author: German Rivera <German.Rivera@freescale.com>
 *	   Lijun Pan <Lijun.Pan@freescale.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation  and/or other materials provided with the distribution.
 * 3. Neither the names of the copyright holders nor the names of any
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include "restool.h"
#include "utils.h"
#include "fsl_mc_cmd.h"
#include "fsl_dprc.h"

/*
 * TODO: Obtain the following constants from the fsl-mc bus driver via an ioctl
 */
#define MC_PORTALS_BASE_PADDR	((phys_addr_t)0x00080C000000ULL)
#define MC_PORTAL_STRIDE	0x10000
#define MC_PORTAL_SIZE		64
#define MAX_MC_PORTALS		512

#define MC_PORTAL_PADDR_TO_PORTAL_ID(_portal_paddr) \
	(((_portal_paddr) - MC_PORTALS_BASE_PADDR) / MC_PORTAL_STRIDE)

static const char restool_version[] = "0.6";

static struct option global_options[] = {
	[GLOBAL_OPT_HELP] = {
		.name = "help",
		.val = 'h',
	},

	[GLOBAL_OPT_VERSION] = {
		.name = "version",
		.val = 'v',
	},

	[GLOBAL_OPT_DEBUG] = {
		.name = "debug",
		.val = 'd',
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(global_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static const struct object_cmd_parser object_cmd_parsers[] = {
	{ .obj_type = "dprc", .obj_commands = dprc_commands },
	{ .obj_type = "dpni", .obj_commands = dpni_commands },
	{ .obj_type = "dpio", .obj_commands = dpio_commands },
	{ .obj_type = "dpbp", .obj_commands = dpbp_commands },
	{ .obj_type = "dpsw", .obj_commands = dpsw_commands },
	{ .obj_type = "dpci", .obj_commands = dpci_commands },
	{ .obj_type = "dpcon", .obj_commands = dpcon_commands },
	{ .obj_type = "dpseci", .obj_commands = dpseci_commands },
	{ .obj_type = "dpdmux", .obj_commands = dpdmux_commands },
};

struct restool restool;

enum mc_cmd_status flib_error_to_mc_status(int error)
{
	switch (error) {
	case 0:
		return MC_CMD_STATUS_OK;
	case -EACCES:
		return MC_CMD_STATUS_AUTH_ERR;
	case -EPERM:
		return MC_CMD_STATUS_NO_PRIVILEGE;
	case -EIO:
		return MC_CMD_STATUS_DMA_ERR;
	case -EINVAL:
		return MC_CMD_STATUS_CONFIG_ERR;
	case -ETIMEDOUT:
		return MC_CMD_STATUS_TIMEOUT;
	case -ENAVAIL:
		return MC_CMD_STATUS_NO_RESOURCE;
	case -ENOMEM:
		return MC_CMD_STATUS_NO_MEMORY;
	case -EBUSY:
		return MC_CMD_STATUS_BUSY;
	case -ENOTSUP:
		return MC_CMD_STATUS_UNSUPPORTED_OP;
	case -ENODEV:
		return MC_CMD_STATUS_INVALID_STATE;
	default:
		assert(false);
		break;
	}

	/* Not expected to reach here */
	return 1000;
}

const char *mc_status_to_string(enum mc_cmd_status status)
{
	static const char *const status_strings[] = {
		[MC_CMD_STATUS_OK] = "Command completed successfully",
		[MC_CMD_STATUS_READY] = "Command ready to be processed",
		[MC_CMD_STATUS_AUTH_ERR] = "Authentication error",
		[MC_CMD_STATUS_NO_PRIVILEGE] = "No privilege",
		[MC_CMD_STATUS_DMA_ERR] = "DMA or I/O error",
		[MC_CMD_STATUS_CONFIG_ERR] = "Configuration error",
		[MC_CMD_STATUS_TIMEOUT] = "Operation timed out",
		[MC_CMD_STATUS_NO_RESOURCE] = "No resources",
		[MC_CMD_STATUS_NO_MEMORY] = "No memory available",
		[MC_CMD_STATUS_BUSY] = "Device is busy",
		[MC_CMD_STATUS_UNSUPPORTED_OP] = "Unsupported operation",
		[MC_CMD_STATUS_INVALID_STATE] = "Invalid state"
	};

	if ((unsigned int)status >= ARRAY_SIZE(status_strings))
		return "Unknown MC error";

	return status_strings[status];
}

int find_target_obj_desc(uint16_t dprc_handle, int nesting_level,
			uint32_t target_id, char *target_type,
			struct dprc_obj_desc *target_obj_desc,
			uint16_t *target_parent_dprc_handle, bool *found)
{
	int num_child_devices;
	int error = 0;
	enum mc_cmd_status mc_status;

	assert(nesting_level <= MAX_DPRC_NESTING);

	if (strcmp(target_type, "dprc") == 0 &&
	    target_id == restool.root_dprc_id) {
		DEBUG_PRINTF("This is root dprc.\n");
		strcpy(target_obj_desc->type, "dprc");
		target_obj_desc->id = 1;
		return 0;
	}

	error = dprc_get_obj_count(&restool.mc_io,
				   dprc_handle,
				   &num_child_devices);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	for (int i = 0; i < num_child_devices; i++) {
		struct dprc_obj_desc obj_desc;
		uint16_t child_dprc_handle;
		int error2;

		error = dprc_get_obj(
				&restool.mc_io,
				dprc_handle,
				i,
				&obj_desc);
		if (error < 0) {
			DEBUG_PRINTF(
				"dprc_get_object(%u) failed with error %d\n",
				i, error);
			goto out;
		}

		DEBUG_PRINTF("it is %s.%u\n", obj_desc.type, obj_desc.id);

		if (strcmp(obj_desc.type, target_type) == 0 &&
		    target_id == (uint32_t)obj_desc.id) {
			*target_obj_desc = obj_desc;
			*target_parent_dprc_handle = dprc_handle;
			DEBUG_PRINTF("object found\n");
			*found = true;
			goto out;
		} else if (strcmp(obj_desc.type, "dprc") == 0) {
			bool found2 = false;

			error = open_dprc(obj_desc.id, &child_dprc_handle);
			if (error < 0)
				goto out;

			DEBUG_PRINTF("entering %s.%u\n", obj_desc.type,
					obj_desc.id);
			error = find_target_obj_desc(child_dprc_handle,
					nesting_level + 1,
					target_id,
					target_type,
					target_obj_desc,
					target_parent_dprc_handle,
					&found2);

			error2 = dprc_close(&restool.mc_io, child_dprc_handle);
			if (error2 < 0) {
				mc_status = flib_error_to_mc_status(error2);
				ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status), mc_status);
				if (error == 0)
					error = error2;

				goto out;
			}

			DEBUG_PRINTF("exiting %s.%u\n", obj_desc.type,
					obj_desc.id);
			if (found2)
				goto out;
		} else {
			continue;
		}
	}

out:
	return error;
}

int print_obj_verbose(struct dprc_obj_desc *target_obj_desc,
			const struct flib_ops *ops)
{
	uint16_t obj_handle;
	uint32_t irq_mask;
	uint32_t irq_status;
	enum mc_cmd_status mc_status;
	int error = 0;

	if (strcmp(target_obj_desc->type, "dprc") == 0 &&
	    target_obj_desc->id == restool.root_dprc_id) {
		printf("root dprc doesn't have verbose info to display\n");
		return 0;
	}

	printf("number of mappable regions: %u\n",
		target_obj_desc->region_count);
	printf("number of interrupts: %u\n", target_obj_desc->irq_count);

	error = ops->obj_open(&restool.mc_io, target_obj_desc->id, &obj_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			mc_status_to_string(mc_status), mc_status);
		return error;
	}

	for (int j = 0; j < target_obj_desc->irq_count; j++) {
		ops->obj_get_irq_mask(&restool.mc_io, obj_handle, j, &irq_mask);
		printf("interrupt %d's mask: %#x\n", j, irq_mask);
		ops->obj_get_irq_status(&restool.mc_io, obj_handle, j,
					&irq_status);
		printf("interrupt %d's status: %#x\n", j, irq_status);
	}

	error = ops->obj_close(&restool.mc_io, obj_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			mc_status_to_string(mc_status), mc_status);
	}

	return error;
}

void print_unexpected_options_error(uint32_t option_mask,
				    const struct option *options)
{
	ERROR_PRINTF("Invalid options:\n");
	for (unsigned int i = 0; i < MAX_NUM_CMD_LINE_OPTIONS; i++) {
		if (option_mask & ONE_BIT_MASK(i))
			fprintf(stderr, "\t--%s\n", options[i].name);

	}
}

static void print_usage(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool [<global-options>] <object-type> <command> <object-name> [ARGS...]\n"
		"\n"
		"Valid <global-options> are:\n"
		"   -v,--version   Displays tool version info\n"
		"   -h,-?,--help   Displays general help info\n"
		"   -d, --debug	   Print out DEBUG info\n"
		"	--debug option must be used together with an object\n"
		"	e.g. restool --debug dpni info dpni.11\n"
		"\n"
		"Valid <object-type> values: <dprc|dpni|dpio|dpsw|dpbp|dpci|dpcon|dpseci|dpdmux>\n"
		"\n"
		"Valid commands vary for each object type.  Use the \'help\'\n"
		"command to see detailed usage info for an object.  The following\n"
		"commands are valid for all object types:\n"
		"   help\n"
		"   info\n"
		"   create\n"
		"   destroy\n"
		"\n"
		"The <object-name> arg is a string containing object type\n"
		"and ID (e.g. dpni.7).\n"
		"\n"
		"For valid [ARGS] values, see the \'help\' command for the object type.\n"
		"\n";

	printf(usage_msg);
	restool.global_option_mask &= ~ONE_BIT_MASK(GLOBAL_OPT_HELP);
}

static void print_version(void)
{
	printf("Freescale MC restool tool version %s\n", restool_version);
	printf("MC firmware version: %u.%u.%u\n",
	       restool.mc_fw_version.major,
	       restool.mc_fw_version.minor,
	       restool.mc_fw_version.revision);

	restool.global_option_mask &= ~ONE_BIT_MASK(GLOBAL_OPT_VERSION);
}

int parse_object_name(const char *obj_name, char *expected_obj_type,
		      uint32_t *obj_id)
{
	int n;
	char obj_type[OBJ_TYPE_MAX_LENGTH + 1];

	assert(expected_obj_type != NULL);
	n = sscanf(obj_name, "%" STRINGIFY(OBJ_TYPE_MAX_LENGTH) "[a-z].%u",
		   obj_type, obj_id);
	if (n != 2) {
		ERROR_PRINTF("Invalid MC object name: %s\n", obj_name);
		return -EINVAL;
	}

	if (strcmp(obj_type, expected_obj_type) != 0) {
		ERROR_PRINTF("Expected \'%s\' object type\n",
			     expected_obj_type);
		return -EINVAL;
	}

	return 0;
}

int open_dprc(uint32_t dprc_id, uint16_t *dprc_handle)
{
	int error;
	enum mc_cmd_status mc_status;

	error = dprc_open(&restool.mc_io,
			  dprc_id,
			  dprc_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	if (*dprc_handle == 0) {
		DEBUG_PRINTF(
			"dprc_open() returned invalid handle (auth 0) for dprc.%u\n",
			dprc_id);

		(void)dprc_close(&restool.mc_io, *dprc_handle);
		error = -ENOENT;
		goto out;
	}

	error = 0;
out:
	return error;
}

static int parse_global_options(int argc, char *argv[],
				int *next_argv_index)
{
	int c;
	int opt_index;
	int error;

	/*
	 * Initialize getopt global variables:
	 */
	optind = 1;
	optarg = NULL;

	restool.global_option_mask = 0;
	for ( ; ; ) {
		opt_index = 0;
		c = getopt_long(argc, argv, "+h?vd", global_options, NULL);
		if (c == -1)
			break;

		switch (c) {
		case 'h':
		case '?':
			opt_index = GLOBAL_OPT_HELP;
			break;

		case 'v':
			opt_index = GLOBAL_OPT_VERSION;
			break;

		case 'd':
			opt_index = GLOBAL_OPT_DEBUG;
			break;

		default:
			assert(false);
		}

		assert((unsigned int)opt_index < MAX_NUM_CMD_LINE_OPTIONS);
		if (restool.global_option_mask & ONE_BIT_MASK(opt_index)) {
			ERROR_PRINTF("Duplicated option: %s\n",
				     global_options[opt_index].name);
			error = -EINVAL;
			goto error;
		}

		restool.global_option_mask |= ONE_BIT_MASK(opt_index);
		if (global_options[opt_index].has_arg)
			restool.global_option_args[opt_index] = optarg;
		else
			restool.global_option_args[opt_index] = NULL;
	}

	DEBUG_PRINTF("optind: %d, argc: %d\n", optind, argc);
	*next_argv_index = optind;
	return 0;
error:
	return error;
}

static int parse_cmd_options(int argc, char *argv[],
			     const struct option options[],
			     int *next_argv_index)
{
	int c;
	int opt_index;
	int error;

	/*
	 * Initialize getopt global variables:
	 */
	optind = 1;
	optarg = NULL;

	restool.cmd_option_mask = 0;
	assert(options != NULL);

	for ( ; ; ) {
		c = getopt_long_only(argc, argv, "+", options, &opt_index);
		if (c == -1)
			break;

		if (c != 0) {
			error = -EINVAL;
			goto error;
		}

		assert((unsigned int)opt_index < MAX_NUM_CMD_LINE_OPTIONS);
		if (restool.cmd_option_mask & ONE_BIT_MASK(opt_index)) {
			ERROR_PRINTF("Duplicated option: %s\n",
				     options[opt_index].name);
			error = -EINVAL;
			goto error;
		}

		restool.cmd_option_mask |= ONE_BIT_MASK(opt_index);
		if (options[opt_index].has_arg)
			restool.cmd_option_args[opt_index] = optarg;
		else
			restool.cmd_option_args[opt_index] = NULL;
	}

	DEBUG_PRINTF("optind: %d, argc: %d\n", optind, argc);
	*next_argv_index = optind;
	return 0;
error:
	return error;
}

static int parse_obj_command(const char *obj_type,
			     const char *cmd_name,
			     int argc,
			     char *argv[])
{
	int error;
	int next_argv_index;
	unsigned int i;
	const struct object_cmd_parser *obj_cmd_parser = NULL;
	struct object_command *obj_commands;
	struct object_command *obj_cmd = NULL;

	assert(argv[0] == cmd_name);

	/*
	 * Lookup object command parser:
	 */
	for (i = 0; i < ARRAY_SIZE(object_cmd_parsers); i++) {
		if (strcmp(obj_type, object_cmd_parsers[i].obj_type) == 0) {
			obj_cmd_parser = &object_cmd_parsers[i];
			break;
		}
	}

	if (obj_cmd_parser == NULL) {
		ERROR_PRINTF("error: invalid object type \'%s\'\n", obj_type);
		print_usage();
		error = -EINVAL;
		goto out;
	}

	/*
	 * Lookup object-level command:
	 */
	obj_commands = obj_cmd_parser->obj_commands;
	for (i = 0; obj_commands[i].cmd_name != NULL; i++) {
		if (strcmp(cmd_name, obj_commands[i].cmd_name) == 0) {
			obj_cmd = &obj_commands[i];
			break;
		}
	}

	if (obj_cmd == NULL) {
		ERROR_PRINTF("Invalid command \'%s\' for object type \'%s\'\n",
			     cmd_name, obj_type);
		print_usage();
		error = -EINVAL;
		goto out;
	}

	restool.obj_cmd = obj_cmd;

	if (argc >= 2 && argv[1][0] != '-') {
		restool.obj_name = argv[1];
		argv++;
		argc--;
	} else {
		restool.obj_name = NULL;
	}

	/*
	 * Parse object-level command options:
	 */
	if (obj_cmd->options != NULL) {
		error = parse_cmd_options(argc, argv,
					  obj_cmd->options,
					  &next_argv_index);

		if (error != 0)
			goto out;

		if (next_argv_index != argc) {
			assert(next_argv_index < argc);
			ERROR_PRINTF("Invalid command line\n");
			print_usage();
			error = -EINVAL;
			goto out;
		}
	} else {
		if (argc != 1) {
			ERROR_PRINTF("Invalid command line\n");
			print_usage();
			error = -EINVAL;
			goto out;
		}
	}

	/*
	 * Execute object-level command:
	 */
	error = obj_cmd->cmd_func();
	if (error < 0)
		goto out;

	if (restool.cmd_option_mask != 0) {
		print_unexpected_options_error(restool.cmd_option_mask,
					       obj_cmd->options);
		error = -EINVAL;
	}
out:
	return error;
}


int main(int argc, char *argv[])
{
	int error;
	int next_argv_index;
	const char *obj_type;
	const char *cmd_name;
	bool mc_io_initialized = false;
	bool root_dprc_opened = false;
	struct ioctl_dprc_info root_dprc_info = { 0 };
	enum mc_cmd_status mc_status;

	#ifdef DEBUG
	restool.debug = true;
	#endif

	DEBUG_PRINTF("restool built on " __DATE__ " " __TIME__ "\n");
	error = mc_io_init(&restool.mc_io);
	if (error != 0)
		goto out;

	mc_io_initialized = true;
	DEBUG_PRINTF("restool.mc_io.fd: %d\n", restool.mc_io.fd);

	error = mc_get_version(&restool.mc_io, &restool.mc_fw_version);
	if (error != 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	if (MC_VER_MAJOR != restool.mc_fw_version.major)
		printf(
			"ERROR: MC firmware major version mismatch (found: %d, expected: %d)\n",
			restool.mc_fw_version.major, MC_VER_MAJOR);

	if (MC_VER_MINOR != restool.mc_fw_version.minor)
		printf(
			"WARNING: MC Firmware minor version mismatch (found: %d, expected: %d)\n",
			restool.mc_fw_version.minor, MC_VER_MINOR);

	DEBUG_PRINTF("MC firmware version: %u.%u.%u\n",
		     restool.mc_fw_version.major,
		     restool.mc_fw_version.minor,
		     restool.mc_fw_version.revision);

	DEBUG_PRINTF("calling ioctl(RESTOOL_GET_ROOT_DPRC_INFO)\n");
	error = ioctl(restool.mc_io.fd, RESTOOL_GET_ROOT_DPRC_INFO,
		      &root_dprc_info);
	if (error == -1) {
		error = -errno;
		goto out;
	}

	DEBUG_PRINTF("ioctl returned dprc_id: %#x, dprc_handle: %#x\n",
		     root_dprc_info.dprc_id,
		     root_dprc_info.dprc_handle);

	restool.root_dprc_id = root_dprc_info.dprc_id;
	error = open_dprc(restool.root_dprc_id, &restool.root_dprc_handle);
	if (error < 0)
		goto out;

	root_dprc_opened = true;
	error = parse_global_options(argc, argv, &next_argv_index);
	if (error < 0)
		goto out;

	if (next_argv_index == argc) {
		if (restool.global_option_mask == 0) {
			ERROR_PRINTF("Incomplete command line\n");
			print_usage();
			error = -EINVAL;
			goto out;
		}

		if (restool.global_option_mask & ONE_BIT_MASK(GLOBAL_OPT_HELP))
			print_usage();

		if (restool.global_option_mask &
		    ONE_BIT_MASK(GLOBAL_OPT_VERSION))
			print_version();

		if (restool.global_option_mask &
		    ONE_BIT_MASK(GLOBAL_OPT_DEBUG)) {
			restool.global_option_mask &=
				~ONE_BIT_MASK(GLOBAL_OPT_DEBUG);
			print_usage();
			error = -EINVAL;
			goto out;
		}

		if (restool.global_option_mask != 0) {
			print_unexpected_options_error(
				restool.global_option_mask,
				global_options);
			error = -EINVAL;
		}
	} else {
		if (restool.global_option_mask &
		    ONE_BIT_MASK(GLOBAL_OPT_DEBUG)) {
			restool.global_option_mask &=
				~ONE_BIT_MASK(GLOBAL_OPT_DEBUG);
			restool.debug = true;
		}

		int num_remaining_args;

		assert(next_argv_index < argc);
		if (restool.global_option_mask != 0) {
			print_unexpected_options_error(
				restool.global_option_mask,
				global_options);
			print_usage();
			error = -EINVAL;
			goto out;
		}

		num_remaining_args = argc - next_argv_index;
		if (num_remaining_args < 2) {
			ERROR_PRINTF("Incomplete command line\n");
			print_usage();
			error = -EINVAL;
			goto out;
		}

		obj_type = argv[next_argv_index];
		cmd_name = argv[next_argv_index + 1];
		error = parse_obj_command(obj_type,
					  cmd_name,
					  num_remaining_args - 1,
					  &argv[next_argv_index + 1]);
	}
out:
	if (root_dprc_opened) {
		int error2;

		error2 = dprc_close(&restool.mc_io, restool.root_dprc_handle);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status), mc_status);
			if (error == 0)
				error = error2;
		}
	}

	if (mc_io_initialized)
		mc_io_cleanup(&restool.mc_io);

	return error;
}


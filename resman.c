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
#include <stdbool.h>
#include <errno.h>
#include <assert.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include "resman.h"
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

static const char resman_version[] = "0.3";

static struct option global_options[] = {
	[GLOBAL_OPT_HELP] = {
		.name = "help",
		.val = 'h',
	},

	[GLOBAL_OPT_VERSION] = {
		.name = "version",
		.val = 'v',
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
};

struct resman resman;

void print_unexpected_options_error(uint32_t option_mask,
				    const struct option *options)
{
	ERROR_PRINTF("Invalid options:\n");
	for (unsigned int i; i < MAX_NUM_CMD_LINE_OPTIONS; i++) {
		if (option_mask & ONE_BIT_MASK(i))
			fprintf(stderr, "\t--%s\n", options[i].name);

	}
}

static void print_usage(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: resman [<global-options>] <object-type> <command> <object-name> [ARGS...]\n"
		"\n"
		"Valid <global-options> are:\n"
		"   -v,--version   Displays tool version info\n"
		"   -h,-?,--help   Displays general help info\n"
		"\n"
		"Valid <object-type> values: <dprc|dpni|dpio|dpsw|dpbp|dpcon|dpci|dpseci|dpmux>\n"
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
	resman.global_option_mask &= ~ONE_BIT_MASK(GLOBAL_OPT_HELP);
}

static void print_version(void)
{
	printf("Freescale MC resman tool version %s\n", resman_version);
	printf("MC firmware version: %u.%u.%u\n",
	       resman.mc_fw_version.major,
	       resman.mc_fw_version.minor,
	       resman.mc_fw_version.revision);

	resman.global_option_mask &= ~ONE_BIT_MASK(GLOBAL_OPT_VERSION);
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
		ERROR_PRINTF(
			"dprc_open() returned invalid handle (auth 0) for dprc.%u\n",
			dprc_id);

		(void)dprc_close(&resman.mc_io, *dprc_handle);
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

	resman.global_option_mask = 0;
	for ( ; ; ) {
		opt_index = 0;
		c = getopt_long(argc, argv, "+h?v", global_options, NULL);
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

		default:
			assert(false);
		}

		assert((unsigned int)opt_index < MAX_NUM_CMD_LINE_OPTIONS);
		if (resman.global_option_mask & ONE_BIT_MASK(opt_index)) {
			ERROR_PRINTF("Duplicated option: %s\n",
				     global_options[opt_index].name);
			error = -EINVAL;
			goto error;
		}

		resman.global_option_mask |= ONE_BIT_MASK(opt_index);
		if (global_options[opt_index].has_arg)
			resman.global_option_args[opt_index] = optarg;
		else
			resman.global_option_args[opt_index] = NULL;
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

	resman.cmd_option_mask = 0;
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
		if (resman.cmd_option_mask & ONE_BIT_MASK(opt_index)) {
			ERROR_PRINTF("Duplicated option: %s\n",
				     options[opt_index].name);
			error = -EINVAL;
			goto error;
		}

		resman.cmd_option_mask |= ONE_BIT_MASK(opt_index);
		if (options[opt_index].has_arg)
			resman.cmd_option_args[opt_index] = optarg;
		else
			resman.cmd_option_args[opt_index] = NULL;
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
		ERROR_PRINTF("error: invalid object type \'%s\'\n", argv[0]);
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

	resman.obj_cmd = obj_cmd;

	if (argc >= 2 && argv[1][0] != '-') {
		resman.obj_name = argv[1];
		argv++;
		argc--;
	} else {
		resman.obj_name = NULL;
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

	if (resman.cmd_option_mask != 0) {
		print_unexpected_options_error(resman.cmd_option_mask,
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

	if (MC_VER_MAJOR != resman.mc_fw_version.major)
		printf(
			"ERROR: MC firmware major version mismatch (found: %d, expected: %d)\n",
			resman.mc_fw_version.major, MC_VER_MAJOR);

	if (MC_VER_MINOR != resman.mc_fw_version.minor)
		printf(
			"WARNING: MC Firmware minor version mismatch (found: %d, expected: %d)\n",
			resman.mc_fw_version.minor, MC_VER_MINOR);

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
	error = parse_global_options(argc, argv, &next_argv_index);

	if (next_argv_index == argc) {
		if (resman.global_option_mask == 0) {
			ERROR_PRINTF("Incomplete command line\n");
			print_usage();
			error = -EINVAL;
			goto out;
		}

		if (resman.global_option_mask & ONE_BIT_MASK(GLOBAL_OPT_HELP))
			print_usage();

		if (resman.global_option_mask &
		    ONE_BIT_MASK(GLOBAL_OPT_VERSION))
			print_version();

		if (resman.global_option_mask != 0) {
			print_unexpected_options_error(
				resman.global_option_mask,
				global_options);
			error = -EINVAL;
		}
	} else {
		int num_remaining_args;

		assert(next_argv_index < argc);
		if (resman.global_option_mask != 0) {
			print_unexpected_options_error(
				resman.global_option_mask,
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


/*
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 * Author: Lijun Pan <Lijun.Pan@freescale.com>
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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <getopt.h>
#include "restool.h"
#include "utils.h"
#include "mc_v8/fsl_dpdmai.h"

enum mc_cmd_status mc_status;

/**
 * dpdmai info command options
 */
enum dpdmai_info_options {
	INFO_OPT_HELP = 0,
	INFO_OPT_VERBOSE,
};

static struct option dpdmai_info_options[] = {
	[INFO_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	[INFO_OPT_VERBOSE] = {
		.name = "verbose",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpdmai_info_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpdmai create command options
 */
enum dpdmai_create_options {
	CREATE_OPT_HELP = 0,
	CREATE_OPT_PRIORITIES,
};

static struct option dpdmai_create_options[] = {
	[CREATE_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_PRIORITIES] = {
		.name = "priorities",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpdmai_create_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpdmai destroy command options
 */
enum dpdmai_destroy_options {
	DESTROY_OPT_HELP = 0,
};

static struct option dpdmai_destroy_options[] = {
	[DESTROY_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpdmai_destroy_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static const struct flib_ops dpdmai_ops = {
	.obj_open = dpdmai_open,
	.obj_close = dpdmai_close,
	.obj_get_irq_mask = dpdmai_get_irq_mask,
	.obj_get_irq_status = dpdmai_get_irq_status,
};

static int cmd_dpdmai_help(void)
{
	static const char help_msg[] =
		"\n"
		"Usage: restool dpdmai <command> [--help] [ARGS...]\n"
		"Where <command> can be:\n"
		"   info - displays detailed information about a DPDMAI object.\n"
		"   create - creates a new child DPDMAI under the root DPRC.\n"
		"   destroy - destroys a child DPDMAI under the root DPRC.\n"
		"\n"
		"For command-specific help, use the --help option of each command.\n"
		"\n";

	printf(help_msg);
	return 0;
}

static int print_dpdmai_attr(uint32_t dpdmai_id,
			struct dprc_obj_desc *target_obj_desc)
{
	uint16_t dpdmai_handle;
	int error;
	struct dpdmai_attr dpdmai_attr;
	bool dpdmai_opened = false;

	error = dpdmai_open(&restool.mc_io, 0, dpdmai_id, &dpdmai_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpdmai_opened = true;
	if (0 == dpdmai_handle) {
		DEBUG_PRINTF(
			"dpdmai_open() returned invalid handle (auth 0) for dpdmai.%u\n",
			dpdmai_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpdmai_attr, 0, sizeof(dpdmai_attr));
	error = dpdmai_get_attributes(&restool.mc_io, 0, dpdmai_handle,
					&dpdmai_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpdmai_id == (uint32_t)dpdmai_attr.id);

	printf("dpdmai version: %u.%u\n", dpdmai_attr.version.major,
	       dpdmai_attr.version.minor);
	printf("dpdmai id: %d\n", dpdmai_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	printf("number of priorities: %u\n", dpdmai_attr.num_of_priorities);
	print_obj_label(target_obj_desc);

	error = 0;

out:
	if (dpdmai_opened) {
		int error2;

		error2 = dpdmai_close(&restool.mc_io, 0, dpdmai_handle);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int print_dpdmai_info(uint32_t dpdmai_id)
{
	int error;
	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	bool found = false;

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
				restool.root_dprc_handle, 0, dpdmai_id,
				"dpdmai", &target_obj_desc,
				&target_parent_dprc_id, &found);
	if (error < 0)
		goto out;

	if (strcmp(target_obj_desc.type, "dpdmai")) {
		printf("dpdmai.%d does not exist\n", dpdmai_id);
		return -EINVAL;
	}

	error = print_dpdmai_attr(dpdmai_id, &target_obj_desc);
	if (error < 0)
		goto out;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		error = print_obj_verbose(&target_obj_desc, &dpdmai_ops);
	}

out:
	return error;
}

static int cmd_dpdmai_info(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpdmai info <dpdmai-object> [--verbose]\n"
		"\n"
		"OPTIONS:\n"
		"--verbose\n"
		"   Shows extended/verbose information about the object\n"
		"\n"
		"EXAMPLE:\n"
		"Display information about dpdmai.5:\n"
		"   $ restool dpdmai info dpdmai.5\n"
		"\n";

	uint32_t obj_id;
	int error;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_HELP)) {
		printf(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_HELP);
		error = 0;
		goto out;
	}

	if (restool.obj_name == NULL) {
		ERROR_PRINTF("<object> argument missing\n");
		printf(usage_msg);
		error = -EINVAL;
		goto out;
	}

	error = parse_object_name(restool.obj_name, "dpdmai", &obj_id);
	if (error < 0)
		goto out;

	error = print_dpdmai_info(obj_id);
out:
	return error;
}

static int parse_dpdmai_priorities(char *priorities_str, uint8_t *priorities,
					int num)
{
	char *cursor = NULL;
	char *endptr;
	char *prio_str = strtok_r(priorities_str, ",", &cursor);
	int i = 0;
	long val;

	while (prio_str != NULL) {
		if (i >= num) {
			ERROR_PRINTF("Only supports %d priorities.\n", num);
			return -EINVAL;
		}

		errno = 0;
		val = strtol(prio_str, &endptr, 0);

		if (STRTOL_ERROR(prio_str, endptr, val, errno) ||
		    (val < 1 || val > 8)) {
			ERROR_PRINTF("Invalid priority value.\n");
			return -EINVAL;
		}

		priorities[i] = (uint8_t)val;
		prio_str = strtok_r(NULL, ",", &cursor);
		++i;
	}

	if (num != i) {
		ERROR_PRINTF("Please set %d priorities\n", num);
		return -EINVAL;
	}

	return 0;
}

static int cmd_dpdmai_create(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpdmai create [OPTIONS]\n"
		"\n"
		"OPTIONS:\n"
		"if options are not specified, create DPDMAI by default options\n"
		"default is: restool dpdmai create --priorities=1,2\n"
		"--priorities=<priority1,priority2>\n"
		"   Valid values for <priorityN> are 1-8.\n"
		"\n"
		"EXAMPLES:\n"
		"create a DPDMAI object with all default options:\n"
		"   $ restool dpdmai create\n"
		"create a DPDMAI with 2,4 priorities:\n"
		"   $ restool dpdmai create --priorities=2,4\n"
		"\n";

	int error;
	struct dpdmai_cfg dpdmai_cfg = { { 0 } };
	uint16_t dpdmai_handle;
	struct dpdmai_attr dpdmai_attr;

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_HELP)) {
		printf(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_HELP);
		return 0;
	}

	if (restool.obj_name != NULL) {
		ERROR_PRINTF("Unexpected argument: \'%s\'\n\n",
			     restool.obj_name);
		printf(usage_msg);
		return -EINVAL;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_PRIORITIES)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_PRIORITIES);

		error = parse_dpdmai_priorities(
			restool.cmd_option_args[CREATE_OPT_PRIORITIES],
			dpdmai_cfg.priorities, DPDMAI_PRIO_NUM);
		if (error < 0) {
			DEBUG_PRINTF(
				"parse_dpdmai_priorities() failed with error %d, cannot get priorities.\n",
				error);
			return error;
		}
	} else {
		dpdmai_cfg.priorities[0] = 1;
		dpdmai_cfg.priorities[1] = 2;
	}

	error = dpdmai_create(&restool.mc_io, 0, &dpdmai_cfg, &dpdmai_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	memset(&dpdmai_attr, 0, sizeof(struct dpdmai_attr));
	error = dpdmai_get_attributes(&restool.mc_io, 0, dpdmai_handle,
					&dpdmai_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	print_new_obj("dpdmai", dpdmai_attr.id, NULL);

	error = dpdmai_close(&restool.mc_io, 0, dpdmai_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	return 0;
}

static int cmd_dpdmai_destroy(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpdmai destroy <dpdmai-object>\n"
		"   e.g. restool dpdmai destroy dpdmai.9\n"
		"\n";

	int error;
	int error2;
	uint32_t dpdmai_id;
	uint16_t dpdmai_handle;
	bool dpdmai_opened = false;

	if (restool.cmd_option_mask & ONE_BIT_MASK(DESTROY_OPT_HELP)) {
		printf(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(DESTROY_OPT_HELP);
		return 0;
	}

	if (restool.obj_name == NULL) {
		ERROR_PRINTF("<object> argument missing\n");
		printf(usage_msg);
		error = -EINVAL;
		goto out;
	}

	if (in_use(restool.obj_name, "destroyed")) {
		error = -EBUSY;
		goto out;
	}

	error = parse_object_name(restool.obj_name, "dpdmai", &dpdmai_id);
	if (error < 0)
		goto out;

	if (!find_obj("dpdmai", dpdmai_id)) {
		error = -EINVAL;
		goto out;
	}

	error = dpdmai_open(&restool.mc_io, 0, dpdmai_id, &dpdmai_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpdmai_opened = true;
	if (0 == dpdmai_handle) {
		DEBUG_PRINTF(
			"dpdmai_open() returned invalid handle (auth 0) for dpdmai.%u\n",
			dpdmai_id);
		error = -ENOENT;
		goto out;
	}

	error = dpdmai_destroy(&restool.mc_io, 0, dpdmai_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpdmai_opened = false;
	printf("dpdmai.%u is destroyed\n", dpdmai_id);

out:
	if (dpdmai_opened) {
		error2 = dpdmai_close(&restool.mc_io, 0, dpdmai_handle);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

struct object_command dpdmai_commands[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpdmai_help },

	{ .cmd_name = "info",
	  .options = dpdmai_info_options,
	  .cmd_func = cmd_dpdmai_info },

	{ .cmd_name = "create",
	  .options = dpdmai_create_options,
	  .cmd_func = cmd_dpdmai_create },

	{ .cmd_name = "destroy",
	  .options = dpdmai_destroy_options,
	  .cmd_func = cmd_dpdmai_destroy },

	{ .cmd_name = NULL },
};


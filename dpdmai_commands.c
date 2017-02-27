/* Copyright 2014-2017 Freescale Semiconductor Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * * Neither the name of the above-listed copyright holders nor the
 * names of any contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
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
#include "mc_v9/fsl_dpdmai.h"
#include "mc_v10/fsl_dpdmai.h"

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
	CREATE_OPT_PARENT_DPRC,
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

	[CREATE_OPT_PARENT_DPRC] = {
		.name = "container",
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

static int print_dpdmai_attr_v9(uint32_t dpdmai_id,
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

static int print_dpdmai_attr_v10(uint32_t dpdmai_id,
				 struct dprc_obj_desc *target_obj_desc)
{
	struct dpdmai_attr_v10 dpdmai_attr;
	bool dpdmai_opened = false;
	uint16_t dpdmai_handle;
	uint16_t obj_major, obj_minor;
	int error;

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
	error = dpdmai_get_attributes_v10(&restool.mc_io, 0, dpdmai_handle,
					&dpdmai_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpdmai_id == (uint32_t)dpdmai_attr.id);

	error = dpdmai_get_version_v10(&restool.mc_io, 0,
				       &obj_major, &obj_minor);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	printf("dpdmai version: %u.%u\n", obj_major, obj_minor);
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

static int print_dpdmai_info(uint32_t dpdmai_id, int mc_fw_version)
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

	if (mc_fw_version == MC_FW_VERSION_9)
		error = print_dpdmai_attr_v9(dpdmai_id, &target_obj_desc);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = print_dpdmai_attr_v10(dpdmai_id, &target_obj_desc);
	if (error < 0)
		goto out;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		error = print_obj_verbose(&target_obj_desc, &dpdmai_ops);
	}

out:
	return error;
}

static int info_dpdmai(int mc_fw_version)
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
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_HELP);
		error = 0;
		goto out;
	}

	if (restool.obj_name == NULL) {
		ERROR_PRINTF("<object> argument missing\n");
		puts(usage_msg);
		error = -EINVAL;
		goto out;
	}

	error = parse_object_name(restool.obj_name, "dpdmai", &obj_id);
	if (error < 0)
		goto out;

	error = print_dpdmai_info(obj_id, mc_fw_version);
out:
	return error;
}

static int cmd_dpdmai_info_v9(void)
{
	return info_dpdmai(MC_FW_VERSION_9);
}

static int cmd_dpdmai_info_v10(void)
{
	return info_dpdmai(MC_FW_VERSION_10);
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

static int create_dpdmai_v9(struct dpdmai_cfg *dpdmai_cfg)
{
	struct dpdmai_attr dpdmai_attr;
	uint16_t dpdmai_handle;
	int error;

	error = dpdmai_create(&restool.mc_io, 0, dpdmai_cfg, &dpdmai_handle);
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

static int create_dpdmai_v10(struct dpdmai_cfg *dpdmai_cfg)
{
	uint32_t dpdmai_id, dprc_id;
	uint16_t dprc_handle;
	bool dprc_opened;
	int error;

	dprc_handle = restool.root_dprc_handle;
	dprc_opened = false;
	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_PARENT_DPRC)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_PARENT_DPRC);
		error = parse_object_name(
				restool.cmd_option_args[CREATE_OPT_PARENT_DPRC],
				"dprc", &dprc_id);
		if (error)
			return error;

		if (restool.root_dprc_id != dprc_id) {
			error = open_dprc(dprc_id, &dprc_handle);
			if (error)
				return error;
			dprc_opened = true;
		}
	}

	error = dpdmai_create_v10(&restool.mc_io, dprc_handle, 0,
				  dpdmai_cfg, &dpdmai_id);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	if (dprc_opened) {
		(void)dprc_close(&restool.mc_io, 0, dprc_handle);
		print_new_obj("dpdmai", dpdmai_id,
			      restool.cmd_option_args[CREATE_OPT_PARENT_DPRC]);
	} else {
		print_new_obj("dpdmai", dpdmai_id, NULL);
	}

	return 0;
}

static int create_dpdmai(int mc_fw_version, const char *usage_msg)
{
	int error;
	struct dpdmai_cfg dpdmai_cfg = { { 0 } };

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_HELP)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_HELP);
		return 0;
	}

	if (restool.obj_name != NULL) {
		ERROR_PRINTF("Unexpected argument: \'%s\'\n\n",
			     restool.obj_name);
		puts(usage_msg);
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

	if (mc_fw_version == MC_FW_VERSION_9)
		error = create_dpdmai_v9(&dpdmai_cfg);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = create_dpdmai_v10(&dpdmai_cfg);
	else
		return -EINVAL;

	return error;
}

static int cmd_dpdmai_create_v9(void)
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


	return create_dpdmai(MC_FW_VERSION_9, usage_msg);
}

static int cmd_dpdmai_create_v10(void)
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
		"--container=<container-name>\n"
		"   Specifies the parent container name. e.g. dprc.2, dprc.3 etc.\n"
		"   If it is not specified, the new object will be created under the default dprc.\n"
		"\n"
		"EXAMPLES:\n"
		"create a DPDMAI object with all default options:\n"
		"   $ restool dpdmai create\n"
		"create a DPDMAI with 2,4 priorities:\n"
		"   $ restool dpdmai create --priorities=2,4\n"
		"\n";

	return create_dpdmai(MC_FW_VERSION_10, usage_msg);
}

static int destroy_dpdmai_v9(uint32_t dpdmai_id)
{
	bool dpdmai_opened = false;
	uint16_t dpdmai_handle;
	int error, error2;

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

static int destroy_dpdmai_v10(uint32_t dpdmai_id)
{
	uint16_t dprc_handle;
	uint32_t dprc_id;
	int error;

	dprc_handle = restool.root_dprc_handle;
	dprc_id = restool.root_dprc_id;
	error = get_parent_dprc_id(dpdmai_id, "dpdmai", &dprc_id);
	if (error)
		return error;

	if (dprc_id != restool.root_dprc_id) {
		error = open_dprc(dprc_id, &dprc_handle);
		if (error)
			return error;
	}

	error = dpdmai_destroy_v10(&restool.mc_io, dprc_handle,
				   0, dpdmai_id);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	printf("dpdmai.%u is destroyed\n", dpdmai_id);

out:
	if (dprc_id != restool.root_dprc_id)
		error = dprc_close(&restool.mc_io, 0, dprc_handle);

	return error;
}

static int destroy_dpdmai(int mc_fw_version)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpdmai destroy <dpdmai-object>\n"
		"   e.g. restool dpdmai destroy dpdmai.9\n"
		"\n";

	int error;
	uint32_t dpdmai_id;

	if (restool.cmd_option_mask & ONE_BIT_MASK(DESTROY_OPT_HELP)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(DESTROY_OPT_HELP);
		return 0;
	}

	if (restool.obj_name == NULL) {
		ERROR_PRINTF("<object> argument missing\n");
		puts(usage_msg);
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

	if (mc_fw_version == MC_FW_VERSION_9)
		error = destroy_dpdmai_v9(dpdmai_id);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = destroy_dpdmai_v10(dpdmai_id);
	else
		return -EINVAL;

out:
	return error;
}

static int cmd_dpdmai_destroy_v9(void)
{
	return destroy_dpdmai(MC_FW_VERSION_9);
}

static int cmd_dpdmai_destroy_v10(void)
{
	return destroy_dpdmai(MC_FW_VERSION_10);
}

struct object_command dpdmai_commands_v9[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpdmai_help },

	{ .cmd_name = "info",
	  .options = dpdmai_info_options,
	  .cmd_func = cmd_dpdmai_info_v9 },

	{ .cmd_name = "create",
	  .options = dpdmai_create_options,
	  .cmd_func = cmd_dpdmai_create_v9 },

	{ .cmd_name = "destroy",
	  .options = dpdmai_destroy_options,
	  .cmd_func = cmd_dpdmai_destroy_v9 },

	{ .cmd_name = NULL },
};

struct object_command dpdmai_commands_v10[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpdmai_help },

	{ .cmd_name = "info",
	  .options = dpdmai_info_options,
	  .cmd_func = cmd_dpdmai_info_v10 },

	{ .cmd_name = "create",
	  .options = dpdmai_create_options,
	  .cmd_func = cmd_dpdmai_create_v10 },

	{ .cmd_name = "destroy",
	  .options = dpdmai_destroy_options,
	  .cmd_func = cmd_dpdmai_destroy_v10 },

	{ .cmd_name = NULL },
};


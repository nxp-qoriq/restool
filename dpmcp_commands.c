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
#include <sys/ioctl.h>
#include "restool.h"
#include "utils.h"
#include "mc_v9/fsl_dpmcp.h"
#include "mc_v10/fsl_dpmcp.h"

enum mc_cmd_status mc_status;

/**
 * dpmcp info command options
 */
enum dpmcp_info_options {
	INFO_OPT_HELP = 0,
	INFO_OPT_VERBOSE,
};

static struct option dpmcp_info_options[] = {
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

C_ASSERT(ARRAY_SIZE(dpmcp_info_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpmcp create command options
 */
enum dpmcp_create_options {
	CREATE_OPT_HELP = 0,
	CREATE_OPT_PARENT_DPRC,
};

static struct option dpmcp_create_options[] = {
	[CREATE_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
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

C_ASSERT(ARRAY_SIZE(dpmcp_create_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpmcp destroy command options
 */
enum dpmcp_destroy_options {
	DESTROY_OPT_HELP = 0,
};

static struct option dpmcp_destroy_options[] = {
	[DESTROY_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpmcp_destroy_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static const struct flib_ops dpmcp_ops = {
	.obj_open = dpmcp_open,
	.obj_close = dpmcp_close,
	.obj_get_irq_mask = dpmcp_get_irq_mask,
	.obj_get_irq_status = dpmcp_get_irq_status,
};

static int cmd_dpmcp_help(void)
{
	static const char help_msg[] =
		"\n"
		"Usage: restool dpmcp <command> [--help] [ARGS...]\n"
		"Where <command> can be:\n"
		"   info - displays detailed information about a DPMCP object.\n"
		"   create - creates a new child DPMCP under the root DPRC.\n"
		"   destroy - destroys a child DPMCP under the root DPRC.\n"
		"\n"
		"For command-specific help, use the --help option of each command.\n"
		"\n";

	printf(help_msg);
	return 0;
}

static int print_dpmcp_attr_v9(uint32_t dpmcp_id,
			       struct dprc_obj_desc *target_obj_desc)
{
	uint16_t dpmcp_handle;
	int error;
	struct dpmcp_attr dpmcp_attr;
	bool dpmcp_opened = false;

	error = dpmcp_open(&restool.mc_io, 0, dpmcp_id, &dpmcp_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpmcp_opened = true;
	if (0 == dpmcp_handle) {
		DEBUG_PRINTF(
			"dpmcp_open() returned invalid handle (auth 0) for dpmcp.%u\n",
			dpmcp_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpmcp_attr, 0, sizeof(dpmcp_attr));
	error = dpmcp_get_attributes(&restool.mc_io, 0, dpmcp_handle,
					&dpmcp_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpmcp_id == (uint32_t)dpmcp_attr.id);

	printf("dpmcp version: %u.%u\n", dpmcp_attr.version.major,
	       dpmcp_attr.version.minor);
	printf("dpmcp object id/portal id: %d\n", dpmcp_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	print_obj_label(target_obj_desc);

	error = 0;

out:
	if (dpmcp_opened) {
		int error2;

		error2 = dpmcp_close(&restool.mc_io, 0, dpmcp_handle);
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

static int print_dpmcp_attr_v10(uint32_t dpmcp_id,
			struct dprc_obj_desc *target_obj_desc)
{
	struct dpmcp_attr_v10 dpmcp_attr;
	uint16_t obj_major, obj_minor;
	bool dpmcp_opened = false;
	uint16_t dpmcp_handle;
	int error;

	error = dpmcp_open(&restool.mc_io, 0, dpmcp_id, &dpmcp_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpmcp_opened = true;
	if (0 == dpmcp_handle) {
		DEBUG_PRINTF(
			"dpmcp_open() returned invalid handle (auth 0) for dpmcp.%u\n",
			dpmcp_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpmcp_attr, 0, sizeof(dpmcp_attr));
	error = dpmcp_get_attributes_v10(&restool.mc_io, 0, dpmcp_handle,
					&dpmcp_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpmcp_id == (uint32_t)dpmcp_attr.id);

	error = dpmcp_get_version_v10(&restool.mc_io, 0,
				      &obj_major, &obj_minor);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	printf("dpmcp version: %u.%u\n", obj_major, obj_minor);
	printf("dpmcp object id/portal id: %d\n", dpmcp_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	print_obj_label(target_obj_desc);

	error = 0;

out:
	if (dpmcp_opened) {
		int error2;

		error2 = dpmcp_close(&restool.mc_io, 0, dpmcp_handle);
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

static int print_dpmcp_info(uint32_t dpmcp_id, int mc_fw_version)
{
	int error;
	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	bool found = false;

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
				restool.root_dprc_handle, 0, dpmcp_id,
				"dpmcp", &target_obj_desc,
				&target_parent_dprc_id, &found);
	if (error < 0)
		goto out;

	if (strcmp(target_obj_desc.type, "dpmcp")) {
		printf("dpmcp.%d does not exist\n", dpmcp_id);
		return -EINVAL;
	}

	if (mc_fw_version == MC_FW_VERSION_9)
		error = print_dpmcp_attr_v9(dpmcp_id, &target_obj_desc);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = print_dpmcp_attr_v10(dpmcp_id, &target_obj_desc);
	if (error < 0)
		goto out;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		error = print_obj_verbose(&target_obj_desc, &dpmcp_ops);
	}

out:
	return error;
}

static int info_dpmcp(int mc_fw_version)
{
	static const char usage_msg[] =
	"\n"
		"Usage: restool dpmcp info <dpmcp-object> [--verbose]\n"
		"\n"
		"OPTIONS:\n"
		"--verbose\n"
		"   Shows extended/verbose information about the object\n"
		"\n"
		"EXAMPLE:\n"
		"Display information about dpmcp.5:\n"
		"   $ restool dpmcp info dpmcp.5\n"
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

	error = parse_object_name(restool.obj_name, "dpmcp", &obj_id);
	if (error < 0)
		goto out;

	error = print_dpmcp_info(obj_id, mc_fw_version);
out:
	return error;
}

static int cmd_dpmcp_info_v9(void)
{
	return info_dpmcp(MC_FW_VERSION_9);
}

static int cmd_dpmcp_info_v10(void)
{
	return info_dpmcp(MC_FW_VERSION_10);
}


static int create_dpmcp_v9(struct dpmcp_cfg *dpmcp_cfg)
{
	struct dpmcp_attr dpmcp_attr;
	uint16_t dpmcp_handle;
	int error;

	error = dpmcp_create(&restool.mc_io, 0, dpmcp_cfg, &dpmcp_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	memset(&dpmcp_attr, 0, sizeof(struct dpmcp_attr));
	error = dpmcp_get_attributes(&restool.mc_io, 0, dpmcp_handle,
					&dpmcp_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	print_new_obj("dpmcp", dpmcp_attr.id, NULL);

	error = dpmcp_close(&restool.mc_io, 0, dpmcp_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	return 0;
}

static int create_dpmcp_v10(struct dpmcp_cfg *dpmcp_cfg)
{
	uint32_t dpmcp_id, dprc_id;
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

	error = dpmcp_create_v10(&restool.mc_io, dprc_handle, 0,
				 dpmcp_cfg, &dpmcp_id);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	if (dprc_opened) {
		(void)dprc_close(&restool.mc_io, 0, dprc_handle);
		print_new_obj("dpmcp", dpmcp_id,
			      restool.cmd_option_args[CREATE_OPT_PARENT_DPRC]);
	} else {
		print_new_obj("dpmcp", dpmcp_id, NULL);
	}

	return 0;
}

static int create_dpmcp(int mc_fw_version, const char *usage_msg)
{
	struct dpmcp_cfg dpmcp_cfg = {0};
	int error;

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

	dpmcp_cfg.portal_id = DPMCP_GET_PORTAL_ID_FROM_POOL;

	if (mc_fw_version == MC_FW_VERSION_9)
		error = create_dpmcp_v9(&dpmcp_cfg);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = create_dpmcp_v10(&dpmcp_cfg);
	else
		return -EINVAL;

	return error;
}

static int cmd_dpmcp_create_v9(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpmcp create\n"
		"\n";

	return create_dpmcp(MC_FW_VERSION_9, usage_msg);
}

static int cmd_dpmcp_create_v10(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpmcp create [OPTIONS]\n"
		"\n"
		"OPTIONS:\n"
		"if options are not specified, create DPSW by default options\n"
		"--container=<container-name>\n"
		"   Specifies the parent container name. e.g. dprc.2, dprc.3 etc.\n"
		"   If it is not specified, the new object will be created under the default dprc.\n"
		"\n";

	return create_dpmcp(MC_FW_VERSION_10, usage_msg);
}

static int destroy_dpmcp_v9(uint32_t dpmcp_id)
{
	bool dpmcp_opened = false;
	uint16_t dpmcp_handle;
	int error, error2;

	error = dpmcp_open(&restool.mc_io, 0, dpmcp_id, &dpmcp_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out_v9;
	}
	dpmcp_opened = true;
	if (0 == dpmcp_handle) {
		DEBUG_PRINTF(
			"dpmcp_open() returned invalid handle (auth 0) for dpmcp.%u\n",
			dpmcp_id);
		error = -ENOENT;
		goto out_v9;
	}

	error = dpmcp_destroy(&restool.mc_io, 0, dpmcp_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out_v9;
	}
	dpmcp_opened = false;
	printf("dpmcp.%u is destroyed\n", dpmcp_id);

out_v9:
	if (dpmcp_opened) {
		error2 = dpmcp_close(&restool.mc_io, 0, dpmcp_handle);
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

static int destroy_dpmcp_v10(uint32_t dpmcp_id)
{
	uint16_t dprc_handle;
	uint32_t dprc_id;
	int error;

	dprc_handle = restool.root_dprc_handle;
	dprc_id = restool.root_dprc_id;
	error = get_parent_dprc_id(dpmcp_id, "dpmcp", &dprc_id);
	if (error)
		return error;

	if (dprc_id != restool.root_dprc_id) {
		error = open_dprc(dprc_id, &dprc_handle);
		if (error)
			return error;
	}

	error = dpmcp_destroy_v10(&restool.mc_io, dprc_handle,
				 0, dpmcp_id);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	printf("dpmcp.%u is destroyed\n", dpmcp_id);

out:
	if (dprc_id != restool.root_dprc_id)
		error = dprc_close(&restool.mc_io, 0, dprc_handle);

	return error;
}

static int destroy_dpmcp(int mc_fw_version)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpmcp destroy <dpmcp-object>\n"
		"   e.g. restool dpmcp destroy dpmcp.9\n"
		"\n";

	int error;
	uint32_t dpmcp_id;

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

	error = parse_object_name(restool.obj_name, "dpmcp", &dpmcp_id);
	if (error < 0)
		goto out;

	if (!find_obj("dpmcp", dpmcp_id)) {
		error = -EINVAL;
		goto out;
	}

	if (mc_fw_version == MC_FW_VERSION_9)
		error = destroy_dpmcp_v9(dpmcp_id);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = destroy_dpmcp_v10(dpmcp_id);
	else
		return -EINVAL;

out:
	return error;
}

static int cmd_dpmcp_destroy_v9(void)
{
	return destroy_dpmcp(MC_FW_VERSION_9);
}

static int cmd_dpmcp_destroy_v10(void)
{
	return destroy_dpmcp(MC_FW_VERSION_10);
}

struct object_command dpmcp_commands_v9[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpmcp_help },

	{ .cmd_name = "info",
	  .options = dpmcp_info_options,
	  .cmd_func = cmd_dpmcp_info_v9 },

	{ .cmd_name = "create",
	  .options = dpmcp_create_options,
	  .cmd_func = cmd_dpmcp_create_v9 },

	{ .cmd_name = "destroy",
	  .options = dpmcp_destroy_options,
	  .cmd_func = cmd_dpmcp_destroy_v9 },

	{ .cmd_name = NULL },
};

struct object_command dpmcp_commands_v10[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpmcp_help },

	{ .cmd_name = "info",
	  .options = dpmcp_info_options,
	  .cmd_func = cmd_dpmcp_info_v10 },

	{ .cmd_name = "create",
	  .options = dpmcp_create_options,
	  .cmd_func = cmd_dpmcp_create_v10 },

	{ .cmd_name = "destroy",
	  .options = dpmcp_destroy_options,
	  .cmd_func = cmd_dpmcp_destroy_v10 },

	{ .cmd_name = NULL },
};


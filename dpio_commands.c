/* Copyright 2014-2016 Freescale Semiconductor Inc.
 * Copyright 2017-2018 NXP
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
#include "mc_v9/fsl_dpio.h"
#include "mc_v10/fsl_dpio.h"

enum mc_cmd_status mc_status;

/**
 * dpio info command options
 */
enum dpio_info_options {
	INFO_OPT_HELP = 0,
	INFO_OPT_VERBOSE,
};

static struct option dpio_info_options[] = {
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

C_ASSERT(ARRAY_SIZE(dpio_info_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpio create command options
 */
enum dpio_create_options {
	CREATE_OPT_HELP = 0,
	CREATE_OPT_CHANNEL_MODE,
	CREATE_OPT_NUM_PRIORITIES,
	CREATE_OPT_PARENT_DPRC,
};

static struct option dpio_create_options[] = {
	[CREATE_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_CHANNEL_MODE] = {
		.name = "channel-mode",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_NUM_PRIORITIES] = {
		.name = "num-priorities",
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

C_ASSERT(ARRAY_SIZE(dpio_create_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpio destroy command options
 */
enum dpio_destroy_options {
	DESTROY_OPT_HELP = 0,
};

static struct option dpio_destroy_options[] = {
	[DESTROY_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpio_destroy_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static const struct flib_ops dpio_ops = {
	.obj_open = dpio_open,
	.obj_close = dpio_close,
	.obj_get_irq_mask = dpio_get_irq_mask,
	.obj_get_irq_status = dpio_get_irq_status,
};

static int cmd_dpio_help(void)
{
	static const char help_msg[] =
		"\n"
		"Usage: restool dpio <command> [--help] [ARGS...]\n"
		"Where <command> can be:\n"
		"   info - displays detailed information about a DPIO object.\n"
		"   create - creates a new child DPIO under the root DPRC.\n"
		"   destroy - destroys a child DPIO under the root DPRC.\n"
		"\n"
		"For command-specific help, use the --help option of each command.\n"
		"\n";

	printf(help_msg);
	return 0;
}

static int print_dpio_attr_v9(uint32_t dpio_id,
			      struct dprc_obj_desc *target_obj_desc)
{
	uint16_t dpio_handle;
	int error;
	struct dpio_attr dpio_attr;
	bool dpio_opened = false;

	error = dpio_open(&restool.mc_io, 0, dpio_id, &dpio_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpio_opened = true;
	if (0 == dpio_handle) {
		DEBUG_PRINTF(
			"dpio_open() returned invalid handle (auth 0) for dpio.%u\n",
			dpio_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpio_attr, 0, sizeof(dpio_attr));
	error = dpio_get_attributes(&restool.mc_io, 0, dpio_handle, &dpio_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpio_id == (uint32_t)dpio_attr.id);

	printf("dpio version: %u.%u\n", dpio_attr.version.major,
	       dpio_attr.version.minor);
	printf("dpio id: %d\n", dpio_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	printf(
		"offset of qbman software portal cache-enabled area: %#llx\n",
		(unsigned long long)dpio_attr.qbman_portal_ce_offset);
	printf(
		"offset of qbman software portal cache-inhibited area: %#llx\n",
		(unsigned long long)dpio_attr.qbman_portal_ci_offset);
	printf("qbman software portal id: %#x\n",
	       (unsigned int)dpio_attr.qbman_portal_id);
	printf("dpio channel mode is: ");
	dpio_attr.channel_mode == 0 ? printf("DPIO_NO_CHANNEL\n") :
	dpio_attr.channel_mode == 1 ? printf("DPIO_LOCAL_CHANNEL\n") :
	printf("wrong mode\n");
	printf("number of priorities is: %#x\n",
	       (unsigned int)dpio_attr.num_priorities);
	print_obj_label(target_obj_desc);

	error = 0;

out:
	if (dpio_opened) {
		int error2;

		error2 = dpio_close(&restool.mc_io, 0, dpio_handle);
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

static int print_dpio_attr_v10(uint32_t dpio_id,
			       struct dprc_obj_desc *target_obj_desc)
{
	struct dpio_attr_v10 dpio_attr;
	bool dpio_opened = false;
	uint16_t dpio_handle;
	uint16_t obj_major, obj_minor;
	int error;

	error = dpio_open_v10(&restool.mc_io, 0, dpio_id, &dpio_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpio_opened = true;
	if (0 == dpio_handle) {
		DEBUG_PRINTF(
			"dpio_open() returned invalid handle (auth 0) for dpio.%u\n",
			dpio_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpio_attr, 0, sizeof(dpio_attr));
	error = dpio_get_attributes_v10(&restool.mc_io, 0,
					dpio_handle, &dpio_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpio_id == (uint32_t)dpio_attr.id);

	error = dpio_get_api_version_v10(&restool.mc_io, 0, &obj_major, &obj_minor);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	printf("dpio version: %u.%u\n", obj_major, obj_minor);
	printf("dpio id: %d\n", dpio_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	printf(
		"offset of qbman software portal cache-enabled area: %#llx\n",
		(unsigned long long)dpio_attr.qbman_portal_ce_offset);
	printf(
		"offset of qbman software portal cache-inhibited area: %#llx\n",
		(unsigned long long)dpio_attr.qbman_portal_ci_offset);
	printf("qbman software portal id: %#x\n",
	       (unsigned int)dpio_attr.qbman_portal_id);
	printf("dpio channel mode is: ");
	dpio_attr.channel_mode == 0 ? printf("DPIO_NO_CHANNEL\n") :
	dpio_attr.channel_mode == 1 ? printf("DPIO_LOCAL_CHANNEL\n") :
	printf("wrong mode\n");
	printf("number of priorities is: %#x\n",
	       (unsigned int)dpio_attr.num_priorities);
	print_obj_label(target_obj_desc);

	error = 0;

out:
	if (dpio_opened) {
		int error2;

		error2 = dpio_close_v10(&restool.mc_io, 0, dpio_handle);
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

static int print_dpio_info(uint32_t dpio_id, int mc_fw_version)
{
	int error;
	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	bool found = false;

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
				restool.root_dprc_handle, 0, dpio_id,
				"dpio", &target_obj_desc,
				&target_parent_dprc_id, &found);
	if (error < 0)
		goto out;

	if (strcmp(target_obj_desc.type, "dpio")) {
		printf("dpio.%d does not exist\n", dpio_id);
		return -EINVAL;
	}

	if (mc_fw_version == MC_FW_VERSION_9)
		error = print_dpio_attr_v9(dpio_id, &target_obj_desc);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = print_dpio_attr_v10(dpio_id, &target_obj_desc);
	if (error < 0)
		goto out;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		error = print_obj_verbose(&target_obj_desc, &dpio_ops);
	}

out:
	return error;
}

static int info_dpio(int mc_fw_version)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpio info <dpio-object> [--verbose]\n"
		"\n"
		"OPTIONS:\n"
		"--verbose\n"
		"   Shows extended/verbose information about the object\n"
		"\n"
		"EXAMPLE:\n"
		"Display information about dpio.5:\n"
		"   $ restool dpio info dpio.5\n"
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

	error = parse_object_name(restool.obj_name, "dpio", &obj_id);
	if (error < 0)
		goto out;

	error = print_dpio_info(obj_id, mc_fw_version);
out:
	return error;
}

static int cmd_dpio_info_v9(void)
{
	return info_dpio(MC_FW_VERSION_9);
}

static int cmd_dpio_info_v10(void)
{
	return info_dpio(MC_FW_VERSION_10);
}

static int create_dpio_v9(struct dpio_cfg *dpio_cfg)
{
	struct dpio_attr dpio_attr;
	uint16_t dpio_handle;
	int error;

	error = dpio_create(&restool.mc_io, 0, dpio_cfg, &dpio_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	memset(&dpio_attr, 0, sizeof(struct dpio_attr));
	error = dpio_get_attributes(&restool.mc_io, 0, dpio_handle, &dpio_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	print_new_obj("dpio", dpio_attr.id, NULL);

	error = dpio_close(&restool.mc_io, 0, dpio_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	return 0;
}

static int create_dpio_v10(struct dpio_cfg_v10 *dpio_cfg)
{
	uint32_t dpio_id, dprc_id;
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

	error = dpio_create_v10(&restool.mc_io, dprc_handle, 0,
				dpio_cfg, &dpio_id);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	if (dprc_opened) {
		(void)dprc_close(&restool.mc_io, 0, dprc_handle);
		print_new_obj("dpio", dpio_id,
			      restool.cmd_option_args[CREATE_OPT_PARENT_DPRC]);
	} else {
		print_new_obj("dpio", dpio_id, NULL);
	}

	return error;
}

static int create_dpio(int mc_fw_version, const char *usage_msg)
{
	int error;
	long val;
	struct dpio_cfg_v10 dpio_cfg_v10;
	struct dpio_cfg dpio_cfg;

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

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_CHANNEL_MODE)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_CHANNEL_MODE);
		if (strcmp(restool.cmd_option_args[CREATE_OPT_CHANNEL_MODE],
		    "DPIO_LOCAL_CHANNEL") == 0) {
			dpio_cfg.channel_mode = DPIO_LOCAL_CHANNEL;
			dpio_cfg_v10.channel_mode = DPIO_LOCAL_CHANNEL;
		} else if (
			strcmp(restool.cmd_option_args[CREATE_OPT_CHANNEL_MODE],
			"DPIO_NO_CHANNEL") == 0) {
			dpio_cfg.channel_mode = DPIO_NO_CHANNEL;
			dpio_cfg_v10.channel_mode = DPIO_NO_CHANNEL;
		} else {
			ERROR_PRINTF("wrong channel mode\n");
			puts(usage_msg);
			return -EINVAL;
		}
	} else {
		dpio_cfg.channel_mode = DPIO_LOCAL_CHANNEL;
		dpio_cfg_v10.channel_mode = DPIO_LOCAL_CHANNEL;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_NUM_PRIORITIES)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_NUM_PRIORITIES);
		error = get_option_value(CREATE_OPT_NUM_PRIORITIES, &val,
					 "Invalid value: num-priorities option",
					 1, 8);
		if (error)
			return -EINVAL;
		dpio_cfg.num_priorities = (uint8_t)val;
		dpio_cfg_v10.num_priorities = (uint8_t)val;
	} else {
		dpio_cfg.num_priorities = 8;
		dpio_cfg_v10.num_priorities = 8;
	}

	if (mc_fw_version == MC_FW_VERSION_9)
		error = create_dpio_v9(&dpio_cfg);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = create_dpio_v10(&dpio_cfg_v10);
	else
		return -EINVAL;

	return error;
}

static int cmd_dpio_create_v9(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpio create [OPTIONS]\n"
		"\n"
		"OPTIONS:\n"
		"if options are not specified, create DPIO by default options\n"
		"--channel-mode=<mode>\n"
		"   Where <mode> is one of:\n"
		"	DPIO_LOCAL_CHANNEL\n"
		"	DPIO_NO_CHANNEL\n"
		"   Default value is DPIO_LOCAL_CHANNEL\n"
		"--num-priorities=<number>\n"
		"   Valid values for <number> are 1-8. Default value is 8.\n"
		"\n"
		"EXAMPLE:\n"
		"Create a DPIO object with all default options:\n"
		"   $ restool dpio create\n"
		"\n";

	return create_dpio(MC_FW_VERSION_9, usage_msg);
}

static int cmd_dpio_create_v10(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpio create [OPTIONS]\n"
		"\n"
		"OPTIONS:\n"
		"if options are not specified, create DPIO by default options\n"
		"--channel-mode=<mode>\n"
		"   Where <mode> is one of:\n"
		"	DPIO_LOCAL_CHANNEL\n"
		"	DPIO_NO_CHANNEL\n"
		"   Default value is DPIO_LOCAL_CHANNEL\n"
		"--num-priorities=<number>\n"
		"   Valid values for <number> are 1-8. Default value is 8.\n"
		"--container=<container-name>\n"
		"   Specifies the parent container name. e.g. dprc.2, dprc.3 etc.\n"
		"   If it is not specified, the new object will be created under the default dprc.\n"
		"\n"
		"EXAMPLE:\n"
		"Create a DPIO object with all default options:\n"
		"   $ restool dpio create\n"
		"\n";

	return create_dpio(MC_FW_VERSION_10, usage_msg);
}

static int destroy_dpio_v9(uint32_t dpio_id)
{
	bool dpio_opened = false;
	uint16_t dpio_handle;
	int error, error2;

	error = dpio_open(&restool.mc_io, 0, dpio_id, &dpio_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpio_opened = true;
	if (0 == dpio_handle) {
		DEBUG_PRINTF(
			"dpio_open() returned invalid handle (auth 0) for dpio.%u\n",
			dpio_id);
		error = -ENOENT;
		goto out;
	}

	error = dpio_destroy(&restool.mc_io, 0, dpio_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpio_opened = false;
	printf("dpio.%u is destroyed\n", dpio_id);

out:
	if (dpio_opened) {
		error2 = dpio_close(&restool.mc_io, 0, dpio_handle);
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

static int destroy_dpio_v10(uint32_t dpio_id)
{
	uint16_t dprc_handle;
	uint32_t dprc_id;
	int error;

	dprc_handle = restool.root_dprc_handle;
	dprc_id = restool.root_dprc_id;
	error = get_parent_dprc_id(dpio_id, "dpio", &dprc_id);
	if (error)
		return error;

	if (dprc_id != restool.root_dprc_id) {
		error = open_dprc(dprc_id, &dprc_handle);
		if (error)
			return error;
	}

	error = dpio_destroy_v10(&restool.mc_io, dprc_handle,
				 0, dpio_id);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	printf("dpio.%u is destroyed\n", dpio_id);

out:
	if (dprc_id != restool.root_dprc_id)
		error = dprc_close(&restool.mc_io, 0, dprc_handle);

	return error;
}

static int destroy_dpio(int mc_fw_version)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpio destroy <dpio-object>\n"
		"   e.g. restool dpio destroy dpio.9\n"
		"\n";

	int error;
	uint32_t dpio_id;

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

	error = parse_object_name(restool.obj_name, "dpio", &dpio_id);
	if (error < 0)
		goto out;

	if (!find_obj("dpio", dpio_id)) {
		error = -EINVAL;
		goto out;
	}

	if (mc_fw_version == MC_FW_VERSION_9)
		error = destroy_dpio_v9(dpio_id);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = destroy_dpio_v10(dpio_id);
	else
		return -EINVAL;

out:
	return error;
}

static int cmd_dpio_destroy_v9(void)
{
	return destroy_dpio(MC_FW_VERSION_9);
}

static int cmd_dpio_destroy_v10(void)
{
	return destroy_dpio(MC_FW_VERSION_10);
}

struct object_command dpio_commands_v9[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpio_help },

	{ .cmd_name = "info",
	  .options = dpio_info_options,
	  .cmd_func = cmd_dpio_info_v9 },

	{ .cmd_name = "create",
	  .options = dpio_create_options,
	  .cmd_func = cmd_dpio_create_v9 },

	{ .cmd_name = "destroy",
	  .options = dpio_destroy_options,
	  .cmd_func = cmd_dpio_destroy_v9 },

	{ .cmd_name = NULL },
};

struct object_command dpio_commands_v10[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpio_help },

	{ .cmd_name = "info",
	  .options = dpio_info_options,
	  .cmd_func = cmd_dpio_info_v10 },

	{ .cmd_name = "create",
	  .options = dpio_create_options,
	  .cmd_func = cmd_dpio_create_v10 },

	{ .cmd_name = "destroy",
	  .options = dpio_destroy_options,
	  .cmd_func = cmd_dpio_destroy_v10 },

	{ .cmd_name = NULL },
};


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
#include "mc_v9/fsl_dpdcei.h"
#include "mc_v10/fsl_dpdcei.h"

enum mc_cmd_status mc_status;

/**
 * dpdcei info command options
 */
enum dpdcei_info_options {
	INFO_OPT_HELP = 0,
	INFO_OPT_VERBOSE,
};

static struct option dpdcei_info_options[] = {
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

C_ASSERT(ARRAY_SIZE(dpdcei_info_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpdcei create command options
 */
enum dpdcei_create_options {
	CREATE_OPT_HELP = 0,
	CREATE_OPT_ENGINE,
	CREATE_OPT_PRIORITY,
	CREATE_OPT_PARENT_DPRC,
};

static struct option dpdcei_create_options[] = {
	[CREATE_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_ENGINE] = {
		.name = "engine",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_PRIORITY] = {
		.name = "priority",
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

C_ASSERT(ARRAY_SIZE(dpdcei_create_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpdcei destroy command options
 */
enum dpdcei_destroy_options {
	DESTROY_OPT_HELP = 0,
};

static struct option dpdcei_destroy_options[] = {
	[DESTROY_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpdcei_destroy_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static const struct flib_ops dpdcei_ops = {
	.obj_open = dpdcei_open,
	.obj_close = dpdcei_close,
	.obj_get_irq_mask = dpdcei_get_irq_mask,
	.obj_get_irq_status = dpdcei_get_irq_status,
};

static int cmd_dpdcei_help(void)
{
	static const char help_msg[] =
		"\n"
		"Usage: restool dpdcei <command> [--help] [ARGS...]\n"
		"Where <command> can be:\n"
		"   info - displays detailed information about a DPDCEI object.\n"
		"   create - creates a new child DPDCEI under the root DPRC.\n"
		"   destroy - destroys a child DPDCEI under the root DPRC.\n"
		"\n"
		"For command-specific help, use the --help option of each command.\n"
		"\n";

	printf(help_msg);
	return 0;
}

static void print_dpdcei_engine(enum dpdcei_engine engine)
{
	printf("DPDCEI engine: ");
	switch (engine) {
	case DPDCEI_ENGINE_COMPRESSION:
		printf("DPDCEI_ENGINE_COMPRESSION\n");
		break;
	case DPDCEI_ENGINE_DECOMPRESSION:
		printf("DPDCEI_ENGINE_DECOMPRESSION\n");
		break;
	default:
		assert(false);
		break;
	}
}

static int print_dpdcei_attr_v9(uint32_t dpdcei_id,
				struct dprc_obj_desc *target_obj_desc)
{
	uint16_t dpdcei_handle;
	int error;
	struct dpdcei_attr dpdcei_attr;
	bool dpdcei_opened = false;

	error = dpdcei_open(&restool.mc_io, 0, dpdcei_id, &dpdcei_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpdcei_opened = true;
	if (0 == dpdcei_handle) {
		DEBUG_PRINTF(
			"dpdcei_open() returned invalid handle (auth 0) for dpdcei.%u\n",
			dpdcei_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpdcei_attr, 0, sizeof(dpdcei_attr));
	error = dpdcei_get_attributes(&restool.mc_io, 0, dpdcei_handle,
					&dpdcei_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpdcei_id == (uint32_t)dpdcei_attr.id);

	printf("dpdcei version: %u.%u\n", dpdcei_attr.version.major,
	       dpdcei_attr.version.minor);
	printf("dpdcei id: %d\n", dpdcei_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	print_dpdcei_engine(dpdcei_attr.engine);
	print_obj_label(target_obj_desc);

	error = 0;

out:
	if (dpdcei_opened) {
		int error2;

		error2 = dpdcei_close(&restool.mc_io, 0, dpdcei_handle);
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

static int print_dpdcei_attr_v10(uint32_t dpdcei_id,
				 struct dprc_obj_desc *target_obj_desc)
{
	struct dpdcei_attr_v10 dpdcei_attr;
	uint16_t obj_major, obj_minor;
	bool dpdcei_opened = false;
	uint16_t dpdcei_handle;
	int error;

	error = dpdcei_open(&restool.mc_io, 0, dpdcei_id, &dpdcei_handle);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpdcei_opened = true;
	if (!dpdcei_handle) {
		DEBUG_PRINTF(
			"dpdcei_open() returned invalid handle (auth 0) for dpdcei.%u\n",
			dpdcei_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpdcei_attr, 0, sizeof(dpdcei_attr));
	error = dpdcei_get_attributes_v10(&restool.mc_io, 0, dpdcei_handle,
					  &dpdcei_attr);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpdcei_id == (uint32_t)dpdcei_attr.id);

	error = dpdcei_get_version_v10(&restool.mc_io, 0,
				       &obj_major, &obj_minor);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	printf("dpdcei version: %u.%u\n", obj_major, obj_minor);
	printf("dpdcei id: %d\n", dpdcei_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	print_dpdcei_engine(dpdcei_attr.engine);
	print_obj_label(target_obj_desc);

	error = 0;

out:
	if (dpdcei_opened) {
		int error2;

		error2 = dpdcei_close(&restool.mc_io, 0, dpdcei_handle);
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

static int print_dpdcei_info(uint32_t dpdcei_id, int mc_fw_version)
{
	int error;
	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	bool found = false;

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
				restool.root_dprc_handle, 0, dpdcei_id,
				"dpdcei", &target_obj_desc,
				&target_parent_dprc_id, &found);
	if (error < 0)
		goto out;

	if (strcmp(target_obj_desc.type, "dpdcei")) {
		printf("dpdcei.%d does not exist\n", dpdcei_id);
		return -EINVAL;
	}

	if (mc_fw_version == MC_FW_VERSION_9)
		error = print_dpdcei_attr_v9(dpdcei_id, &target_obj_desc);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = print_dpdcei_attr_v10(dpdcei_id, &target_obj_desc);
	if (error < 0)
		goto out;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		error = print_obj_verbose(&target_obj_desc, &dpdcei_ops);
	}

out:
	return error;
}

static int info_dpdcei(int mc_fw_version)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpdcei info <dpdcei-object> [--verbose]\n"
		"\n"
		"OPTIONS:\n"
		"--verbose\n"
		"   Shows extended/verbose information about the object\n"
		"\n"
		"EXAMPLE:\n"
		"Display information about dpdcei.5:\n"
		"   $ restool dpdcei info dpdcei.5\n"
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

	error = parse_object_name(restool.obj_name, "dpdcei", &obj_id);
	if (error < 0)
		goto out;

	error = print_dpdcei_info(obj_id, mc_fw_version);
out:
	return error;
}

static int cmd_dpdcei_info_v9(void)
{
	return info_dpdcei(MC_FW_VERSION_9);
}

static int cmd_dpdcei_info_v10(void)
{
	return info_dpdcei(MC_FW_VERSION_10);
}

static int parse_dpdcei_engine(char *engine_str, enum dpdcei_engine *engine)
{
	if (strcmp(engine_str, "DPDCEI_ENGINE_COMPRESSION") == 0) {
		*engine = DPDCEI_ENGINE_COMPRESSION;
		return 0;
	}

	if (strcmp(engine_str, "DPDCEI_ENGINE_DECOMPRESSION") == 0) {
		*engine = DPDCEI_ENGINE_DECOMPRESSION;
		return 0;
	}

	printf("Invalid dpdcei engine input.\n");
	return -EINVAL;
}

static int create_dpdcei_v9(struct dpdcei_cfg *dpdcei_cfg)
{
	struct dpdcei_attr dpdcei_attr;
	uint16_t dpdcei_handle;
	int error;

	error = dpdcei_create(&restool.mc_io, 0, dpdcei_cfg, &dpdcei_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	memset(&dpdcei_attr, 0, sizeof(struct dpdcei_attr));
	error = dpdcei_get_attributes(&restool.mc_io, 0, dpdcei_handle,
					&dpdcei_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	print_new_obj("dpdcei", dpdcei_attr.id, NULL);

	error = dpdcei_close(&restool.mc_io, 0, dpdcei_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	return 0;
}

static int create_dpdcei_v10(struct dpdcei_cfg *dpdcei_cfg)
{
	uint32_t dpdcei_id, dprc_id;
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

	error = dpdcei_create_v10(&restool.mc_io, dprc_handle, 0,
				  dpdcei_cfg, &dpdcei_id);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	if (dprc_opened) {
		(void)dprc_close(&restool.mc_io, 0, dprc_handle);
		print_new_obj("dpdcei", dpdcei_id,
			      restool.cmd_option_args[CREATE_OPT_PARENT_DPRC]);
	} else {
		print_new_obj("dpdcei", dpdcei_id, NULL);
	}

	return 0;
}

static int create_dpdcei(int mc_fw_version, const char *usage_msg)
{
	int error;
	struct dpdcei_cfg dpdcei_cfg;
	long val;

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

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_ENGINE)) {
		restool.cmd_option_mask &=
				~ONE_BIT_MASK(CREATE_OPT_ENGINE);
		error = parse_dpdcei_engine(
			restool.cmd_option_args[CREATE_OPT_ENGINE],
			&dpdcei_cfg.engine);
		if (error < 0) {
			DEBUG_PRINTF(
				"parse_dpdcei_engine() failed with error %d, cannot get dpdcei engine\n",
				error);
			return error;
		}
	} else {
		ERROR_PRINTF("--engine option missing\n");
		puts(usage_msg);
		return -EINVAL;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_PRIORITY)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_PRIORITY);
		error = get_option_value(CREATE_OPT_PRIORITY, &val,
					 "Invalid value: priority option",
					 1, 8);
		if (error)
			return -EINVAL;
		dpdcei_cfg.priority = (uint8_t)val;
	} else {
		ERROR_PRINTF("--priority option missing\n");
		puts(usage_msg);
		return -EINVAL;
	}

	if (mc_fw_version == MC_FW_VERSION_9)
		error = create_dpdcei_v9(&dpdcei_cfg);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = create_dpdcei_v10(&dpdcei_cfg);
	else
		return -EINVAL;

	return error;
}

static int cmd_dpdcei_create_v9(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpdcei create --engine=<engine> --priority=<number>\n"
		"\n"
		"--engine=<engine>\n"
		"   compression or decompression engine to be selected.\n"
		"   A valid value is one of the following:\n"
		"	DPDCEI_ENGINE_COMPRESSION\n"
		"	DPDCEI_ENGINE_DECOMPRESSION\n"
		"--priority=<number>\n"
		"   Priority for DCE hardware processing (valid values 1-8)\n"
		"\n";

	return create_dpdcei(MC_FW_VERSION_9, usage_msg);
}

static int cmd_dpdcei_create_v10(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpdcei create --engine=<engine> --priority=<number> [OPTIONS]\n"
		"\n"
		"--engine=<engine>\n"
		"   compression or decompression engine to be selected.\n"
		"   A valid value is one of the following:\n"
		"	DPDCEI_ENGINE_COMPRESSION\n"
		"	DPDCEI_ENGINE_DECOMPRESSION\n"
		"--priority=<number>\n"
		"   Priority for DCE hardware processing (valid values 1-8)\n"
		"\n"
		"OPTIONS:\n"
		"if options are not specified, create DPSW by default options\n"
		"--container=<container-name>\n"
		"   Specifies the parent container name. e.g. dprc.2, dprc.3 etc.\n"
		"   If it is not specified, the new object will be created under the default dprc.\n"
		"\n";

	return create_dpdcei(MC_FW_VERSION_10, usage_msg);
}

static int destroy_dpdcei_v9(uint32_t dpdcei_id)
{
	bool dpdcei_opened = false;
	uint16_t dpdcei_handle;
	int error, error2;

	error = dpdcei_open(&restool.mc_io, 0, dpdcei_id, &dpdcei_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpdcei_opened = true;
	if (0 == dpdcei_handle) {
		DEBUG_PRINTF(
			"dpdcei_open() returned invalid handle (auth 0) for dpdcei.%u\n",
			dpdcei_id);
		error = -ENOENT;
		goto out;
	}

	error = dpdcei_destroy(&restool.mc_io, 0, dpdcei_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpdcei_opened = false;
	printf("dpdcei.%u is destroyed\n", dpdcei_id);

out:
	if (dpdcei_opened) {
		error2 = dpdcei_close(&restool.mc_io, 0, dpdcei_handle);
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

static int destroy_dpdcei_v10(uint32_t dpdcei_id)
{
	uint16_t dprc_handle;
	uint32_t dprc_id;
	int error;

	dprc_handle = restool.root_dprc_handle;
	dprc_id = restool.root_dprc_id;
	error = get_parent_dprc_id(dpdcei_id, "dpdcei", &dprc_id);
	if (error)
		return error;

	if (dprc_id != restool.root_dprc_id) {
		error = open_dprc(dprc_id, &dprc_handle);
		if (error)
			return error;
	}

	error = dpdcei_destroy_v10(&restool.mc_io, dprc_handle,
				   0, dpdcei_id);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	printf("dpdcei.%u is destroyed\n", dpdcei_id);

out:
	if (dprc_id != restool.root_dprc_id)
		error = dprc_close(&restool.mc_io, 0, dprc_handle);

	return error;
}

static int destroy_dpdcei(int mc_fw_version)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpdcei destroy <dpdcei-object>\n"
		"   e.g. restool dpdcei destroy dpdcei.9\n"
		"\n";

	int error;
	uint32_t dpdcei_id;

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

	error = parse_object_name(restool.obj_name, "dpdcei", &dpdcei_id);
	if (error < 0)
		goto out;

	if (!find_obj("dpdcei", dpdcei_id)) {
		error = -EINVAL;
		goto out;
	}

	if (mc_fw_version == MC_FW_VERSION_9)
		error = destroy_dpdcei_v9(dpdcei_id);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = destroy_dpdcei_v10(dpdcei_id);
	else
		return -EINVAL;

out:
	return error;
}

static int cmd_dpdcei_destroy_v9(void)
{
	return destroy_dpdcei(MC_FW_VERSION_9);
}

static int cmd_dpdcei_destroy_v10(void)
{
	return destroy_dpdcei(MC_FW_VERSION_10);
}

struct object_command dpdcei_commands_v9[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpdcei_help },

	{ .cmd_name = "info",
	  .options = dpdcei_info_options,
	  .cmd_func = cmd_dpdcei_info_v9 },

	{ .cmd_name = "create",
	  .options = dpdcei_create_options,
	  .cmd_func = cmd_dpdcei_create_v9 },

	{ .cmd_name = "destroy",
	  .options = dpdcei_destroy_options,
	  .cmd_func = cmd_dpdcei_destroy_v9 },

	{ .cmd_name = NULL },
};

struct object_command dpdcei_commands_v10[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpdcei_help },

	{ .cmd_name = "info",
	  .options = dpdcei_info_options,
	  .cmd_func = cmd_dpdcei_info_v10 },

	{ .cmd_name = "create",
	  .options = dpdcei_create_options,
	  .cmd_func = cmd_dpdcei_create_v10 },

	{ .cmd_name = "destroy",
	  .options = dpdcei_destroy_options,
	  .cmd_func = cmd_dpdcei_destroy_v10 },

	{ .cmd_name = NULL },
};


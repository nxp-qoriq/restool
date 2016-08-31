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
#include <sys/ioctl.h>
#include "restool.h"
#include "utils.h"
#include "mc_v8/fsl_dpcon.h"
#include "mc_v10/fsl_dpcon.h"

enum mc_cmd_status mc_status;

/**
 * dpcon info command options
 */
enum dpcon_info_options {
	INFO_OPT_HELP = 0,
	INFO_OPT_VERBOSE,
};

static struct option dpcon_info_options[] = {
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

C_ASSERT(ARRAY_SIZE(dpcon_info_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpcon create command options
 */
enum dpcon_create_options {
	CREATE_OPT_HELP = 0,
	CREATE_OPT_NUM_PRIORITIES,
};

static struct option dpcon_create_options[] = {
	[CREATE_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_NUM_PRIORITIES] = {
		.name = "num-priorities",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpcon_create_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpcon destroy command options
 */
enum dpcon_destroy_options {
	DESTROY_OPT_HELP = 0,
};

static struct option dpcon_destroy_options[] = {
	[DESTROY_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpcon_destroy_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static const struct flib_ops dpcon_ops = {
	.obj_open = dpcon_open,
	.obj_close = dpcon_close,
	.obj_get_irq_mask = dpcon_get_irq_mask,
	.obj_get_irq_status = dpcon_get_irq_status,
};

static int cmd_dpcon_help(void)
{
	static const char help_msg[] =
		"\n"
		"Usage: restool dpcon <command> [--help] [ARGS...]\n"
		"Where <command> can be:\n"
		"   info - displays detailed information about a DPCON object.\n"
		"   create - creates a new child DPCON under the root DPRC.\n"
		"   destroy - destroys a child DPCON under the root DPRC.\n"
		"\n"
		"For command-specific help, use the --help option of each command.\n"
		"\n";

	printf(help_msg);
	return 0;
}

static int print_dpcon_attr(uint32_t dpcon_id,
			struct dprc_obj_desc *target_obj_desc)
{
	uint16_t dpcon_handle;
	int error;
	struct dpcon_attr dpcon_attr;
	bool dpcon_opened = false;

	error = dpcon_open(&restool.mc_io, 0, dpcon_id, &dpcon_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpcon_opened = true;
	if (0 == dpcon_handle) {
		DEBUG_PRINTF(
			"dpcon_open() returned invalid handle (auth 0) for dpcon.%u\n",
			dpcon_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpcon_attr, 0, sizeof(dpcon_attr));
	error = dpcon_get_attributes(&restool.mc_io, 0, dpcon_handle,
					&dpcon_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpcon_id == (uint32_t)dpcon_attr.id);

	printf("dpcon version: %u.%u\n", dpcon_attr.version.major,
	       dpcon_attr.version.minor);
	printf("dpcon id: %d\n", dpcon_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	printf("qbman channel id to be used by dequeue operation: %u\n",
		dpcon_attr.qbman_ch_id);
	printf("number of priorities for the DPCON channel: %u\n",
		dpcon_attr.num_priorities);
	print_obj_label(target_obj_desc);

	error = 0;

out:
	if (dpcon_opened) {
		int error2;

		error2 = dpcon_close(&restool.mc_io, 0, dpcon_handle);
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

static int print_dpcon_attr_v10(uint32_t dpcon_id,
				struct dprc_obj_desc *target_obj_desc)
{
	struct dpcon_attr_v10 dpcon_attr;
	uint16_t obj_major, obj_minor;
	bool dpcon_opened = false;
	uint16_t dpcon_handle;
	int error;

	error = dpcon_open(&restool.mc_io, 0, dpcon_id, &dpcon_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpcon_opened = true;
	if (0 == dpcon_handle) {
		DEBUG_PRINTF(
			"dpcon_open() returned invalid handle (auth 0) for dpcon.%u\n",
			dpcon_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpcon_attr, 0, sizeof(dpcon_attr));
	error = dpcon_get_attributes_v10(&restool.mc_io, 0, dpcon_handle,
					 &dpcon_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpcon_id == (uint32_t)dpcon_attr.id);

	error = dpcon_get_version_v10(&restool.mc_io, 0,
				      &obj_major, &obj_minor);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	printf("dpcon version: %u.%u\n", obj_major, obj_minor);
	printf("dpcon id: %d\n", dpcon_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	printf("qbman channel id to be used by dequeue operation: %u\n",
		dpcon_attr.qbman_ch_id);
	printf("number of priorities for the DPCON channel: %u\n",
		dpcon_attr.num_priorities);
	print_obj_label(target_obj_desc);

	error = 0;

out:
	if (dpcon_opened) {
		int error2;

		error2 = dpcon_close(&restool.mc_io, 0, dpcon_handle);
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

static int print_dpcon_info(uint32_t dpcon_id, int mc_fw_version)
{
	int error;
	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	bool found = false;

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
				restool.root_dprc_handle, 0, dpcon_id,
				"dpcon", &target_obj_desc,
				&target_parent_dprc_id, &found);
	if (error < 0)
		goto out;

	if (strcmp(target_obj_desc.type, "dpcon")) {
		printf("dpcon.%d does not exist\n", dpcon_id);
		return -EINVAL;
	}

	if (mc_fw_version == MC_FW_VERSION_8 || mc_fw_version == MC_FW_VERSION_9)
		error = print_dpcon_attr(dpcon_id, &target_obj_desc);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = print_dpcon_attr_v10(dpcon_id, &target_obj_desc);
	if (error < 0)
		goto out;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		error = print_obj_verbose(&target_obj_desc, &dpcon_ops);
	}

out:
	return error;
}

static int info_dpcon(int mc_fw_version)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpcon info <dpcon-object> [--verbose]\n"
		"\n"
		"OPTIONS:\n"
		"--verbose\n"
		"   Shows extended/verbose information about the object\n"
		"\n"
		"EXAMPLE:\n"
		"Display information about dpcon.5:\n"
		"   $ restool dpcon info dpcon.5\n"
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

	error = parse_object_name(restool.obj_name, "dpcon", &obj_id);
	if (error < 0)
		goto out;

	error = print_dpcon_info(obj_id, mc_fw_version);
out:
	return error;
}

static int cmd_dpcon_info(void)
{
	return info_dpcon(MC_FW_VERSION_8);
}

static int cmd_dpcon_info_v10(void)
{
	return info_dpcon(MC_FW_VERSION_10);
}

static int create_dpcon_v8(struct dpcon_cfg *dpcon_cfg)
{
	struct dpcon_attr dpcon_attr;
	uint16_t dpcon_handle;
	int error;

	error = dpcon_create(&restool.mc_io, 0, dpcon_cfg, &dpcon_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	memset(&dpcon_attr, 0, sizeof(struct dpcon_attr));
	error = dpcon_get_attributes(&restool.mc_io, 0, dpcon_handle,
					&dpcon_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	print_new_obj("dpcon", dpcon_attr.id, NULL);

	error = dpcon_close(&restool.mc_io, 0, dpcon_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	return 0;
}

static int create_dpcon_v10(struct dpcon_cfg *dpcon_cfg)
{
	uint32_t dpcon_id;
	int error;

	error = dpcon_create_v10(&restool.mc_io, 0, 0, dpcon_cfg, &dpcon_id);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	print_new_obj("dpcon", dpcon_id, NULL);

	return 0;
}

static int create_dpcon(int mc_fw_version)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpcon create [OPTIONS]\n"
		"\n"
		"OPTIONS:\n"
		"if options are not specified, create DPCON by default options\n"
		"--num-priorities=<number>\n"
		"   Valid values for <number> are 1-8. Default value is 1.\n"
		"\n"
		"EXAMPLES:\n"
		"Create a DPCON object with all default options:\n"
		"   $ restool dpcon create\n"
		"Create a DPCON with 4 priorities:\n"
		"   $ restool dpcon create --num-priorities=4\n"
		"\n";

	int error;
	long val;
	struct dpcon_cfg dpcon_cfg;

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

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_NUM_PRIORITIES)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_NUM_PRIORITIES);
		error = get_option_value(CREATE_OPT_NUM_PRIORITIES, &val,
					 "Invalid value: num-priorities option",
					 1, 8);
		if (error)
			return -EINVAL;

		dpcon_cfg.num_priorities = (uint8_t)val;
	} else {
		dpcon_cfg.num_priorities = 1;
	}

	if (mc_fw_version == MC_FW_VERSION_8)
		error = create_dpcon_v8(&dpcon_cfg);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = create_dpcon_v10(&dpcon_cfg);
	else
		return -EINVAL;

	return error;
}

static int cmd_dpcon_create(void)
{
	return create_dpcon(MC_FW_VERSION_8);
}

static int cmd_dpcon_create_v10(void)
{
	return create_dpcon(MC_FW_VERSION_10);
}

static int destroy_dpcon_v8(uint32_t dpcon_id)
{
	bool dpcon_opened = false;
	uint16_t dpcon_handle;
	int error, error2;

	error = dpcon_open(&restool.mc_io, 0, dpcon_id, &dpcon_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpcon_opened = true;
	if (0 == dpcon_handle) {
		DEBUG_PRINTF(
			"dpcon_open() returned invalid handle (auth 0) for dpcon.%u\n",
			dpcon_id);
		error = -ENOENT;
		goto out;
	}

	error = dpcon_destroy(&restool.mc_io, 0, dpcon_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpcon_opened = false;
	printf("dpcon.%u is destroyed\n", dpcon_id);

out:
	if (dpcon_opened) {
		error2 = dpcon_close(&restool.mc_io, 0, dpcon_handle);
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

static int destroy_dpcon_v10(uint32_t dpcon_id)
{
	int error;

	error = dpcon_destroy_v10(&restool.mc_io, restool.root_dprc_handle,
				  0, dpcon_id);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	printf("dpcon.%u is destroyed\n", dpcon_id);

out:
	return error;
}

static int destroy_dpcon(int mc_fw_version)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpcon destroy <dpcon-object>\n"
		"   e.g. restool dpcon destroy dpcon.9\n"
		"\n";

	int error;
	uint32_t dpcon_id;

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

	error = parse_object_name(restool.obj_name, "dpcon", &dpcon_id);
	if (error < 0)
		goto out;

	if (!find_obj("dpcon", dpcon_id)) {
		error = -EINVAL;
		goto out;
	}

	if (mc_fw_version == MC_FW_VERSION_8)
		error = destroy_dpcon_v8(dpcon_id);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = destroy_dpcon_v10(dpcon_id);
	else
		return -EINVAL;

out:
	return error;
}

static int cmd_dpcon_destroy(void)
{
	return destroy_dpcon(MC_FW_VERSION_8);
}

static int cmd_dpcon_destroy_v10(void)
{
	return destroy_dpcon(MC_FW_VERSION_10);
}

struct object_command dpcon_commands[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpcon_help },

	{ .cmd_name = "info",
	  .options = dpcon_info_options,
	  .cmd_func = cmd_dpcon_info },

	{ .cmd_name = "create",
	  .options = dpcon_create_options,
	  .cmd_func = cmd_dpcon_create },

	{ .cmd_name = "destroy",
	  .options = dpcon_destroy_options,
	  .cmd_func = cmd_dpcon_destroy },

	{ .cmd_name = NULL },
};

struct object_command dpcon_commands_v10[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpcon_help },

	{ .cmd_name = "info",
	  .options = dpcon_info_options,
	  .cmd_func = cmd_dpcon_info_v10 },

	{ .cmd_name = "create",
	  .options = dpcon_create_options,
	  .cmd_func = cmd_dpcon_create_v10 },

	{ .cmd_name = "destroy",
	  .options = dpcon_destroy_options,
	  .cmd_func = cmd_dpcon_destroy_v10 },

	{ .cmd_name = NULL },
};


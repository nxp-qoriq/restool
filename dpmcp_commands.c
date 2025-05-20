/* Copyright 2014-2016 Freescale Semiconductor Inc.
 * Copyright 2017-2022, 2025 NXP
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
#include "mc_v10/fsl_dpmcp.h"

#define ALL_DPMCP_OPTS (		\
	DPMCP_OPT_HIGH_PRIO_CMD_DIS)

static enum mc_cmd_status mc_status;

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
	CREATE_OPT_OPTIONS,
	CREATE_OPT_PARENT_DPRC,
};

static struct option dpmcp_create_options[] = {
	[CREATE_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_OPTIONS] = {
		.name = "options",
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

const struct flib_ops dpmcp_ops = {
	.obj_open = dpmcp_open,
	.obj_close = dpmcp_close,
	.obj_get_irq_mask = dpmcp_get_irq_mask,
	.obj_get_irq_status = dpmcp_get_irq_status,
};

static struct option_entry options_map[] = {
	OPTION_MAP_ENTRY(DPMCP_OPT_HIGH_PRIO_CMD_DIS),
};
static unsigned int options_num = ARRAY_SIZE(options_map);

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

static void print_dpmcp_options(uint32_t options)
{
	if ((options & ~ALL_DPMCP_OPTS) != 0) {
		printf("\tUnrecognized options found...\n");
		return;
	}

	if (options & DPMCP_OPT_HIGH_PRIO_CMD_DIS)
		printf("\tDPMCP_OPT_HIGH_PRIO_CMD_DIS\n");
}

static int print_dpmcp_attr(uint32_t dpmcp_id, struct dprc_obj_desc *target_obj_desc)
{
	uint16_t obj_major, obj_minor;
	struct dpmcp_attr dpmcp_attr;
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
	error = dpmcp_get_attributes(&restool.mc_io, 0, dpmcp_handle, &dpmcp_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpmcp_id == (uint32_t)dpmcp_attr.id);

	error = dpmcp_get_api_version(&restool.mc_io, 0, &obj_major, &obj_minor);
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
	printf("dpmcp_attr.options value is: %#lx\n",
			(unsigned long) dpmcp_attr.options);
	print_dpmcp_options(dpmcp_attr.options);

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

static int cmd_dpmcp_info(void)
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
	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	bool found = false;
	uint32_t dpmcp_id;
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

	error = parse_object_name(restool.obj_name, "dpmcp", &dpmcp_id);
	if (error < 0)
		goto out;

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

	error = print_dpmcp_attr(dpmcp_id, &target_obj_desc);
	if (error < 0)
		goto out;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		error = print_obj_verbose(&target_obj_desc, &dpmcp_ops);
	}

out:
	return error;
}

static int create_dpmcp(struct dpmcp_cfg *dpmcp_cfg)
{
	uint32_t dpmcp_id, dprc_id;
	uint64_t dpmcp_cfg_options;
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

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_OPTIONS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_OPTIONS);

		error = parse_generic_create_options(restool.cmd_option_args[CREATE_OPT_OPTIONS],
						     &dpmcp_cfg_options, options_map, options_num);

		if (error) {
			DEBUG_PRINTF("parse_generic_create_options() = %d\n",
				     error);
			return error;
		}

		dpmcp_cfg->options = (uint32_t)dpmcp_cfg_options;
		if (dpmcp_cfg_options > UINT32_MAX)
			DEBUG_PRINTF("parse_generic_create_options() produces overflow while getting options-mask\n");
	}

	error = dpmcp_create(&restool.mc_io, dprc_handle, 0, dpmcp_cfg, &dpmcp_id);
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

static int cmd_dpmcp_create(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpmcp create [OPTIONS]\n"
		"\n"
		"OPTIONS:\n"
		"--options=<option-mask>\n"
		"   Where <option-mask> is a comma or space separated list of DPMCP options:\n"
		"	DPMCP_OPT_HIGH_PRIO_CMD_DIS\n"
		"   If it is not specified, the new object will be created with no options.\n"
		"--container=<container-name>\n"
		"   Specifies the parent container name. e.g. dprc.2, dprc.3 etc.\n"
		"   If it is not specified, the new object will be created under the default dprc.\n"
		"\n";
	struct dpmcp_cfg dpmcp_cfg = {0};

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_HELP)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_HELP);
		return 0;
	}

	if (restool.obj_name != NULL) {
		ERROR_PRINTF("Unexpected argument: \'%s\'\n\n", restool.obj_name);
		puts(usage_msg);
		return -EINVAL;
	}

	dpmcp_cfg.portal_id = DPMCP_GET_PORTAL_ID_FROM_POOL;

	return create_dpmcp(&dpmcp_cfg);
}

static int destroy_dpmcp(uint32_t dpmcp_id)
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

	error = dpmcp_destroy(&restool.mc_io, dprc_handle, 0, dpmcp_id);
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

static int cmd_dpmcp_destroy(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpmcp destroy <dpmcp-object>\n"
		"   e.g. restool dpmcp destroy dpmcp.9\n"
		"\n";

	uint32_t dpmcp_id;
	int error;

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

	error = destroy_dpmcp(dpmcp_id);

out:
	return error;

}

struct object_command dpmcp_commands_v10[] = {
	{ .cmd_name = "--help",
	  .options = NULL,
	  .cmd_func = cmd_dpmcp_help },

	{ .cmd_name = "info",
	  .options = dpmcp_info_options,
	  .cmd_func = cmd_dpmcp_info },

	{ .cmd_name = "create",
	  .options = dpmcp_create_options,
	  .cmd_func = cmd_dpmcp_create },

	{ .cmd_name = "destroy",
	  .options = dpmcp_destroy_options,
	  .cmd_func = cmd_dpmcp_destroy },

	{ .cmd_name = NULL },
};

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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include "restool.h"
#include "utils.h"
#include "fsl_dpbp.h"

enum mc_cmd_status mc_status;

/**
 * dpbp info command options
 */
enum dpbp_info_options {
	INFO_OPT_HELP = 0,
	INFO_OPT_VERBOSE,
};

static struct option dpbp_info_options[] = {
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

C_ASSERT(ARRAY_SIZE(dpbp_info_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpbp create command options
 */
enum dpbp_create_options {
	CREATE_OPT_HELP = 0,
	CREATE_OPT_BUFFER_SIZE,
};

static struct option dpbp_create_options[] = {
	[CREATE_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_BUFFER_SIZE] = {
		.name = "buffer-size",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpbp_create_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpbp destroy command options
 */
enum dpbp_destroy_options {
	DESTROY_OPT_HELP = 0,
};

static struct option dpbp_destroy_options[] = {
	[DESTROY_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpbp_destroy_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static const struct flib_ops dpbp_ops = {
	.obj_open = dpbp_open,
	.obj_close = dpbp_close,
	.obj_get_irq_mask = dpbp_get_irq_mask,
	.obj_get_irq_status = dpbp_get_irq_status,
};

static int cmd_dpbp_help(void)
{
	static const char help_msg[] =
		"\n"
		"restool dpbp <command> [--help] [ARGS...]\n"
		"Where <command> can be:\n"
		"   info - displays detailed information about a DPBP object.\n"
		"   create - creates a new child DPBP under the root DPRC.\n"
		"   destroy - destroys a child DPBP under the root DPRC.\n"
		"\n"
		"For command-specific help, use the --help option of each command.\n"
		"\n";

	printf(help_msg);
	return 0;
}

static int print_dpbp_attr(uint32_t dpbp_id,
			struct dprc_obj_desc *target_obj_desc)
{
	uint16_t dpbp_handle;
	int error;
	struct dpbp_attr dpbp_attr;
	bool dpbp_opened = false;

	error = dpbp_open(&restool.mc_io, 0, dpbp_id, &dpbp_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpbp_opened = true;
	if (0 == dpbp_handle) {
		DEBUG_PRINTF(
			"dpbp_open() returned invalid handle (auth 0) for dpbp.%u\n",
			dpbp_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpbp_attr, 0, sizeof(dpbp_attr));
	error = dpbp_get_attributes(&restool.mc_io, 0, dpbp_handle, &dpbp_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpbp_id == (uint32_t)dpbp_attr.id);

	printf("dpbp version: %u.%u\n", dpbp_attr.version.major,
	       dpbp_attr.version.minor);
	printf("dpbp id: %d\n", dpbp_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	printf("buffer pool id: %u\n", (unsigned int)dpbp_attr.bpid);
	print_obj_label(target_obj_desc);

	error = 0;
out:
	if (dpbp_opened) {
		int error2;

		error2 = dpbp_close(&restool.mc_io, 0, dpbp_handle);
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

static int print_dpbp_info(uint32_t dpbp_id)
{
	int error;
	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	bool found = false;

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
				restool.root_dprc_handle, 0, dpbp_id,
				"dpbp", &target_obj_desc,
				&target_parent_dprc_id, &found);
	if (error < 0)
		goto out;

	if (strcmp(target_obj_desc.type, "dpbp")) {
		printf("dpbp.%d does not exist\n", dpbp_id);
		return -EINVAL;
	}

	error = print_dpbp_attr(dpbp_id, &target_obj_desc);
	if (error < 0)
		goto out;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		error = print_obj_verbose(&target_obj_desc, &dpbp_ops);
	}

out:
	return error;
}

static int cmd_dpbp_info(void)
{
	static const char usage_msg[] =
	"\n"
	"Usage: restool dpbp info <dpbp-object> [--verbose]\n"
	"   e.g. restool dpbp info dpbp.5\n"
	"\n"
	"--verbose\n"
	"   Shows extended/verbose information about the object\n"
	"   e.g. restool dpbp info dpbp.5 --verbose\n"
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

	error = parse_object_name(restool.obj_name, "dpbp", &obj_id);
	if (error < 0)
		goto out;

	error = print_dpbp_info(obj_id);
out:
	return error;
}

static int cmd_dpbp_create(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpbp create [OPTIONS]\n"
		"   e.g. create a DPBP object with all default options:\n"
		"	restool dpbp create\n"
		"\n"
		"OPTIONS:\n"
		"if options are not specified, create DPBP by default options\n"
		"--buffer-size=<size>\n"
		"   size should > 0\n"
		"   Default value is 0x200. i.e. 512\n"
		"   e.g. restool dpbp create --buffer-size=512\n"
		"\n";

	int error;
	long val;
	char *endptr;
	char *str;
	struct dpbp_cfg dpbp_cfg;
	uint16_t dpbp_handle;
	struct dpbp_attr dpbp_attr;

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

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_BUFFER_SIZE)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_BUFFER_SIZE);
		errno = 0;
		str = restool.cmd_option_args[CREATE_OPT_BUFFER_SIZE];
		val = strtol(str, &endptr, 0);

		if (STRTOL_ERROR(str, endptr, val, errno) || (val < 0)) {
			printf(usage_msg);
			return -EINVAL;
		}

		dpbp_cfg.options = val;
	} else {
		dpbp_cfg.options = 512;
	}

	error = dpbp_create(&restool.mc_io, 0, &dpbp_cfg, &dpbp_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	memset(&dpbp_attr, 0, sizeof(struct dpbp_attr));
	error = dpbp_get_attributes(&restool.mc_io, 0, dpbp_handle, &dpbp_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	print_new_obj("dpbp", dpbp_attr.id, NULL);

	error = dpbp_close(&restool.mc_io, 0, dpbp_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	return 0;
}

static int cmd_dpbp_destroy(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpbp destroy <dpbp-object>\n"
		"   e.g. restool dpbp destroy dpbp.9\n"
		"\n";

	int error;
	int error2;
	uint32_t dpbp_id;
	uint16_t dpbp_handle;
	bool dpbp_opened = false;

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

	error = parse_object_name(restool.obj_name, "dpbp", &dpbp_id);
	if (error < 0)
		goto out;

	if (!find_obj("dpbp", dpbp_id)) {
		error = -EINVAL;
		goto out;
	}

	error = dpbp_open(&restool.mc_io, 0, dpbp_id, &dpbp_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpbp_opened = true;
	if (0 == dpbp_handle) {
		DEBUG_PRINTF(
			"dpbp_open() returned invalid handle (auth 0) for dpbp.%u\n",
			dpbp_id);
		error = -ENOENT;
		goto out;
	}

	error = dpbp_destroy(&restool.mc_io, 0, dpbp_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpbp_opened = false;
	printf("dpbp.%u is destroyed\n", dpbp_id);

out:
	if (dpbp_opened) {
		error2 = dpbp_close(&restool.mc_io, 0, dpbp_handle);
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

struct object_command dpbp_commands[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpbp_help },

	{ .cmd_name = "info",
	  .options = dpbp_info_options,
	  .cmd_func = cmd_dpbp_info },

	{ .cmd_name = "create",
	  .options = dpbp_create_options,
	  .cmd_func = cmd_dpbp_create },

	{ .cmd_name = "destroy",
	  .options = dpbp_destroy_options,
	  .cmd_func = cmd_dpbp_destroy },

	{ .cmd_name = NULL },
};


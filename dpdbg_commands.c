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
#include "mc_v9/fsl_dpdbg.h"

enum mc_cmd_status mc_status;

/**
 * dpdbg info command options
 */
enum dpdbg_info_options {
	INFO_OPT_HELP = 0,
};

static struct option dpdbg_info_options[] = {
	[INFO_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpdbg_info_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static int cmd_dpdbg_help(void)
{
	static const char help_msg[] =
		"\n"
		"Usage: restool dpdbg <command> [--help] [ARGS...]\n"
		"Where <command> can be:\n"
		"   info - displays detailed information about a DPDBG object.\n"
		"\n"
		"For command-specific help, use the --help option of each command.\n"
		"\n";

	printf(help_msg);
	return 0;
}

static int print_dpdbg_attr(uint32_t dpdbg_id,
			struct dprc_obj_desc *target_obj_desc)
{
	uint16_t dpdbg_handle;
	int error;
	struct dpdbg_attr dpdbg_attr;
	bool dpdbg_opened = false;

	error = dpdbg_open(&restool.mc_io, 0, dpdbg_id, &dpdbg_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpdbg_opened = true;
	if (0 == dpdbg_handle) {
		DEBUG_PRINTF(
			"dpdbg_open() returned invalid handle (auth 0) for dpdbg.%u\n",
			dpdbg_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpdbg_attr, 0, sizeof(dpdbg_attr));
	error = dpdbg_get_attributes(&restool.mc_io, 0, dpdbg_handle,
				     &dpdbg_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpdbg_id == (uint32_t)dpdbg_attr.id);

	printf("dpdbg version: %u.%u\n", dpdbg_attr.version.major,
	       dpdbg_attr.version.minor);
	printf("dpdbg id: %d\n", dpdbg_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	print_obj_label(target_obj_desc);

	error = 0;
out:
	if (dpdbg_opened) {
		int error2;

		error2 = dpdbg_close(&restool.mc_io, 0, dpdbg_handle);
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

static int print_dpdbg_info(uint32_t dpdbg_id)
{
	int error;
	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	bool found = false;

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
				restool.root_dprc_handle, 0, dpdbg_id,
				"dpdbg", &target_obj_desc,
				&target_parent_dprc_id, &found);
	if (error < 0)
		goto out;

	if (strcmp(target_obj_desc.type, "dpdbg")) {
		printf("dpdbg.%d does not exist\n", dpdbg_id);
		return -EINVAL;
	}

	error = print_dpdbg_attr(dpdbg_id, &target_obj_desc);
out:
	return error;
}

static int cmd_dpdbg_info(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpdbg info <dpdbg-object>\n"
		"\n"
		"EXAMPLE:\n"
		"Display information about dpdbg.5:\n"
		"   $ restool dpdbg info dpdbg.5\n"
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

	error = parse_object_name(restool.obj_name, "dpdbg", &obj_id);
	if (error < 0)
		goto out;

	error = print_dpdbg_info(obj_id);
out:
	return error;
}

struct object_command dpdbg_commands[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpdbg_help },

	{ .cmd_name = "info",
	  .options = dpdbg_info_options,
	  .cmd_func = cmd_dpdbg_info },

	{ .cmd_name = NULL },
};


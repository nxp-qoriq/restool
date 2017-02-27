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
#include "mc_v9/fsl_dpci.h"
#include "mc_v10/fsl_dpci.h"

enum mc_cmd_status mc_status;

/**
 * dpci info command options
 */
enum dpci_info_options {
	INFO_OPT_HELP = 0,
	INFO_OPT_VERBOSE,
};

static struct option dpci_info_options[] = {
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

C_ASSERT(ARRAY_SIZE(dpci_info_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpci create command options
 */
enum dpci_create_options {
	CREATE_OPT_HELP = 0,
	CREATE_OPT_NUM_PRIORITIES,
	CREATE_OPT_PARENT_DPRC,
	CREATE_OPT_OPTIONS,
};

static struct option dpci_create_options[] = {
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

	[CREATE_OPT_PARENT_DPRC] = {
		.name = "container",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_OPTIONS] = {
		.name = "options",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpci_create_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpci destroy command options
 */
enum dpci_destroy_options {
	DESTROY_OPT_HELP = 0,
};

static struct option dpci_destroy_options[] = {
	[DESTROY_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpci_destroy_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static const struct flib_ops dpci_ops = {
	.obj_open = dpci_open,
	.obj_close = dpci_close,
	.obj_get_irq_mask = dpci_get_irq_mask,
	.obj_get_irq_status = dpci_get_irq_status,
};

static struct option_entry options_map_v10[] = {
	OPTION_MAP_ENTRY(DPCI_OPT_HAS_OPR),
	OPTION_MAP_ENTRY(DPCI_OPT_OPR_SHARED),
};
static unsigned int options_num_v10 = ARRAY_SIZE(options_map_v10);

static int cmd_dpci_help(void)
{
	static const char help_msg[] =
		"\n"
		"Usage: restool dpci <command> [--help] [ARGS...]\n"
		"Where <command> can be:\n"
		"   info - displays detailed information about a DPCI object.\n"
		"   create - creates a new child DPCI under the root DPRC.\n"
		"   destroy - destroys a child DPCI under the root DPRC.\n"
		"\n"
		"For command-specific help, use the --help option of each command.\n"
		"\n";

	printf(help_msg);
	return 0;
}

static int print_dpci_attr_v9(uint32_t dpci_id,
			      struct dprc_obj_desc *target_obj_desc)
{
	uint16_t dpci_handle;
	int error;
	struct dpci_attr dpci_attr;
	struct dpci_peer_attr dpci_peer_attr;
	bool dpci_opened = false;
	int link_state;

	error = dpci_open(&restool.mc_io, 0, dpci_id, &dpci_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpci_opened = true;
	if (0 == dpci_handle) {
		DEBUG_PRINTF(
			"dpci_open() returned invalid handle (auth 0) for dpci.%u\n",
			dpci_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpci_attr, 0, sizeof(dpci_attr));
	error = dpci_get_attributes(&restool.mc_io, 0, dpci_handle, &dpci_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpci_id == (uint32_t)dpci_attr.id);

	error = dpci_get_peer_attributes(&restool.mc_io, 0, dpci_handle,
					 &dpci_peer_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	error = dpci_get_link_state(&restool.mc_io, 0, dpci_handle,
					&link_state);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	printf("dpci version: %u.%u\n", dpci_attr.version.major,
	       dpci_attr.version.minor);
	printf("dpci id: %d\n", dpci_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	printf("num_of_priorities: %u\n",
	       (unsigned int)dpci_attr.num_of_priorities);
	printf("connected peer: ");
	if (-1 == dpci_peer_attr.peer_id) {
		printf("no peer\n");
	} else {
		printf("dpci.%d\n", dpci_peer_attr.peer_id);
		printf("peer's num_of_priorities: %u\n",
		       (unsigned int)dpci_peer_attr.num_of_priorities);
	}
	printf("link status: %d - ", link_state);
	link_state == 0 ? printf("down\n") :
	link_state == 1 ? printf("up\n") : printf("error state\n");
	print_obj_label(target_obj_desc);

	error = 0;

out:
	if (dpci_opened) {
		int error2;

		error2 = dpci_close(&restool.mc_io, 0, dpci_handle);
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

static int print_dpci_attr_v10(uint32_t dpci_id,
			       struct dprc_obj_desc *target_obj_desc)
{
	struct dpci_peer_attr dpci_peer_attr;
	struct dpci_attr_v10 dpci_attr;
	uint16_t obj_major, obj_minor;
	uint16_t dpci_handle;
	bool dpci_opened = false;
	int error, error2;
	int link_state;

	error = dpci_open(&restool.mc_io, 0, dpci_id, &dpci_handle);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	dpci_opened = true;
	if (!dpci_handle) {
		DEBUG_PRINTF(
			"dpci_open() returned invalid handle (auth 0) for dpci.%u\n",
			dpci_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpci_attr, 0, sizeof(dpci_attr));
	error = dpci_get_attributes_v10(&restool.mc_io, 0,
					dpci_handle, &dpci_attr);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpci_id == (uint32_t)dpci_attr.id);

	error = dpci_get_peer_attributes(&restool.mc_io, 0, dpci_handle,
					 &dpci_peer_attr);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	error = dpci_get_link_state(&restool.mc_io, 0, dpci_handle,
					&link_state);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	error = dpci_get_version_v10(&restool.mc_io, 0, &obj_major, &obj_minor);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	printf("dpci version: %u.%u\n", obj_major, obj_minor);
	printf("dpci id: %d\n", dpci_id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	printf("num_priorities: %u\n",
	       (unsigned int)dpci_attr.num_of_priorities);
	printf("connected peer: ");
	if (-1 == dpci_peer_attr.peer_id) {
		printf("no peer\n");
	} else {
		printf("dpci.%d\n", dpci_peer_attr.peer_id);
		printf("peer's num_of_priorities: %u\n",
		       (unsigned int)dpci_peer_attr.num_of_priorities);
	}
	printf("link status: %d - ", link_state);
	link_state == 0 ? printf("down\n") :
	link_state == 1 ? printf("up\n") : printf("error state\n");
	print_obj_label(target_obj_desc);

	error = 0;
out:
	if (dpci_opened) {
		error2 = dpci_close(&restool.mc_io, 0, dpci_handle);
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

static int print_dpci_info(uint32_t dpci_id, int mc_fw_version)
{
	int error;
	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	bool found = false;

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
				restool.root_dprc_handle, 0, dpci_id,
				"dpci", &target_obj_desc,
				&target_parent_dprc_id, &found);
	if (error < 0)
		goto out;

	if (strcmp(target_obj_desc.type, "dpci")) {
		printf("dpci.%d does not exist\n", dpci_id);
		return -EINVAL;
	}

	if (mc_fw_version == MC_FW_VERSION_9)
		error = print_dpci_attr_v9(dpci_id, &target_obj_desc);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = print_dpci_attr_v10(dpci_id, &target_obj_desc);
	if (error < 0)
		goto out;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		error = print_obj_verbose(&target_obj_desc, &dpci_ops);
	}

out:
	return error;
}

static int info_dpci(int mc_fw_version)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpci info <dpci-object> [--verbose]\n"
		"\n"
		"OPTIONS:\n"
		"--verbose\n"
		"   Shows extended/verbose information about the object\n"
		"\n"
		"EXAMPLE:\n"
		"Display information about dpci.5:\n"
		"   $ restool dpci info dpci.5\n"
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

	error = parse_object_name(restool.obj_name, "dpci", &obj_id);
	if (error < 0)
		goto out;

	error = print_dpci_info(obj_id, mc_fw_version);
out:
	return error;
}

static int cmd_dpci_info_v9(void)
{
	return info_dpci(MC_FW_VERSION_9);
}

static int cmd_dpci_info_v10(void)
{
	return info_dpci(MC_FW_VERSION_10);
}

static int create_dpci_v9(const char *usage_msg)
{
	struct dpci_attr dpci_attr;
	struct dpci_cfg dpci_cfg;
	uint16_t dpci_handle;
	int error;
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

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_NUM_PRIORITIES)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_NUM_PRIORITIES);
		error = get_option_value(CREATE_OPT_NUM_PRIORITIES, &val,
					 "Invalid value: num-priorities option",
					 1, 2);
		if (error)
			return -EINVAL;

		dpci_cfg.num_of_priorities = (uint8_t)val;
	} else {
		dpci_cfg.num_of_priorities = 1;
	}

	error = dpci_create(&restool.mc_io, 0, &dpci_cfg, &dpci_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	memset(&dpci_attr, 0, sizeof(struct dpci_attr));
	error = dpci_get_attributes(&restool.mc_io, 0, dpci_handle, &dpci_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	print_new_obj("dpci", dpci_attr.id, NULL);

	error = dpci_close(&restool.mc_io, 0, dpci_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	return 0;
}

static int create_dpci_v10(const char *usage_msg)
{
	struct dpci_cfg_v10 dpci_cfg;
	uint32_t dpci_id, dprc_id;
	uint16_t dprc_handle;
	bool dprc_opened;
	int error;
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

	if (restool.mc_fw_version.minor >= 1) {
		if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_OPTIONS)) {
			restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_OPTIONS);
			error = parse_generic_create_options(
					restool.cmd_option_args[CREATE_OPT_OPTIONS],
					(uint64_t *)&dpci_cfg.options,
					options_map_v10,
					options_num_v10);
			if (error < 0) {
				DEBUG_PRINTF(
					"parse_generic_create_options() failed with error %d, cannot get options-mask\n",
					error);
				return error;
			}
		}
	}
	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_NUM_PRIORITIES)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_NUM_PRIORITIES);
		error = get_option_value(CREATE_OPT_NUM_PRIORITIES, &val,
					 "Invalid value: num-priorities option",
					 1, 2);
		if (error)
			return -EINVAL;

		dpci_cfg.num_of_priorities = (uint8_t)val;
	} else {
		dpci_cfg.num_of_priorities = 1;
	}

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

	if (restool.mc_fw_version.minor == 0)
		error = dpci_create_v10_0(&restool.mc_io, dprc_handle, 0, &dpci_cfg, &dpci_id);
	else
		error = dpci_create_v10_1(&restool.mc_io, dprc_handle, 0, &dpci_cfg, &dpci_id);

	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	if (dprc_opened) {
		(void)dprc_close(&restool.mc_io, 0, dprc_handle);
		print_new_obj("dpci", dpci_id,
			      restool.cmd_option_args[CREATE_OPT_PARENT_DPRC]);
	} else {
		print_new_obj("dpci", dpci_id, NULL);

	}

	return 0;
}

static int cmd_dpci_create_v9(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpci create [OPTIONS]\n"
		"\n"
		"OPTIONS:\n"
		"if options are not specified, create DPCI by default options\n"
		"--num-priorities=<number>\n"
		"   specifies the number of priorities\n"
		"   valid values are 1-2\n"
		"   Default value is 1\n"
		"\n"
		"EXAMPLES:\n"
		"Create a DPCI object with all default options:\n"
		"   $ restool dpci create\n"
		"Create a DPCI object with 2 priorities:\n"
		"   $ restool dpci create --num-priorities=2\n"
		"\n";

	return create_dpci_v9(usage_msg);
}

static int cmd_dpci_create_v10(void)
{
	static const char usage_msg_0[] =
		"\n"
		"Usage: restool dpci create [OPTIONS]\n"
		"\n"
		"OPTIONS:\n"
		"if options are not specified, create DPCI by default options\n"
		"--num-priorities=<number>\n"
		"   specifies the number of priorities\n"
		"   valid values are 1-2\n"
		"   Default value is 1\n"
		"--container=<container-name>\n"
		"   Specifies the parent container name. e.g. dprc.2, dprc.3 etc.\n"
		"   If it is not specified, the new object will be created under the default dprc.\n"
		"\n"
		"EXAMPLES:\n"
		"Create a DPCI object with all default options:\n"
		"   $ restool dpci create\n"
		"Create a DPCI object with 2 priorities:\n"
		"   $ restool dpci create --num-priorities=2\n"
		"\n";

	static const char usage_msg_1[] =
		"\n"
		"Usage: restool dpci create [OPTIONS]\n"
		"\n"
		"OPTIONS:\n"
		"if options are not specified, create DPCI by default options\n"
		"--options=<options-mask>\n"
		"   Where <options-mask> is a comma or space separated list of DPCI options:\n"
		"	DPCI_OPT_HAS_OPR\n"
		"	DPCI_OPT_OPR_SHARED\n"
		"--num-priorities=<number>\n"
		"   specifies the number of priorities\n"
		"   valid values are 1-2\n"
		"   Default value is 1\n"
		"--container=<container-name>\n"
		"   Specifies the parent container name. e.g. dprc.2, dprc.3 etc.\n"
		"   If it is not specified, the new object will be created under the default dprc.\n"
		"\n"
		"EXAMPLES:\n"
		"Create a DPCI object with all default options:\n"
		"   $ restool dpci create\n"
		"Create a DPCI object with 2 priorities:\n"
		"   $ restool dpci create --num-priorities=2\n"
		"\n";

	if (restool.mc_fw_version.minor == 0)
		return create_dpci_v10(usage_msg_0);
	else
		return create_dpci_v10(usage_msg_1);

	return -EINVAL;
}

static int destroy_dpci_v9(uint32_t dpci_id)
{
	bool dpci_opened = false;
	uint16_t dpci_handle;
	int error, error2;

	error = dpci_open(&restool.mc_io, 0, dpci_id, &dpci_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpci_opened = true;
	if (0 == dpci_handle) {
		DEBUG_PRINTF(
			"dpci_open() returned invalid handle (auth 0) for dpci.%u\n",
			dpci_id);
		error = -ENOENT;
		goto out;
	}

	error = dpci_destroy(&restool.mc_io, 0, dpci_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpci_opened = false;
	printf("dpci.%u is destroyed\n", dpci_id);

out:
	if (dpci_opened) {
		error2 = dpci_close(&restool.mc_io, 0, dpci_handle);
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

static int destroy_dpci_v10(uint32_t dpci_id)
{
	uint16_t dprc_handle;
	uint32_t dprc_id;
	int error;

	dprc_handle = restool.root_dprc_handle;
	dprc_id = restool.root_dprc_id;
	error = get_parent_dprc_id(dpci_id, "dpci", &dprc_id);
	if (error)
		return error;

	if (dprc_id != restool.root_dprc_id) {
		error = open_dprc(dprc_id, &dprc_handle);
		if (error)
			return error;
	}

	error = dpci_destroy_v10(&restool.mc_io, dprc_handle,
				 0, dpci_id);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	printf("dpci.%u is destroyed\n", dpci_id);

out:
	if (dprc_id != restool.root_dprc_id)
		error = dprc_close(&restool.mc_io, 0, dprc_handle);

	return error;
}

static int destroy_dpci(int mc_fw_version)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpci destroy <dpci-object>\n"
		"   e.g. restool dpci destroy dpci.3\n"
		"\n";

	int error;
	uint32_t dpci_id;

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

	error = parse_object_name(restool.obj_name, "dpci", &dpci_id);
	if (error < 0)
		goto out;

	if (!find_obj("dpci", dpci_id)) {
		error = -EINVAL;
		goto out;
	}

	if (mc_fw_version == MC_FW_VERSION_9)
		error = destroy_dpci_v9(dpci_id);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = destroy_dpci_v10(dpci_id);
	else
		return -EINVAL;

out:
	return error;
}

static int cmd_dpci_destroy_v9(void)
{
	return destroy_dpci(MC_FW_VERSION_9);
}

static int cmd_dpci_destroy_v10(void)
{
	return destroy_dpci(MC_FW_VERSION_10);
}

struct object_command dpci_commands_v9[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpci_help },

	{ .cmd_name = "info",
	  .options = dpci_info_options,
	  .cmd_func = cmd_dpci_info_v9 },

	{ .cmd_name = "create",
	  .options = dpci_create_options,
	  .cmd_func = cmd_dpci_create_v9 },

	{ .cmd_name = "destroy",
	  .options = dpci_destroy_options,
	  .cmd_func = cmd_dpci_destroy_v9 },

	{ .cmd_name = NULL },
};

struct object_command dpci_commands_v10[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpci_help },

	{ .cmd_name = "info",
	  .options = dpci_info_options,
	  .cmd_func = cmd_dpci_info_v10 },

	{ .cmd_name = "create",
	  .options = dpci_create_options,
	  .cmd_func = cmd_dpci_create_v10 },

	{ .cmd_name = "destroy",
	  .options = dpci_destroy_options,
	  .cmd_func = cmd_dpci_destroy_v10 },

	{ .cmd_name = NULL },
};


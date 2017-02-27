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
#include "mc_v9/fsl_dpdmux.h"
#include "mc_v10/fsl_dpdmux.h"

#define ALL_DPDMUX_OPTS		DPDMUX_OPT_BRIDGE_EN

enum mc_cmd_status mc_status;

/**
 * dpdmux info command options
 */
enum dpdmux_info_options {
	INFO_OPT_HELP = 0,
	INFO_OPT_VERBOSE,
};

static struct option dpdmux_info_options[] = {
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

C_ASSERT(ARRAY_SIZE(dpdmux_info_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpdmux create command options
 */
enum dpdmux_create_options {
	CREATE_OPT_HELP = 0,
	CREATE_OPT_NUM_IFS,
	CREATE_OPT_CONTROL_IF,
	CREATE_OPT_METHOD,
	CREATE_OPT_MANIP,
	CREATE_OPT_OPTIONS,
	CREATE_OPT_MAX_DMAT_ENTRIES,
	CREATE_OPT_MAX_MC_GROUPS,
};

static struct option dpdmux_create_options[] = {
	[CREATE_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_NUM_IFS] = {
		.name = "num-ifs",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_CONTROL_IF] = {
		.name = "control-if",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_METHOD] = {
		.name = "method",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MANIP] = {
		.name = "manip",
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

	[CREATE_OPT_MAX_DMAT_ENTRIES] = {
		.name = "max-dmat-entries",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_MC_GROUPS] = {
		.name = "max-mc-groups",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpdmux_create_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpdmux create command options
 */
enum dpdmux_create_options_v9 {
	CREATE_OPT_HELP_V9 = 0,
	CREATE_OPT_NUM_IFS_V9,
	CREATE_OPT_METHOD_V9,
	CREATE_OPT_MANIP_V9,
	CREATE_OPT_OPTIONS_V9,
	CREATE_OPT_MAX_DMAT_ENTRIES_V9,
	CREATE_OPT_MAX_MC_GROUPS_V9,
	CREATE_OPT_PARENT_DPRC,
};

static struct option dpdmux_create_options_v9[] = {
	[CREATE_OPT_HELP_V9] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_NUM_IFS_V9] = {
		.name = "num-ifs",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_METHOD_V9] = {
		.name = "method",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MANIP_V9] = {
		.name = "manip",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_OPTIONS_V9] = {
		.name = "options",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_DMAT_ENTRIES_V9] = {
		.name = "max-dmat-entries",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_MC_GROUPS_V9] = {
		.name = "max-mc-groups",
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

C_ASSERT(ARRAY_SIZE(dpdmux_create_options_v9) <= MAX_NUM_CMD_LINE_OPTIONS + 1);


/**
 * dpdmux destroy command options
 */
enum dpdmux_destroy_options {
	DESTROY_OPT_HELP = 0,
};

static struct option dpdmux_destroy_options[] = {
	[DESTROY_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpdmux_destroy_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static struct option_entry options_map[] = {
	OPTION_MAP_ENTRY(DPDMUX_OPT_BRIDGE_EN),
};
static unsigned options_num = ARRAY_SIZE(options_map);

static const struct flib_ops dpdmux_ops = {
	.obj_open = dpdmux_open,
	.obj_close = dpdmux_close,
	.obj_get_irq_mask = dpdmux_get_irq_mask,
	.obj_get_irq_status = dpdmux_get_irq_status,
};

static const struct flib_ops dpdmux_ops_v9 = {
	.obj_open = dpdmux_open,
	.obj_close = dpdmux_close,
	.obj_get_irq_mask = dpdmux_get_irq_mask,
	.obj_get_irq_status = dpdmux_get_irq_status_v9,
};

static int cmd_dpdmux_help(void)
{
	static const char help_msg[] =
		"\n"
		"Usage: restool dpdmux <command> [--help] [ARGS...]\n"
		"Where <command> can be:\n"
		"   info - displays detailed information about a DPDMUX object.\n"
		"   create - creates a new child DPDMUX under the root DPRC.\n"
		"   destroy - destroys a child DPDMUX under the root DPRC.\n"
		"\n"
		"For command-specific help, use the --help option of each command.\n"
		"\n";

	printf(help_msg);
	return 0;
}

static int print_dpdmux_endpoint(uint32_t target_id, uint16_t num_ifs)
{
	struct dprc_endpoint endpoint1;
	struct dprc_endpoint endpoint2;
	int state;
	int error = 0;
	int k;

	printf("endpoints:\n");
	for (k = 0; k < num_ifs; ++k) {
		memset(&endpoint1, 0, sizeof(struct dprc_endpoint));
		memset(&endpoint2, 0, sizeof(struct dprc_endpoint));
		strncpy(endpoint1.type, "dpdmux", EP_OBJ_TYPE_MAX_LEN);
		endpoint1.type[EP_OBJ_TYPE_MAX_LEN] = '\0';
		endpoint1.id = target_id;
		endpoint1.if_id = k;

		error = dprc_get_connection(&restool.mc_io, 0,
					restool.root_dprc_handle,
					&endpoint1,
					&endpoint2,
					&state);
		printf("interface %d:\n", k);
		if (error == 0 && state == -1) {
			printf("\tconnection: none\n");
			printf("\tlink state: n/a\n");
		} else if (error == 0) {
			if (strcmp(endpoint2.type, "dpsw") == 0 ||
			    strcmp(endpoint2.type, "dpdmux") == 0) {
				printf("\tconnection: %s.%d.%d\n",
					endpoint2.type, endpoint2.id,
					endpoint2.if_id);
			} else if (endpoint2.if_id == 0) {
				printf("\tconnection: %s.%d\n",
					endpoint2.type, endpoint2.id);
			}

			if (state == 1)
				printf("\tlink state: up\n");
			else if (state == 0)
				printf("\tlink state: down\n");
			else
				printf("\tlink state: error\n");
		} else {
			mc_status = flib_error_to_mc_status(error);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status), mc_status);
		}

	}

	return 0;
}

static void print_dpdmux_options(uint64_t options)
{
	if ((options & ~ALL_DPDMUX_OPTS) != 0) {
		printf("\tUnrecognized options found...\n");
		return;
	}

	if (options & DPDMUX_OPT_BRIDGE_EN)
		printf("\tDPDMUX_OPT_BRIDGE_EN\n");
}

static void print_dpdmux_method(enum dpdmux_method method)
{
	printf("DPDMUX address table method: ");
	switch (method) {
	case DPDMUX_METHOD_NONE:
		printf("DPDMUX_METHOD_NONE\n");
		break;
	case DPDMUX_METHOD_C_VLAN_MAC:
		printf("DPDMUX_METHOD_C_VLAN_MAC\n");
		break;
	case DPDMUX_METHOD_MAC:
		printf("DPDMUX_METHOD_MAC\n");
		break;
	case DPDMUX_METHOD_C_VLAN:
		printf("DPDMUX_METHOD_C_VLAN\n");
		break;
	default:
		assert(false);
		break;
	}
}

static void print_dpdmux_manip(enum dpdmux_manip manip)
{
	printf("DPDMUX manipulation type: ");
	switch (manip) {
	case DPDMUX_MANIP_NONE:
		printf("DPDMUX_MANIP_NONE\n");
		break;
	default:
		assert(false);
		break;
	}
}

static int print_dpdmux_attr_v9(uint32_t dpdmux_id,
				struct dprc_obj_desc *target_obj_desc)
{
	uint16_t dpdmux_handle;
	int error;
	struct dpdmux_attr_v9 dpdmux_attr;
	bool dpdmux_opened = false;

	error = dpdmux_open(&restool.mc_io, 0, dpdmux_id, &dpdmux_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpdmux_opened = true;
	if (0 == dpdmux_handle) {
		DEBUG_PRINTF(
			"dpdmux_open() returned invalid handle (auth 0) for dpdmux.%u\n",
			dpdmux_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpdmux_attr, 0, sizeof(dpdmux_attr));
	error = dpdmux_get_attributes_v9(&restool.mc_io, 0, dpdmux_handle,
					&dpdmux_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpdmux_id == (uint32_t)dpdmux_attr.id);

	printf("dpdmux version: %u.%u\n", dpdmux_attr.version.major,
	       dpdmux_attr.version.minor);
	printf("dpdmux id: %d\n", dpdmux_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	print_dpdmux_endpoint(dpdmux_id, dpdmux_attr.num_ifs + 1);
	printf("dpdmux_attr.options value is: %#llx\n",
	       (unsigned long long)dpdmux_attr.options);
	print_dpdmux_options(dpdmux_attr.options);
	print_dpdmux_method(dpdmux_attr.method);
	print_dpdmux_manip(dpdmux_attr.manip);
	printf("number of interfaces (excluding the uplink interface): %u\n",
		(uint32_t)dpdmux_attr.num_ifs);
	printf("frame storage memory size: %u\n",
		(uint32_t)dpdmux_attr.mem_size);
	print_obj_label(target_obj_desc);

	error = 0;

out:
	if (dpdmux_opened) {
		int error2;

		error2 = dpdmux_close(&restool.mc_io, 0, dpdmux_handle);
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

static int print_dpdmux_attr_v10(uint32_t dpdmux_id,
				 struct dprc_obj_desc *target_obj_desc)
{
	struct dpdmux_attr_v10 dpdmux_attr;
	uint16_t obj_major, obj_minor;
	bool dpdmux_opened = false;
	uint16_t dpdmux_handle;
	int error;

	error = dpdmux_open(&restool.mc_io, 0, dpdmux_id, &dpdmux_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpdmux_opened = true;
	if (0 == dpdmux_handle) {
		DEBUG_PRINTF(
			"dpdmux_open() returned invalid handle (auth 0) for dpdmux.%u\n",
			dpdmux_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpdmux_attr, 0, sizeof(dpdmux_attr));
	error = dpdmux_get_attributes_v10(&restool.mc_io, 0, dpdmux_handle,
					  &dpdmux_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpdmux_id == (uint32_t)dpdmux_attr.id);

	error = dpdmux_get_version_v10(&restool.mc_io, 0,
				       &obj_major, &obj_minor);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	printf("dpdmux version: %u.%u\n", obj_major, obj_minor);
	printf("dpdmux id: %d\n", dpdmux_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	print_dpdmux_endpoint(dpdmux_id, dpdmux_attr.num_ifs + 1);
	printf("dpdmux_attr.options value is: %#llx\n",
	       (unsigned long long)dpdmux_attr.options);
	print_dpdmux_options(dpdmux_attr.options);
	print_dpdmux_method(dpdmux_attr.method);
	print_dpdmux_manip(dpdmux_attr.manip);
	printf("number of interfaces (excluding the uplink interface): %u\n",
		(uint32_t)dpdmux_attr.num_ifs);
	printf("frame storage memory size: %u\n",
		(uint32_t)dpdmux_attr.mem_size);
	print_obj_label(target_obj_desc);

	error = 0;

out:
	if (dpdmux_opened) {
		int error2;

		error2 = dpdmux_close(&restool.mc_io, 0, dpdmux_handle);
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


static int print_dpdmux_info(uint32_t dpdmux_id, int mc_fw_version)
{
	int error;
	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	bool found = false;

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
				restool.root_dprc_handle, 0, dpdmux_id,
				"dpdmux", &target_obj_desc,
				&target_parent_dprc_id, &found);
	if (error < 0)
		goto out;

	if (strcmp(target_obj_desc.type, "dpdmux")) {
		printf("dpdmux.%d does not exist\n", dpdmux_id);
		return -EINVAL;
	}

	if (mc_fw_version == MC_FW_VERSION_9)
		error = print_dpdmux_attr_v9(dpdmux_id, &target_obj_desc);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = print_dpdmux_attr_v10(dpdmux_id, &target_obj_desc);
	if (error < 0)
		goto out;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		error = print_obj_verbose(&target_obj_desc, &dpdmux_ops_v9);
	}

out:
	return error;
}

static int info_dpdmux(int mc_fw_version)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpdmux info <dpdmux-object> [--verbose]\n"
		"\n"
		"OPTIONS:\n"
		"--verbose\n"
		"   Shows extended/verbose information about the object\n"
		"\n"
		"EXAMPLE:\n"
		"Display information about dpdmux.5:\n"
		"   $ restool dpdmux info dpdmux.5\n"
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

	error = parse_object_name(restool.obj_name, "dpdmux", &obj_id);
	if (error < 0)
		goto out;

	error = print_dpdmux_info(obj_id, mc_fw_version);

out:
	return error;
}

static int cmd_dpdmux_info_v9(void)
{
	return info_dpdmux(MC_FW_VERSION_9);
}

static int cmd_dpdmux_info_v10(void)
{
	return info_dpdmux(MC_FW_VERSION_10);
}

static int parse_dpdmux_manip(char *manip_str, enum dpdmux_manip *manip)
{
	if (strcmp(manip_str, "DPDMUX_MANIP_NONE") == 0) {
		*manip = DPDMUX_MANIP_NONE;
		return 0;
	}

/* TODO: Enable DPDMUX_MANIP_ADD_REMOVE_S_VLAN when MC support added */

	printf("Invalid dpdmux manip input.\n");
	return -EINVAL;
}


static int parse_dpdmux_method(char *method_str, enum dpdmux_method *method)
{
	if (strcmp(method_str, "DPDMUX_METHOD_NONE") == 0) {
		*method = DPDMUX_METHOD_NONE;
		return 0;
	}

	if (strcmp(method_str, "DPDMUX_METHOD_C_VLAN_MAC") == 0) {
		*method = DPDMUX_METHOD_C_VLAN_MAC;
		return 0;
	}

	if (strcmp(method_str, "DPDMUX_METHOD_MAC") == 0) {
		*method = DPDMUX_METHOD_MAC;
		return 0;
	}

	if (strcmp(method_str, "DPDMUX_METHOD_C_VLAN") == 0) {
		*method = DPDMUX_METHOD_C_VLAN;
		return 0;
	}

/* TODO: Enable DPDMUX_METHOD_S_VLAN when MC support added */

	printf("Invalid dpdmux method input.\n");
	return -EINVAL;
}

/**
 * Dpdmux create commands
 * Create commands for mc version 9
 */
static int create_dpdmux_v9(const char *usage_msg)
{
	int error;
	struct dpdmux_cfg_v9 dpdmux_cfg = {0};
	uint16_t dpdmux_handle;
	long val;
	struct dpdmux_attr_v9 dpdmux_attr;

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_HELP_V9)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_HELP_V9);
		return 0;
	}

	if (restool.obj_name != NULL) {
		ERROR_PRINTF("Unexpected argument: \'%s\'\n\n",
			     restool.obj_name);
		puts(usage_msg);
		return -EINVAL;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_OPTIONS_V9)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_OPTIONS_V9);
		error = parse_generic_create_options(
				restool.cmd_option_args[CREATE_OPT_OPTIONS_V9],
				&dpdmux_cfg.adv.options,
				options_map,
				options_num);
		if (error < 0) {
			DEBUG_PRINTF(
				"parse_generic_create_options() failed with error %d, cannot get options-mask\n",
				error);
			return error;
		}
	} else {
		dpdmux_cfg.adv.options = 0;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_METHOD_V9)) {
		restool.cmd_option_mask &=
				~ONE_BIT_MASK(CREATE_OPT_METHOD_V9);
		error = parse_dpdmux_method(
			restool.cmd_option_args[CREATE_OPT_METHOD_V9],
			&dpdmux_cfg.method);
		if (error < 0) {
			DEBUG_PRINTF(
				"parse_dpdmux_method() failed with error %d, cannot get dpdmux method\n",
				error);
			return error;
		}
	} else {
		dpdmux_cfg.method = DPDMUX_METHOD_C_VLAN_MAC;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_MANIP_V9)) {
		restool.cmd_option_mask &=
				~ONE_BIT_MASK(CREATE_OPT_MANIP_V9);
		error = parse_dpdmux_manip(
				restool.cmd_option_args[CREATE_OPT_MANIP_V9],
				&dpdmux_cfg.manip);
		if (error < 0) {
			DEBUG_PRINTF(
				"parse_dpdmux_manip() failed with error %d, cannot get dpdmux manip\n",
				error);
			return error;
		}
	} else {
		dpdmux_cfg.manip = DPDMUX_MANIP_NONE;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_NUM_IFS_V9)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_NUM_IFS_V9);
		error = get_option_value(CREATE_OPT_NUM_IFS_V9, &val,
					 "Invalid number of interfaces",
					 0, UINT16_MAX);
		if (error)
			return -EINVAL;
		dpdmux_cfg.num_ifs = (uint16_t)val;
	} else {
		ERROR_PRINTF("--num-ifs option missing\n");
		puts(usage_msg);
		return -EINVAL;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_DMAT_ENTRIES_V9)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_DMAT_ENTRIES_V9);
		error = get_option_value(CREATE_OPT_MAX_DMAT_ENTRIES_V9, &val,
					 "Invalid max DPDMUX address table",
					 0, UINT16_MAX);
		if (error)
			return -EINVAL;
		dpdmux_cfg.adv.max_dmat_entries = (uint16_t)val;
	} else {
		dpdmux_cfg.adv.max_dmat_entries = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_MC_GROUPS_V9)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_MC_GROUPS_V9);
		error = get_option_value(CREATE_OPT_MAX_MC_GROUPS_V9, &val,
					 "Invalid max multicast group",
					 0, UINT16_MAX);
		if (error)
			return -EINVAL;
		dpdmux_cfg.adv.max_mc_groups = (uint16_t)val;
	} else {
		dpdmux_cfg.adv.max_mc_groups = 0;
	}

	error = dpdmux_create_v9(&restool.mc_io, 0, &dpdmux_cfg,
				 &dpdmux_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	memset(&dpdmux_attr, 0, sizeof(struct dpdmux_attr_v9));
	error = dpdmux_get_attributes_v9(&restool.mc_io, 0, dpdmux_handle,
					&dpdmux_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	print_new_obj("dpdmux", dpdmux_attr.id, NULL);

	error = dpdmux_close(&restool.mc_io, 0, dpdmux_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	return 0;
}

static int cmd_dpdmux_create_v9(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpdmux create --num-ifs=<number> [OPTIONS]\n"
		"   --num-ifs=<number>\n"
		"      Number of virtual interfaces (excluding the uplink interface)\n"
		"\n"
		"OPTIONS:\n"
		"--method=<dmat-method>\n"
		"   Where <dmat-method> defines the method of the DPDMUX address table.\n"
		"   A valid value is one of the following:\n"
		"	DPDMUX_METHOD_NONE\n"
		"	DPDMUX_METHOD_C_VLAN_MAC\n"
		"	DPDMUX_METHOD_MAC\n"
		"	DPDMUX_METHOD_C_VLAN\n"
		"   Default is DPDMUX_METHOD_C_VLAN_MAC\n"
		"--manip=<manip>\n"
		"   Where <manip> defines the DPDMUX required manipulation operation.\n"
		"   A valid value is one of the following:\n"
		"	DPDMUX_MANIP_NONE\n"
		"   Default is DPDMUX_MANIP_NONE\n"
		"--options=<options-mask>\n"
		"   Where <options-mask> is a comma separated list of DPDMUX options:\n"
		"	DPDMUX_OPT_BRIDGE_EN\n"
		"   Default is 0\n"
		"--max-dmat-entries=<number>\n"
		"   Maximum entries in DPDMUX address table. Default is 64.\n"
		"--max-mc-groups=<number>\n"
		"   Number of multicast groups in DPDMUX address table. Default is 32 groups.\n"
		"\n";

	return create_dpdmux_v9(usage_msg);
}

static int create_dpdmux_v10(const char *usage_msg)
{
	struct dpdmux_cfg_v9 dpdmux_cfg = {0};
	uint32_t dpdmux_id, dprc_id;
	uint16_t dprc_handle;
	bool dprc_opened;
	int error;
	long val;

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_HELP_V9)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_HELP_V9);
		return 0;
	}

	if (restool.obj_name != NULL) {
		ERROR_PRINTF("Unexpected argument: \'%s\'\n\n",
			     restool.obj_name);
		puts(usage_msg);
		return -EINVAL;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_OPTIONS_V9)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_OPTIONS_V9);
		error = parse_generic_create_options(
				restool.cmd_option_args[CREATE_OPT_OPTIONS_V9],
				&dpdmux_cfg.adv.options,
				options_map,
				options_num);
		if (error < 0) {
			DEBUG_PRINTF(
				"parse_generic_create_options() failed with error %d, cannot get options-mask\n",
				error);
			return error;
		}
	} else {
		dpdmux_cfg.adv.options = 0;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_METHOD_V9)) {
		restool.cmd_option_mask &=
				~ONE_BIT_MASK(CREATE_OPT_METHOD_V9);
		error = parse_dpdmux_method(
			restool.cmd_option_args[CREATE_OPT_METHOD_V9],
			&dpdmux_cfg.method);
		if (error < 0) {
			DEBUG_PRINTF(
				"parse_dpdmux_method() failed with error %d, cannot get dpdmux method\n",
				error);
			return error;
		}
	} else {
		dpdmux_cfg.method = DPDMUX_METHOD_C_VLAN_MAC;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_MANIP_V9)) {
		restool.cmd_option_mask &=
				~ONE_BIT_MASK(CREATE_OPT_MANIP_V9);
		error = parse_dpdmux_manip(
				restool.cmd_option_args[CREATE_OPT_MANIP_V9],
				&dpdmux_cfg.manip);
		if (error < 0) {
			DEBUG_PRINTF(
				"parse_dpdmux_manip() failed with error %d, cannot get dpdmux manip\n",
				error);
			return error;
		}
	} else {
		dpdmux_cfg.manip = DPDMUX_MANIP_NONE;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_NUM_IFS_V9)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_NUM_IFS_V9);
		error = get_option_value(CREATE_OPT_NUM_IFS_V9, &val,
				     "Invalid number of interfaces\n",
				     1, UINT16_MAX);
		if (error)
			return error;
		dpdmux_cfg.num_ifs = (uint16_t)val;
	} else {
		ERROR_PRINTF("--num-ifs option missing\n");
		puts(usage_msg);
		return -EINVAL;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_DMAT_ENTRIES_V9)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_DMAT_ENTRIES_V9);
		error = get_option_value(CREATE_OPT_MAX_DMAT_ENTRIES_V9, &val,
				     "Invalid max DPDMUX address table\n",
				     1, UINT16_MAX);
		if (error)
			return error;
		dpdmux_cfg.adv.max_dmat_entries = (uint16_t)val;
	} else {
		dpdmux_cfg.adv.max_dmat_entries = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_MC_GROUPS_V9)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_MC_GROUPS_V9);
		error = get_option_value(CREATE_OPT_MAX_MC_GROUPS_V9, &val,
				     "Invalid max multicast group\n",
				     1, UINT16_MAX);
		if (error)
			return error;
		dpdmux_cfg.adv.max_mc_groups = (uint16_t)val;
	} else {
		dpdmux_cfg.adv.max_mc_groups = 0;
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

	error = dpdmux_create_v10(&restool.mc_io, dprc_handle, 0,
				  &dpdmux_cfg, &dpdmux_id);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	if (dprc_opened) {
		(void)dprc_close(&restool.mc_io, 0, dprc_handle);
		print_new_obj("dpdmux", dpdmux_id,
			      restool.cmd_option_args[CREATE_OPT_PARENT_DPRC]);
	} else {
		print_new_obj("dpdmux", dpdmux_id, NULL);
	}

	return 0;
}


static int cmd_dpdmux_create_v10(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpdmux create --num-ifs=<number> [OPTIONS]\n"
		"   --num-ifs=<number>\n"
		"      Number of virtual interfaces (excluding the uplink interface)\n"
		"\n"
		"OPTIONS:\n"
		"--method=<dmat-method>\n"
		"   Where <dmat-method> defines the method of the DPDMUX address table.\n"
		"   A valid value is one of the following:\n"
		"	DPDMUX_METHOD_NONE\n"
		"	DPDMUX_METHOD_C_VLAN_MAC\n"
		"	DPDMUX_METHOD_MAC\n"
		"	DPDMUX_METHOD_C_VLAN\n"
		"   Default is DPDMUX_METHOD_C_VLAN_MAC\n"
		"--manip=<manip>\n"
		"   Where <manip> defines the DPDMUX required manipulation operation.\n"
		"   A valid value is one of the following:\n"
		"	DPDMUX_MANIP_NONE\n"
		"   Default is DPDMUX_MANIP_NONE\n"
		"--options=<options-mask>\n"
		"   Where <options-mask> is a comma separated list of DPDMUX options:\n"
		"	DPDMUX_OPT_BRIDGE_EN\n"
		"   Default is 0\n"
		"--max-dmat-entries=<number>\n"
		"   Maximum entries in DPDMUX address table. Default is 64.\n"
		"--max-mc-groups=<number>\n"
		"   Number of multicast groups in DPDMUX address table. Default is 32 groups.\n"
		"--container=<container-name>\n"
		"   Specifies the parent container name. e.g. dprc.2, dprc.3 etc.\n"
		"   If it is not specified, the new object will be created under the default dprc.\n"
		"\n";

	return create_dpdmux_v10(usage_msg);
}

static int destroy_dpdmux_v9(uint32_t dpdmux_id)
{
	bool dpdmux_opened = false;
	uint16_t dpdmux_handle;
	int error, error2;

	error = dpdmux_open(&restool.mc_io, 0, dpdmux_id, &dpdmux_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpdmux_opened = true;
	if (0 == dpdmux_handle) {
		DEBUG_PRINTF(
			"dpdmux_open() returned invalid handle (auth 0) for dpdmux.%u\n",
			dpdmux_id);
		error = -ENOENT;
		goto out;
	}

	error = dpdmux_destroy(&restool.mc_io, 0, dpdmux_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpdmux_opened = false;
	printf("dpdmux.%u is destroyed\n", dpdmux_id);

out:
	if (dpdmux_opened) {
		error2 = dpdmux_close(&restool.mc_io, 0, dpdmux_handle);
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

static int destroy_dpdmux_v10(uint32_t dpdmux_id)
{
	uint16_t dprc_handle;
	uint32_t dprc_id;
	int error;

	dprc_handle = restool.root_dprc_handle;
	dprc_id = restool.root_dprc_id;
	error = get_parent_dprc_id(dpdmux_id, "dpdmux", &dprc_id);
	if (error)
		return error;

	if (dprc_id != restool.root_dprc_id) {
		error = open_dprc(dprc_id, &dprc_handle);
		if (error)
			return error;
	}

	error = dpdmux_destroy_v10(&restool.mc_io, dprc_handle,
				   0, dpdmux_id);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	printf("dpdmux.%u is destroyed\n", dpdmux_id);

out:
	if (dprc_id != restool.root_dprc_id)
		error = dprc_close(&restool.mc_io, 0, dprc_handle);

	return error;
}

static int destroy_dpdmux(int mc_fw_version)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpdmux destroy <dpdmux-object>\n"
		"   e.g. restool dpdmux destroy dpdmux.9\n"
		"\n";

	int error;
	uint32_t dpdmux_id;

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

	error = parse_object_name(restool.obj_name, "dpdmux", &dpdmux_id);
	if (error < 0)
		goto out;

	if (!find_obj("dpdmux", dpdmux_id)) {
		error = -EINVAL;
		goto out;
	}

	if (mc_fw_version == MC_FW_VERSION_9)
		error = destroy_dpdmux_v9(dpdmux_id);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = destroy_dpdmux_v10(dpdmux_id);
	else
		return -EINVAL;

out:
	return error;
}

static int cmd_dpdmux_destroy_v9(void)
{
	return destroy_dpdmux(MC_FW_VERSION_9);
}

static int cmd_dpdmux_destroy_v10(void)
{
	return destroy_dpdmux(MC_FW_VERSION_10);
}

struct object_command dpdmux_commands_v9[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpdmux_help },

	{ .cmd_name = "info",
	  .options = dpdmux_info_options,
	  .cmd_func = cmd_dpdmux_info_v9 },

	{ .cmd_name = "create",
	  .options = dpdmux_create_options_v9,
	  .cmd_func = cmd_dpdmux_create_v9 },

	{ .cmd_name = "destroy",
	  .options = dpdmux_destroy_options,
	  .cmd_func = cmd_dpdmux_destroy_v9 },

	{ .cmd_name = NULL },
};

struct object_command dpdmux_commands_v10[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpdmux_help },

	{ .cmd_name = "info",
	  .options = dpdmux_info_options,
	  .cmd_func = cmd_dpdmux_info_v10 },

	{ .cmd_name = "create",
	  .options = dpdmux_create_options_v9,
	  .cmd_func = cmd_dpdmux_create_v10 },

	{ .cmd_name = "destroy",
	  .options = dpdmux_destroy_options,
	  .cmd_func = cmd_dpdmux_destroy_v10 },

	{ .cmd_name = NULL },
};


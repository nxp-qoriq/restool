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
#include "mc_v9/fsl_dpmac.h"
#include "mc_v10/fsl_dpmac.h"

enum mc_cmd_status mc_status;

/**
 * dpmac info command options
 */
enum dpmac_info_options {
	INFO_OPT_HELP = 0,
	INFO_OPT_VERBOSE,
};

static struct option dpmac_info_options[] = {
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

C_ASSERT(ARRAY_SIZE(dpmac_info_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpmac create command options
 */
enum dpmac_create_options {
	CREATE_OPT_HELP = 0,
	CREATE_OPT_MAC_ID,
	CREATE_OPT_PARENT_DPRC,
};

static struct option dpmac_create_options[] = {
	[CREATE_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAC_ID] = {
		.name = "mac-id",
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

C_ASSERT(ARRAY_SIZE(dpmac_create_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpmac destroy command options
 */
enum dpmac_destroy_options {
	DESTROY_OPT_HELP = 0,
};

static struct option dpmac_destroy_options[] = {
	[DESTROY_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpmac_destroy_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static const struct flib_ops dpmac_ops = {
	.obj_open = dpmac_open,
	.obj_close = dpmac_close,
	.obj_get_irq_mask = dpmac_get_irq_mask,
	.obj_get_irq_status = dpmac_get_irq_status,
};

static int cmd_dpmac_help(void)
{
	static const char help_msg[] =
		"\n"
		"Usage: restool dpmac <command> [--help] [ARGS...]\n"
		"Where <command> can be:\n"
		"   info - displays detailed information about a DPMAC object.\n"
		"   create - creates a new child DPMAC under the root DPRC.\n"
		"   destroy - destroys a child DPMAC under the root DPRC.\n"
		"\n"
		"For command-specific help, use the --help option of each command.\n"
		"\n";

	printf(help_msg);
	return 0;
}

static int print_dpmac_endpoint(uint32_t target_id)
{
	struct dprc_endpoint endpoint1;
	struct dprc_endpoint endpoint2;
	int state;
	int error = 0;

	memset(&endpoint1, 0, sizeof(struct dprc_endpoint));
	memset(&endpoint2, 0, sizeof(struct dprc_endpoint));

	strncpy(endpoint1.type, "dpmac", EP_OBJ_TYPE_MAX_LEN);
	endpoint1.type[EP_OBJ_TYPE_MAX_LEN] = '\0';
	endpoint1.id = target_id;
	endpoint1.if_id = 0;

	error = dprc_get_connection(&restool.mc_io, 0,
					restool.root_dprc_handle,
					&endpoint1, &endpoint2, &state);
	printf("endpoint state: %d\n", state);

	if (error == 0 && state == -1) {
		printf("endpoint: No object associated\n");
	} else if (error == 0) {
		if (strcmp(endpoint2.type, "dpsw") == 0 ||
		    strcmp(endpoint2.type, "dpdmux") == 0) {
			printf("endpoint: %s.%d.%d",
				endpoint2.type, endpoint2.id,
				endpoint2.if_id);
		} else if (endpoint2.if_id == 0) {
			printf("endpoint: %s.%d",
				endpoint2.type, endpoint2.id);
		}

		if (state == 1)
			printf(", link is up\n");
		else if (state == 0)
			printf(", link is down\n");
		else
			printf(", link is in error state\n");

	} else {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			mc_status_to_string(mc_status), mc_status);
		return error;
	}

	return 0;
}

static void print_dpmac_link_type(enum dpmac_link_type link_type)
{
	printf("DPMAC link type: ");
	switch (link_type) {
	case DPMAC_LINK_TYPE_NONE:
		printf("DPMAC_LINK_TYPE_NONE\n");
		break;
	case DPMAC_LINK_TYPE_FIXED:
		printf("DPMAC_LINK_TYPE_FIXED\n");
		break;
	case DPMAC_LINK_TYPE_PHY:
		printf("DPMAC_LINK_TYPE_PHY\n");
		break;
	case DPMAC_LINK_TYPE_BACKPLANE:
		printf("DPMAC_LINK_TYPE_BACKPLANE\n");
		break;
	default:
		assert(false);
		break;
	}
}

static void print_dpmac_eth_if(enum dpmac_eth_if eth_if)
{
	printf("DPMAC ethernet interface: ");
	switch (eth_if) {
	case DPMAC_ETH_IF_MII:
		printf("DPMAC_ETH_IF_MII\n");
		break;
	case DPMAC_ETH_IF_RMII:
		printf("DPMAC_ETH_IF_RMII\n");
		break;
	case DPMAC_ETH_IF_SMII:
		printf("DPMAC_ETH_IF_SMII\n");
		break;
	case DPMAC_ETH_IF_GMII:
		printf("DPMAC_ETH_IF_GMII\n");
		break;
	case DPMAC_ETH_IF_RGMII:
		printf("DPMAC_ETH_IF_RGMII\n");
		break;
	case DPMAC_ETH_IF_SGMII:
		printf("DPMAC_ETH_IF_SGMII\n");
		break;
	case DPMAC_ETH_IF_QSGMII:
		printf("DPMAC_ETH_IF_QSGMII\n");
		break;
	case DPMAC_ETH_IF_XAUI:
		printf("DPMAC_ETH_IF_XAUI\n");
		break;
	case DPMAC_ETH_IF_XFI:
		printf("DPMAC_ETH_IF_XFI\n");
		break;
	default:
		assert(false);
		break;
	}
}

static int print_dpmac_attr_v9(uint32_t dpmac_id,
			       struct dprc_obj_desc *target_obj_desc)
{
	uint16_t dpmac_handle;
	int error;
	struct dpmac_attr dpmac_attr;
	bool dpmac_opened = false;

	error = dpmac_open(&restool.mc_io, 0, dpmac_id, &dpmac_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpmac_opened = true;
	if (0 == dpmac_handle) {
		DEBUG_PRINTF(
			"dpmac_open() returned invalid handle (auth 0) for dpmac.%u\n",
			dpmac_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpmac_attr, 0, sizeof(dpmac_attr));
	error = dpmac_get_attributes(&restool.mc_io, 0,
					dpmac_handle, &dpmac_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpmac_id == (uint32_t)dpmac_attr.id);

	printf("dpmac version: %u.%u\n", dpmac_attr.version.major,
	       dpmac_attr.version.minor);
	printf("dpmac object id/portal id: %d\n", dpmac_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	print_dpmac_endpoint(dpmac_id);
	print_dpmac_link_type(dpmac_attr.link_type);
	print_dpmac_eth_if(dpmac_attr.eth_if);
	printf("maximum supported rate %lu Mbps\n",
			(unsigned long)dpmac_attr.max_rate);
	print_obj_label(target_obj_desc);

	error = 0;

out:
	if (dpmac_opened) {
		int error2;

		error2 = dpmac_close(&restool.mc_io, 0, dpmac_handle);
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

static int print_dpmac_attr_v10(uint32_t dpmac_id,
			struct dprc_obj_desc *target_obj_desc)
{
	struct dpmac_attr_v10 dpmac_attr;
	uint16_t dpmac_handle;
	bool dpmac_opened = false;
	uint16_t obj_major, obj_minor;
	int error;

	error = dpmac_open(&restool.mc_io, 0, dpmac_id, &dpmac_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpmac_opened = true;
	if (0 == dpmac_handle) {
		DEBUG_PRINTF(
			"dpmac_open() returned invalid handle (auth 0) for dpmac.%u\n",
			dpmac_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpmac_attr, 0, sizeof(dpmac_attr));
	error = dpmac_get_attributes_v10(&restool.mc_io, 0,
					 dpmac_handle, &dpmac_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpmac_id == (uint32_t)dpmac_attr.id);

	error = dpmac_get_version_v10(&restool.mc_io, 0,
				      &obj_major, &obj_minor);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	printf("dpmac version: %u.%u\n", obj_major, obj_minor);
	printf("dpmac object id/portal id: %d\n", dpmac_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	print_dpmac_endpoint(dpmac_id);
	print_dpmac_link_type(dpmac_attr.link_type);
	print_dpmac_eth_if(dpmac_attr.eth_if);
	printf("maximum supported rate %lu Mbps\n",
			(unsigned long)dpmac_attr.max_rate);
	print_obj_label(target_obj_desc);

	error = 0;

out:
	if (dpmac_opened) {
		int error2;

		error2 = dpmac_close(&restool.mc_io, 0, dpmac_handle);
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

static int print_dpmac_info(uint32_t dpmac_id, int mc_fw_version)
{
	int error;
	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	bool found = false;

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
				restool.root_dprc_handle, 0, dpmac_id,
				"dpmac", &target_obj_desc,
				&target_parent_dprc_id, &found);
	if (error < 0)
		goto out;

	if (strcmp(target_obj_desc.type, "dpmac")) {
		printf("dpmac.%d does not exist\n", dpmac_id);
		return -EINVAL;
	}

	if (mc_fw_version == MC_FW_VERSION_9)
		error = print_dpmac_attr_v9(dpmac_id, &target_obj_desc);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = print_dpmac_attr_v10(dpmac_id, &target_obj_desc);
	if (error < 0)
		goto out;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		error = print_obj_verbose(&target_obj_desc, &dpmac_ops);
	}

out:
	return error;
}

static int info_dpmac(int mc_fw_version)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpmac info <dpmac-object> [--verbose]\n"
		"\n"
		"OPTIONS:\n"
		"--verbose\n"
		"   Shows extended/verbose information about the object\n"
		"\n"
		"EXAMPLE:\n"
		"Display information about dpmac.5:\n"
		"   $ restool dpmac info dpmac.5\n"
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

	error = parse_object_name(restool.obj_name, "dpmac", &obj_id);
	if (error < 0)
		goto out;

	error = print_dpmac_info(obj_id, mc_fw_version);
out:
	return error;
}

static int cmd_dpmac_info_v9(void)
{
	return info_dpmac(MC_FW_VERSION_9);
}

static int cmd_dpmac_info_v10(void)
{
	return info_dpmac(MC_FW_VERSION_10);
}

static int create_dpmac_v9(struct dpmac_cfg *dpmac_cfg)
{
	struct dpmac_attr dpmac_attr;
	uint16_t dpmac_handle;
	int error;

	error = dpmac_create(&restool.mc_io, 0, dpmac_cfg, &dpmac_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	memset(&dpmac_attr, 0, sizeof(struct dpmac_attr));
	error = dpmac_get_attributes(&restool.mc_io, 0, dpmac_handle,
					&dpmac_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	print_new_obj("dpmac", dpmac_attr.id, NULL);

	error = dpmac_close(&restool.mc_io, 0, dpmac_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	return 0;
}

static int create_dpmac_v10(struct dpmac_cfg *dpmac_cfg)
{
	uint32_t dpmac_id, dprc_id;
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

	error = dpmac_create_v10(&restool.mc_io, dprc_handle, 0,
				 dpmac_cfg, &dpmac_id);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	if (dprc_opened) {
		(void)dprc_close(&restool.mc_io, 0, dprc_handle);
		print_new_obj("dpmac", dpmac_id,
			      restool.cmd_option_args[CREATE_OPT_PARENT_DPRC]);
	} else {
		print_new_obj("dpmac", dpmac_id, NULL);
	}

	return 0;
}

static int create_dpmac(int mc_fw_version, const char *usage_msg)
{
	int error;
	long val;
	struct dpmac_cfg dpmac_cfg;

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

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_MAC_ID)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAC_ID);
		error = get_option_value(CREATE_OPT_MAC_ID, &val,
					 "Invalid option: mac-id value",
					 0, INT32_MAX);
		if (error)
			return -EINVAL;
		dpmac_cfg.mac_id = val;
	} else {
		ERROR_PRINTF("--mac-id option missing\n");
		puts(usage_msg);
		return -EINVAL;
	}

	if (mc_fw_version == MC_FW_VERSION_9)
		error = create_dpmac_v9(&dpmac_cfg);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = create_dpmac_v10(&dpmac_cfg);
	else
		return -EINVAL;

	return error;
}

static int cmd_dpmac_create_v9(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpmac create --mac-id=<number>\n"
		"   --mac-id=<number>\n"
		"      Where <number> specifies the id of the hardware MAC associated with\n"
		"      the DPMAC object.\n"
		"\n"
		"EXAMPLE:\n"
		"create a DPMAC with specified mac id:\n"
		"   $ restool dpmac create --mac-id=4\n"
		"\n";

	return create_dpmac(MC_FW_VERSION_9, usage_msg);
}

static int cmd_dpmac_create_v10(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpmac create --mac-id=<number> [OPTIONS]\n"
		"   --mac-id=<number>\n"
		"      Where <number> specifies the id of the hardware MAC associated with\n"
		"      the DPMAC object.\n"
		"\n"
		"OPTIONS:\n"
		"if options are not specified, create DPSW by default options\n"
		"--container=<container-name>\n"
		"   Specifies the parent container name. e.g. dprc.2, dprc.3 etc.\n"
		"   If it is not specified, the new object will be created under the default dprc.\n"
		"\n"
		"EXAMPLE:\n"
		"create a DPMAC with specified mac id:\n"
		"   $ restool dpmac create --mac-id=4\n"
		"\n";

	return create_dpmac(MC_FW_VERSION_10, usage_msg);
}

static int destroy_dpmac_v9(uint32_t dpmac_id)
{
	bool dpmac_opened = false;
	uint16_t dpmac_handle;
	int error, error2;

	error = dpmac_open(&restool.mc_io, 0, dpmac_id, &dpmac_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpmac_opened = true;
	if (0 == dpmac_handle) {
		DEBUG_PRINTF(
			"dpmac_open() returned invalid handle (auth 0) for dpmac.%u\n",
			dpmac_id);
		error = -ENOENT;
		goto out;
	}

	error = dpmac_destroy(&restool.mc_io, 0, dpmac_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpmac_opened = false;
	printf("dpmac.%u is destroyed\n", dpmac_id);

out:
	if (dpmac_opened) {
		error2 = dpmac_close(&restool.mc_io, 0, dpmac_handle);
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

static int destroy_dpmac_v10(uint32_t dpmac_id)
{
	uint16_t dprc_handle;
	uint32_t dprc_id;
	int error;

	dprc_handle = restool.root_dprc_handle;
	dprc_id = restool.root_dprc_id;
	error = get_parent_dprc_id(dpmac_id, "dpmac", &dprc_id);
	if (error)
		return error;

	if (dprc_id != restool.root_dprc_id) {
		error = open_dprc(dprc_id, &dprc_handle);
		if (error)
			return error;
	}

	error = dpmac_destroy_v10(&restool.mc_io, dprc_handle,
				 0, dpmac_id);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	printf("dpmac.%u is destroyed\n", dpmac_id);

out:
	if (dprc_id != restool.root_dprc_id)
		error = dprc_close(&restool.mc_io, 0, dprc_handle);

	return error;
}

static int destroy_dpmac(int mc_fw_version)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpmac destroy <dpmac-object>\n"
		"   e.g. restool dpmac destroy dpmac.9\n"
		"\n";

	int error;
	uint32_t dpmac_id;

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

	error = parse_object_name(restool.obj_name, "dpmac", &dpmac_id);
	if (error < 0)
		goto out;

	if (!find_obj("dpmac", dpmac_id)) {
		error = -EINVAL;
		goto out;
	}

	if (mc_fw_version == MC_FW_VERSION_9)
		error = destroy_dpmac_v9(dpmac_id);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = destroy_dpmac_v10(dpmac_id);
	else
		return -EINVAL;

out:
	return error;
}

static int cmd_dpmac_destroy_v9(void)
{
	return destroy_dpmac(MC_FW_VERSION_9);
}

static int cmd_dpmac_destroy_v10(void)
{
	return destroy_dpmac(MC_FW_VERSION_10);
}

struct object_command dpmac_commands_v9[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpmac_help },

	{ .cmd_name = "info",
	  .options = dpmac_info_options,
	  .cmd_func = cmd_dpmac_info_v9 },

	{ .cmd_name = "create",
	  .options = dpmac_create_options,
	  .cmd_func = cmd_dpmac_create_v9 },

	{ .cmd_name = "destroy",
	  .options = dpmac_destroy_options,
	  .cmd_func = cmd_dpmac_destroy_v9 },

	{ .cmd_name = NULL },
};

struct object_command dpmac_commands_v10[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpmac_help },

	{ .cmd_name = "info",
	  .options = dpmac_info_options,
	  .cmd_func = cmd_dpmac_info_v10 },

	{ .cmd_name = "create",
	  .options = dpmac_create_options,
	  .cmd_func = cmd_dpmac_create_v10 },

	{ .cmd_name = "destroy",
	  .options = dpmac_destroy_options,
	  .cmd_func = cmd_dpmac_destroy_v10 },

	{ .cmd_name = NULL },
};


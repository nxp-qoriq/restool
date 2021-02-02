/* Copyright 2014-2016 Freescale Semiconductor Inc.
 * Copyright 2017-2021 NXP
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
#include "mc_v9/fsl_dpsw.h"
#include "mc_v10/fsl_dpsw.h"

#define ALL_DPSW_OPTS (			\
	DPSW_OPT_FLOODING_DIS |		\
	DPSW_OPT_MULTICAST_DIS |	\
	DPSW_OPT_CTRL_IF_DIS |	\
	DPSW_OPT_FLOODING_METERING_DIS |	\
	DPSW_OPT_METERING_EN |		\
	DPSW_OPT_LAG_DIS |		\
	DPSW_OPT_BP_PER_IF)

static enum mc_cmd_status mc_status;

/**
 * dpsw info command options
 */
enum dpsw_info_options {
	INFO_OPT_HELP = 0,
	INFO_OPT_VERBOSE,
};

static struct option dpsw_info_options[] = {
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

C_ASSERT(ARRAY_SIZE(dpsw_info_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static const char *const dpsw_counter_stats[] = {
	"dpsw_cnt_ing_frame",
	"dpsw_cnt_ing_byte",
	"dpsw_cnt_ing_fltr_frame",
	"dpsw_cnt_ing_frame_discard",
	"dpsw_cnt_ing_mcast_frame",
	"dpsw_cnt_ing_mcast_byte",
	"dpsw_cnt_ing_bcast_frame",
	"dpsw_cnt_ing_bcast_bytes",
	"dpsw_cnt_egr_frame",
	"dpsw_cnt_egr_byte",
	"dpsw_cnt_egr_frame_discard",
	"dpsw_cnt_egr_stp_frame_discard",
	"dpsw_cnt_ing_no_buffer_discard"
};

/**
 * dpsw create command options
 */
enum dpsw_create_options {
	CREATE_OPT_HELP = 0,
	CREATE_OPT_NUM_IFS,
	CREATE_OPT_OPTIONS,
	CREATE_OPT_MAX_VLANS,
	CREATE_OPT_MAX_FDBS,
	CREATE_OPT_MAX_FDB_ENTRIES,
	CREATE_OPT_FDB_AGING_TIME,
	CREATE_OPT_MAX_FDB_MC_GROUPS,
	CREATE_OPT_PARENT_DPRC,
	CREATE_OPT_COMPONENT_TYPE,
	CREATE_OPT_MEM_SIZE,
	CREATE_OPT_FLOODING_CFG,
	CREATE_OPT_BROADCAST_CFG,
};

static struct option dpsw_create_options[] = {
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

	[CREATE_OPT_OPTIONS] = {
		.name = "options",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_VLANS] = {
		.name = "max-vlans",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_FDBS] = {
		.name = "max-fdbs",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_FDB_ENTRIES] = {
		.name = "max-fdb-entries",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_FDB_AGING_TIME] = {
		.name = "fdb-aging-time",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_FDB_MC_GROUPS] = {
		.name = "max-fdb-mc-groups",
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

	[CREATE_OPT_COMPONENT_TYPE] = {
		.name = "component-type",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_FLOODING_CFG] = {
		.name = "flooding-cfg",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MEM_SIZE] = {
		.name = "mem-size",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_BROADCAST_CFG] = {
		.name = "broadcast-cfg",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpsw_create_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpsw destroy command options
 */
enum dpsw_destroy_options {
	DESTROY_OPT_HELP = 0,
};

static struct option dpsw_destroy_options[] = {
	[DESTROY_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

/**
 * dpsw update command options
 */
enum dpsw_update_options {
	UPDATE_OPT_HELP = 0,
	UPDATE_OPT_IF_ID,
	UPDATE_OPT_TAILDROP,
	UPDATE_OPT_UNITS,
	UPDATE_OPT_THRESHOLD,
};

static struct option dpsw_update_options[] = {
	[UPDATE_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	[UPDATE_OPT_IF_ID] = {
		.name = "if-id",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[UPDATE_OPT_TAILDROP] = {
		.name = "taildrop",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[UPDATE_OPT_UNITS] = {
		.name = "units",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[UPDATE_OPT_THRESHOLD] = {
		.name = "threshold",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpsw_destroy_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

const struct flib_ops dpsw_ops = {
	.obj_open = dpsw_open_v10,
	.obj_close = dpsw_close_v10,
	.obj_get_irq_mask = dpsw_get_irq_mask_v10,
	.obj_get_irq_status = dpsw_get_irq_status_v10,
};

static struct option_entry options_map[] = {
	OPTION_MAP_ENTRY(DPSW_OPT_FLOODING_DIS),
	OPTION_MAP_ENTRY(DPSW_OPT_MULTICAST_DIS),
	OPTION_MAP_ENTRY(DPSW_OPT_CTRL_IF_DIS),
	OPTION_MAP_ENTRY(DPSW_OPT_FLOODING_METERING_DIS),
	OPTION_MAP_ENTRY(DPSW_OPT_METERING_EN),
	OPTION_MAP_ENTRY(DPSW_OPT_LAG_DIS),
	OPTION_MAP_ENTRY(DPSW_OPT_BP_PER_IF),
};
static unsigned int options_num = ARRAY_SIZE(options_map);

static struct option_entry component_type_map[] = {
	OPTION_MAP_ENTRY(DPSW_COMPONENT_TYPE_C_VLAN),
	OPTION_MAP_ENTRY(DPSW_COMPONENT_TYPE_S_VLAN),
};
static unsigned int component_type_num = ARRAY_SIZE(component_type_map);

static struct option_entry flooding_map[] = {
	OPTION_MAP_ENTRY(DPSW_FLOODING_PER_VLAN),
	OPTION_MAP_ENTRY(DPSW_FLOODING_PER_FDB),
};
static unsigned int flooding_num = ARRAY_SIZE(flooding_map);

static struct option_entry broadcast_map[] = {
	OPTION_MAP_ENTRY(DPSW_BROADCAST_PER_OBJECT),
	OPTION_MAP_ENTRY(DPSW_BROADCAST_PER_FDB),
};
static unsigned int broadcast_num = ARRAY_SIZE(broadcast_map);

static int cmd_dpsw_help(void)
{
	static const char help_msg[] =
		"\n"
		"Usage: restool dpsw <command> [--help] [ARGS...]\n"
		"Where <command> can be:\n"
		"   info - displays detailed information about a DPSW object.\n"
		"   create - creates a new child DPSW under the root DPRC.\n"
		"   destroy - destroys a child DPSW under the root DPRC.\n"
		"   update - configure a child DPSW under the root DPRC.\n"
		"\n"
		"For command-specific help, use the --help option of each command.\n"
		"\n";

	printf(help_msg);
	return 0;
}

static void print_dpsw_options(uint64_t options)
{
	if ((options & ~ALL_DPSW_OPTS) != 0) {
		printf("\tUnrecognized options found...\n");
		return;
	}

	if (options & DPSW_OPT_FLOODING_DIS)
		printf("\tDPSW_OPT_FLOODING_DIS\n");

	if (options & DPSW_OPT_MULTICAST_DIS)
		printf("\tDPSW_OPT_MULTICAST_DIS\n");

	if (options & DPSW_OPT_CTRL_IF_DIS)
		printf("\tDPSW_OPT_CTRL_IF_DIS\n");

	if (options & DPSW_OPT_FLOODING_METERING_DIS)
		printf("\tDPSW_OPT_FLOODING_METERING_DIS\n");

	if (options & DPSW_OPT_METERING_EN)
		printf("\tDPSW_OPT_METERING_EN\n");

	if (options & DPSW_OPT_LAG_DIS)
		printf("\tDPSW_OPT_LAG_DIS\n");

	if (options & DPSW_OPT_BP_PER_IF)
		printf("\tDPSW_OPT_BP_PER_IF\n");
}

static int print_dpsw_endpoint(uint32_t target_id, uint16_t num_ifs,
			       uint16_t token)
{
	struct dprc_endpoint endpoint1;
	struct dprc_endpoint endpoint2;
	struct dpsw_taildrop_cfg cfg;
	int state;
	int error = 0;
	int k;
	uint32_t dpsw_id;
	int counter_iterator;
	uint64_t counter;
	uint16_t max_frame_length;

	error = parse_object_name(restool.obj_name, "dpsw", &dpsw_id);
	if (error)
		return error;

	printf("endpoints:\n");
	for (k = 0; k < num_ifs; ++k) {
		memset(&endpoint1, 0, sizeof(struct dprc_endpoint));
		memset(&endpoint2, 0, sizeof(struct dprc_endpoint));
		strncpy(endpoint1.type, "dpsw", EP_OBJ_TYPE_MAX_LEN);
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
		if (error == 0) {
			error = dpsw_if_get_taildrop(&restool.mc_io,
					0, token, endpoint1.if_id,
					0, &cfg);
			if (error)
				return error;
			printf("\tTaildrop enabled: ");
			if (cfg.enable == 0)
				printf("false\n");
			else
				printf("true\n");
			printf("\tTaildrop units: ");
			switch(cfg.units) {
			case DPSW_TAILDROP_DROP_UNIT_BYTE:
				printf("BYTES\n");
				break;
			case DPSW_TAILDROP_DROP_UNIT_FRAMES:
				printf("FRAMES\n");
				break;
			case DPSW_TAILDROP_DROP_UNIT_BUFFERS:
				printf("BUFFERS\n");
				break;
			}
			printf("\tTaildrop threshold: %d\n", cfg.threshold);
		}

		error = dpsw_if_get_max_frame_length(&restool.mc_io, 0, token,
						     k, &max_frame_length);
		if (error)
			return error;
		printf("\tmax frame length: %hu\n", max_frame_length);

		// print the interface stats
		for (counter_iterator = DPSW_CNT_ING_FRAME;
		     counter_iterator <= DPSW_CNT_ING_NO_BUFFER_DISCARD;
		     counter_iterator++) {
			error = dpsw_if_get_counter(&restool.mc_io, 0,
					token, k, counter_iterator,
					&counter);
			if (error)
				return error;

			printf("\t%s: %ld\n",
				dpsw_counter_stats[counter_iterator],
				counter);
		}
	}
	return 0;
}

static int print_dpsw_attr_v9(uint32_t dpsw_id,
			struct dprc_obj_desc *target_obj_desc)
{
	uint16_t dpsw_handle;
	int error;
	struct dpsw_attr_v9 dpsw_attr;
	bool dpsw_opened = false;

	error = dpsw_open(&restool.mc_io, 0, dpsw_id, &dpsw_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpsw_opened = true;
	if (0 == dpsw_handle) {
		DEBUG_PRINTF(
			"dpsw_open() returned invalid handle (auth 0) for dpsw.%u\n",
			dpsw_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpsw_attr, 0, sizeof(dpsw_attr));
	error = dpsw_get_attributes_v9(&restool.mc_io, 0, dpsw_handle,
				       &dpsw_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpsw_id == (uint32_t)dpsw_attr.id);

	printf("dpsw version: %u.%u\n", dpsw_attr.version.major,
	       dpsw_attr.version.minor);
	printf("dpsw id: %d\n", dpsw_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	print_dpsw_endpoint(dpsw_id, dpsw_attr.num_ifs, dpsw_handle);
	printf("dpsw_attr.options value is: %#llx\n",
	       (unsigned long long)dpsw_attr.options);
	print_dpsw_options(dpsw_attr.options);
	printf("max VLANs: %u\n", (uint32_t)dpsw_attr.max_vlans);
	printf("max FDBs: %u\n", (uint32_t)dpsw_attr.max_fdbs);
	printf("frame storage memory size: %u\n",
	       (uint32_t)dpsw_attr.mem_size);
	printf("number of interfaces: %u\n", (uint32_t)dpsw_attr.num_ifs);
	printf("current number of VLANs: %u\n", (uint32_t)dpsw_attr.num_vlans);
	printf("current number of FDBs: %u\n", (uint32_t)dpsw_attr.num_fdbs);
	print_obj_label(target_obj_desc);

	error = 0;

out:
	if (dpsw_opened) {
		int error2;

		error2 = dpsw_close(&restool.mc_io, 0, dpsw_handle);
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

static int print_dpsw_attr_v10(uint32_t dpsw_id,
			       struct dprc_obj_desc *target_obj_desc)
{
	struct dpsw_attr_v10 dpsw_attr;
	uint16_t obj_major, obj_minor;
	bool dpsw_opened = false;
	uint16_t dpsw_handle;
	int error;

	error = dpsw_open_v10(&restool.mc_io, 0, dpsw_id, &dpsw_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpsw_opened = true;
	if (0 == dpsw_handle) {
		DEBUG_PRINTF(
			"dpsw_open() returned invalid handle (auth 0) for dpsw.%u\n",
			dpsw_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpsw_attr, 0, sizeof(dpsw_attr));
	error = dpsw_get_attributes_v10(&restool.mc_io, 0, dpsw_handle,
					&dpsw_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpsw_id == (uint32_t)dpsw_attr.id);

	error = dpsw_get_api_version_v10(&restool.mc_io, 0, &obj_major, &obj_minor);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	printf("dpsw version: %u.%u\n", obj_major, obj_minor);
	printf("dpsw id: %d\n", dpsw_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	print_dpsw_endpoint(dpsw_id, dpsw_attr.num_ifs, dpsw_handle);
	printf("dpsw_attr.options value is: %#llx\n",
	       (unsigned long long)dpsw_attr.options);
	print_dpsw_options(dpsw_attr.options);
	printf("max VLANs: %u\n", (uint32_t)dpsw_attr.max_vlans);
	printf("max FDBs: %u\n", (uint32_t)dpsw_attr.max_fdbs);
	printf("max FDB entries: %u\n", (uint32_t)dpsw_attr.max_fdb_entries);
	printf("FDB aging time: %u\n", (uint32_t)dpsw_attr.fdb_aging_time);
	printf("max FDB MC groups: %u\n", (uint32_t)dpsw_attr.max_fdb_mc_groups);
	printf("frame storage memory size: %u\n",
	       (uint32_t)dpsw_attr.mem_size);
	printf("number of interfaces: %u\n", (uint32_t)dpsw_attr.num_ifs);
	printf("current number of VLANs: %u\n", (uint32_t)dpsw_attr.num_vlans);
	printf("current number of FDBs: %u\n", (uint32_t)dpsw_attr.num_fdbs);
	printf("component_type: %s\n",
	       dpsw_attr.component_type == DPSW_COMPONENT_TYPE_C_VLAN ?
	       "DPSW_COMPONENT_TYPE_C_VLAN" : "DPSW_COMPONENT_TYPE_S_VLAN");
	print_obj_label(target_obj_desc);
	printf("flooding cfg: %s\n", dpsw_attr.flooding_cfg == DPSW_FLOODING_PER_FDB ?
			"DPSW_FLOODING_PER_FDB" : "DPSW_FLOODING_PER_VLAN");
	printf("broadcast cfg: %s\n", dpsw_attr.broadcast_cfg == DPSW_BROADCAST_PER_FDB ?
			"DPSW_BRODCAST_PER_FDB" : "DPSW_BROADCAST_PER_OBJECT");

	error = 0;

out:
	if (dpsw_opened) {
		int error2;

		error2 = dpsw_close_v10(&restool.mc_io, 0, dpsw_handle);
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

static int print_dpsw_info(uint32_t dpsw_id, int mc_fw_version)
{
	int error;
	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	bool found = false;

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
				restool.root_dprc_handle, 0, dpsw_id,
				"dpsw", &target_obj_desc,
				&target_parent_dprc_id, &found);
	if (error < 0)
		goto out;

	if (strcmp(target_obj_desc.type, "dpsw")) {
		printf("dpsw.%d does not exist\n", dpsw_id);
		return -EINVAL;
	}

	if (mc_fw_version == MC_FW_VERSION_9)
		error = print_dpsw_attr_v9(dpsw_id, &target_obj_desc);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = print_dpsw_attr_v10(dpsw_id, &target_obj_desc);
	if (error < 0)
		goto out;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		error = print_obj_verbose(&target_obj_desc, &dpsw_ops);
	}

out:
	return error;
}

static int info_dpsw(int mc_fw_version)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpsw info <dpsw-object> [--verbose]\n"
		"\n"
		"OPTIONS:\n"
		"--verbose\n"
		"   Shows extended/verbose information about the object\n"
		"\n"
		"EXAMPLE:\n"
		"Display information about dpsw.0:\n"
		"   $ restool dpsw info dpsw.0\n"
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

	error = parse_object_name(restool.obj_name, "dpsw", &obj_id);
	if (error < 0)
		goto out;

	error = print_dpsw_info(obj_id, mc_fw_version);

out:
	return error;
}

static int cmd_dpsw_info_v9(void)
{
	return info_dpsw(MC_FW_VERSION_9);
}

static int cmd_dpsw_info_v10(void)
{
	return info_dpsw(MC_FW_VERSION_10);
}

static int create_dpsw_v9(const char *usage_msg)
{
	struct dpsw_cfg_v9 dpsw_cfg = {0};
	struct dpsw_attr_v9 dpsw_attr;
	uint16_t dpsw_handle;
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

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_NUM_IFS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_NUM_IFS);
		error = get_option_value(CREATE_OPT_NUM_IFS, &val,
					 "Invalid number of interfaces",
					 0, UINT16_MAX);
		if (error)
			return -EINVAL;
		dpsw_cfg.num_ifs = (uint16_t)val;
	} else {
		dpsw_cfg.num_ifs = 4; /* Todo: default value not defined */
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_OPTIONS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_OPTIONS);
		error = parse_generic_create_options(
				restool.cmd_option_args[CREATE_OPT_OPTIONS],
				&dpsw_cfg.adv.options,
				options_map,
				options_num);
		if (error < 0) {
			DEBUG_PRINTF(
				"parse_generic_create_options() failed with error %d, cannot get options-mask\n",
				error);
			return error;
		}
	} else {
		dpsw_cfg.adv.options = 0;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_MAX_VLANS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_MAX_VLANS);
		error = get_option_value(CREATE_OPT_MAX_VLANS, &val,
					 "Invalid max vlans",
					 0, UINT16_MAX);
		if (error)
			return -EINVAL;
		dpsw_cfg.adv.max_vlans = (uint16_t)val;
	} else {
		dpsw_cfg.adv.max_vlans = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_FDBS)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_FDBS);
		error = get_option_value(CREATE_OPT_MAX_FDBS, &val,
					 "Invalid max FDB", 0, UINT8_MAX);
		if (error)
			return -EINVAL;
		dpsw_cfg.adv.max_fdbs = (uint8_t)val;
	} else {
		dpsw_cfg.adv.max_fdbs = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_FDB_ENTRIES)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_FDB_ENTRIES);
		error = get_option_value(CREATE_OPT_MAX_FDB_ENTRIES, &val,
					 "Invalid number of FDB entries",
					 0, UINT16_MAX);
		if (error)
			return -EINVAL;
		dpsw_cfg.adv.max_fdb_entries = (uint16_t)val;
	} else {
		dpsw_cfg.adv.max_fdb_entries = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_FDB_AGING_TIME)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_FDB_AGING_TIME);
		error = get_option_value(CREATE_OPT_FDB_AGING_TIME, &val,
					 "Invalid FDB aging time",
					 0, UINT16_MAX);
		if (error)
			return -EINVAL;
		dpsw_cfg.adv.fdb_aging_time = (uint16_t)val;
	} else {
		dpsw_cfg.adv.fdb_aging_time = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_FDB_MC_GROUPS)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_FDB_MC_GROUPS);
		error = get_option_value(CREATE_OPT_MAX_FDB_MC_GROUPS, &val,
					 "Invalid number of multicast groups",
					 0, UINT16_MAX);
		if (error)
			return -EINVAL;
		dpsw_cfg.adv.max_fdb_mc_groups = (uint16_t)val;
	} else {
		dpsw_cfg.adv.max_fdb_mc_groups = 0;
	}

	error = dpsw_create_v9(&restool.mc_io, 0, &dpsw_cfg, &dpsw_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	memset(&dpsw_attr, 0, sizeof(struct dpsw_attr_v9));
	error = dpsw_get_attributes_v9(&restool.mc_io, 0, dpsw_handle, &dpsw_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	print_new_obj("dpsw", dpsw_attr.id, NULL);

	error = dpsw_close(&restool.mc_io, 0, dpsw_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	return 0;
}

static int cmd_dpsw_create_v9(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpsw create [OPTIONS]\n"
		"\n"
		"OPTIONS:\n"
		"if options are not specified, create DPSW by default options\n"
		"--num-ifs=<number>\n"
		"	Number of external and internal interfaces.\n"
		"--options=<options-mask>\n"
		"   Where <options-mask> is a comma separated list of DPSW options:\n"
		"	DPSW_OPT_FLOODING_DIS\n"
		"	DPSW_OPT_MULTICAST_DIS\n"
		"	DPSW_OPT_CTRL_IF_DIS\n"
		"	DPSW_OPT_FLOODING_METERING_DIS\n"
		"	DPSW_OPT_METERING_EN\n"
		"--max-vlans=<number>\n"
		"	Maximum number of VLAN's. Default is 16.\n"
		"--max-fdbs=<number>\n"
		"	Maximum Number of FDB's. Default is 16.\n"
		"--max-fdb-entries=<number>\n"
		"	Number of FDB entries. Default is 1024;\n"
		"--fdb-aging-time=<number>\n"
		"	Default FDB aging time in seconds. Default is 300 seconds.\n"
		"--max-fdb-mc-groups=<number>\n"
		"	Number of multicast groups in each FDB table. Default is 32.\n"
		"\n"
		"EXAMPLE:\n"
		"Create a DPSW object with all default options:\n"
		"   $ restool dpsw create\n"
		"\n";

	return create_dpsw_v9(usage_msg);
}

static int dpsw_parse_component_type(char *options_str,
				     enum dpsw_component_type *component_type,
				     struct option_entry options_map[],
				     unsigned int component_type_num)
{
	unsigned int i;

	for (i = 0; i < component_type_num; ++i) {
		if (strcmp(options_str, options_map[i].str) == 0) {
			*component_type = options_map[i].value;
			return 0;
		}
	}

	ERROR_PRINTF("Invalid configuration: '%s'\n", options_str);
	return -EINVAL;
}

static int dpsw_parse_flooding(char *options_str, enum dpsw_flooding_cfg *flooding_cfg,
			       struct option_entry options_map[],
			       unsigned int num_options)
{
	unsigned int i;

	for (i = 0; i < num_options; ++i) {
		if (strcmp(options_str, options_map[i].str) == 0) {
			*flooding_cfg = options_map[i].value;
			return 0;
		}
	}

	ERROR_PRINTF("Invalid configuration: '%s'\n", options_str);
	return -EINVAL;
}

static int dpsw_parse_broadcast(char *options_str, enum dpsw_broadcast_cfg *broadcast_cfg,
				struct option_entry options_map[],
				unsigned int num_options)
{
	unsigned int i;

	for (i = 0; i < num_options; ++i) {
		if (strcmp(options_str, options_map[i].str) == 0) {
			*broadcast_cfg = options_map[i].value;
			return 0;
		}
	}

	ERROR_PRINTF("Invalid configuration: '%s'\n", options_str);
	return -EINVAL;
}

static int create_dpsw_v10(const char *usage_msg)
{
	struct dpsw_cfg_v10 dpsw_cfg = {0};
	uint32_t dpsw_id, dprc_id;
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

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_NUM_IFS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_NUM_IFS);
		error = get_option_value(CREATE_OPT_NUM_IFS, &val,
				     "Invalid number of interfaces\n",
				     1, UINT16_MAX);
		if (error)
			return error;
		dpsw_cfg.num_ifs = (uint16_t)val;
	} else {
		dpsw_cfg.num_ifs = 4; /* Todo: default value not defined */
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_OPTIONS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_OPTIONS);
		error = parse_generic_create_options(
				restool.cmd_option_args[CREATE_OPT_OPTIONS],
				&dpsw_cfg.adv.options,
				options_map,
				options_num);
		if (error < 0) {
			DEBUG_PRINTF(
				"parse_generic_create_options() failed with error %d, cannot get options-mask\n",
				error);
			return error;
		}
	} else { /* Todo: default option may change with spec */
		dpsw_cfg.adv.options = 0;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_MAX_VLANS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_MAX_VLANS);
		error = get_option_value(CREATE_OPT_MAX_VLANS, &val,
				     "Invalid max vlans\n", 1, UINT16_MAX);
		if (error)
			return error;
		dpsw_cfg.adv.max_vlans = (uint16_t)val;
	} else {
		dpsw_cfg.adv.max_vlans = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_FDBS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_MAX_FDBS);
		error = get_option_value(CREATE_OPT_MAX_FDBS, &val,
				     "Invalid max FDB\n", 1, UINT8_MAX);
		if (error)
			return error;
		dpsw_cfg.adv.max_fdbs = (uint8_t)val;
	} else {
		dpsw_cfg.adv.max_fdbs = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_FDB_ENTRIES)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_FDB_ENTRIES);
		error = get_option_value(CREATE_OPT_MAX_FDB_ENTRIES, &val,
				     "Invalid number of FDB entries\n",
				     1, UINT16_MAX);
		if (error)
			return error;
		dpsw_cfg.adv.max_fdb_entries = (uint16_t)val;
	} else {
		dpsw_cfg.adv.max_fdb_entries = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_FDB_AGING_TIME)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_FDB_AGING_TIME);
		error = get_option_value(CREATE_OPT_FDB_AGING_TIME, &val,
				     "Invalid FDB aging time\n", 1, UINT16_MAX);
		if (error)
			return error;
		dpsw_cfg.adv.fdb_aging_time = (uint16_t)val;
	} else {
		dpsw_cfg.adv.fdb_aging_time = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_FDB_MC_GROUPS)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_FDB_MC_GROUPS);
		error = get_option_value(CREATE_OPT_MAX_FDB_MC_GROUPS, &val,
				     "Invalid number of multicast groups\n",
				     1, UINT16_MAX);
		if (error)
			return error;
		dpsw_cfg.adv.max_fdb_mc_groups = (uint16_t)val;
	} else {
		dpsw_cfg.adv.max_fdb_mc_groups = 0;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_MEM_SIZE)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_MEM_SIZE);
		error = get_option_value(CREATE_OPT_MEM_SIZE, &val,
				"Invalid mem_size value\n", 0, UINT16_MAX);
		if (error)
			return error;
		dpsw_cfg.adv.mem_size = (uint16_t)val;
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

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_COMPONENT_TYPE)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_COMPONENT_TYPE);
		error = dpsw_parse_component_type(
				restool.cmd_option_args[CREATE_OPT_COMPONENT_TYPE],
				&dpsw_cfg.adv.component_type,
				component_type_map, component_type_num);
		if (error)
			return error;
	} else {
		dpsw_cfg.adv.component_type = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_FLOODING_CFG)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_FLOODING_CFG);
		error = dpsw_parse_flooding(restool.cmd_option_args[CREATE_OPT_FLOODING_CFG],
					    &dpsw_cfg.adv.flooding_cfg,
					    flooding_map, flooding_num);
		if (error)
			return error;
	} else {
		dpsw_cfg.adv.flooding_cfg = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_BROADCAST_CFG)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_BROADCAST_CFG);
		error = dpsw_parse_broadcast(restool.cmd_option_args[CREATE_OPT_BROADCAST_CFG],
					     &dpsw_cfg.adv.broadcast_cfg,
					     broadcast_map, broadcast_num);
		if (error)
			return error;
	} else {
		dpsw_cfg.adv.broadcast_cfg = 0;
	}

	error = dpsw_create_v10(&restool.mc_io, dprc_handle, 0,
				&dpsw_cfg, &dpsw_id);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	if (dprc_opened) {
		(void)dprc_close(&restool.mc_io, 0, dprc_handle);
		print_new_obj("dpsw", dpsw_id,
			      restool.cmd_option_args[CREATE_OPT_PARENT_DPRC]);
	} else {
		print_new_obj("dpsw", dpsw_id, NULL);
	}

	return error;
}

static int cmd_dpsw_create_v10(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpsw create [OPTIONS]\n"
		"\n"
		"OPTIONS:\n"
		"if options are not specified, create DPSW by default options\n"
		"--num-ifs=<number>\n"
		"	Number of external and internal interfaces.\n"
		"--options=<options-mask>\n"
		"   Where <options-mask> is a comma separated list of DPSW options:\n"
		"	DPSW_OPT_FLOODING_DIS\n"
		"	DPSW_OPT_MULTICAST_DIS\n"
		"	DPSW_OPT_CTRL_IF_DIS\n"
		"	DPSW_OPT_FLOODING_METERING_DIS\n"
		"	DPSW_OPT_METERING_EN\n"
		"	DPSW_OPT_LAG_DIS\n"
		"	DPSW_OPT_BP_PER_IF\n"
		"--max-vlans=<number>\n"
		"	Maximum number of VLAN's. Default is 16.\n"
		"--max-fdbs=<number>\n"
		"	Maximum Number of FDB's. Default is 16.\n"
		"--max-fdb-entries=<number>\n"
		"	Number of FDB entries. Default is 1024;\n"
		"--fdb-aging-time=<number>\n"
		"	Default FDB aging time in seconds. Default is 300 seconds.\n"
		"--max-fdb-mc-groups=<number>\n"
		"	Number of multicast groups in each FDB table. Default is 32.\n"
		"--flooding-cfg=<flooding configuration>\n"
		"   Where <flooding configuration> is one of the following:\n"
		"	DPSW_FLOODING_PER_VLAN\n"
		"	DPSW_FLOODING_PER_FDB\n"
		"--broadcast-cfg=<broadcast configuration>\n"
		"   Where <broadcast configuration> is one of the following:\n"
		"	DPSW_BROADCAST_PER_OBJECT\n"
		"	DPSW_BROADCAST_PER_FDB\n"
		"--container=<container-name>\n"
		"   Specifies the parent container name. e.g. dprc.2, dprc.3 etc.\n"
		"   If it is not specified, the new object will be created under the default dprc.\n"
		"--component-type=<option_type>\n"
		"	Where <option_type> is one of the following:\n"
		"		DPSW_COMPONENT_TYPE_C_VLAN\n"
		"		DPSW_COMPONENT_TYPE_S_VLAN\n"
		"	Default is DPSW_COMPONENT_TYPE_C_VLAN.\n"
		"--mem-size=<number>\n"
		"   Size of the memory used for internal buffers expressed as number of 256byte buffers.\n"
		"   Default is 32.\n"
		"\n"
		"EXAMPLE:\n"
		"Create a DPSW object with all default options:\n"
		"   $ restool dpsw create\n"
		"\n";

	return create_dpsw_v10(usage_msg);
}

static int destroy_dpsw_v9(uint32_t dpsw_id)
{
	bool dpsw_opened = false;
	uint16_t dpsw_handle;
	int error, error2;

	error = dpsw_open(&restool.mc_io, 0, dpsw_id, &dpsw_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpsw_opened = true;
	if (0 == dpsw_handle) {
		DEBUG_PRINTF(
			"dpsw_open() returned invalid handle (auth 0) for dpsw.%u\n",
			dpsw_id);
		error = -ENOENT;
		goto out;
	}

	error = dpsw_destroy(&restool.mc_io, 0, dpsw_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpsw_opened = false;
	printf("dpsw.%u is destroyed\n", dpsw_id);

out:
	if (dpsw_opened) {
		error2 = dpsw_close(&restool.mc_io, 0, dpsw_handle);
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

static int destroy_dpsw_v10(uint32_t dpsw_id)
{
	uint16_t dprc_handle;
	uint32_t dprc_id;
	int error;

	dprc_handle = restool.root_dprc_handle;
	dprc_id = restool.root_dprc_id;
	error = get_parent_dprc_id(dpsw_id, "dpsw", &dprc_id);
	if (error)
		return error;

	if (dprc_id != restool.root_dprc_id) {
		error = open_dprc(dprc_id, &dprc_handle);
		if (error)
			return error;
	}

	error = dpsw_destroy_v10(&restool.mc_io, dprc_handle,
				 0, dpsw_id);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	printf("dpsw.%u is destroyed\n", dpsw_id);

out:
	if (dprc_id != restool.root_dprc_id)
		error = dprc_close(&restool.mc_io, 0, dprc_handle);

	return error;
}

static int destroy_dpsw(int mc_fw_version)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpsw destroy <dpsw-object>\n"
		"   e.g. restool dpsw destroy dpsw.8\n"
		"\n";

	int error;
	uint32_t dpsw_id;

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

	error = parse_object_name(restool.obj_name, "dpsw", &dpsw_id);
	if (error < 0)
		goto out;

	if (!find_obj("dpsw", dpsw_id)) {
		error = -EINVAL;
		goto out;
	}

	if (mc_fw_version == MC_FW_VERSION_9)
		error = destroy_dpsw_v9(dpsw_id);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = destroy_dpsw_v10(dpsw_id);
	else
		return -EINVAL;

out:
	return error;
}

static int cmd_dpsw_destroy_v9(void)
{
	return destroy_dpsw(MC_FW_VERSION_9);
}

static int cmd_dpsw_destroy_v10(void)
{
	return destroy_dpsw(MC_FW_VERSION_10);
}

static int update_dpsw(const char *usage_msg)
{
	long id_if = -1, enable = -1, threshold = -1;
	char *str, changed_congestion_unit = 0;
	enum dpsw_congestion_unit units = {0};
	struct dpsw_attr_v10 dpsw_attr;
	struct dpsw_taildrop_cfg cfg;
	uint16_t dpsw_handle;
	uint32_t dpsw_id;
	int error, k;

	if (restool.obj_name == NULL) {
		ERROR_PRINTF("Should provide a dpsw!\n");
		puts(usage_msg);
		return -EINVAL;
	}

	error = parse_object_name(restool.obj_name, "dpsw", &dpsw_id);
	if (error)
		return error;

	error = dpsw_open_v10(&restool.mc_io, 0, dpsw_id, &dpsw_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return 0;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(UPDATE_OPT_HELP)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(UPDATE_OPT_HELP);
		puts(usage_msg);
		return 0;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(UPDATE_OPT_IF_ID)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(UPDATE_OPT_IF_ID);
		error = get_option_value(UPDATE_OPT_IF_ID, &id_if,
					 "Invalid interface id",
					 0, UINT16_MAX);
		if (error)
			return -EINVAL;
		error = dpsw_if_get_taildrop(&restool.mc_io, 0, dpsw_handle,
					     id_if, 0, &cfg);
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(UPDATE_OPT_TAILDROP)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(UPDATE_OPT_TAILDROP);
		error = get_option_value(UPDATE_OPT_TAILDROP, &enable,
					 "Invalid taildrop enable value",
					 0, 1);
		if (error)
			return -EINVAL;
		cfg.enable = enable;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(UPDATE_OPT_UNITS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(UPDATE_OPT_UNITS);
		str = restool.cmd_option_args[UPDATE_OPT_UNITS];
		if (!strcmp(str, "BYTES"))
			cfg.units = DPSW_TAILDROP_DROP_UNIT_BYTE;
		else if (!strcmp(str, "FRAMES"))
			cfg.units = DPSW_TAILDROP_DROP_UNIT_FRAMES;
		else if (!strcmp(str, "BUFFERS"))
			cfg.units = DPSW_TAILDROP_DROP_UNIT_BUFFERS;
		else
			return -EINVAL;
		units = cfg.units;
		changed_congestion_unit = 1;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(UPDATE_OPT_THRESHOLD)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(UPDATE_OPT_THRESHOLD);
		error = get_option_value(UPDATE_OPT_THRESHOLD, &threshold,
					 "Invalid threshold value",
					 0,
					 UINT32_MAX);
		if (error)
			return -EINVAL;
		cfg.threshold = threshold;
	}

	if (id_if != -1) {
		return dpsw_if_set_taildrop(&restool.mc_io,
				0, dpsw_handle, id_if, 0, &cfg);
	} else {
		memset(&dpsw_attr, 0, sizeof(struct dpsw_attr_v10));
		error = dpsw_get_attributes_v10(&restool.mc_io,
				0, dpsw_handle, &dpsw_attr);
		if (error)
			return error;

		for (k = 0; k < dpsw_attr.num_ifs; ++k) {
			if (enable == -1 || changed_congestion_unit == 0 ||
				threshold == -1)
			{
				error = dpsw_if_get_taildrop(&restool.mc_io,
						0, dpsw_handle, k, 0, &cfg);
				if (error)
					return error;
			}
			if (enable != -1)
				cfg.enable = enable;
			if (changed_congestion_unit != 0)
				cfg.units = units;
			if (threshold != -1)
				cfg.threshold = threshold;
			error = dpsw_if_set_taildrop(&restool.mc_io,
					0, dpsw_handle, k, 0, &cfg);
			if (error)
				return error;
		}
	}

	dpsw_close(&restool.mc_io, 0, dpsw_handle);
	return 0;
}

static int cmd_dpsw_update_v10(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpsw update <dpsw-object> [OPTIONS]\n"
		"\n"
		"OPTIONS:\n"
		"--if-id=<number>\n"
		"\tSpecifies the number of the interface to configure.\n"
		"\tIf not specified, configure all interfaces.\n"
		"--taildrop=<0|1>\n"
		"\tUsed to enable or disable the feature.\n"
		"--units=<units>\n"
		"\tSpecifies taildrop units: BYTES\\FRAMES\\BUFFERS\n"
		"--threshold=<number>\n"
		"\tSpecifies taildrop threshold\n";
	return update_dpsw(usage_msg);
}

struct object_command dpsw_commands_v9[] = {
	{ .cmd_name = "--help",
	  .options = NULL,
	  .cmd_func = cmd_dpsw_help },

	{ .cmd_name = "info",
	  .options = dpsw_info_options,
	  .cmd_func = cmd_dpsw_info_v9 },

	{ .cmd_name = "create",
	  .options = dpsw_create_options,
	  .cmd_func = cmd_dpsw_create_v9 },

	{ .cmd_name = "destroy",
	  .options = dpsw_destroy_options,
	  .cmd_func = cmd_dpsw_destroy_v9 },

	{ .cmd_name = NULL },
};

struct object_command dpsw_commands_v10[] = {
	{ .cmd_name = "--help",
	  .options = NULL,
	  .cmd_func = cmd_dpsw_help },

	{ .cmd_name = "info",
	  .options = dpsw_info_options,
	  .cmd_func = cmd_dpsw_info_v10 },

	{ .cmd_name = "create",
	  .options = dpsw_create_options,
	  .cmd_func = cmd_dpsw_create_v10 },

	{ .cmd_name = "destroy",
	  .options = dpsw_destroy_options,
	  .cmd_func = cmd_dpsw_destroy_v10 },

	{ .cmd_name = "update",
	  .options = dpsw_update_options,
	  .cmd_func = cmd_dpsw_update_v10 }, 

	{ .cmd_name = NULL },
};


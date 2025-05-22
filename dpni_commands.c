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
#include <inttypes.h>
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
#include "mc_v10/fsl_dpni.h"

static enum mc_cmd_status mc_status;

/**
 * dpni info command options
 */
enum dpni_info_options {
	INFO_OPT_HELP = 0,
	INFO_OPT_VERBOSE,
};

static struct option dpni_info_options[] = {
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

C_ASSERT(ARRAY_SIZE(dpni_info_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpni create command options
 */
enum dpni_create_options {
	CREATE_OPT_HELP = 0,
	CREATE_OPT_OPTIONS,
	CREATE_OPT_MAC_ADDR,
	CREATE_OPT_MAX_SENDERS,
	CREATE_OPT_MAX_TCS,
	CREATE_OPT_MAX_DIST_PER_TC,
	CREATE_OPT_MAX_FS_ENTRIES_PER_TC,
	CREATE_OPT_MAX_UNICAST_FILTERS,
	CREATE_OPT_MAX_MULTICAST_FILTERS,
	CREATE_OPT_MAX_VLAN_FILTERS,
	CREATE_OPT_MAX_QOS_ENTRIES,
	CREATE_OPT_MAX_QOS_KEY_SIZE,
	CREATE_OPT_MAX_DIST_KEY_SIZE,
	CREATE_OPT_NUM_QUEUES,
	CREATE_OPT_NUM_TCS,
	CREATE_OPT_MAC_ENTRIES,
	CREATE_OPT_VLAN_ENTRIES,
	CREATE_OPT_QOS_ENTRIES,
	CREATE_OPT_FS_ENTRIES,
	CREATE_OPT_PARENT_DPRC,
	CREATE_OPT_MAC_FILTER_ENTRIES,
	CREATE_OPT_VLAN_FILTER_ENTRIES,
	CREATE_OPT_NUM_CGS,
	CREATE_OPT_DIST_KEY_SIZE,
	CREATE_OPT_NUM_CHANNELS,
	CREATE_OPT_NUM_OPR,
};

static struct option dpni_create_options[] = {
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

	[CREATE_OPT_MAC_ADDR] = {
		.name = "mac-addr",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_SENDERS] = {
		.name = "max-senders",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_TCS] = {
		.name = "max-tcs",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_DIST_PER_TC] = {
		.name = "max-dist-per-tc",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_FS_ENTRIES_PER_TC] = {
		.name = "max-fs-entries-per-tc",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_UNICAST_FILTERS] = {
		.name = "max-unicast-filters",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_MULTICAST_FILTERS] = {
		.name = "max-multicast-filters",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_VLAN_FILTERS] = {
		.name = "max-vlan-filters",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_QOS_ENTRIES] = {
		.name = "max-qos-entries",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_QOS_KEY_SIZE] = {
		.name = "max-qos-key-size",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_DIST_KEY_SIZE] = {
		.name = "max-dist-key-size",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_NUM_QUEUES] = {
		.name = "num-queues",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_NUM_TCS] = {
		.name = "num-tcs",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAC_ENTRIES] = {
		.name = "mac-entries",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_VLAN_ENTRIES] = {
		.name = "vlan-entries",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_QOS_ENTRIES] = {
		.name = "qos-entries",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_FS_ENTRIES] = {
		.name = "fs-entries",
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

	[CREATE_OPT_MAC_FILTER_ENTRIES] = {
		.name = "mac-filter-entries",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_VLAN_FILTER_ENTRIES] = {
		.name = "vlan-filter-entries",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_NUM_CGS] = {
		.name = "num-cgs",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_DIST_KEY_SIZE] = {
		.name = "dist-key-size",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_NUM_CHANNELS] = {
		.name = "num-channels",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_NUM_OPR] = {
		.name = "num-opr",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpni_create_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpni destroy command options
 */
enum dpni_destroy_options {
	DESTROY_OPT_HELP = 0,
};

static struct option dpni_destroy_options[] = {
	[DESTROY_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpni_destroy_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

enum dpni_update_options {
	UPDATE_OPT_HELP = 0,
	UPDATE_MAC_ADDR,
};

static struct option dpni_update_options[] = {
	[UPDATE_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	[UPDATE_MAC_ADDR] = {
		.name = "mac-addr",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpni_update_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

const struct flib_ops dpni_ops = {
	.obj_open = dpni_open,
	.obj_close = dpni_close,
	.obj_get_irq_mask = dpni_get_irq_mask,
	.obj_get_irq_status = dpni_get_irq_status,
};

static struct option_entry options_map[] = {
	OPTION_MAP_ENTRY(DPNI_OPT_TX_FRM_RELEASE),
	OPTION_MAP_ENTRY(DPNI_OPT_NO_MAC_FILTER),
	OPTION_MAP_ENTRY(DPNI_OPT_HAS_POLICING),
	OPTION_MAP_ENTRY(DPNI_OPT_SHARED_CONGESTION),
	OPTION_MAP_ENTRY(DPNI_OPT_HAS_KEY_MASKING),
	OPTION_MAP_ENTRY(DPNI_OPT_NO_FS),
	OPTION_MAP_ENTRY(DPNI_OPT_HAS_OPR),
	OPTION_MAP_ENTRY(DPNI_OPT_OPR_PER_TC),
	OPTION_MAP_ENTRY(DPNI_OPT_SINGLE_SENDER),
	OPTION_MAP_ENTRY(DPNI_OPT_CUSTOM_CG),
	OPTION_MAP_ENTRY(DPNI_OPT_CUSTOM_OPR),
	OPTION_MAP_ENTRY(DPNI_OPT_SHARED_HASH_KEY),
	OPTION_MAP_ENTRY(DPNI_OPT_SHARED_FS),
	OPTION_MAP_ENTRY(DPNI_OPT_STASHING_DIS),
};
static unsigned int options_num = ARRAY_SIZE(options_map);

#define DPNI_STATS_PER_PAGE 6

static const char *dpni_stats_strings[][DPNI_STATS_PER_PAGE] = {
	{
	"ingress_all_frames",
	"ingress_all_bytes",
	"ingress_multicast_frames",
	"ingress_multicast_bytes",
	"ingress_broadcast_frames",
	"ingress_broadcast_bytes"
	}, {
	"egress_all_frames",
	"egress_all_bytes",
	"egress_multicast_frames",
	"egress_multicast_bytes",
	"egress_broadcast_frames",
	"egress_broadcast_bytes"
	}, {
	"ingress_filtered_frames",
	"ingress_discarded_frames",
	"ingress_nobuffer_discards",
	"egress_discarded_frames",
	"egress_confirmed_frames",
	""
	}, {
	"ceetm_dequeue_bytes",
	"ceetm_dequeue_frames",
	"ceetm_reject_bytes",
	"ceetm_reject_frames",
	""
	}, {
	"cgr_reject_frames",
	"cgr_reject_bytes",
	""
	}, {
	"policer_cnt_red",
	"policer_cnt_yellow",
	"policer_cnt_green",
	"policer_cnt_re_red",
	"policer_cnt_re_yellow",
	""
	}, {
	"tx_pending_frames_cnt",
	""
	}
};

static int cmd_dpni_help(void)
{
	static const char help_msg[] =
		"\n"
		"Usage: restool dpni <command> [--help] [ARGS...]\n"
		"Where <command> can be:\n"
		"   info - displays detailed information about a DPNI object.\n"
		"   create - creates a new child DPNI under the root DPRC.\n"
		"   destroy - destroys a child DPNI under the root DPRC.\n"
		"   update - update attributes of already created DPNI.\n"
		"\n"
		"For command-specific help, use the --help option of each command.\n"
		"\n";

	printf(help_msg);
	return 0;
}

static void print_dpni_options(uint32_t options)
{
	if ((options & ~ALL_DPNI_OPTS) != 0) {
		printf("\tUnrecognized options found...\n");
		return;
	}

	if (options & DPNI_OPT_TX_FRM_RELEASE)
		printf("\tDPNI_OPT_TX_FRM_RELEASE\n");

	if (options & DPNI_OPT_NO_MAC_FILTER)
		printf("\tDPNI_OPT_NO_MAC_FILTER\n");

	if (options & DPNI_OPT_HAS_POLICING)
		printf("\tDPNI_OPT_HAS_POLICING\n");

	if (options & DPNI_OPT_SHARED_CONGESTION)
		printf("\tDPNI_OPT_SHARED_CONGESTION\n");

	if (options & DPNI_OPT_HAS_KEY_MASKING)
		printf("\tDPNI_OPT_HAS_KEY_MASKING\n");

	if (options & DPNI_OPT_NO_FS)
		printf("\tDPNI_OPT_NO_FS\n");

	if (options & DPNI_OPT_HAS_OPR)
		printf("\tDPNI_OPT_HAS_OPR\n");

	if (options & DPNI_OPT_OPR_PER_TC)
		printf("\tDPNI_OPT_OPR_PER_TC\n");

	if (options & DPNI_OPT_SINGLE_SENDER)
		printf("\tDPNI_OPT_SINGLE_SENDER\n");

	if (options & DPNI_OPT_CUSTOM_CG)
		printf("\tDPNI_OPT_CUSTOM_CG\n");

	if (options & DPNI_OPT_CUSTOM_OPR)
		printf("\tDPNI_OPT_CUSTOM_OPR\n");

	if (options & DPNI_OPT_SHARED_HASH_KEY)
		printf("\tDPNI_OPT_SHARED_HASH_KEY\n");

	if (options & DPNI_OPT_SHARED_FS)
		printf("\tDPNI_OPT_SHARED_FS\n");

	if (options & DPNI_OPT_STASHING_DIS)
		printf("\tDPNI_OPT_STASHING_DIS\n");
}

static int print_dpni_endpoint(uint32_t target_id)
{
	struct dprc_endpoint endpoint1;
	struct dprc_endpoint endpoint2;
	int state;
	int error = 0;

	memset(&endpoint1, 0, sizeof(struct dprc_endpoint));
	memset(&endpoint2, 0, sizeof(struct dprc_endpoint));

	strncpy(endpoint1.type, "dpni", EP_OBJ_TYPE_MAX_LEN);
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

static void print_mac_address(uint8_t mac_addr[6])
{
	printf("mac address: ");
	for (int j = 0; j < 5; ++j)
		printf("%02x:", mac_addr[j]);
	printf("%02x\n", mac_addr[5]);
}

static void dpni_print_stats(const char *strings[],
			     union dpni_statistics dpni_stats)
{
	uint64_t *stat;
	int i;

	stat = (uint64_t *)&dpni_stats.raw;
	for (i = 0; i < DPNI_STATS_PER_PAGE; i++) {
		if (strcmp(strings[i], "\0") == 0)
			break;
		printf("%s: %" PRIu64 "\n", strings[i], *stat);
		stat++;
	}
}

static int print_dpni_attr(uint32_t dpni_id, struct dprc_obj_desc *target_obj_desc)
{
	uint16_t dpni_handle, dpni_major, dpni_minor;
	struct dpni_link_state link_state;
	union dpni_statistics dpni_stats;
	struct dpni_attr dpni_attr;
	bool dpni_opened = false;
	uint8_t mac_addr[6];
	int tc, q, ch;
	int error = 0;
	int error2;
	unsigned int page;
	uint16_t max_frame_length;

	error = dpni_open(&restool.mc_io, 0, dpni_id, &dpni_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpni_opened = true;
	if (0 == dpni_handle) {
		DEBUG_PRINTF(
			"dpni_open() returned invalid handle (auth 0) for dpni.%u\n",
			dpni_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpni_attr, 0, sizeof(dpni_attr));
	error = dpni_get_attributes(&restool.mc_io, 0, dpni_handle, &dpni_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	error = dpni_get_api_version(&restool.mc_io, 0, &dpni_major, &dpni_minor);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	memset(&link_state, 0, sizeof(link_state));
	error = dpni_get_link_state(&restool.mc_io, 0, dpni_handle, &link_state);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	printf("dpni version: %u.%u\n", dpni_major, dpni_minor);
	printf("dpni id: %d\n", dpni_id);

	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	print_dpni_endpoint(dpni_id);
	printf("link status: %d - ", link_state.up);
	link_state.up == 0 ? printf("down\n") :
	link_state.up == 1 ? printf("up\n") : printf("error state\n");

	if (!(dpni_attr.options & DPNI_OPT_NO_MAC_FILTER)) {
		error = dpni_get_primary_mac_addr(&restool.mc_io, 0, dpni_handle, mac_addr);
		if (error < 0) {
			mc_status = flib_error_to_mc_status(error);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
			goto out;
		}

		print_mac_address(mac_addr);
	} else {
		printf("DPNI running in promisc mode (DPNI_OPT_NO_MAC_FILTER)\n");
	}

	error = dpni_get_max_frame_length(&restool.mc_io, 0, dpni_handle,
					  &max_frame_length);
	if (error)
		return error;
	printf("max frame length: %hu\n", max_frame_length);

	printf("dpni_attr.options value is: %#lx\n",
	       (unsigned long)dpni_attr.options);
	print_dpni_options(dpni_attr.options);

	printf("num_queues: %u\n", (uint32_t)dpni_attr.num_queues);
	printf("num_cgs: %u\n", (uint32_t)dpni_attr.num_cgs);
	printf("num_rx_tcs: %u\n", (uint32_t)dpni_attr.num_rx_tcs);
	printf("num_tx_tcs: %u\n", (uint32_t)dpni_attr.num_tx_tcs);
	printf("mac_entries: %u\n",
	       (uint32_t)dpni_attr.mac_filter_entries);
	printf("vlan_entries: %u\n",
	       (uint32_t)dpni_attr.vlan_filter_entries);
	printf("qos_entries: %u\n", (uint32_t)dpni_attr.qos_entries);
	printf("fs_entries: %u\n", (uint32_t)dpni_attr.fs_entries);
	printf("qos_key_size: %u\n", (uint32_t)dpni_attr.qos_key_size);
	printf("fs_key_size: %u\n", (uint32_t)dpni_attr.fs_key_size);
	printf("num_channels: %u\n", (uint32_t)dpni_attr.num_ceetm_ch);
	printf("num_opr: %u\n", (uint32_t)dpni_attr.num_opr);

	for (page = 0; page < 7; page++) {
		memset(&dpni_stats, 0, sizeof(dpni_stats));

		switch (page) {
		case 3:
			for (ch = 0; ch < dpni_attr.num_ceetm_ch; ch++) {
				int tc;

				for (tc = 0; tc < dpni_attr.num_tx_tcs; tc++) {
					uint16_t param = (ch << 8) | tc;

					memset(&dpni_stats, 0, sizeof(dpni_stats));
					error = dpni_get_statistics(&restool.mc_io, 0,
								    dpni_handle, page, param,
								    &dpni_stats);
					if (!error) {
						printf("+ CEETM stats Tx channel %d, TC %d\n", ch, tc);
						dpni_print_stats(dpni_stats_strings[page], dpni_stats);
					}
				}
			}
			break;
		case 4:
			if (dpni_attr.options & DPNI_OPT_CUSTOM_CG) {
				for (q = 0; q < dpni_attr.num_queues; q++) {
					for (tc = 0; tc < dpni_attr.num_rx_tcs; tc++) {
						uint16_t param = (q << 8 ) | tc;

						memset(&dpni_stats, 0, sizeof(dpni_stats));
						error = dpni_get_statistics(&restool.mc_io, 0, dpni_handle,
									    page, param, &dpni_stats);
						if (!error) {
							printf("+ Congestion stats for Queue %d, Rx TC %d\n", q, tc);
							dpni_print_stats(dpni_stats_strings[page], dpni_stats);
						}
					}
				}
			} else {
				for (tc = 0; tc < dpni_attr.num_rx_tcs; tc++) {
					uint16_t param = tc;

					memset(&dpni_stats, 0, sizeof(dpni_stats));
					error = dpni_get_statistics(&restool.mc_io, 0, dpni_handle,
								    page, param, &dpni_stats);
					if (!error) {
						printf("+ Congestion stats for Rx TC %d\n", tc);
						dpni_print_stats(dpni_stats_strings[page], dpni_stats);
					}
				}
			}

			break;
		case 5:
			for (tc = 0; tc < dpni_attr.num_rx_tcs; tc++) {
				uint16_t param = tc;

				memset(&dpni_stats, 0, sizeof(dpni_stats));
				error = dpni_get_statistics(&restool.mc_io, 0, dpni_handle,
							    page, param, &dpni_stats);
				if (!error) {
					printf("+ Policer stats for TC %d\n", tc);
					dpni_print_stats(dpni_stats_strings[page], dpni_stats);
				}
			}

			break;
		default:
			error = dpni_get_statistics(&restool.mc_io, 0,
						    dpni_handle, page,
						    0, &dpni_stats);
			dpni_print_stats(dpni_stats_strings[page], dpni_stats);
			break;
		}
	}

	print_obj_label(target_obj_desc);

out:
	if (dpni_opened) {

		error2 = dpni_close(&restool.mc_io, 0, dpni_handle);
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

static int cmd_dpni_info(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpni info <dpni-object> [--verbose]\n"
		"\n"
		"OPTIONS:\n"
		"--verbose\n"
		"   Shows extended/verbose information about the object\n"
		"\n"
		"EXAMPLE:\n"
		"Display information about dpni.5:\n"
		"   $ restool dpni info dpni.5\n"
		"\n";
	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	bool found = false;
	uint32_t dpni_id;
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

	error = parse_object_name(restool.obj_name, "dpni", &dpni_id);
	if (error < 0)
		goto out;

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
				restool.root_dprc_handle, 0, dpni_id,
				"dpni", &target_obj_desc,
				&target_parent_dprc_id, &found);
	if (error < 0)
		goto out;

	if (strcmp(target_obj_desc.type, "dpni")) {
		printf("dpni.%d does not exist\n", dpni_id);
		return -EINVAL;
	}

	error = print_dpni_attr(dpni_id, &target_obj_desc);
	if (error < 0)
		goto out;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		error = print_obj_verbose(&target_obj_desc, &dpni_ops);
	}

out:
	return error;
}

static int parse_dpni_mac_addr(char *mac_addr_str, uint8_t *mac_addr)
{
	char *cursor = NULL;
	char *endptr;
	char *mac_str = strtok_r(mac_addr_str, ":", &cursor);
	long val;
	int i = 0;

	while (mac_str != NULL) {
		if (i >= 6) { /* mac address is more than 6 parts */
			ERROR_PRINTF("Invalid MAC address.\n");
			ERROR_PRINTF(
				"Please enter 48 bits MAC address, eg. 00:0e:0c:55:12:03\n");
			return -EINVAL;
		}

		errno = 0;
		val = strtol(mac_str, &endptr, 16);

		if (STRTOL_ERROR(mac_str, endptr, val, errno) ||
		    (val < 0 || val > UINT8_MAX)) {
			ERROR_PRINTF("Invalid MAC address.\n");
			ERROR_PRINTF(
				"Please enter 48 bits MAC address, eg. 00:0e:0c:55:12:03\n");
			return -EINVAL;
		}

		mac_addr[i] = (uint8_t)val;
		mac_str = strtok_r(NULL, ":", &cursor);
		++i;
	}

	if (6 != i) { /* mac address is less than 6 parts */
		ERROR_PRINTF("Invalid MAC address.\n");
		ERROR_PRINTF(
			"Please enter 48 bits MAC address, eg. 00:0e:0c:55:12:03\n");
		return -EINVAL;
	}

	return 0;
}

static int create_dpni(const char *usage_msg)
{
	struct dpni_cfg dpni_cfg;
	uint32_t dpni_id, dprc_id;
	uint64_t dpni_cfg_options;
	uint16_t dprc_handle;
	bool dprc_opened;
	long value;
	int error;

	memset(&dpni_cfg, 0, sizeof(dpni_cfg));

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

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_MAC_ENTRIES) &&
	    restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_MAC_FILTER_ENTRIES)) {
		ERROR_PRINTF("Please only use on of the following options " \
			     "since they are interchangeable:" \
			     " --mac-entries, --mac-filter-entries.\n");
		return -EINVAL;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_VLAN_ENTRIES) &&
	    restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_VLAN_FILTER_ENTRIES)) {
		ERROR_PRINTF("Please only use on of the following options " \
			     "since they are interchangeable:" \
			     " --vlan-entries, --vlan-filter-entries.\n");
		return -EINVAL;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_OPTIONS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_OPTIONS);

		error = parse_generic_create_options(
				restool.cmd_option_args[CREATE_OPT_OPTIONS],
				&dpni_cfg_options,
				options_map, options_num);

		dpni_cfg.options = (uint32_t)dpni_cfg_options;
		if (dpni_cfg_options > UINT32_MAX)
			DEBUG_PRINTF(
				"parse_generic_create_options() produces overflow while getting options-mask\n");

		if (error) {
			DEBUG_PRINTF("parse_generic_create_options() = %d\n",
				     error);
			return error;
		}
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_NUM_QUEUES)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_NUM_QUEUES);
		error = get_option_value(CREATE_OPT_NUM_QUEUES, &value,
				     "Invalid num-queues value\n", 1, 32);
		if (error)
			return error;
		dpni_cfg.num_queues = (uint8_t)value;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_NUM_CHANNELS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_NUM_CHANNELS);
		error = get_option_value(CREATE_OPT_NUM_CHANNELS, &value,
				     "Invalid num-channels value\n", 1, 32);
		if (error)
			return error;

		dpni_cfg.num_ceetm_ch = (uint8_t)value;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_NUM_TCS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_NUM_TCS);
		error = get_option_value(CREATE_OPT_NUM_TCS, &value,
				     "Invalid num-tcs value\n", 1, 16);
		if (error)
			return error;
		dpni_cfg.num_tcs = (uint8_t)value;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_NUM_OPR)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_NUM_OPR);
		error = get_option_value(CREATE_OPT_NUM_OPR, &value,
					"Invalid num-opr value\n", 1,
					DPNI_MAX_OPR);
		if (error)
			return error;

		dpni_cfg.num_opr = (uint16_t)value;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_MAC_ENTRIES)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_MAC_ENTRIES);
		error = get_option_value(CREATE_OPT_MAC_ENTRIES, &value,
				     "Invalid mac-entries value\n", 1, 80);
		if (error)
			return error;
		dpni_cfg.mac_filter_entries = (uint8_t)value;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_MAC_FILTER_ENTRIES)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_MAC_FILTER_ENTRIES);
		error = get_option_value(CREATE_OPT_MAC_FILTER_ENTRIES, &value,
				     "Invalid mac-filter-entries value\n", 1, 80);
		if (error)
			return error;
		dpni_cfg.mac_filter_entries = (uint8_t)value;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_VLAN_ENTRIES)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_VLAN_ENTRIES);
		error = get_option_value(CREATE_OPT_VLAN_ENTRIES, &value,
				     "Invalid vlan-entries value\n", 1, 16);
		if (error)
			return error;
		dpni_cfg.vlan_filter_entries = (uint8_t)value;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_VLAN_FILTER_ENTRIES)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_VLAN_FILTER_ENTRIES);
		error = get_option_value(CREATE_OPT_VLAN_FILTER_ENTRIES, &value,
				     "Invalid vlan-filter-entries value\n", 1, 16);
		if (error)
			return error;
		dpni_cfg.vlan_filter_entries = (uint8_t)value;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_QOS_ENTRIES)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_QOS_ENTRIES);
		error = get_option_value(CREATE_OPT_QOS_ENTRIES, &value,
				     "Invalid qos-entries value\n", 1, 64);
		if (error)
			return error;
		dpni_cfg.qos_entries = (uint8_t)value;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_FS_ENTRIES)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_FS_ENTRIES);
		error = get_option_value(CREATE_OPT_FS_ENTRIES, &value,
				     "Invalid fs-entries value\n", 1, 1024);
		if (error)
			return error;
		dpni_cfg.fs_entries = (uint16_t)value;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_NUM_CGS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_NUM_CGS);
		error = get_option_value(CREATE_OPT_NUM_CGS, &value,
				     "Invalid num-cgs value\n", 1, 128);
		if (error)
			return error;
		dpni_cfg.num_cgs = (uint8_t)value;
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

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_DIST_KEY_SIZE)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_DIST_KEY_SIZE);
		error = get_option_value(CREATE_OPT_DIST_KEY_SIZE, &value,
				"Invalid dist-key-size value", 1, 56);
		if (error)
			return error;
		dpni_cfg.dist_key_size = (uint8_t)value;
	}

	error = dpni_create(&restool.mc_io, dprc_handle, 0, &dpni_cfg, &dpni_id);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			mc_status_to_string(mc_status), mc_status);
		return error;
	}

	if (dprc_opened) {
		(void)dprc_close(&restool.mc_io, 0, dprc_handle);
		print_new_obj("dpni", dpni_id,
			      restool.cmd_option_args[CREATE_OPT_PARENT_DPRC]);
	} else {
		print_new_obj("dpni", dpni_id, NULL);
	}

	return 0;
}
static int cmd_dpni_create(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpni create [OPTIONS]\n"
		"\n"
		"OPTIONS:\n"
		"--options=<options-mask>\n"
		"   Where <options-mask> is a comma or space separated list of DPNI options:\n"
		"	DPNI_OPT_TX_FRM_RELEASE\n"
		"	DPNI_OPT_NO_MAC_FILTER\n"
		"	DPNI_OPT_HAS_POLICING\n"
		"	DPNI_OPT_SHARED_CONGESTION\n"
		"	DPNI_OPT_HAS_KEY_MASKING\n"
		"	DPNI_OPT_NO_FS\n"
		"	DPNI_OPT_HAS_OPR\n"
		"	DPNI_OPT_OPR_PER_TC\n"
		"	DPNI_OPT_SINGLE_SENDER\n"
		"	DPNI_OPT_CUSTOM_CG\n"
		"	DPNI_OPT_CUSTOM_OPR\n"
		"	DPNI_OPT_SHARED_HASH_KEY\n"
		"	DPNI_OPT_SHARED_FS\n"
		"	DPNI_OPT_STASHING_DIS\n"
		"--num-queues=<number>\n"
		"   Number of TX/RX queues use for traffic distribution.\n"
		"   Used to distribute traffic to multiple GPP cores,\n"
		"   Defaults to one queue. Maximim supported value is 32\n"
		"--num-tcs=<number>\n"
		"   Number of traffic classes (TCs), reserved for the DPNI.\n"
		"   Defaults to one TC. Maximum supported value is 16.\n"
		"   There are maximum 16 TCs for Tx and 8 TCs for Rx.\n"
		"   When num_tcs > 8, Tx will use this value but Rx will have\n"
		"   only 8 traffic classes.\n"
		"--mac-filter-entries=<number>\n"
		"   Number of entries in the MAC address filtering table.\n"
		"   Allows both unicast and multicast entries.\n"
		"   By default, there are 80 entries.Maximum supported value is 80.\n"
		"   This option is the former --mac-entries which can still be used.\n"
		"   The change was made to align with the options passed through DPL.\n"
		"--vlan-filter-entries=<number>\n"
		"   Number of entries in the VLAN address filtering table\n"
		"   By default, VLAN filtering is disabled. Maximum values is 16\n"
		"   This option is the former --vlan-entries which can still be used.\n"
		"   The change was made to align with the options passed through DPL.\n"
		"--qos-entries=<number>\n"
		"   Number of entries in the QoS classification table.\n"
		"   Ignored of DPNI has a single TC. By default, set to 64.\n"
		"--fs-entries=<number>\n"
		"   Number of entries in the flow steering table.\n"
		"   Defaults to 64. Maximum value is 1024\n"
		"--num-cgs=<number>\n"
		"   Number of congestion groups (CGs), reserved for the DPNI.\n"
		"   Defaults to one per TC. Maximum supported value is 128.\n"
		"--container=<container-name>\n"
		"   Specifies the parent container name. e.g. dprc.2, dprc.3 etc.\n"
		"--dist-key-size=<number>\n"
		"   maximum key size for the distribution;\n"
		"   '0' will be treated as '24' which enough for IPv4 5-tuple\n"
		"--num-channels=<number>\n"
		"   Number of egress channels used by this dpni object.\n"
		"   If not specified the dpni object will use a single CEETM channel\n"
		"--num-opr=<number>\n"
		"   Number of desired custom number of order point records when\n"
		"   DPNI_OPT_CUSTOM_OPR is set.\n"
		"   Maximum supported value is num_tcs * num_queues.\n"
		"   Defaults to num_tcs * num_queues.\n"
		"\n";

	return create_dpni(usage_msg);
}

static int destroy_dpni(uint32_t dpni_id)
{
	uint16_t dprc_handle;
	uint32_t dprc_id;
	int error;

	dprc_handle = restool.root_dprc_handle;
	dprc_id = restool.root_dprc_id;
	error = get_parent_dprc_id(dpni_id, "dpni", &dprc_id);
	if (error)
		return error;

	if (dprc_id != restool.root_dprc_id) {
		error = open_dprc(dprc_id, &dprc_handle);
		if (error) {
			mc_status = flib_error_to_mc_status(error);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status), mc_status);
			return error;
		}
	}

	error = dpni_destroy(&restool.mc_io, dprc_handle, 0, dpni_id);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	printf("dpni.%u is destroyed\n", dpni_id);

out:
	if (dprc_id != restool.root_dprc_id) {
		error = dprc_close(&restool.mc_io, 0, dprc_handle);
		if (error) {
			mc_status = flib_error_to_mc_status(error);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status), mc_status);
		}
	}

	return error;
}

static int cmd_dpni_destroy(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpni destroy <dpni-object>\n"
		"   e.g. restool dpni destroy dpni.9\n"
		"\n";

	int error;
	uint32_t dpni_id;

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

	error = parse_object_name(restool.obj_name, "dpni", &dpni_id);
	if (error < 0)
		goto out;

	if (!find_obj("dpni", dpni_id)) {
		error = -EINVAL;
		goto out;
	}

	error = destroy_dpni(dpni_id);
out:
	return error;

}

static int update_dpni(const char *usage_msg)
{
	struct dpni_attr dpni_attr = {0};
	uint8_t mac_addr[6];
	uint16_t dpni_handle;
	uint32_t dpni_id;
	int error = 0;
	bool minimal_options = false;

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_HELP)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_HELP);
		return 0;
	}

	if (restool.obj_name == NULL) {
		ERROR_PRINTF("Should provide a dpni!\n");
		puts(usage_msg);
		return -EINVAL;
	}

	error = parse_object_name(restool.obj_name, "dpni", &dpni_id);
	if (error) {
		puts(usage_msg);
		goto out;
	}

	error = dpni_open(&restool.mc_io, 0, dpni_id, &dpni_handle);
	if (error) {
		ERROR_PRINTF("Could not open specified object!\n");
		puts(usage_msg);
		goto out;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(UPDATE_MAC_ADDR)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(UPDATE_MAC_ADDR);
		error  = parse_dpni_mac_addr(
				restool.cmd_option_args[UPDATE_MAC_ADDR],
				mac_addr);

		minimal_options = true;

		error = dpni_get_attributes(&restool.mc_io, 0, dpni_handle, &dpni_attr);
		if (error < 0) {
			ERROR_PRINTF("dpni_get_attributes() = %d\n", error);
			goto dpni_close;
		}

		if (dpni_attr.options & DPNI_OPT_NO_MAC_FILTER) {
			ERROR_PRINTF("Cannot set MAC address when DPNI_OPT_NO_MAC_FILTER is set!\n");
			goto dpni_close;
		}

		error = dpni_set_primary_mac_addr(&restool.mc_io, 0, dpni_handle, mac_addr);
		if (error)
			ERROR_PRINTF("dpni_set_primary_mac_addr() = %d\n", error);
	}

dpni_close:
	dpni_close(&restool.mc_io, 0, dpni_handle);
out:

	if (!minimal_options && !error) {
		ERROR_PRINTF("Specify at least an option to update!\n");
		puts(usage_msg);
	}

	return error;
}


static int cmd_dpni_update(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpni update dpni.X [OPTIONS]\n"
		"\n"
		"OPTIONS:\n"
		"   --mac-addr=<addr>\n"
		"	String specifying primary MAC address (e.g. 00:00:05:00:00:05).\n"
		"\n";

	return update_dpni(usage_msg);
}

struct object_command dpni_commands_v10[] = {
	{ .cmd_name = "--help",
	  .options = NULL,
	  .cmd_func = cmd_dpni_help },

	{ .cmd_name = "info",
	  .options = dpni_info_options,
	  .cmd_func = cmd_dpni_info },

	{ .cmd_name = "create",
	  .options = dpni_create_options,
	  .cmd_func = cmd_dpni_create },

	{ .cmd_name = "destroy",
	  .options = dpni_destroy_options,
	  .cmd_func = cmd_dpni_destroy },

	{ .cmd_name = "update",
	  .options = dpni_update_options,
	  .cmd_func = cmd_dpni_update },

	{ .cmd_name = NULL },
};

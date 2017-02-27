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
#include "mc_v9/fsl_dpni.h"
#include "mc_v10/fsl_dpni.h"

#define ALL_DPNI_OPTS (					\
	DPNI_OPT_ALLOW_DIST_KEY_PER_TC |		\
	DPNI_OPT_TX_CONF_DISABLED |			\
	DPNI_OPT_PRIVATE_TX_CONF_ERROR_DISABLED |	\
	DPNI_OPT_DIST_HASH |				\
	DPNI_OPT_DIST_FS |				\
	DPNI_OPT_UNICAST_FILTER	|			\
	DPNI_OPT_MULTICAST_FILTER |			\
	DPNI_OPT_VLAN_FILTER |				\
	DPNI_OPT_IPR |					\
	DPNI_OPT_IPF |					\
	DPNI_OPT_VLAN_MANIPULATION |			\
	DPNI_OPT_QOS_MASK_SUPPORT |			\
	DPNI_OPT_FS_MASK_SUPPORT)


#define ALL_DPNI_OPTS_V10 (				\
	DPNI_OPT_TX_FRM_RELEASE |			\
	DPNI_OPT_NO_MAC_FILTER |			\
	DPNI_OPT_HAS_POLICING |				\
	DPNI_OPT_SHARED_CONGESTION |			\
	DPNI_OPT_HAS_KEY_MASKING |			\
	DPNI_OPT_NO_FS |				\
	DPNI_OPT_HAS_OPR |				\
	DPNI_OPT_OPR_PER_TC)

enum mc_cmd_status mc_status;

/**
 * max_dist: Maximum distribution size for Rx traffic class;
 * supported values: 1,2,3,4,6,7,8,12,14,16,24,28,32,48,56,64,96,
 * 112,128,192,224,256,384,448,512,768,896,1024;
 */
#define MAX_DIST_SIZE	1024

struct dpni_config {
	struct dpni_extended_cfg dpni_extended_cfg;
	struct dpni_cfg_v9 dpni_cfg;
	struct dpni_attr_v9 dpni_attr;
	uint16_t dpni_handle;
};


#define RESTOOL_DPNI_CREATE \
	_IOWR(RESTOOL_IOCTL_TYPE, 0x5, struct dpni_config)

#define RESTOOL_DPNI_GET_ATTRIBUTES \
	_IOWR(RESTOOL_IOCTL_TYPE, 0x6, struct dpni_config)


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

enum dpni_update_options_v10 {
	UPDATE_OPT_HELP = 0,
	UPDATE_MAC_ADDR,
};

static struct option dpni_update_options_v10[] = {
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

C_ASSERT(ARRAY_SIZE(dpni_update_options_v10) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static const struct flib_ops dpni_ops = {
	.obj_open = dpni_open,
	.obj_close = dpni_close,
	.obj_get_irq_mask = dpni_get_irq_mask,
	.obj_get_irq_status = dpni_get_irq_status,
};

static const struct flib_ops dpni_ops_v9 = {
	.obj_open = dpni_open,
	.obj_close = dpni_close,
	.obj_get_irq_mask = dpni_get_irq_mask,
	.obj_get_irq_status = dpni_get_irq_status_v9,
};

static struct option_entry options_map_v9[] = {
	OPTION_MAP_ENTRY(DPNI_OPT_ALLOW_DIST_KEY_PER_TC),
	OPTION_MAP_ENTRY(DPNI_OPT_TX_CONF_DISABLED),
	OPTION_MAP_ENTRY(DPNI_OPT_PRIVATE_TX_CONF_ERROR_DISABLED),
	OPTION_MAP_ENTRY(DPNI_OPT_DIST_HASH),
	OPTION_MAP_ENTRY(DPNI_OPT_DIST_FS),
	OPTION_MAP_ENTRY(DPNI_OPT_UNICAST_FILTER),
	OPTION_MAP_ENTRY(DPNI_OPT_MULTICAST_FILTER),
	OPTION_MAP_ENTRY(DPNI_OPT_VLAN_FILTER),
	OPTION_MAP_ENTRY(DPNI_OPT_IPR),
	OPTION_MAP_ENTRY(DPNI_OPT_IPF),
	OPTION_MAP_ENTRY(DPNI_OPT_VLAN_MANIPULATION),
	OPTION_MAP_ENTRY(DPNI_OPT_QOS_MASK_SUPPORT),
	OPTION_MAP_ENTRY(DPNI_OPT_FS_MASK_SUPPORT),
};
static unsigned int options_num_v9 = ARRAY_SIZE(options_map_v9);

static struct option_entry options_map_v10_0[] = {
	OPTION_MAP_ENTRY(DPNI_OPT_TX_FRM_RELEASE),
	OPTION_MAP_ENTRY(DPNI_OPT_NO_MAC_FILTER),
	OPTION_MAP_ENTRY(DPNI_OPT_HAS_POLICING),
	OPTION_MAP_ENTRY(DPNI_OPT_SHARED_CONGESTION),
	OPTION_MAP_ENTRY(DPNI_OPT_HAS_KEY_MASKING),
	OPTION_MAP_ENTRY(DPNI_OPT_NO_FS),
};
static unsigned int options_num_v10_0 = ARRAY_SIZE(options_map_v10_0);

static struct option_entry options_map_v10_1[] = {
	OPTION_MAP_ENTRY(DPNI_OPT_TX_FRM_RELEASE),
	OPTION_MAP_ENTRY(DPNI_OPT_NO_MAC_FILTER),
	OPTION_MAP_ENTRY(DPNI_OPT_HAS_POLICING),
	OPTION_MAP_ENTRY(DPNI_OPT_SHARED_CONGESTION),
	OPTION_MAP_ENTRY(DPNI_OPT_HAS_KEY_MASKING),
	OPTION_MAP_ENTRY(DPNI_OPT_NO_FS),
	OPTION_MAP_ENTRY(DPNI_OPT_HAS_OPR),
	OPTION_MAP_ENTRY(DPNI_OPT_OPR_PER_TC),
};
static unsigned int options_num_v10_1 = ARRAY_SIZE(options_map_v10_1);

#define DPNI_STATS_PER_PAGE_V10 6

static const char *dpni_stats_v10[][DPNI_STATS_PER_PAGE_V10] = {
	{
	"ingress_all_frames",
	"ingress_all_bytes",
	"ingress_multicast_frames",
	"ingress_multicast_bytes",
	"ingress_broadcast_frames",
	"ingress_broadcast_bytes",
	}, {
	"egress_all_frames",
	"egress_all_bytes",
	"egress_multicast_frames",
	"egress_multicast_bytes",
	"egress_broadcast_frames",
	"egress_broadcast_bytes",
	}, {
	"ingress_filtered_frames",
	"ingress_discarded_frames",
	"ingress_nobuffer_discards",
	"egress_discarded_frames",
	"egress_confirmed_frames",
	""
	},
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
		"\n"
		"For command-specific help, use the --help option of each command.\n"
		"\n";

	printf(help_msg);
	return 0;
}

static int cmd_dpni_help_v10(void)
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

	if (options & DPNI_OPT_ALLOW_DIST_KEY_PER_TC)
		printf("\tDPNI_OPT_ALLOW_DIST_KEY_PER_TC\n");

	if (options & DPNI_OPT_TX_CONF_DISABLED)
		printf("\tDPNI_OPT_TX_CONF_DISABLED\n");

	if (options & DPNI_OPT_PRIVATE_TX_CONF_ERROR_DISABLED)
		printf("\tDPNI_OPT_PRIVATE_TX_CONF_ERROR_DISABLED\n");

	if (options & DPNI_OPT_DIST_HASH)
		printf("\tDPNI_OPT_DIST_HASH\n");

	if (options & DPNI_OPT_DIST_FS)
		printf("\tDPNI_OPT_DIST_FS\n");

	if (options & DPNI_OPT_UNICAST_FILTER)
		printf("\tDPNI_OPT_UNICAST_FILTER\n");

	if (options & DPNI_OPT_MULTICAST_FILTER)
		printf("\tDPNI_OPT_MULTICAST_FILTER\n");

	if (options & DPNI_OPT_VLAN_FILTER)
		printf("\tDPNI_OPT_VLAN_FILTER\n");

	if (options & DPNI_OPT_IPR)
		printf("\tDPNI_OPT_IPR\n");

	if (options & DPNI_OPT_IPF)
		printf("\tDPNI_OPT_IPF\n");

	if (options & DPNI_OPT_VLAN_MANIPULATION)
		printf("\tDPNI_OPT_VLAN_MANIPULATION\n");

	if (options & DPNI_OPT_QOS_MASK_SUPPORT)
		printf("\tDPNI_OPT_QOS_MASK_SUPPORT\n");

	if (options & DPNI_OPT_FS_MASK_SUPPORT)
		printf("\tDPNI_OPT_FS_MASK_SUPPORT\n");
}

static void print_dpni_options_v10(uint32_t options)
{
	if ((options & ~ALL_DPNI_OPTS_V10) != 0) {
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

static int print_dpni_attr_v9(uint32_t dpni_id,
			      struct dprc_obj_desc *target_obj_desc)
{
	uint16_t dpni_handle;
	int error;
	int i;
	struct dpni_attr_v9 dpni_attr;
	struct dpni_extended_cfg ext_cfg;
	uint8_t mac_addr[6];
	bool dpni_opened = false;
	struct dpni_link_state link_state;

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

	error = dpni_get_attributes_v9(&restool.mc_io, 0, dpni_handle,
				       &dpni_attr, &ext_cfg);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	assert(dpni_id == (uint32_t)dpni_attr.id);
	assert(DPNI_MAX_TC >= dpni_attr.max_tcs);

	error = dpni_get_primary_mac_addr(&restool.mc_io, 0,
					dpni_handle, mac_addr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	memset(&link_state, 0, sizeof(link_state));
	error = dpni_get_link_state(&restool.mc_io, 0, dpni_handle,
					&link_state);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	printf("dpni version: %u.%u\n", dpni_attr.version.major,
	       dpni_attr.version.minor);
	printf("dpni id: %d\n", dpni_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	print_dpni_endpoint(dpni_id);
	printf("link status: %d - ", link_state.up);
	link_state.up == 0 ? printf("down\n") :
	link_state.up == 1 ? printf("up\n") : printf("error state\n");
	print_mac_address(mac_addr);
	printf("dpni_attr.options value is: %#lx\n",
	       (unsigned long)dpni_attr.options);
	print_dpni_options(dpni_attr.options);
	printf("max senders: %u\n", (uint32_t)dpni_attr.max_senders);
	printf("max traffic classes: %u\n", (uint32_t)dpni_attr.max_tcs);
	for (i = 0; i < dpni_attr.max_tcs; i++)
		printf("\ttc[%d]: max_dist=%d, max_fs_entries=%d\n",
			i,
			ext_cfg.tc_cfg[i].max_dist,
			dpni_attr.options & DPNI_OPT_DIST_FS ?
				ext_cfg.tc_cfg[i].max_fs_entries : 0);
	printf("max unicast filters: %u\n",
	       (uint32_t)dpni_attr.max_unicast_filters);
	printf("max multicast filters: %u\n",
	       (uint32_t)dpni_attr.max_multicast_filters);
	printf("max vlan filters: %u\n", (uint32_t)dpni_attr.max_vlan_filters);
	printf("max QoS entries: %u\n", (uint32_t)dpni_attr.max_qos_entries);
	printf("max QoS key size: %u\n", (uint32_t)dpni_attr.max_qos_key_size);
	printf("max distribution key size: %u\n",
	       (uint32_t)dpni_attr.max_dist_key_size);
	printf("max policers: %u\n", (uint32_t)dpni_attr.max_policers);
	printf("max congestion control: %u\n",
		(uint32_t)dpni_attr.max_congestion_ctrl);

	printf("max_dist per RX traffic class:\n");
	for (int k = 0; k < dpni_attr.max_tcs; ++k)
		printf("\tclass %d's max_dist: %u\n", k,
		       (uint32_t)ext_cfg.tc_cfg[k].max_dist);

	printf("max_fs_entries per RX traffic class:\n");
	for (int m = 0; m < dpni_attr.max_tcs; ++m)
		printf("\tclass %d's max_fs_entries: %u\n", m,
		       (uint32_t)ext_cfg.tc_cfg[m].max_fs_entries);

	printf("max_reass_frm_size: %u\n",
		(uint32_t)ext_cfg.ipr_cfg.max_reass_frm_size);
	printf("min_frag_size_ipv4: %u\n",
		(uint32_t)ext_cfg.ipr_cfg.min_frag_size_ipv4);
	printf("min_frag_size_ipv6: %u\n",
		(uint32_t)ext_cfg.ipr_cfg.min_frag_size_ipv6);
	printf("max_open_frames_ipv4: %u\n",
		(uint32_t)ext_cfg.ipr_cfg.max_open_frames_ipv4);
	printf("max_open_frames_ipv6: %u\n",
		(uint32_t)ext_cfg.ipr_cfg.max_open_frames_ipv6);

	print_obj_label(target_obj_desc);

	error = 0;

out:
	if (dpni_opened) {
		int error2;

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

static void dpni_print_stats(const char *strings[],
			     union dpni_statistics_v10 dpni_stats)
{
	uint64_t *stat;
	int i;

	stat = (uint64_t *)&dpni_stats.raw;
	for (i = 0; i < DPNI_STATS_PER_PAGE_V10; i++) {
		if (strcmp(strings[i], "\0") == 0)
			break;
		printf("%s: %lu\n", strings[i], *stat);
		stat++;
	}
}

static int print_dpni_attr_v10(uint32_t dpni_id,
			      struct dprc_obj_desc *target_obj_desc)
{
	struct dpni_attr_v10 dpni_attr;
	union dpni_statistics_v10 dpni_stats;
	uint16_t dpni_handle, dpni_major, dpni_minor;
	struct dpni_link_state link_state;
	bool dpni_opened = false;
	uint8_t mac_addr[6];
	int error = 0;
	int error2;
	unsigned int page;

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
	error = dpni_get_attributes_v10(&restool.mc_io, 0,
					dpni_handle, &dpni_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	error = dpni_get_version_v10(&restool.mc_io, 0,
				     &dpni_major, &dpni_minor);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	error = dpni_get_primary_mac_addr(&restool.mc_io, 0,
					dpni_handle, mac_addr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	memset(&link_state, 0, sizeof(link_state));
	error = dpni_get_link_state(&restool.mc_io, 0, dpni_handle,
				    &link_state);
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

	print_mac_address(mac_addr);

	printf("dpni_attr.options value is: %#lx\n",
	       (unsigned long)dpni_attr.options);
	print_dpni_options_v10(dpni_attr.options);

	printf("num_queues: %u\n", (uint32_t)dpni_attr.num_queues);
	printf("num_tcs: %u\n", (uint32_t)dpni_attr.num_tcs);
	printf("mac_entries: %u\n",
	       (uint32_t)dpni_attr.mac_filter_entries);
	printf("vlan_entries: %u\n",
	       (uint32_t)dpni_attr.vlan_filter_entries);
	printf("qos_entries: %u\n", (uint32_t)dpni_attr.qos_entries);
	printf("fs_entries: %u\n", (uint32_t)dpni_attr.fs_entries);
	printf("qos_key_size: %u\n", (uint32_t)dpni_attr.qos_key_size);
	printf("fs_key_size: %u\n", (uint32_t)dpni_attr.fs_key_size);

	for (page = 0; page < 3; page++) {
		error = dpni_get_statistics_v10(&restool.mc_io, 0,
						dpni_handle, page, &dpni_stats);
		dpni_print_stats(dpni_stats_v10[page], dpni_stats);
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

static int print_dpni_info(uint32_t dpni_id, int mc_fw_version)
{
	int error;
	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	bool found = false;

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

	if (mc_fw_version == MC_FW_VERSION_9)
		error = print_dpni_attr_v9(dpni_id, &target_obj_desc);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = print_dpni_attr_v10(dpni_id, &target_obj_desc);
	if (error < 0)
		goto out;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		error = print_obj_verbose(&target_obj_desc, &dpni_ops);
	}

out:
	return error;
}

static int info_dpni(int mc_fw_version)
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

	error = parse_object_name(restool.obj_name, "dpni", &obj_id);
	if (error < 0)
		goto out;

	error = print_dpni_info(obj_id, mc_fw_version);

out:
	return error;
}

static int cmd_dpni_info_v9(void)
{
	return info_dpni(MC_FW_VERSION_9);
}

static int cmd_dpni_info_v10(void)
{
	return info_dpni(MC_FW_VERSION_10);
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

static int parse_dpni_max_dist_per_tc_v9(char *max_dist_per_tc_str,
	struct dpni_extended_cfg *dpni_extended_cfg, uint8_t max_tcs)
{
	char *cursor = NULL;
	char *endptr;
	char *max_dist_str = strtok_r(max_dist_per_tc_str, ",", &cursor);
	int i = 0;
	long val;

	while (max_dist_str != NULL) {
		if (i >= DPNI_MAX_TC) {
			ERROR_PRINTF("Invalid max-dist-per-tc\n");
			ERROR_PRINTF("maximum number of traffic class <= %d\n",
			     DPNI_MAX_TC);
			return -EINVAL;
		}

		errno = 0;
		val = strtol(max_dist_str, &endptr, 0);

		if (STRTOL_ERROR(max_dist_str, endptr, val, errno) ||
		    (val < 0 || val > MAX_DIST_SIZE)) {
			ERROR_PRINTF("Invalid dist-size.\n");
			return -EINVAL;
		}

		dpni_extended_cfg->tc_cfg[i].max_dist = (uint16_t)val;
		max_dist_str = strtok_r(NULL, ",", &cursor);
		++i;
	}

	if (max_tcs != i) {
		ERROR_PRINTF(
			"size of max_dist_per_tc does not match max_tcs: %u\n",
			(unsigned int)max_tcs);
		return -EINVAL;
	}

	return 0;
}

static int parse_dpni_max_fs_entries_per_tc(char *max_fs_entries_per_tc_str,
	struct dpni_extended_cfg *dpni_extended_cfg, uint8_t max_tcs)
{
	char *cursor = NULL;
	char *endptr;
	char *max_fs_entries_str =
			strtok_r(max_fs_entries_per_tc_str, ",", &cursor);
	int i = 0;
	long val;

	while (max_fs_entries_str != NULL) {
		if (i >= DPNI_MAX_TC) {
			ERROR_PRINTF("Invalid max-dist-per-tc\n");
			ERROR_PRINTF("maximum number of traffic class <= %d\n",
			     DPNI_MAX_TC);
			return -EINVAL;
		}

		errno = 0;
		val = strtol(max_fs_entries_str, &endptr, 0);

		if (STRTOL_ERROR(max_fs_entries_str, endptr, val, errno) ||
		    (val < 0 || val > MAX_DIST_SIZE)) {
			ERROR_PRINTF("Invalid dist-size.\n");
			return -EINVAL;
		}

		dpni_extended_cfg->tc_cfg[i].max_fs_entries = (uint16_t)val;
		max_fs_entries_str = strtok_r(NULL, ",", &cursor);
		++i;
	}

	if (max_tcs != i) {
		ERROR_PRINTF(
			"size of max_fs_entries_per_tc does not match max_tcs: %u\n",
			(unsigned int)max_tcs);
		return -EINVAL;
	}

	return 0;
}

static int create_dpni_v9(const char *usage_msg)
{
	struct dpni_extended_cfg dpni_extended_cfg;
	struct dpni_attr_v9 dpni_attr;
	struct dpni_cfg_v9 dpni_cfg;
	uint16_t dpni_handle;
	int error;
	long val;

	memset(&dpni_cfg, 0, sizeof(dpni_cfg));
	memset(&dpni_extended_cfg, 0, sizeof(dpni_extended_cfg));

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

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_OPTIONS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_OPTIONS);
		error = parse_generic_create_options(
				restool.cmd_option_args[CREATE_OPT_OPTIONS],
				(uint64_t *)&dpni_cfg.adv.options,
				options_map_v9,
				options_num_v9);
		if (error < 0) {
			DEBUG_PRINTF(
				"parse_generic_create_options() failed with error %d, cannot get options-mask\n",
				error);
			return error;
		}
	} else {
		dpni_cfg.adv.options = DPNI_OPT_UNICAST_FILTER |
				       DPNI_OPT_MULTICAST_FILTER;
	}

	if (!(restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_MAC_ADDR))) {
		ERROR_PRINTF("--mac-addr option missing\n");
		puts(usage_msg);
		return -EINVAL;
	}

	restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_MAC_ADDR);
	error  = parse_dpni_mac_addr(
			restool.cmd_option_args[CREATE_OPT_MAC_ADDR],
			dpni_cfg.mac_addr);
	if (error < 0) {
		DEBUG_PRINTF(
			"parse_dpni_mac_addr() failed with error %d, cannot get mac address\n",
			error);
		return error;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_MAX_TCS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_MAX_TCS);
		error = get_option_value(CREATE_OPT_MAX_TCS, &val,
					 "Invalid max tcs", 0, DPNI_MAX_TC);
		if (error)
			return error;
		dpni_cfg.adv.max_tcs = (uint8_t)val;
	} else {
		dpni_cfg.adv.max_tcs = 1;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_DIST_PER_TC)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_DIST_PER_TC);
		error = parse_dpni_max_dist_per_tc_v9(
			restool.cmd_option_args[CREATE_OPT_MAX_DIST_PER_TC],
			&dpni_extended_cfg,
			dpni_cfg.adv.max_tcs);
		if (error < 0) {
			DEBUG_PRINTF(
				"parse_dpni_max_dist_per_tc_v9() failed with error %d, cannot get maximum distribution's size per RX traffic-class\n",
				error);
			return error;
		}
	} else {
		for (int i = 0; i < dpni_cfg.adv.max_tcs; ++i)
			dpni_extended_cfg.tc_cfg[i].max_dist = 1;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_FS_ENTRIES_PER_TC)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_FS_ENTRIES_PER_TC);
		error = parse_dpni_max_fs_entries_per_tc(
		restool.cmd_option_args[CREATE_OPT_MAX_FS_ENTRIES_PER_TC],
			&dpni_extended_cfg,
			dpni_cfg.adv.max_tcs);
		if (error < 0) {
			DEBUG_PRINTF(
				"parse_dpni_max_fs_entries_per_tc() failed with error %d, cannot get max_fs_entries per RX traffic-class\n",
				error);
			return error;
		}
	} else {
		for (int i = 0; i < dpni_cfg.adv.max_tcs; ++i)
			dpni_extended_cfg.tc_cfg[i].max_fs_entries = 1;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_MAX_SENDERS)) {
		restool.cmd_option_mask &=
				~ONE_BIT_MASK(CREATE_OPT_MAX_SENDERS);
		error = get_option_value(CREATE_OPT_MAX_SENDERS, &val,
					 "Invalid max senders", 0, UINT8_MAX);
		if (error)
			return error;
		dpni_cfg.adv.max_senders = (uint8_t)val;
	} else {
		dpni_cfg.adv.max_senders = 1;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_UNICAST_FILTERS)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_UNICAST_FILTERS);
		error = get_option_value(CREATE_OPT_MAX_UNICAST_FILTERS, &val,
					 "Invalid max unicast filters", 0, UINT8_MAX);
		if (error)
			return error;
		dpni_cfg.adv.max_unicast_filters = (uint8_t)val;
	} else {
		dpni_cfg.adv.max_unicast_filters = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_MULTICAST_FILTERS)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_MULTICAST_FILTERS);
		error = get_option_value(CREATE_OPT_MAX_MULTICAST_FILTERS, &val,
					 "Invalid max multicast filters",
					 0, UINT8_MAX);
		if (error)
			return error;
		dpni_cfg.adv.max_multicast_filters = (uint8_t)val;
	} else {
		dpni_cfg.adv.max_multicast_filters = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_VLAN_FILTERS)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_VLAN_FILTERS);
		error = get_option_value(CREATE_OPT_MAX_VLAN_FILTERS, &val,
					 "Invalid max vlan filters",
					 0, UINT8_MAX);
		if (error)
			return error;
		dpni_cfg.adv.max_vlan_filters = (uint8_t)val;
	} else {
		dpni_cfg.adv.max_vlan_filters = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_QOS_ENTRIES)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_QOS_ENTRIES);
		error = get_option_value(CREATE_OPT_MAX_QOS_ENTRIES, &val,
					 "Invalid max qos", 0, UINT8_MAX);
		if (error)
			return error;
		dpni_cfg.adv.max_qos_entries = (uint8_t)val;
	} else {
		dpni_cfg.adv.max_qos_entries = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_QOS_KEY_SIZE)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_QOS_KEY_SIZE);
		error = get_option_value(CREATE_OPT_MAX_QOS_KEY_SIZE, &val,
					 "Invalid max qos key size", 0, UINT8_MAX);
		if (error)
			return error;
		dpni_cfg.adv.max_qos_key_size = (uint8_t)val;
	} else {
		dpni_cfg.adv.max_qos_key_size = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_DIST_KEY_SIZE)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_DIST_KEY_SIZE);
		error = get_option_value(CREATE_OPT_MAX_DIST_KEY_SIZE, &val,
					 "Invalid max dist key size",
					 0, UINT8_MAX);
		if (error)
			return error;
		dpni_cfg.adv.max_dist_key_size = (uint8_t)val;
	} else {
		dpni_cfg.adv.max_dist_key_size = 0;
	}

	/**
	 * hack to get get 0.8.x flibs to work with mc
	 */
	error = dpni_create_v9(&restool.mc_io, 0, &dpni_cfg, &dpni_extended_cfg,
			       &dpni_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			mc_status_to_string(mc_status), mc_status);
		return error;
	}


	memset(&dpni_attr, 0, sizeof(struct dpni_attr));
	error = dpni_get_attributes_v9(&restool.mc_io, 0, dpni_handle,
				       &dpni_attr, &dpni_extended_cfg);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	print_new_obj("dpni", dpni_attr.id, NULL);

	error = dpni_close(&restool.mc_io, 0, dpni_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	return 0;
}

static int cmd_dpni_create_v9(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpni create --mac-addr=<addr> [OPTIONS]\n"
		"   --mac-addr=<addr>\n"
		"	String specifying primary MAC address (e.g. 00:00:05:00:00:05).\n"
		"\n"
		"OPTIONS:\n"
		"--max-senders=<number>\n"
		"	maximum number of different senders;\n"
		"	will be used as the number of dedicated TX flows;\n"
		"	If it is not a power of two it will be rounded up to the next\n"
		"	power of two value.\n"
		"--options=<options-mask>\n"
		"   Where <options-mask> is a comma or space separated list of DPNI options:\n"
		"	DPNI_OPT_ALLOW_DIST_KEY_PER_TC\n"
		"	DPNI_OPT_TX_CONF_DISABLED\n"
		"	DPNI_OPT_PRIVATE_TX_CONF_ERROR_DISABLED\n"
		"	DPNI_OPT_DIST_HASH\n"
		"	DPNI_OPT_DIST_FS\n"
		"	DPNI_OPT_UNICAST_FILTER\n"
		"	DPNI_OPT_MULTICAST_FILTER\n"
		"	DPNI_OPT_VLAN_FILTER\n"
		"	DPNI_OPT_QOS_MASK_SUPPORT\n"
		"	DPNI_OPT_FS_MASK_SUPPORT\n"
		"--max-tcs=<number>\n"
		"	Specifies the maximum number of traffic-classes\n"
		"	0 will be treated as 1\n"
		"--max-dist-per-tc=<dist-size>,<dist-size>,...\n"
		"	Comma separated list of counts specifying the\n"
		"	maximum distribution's size per RX traffic-class\n"
		"--max-fs-entries-per-tc=<fs-entries>,<fs-entries>,...\n"
		"	Comma separated list of counts specifying the\n"
		"	maximum fs-entries per RX traffic-class\n"
		"--max-unicast-filters=<number>\n"
		"	maximum number of unicast filters (0 will be treated as 16)\n"
		"--max-multicast-filters=<number>\n"
		"	maximum number of multicast filters (0 will be treated as 64)\n"
		"--max-vlan-filters=<number>\n"
		"	maximum number of vlan filters (0 will be treated as 16)\n"
		"--max-qos-entries=<number>\n"
		"	if max_tcs > 1, declares the maximum entries for\n"
		"	the QoS table; '0' will be treated as '64'\n"
		"--max-qos-key-size=<number>\n"
		"	maximum key size for the QoS look-up;\n"
		"	'0' will be treated as '24' which enough for IPv4 5-tuple\n"
		"--max-dist-key-size=<number>\n"
		"	maximum key size for the distribution;\n"
		"	'0' will be treated as '24' which enough for IPv4 5-tuple\n"
		"\n"
		"EXAMPLE:\n"
		"Create a DPNI object with all default options:\n"
		"   $ restool dpni create --mac-addr=<addr>\n"
		"\n";

	return create_dpni_v9(usage_msg);
}

static int create_dpni_v10(const char *usage_msg)
{
	struct dpni_cfg_v10 dpni_cfg;
	uint32_t dpni_id, dprc_id;
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

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_OPTIONS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_OPTIONS);

		if (restool.mc_fw_version.minor == 0)
			error = parse_generic_create_options(
					restool.cmd_option_args[CREATE_OPT_OPTIONS],
					(uint64_t *)&dpni_cfg.options,
					options_map_v10_0,
					options_num_v10_0);
		else if (restool.mc_fw_version.minor == 1)
			error = parse_generic_create_options(
					restool.cmd_option_args[CREATE_OPT_OPTIONS],
					(uint64_t *)&dpni_cfg.options,
					options_map_v10_1,
					options_num_v10_1);

		if (error) {
			DEBUG_PRINTF("parse_generic_create_options() = %d\n",
				     error);
			return error;
		}
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_NUM_QUEUES)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_NUM_QUEUES);
		error = get_option_value(CREATE_OPT_NUM_QUEUES, &value,
				     "Invalid num-queues value\n", 1, 8);
		if (error)
			return error;
		dpni_cfg.num_queues = (uint8_t)value;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_NUM_TCS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_NUM_TCS);
		error = get_option_value(CREATE_OPT_NUM_TCS, &value,
				     "Invalid num-tcs value\n", 1, 8);
		if (error)
			return error;
		dpni_cfg.num_tcs = (uint8_t)value;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_MAC_ENTRIES)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_MAC_ENTRIES);
		error = get_option_value(CREATE_OPT_MAC_ENTRIES, &value,
				     "Invalid mac-entries value\n", 1, 80);
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

	error = dpni_create_v10(&restool.mc_io, dprc_handle, 0,
				&dpni_cfg, &dpni_id);
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
static int cmd_dpni_create_v10(void)
{
	static const char usage_msg_v10_0[] =
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
		"--num-queues=<number>\n"
		"   Number of TX/RX queues use for traffic distribution.\n"
		"   Used to distribute traffic to multiple GPP cores,\n"
		"   Defaults to one queue. Maximim supported value is 8\n"
		"--num-tcs=<number>\n"
		"   Number of traffic classes (TCs), reserved for the DPNI.\n"
		"   Defaults to one TC. Maximum supported value is 8\n"
		"--mac-entries=<number>\n"
		"   Number of entries in the MAC address filtering table.\n"
		"   Allows both unicast and multicast entries.\n"
		"   By default, there are 80 entries.Maximum supported value is 80.\n"
		"--vlan-entries=<number>\n"
		"   Number of entries in the VLAN address filtering table\n"
		"   By default, VLAN filtering is disabled. Maximum values is 16\n"
		"--qos-entries=<number>\n"
		"   Number of entries in the QoS classification table.\n"
		"   Ignored of DPNI has a single TC. By default, set to 64.\n"
		"--fs-entries=<number>\n"
		"   Number of entries in the flow steering table.\n"
		"   Defaults to 64. Maximum value is 1024\n"
		"--container=<container-name>\n"
		"   Specifies the parent container name. e.g. dprc.2, dprc.3 etc.\n"
		"\n";

	static const char usage_msg_v10_1[] =
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
		"--num-queues=<number>\n"
		"   Number of TX/RX queues use for traffic distribution.\n"
		"   Used to distribute traffic to multiple GPP cores,\n"
		"   Defaults to one queue. Maximim supported value is 8\n"
		"--num-tcs=<number>\n"
		"   Number of traffic classes (TCs), reserved for the DPNI.\n"
		"   Defaults to one TC. Maximum supported value is 8\n"
		"--mac-entries=<number>\n"
		"   Number of entries in the MAC address filtering table.\n"
		"   Allows both unicast and multicast entries.\n"
		"   By default, there are 80 entries.Maximum supported value is 80.\n"
		"--vlan-entries=<number>\n"
		"   Number of entries in the VLAN address filtering table\n"
		"   By default, VLAN filtering is disabled. Maximum values is 16\n"
		"--qos-entries=<number>\n"
		"   Number of entries in the QoS classification table.\n"
		"   Ignored of DPNI has a single TC. By default, set to 64.\n"
		"--fs-entries=<number>\n"
		"   Number of entries in the flow steering table.\n"
		"   Defaults to 64. Maximum value is 1024\n"
		"--container=<container-name>\n"
		"   Specifies the parent container name. e.g. dprc.2, dprc.3 etc.\n"
		"\n";

	if (restool.mc_fw_version.minor == 0)
		return create_dpni_v10(usage_msg_v10_0);
	else if (restool.mc_fw_version.minor == 1)
		return create_dpni_v10(usage_msg_v10_1);
	return -EINVAL;
}

static int destroy_dpni_v9(uint32_t dpni_id)
{
	bool dpni_opened = false;
	uint16_t dpni_handle;
	int error, error2;

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

	error = dpni_destroy(&restool.mc_io, 0, dpni_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpni_opened = false;
	printf("dpni.%u is destroyed\n", dpni_id);

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

static int destroy_dpni_v10(uint32_t dpni_id)
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

	error = dpni_destroy_v10(&restool.mc_io, dprc_handle,
				 0, dpni_id);
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

static int destroy_dpni(int mc_fw_version)
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

	if (mc_fw_version == MC_FW_VERSION_9)
		error = destroy_dpni_v9(dpni_id);
	else if (mc_fw_version == MC_FW_VERSION_10)
		error = destroy_dpni_v10(dpni_id);
	else
		return -EINVAL;
out:
	return error;
}

static int cmd_dpni_destroy_v9(void)
{
	return destroy_dpni(MC_FW_VERSION_9);
}

static int cmd_dpni_destroy_v10(void)
{
	return destroy_dpni(MC_FW_VERSION_10);
}

static int update_dpni_v10(const char *usage_msg)
{
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

		error = dpni_set_primary_mac_addr_v10(&restool.mc_io, 0,
						      dpni_handle, mac_addr);
		if (error)
			ERROR_PRINTF("dpni_set_primary_mac_addr_v10() = %d\n", error);

		minimal_options = true;
	}

	dpni_close(&restool.mc_io, 0, dpni_handle);
out:

	if (!minimal_options && !error) {
		ERROR_PRINTF("Specify at least an option to update!\n");
		puts(usage_msg);
	}

	return error;
}


static int cmd_dpni_update_v10(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpni update dpni.X [OPTIONS]\n"
		"\n"
		"OPTIONS:\n"
		"   --mac-addr=<addr>\n"
		"	String specifying primary MAC address (e.g. 00:00:05:00:00:05).\n"
		"\n";

	return update_dpni_v10(usage_msg);
}

struct object_command dpni_commands_v9[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpni_help },

	{ .cmd_name = "info",
	  .options = dpni_info_options,
	  .cmd_func = cmd_dpni_info_v9 },

	{ .cmd_name = "create",
	  .options = dpni_create_options,
	  .cmd_func = cmd_dpni_create_v9 },

	{ .cmd_name = "destroy",
	  .options = dpni_destroy_options,
	  .cmd_func = cmd_dpni_destroy_v9 },

	{ .cmd_name = NULL },
};

struct object_command dpni_commands_v10[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpni_help_v10 },

	{ .cmd_name = "info",
	  .options = dpni_info_options,
	  .cmd_func = cmd_dpni_info_v10 },

	{ .cmd_name = "create",
	  .options = dpni_create_options,
	  .cmd_func = cmd_dpni_create_v10 },

	{ .cmd_name = "destroy",
	  .options = dpni_destroy_options,
	  .cmd_func = cmd_dpni_destroy_v10 },

	{ .cmd_name = "update",
	  .options = dpni_update_options_v10,
	  .cmd_func = cmd_dpni_update_v10 },

	{ .cmd_name = NULL },
};


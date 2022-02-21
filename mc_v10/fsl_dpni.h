/* Copyright 2013-2016 Freescale Semiconductor Inc.
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
#ifndef __FSL_DPNI__v10_H
#define __FSL_DPNI__v10_H

#include "../mc_v9/fsl_dpkg.h"
#include "../mc_v9/fsl_dpni.h"

struct fsl_mc_io;

/**
 * Data Path Network Interface API
 * Contains initialization APIs and runtime control APIs for DPNI
 */

/** General DPNI macros */

/**
 * Maximum number of OPR(order point record)
 */
#define DPNI_MAX_OPR 16 * 8
/**
 * Maximum number of traffic classes
 */
#define DPNI_MAX_TC				8
/**
 * Maximum number of buffer pools per DPNI
 */
#define DPNI_MAX_DPBP				8
/**
 * Maximum number of storage-profiles per DPNI
 */
#define DPNI_MAX_SP				2

/**
 * All traffic classes considered; see dpni_set_queue()
 */
#define DPNI_ALL_TCS				(uint8_t)(-1)
/**
 * All flows within traffic class considered; see dpni_set_queue()
 */
#define DPNI_ALL_TC_FLOWS			(uint16_t)(-1)

/**
 * Tx traffic is always released to a buffer pool on transmit, there are no
 * resources allocated to have the frames confirmed back to the source after
 * transmission.
 */
#define DPNI_OPT_TX_FRM_RELEASE			0x000001
/**
 * Disables support for MAC address filtering for addresses other than primary
 * MAC address. This affects both unicast and multicast. Promiscuous mode can
 * still be enabled/disabled for both unicast and multicast. If promiscuous mode
 * is disabled, only traffic matching the primary MAC address will be accepted.
 */
#define DPNI_OPT_NO_MAC_FILTER			0x000002
/**
 * Allocate policers for this DPNI. They can be used to rate-limit traffic per
 * traffic class (TC) basis.
 */
#define DPNI_OPT_HAS_POLICING			0x000004
/**
 * Congestion can be managed in several ways, allowing the buffer pool to
 * deplete on ingress, taildrop on each queue or use congestion groups for sets
 * of queues. If set, it configures a single congestion groups across all TCs.
 * If reset, a congestion group is allocated for each TC. Only relevant if the
 * DPNI has multiple traffic classes.
 */
#define DPNI_OPT_SHARED_CONGESTION		0x000008
/**
 * Enables TCAM for Flow Steering and QoS look-ups. If not specified, all
 * look-ups are exact match. Note that TCAM is not available on LS1088 and its
 * variants. Setting this bit on these SoCs will trigger an error.
 */
#define DPNI_OPT_HAS_KEY_MASKING		0x000010
/**
 * Disables the flow steering table.
 */
#define DPNI_OPT_NO_FS				0x000020

/**
 * Enable the Order Restoration support
 */
#define DPNI_OPT_HAS_OPR				0x000040

/**
 * Order Point Records are shared for the entire TC
 */
#define DPNI_OPT_OPR_PER_TC				0x000080
/**
 * All Tx traffic classes will use a single sender (ignore num_queueus for tx)
 */
#define DPNI_OPT_SINGLE_SENDER			0x000100
/**
 * Define a custom number of congestion groups
 */
#define DPNI_OPT_CUSTOM_CG				0x000200
/**
 * Define a custom number of order point records
 */
#define DPNI_OPT_CUSTOM_OPR				0x000400
/**
 * Hash key is shared between all traffic classes
 */
#define DPNI_OPT_SHARED_HASH_KEY			0x000800
/**
 * Flow steering table is shared between all traffic classes
 */
#define DPNI_OPT_SHARED_FS				0x001000
/*
 * Fq frame data, context and annotations stashing disable.
 * The stashing is enabled by default.
 */
#define DPNI_OPT_STASHING_DIS				0x002000
/**
 * struct dpni_cfg_v10 - Structure representing DPNI configuration
 * @mac_addr:	Primary MAC address
 * @adv:	Advanced parameters; default is all zeros;
 *		use this structure to change default settings
 */
struct dpni_cfg_v10 {
	/**
	 * @options: Any combination of the following options:
	 *		DPNI_OPT_TX_FRM_RELEASE
	 *		DPNI_OPT_NO_MAC_FILTER
	 *		DPNI_OPT_HAS_POLICING
	 *		DPNI_OPT_SHARED_CONGESTION
	 *		DPNI_OPT_HAS_KEY_MASKING
	 *		DPNI_OPT_NO_FS
	 *		DPNI_OPT_HAS_OPR
	 *		DPNI_OPT_OPR_PER_TC
	 *		DPNI_OPT_SINGLE_SENDER
	 *		DPNI_OPT_CUSTOM_CG
	 *		DPNI_OPT_CUSTOM_OPR
	 *		DPNI_OPT_SHARED_HASH_KEY
	 *		DPNI_OPT_SHARED_FS
	 *		DPNI_OPT_STASHING_DIS
	 * @fs_entries: Number of entries in the flow steering table.
	 *		This table is used to select the ingress queue for
	 *		ingress traffic, targeting a GPP core or another.
	 *		In addition it can be used to discard traffic that
	 *		matches the set rule. It is either an exact match table
	 *		or a TCAM table, depending on DPNI_OPT_ HAS_KEY_MASKING
	 *		bit in OPTIONS field. This field is ignored if
	 *		DPNI_OPT_NO_FS bit is set in OPTIONS field. Otherwise,
	 *		value 0 defaults to 64. Maximum supported value is 1024.
	 *		Note that the total number of entries is limited on the
	 *		SoC to as low as 512 entries if TCAM is used.
	 * @vlan_filter_entries: Number of entries in the VLAN address filtering
	 *		table. This is an exact match table used to filter
	 *		ingress traffic based on VLAN IDs. Value 0 disables VLAN
	 *		filtering. Maximum supported value is 16.
	 * @mac_filter_entries: Number of entries in the MAC address filtering
	 *		table. This is an exact match table and allows both
	 *		unicast and multicast entries. The primary MAC address
	 *		of the network interface is not part of this table,
	 *		this contains only entries in addition to it. This
	 *		field is ignored if DPNI_OPT_ NO_MAC_FILTER is set in
	 *		OPTIONS field. Otherwise, value 0 defaults to 80.
	 *		Maximum supported value is 80.
	 * @num_queues: Number of Tx and Rx queues used for traffic
	 *		distribution. This is orthogonal to QoS and is only
	 *		used to distribute traffic to multiple GPP cores.
	 *		This configuration affects the number of Tx queues
	 *		(logical FQs, all associated with a single CEETM queue),
	 *		Rx queues and Tx confirmation queues, if applicable.
	 *		Value 0 defaults to one queue. Maximum supported value
	 *		is 32.
	 * @num_tcs: Number of traffic classes (TCs), reserved for the DPNI.
	 *		TCs can have different priority levels for the purpose
	 *		of Tx scheduling (see DPNI_SET_TX_SELECTION), different
	 *		BPs (DPNI_ SET_POOLS), policers. There are dedicated QM
	 *		queues for traffic classes (including class queues on
	 *		Tx). Value 0 defaults to one TC. Maximum supported value
	 *		is 16. There are maximum 16 TCs for Tx and 8 TCs for Rx.
	 *		When num_tcs>8 Tx will use this value but Rx will have
	 *		only 8 traffic classes.
	 * @num_rx_tcs: if set to other value than zero represents number
	 *		of TCs used for Rx. Maximum value is 8. If set to zero the
	 *		number of Rx TCs will be initialized with the value provided
	 *		in num_tcs parameter.
	 * @qos_entries: Number of entries in the QoS classification table. This
	 *		table is used to select the TC for ingress traffic. It
	 *		is either an exact match or a TCAM table, depending on
	 *		DPNI_OPT_ HAS_KEY_MASKING bit in OPTIONS field. This
	 *		field is ignored if the DPNI has a single TC. Otherwise,
	 *		a value of 0 defaults to 64. Maximum supported value
	 *		is 64.
	 */
	uint32_t options;
	uint16_t fs_entries;
	uint8_t  vlan_filter_entries;
	uint8_t  mac_filter_entries;
	uint8_t  num_queues;
	uint8_t  num_tcs;
	uint8_t  num_rx_tcs;
	uint8_t  qos_entries;
	uint8_t  num_cgs;
	uint16_t num_opr;
	uint8_t	 dist_key_size;
	uint8_t  num_ceetm_ch;
};

int dpni_create_v10(struct fsl_mc_io *mc_io,
		    uint16_t dprc_token,
		    uint32_t cmd_flags,
		    const struct dpni_cfg_v10 *cfg,
		    uint32_t *obj_id);

int dpni_destroy_v10(struct fsl_mc_io *mc_io,
		     uint16_t dprc_token,
		     uint32_t cmd_flags,
		     uint32_t object_id);

int dpni_open_v10(struct fsl_mc_io *mc_io,
		  uint32_t cmd_flags,
		  int dpni_id,
		  uint16_t *token);

int dpni_close_v10(struct fsl_mc_io *mc_io,
		   uint32_t cmd_flags,
		   uint16_t token);

/**
 * struct dpni_attr_v10 - Structure representing DPNI attributes
 * @options: Any combination of the following options:
 *		DPNI_OPT_TX_FRM_RELEASE
 *		DPNI_OPT_NO_MAC_FILTER
 *		DPNI_OPT_HAS_POLICING
 *		DPNI_OPT_SHARED_CONGESTION
 *		DPNI_OPT_HAS_KEY_MASKING
 *		DPNI_OPT_NO_FS
 *		DPNI_OPT_HAS_OPR
 *		DPNI_OPT_OPR_PER_TC
 *		DPNI_OPT_SINGLE_SENDER
 *		DPNI_OPT_CUSTOM_CG
 *		DPNI_OPT_CUSTOM_OPR
 *		DPNI_OPT_SHARED_HASH_KEY
 *		DPNI_OPT_SHARED_FS
 *		DPNI_OPT_STASHING_DIS
 * @num_queues: Number of Tx and Rx queues used for traffic distribution.
 * @num_rx_tcs: Number of RX traffic classes (TCs), reserved for the DPNI.
 * @num_tx_tcs: Number of TX traffic classes (TCs), reserved for the DPNI.
 * @mac_filter_entries: Number of entries in the MAC address filtering
 *		table.
 * @vlan_filter_entries: Number of entries in the VLAN address filtering
 *		table.
 * @qos_entries: Number of entries in the QoS classification table.
 * @fs_entries: Number of entries in the flow steering table.
 * @qos_key_size: Size, in bytes, of the QoS look-up key. Defining a key larger
 *			than this when adding QoS entries will result
 *			in an error.
 * @fs_key_size: Size, in bytes, of the flow steering look-up key. Defining a
 *			key larger than this when composing the hash + FS key
 *			will result in an error.
 * @wriop_version: Version of WRIOP HW block.
 *			The 3 version values are stored on 6, 5, 5 bits
 *			respectively.
 *			Values returned:
 *			- 0x400 - WRIOP version 1.0.0, used on LS2080 and
 *			variants,
 *			- 0x421 - WRIOP version 1.1.1, used on LS2088 and
 *			variants,
 *			- 0x422 - WRIOP version 1.1.2, used on LS1088 and
 *			variants.
 *			- 0xC00 - WRIOP version 3.0.0, used on LX2160 and
 *			variants.
 * @num_ceetm_ch: Number of egress channels used by this dpni object. If
 * 			not specified the dpni object will use a single CEETM channel.
 * @num_opr: Number of desired custom number of order point
 * 			records when DPNI_OPT_CUSTOM_OPR is set.
 */
struct dpni_attr_v10 {
	uint32_t options;
	uint8_t  num_queues;
	uint8_t  num_rx_tcs;
	uint8_t  num_tx_tcs;
	uint8_t  mac_filter_entries;
	uint8_t  vlan_filter_entries;
	uint8_t  qos_entries;
	uint16_t fs_entries;
	uint8_t  qos_key_size;
	uint8_t  fs_key_size;
	uint16_t wriop_version;
	uint8_t  num_cgs;
	uint8_t  num_ceetm_ch;
	uint16_t num_opr;
};

int dpni_get_attributes_v10(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    struct dpni_attr_v10 *attr);

#define DPNI_STATISTICS_CNT		7

union dpni_statistics_v10 {
	/**
	 * struct page_0 - Page_0 statistics structure
	 * @ingress_all_frames: Ingress frame count
	 * @ingress_all_bytes: Ingress byte count
	 * @ingress_multicast_frames: Ingress multicast frame count
	 * @ingress_multicast_bytes: Ingress multicast byte count
	 * @ingress_broadcast_frames: Ingress broadcast frame count
	 * @ingress_broadcast_bytes: Ingress broadcast byte count
	 */
	struct {
		uint64_t ingress_all_frames;
		uint64_t ingress_all_bytes;
		uint64_t ingress_multicast_frames;
		uint64_t ingress_multicast_bytes;
		uint64_t ingress_broadcast_frames;
		uint64_t ingress_broadcast_bytes;
	} page_0;
	/**
	 * struct page_1 - Page_1 statistics structure
	 * @egress_all_frames: Egress frame count
	 * @egress_all_bytes: Egress byte count
	 * @egress_multicast_frames: Egress multicast frame count
	 * @egress_multicast_bytes: Egress multicast byte count
	 * @egress_broadcast_frames: Egress broadcast frame count
	 * @egress_broadcast_bytes: Egress broadcast byte count
	 */
	struct {
		uint64_t egress_all_frames;
		uint64_t egress_all_bytes;
		uint64_t egress_multicast_frames;
		uint64_t egress_multicast_bytes;
		uint64_t egress_broadcast_frames;
		uint64_t egress_broadcast_bytes;
	} page_1;
	/**
	 * struct page_2 - Page_2 statistics structure
	 * @ingress_filtered_frames: Ingress filtered frame count
	 * @ingress_discarded_frames: Ingress discarded frame count
	 * @ingress_nobuffer_discards: Ingress discarded frame count due to
	 *					lack of buffers
	 * @egress_discarded_frames: Egress discarded frame count
	 * @egress_confirmed_frames: Egress confirmed frame count
	 */
	struct {
		uint64_t ingress_filtered_frames;
		uint64_t ingress_discarded_frames;
		uint64_t ingress_nobuffer_discards;
		uint64_t egress_discarded_frames;
		uint64_t egress_confirmed_frames;
	} page_2;
	/**
	 * struct page_3 - Page_3 statistics structure with values for the selected TC
	 * @ceetm_dequeue_bytes: Cumulative count of the number of bytes dequeued
	 * @ceetm_dequeue_frames: Cumulative count of the number of frames dequeued
	 * @ceetm_reject_bytes: Cumulative count of the number of bytes in all frames whose
						enqueue was rejected
	 * @ceetm_reject_frames: Cumulative count of all frame enqueues rejected
	 */
	struct {
		uint64_t ceetm_dequeue_bytes;
		uint64_t ceetm_dequeue_frames;
		uint64_t ceetm_reject_bytes;
		uint64_t ceetm_reject_frames;
	} page_3;
	struct {
		uint64_t cgr_reject_frames;
		uint64_t cgr_reject_bytes;
	} page_4;
	struct {
		uint64_t policer_cnt_red;
		uint64_t policer_cnt_yellow;
		uint64_t policer_cnt_green;
		uint64_t policer_cnt_re_red;
		uint64_t policer_cnt_re_yellow;
	} page_5;
	struct {
		uint64_t tx_pending_frames_cnt;
	} page_6;
	struct {
		uint64_t counter[DPNI_STATISTICS_CNT];
	} raw;
};

int dpni_get_api_version_v10(struct fsl_mc_io *mc_io,
			     uint32_t cmd_flags,
			     uint16_t *major_ver,
			     uint16_t *minor_ver);

int dpni_get_statistics_v10(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    uint8_t page,
			    uint16_t param,
			    union dpni_statistics_v10 *stat);

int dpni_get_primary_mac_addr_v10(struct fsl_mc_io *mc_io,
				  uint32_t cmd_flags,
				  uint16_t token,
				  uint8_t mac_addr[6]);

int dpni_set_primary_mac_addr_v10(struct fsl_mc_io *mc_io,
				  uint32_t cmd_flags,
				  uint16_t token,
				  const uint8_t mac_addr[6]);

/**
 * struct dpni_link_state - Structure representing DPNI link state
 * @rate:	Rate
 * @options:	Mask of available options; use 'DPNI_LINK_OPT_<X>' values
 * @up:		Link state; '0' for down, '1' for up
 */
struct dpni_link_state_v10 {
	uint32_t rate;
	uint64_t options;
	int up;
};

int dpni_get_link_state_v10(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    struct dpni_link_state_v10 *state);

int dpni_get_irq_mask_v10(struct fsl_mc_io *mc_io,
			  uint32_t cmd_flags,
			  uint16_t token,
			  uint8_t irq_index,
			  uint32_t *mask);

int dpni_get_irq_status_v10(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    uint8_t irq_index,
			    uint32_t *status);

int dpni_get_max_frame_length(struct fsl_mc_io *mc_io,
			      uint32_t cmd_flags,
			      uint16_t token,
			      uint16_t *max_frame_length);

#endif /* __FSL_DPNI_v10_H */

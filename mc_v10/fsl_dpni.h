/* Copyright 2013-2017 Freescale Semiconductor Inc.
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
 * All traffic classes considered; see dpni_set_rx_flow()
 */
#define DPNI_ALL_TCS				(uint8_t)(-1)
/**
 * All flows within traffic class considered; see dpni_set_rx_flow()
 */
#define DPNI_ALL_TC_FLOWS			(uint16_t)(-1)
/**
 * Generate new flow ID; see dpni_set_tx_flow()
 */
#define DPNI_NEW_FLOW_ID			(uint16_t)(-1)
/* use for common tx-conf queue; see dpni_set_tx_conf_<x>() */
#define DPNI_COMMON_TX_CONF			(uint16_t)(-1)
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
#define DPNI_OPT_NO_FS					0x000020

/**
 *  * Enable the Order Restoration support
 */
#define DPNI_OPT_HAS_OPR				0x000040

/**
 *  * Order Point Records are shared for the entire TC
 */
#define DPNI_OPT_OPR_PER_TC				0x000080

/* DPNI configuration options */

/**
 * Allow different distribution key profiles for different traffic classes;
 * if not set, a single key profile is assumed
 */
#define DPNI_OPT_ALLOW_DIST_KEY_PER_TC		0x00000001

/**
 * Disable all non-error transmit confirmation; error frames are reported
 * back to a common Tx error queue
 */
#define DPNI_OPT_TX_CONF_DISABLED		0x00000002

/**
 * Disable per-sender private Tx confirmation/error queue
 */
#define DPNI_OPT_PRIVATE_TX_CONF_ERROR_DISABLED	0x00000004

/**
 * Support distribution based on hashed key;
 * allows statistical distribution over receive queues in a traffic class
 */
#define DPNI_OPT_DIST_HASH			0x00000010

/**
 * DEPRECATED - if this flag is selected and and all new 'max_fs_entries' are
 * '0' then backward compatibility is preserved;
 * Support distribution based on flow steering;
 * allows explicit control of distribution over receive queues in a traffic
 * class
 */
#define DPNI_OPT_DIST_FS			0x00000020

/**
 * struct dpni_cfg_v10 - Structure representing DPNI configuration
 * @mac_addr: Primary MAC address
 * @adv: Advanced parameters; default is all zeros;
 *		use this structure to change default settings
 */
struct dpni_cfg_v10 {
	/**
	 *@options: Any combination of the following options:
	 *		DPNI_OPT_TX_FRM_RELEASE
	 *		DPNI_OPT_NO_MAC_FILTER
	 *		DPNI_OPT_HAS_POLICING
	 *		DPNI_OPT_SHARED_CONGESTION
	 *		DPNI_OPT_HAS_KEY_MASKING
	 *		DPNI_OPT_NO_FS
	 *		DPNI_OPT_HAS_OPR
	 *		DPNI_OPT_OPR_PER_TC
	 *@fs_entries: Number of entries in the flow steering table.
	 *		This table is used to select the ingress queue for ingress traffic,
	 *		targeting a GPP core or another. In addition it can	be used to
	 *		discard traffic that matches the set rule. It is either an exact
	 *		match table or a TCAM table, depending on DPNI_OPT_ HAS_KEY_MASKING
	 *		bit in OPTIONS field. This field is ignored if DPNI_OPT_NO_FS bit is
	 *		set in OPTIONS field. Otherwise, value 0 defaults to 64.
	 *		Maximum supported value is 1024.
	 *		Note that the total number of entries is limited on the SoC to as
	 *		low as 512 entries if TCAM is used.
	 *@vlan_filter_entries: Number of entries in the VLAN address filtering
	 *		table. This is an exact match table used to filter ingress traffic
	 *		based on VLAN IDs. Value 0 disables VLAN filtering.
	 *		Maximum supported value is 16.
	 *@mac_filter_entries: Number of entries in the MAC address filtering
	 *		table. This is an exact match table and allows both unicast and
	 *		multicast entries. The primary MAC address of the network interface
	 *		is not part of this table, this contains only entries in addition to
	 *		it. This field is ignored if DPNI_OPT_ NO_MAC_FILTER is set in
	 *		OPTIONS field. Otherwise, value 0 defaults to 80. Maximum supported
	 *		value is 80.
	 *@num_queues: Number of Tx and Rx queues used for traffic distribution.
	 *		This is orthogonal to QoS and is only used to distribute traffic to
	 *		multiple GPP cores. This configuration affects the number of Tx
	 *		queues (logical FQs, all associated
	 *		with a single CEETM queue), Rx queues and Tx confirmation queues,
	 *		if applicable. Value 0 defaults to one queue. Maximum supported
	 *		value is 8.
	 *@num_tcs: Number of traffic classes (TCs), reserved for the DPNI. TCs can
	 *		have different priority levels for the purpose of Tx scheduling (see
	 *		DPNI_SET_TX_SELECTION), different BPs (DPNI_ SET_POOLS), policers.
	 *		There are dedicated QM queues for traffic classes (including class
	 *		queues on Tx). Value 0 defaults to one TC. Maximum supported value
	 *		is 8.
	 *@qos_entries: Number of entries in the QoS classification table. This
	 *		table is used to select the TC for ingress traffic. It is either an
	 *		exact match or a TCAM table, depending on DPNI_OPT_ HAS_KEY_MASKING
	 *		bit in OPTIONS field. This field is ignored if the DPNI has a single
	 *		TC. Otherwise, a value of 0 defaults to 64. Maximum supported value
	 *		is 64.
	 */
	uint32_t options;
	uint16_t fs_entries;
	uint8_t vlan_filter_entries;
	uint8_t mac_filter_entries;
	uint8_t num_queues;
	uint8_t num_tcs;
	uint8_t qos_entries;
};

/**
 * dpni_create_v10() - Create the DPNI object
 * @mc_io:	Pointer to MC portal's I/O object
 * @dprc_token:	Parent container token; '0' for default container
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @cfg:	Configuration structure
 * @obj_id:	Returned object id
 *
 * Create the DPNI object, allocate required resources and
 * perform required initialization.
 *
 * The object can be created either by declaring it in the
 * DPL file, or by calling this function.
 *
 * The function accepts an authentication token of a parent
 * container that this object should be assigned to. The token
 * can be '0' so the object will be assigned to the default container.
 * The newly created object can be opened with the returned
 * object id and using the container's associated tokens and MC portals.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_create_v10(struct fsl_mc_io *mc_io,
		    uint16_t dprc_token,
		    uint32_t cmd_flags,
		    const struct dpni_cfg_v10 *cfg,
		    uint32_t *obj_id);

/**
 * dpni_destroy_v10() - Destroy the DPNI object and release all its resources.
 * @mc_io:	Pointer to MC portal's I/O object
 * @dprc_token: Parent container token; '0' for default container
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @object_id:	The object id; it must be a valid id within the container that
 * created this object;
 *
 * The function accepts the authentication token of the parent container that
 * created the object (not the one that currently owns the object). The object
 * is searched within parent using the provided 'object_id'.
 * All tokens to the object must be closed before calling destroy.
 *
 * Return:	'0' on Success; error code otherwise.
 */
int dpni_destroy_v10(struct fsl_mc_io *mc_io,
		     uint16_t dprc_token,
		     uint32_t cmd_flags,
		     uint32_t object_id);

/**
 * struct dpni_attr_v10 - Structure representing DPNI attributes
 *@options: Any combination of the following options:
 *		DPNI_OPT_TX_FRM_RELEASE
 *		DPNI_OPT_NO_MAC_FILTER
 *		DPNI_OPT_HAS_POLICING
 *		DPNI_OPT_SHARED_CONGESTION
 *		DPNI_OPT_HAS_KEY_MASKING
 *		DPNI_OPT_NO_FS
 *@num_queues: Number of Tx and Rx queues used for traffic distribution.
 *@num_tcs: Number of traffic classes (TCs), reserved for the DPNI.
 *@mac_filter_entries: Number of entries in the MAC address filtering
 *		table.
 *@vlan_filter_entries: Number of entries in the VLAN address filtering
 *		table.
 *@qos_entries: Number of entries in the QoS classification table.
 *@fs_entries: Number of entries in the flow steering table.
 *@qos_key_size: Size, in bytes, of the QoS look-up key. Defining a key larger
 *				than this when adding QoS entries will result in an error.
 *@fs_key_size: Size, in bytes, of the flow steering look-up key. Defining a
 *				key larger than this when composing the hash + FS key will
 *				result in an error.
 */
struct dpni_attr_v10 {
	uint32_t options;
	uint8_t num_queues;
	uint8_t num_tcs;
	uint8_t mac_filter_entries;
	uint8_t vlan_filter_entries;
	uint8_t qos_entries;
	uint16_t fs_entries;
	uint8_t qos_key_size;
	uint8_t fs_key_size;
};

/**
 * dpni_get_attributes_v10() - Retrieve DPNI attributes.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPNI object
 * @attr:	Object's attributes
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_attributes_v10(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    struct dpni_attr_v10 *attr);

union dpni_statistics_v10 {
	/**
	 * struct page_0 - Page_0 statistics structure
	 * @ingress_all_frames:		Ingress frame count
	 * @ingress_all_bytes:		Ingress byte count
	 * @ingress_multicast_frames:	Ingress multicast frame count
	 * @ingress_multicast_bytes:	Ingress multicast byte count
	 * @ingress_broadcast_frames:	Ingress broadcast frame count
	 * @ingress_broadcast_bytes:	Ingress broadcast byte count
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
	 * @egress_all_frames:		Egress frame count
	 * @egress_all_bytes:		Egress byte count
	 * @egress_multicast_frames:	Egress multicast frame count
	 * @egress_multicast_bytes:	Egress multicast byte count
	 * @egress_broadcast_frames:	Egress broadcast frame count
	 * @egress_broadcast_bytes:	Egress broadcast byte count
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
	 * @ingress_filtered_frames:	Ingress filtered frame count
	 * @ingress_discarded_frames:	Ingress discarded frame count
	 * @ingress_nobuffer_discards:	Ingress discarded frame count due
	 *				to lack of buffers
	 * @egress_discarded_frames:	Egress discarded frame count
	 * @egress_confirmed_frames:	Egress confirmed frame count
	 */
	struct {
		uint64_t ingress_filtered_frames;
		uint64_t ingress_discarded_frames;
		uint64_t ingress_nobuffer_discards;
		uint64_t egress_discarded_frames;
		uint64_t egress_confirmed_frames;
	} page_2;
	/**
	 * struct raw - raw statistics structure
	 * @counter0:
	 * @counter1:
	 * @counter2:
	 * @counter3:
	 * @counter4:
	 * @counter5:
	 * @counter6: Reserved
	 */
	struct {
		uint64_t counter0;
		uint64_t counter1;
		uint64_t counter2;
		uint64_t counter3;
		uint64_t counter4;
		uint64_t counter5;
		uint64_t counter6;
	} raw;
};

/**
 * stashes the whole annotation area (up to 192 bytes)
 */
#define DPNI_FLC_STASH_FRAME_ANNOTATION	0x00000001

enum dpni_queue_type_v10 {
	DPNI_QUEUE_RX,
	DPNI_QUEUE_TX,
	DPNI_QUEUE_TX_CONFIRM,
	DPNI_QUEUE_RX_ERR,
};

/**
 * struct dpni_queue_v10 - Queue structure
 * @fqid:	FQID used for enqueueing to and/or configuration of this specific FQ
 * @qdid:	Queueing destination ID, used to enqueue using QDID, DQBIN, QPRI.
 *		Only relevant for Tx queues.
 * @qdbin:	Queueing bin, used to enqueue using QDID, DQBIN, QPRI. Only relevant
 *		for Tx queues.
 * @user_context:	User data, presented to the user along with any frames from
 *			this queue. Not relevant for Tx queues.
 */
struct dpni_queue_v10 {
	/**
	 * struct destination - Destination structure
	 * @id:		ID of the destination, only relevant if DEST_TYPE is > 0.
	 *		Identifies either a DPIO or a DPCON object. Not relevant for Tx
	 *		queues.
	 * @type:	May be one of the following:
	 *		0 - No destination, queue can be manually queried, but will not
	 *			push traffic or notifications to a DPIO;
	 *		1 - The destination is a DPIO. When traffic becomes available in
	 *			the queue a FQDAN (FQ data available notification) will be
	 *			generated to selected DPIO;
	 *		2 - The destination is a DPCON. The queue is associated with a
	 *			DPCON object for the purpose of scheduling between multiple
	 *			queues. The DPCON may be independently configured to
	 *			generate notifications. Not relevant for Tx queues.
	 * @hold_active: Hold active
	 */
	struct {
		uint16_t id;
		enum dpni_dest type;
		char hold_active;
	} destination;
	uint32_t fqid;
	uint16_t qdid;
	uint16_t qdbin;
	uint64_t user_context;
};

/**
 * dpni_add_qos_entry_v10() - Add QoS mapping entry (to select a traffic class)
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPNI object
 * @cfg:	QoS rule to add
 * @has_index:	If set, indicates that the command contains a valid index field
 * @tc_id:	Traffic class selection (0-7)
 * @index:	Location in the QoS table where to insert the entry.
 *		Only relevant if HAS_INDEX bit is set. This is relevant for
 *		TCAM look-up, where ordering of entries is important.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_add_qos_entry_v10(struct fsl_mc_io *mc_io,
			   uint32_t cmd_flags,
			   uint16_t token,
			   const struct dpni_rule_cfg *cfg,
			   char has_index,
			   uint8_t tc_id,
			   uint16_t index);

/**
 * dpni_add_fs_entry_v10() - Add Flow Steering entry for a specific traffic class
 *			(to select a flow ID)
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPNI object
 * @tc_id:	Traffic class selection (0-7)
 * @cfg:	Flow steering rule to add
 * @flow_id:	Flow id selection (must be smaller than the
 *		distribution size of the traffic class)
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_add_fs_entry_v10(struct fsl_mc_io *mc_io,
			  uint32_t cmd_flags,
			  uint16_t token,
			  char has_index,
			  uint8_t tc_id,
			  uint16_t index,
			  const struct dpni_rule_cfg *cfg,
			  uint16_t flow_id);

/**
 * dpni_get_version_v10() - Get Data Path Network Interface version
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @majorVer:	Major version of data path network interface object
 * @minorVer:	Minor version of data path network interface object
 *
 * Return:  '0' on Success; Error code otherwise.
 */
int dpni_get_version_v10(struct fsl_mc_io *mc_io,
			   uint32_t cmd_flags,
			   uint16_t *majorVer,
			   uint16_t *minorVer);

/**
 * dpni_set_queue_v10() - Set queue parameters
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPNI object
 * @type:	Type of queue
 * @tc:		Traffic class, in range 0 to NUM_TCS - 1
 * @index:	Selects the specific queue out of the set allocated for the same
 *		TC. Value must be in range 0 to NUM_QUEUES - 1
 * @queue:	Queue structure
 *
 * Return:  '0' on Success; Error code otherwise.
 */
int dpni_set_queue_v10(struct fsl_mc_io *mc_io,
		       uint32_t cmd_flags,
		       uint16_t token,
		       enum dpni_queue_type_v10 type,
		       uint8_t tc,
		       uint8_t index,
		       const struct dpni_queue_v10 *queue);

/**
 * dpni_set_queue_v10() - Get queue parameters
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPNI object
 * @type:	Type of queue
 * @tc:		Traffic class, in range 0 to NUM_TCS - 1
 * @index:	Selects the specific queue out of the set allocated for the same
 *		TC. Value must be in range 0 to NUM_QUEUES - 1
 * @queue:	Queue structure
 *
 * Return:  '0' on Success; Error code otherwise.
 */
int dpni_get_queue_v10(struct fsl_mc_io *mc_io,
		       uint32_t cmd_flags,
		       uint16_t token,
		       enum dpni_queue_type_v10 type,
		       uint8_t tc,
		       uint8_t index,
		       struct dpni_queue_v10 *queue);

/**
 * dpni_get_statistics_v10() - Get DPNI statistics
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPNI object
 * @page:	Selects the statistics page to retrieve, see DPNI_GET_STATISTICS
 *		output. Pages are numbered 0 to 2.
 * @stat:	Structure containing the statistics
 *
 * Return:  '0' on Success; Error code otherwise.
 */
int dpni_get_statistics_v10(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    uint8_t page,
			    union dpni_statistics_v10 *stat);

/**
 * dpni_reset_statistics() - Clears DPNI statistics
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPNI object
 *
 * Return:  '0' on Success; Error code otherwise.
 */
int dpni_reset_statistics_v10(struct fsl_mc_io *mc_io,
			      uint32_t cmd_flags,
			      uint16_t token);

/**
 * enum dpni_congestion_point - Structure representing congestion point
 * @DPNI_CP_QUEUE:	Set taildrop per queue, identified by QUEUE_TYPE, TC and
 *			QUEUE_INDEX
 * @DPNI_CP_GROUP:	Set taildrop per queue group. Depending on options used to
 *			define the DPNI this can be either per TC (default) or per
 *			interface (DPNI_OPT_SHARED_CONGESTION set at DPNI create).
 *			QUEUE_INDEX is ignored if this type is used.
 */
enum dpni_congestion_point {
	DPNI_CP_QUEUE,
	DPNI_CP_GROUP,
};

/**
 * enum dpni_congestion_unit - DPNI congestion units
 * @DPNI_CONGESTION_UNIT_BYTES:		bytes unit
 * @DPNI_CONGESTION_UNIT_FRAMES:	frames units
 */
enum dpni_congestion_unit {
	DPNI_CONGESTION_UNIT_BYTES = 0,
	DPNI_CONGESTION_UNIT_FRAMES,
};

/**
 * struct dpni_taildrop_v10 - Structure representing the taildrop
 * @enable:	Indicates whether the taildrop is active or not.
 * @units:	Indicates the unit of THRESHOLD. Queue taildrop only supports
 *		byte units, this field is ignored and assumed = 0 if
 *		CONGESTION_POINT is 0.
 * @threshold:	Threshold value, in units identified by UNITS field. Value 0
 *		cannot be used as a valid taildrop threshold, THRESHOLD must
 *		be > 0 if the taildrop is enabled.
 */
struct dpni_taildrop_v10 {
	char enable;
	enum dpni_congestion_unit units;
	uint32_t threshold;
};

/**
 * dpni_set_taildrop_v10() - Set taildrop per queue
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPNI object
 * @cg_point:	Congestion point
 * @q_type:	Queue type
 * @tc:		Traffic class to apply this taildrop to
 * @q_index:	Index of the queue if the DPNI supports multiple queues for
 *		traffic distribution. Ignored if CONGESTION_POINT is not 0.
 * @taildrop:	Taildrop structure
 *
 * Return:  '0' on Success; Error code otherwise.
 */
int dpni_set_taildrop_v10(struct fsl_mc_io *mc_io,
			  uint32_t cmd_flags,
			  uint16_t token,
			  enum dpni_congestion_point cg_point,
			  enum dpni_queue_type_v10 q_type,
			  uint8_t tc,
			  uint8_t q_index,
			  struct dpni_taildrop_v10 *taildrop);

/**
 * dpni_get_taildrop_v10() - Get taildrop information
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPNI object
 * @cg_point:	Congestion point
 * @q_type:	Queue type
 * @tc:		Traffic class to apply this taildrop to
 * @q_index:	Index of the queue if the DPNI supports multiple queues for
 *		traffic distribution. Ignored if CONGESTION_POINT is not 0.
 * @taildrop:	Taildrop structure
 *
 * Return:  '0' on Success; Error code otherwise.
 */
int dpni_get_taildrop_v10(struct fsl_mc_io *mc_io,
			  uint32_t cmd_flags,
			  uint16_t token,
			  enum dpni_congestion_point cg_point,
			  enum dpni_queue_type_v10 q_type,
			  uint8_t tc,
			  uint8_t q_index,
			  struct dpni_taildrop_v10 *taildrop);

/**
 * dpni_set_primary_mac_addr_v10() - Set the primary MAC address
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPNI object
 * @mac_addr:	MAC address to set as primary address
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_primary_mac_addr_v10(struct fsl_mc_io *mc_io,
				  uint32_t cmd_flags,
				  uint16_t token,
				  const uint8_t mac_addr[6]);

#endif /* __FSL_DPNI_v10_H */

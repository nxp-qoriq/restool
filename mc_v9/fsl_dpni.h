/* Copyright 2013-2015 Freescale Semiconductor Inc.
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
#ifndef __FSL_DPNI_v9_H
#define __FSL_DPNI_v9_H

#include "fsl_dpkg.h"

/* DPNI configuration options */

#define DPNI_MAX_TC                          8

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

/* Disable per-sender private Tx confirmation/error queue */
#define DPNI_OPT_PRIVATE_TX_CONF_ERROR_DISABLED	0x00000004

/**
 * Support distribution based on hashed key;
 * allows statistical distribution over receive queues in a traffic class
 */
#define DPNI_OPT_DIST_HASH			0x00000010

/**
 * Support distribution based on flow steering;
 * allows explicit control of distribution over receive queues in a traffic
 * class
 */
#define DPNI_OPT_DIST_FS			0x00000020

/* Unicast filtering support */
#define DPNI_OPT_UNICAST_FILTER			0x00000080
/* Multicast filtering support */
#define DPNI_OPT_MULTICAST_FILTER		0x00000100
/* VLAN filtering support */
#define DPNI_OPT_VLAN_FILTER			0x00000200
/* Support IP reassembly on received packets */
#define DPNI_OPT_IPR				0x00000800
/* Support IP fragmentation on transmitted packets */
#define DPNI_OPT_IPF				0x00001000
/* VLAN manipulation support */
#define DPNI_OPT_VLAN_MANIPULATION		0x00010000
/* Support masking of QoS lookup keys */
#define DPNI_OPT_QOS_MASK_SUPPORT		0x00020000
/* Support masking of Flow Steering lookup keys */
#define DPNI_OPT_FS_MASK_SUPPORT		0x00040000

/**
 * enum dpni_counter - DPNI counter types
 * @DPNI_CNT_ING_FRAME: Counts ingress frames
 * @DPNI_CNT_ING_BYTE: Counts ingress bytes
 * @DPNI_CNT_ING_FRAME_DROP: Counts ingress frames dropped due to explicit
 *		'drop' setting
 * @DPNI_CNT_ING_FRAME_DISCARD: Counts ingress frames discarded due to errors
 * @DPNI_CNT_ING_MCAST_FRAME: Counts ingress multicast frames
 * @DPNI_CNT_ING_MCAST_BYTE: Counts ingress multicast bytes
 * @DPNI_CNT_ING_BCAST_FRAME: Counts ingress broadcast frames
 * @DPNI_CNT_ING_BCAST_BYTES: Counts ingress broadcast bytes
 * @DPNI_CNT_EGR_FRAME: Counts egress frames
 * @DPNI_CNT_EGR_BYTE: Counts egress bytes
 * @DPNI_CNT_EGR_FRAME_DISCARD: Counts egress frames discarded due to errors
 */
enum dpni_counter {
	DPNI_CNT_ING_FRAME = 0x0,
	DPNI_CNT_ING_BYTE = 0x1,
	DPNI_CNT_ING_FRAME_DROP = 0x2,
	DPNI_CNT_ING_FRAME_DISCARD = 0x3,
	DPNI_CNT_ING_MCAST_FRAME = 0x4,
	DPNI_CNT_ING_MCAST_BYTE = 0x5,
	DPNI_CNT_ING_BCAST_FRAME = 0x6,
	DPNI_CNT_ING_BCAST_BYTES = 0x7,
	DPNI_CNT_EGR_FRAME = 0x8,
	DPNI_CNT_EGR_BYTE = 0x9,
	DPNI_CNT_EGR_FRAME_DISCARD = 0xa
};

/**
 * enum dpni_dest - DPNI destination types
 * @DPNI_DEST_NONE: Unassigned destination; The queue is set in parked mode and
 *		does not generate FQDAN notifications; user is expected to
 *		dequeue from the queue based on polling or other user-defined
 *		method
 * @DPNI_DEST_DPIO: The queue is set in schedule mode and generates FQDAN
 *		notifications to the specified DPIO; user is expected to dequeue
 *		from the queue only after notification is received
 * @DPNI_DEST_DPCON: The queue is set in schedule mode and does not generate
 *		FQDAN notifications, but is connected to the specified DPCON
 *		object; user is expected to dequeue from the DPCON channel
 */
enum dpni_dest {
	DPNI_DEST_NONE = 0,
	DPNI_DEST_DPIO = 1,
	DPNI_DEST_DPCON = 2
};

/**
 * struct dpni_rule_cfg - Rule configuration for table lookup
 * @key_iova: I/O virtual address of the key (must be in DMA-able memory)
 * @mask_iova: I/O virtual address of the mask (must be in DMA-able memory)
 * @key_size: key and mask size (in bytes)
 */
struct dpni_rule_cfg {
	uint64_t	key_iova;
	uint64_t	mask_iova;
	uint8_t	key_size;
};

/**
 * struct dpni_ipr_cfg - Structure representing IP reassembly configuration
 * @max_reass_frm_size: Maximum size of the reassembled frame
 * @min_frag_size_ipv4: Minimum fragment size of IPv4 fragments
 * @min_frag_size_ipv6: Minimum fragment size of IPv6 fragments
 * @max_open_frames_ipv4: Maximum concurrent IPv4 packets in reassembly process
 * @max_open_frames_ipv6: Maximum concurrent IPv6 packets in reassembly process
 */
struct dpni_ipr_cfg {
	uint16_t max_reass_frm_size;
	uint16_t min_frag_size_ipv4;
	uint16_t min_frag_size_ipv6;
	uint16_t max_open_frames_ipv4;
	uint16_t max_open_frames_ipv6;
};

/**
 * struct dpni_cfg - Structure representing DPNI configuration
 * @mac_addr: Primary MAC address
 * @adv: Advanced parameters; default is all zeros;
 *		use this structure to change default settings
 */
struct dpni_cfg {
	uint8_t mac_addr[6];
	/**
	 * struct adv - Advanced parameters
	 * @options: Mask of available options; use 'DPNI_OPT_<X>' values
	 * @start_hdr: Selects the packet starting header for parsing;
	 *		'NET_PROT_NONE' is treated as default: 'NET_PROT_ETH'
	 * @max_senders: Maximum number of different senders; used as the number
	 *		of dedicated Tx flows; Non-power-of-2 values are rounded
	 *		up to the next power-of-2 value as hardware demands it;
	 *		'0' will be treated as '1'
	 * @max_tcs: Maximum number of traffic classes (for both Tx and Rx);
	 *		'0' will e treated as '1'
	 * @max_dist_per_tc: Maximum distribution size per Rx traffic class;
	 *			Non-power-of-2 values are rounded up to the next
	 *			power-of-2 value as hardware demands it
	 * @max_unicast_filters: Maximum number of unicast filters;
	 *			'0' is treated	as '16'
	 * @max_multicast_filters: Maximum number of multicast filters;
	 *			'0' is treated as '64'
	 * @max_qos_entries: if 'max_tcs > 1', declares the maximum entries in
	 *			the QoS	table; '0' is treated as '64'
	 * @max_qos_key_size: Maximum key size for the QoS look-up;
	 *			'0' is treated as '24' which is enough for IPv4
	 *			5-tuple
	 * @max_dist_key_size: Maximum key size for the distribution;
	 *		'0' is treated as '24' which is enough for IPv4 5-tuple
	 * @max_policers: Maximum number of policers;
	 *		should be between '0' and max_tcs
	 * @max_congestion_ctrl: Maximum number of congestion control groups
	 *		(CGs); covers early drop and congestion notification
	 *		requirements for traffic classes;
	 *		should be between '0' and max_tcs
	 * @ipr_cfg: IP reassembly configuration
	 */
	struct {
		uint32_t		options;
		enum net_prot		start_hdr;
		uint8_t		max_senders;
		uint8_t		max_tcs;
		uint8_t		max_dist_per_tc[DPNI_MAX_TC];
		uint8_t		max_unicast_filters;
		uint8_t		max_multicast_filters;
		uint8_t			max_vlan_filters;
		uint8_t		max_qos_entries;
		uint8_t		max_qos_key_size;
		uint8_t		max_dist_key_size;
		uint8_t		max_policers;
		uint8_t		max_congestion_ctrl;
		struct dpni_ipr_cfg	ipr_cfg;
	} adv;
};

/*
 * struct dpni_attr - Structure representing DPNI attributes
 * @id: DPNI object ID
 * @version: DPNI version
 * @start_hdr: Indicates the packet starting header for parsing
 * @options: Mask of available options; reflects the value as was given in
 *		object's creation
 * @max_senders: Maximum number of different senders; used as the number
 *		of dedicated Tx flows;
 * @max_tcs: Maximum number of traffic classes (for both Tx and Rx)
 * @max_dist_per_tc: Maximum distribution size per Rx traffic class;
 *			Set to the required value minus 1
 * @max_unicast_filters: Maximum number of unicast filters
 * @max_multicast_filters: Maximum number of multicast filters
 * @max_vlan_filters: Maximum number of VLAN filters
 * @max_qos_entries: if 'max_tcs > 1', declares the maximum entries in QoS table
 * @max_qos_key_size: Maximum key size for the QoS look-up
 * @max_dist_key_size: Maximum key size for the distribution look-up
 * @max_policers: Maximum number of policers;
 * @max_congestion_ctrl: Maximum number of congestion control groups (CGs);
 * @ipr_cfg: IP reassembly configuration
 */
struct dpni_attr {
	int			id;
	/**
	 * struct version - DPNI version
	 * @major: DPNI major version
	 * @minor: DPNI minor version
	 */
	struct {
		uint16_t major;
		uint16_t minor;
	} version;
	enum net_prot		start_hdr;
	uint32_t		options;
	uint8_t			max_senders;
	uint8_t		max_tcs;
	uint8_t		max_dist_per_tc[DPNI_MAX_TC];
	uint8_t		max_unicast_filters;
	uint8_t		max_multicast_filters;
	uint8_t		max_vlan_filters;
	uint8_t		max_qos_entries;
	uint8_t		max_qos_key_size;
	uint8_t		max_dist_key_size;
	uint8_t			max_policers;
	uint8_t		max_congestion_ctrl;
	struct dpni_ipr_cfg	ipr_cfg;
};

/**
 * dpni_open() - Open a control session for the specified object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @dpni_id:	DPNI unique ID
 * @token:	Returned token; use in subsequent API calls
 *
 * This function can be used to open a control session for an
 * already created object; an object may have been declared in
 * the DPL or by calling the dpni_create() function.
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent commands for
 * this specific object.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_open(struct fsl_mc_io	*mc_io,
	      uint32_t		cmd_flags,
	      int		dpni_id,
	      uint16_t		*token);

/**
 * dpni_close() - Close the control session of the object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPNI object
 *
 * After this function is called, no further operations are
 * allowed on the object without opening a new control session.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_close(struct fsl_mc_io	*mc_io,
	       uint32_t		cmd_flags,
	       uint16_t		token);

/**
 * Data Path Network Interface API
 * Contains initialization APIs and runtime control APIs for DPNI
 */

/** General DPNI macros */

/* use for common tx-conf queue; see dpni_set_tx_conf_<x>() */
#define DPNI_COMMON_TX_CONF			(uint16_t)(-1)

/**
 * struct dpni_extended_cfg - Structure representing extended DPNI configuration
 * @tc_cfg: TCs configuration
 * @ipr_cfg: IP reassembly configuration
 */
struct dpni_extended_cfg {
	/**
	 * struct tc_cfg - TC configuration
	 * @max_dist: Maximum distribution size for Rx traffic class;
	 *	supported values: 1,2,3,4,6,7,8,12,14,16,24,28,32,48,56,64,96,
	 *	112,128,192,224,256,384,448,512,768,896,1024;
	 *	value '0' will be treated as '1'.
	 *	other unsupported values will be round down to the nearest
	 *	supported value.
	 * @max_fs_entries: Maximum FS entries for Rx traffic class;
	 *	'0' means no support for this TC;
	 */
	struct {
		uint16_t	max_dist;
		uint16_t	max_fs_entries;
	} tc_cfg[DPNI_MAX_TC];
	/**
	 * struct ipr_cfg - Structure representing IP reassembly configuration
	 * @max_reass_frm_size: Maximum size of the reassembled frame
	 * @min_frag_size_ipv4: Minimum fragment size of IPv4 fragments
	 * @min_frag_size_ipv6: Minimum fragment size of IPv6 fragments
	 * @max_open_frames_ipv4: Maximum concurrent IPv4 packets in reassembly
	 *		process
	 * @max_open_frames_ipv6: Maximum concurrent IPv6 packets in reassembly
	 *		process
	 */
	struct {
		uint16_t max_reass_frm_size;
		uint16_t min_frag_size_ipv4;
		uint16_t min_frag_size_ipv6;
		uint16_t max_open_frames_ipv4;
		uint16_t max_open_frames_ipv6;
	} ipr_cfg;
};

/**
 * dpni_prepare_extended_cfg() - function prepare extended parameters
 * @cfg: extended structure
 * @ext_cfg_buf: Zeroed 256 bytes of memory before mapping it to DMA
 *
 * This function has to be called before dpni_create()
 */
int dpni_prepare_extended_cfg(const struct dpni_extended_cfg	*cfg,
			      uint8_t			*ext_cfg_buf);

/**
 * struct dpni_cfg - Structure representing DPNI configuration
 * @mac_addr: Primary MAC address
 * @adv: Advanced parameters; default is all zeros;
 *		use this structure to change default settings
 */
struct dpni_cfg_v9 {
	uint8_t mac_addr[6];
	/**
	 * struct adv - Advanced parameters
	 * @options: Mask of available options; use 'DPNI_OPT_<X>' values
	 * @start_hdr: Selects the packet starting header for parsing;
	 *		'NET_PROT_NONE' is treated as default: 'NET_PROT_ETH'
	 * @max_senders: Maximum number of different senders; used as the number
	 *		of dedicated Tx flows; Non-power-of-2 values are rounded
	 *		up to the next power-of-2 value as hardware demands it;
	 *		'0' will be treated as '1'
	 * @max_tcs: Maximum number of traffic classes (for both Tx and Rx);
	 *		'0' will e treated as '1'
	 * @max_unicast_filters: Maximum number of unicast filters;
	 *			'0' is treated	as '16'
	 * @max_multicast_filters: Maximum number of multicast filters;
	 *			'0' is treated as '64'
	 * @max_qos_entries: if 'max_tcs > 1', declares the maximum entries in
	 *			the QoS	table; '0' is treated as '64'
	 * @max_qos_key_size: Maximum key size for the QoS look-up;
	 *			'0' is treated as '24' which is enough for IPv4
	 *			5-tuple
	 * @max_dist_key_size: Maximum key size for the distribution;
	 *		'0' is treated as '24' which is enough for IPv4 5-tuple
	 * @max_policers: Maximum number of policers;
	 *		should be between '0' and max_tcs
	 * @max_congestion_ctrl: Maximum number of congestion control groups
	 *		(CGs); covers early drop and congestion notification
	 *		requirements;
	 *		should be between '0' and ('max_tcs' + 'max_senders')
	 * @ext_cfg_iova: I/O virtual address of 256 bytes DMA-able memory
	 *		filled with the extended configuration by calling
	 *		dpni_prepare_extended_cfg()
	 */
	struct {
		uint32_t	options;
		enum net_prot	start_hdr;
		uint8_t		max_senders;
		uint8_t		max_tcs;
		uint8_t		max_unicast_filters;
		uint8_t		max_multicast_filters;
		uint8_t		max_vlan_filters;
		uint8_t		max_qos_entries;
		uint8_t		max_qos_key_size;
		uint8_t		max_dist_key_size;
		uint8_t		max_policers;
		uint8_t		max_congestion_ctrl;
		uint64_t	ext_cfg_iova;
	} adv;
};

/**
 * dpni_create() - Create the DPNI object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @cfg:	Configuration structure
 * @token:	Returned token; use in subsequent API calls
 *
 * Create the DPNI object, allocate required resources and
 * perform required initialization.
 *
 * The object can be created either by declaring it in the
 * DPL file, or by calling this function.
 *
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent calls to
 * this specific object. For objects that are created using the
 * DPL file, call dpni_open() function to get an authentication
 * token first.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_create_v9(struct fsl_mc_io		  *mc_io,
		   uint32_t			  cmd_flags,
		   const struct dpni_cfg_v9	  *cfg,
		   const struct dpni_extended_cfg *ext_cfg,
		   uint16_t			  *token);

/**
 * dpni_destroy() - Destroy the DPNI object and release all its resources.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPNI object
 *
 * Return:	'0' on Success; error code otherwise.
 */
int dpni_destroy(struct fsl_mc_io	*mc_io,
		 uint32_t		cmd_flags,
		 uint16_t		token);

/**
 * dpni_get_irq_status() - Get the current status of any pending interrupts.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPNI object
 * @irq_index:	The interrupt index to configure
 * @status:	Returned interrupts status - one bit per cause:
 *			0 = no interrupt pending
 *			1 = interrupt pending
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_irq_status_v9(struct fsl_mc_io	*mc_io,
			   uint32_t		cmd_flags,
			   uint16_t		token,
			   uint8_t			irq_index,
			   uint32_t		*status);
/**
 * struct dpni_attr - Structure representing DPNI attributes
 * @id: DPNI object ID
 * @version: DPNI version
 * @start_hdr: Indicates the packet starting header for parsing
 * @options: Mask of available options; reflects the value as was given in
 *		object's creation
 * @max_senders: Maximum number of different senders; used as the number
 *		of dedicated Tx flows;
 * @max_tcs: Maximum number of traffic classes (for both Tx and Rx)
 * @max_unicast_filters: Maximum number of unicast filters
 * @max_multicast_filters: Maximum number of multicast filters
 * @max_vlan_filters: Maximum number of VLAN filters
 * @max_qos_entries: if 'max_tcs > 1', declares the maximum entries in QoS table
 * @max_qos_key_size: Maximum key size for the QoS look-up
 * @max_dist_key_size: Maximum key size for the distribution look-up
 * @max_policers: Maximum number of policers;
 * @max_congestion_ctrl: Maximum number of congestion control groups (CGs);
 * @ext_cfg_iova: I/O virtual address of 256 bytes DMA-able memory;
 *	call dpni_extract_extended_cfg() to extract the extended configuration
 */
struct dpni_attr_v9 {
	int		id;
	/**
	 * struct version - DPNI version
	 * @major: DPNI major version
	 * @minor: DPNI minor version
	 */
	struct {
		uint16_t major;
		uint16_t minor;
	} version;
	enum net_prot	start_hdr;
	uint32_t	options;
	uint8_t		max_senders;
	uint8_t		max_tcs;
	uint8_t		max_unicast_filters;
	uint8_t		max_multicast_filters;
	uint8_t		max_vlan_filters;
	uint8_t		max_qos_entries;
	uint8_t		max_qos_key_size;
	uint8_t		max_dist_key_size;
	uint8_t		max_policers;
	uint8_t		max_congestion_ctrl;
	uint64_t	ext_cfg_iova;
};

/**
 * dpni_get_attributes() - Retrieve DPNI attributes.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPNI object
 * @attr:	Object's attributes
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_attributes_v9(struct fsl_mc_io	*mc_io,
			   uint32_t		cmd_flags,
			   uint16_t		token,
			   struct dpni_attr_v9	*attr,
			   struct dpni_extended_cfg *ext_cfg);

/**
 * dpni_extract_extended_cfg() - extract the extended parameters
 * @cfg: extended structure
 * @ext_cfg_buf: 256 bytes of DMA-able memory
 *
 * This function has to be called after dpni_get_attributes()
 */
int dpni_extract_extended_cfg(struct dpni_extended_cfg	*cfg,
			      const uint8_t		*ext_cfg_buf);

/**
 * dpni_get_irq_mask() - Get interrupt mask.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPNI object
 * @irq_index:	The interrupt index to configure
 * @mask:	Returned event mask to trigger interrupt
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_irq_mask(struct fsl_mc_io	*mc_io,
		      uint32_t		cmd_flags,
		      uint16_t		token,
		      uint8_t		irq_index,
		      uint32_t		*mask);

/**
 * dpni_get_irq_status() - Get the current status of any pending interrupts.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPNI object
 * @irq_index:	The interrupt index to configure
 * @status:	Returned interrupts status - one bit per cause:
 *			0 = no interrupt pending
 *			1 = interrupt pending
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_irq_status(struct fsl_mc_io	*mc_io,
			uint32_t		cmd_flags,
			uint16_t		token,
			uint8_t			irq_index,
			uint32_t		*status);

/**
 * dpni_get_primary_mac_addr() - Get the primary MAC address
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPNI object
 * @mac_addr:	Returned MAC address
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_primary_mac_addr(struct fsl_mc_io	*mc_io,
			      uint32_t		cmd_flags,
			      uint16_t		token,
			      uint8_t		mac_addr[6]);

/**
 * struct dpni_link_state - Structure representing DPNI link state
 * @rate: Rate
 * @options: Mask of available options; use 'DPNI_LINK_OPT_<X>' values
 * @up: Link state; '0' for down, '1' for up
 */
struct dpni_link_state {
	uint32_t	rate;
	uint64_t	options;
	int		up;
};

/**
 * dpni_get_link_state() - Return the link state (either up or down)
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPNI object
 * @state:	Returned link state;
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_link_state(struct fsl_mc_io	*mc_io,
			uint32_t		cmd_flags,
			uint16_t		token,
			struct dpni_link_state	*state);

#endif /* __FSL_DPNI_V9_H */

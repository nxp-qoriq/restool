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
#ifndef __FSL_DPNI_H
#define __FSL_DPNI_H

#include "fsl_dpkg.h"

/* Data Path Network Interface API
 * Contains initialization APIs and runtime control APIs for DPNI
 */

struct fsl_mc_io;

/* General DPNI macros */

/* Maximum number of traffic classes */
#define DPNI_MAX_TC				8
/* Maximum number of buffer pools per DPNI */
#define DPNI_MAX_DPBP				8

/* All traffic classes considered; see dpni_set_rx_flow() */
#define DPNI_ALL_TCS				(uint8_t)(-1)
/* All flows within traffic class considered; see dpni_set_rx_flow() */
#define DPNI_ALL_TC_FLOWS			(uint16_t)(-1)
/* Generate new flow ID; see dpni_set_tx_flow() */
#define DPNI_NEW_FLOW_ID			(uint16_t)(-1)

/**
 * dpni_open() - Open a control session for the specified object
 * @mc_io:	Pointer to MC portal's I/O object
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
int dpni_open(struct fsl_mc_io *mc_io, int dpni_id, uint16_t *token);

/**
 * dpni_close() - Close the control session of the object
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 *
 * After this function is called, no further operations are
 * allowed on the object without opening a new control session.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_close(struct fsl_mc_io *mc_io, uint16_t token);

/* DPNI configuration options */

/* Allow different distribution key profiles for different traffic classes;
 * if not set, a single key profile is assumed
 */
#define DPNI_OPT_ALLOW_DIST_KEY_PER_TC		0x00000001

/* Disable all non-error transmit confirmation; error frames are reported
 * back to a common Tx error queue
 */
#define DPNI_OPT_TX_CONF_DISABLED		0x00000002

/* Disable per-sender private Tx confirmation/error queue */
#define DPNI_OPT_PRIVATE_TX_CONF_ERROR_DISABLED	0x00000004

/* Support distribution based on hashed key;
 * allows statistical distribution over receive queues in a traffic class
 */
#define DPNI_OPT_DIST_HASH			0x00000010

/* Support distribution based on flow steering;
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
	 *			Must be set to the required value minus 1;
	 *			i.e. 0->1, 1->2, ... ,255->256;
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
	 * @ipr_cfg: IP reassembly configuration
	 */
	struct {
		uint64_t options;
		enum net_prot start_hdr;
		uint8_t max_senders;
		uint8_t max_tcs;
		uint8_t max_dist_per_tc[DPNI_MAX_TC];
		uint8_t max_unicast_filters;
		uint8_t max_multicast_filters;
		uint8_t max_vlan_filters;
		uint8_t max_qos_entries;
		uint8_t max_qos_key_size;
		uint8_t max_dist_key_size;
		struct dpni_ipr_cfg ipr_cfg;
	} adv;
};

/**
 * dpni_create() - Create the DPNI object
 * @mc_io:	Pointer to MC portal's I/O object
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
int dpni_create(struct fsl_mc_io	*mc_io,
		const struct dpni_cfg	*cfg,
		uint16_t		*token);

/**
 * dpni_destroy() - Destroy the DPNI object and release all its resources.
 * @mc_io	Pointer to MC portal's I/O object
 * @token	Token of DPNI object
 *
 * Return:	'0' on Success; error code otherwise.
 */
int dpni_destroy(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * struct dpni_pools_cfg - Structure representing buffer pools configuration
 * @num_dpbp: Number of DPBPs
 * @pools: Array of buffer pools parameters; The number of valid entries
 *	must match 'num_dpbp' value
 */
struct dpni_pools_cfg {
	uint8_t num_dpbp;
	/**
	 * struct pools - Buffer pools parameters
	 * @dpbp_id: DPBP object ID
	 * @buffer_size: Buffer size
	 */
	struct {
		int dpbp_id;
		uint16_t buffer_size;
	} pools[DPNI_MAX_DPBP];
};

/**
 * dpni_set_pools() - Set buffer pools configuration
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @cfg:	Buffer pools configuration
 *
 * mandatory for DPNI operation
 * warning:Allowed only when DPNI is disabled
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_pools(struct fsl_mc_io		*mc_io,
		   uint16_t			token,
		   const struct dpni_pools_cfg	*cfg);

/**
 * dpni_enable() - Enable the DPNI, allow sending and receiving frames.
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPNI object
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_enable(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * dpni_disable() - Disable the DPNI, stop sending and receiving frames.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_disable(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * dpni_is_enabled() - Check if the DPNI is enabled.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @en:		Returns '1' if object is enabled; '0' otherwise
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_is_enabled(struct fsl_mc_io *mc_io, uint16_t token, int *en);

/**
 * @dpni_reset() - Reset the DPNI, returns the object to initial state.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_reset(struct fsl_mc_io *mc_io, uint16_t token);

/* DPNI IRQ Index and Events */

/* IRQ index */
#define DPNI_IRQ_INDEX				0
/* IRQ event - indicates a change in link state */
#define DPNI_IRQ_EVENT_LINK_CHANGED		0x00000001

/**
 * dpni_set_irq() - Set IRQ information for the DPNI to trigger an interrupt.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @irq_index:	Identifies the interrupt index to configure
 * @irq_addr:	Address that must be written to
 *			signal a message-based interrupt
 * @irq_val:	Value to write into irq_addr address
 * @user_irq_id: A user defined number associated with this IRQ
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_irq(struct fsl_mc_io	*mc_io,
		 uint16_t		token,
		 uint8_t		irq_index,
		 uint64_t		irq_addr,
		 uint32_t		irq_val,
		 int			user_irq_id);

/**
 * dpni_get_irq() - Get IRQ information from the DPNI.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @irq_index:	The interrupt index to configure
 * @type:	Interrupt type: 0 represents message interrupt
 *			type (both irq_addr and irq_val are valid)
 * @irq_addr:	Returned address that must be written to
 *			signal the message-based interrupt
 * @irq_val:	Value to write into irq_addr address
 * @user_irq_id: Returned a user defined number associated with this IRQ
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_irq(struct fsl_mc_io	*mc_io,
		 uint16_t		token,
		 uint8_t		irq_index,
		 int			*type,
		 uint64_t		*irq_addr,
		 uint32_t		*irq_val,
		 int			*user_irq_id);

/**
 * dpni_set_irq_enable() - Set overall interrupt state.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @irq_index:	The interrupt index to configure
 * @en:		Interrupt state: - enable = 1, disable = 0
 *
 * Allows GPP software to control when interrupts are generated.
 * Each interrupt can have up to 32 causes.  The enable/disable control's the
 * overall interrupt state. if the interrupt is disabled no causes will cause
 * an interrupt.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_irq_enable(struct fsl_mc_io	*mc_io,
			uint16_t		token,
			uint8_t			irq_index,
			uint8_t			en);

/**
 * dpni_get_irq_enable() - Get overall interrupt state
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @irq_index:	The interrupt index to configure
 * @en:		Returned interrupt state - enable = 1, disable = 0
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_irq_enable(struct fsl_mc_io	*mc_io,
			uint16_t		token,
			uint8_t			irq_index,
			uint8_t			*en);

/**
 * dpni_set_irq_mask() - Set interrupt mask.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @irq_index:	The interrupt index to configure
 * @mask:	event mask to trigger interrupt;
 *			each bit:
 *				0 = ignore event
 *				1 = consider event for asserting IRQ
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_irq_mask(struct fsl_mc_io	*mc_io,
		      uint16_t		token,
		      uint8_t		irq_index,
		      uint32_t		mask);

/**
 * dpni_get_irq_mask() - Get interrupt mask.
 * @mc_io:	Pointer to MC portal's I/O object
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
		      uint16_t		token,
		      uint8_t		irq_index,
		      uint32_t		*mask);

/**
 * dpni_get_irq_status() - Get the current status of any pending interrupts.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @irq_index:	The interrupt index to configure
 * @status:	Returned interrupts status - one bit per cause:
 *			0 = no interrupt pending
 *			1 = interrupt pending
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_irq_status(struct fsl_mc_io	*mc_io,
			uint16_t		token,
			uint8_t			irq_index,
			uint32_t		*status);

/**
 * dpni_clear_irq_status() - Clear a pending interrupt's status
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @irq_index:	The interrupt index to configure
 * @status:	bits to clear (W1C) - one bit per cause:
 *			0 = don't change
 *			1 = clear status bit
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_clear_irq_status(struct fsl_mc_io	*mc_io,
			  uint16_t		token,
			  uint8_t		irq_index,
			  uint32_t		status);

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
 * @max_dist_per_tc: Maximum distribution size per Rx traffic class;
 *			Set to the required value minus 1
 * @max_unicast_filters: Maximum number of unicast filters
 * @max_multicast_filters: Maximum number of multicast filters
 * @max_vlan_filters: Maximum number of VLAN filters
 * @max_qos_entries: if 'max_tcs > 1', declares the maximum entries in QoS table
 * @max_qos_key_size: Maximum key size for the QoS look-up
 * @max_dist_key_size: Maximum key size for the distribution look-up
 * @ipr_cfg: IP reassembly configuration
 */
struct dpni_attr {
	int id;
	/**
	 * struct version - DPNI version
	 * @major: DPNI major version
	 * @minor: DPNI minor version
	 */
	struct {
		uint16_t major;
		uint16_t minor;
	} version;
	enum net_prot start_hdr;
	uint64_t options;
	uint8_t max_senders;
	uint8_t max_tcs;
	uint8_t max_dist_per_tc[DPNI_MAX_TC];
	uint8_t max_unicast_filters;
	uint8_t max_multicast_filters;
	uint8_t max_vlan_filters;
	uint8_t max_qos_entries;
	uint8_t max_qos_key_size;
	uint8_t max_dist_key_size;
	struct dpni_ipr_cfg ipr_cfg;
};

/**
 * dpni_get_attributes() - Retrieve DPNI attributes.
 * @mc_io:	Pointer to MC portal's I/O objec
 * @token:	Token of DPNI object
 * @attr:	Returned object's attributes
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_attributes(struct fsl_mc_io	*mc_io,
			uint16_t		token,
			struct dpni_attr	*attr);

/* DPNI errors */

/* Extract out of frame header error */
#define DPNI_ERROR_EOFHE	0x00020000
/* Frame length error */
#define DPNI_ERROR_FLE		0x00002000
/* Frame physical error */
#define DPNI_ERROR_FPE		0x00001000
/* Parsing header error */
#define DPNI_ERROR_PHE		0x00000020
/* Parser L3 checksum error */
#define DPNI_ERROR_L3CE		0x00000004
/* Parser L3 checksum error */
#define DPNI_ERROR_L4CE		0x00000001


/**
 *  enum dpni_error_action - Defines DPNI behavior for errors
 *  @DPNI_ERROR_ACTION_DISCARD: Discard the frame
 *  @DPNI_ERROR_ACTION_CONTINUE: Continue with the normal flow
 *  @DPNI_ERROR_ACTION_SEND_TO_ERROR_QUEUE: Send the frame to the error queue
 */

enum dpni_error_action {
	DPNI_ERROR_ACTION_DISCARD = 0,
	DPNI_ERROR_ACTION_CONTINUE = 1,
	DPNI_ERROR_ACTION_SEND_TO_ERROR_QUEUE = 2
};

/**
 * struct dpni_error_cfg - Structure representing DPNI errors treatment
 * @errors: Errors mask; use 'DPNI_ERROR__<X>
 * @error_action: The desired action for the errors mask
 * @set_frame_annotation: Set to '1' to mark the errors in frame annotation
 *		status (FAS); relevant only for the non-discard action
 */
struct dpni_error_cfg {
	uint32_t errors;
	enum dpni_error_action error_action;
	int set_frame_annotation;
};

/**
 * dpni_set_errors_behavior() - Set errors behavior
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @cfg:	Errors configuration
 *
 * this function may be called numerous times with different
 * error masks
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_errors_behavior(struct fsl_mc_io		*mc_io,
			     uint16_t			token,
			     struct dpni_error_cfg	*cfg);

/* DPNI buffer layout modification options */

/* Select to modify the time-stamp setting */
#define DPNI_BUF_LAYOUT_OPT_TIMESTAMP		0x00000001
/* Select to modify the parser-result setting; not applicable for Tx */
#define DPNI_BUF_LAYOUT_OPT_PARSER_RESULT	0x00000002
/* Select to modify the frame-status setting */
#define DPNI_BUF_LAYOUT_OPT_FRAME_STATUS	0x00000004
/* Select to modify the private-data-size setting */
#define DPNI_BUF_LAYOUT_OPT_PRIVATE_DATA_SIZE	0x00000008
/* Select to modify the data-alignment setting */
#define DPNI_BUF_LAYOUT_OPT_DATA_ALIGN		0x00000010
/* Select to modify the data-head-room setting */
#define DPNI_BUF_LAYOUT_OPT_DATA_HEAD_ROOM	0x00000020
/*!< Select to modify the data-tail-room setting */
#define DPNI_BUF_LAYOUT_OPT_DATA_TAIL_ROOM	0x00000040

/**
 * struct dpni_buffer_layout - Structure representing DPNI buffer layout
 * @options: Flags representing the suggested modifications to the buffer
 *		layout; Use any combination of 'DPNI_BUF_LAYOUT_OPT_<X>' flags
 * @pass_timestamp: Pass timestamp value
 * @pass_parser_result: Pass parser results
 * @pass_frame_status: Pass frame status
 * @private_data_size: Size kept for private data (in bytes)
 * @data_align: Data alignment
 * @data_head_room: Data head room
 * @data_tail_room: Data tail room
 */
struct dpni_buffer_layout {
	uint32_t options;
	int pass_timestamp;
	int pass_parser_result;
	int pass_frame_status;
	uint16_t private_data_size;
	uint16_t data_align;
	uint16_t data_head_room;
	uint16_t data_tail_room;
};

/**
 * dpni_get_rx_buffer_layout() - Retrieve Rx buffer layout attributes.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @layout:	Returns buffer layout attributes
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_rx_buffer_layout(struct fsl_mc_io		*mc_io,
			      uint16_t			token,
			      struct dpni_buffer_layout	*layout);

/**
 * dpni_set_rx_buffer_layout() - Set Rx buffer layout configuration.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @layout:	Buffer layout configuration
 *
 * Return:	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 */
int dpni_set_rx_buffer_layout(struct fsl_mc_io			*mc_io,
			      uint16_t				token,
			      const struct dpni_buffer_layout	*layout);

/**
 * dpni_get_tx_buffer_layout() - Retrieve Tx buffer layout attributes.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @layout:	Returns buffer layout attributes
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_buffer_layout(struct fsl_mc_io		*mc_io,
			      uint16_t			token,
			      struct dpni_buffer_layout	*layout);

/**
 * dpni_set_tx_buffer_layout() - Set Tx buffer layout configuration.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @layout:	Buffer layout configuration
 *
 * Return:	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 */
int dpni_set_tx_buffer_layout(struct fsl_mc_io			*mc_io,
			      uint16_t				token,
			      const struct dpni_buffer_layout	*layout);

/**
 * dpni_get_tx_conf_buffer_layout() - Retrieve Tx confirmation buffer layout
 *				attributes.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @layout:	Returns buffer layout attributes
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_conf_buffer_layout(struct fsl_mc_io		*mc_io,
				   uint16_t			token,
				   struct dpni_buffer_layout	*layout);

/**
 * dpni_set_tx_conf_buffer_layout() - Set Tx confirmation buffer layout
 *					configuration.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @layout:	Buffer layout configuration
 *
 * Return:	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 */
int dpni_set_tx_conf_buffer_layout(struct fsl_mc_io		   *mc_io,
				   uint16_t			   token,
				   const struct dpni_buffer_layout *layout);

/**
 * dpni_set_l3_chksum_validation() - Enable/disable L3 checksum validation
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @en:		Set to '1' to enable; '0' to disable
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_l3_chksum_validation(struct fsl_mc_io	*mc_io,
				  uint16_t		token,
				  int			en);

/**
 * dpni_get_l3_chksum_validation() - Get L3 checksum validation mode
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @en:		Returns '1' if enabled; '0' otherwise
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_l3_chksum_validation(struct fsl_mc_io	*mc_io,
				  uint16_t		token,
				  int			*en);

/**
 * dpni_set_l4_chksum_validation() - Enable/disable L4 checksum validation
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @en:		Set to '1' to enable; '0' to disable
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_l4_chksum_validation(struct fsl_mc_io	*mc_io,
				  uint16_t		token,
				  int			en);

/**
 * dpni_get_l4_chksum_validation() - Get L4 checksum validation mode
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @en:		Returns '1' if enabled; '0' otherwise
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_l4_chksum_validation(struct fsl_mc_io	*mc_io,
				  uint16_t		token,
				  int			*en);

/**
 * dpni_get_qdid() - Get the Queuing Destination ID (QDID) that should be used
 *			for enqueue operations
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @qdid:	Returned virtual QDID value that should be used as an argument
 *			in all enqueue operations
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_qdid(struct fsl_mc_io *mc_io, uint16_t token, uint16_t *qdid);

/**
 * dpni_get_spid() - Get the AIOP storage profile ID associated with the DPNI
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @spid:	Returned aiop storage-profile ID
 *
 * Return:	'0' on Success; Error code otherwise.
 *
 * @warning	Only relevant for DPNI that belongs to AIOP container.
 */
int dpni_get_spid(struct fsl_mc_io *mc_io, uint16_t token, uint16_t *spid);

/**
 * dpni_get_tx_data_offset() - Get the Tx data offset (from start of buffer)
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @data_offset: Tx data offset (from start of buffer)
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_data_offset(struct fsl_mc_io	*mc_io,
			    uint16_t		token,
			    uint16_t		*data_offset);

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
 * dpni_get_counter() - Read a specific DPNI counter
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @counter:	The requested counter
 * @value:	Returned counter's current value
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_counter(struct fsl_mc_io	*mc_io,
		     uint16_t		token,
		     enum dpni_counter	counter,
		     uint64_t		*value);

/**
 * dpni_set_counter() - Set (or clear) a specific DPNI counter
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @counter:	The requested counter
 * @value:	New counter value; typically pass '0' for resetting
 *			the counter.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_counter(struct fsl_mc_io	*mc_io,
		     uint16_t		token,
		     enum dpni_counter	counter,
		     uint64_t		value);

/**
 * struct - Structure representing DPNI link configuration
 * @rate: Rate
 * @options: Mask of available options; use 'DPNI_LINK_OPT_<X>' values
 */
struct dpni_link_cfg {
	uint64_t rate;
	uint64_t options;
};

/**
 * dpni_set_link_cfg() - set the link configuration.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @cfg:	Link configuration
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_link_cfg(struct fsl_mc_io *mc_io,
		      uint16_t token,
		      struct dpni_link_cfg *cfg);

/**
 * struct dpni_link_state - Structure representing DPNI link state
 * @rate: Rate
 * @options: Mask of available options; use 'DPNI_LINK_OPT_<X>' values
 * @up: Link state; '0' for down, '1' for up
 */
struct dpni_link_state {
	uint64_t rate;
	uint64_t options;
	int up;
};

/**
 * dpni_get_link_state() - Return the link state (either up or down)
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @state:	Returned link state;
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_link_state(struct fsl_mc_io *mc_io,
			uint16_t token,
			struct dpni_link_state *state);

/**
 * dpni_set_max_frame_length() - Set the maximum received frame length.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @max_frame_length:	Maximum received frame length (in
 *				bytes); frame is discarded if its
 *				length exceeds this value
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_max_frame_length(struct fsl_mc_io	*mc_io,
			      uint16_t		token,
			      uint16_t		max_frame_length);

/**
 * dpni_get_max_frame_length() - Get the maximum received frame length.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @max_frame_length:	Maximum received frame length (in
 *				bytes); frame is discarded if its
 *				length exceeds this value
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_max_frame_length(struct fsl_mc_io	*mc_io,
			      uint16_t		token,
			      uint16_t		*max_frame_length);

/**
 * dpni_set_mtu() - Set the MTU for the interface.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @mtu:	MTU length (in bytes)
 *
 * MTU determines the maximum fragment size for performing IP
 * fragmentation on egress packets.
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_mtu(struct fsl_mc_io *mc_io, uint16_t token, uint16_t mtu);

/**
 * dpni_get_mtu() - Get the MTU.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @mtu:	Returned MTU length (in bytes)
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_mtu(struct fsl_mc_io *mc_io, uint16_t token, uint16_t *mtu);

/**
 * dpni_set_multicast_promisc() - Enable/disable multicast promiscuous mode
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @en:		Set to '1' to enable; '0' to disable
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_multicast_promisc(struct fsl_mc_io	*mc_io,
			       uint16_t		token,
			       int		en);

/**
 * dpni_get_multicast_promisc() - Get multicast promiscuous mode
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @en:		Returns '1' if enabled; '0' otherwise
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_multicast_promisc(struct fsl_mc_io	*mc_io,
			       uint16_t		token,
			       int		*en);

/**
 * dpni_set_unicast_promisc() - Enable/disable unicast promiscuous mode
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @en:		Set to '1' to enable; '0' to disable
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_unicast_promisc(struct fsl_mc_io *mc_io, uint16_t token, int en);

/**
 * dpni_get_unicast_promisc() - Get unicast promiscuous mode
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @en:		Returns '1' if enabled; '0' otherwise
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_unicast_promisc(struct fsl_mc_io *mc_io, uint16_t token, int *en);

/**
 * dpni_set_primary_mac_addr() - Set the primary MAC address
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @mac_addr:	MAC address to set as primary address
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_primary_mac_addr(struct fsl_mc_io	*mc_io,
			      uint16_t		token,
			      const uint8_t	mac_addr[6]);

/**
 * dpni_get_primary_mac_addr() - Get the primary MAC address
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @mac_addr:	Returned MAC address
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_primary_mac_addr(struct fsl_mc_io	*mc_io,
			      uint16_t		token,
			      uint8_t		mac_addr[6]);

/**
 * dpni_add_mac_addr() - Add MAC address filter
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @mac_addr:	MAC address to add
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_add_mac_addr(struct fsl_mc_io	*mc_io,
		      uint16_t		token,
		      const uint8_t	mac_addr[6]);

/**
 * dpni_remove_mac_addr() - Remove MAC address filter
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @mac_addr:	MAC address to remove
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_remove_mac_addr(struct fsl_mc_io	*mc_io,
			 uint16_t		token,
			 const uint8_t		mac_addr[6]);

/**
 * dpni_clear_mac_filters() - Clear all unicast and/or multicast MAC filters
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @unicast:	Set to '1' to clear unicast addresses
 * @multicast:	Set to '1' to clear multicast addresses
 *
 * The primary MAC address is not cleared by this operation.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_clear_mac_filters(struct fsl_mc_io	*mc_io,
			   uint16_t		token,
			   int			unicast,
			   int			multicast);

/**
 * dpni_set_vlan_filters() - Enable/disable VLAN filtering mode
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @en:		Set to '1' to enable; '0' to disable
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_vlan_filters(struct fsl_mc_io *mc_io, uint16_t token, int en);

/**
 * dpni_add_vlan_id() - Add VLAN ID filter
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @vlan_id:	VLAN ID to add
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_add_vlan_id(struct fsl_mc_io	*mc_io,
		     uint16_t		token,
		     uint16_t		vlan_id);

/**
 * dpni_remove_vlan_id() - Remove VLAN ID filter
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @vlan_id:	VLAN ID to remove
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_remove_vlan_id(struct fsl_mc_io	*mc_io,
			uint16_t		token,
			uint16_t		vlan_id);

/**
 * dpni_clear_vlan_filters() - Clear all VLAN filters
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_clear_vlan_filters(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * struct dpni_tx_tc_cfg - Structure representing Tx traffic class configuration
 * @depth_limit: Limit the depth of a queue to the given value; note, that this
 *		may result in frames being rejected from the queue;
 *		set to '0' to remove any limitation
 */
struct dpni_tx_tc_cfg {
	uint16_t depth_limit;
};

/**
 * dpni_set_tx_tc() - Set Tx traffic class configuration
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @tc_id:	Traffic class selection (0-7)
 * @cfg:	Traffic class configuration
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_tx_tc(struct fsl_mc_io		*mc_io,
		   uint16_t			token,
		   uint8_t			tc_id,
		   const struct dpni_tx_tc_cfg	*cfg);

/**
 * enum dpni_dist_mode - DPNI distribution mode
 * @DPNI_DIST_MODE_NONE: No distribution
 * @DPNI_DIST_MODE_HASH: Use hash distribution; only relevant if
 *		the 'DPNI_OPT_DIST_HASH' option was set at DPNI creation
 * @DPNI_DIST_MODE_FS:  Use explicit flow steering; only relevant if
 *	 the 'DPNI_OPT_DIST_FS' option was set at DPNI creation
 */
enum dpni_dist_mode {
	DPNI_DIST_MODE_NONE = 0,
	DPNI_DIST_MODE_HASH = 1,
	DPNI_DIST_MODE_FS = 2
};

/**
 * enum dpni_fs_miss_action -   DPNI Flow Steering miss action
 * @DPNI_FS_MISS_DROP: In case of no-match, drop the frame
 * @DPNI_FS_MISS_EXPLICIT_FLOWID: In case of no-match, use explicit flow-id
 * @DPNI_FS_MISS_HASH: In case of no-match, distribute using hash
 */
enum dpni_fs_miss_action {
	DPNI_FS_MISS_DROP = 0,
	DPNI_FS_MISS_EXPLICIT_FLOWID = 1,
	DPNI_FS_MISS_HASH = 2
};

/**
 * struct dpni_fs_tbl_cfg - Flow Steering table configuration
 * @miss_action: Miss action selection
 * @default_flow_id: Used when 'miss_action = DPNI_FS_MISS_EXPLICIT_FLOWID'
 */
struct dpni_fs_tbl_cfg {
	enum dpni_fs_miss_action miss_action;
	uint16_t default_flow_id;
};

/**
 * struct dpni_rx_tc_dist_cfg - Rx traffic class distribution configuration
 * @dist_size: Set the distribution size; Must be set to the required value
 *		minus 1, for example: 0->1, 1->2, ... ,255->256;
 *		Non-power-of-2 values are rounded up to the next power-of-2
 *		value as HW demands it
 * @dist_mode: Distribution mode
 * @dist_key_cfg: Select the extractions to be used for the distribution key
 * @fs_cfg: Flow Steering table configuration; only relevant if
 *		'dist_mode = DPNI_DIST_MODE_FS'
 */
struct dpni_rx_tc_dist_cfg {
	uint8_t dist_size;
	enum dpni_dist_mode dist_mode;
	struct dpkg_profile_cfg *dist_key_cfg;
	struct dpni_fs_tbl_cfg fs_cfg;
};

/**
 * dpni_set_rx_tc_dist() - Set Rx traffic class distribution configuration
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @tc_id:	Traffic class selection (0-7)
 * @cfg:	Traffic class distribution configuration
 * @params_iova: I/O virtual address of zeroed 256 bytes of
 *				DMA-able memory
 *
 * warning: Allowed only when DPNI is disabled
 *
 * Return:	'0' on Success; error code otherwise.
 */
int dpni_set_rx_tc_dist(struct fsl_mc_io			*mc_io,
			uint16_t				token,
			uint8_t					tc_id,
			const struct dpni_rx_tc_dist_cfg	*cfg,
			uint64_t				params_iova);

/**
 * enum dpni_dest - DPNI destination types
 * DPNI_DEST_NONE: Unassigned destination; The queue is set in parked mode and
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
 * struct dpni_dest_cfg - Structure representing DPNI destination parameters
 * @dest_type: Destination type
 * @dest_id: Either DPIO ID or DPCON ID, depending on the destination type
 * @priority: Priority selection within the DPIO or DPCON channel; valid values
 *		are 0-1 or 0-7, depending on the number of priorities in that
 *		channel; not relevant for 'DPNI_DEST_NONE' option
 */
struct dpni_dest_cfg {
	enum dpni_dest dest_type;
	int dest_id;
	uint8_t priority;
};

/* DPNI queue modification options */

/* Select to modify the user's context associated with the queue */
#define DPNI_QUEUE_OPT_USER_CTX		0x00000001
/* Select to modify the queue's destination */
#define DPNI_QUEUE_OPT_DEST		0x00000002


/**
 * struct dpni_queue_cfg - Structure representing queue configuration
 * @options: Flags representing the suggested modifications to the queue;
 *		Use any combination of 'DPNI_QUEUE_OPT_<X>' flags
 * @user_ctx: User context value provided in the frame descriptor of each
 *		dequeued frame; valid only if 'DPNI_QUEUE_OPT_USER_CTX'
 *		is contained in 'options'
 * @dest_cfg: Queue destination parameters;
 *		valid only if 'DPNI_QUEUE_OPT_DEST' is contained in 'options'
 */
struct dpni_queue_cfg {
	uint32_t options;
	uint64_t user_ctx;
	struct dpni_dest_cfg dest_cfg;
};

/**
 * struct dpni_queue_attr - Structure representing queue attributes
 * @user_ctx: User context value provided in the frame descriptor of each
 *	dequeued frame
 * @dest_cfg: Queue destination configuration
 * @fqid: Virtual fqid value to be used for dequeue operations
 */
struct dpni_queue_attr {
	uint64_t user_ctx;
	struct dpni_dest_cfg dest_cfg;
	uint32_t fqid;
};

/* DPNI Tx flow modification options */

/* Select to modify the settings for dedicate Tx confirmation/error */
#define DPNI_TX_FLOW_OPT_TX_CONF_ERROR	0x00000001
/*!< Select to modify the Tx confirmation and/or error setting */
#define DPNI_TX_FLOW_OPT_ONLY_TX_ERROR	0x00000002
/*!< Select to modify the queue configuration */
#define DPNI_TX_FLOW_OPT_QUEUE		0x00000004
/*!< Select to modify the L3 checksum generation setting */
#define DPNI_TX_FLOW_OPT_L3_CHKSUM_GEN	0x00000010
/*!< Select to modify the L4 checksum generation setting */
#define DPNI_TX_FLOW_OPT_L4_CHKSUM_GEN	0x00000020

/**
 * struct dpni_tx_flow_cfg - Structure representing Tx flow configuration
 * @options: Flags representing the suggested modifications to the Tx flow;
 *		Use any combination 'DPNI_TX_FLOW_OPT_<X>' flags
 * @conf_err_cfg: Tx confirmation and error configuration; these settings are
 *		ignored if 'DPNI_OPT_PRIVATE_TX_CONF_ERROR_DISABLED' was set at
 *		DPNI creation
 * @l3_chksum_gen: Set to '1' to enable L3 checksum generation; '0' to disable;
 *		valid only if 'DPNI_TX_FLOW_OPT_L3_CHKSUM_GEN' is contained in
 *		'options'
 * @l4_chksum_gen: Set to '1' to enable L4 checksum generation; '0' to disable;
 *		valid only if 'DPNI_TX_FLOW_OPT_L4_CHKSUM_GEN' is contained in
 *		'options'
 */
struct dpni_tx_flow_cfg {
	uint32_t options;
	/**
	 * struct cnf_err_cfg - Tx confirmation and error configuration
	 * @use_default_queue: Set to '1' to use the common (default) Tx
	 *		confirmation and error queue; Set to '0' to use the
	 *		private Tx confirmation and error queue; valid only if
	 *		'DPNI_TX_FLOW_OPT_TX_CONF_ERROR' is contained in
	 *		'options'
	 * @errors_only: Set to '1' to report back only error frames;
	 *		Set to '0' to confirm transmission/error for all
	 *		transmitted frames;
	 *		valid only if 'DPNI_TX_FLOW_OPT_ONLY_TX_ERROR' is
	 *		contained in 'options' and 'use_default_queue = 0';
	 * @queue_cfg: Queue configuration; valid only if
	 *		'DPNI_TX_FLOW_OPT_QUEUE' is contained in 'options'
	 */
	struct {
		int use_default_queue;
		int errors_only;
		struct dpni_queue_cfg queue_cfg;
	} conf_err_cfg;
	int l3_chksum_gen;
	int l4_chksum_gen;
};

/**
 * dpni_set_tx_flow() - Set Tx flow configuration
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @flow_id:	Provides (or returns) the sender's flow ID;
 *				for each new sender set (*flow_id) to
 *				'DPNI_NEW_FLOW_ID' to generate a new flow_id;
 *				this ID should be used as the QDBIN argument
 *				in enqueue operations
 * @cfg:	Tx flow configuration
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_tx_flow(struct fsl_mc_io			*mc_io,
		     uint16_t				token,
		     uint16_t				*flow_id,
		     const struct dpni_tx_flow_cfg	*cfg);

/**
 * struct dpni_tx_flow_attr - Structure representing Tx flow attributes
 * @conf_err_attr: Tx confirmation and error attributes
 * @l3_chksum_gen: '1' if L3 checksum generation is enabled; '0' if disabled
 * @l4_chksum_gen: '1' if L4 checksum generation is enabled; '0' if disabled
 */
struct dpni_tx_flow_attr {
	/**
	 * struct conf_err_attr - Tx confirmation and error attributes
	 * @use_default_queue: '1' if using common (default) Tx confirmation and
	 *			error queue;
	 *			'0' if using private Tx confirmation and error
	 *			queue
	 * @errors_only: '1' if only error frames are reported back; '0' if all
	 *		transmitted frames are confirmed
	 * @queue_attr: Queue attributes
	 */
	struct {
		int use_default_queue;
		int errors_only;
		struct dpni_queue_attr queue_attr;
	} conf_err_attr;
	int l3_chksum_gen;
	int l4_chksum_gen;
};

/**
 * dpni_get_tx_flow() - Get Tx flow attributes
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @flow_id:	The sender's flow ID, as returned by the
 *			dpni_set_tx_flow() function
 * @attr:	Returned Tx flow attributes
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_flow(struct fsl_mc_io		*mc_io,
		     uint16_t			token,
		     uint16_t			flow_id,
		     struct dpni_tx_flow_attr	*attr);

/**
 * dpni_set_rx_flow() - Set Rx flow configuration
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @tc_id:	Traffic class selection (0-7);
 *			use 'DPNI_ALL_TCS' to set all TCs and all flows
 * @flow_id	Rx flow id within the traffic class; use
 *			'DPNI_ALL_TC_FLOWS' to set all flows within
 *			this tc_id; ignored if tc_id is set to
 *			'DPNI_ALL_TCS';
 * @cfg:	Rx flow configuration
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_rx_flow(struct fsl_mc_io			*mc_io,
		     uint16_t				token,
		     uint8_t				tc_id,
		     uint16_t				flow_id,
		     const struct dpni_queue_cfg	*cfg);

/**
 * dpni_get_rx_flow() -	Get Rx flow attributes
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @tc_id:	Traffic class selection (0-7)
 * @flow_id:	Rx flow id within the traffic class
 * @attr:	Returned Rx flow attributes
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_rx_flow(struct fsl_mc_io		*mc_io,
		     uint16_t			token,
		     uint8_t			tc_id,
		     uint16_t			flow_id,
		     struct dpni_queue_attr	*attr);

/**
 * dpni_set_rx_err_queue() - Set Rx error queue configuration
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @cfg:	Queue configuration
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_rx_err_queue(struct fsl_mc_io		*mc_io,
			  uint16_t			token,
			  const struct dpni_queue_cfg	*cfg);

/**
 * dpni_get_rx_err_queue() - Get Rx error queue attributes
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @attr:	Returned Queue attributes
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_rx_err_queue(struct fsl_mc_io		*mc_io,
			  uint16_t			token,
			  struct dpni_queue_attr	*attr);

/**
 * dpni_set_tx_conf_err_queue() - Set Tx confirmation and error queue
 *			configuration
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @cfg:	Queue configuration
 *
 * If 'DPNI_OPT_TX_CONF_DISABLED' was selected at DPNI creation,
 * only error frames are reported back - successfully transmitted
 * frames are not confirmed. Otherwise, all transmitted frames
 * are sent for confirmation.
 *
 * If private Tx confirmation and error queues are used with this
 * DPNI, then this queue serves all Tx flows that are configured
 * with 'use_default_queue' option (see dpni_tx_flow_cfg).
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_tx_conf_err_queue(struct fsl_mc_io			*mc_io,
			       uint16_t				token,
			       const struct dpni_queue_cfg	*cfg);

/**
 * dpni_get_tx_conf_err_queue() - Get Tx confirmation and error queue attributes
 * @mc_io	Pointer to MC portal's I/O object
 * @token	Token of DPNI object
 * @attr	Returned queue attributes
 *
 * If 'DPNI_OPT_TX_CONF_DISABLED' was selected at DPNI creation,
 * only error frames are reported back - successfully transmitted
 * frames are not confirmed. Otherwise, all transmitted frames
 * are sent for confirmation.
 *
 * If private Tx confirmation and error queues are used with this
 * DPNI, then this queue serves all Tx flows that are configured
 * with 'use_default_queue' option (see dpni_tx_flow_cfg).
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_conf_err_queue(struct fsl_mc_io		*mc_io,
			       uint16_t			token,
			       struct dpni_queue_attr	*attr);

/**
 * struct dpni_qos_tbl_cfg - Structure representing QOS table configuration
 * @qos_key_cfg: Selects key extractions to be used as the QoS criteria
 * @discard_on_miss: Set to '1' to discard frames in case of no match (miss);
 *		'0' to use the 'default_tc' in such cases
 * @default_tc: Used in case of no-match and 'discard_on_miss'= 0
 */
struct dpni_qos_tbl_cfg {
	struct dpkg_profile_cfg *qos_key_cfg;
	int discard_on_miss;
	uint8_t default_tc;
};

/**
 * dpni_set_qos_table() - Set QoS mapping table
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @cfg:	QoS table configuration
 * @params_iova: I/O virtual address of zeroed 256 bytes of
 *			DMA-able memory
 *
 * This function and all QoS-related functions require that
 *'max_tcs > 1' was set at DPNI creation.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_qos_table(struct fsl_mc_io			*mc_io,
		       uint16_t				token,
		       const struct dpni_qos_tbl_cfg	*cfg,
		       uint64_t				params_iova);

/**
 * struct dpni_rule_cfg - Rule configuration for table lookup
 * @key_iova: I/O virtual address of the key (must be in DMA-able memory)
 * @mask_iova: I/O virtual address of the mask (must be in DMA-able memory)
 * @key_size: key and mask size (in bytes)
 */
struct dpni_rule_cfg {
	uint64_t key_iova;
	uint64_t mask_iova;
	uint8_t key_size;
};

/**
 * dpni_add_qos_entry() - Add QoS mapping entry (to select a traffic class)
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @cfg:	QoS rule to add
 * @tc_id:	Traffic class selection (0-7)
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_add_qos_entry(struct fsl_mc_io			*mc_io,
		       uint16_t				token,
		       const struct dpni_rule_cfg	*cfg,
		       uint8_t				tc_id);

/**
 * dpni_remove_qos_entry() - Remove QoS mapping entry
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @cfg:	QoS rule to remove
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_remove_qos_entry(struct fsl_mc_io		*mc_io,
			  uint16_t			token,
			  const struct dpni_rule_cfg	*cfg);

/**
 * dpni_clear_qos_table() - Clear all QoS mapping entries
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 *
 * Following this function call, all frames are directed to
 * the default traffic class (0)
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_clear_qos_table(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * dpni_add_fs_entry() - Add Flow Steering entry for a specific traffic class
 *			(to select a flow ID)
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @tc_id:	Traffic class selection (0-7)
 * @cfg:	Flow steering rule to add
 * @flow_id:	Flow id selection (must be smaller than the
 *			distribution size of the traffic class)
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_add_fs_entry(struct fsl_mc_io			*mc_io,
		      uint16_t				token,
		      uint8_t				tc_id,
		      const struct dpni_rule_cfg	*cfg,
		      uint16_t				flow_id);

/**
 * dpni_remove_fs_entry() - Remove Flow Steering entry from a specific
 *			traffic class
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @tc_id:	Traffic class selection (0-7)
 * @cfg:	Flow steering rule to remove
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_remove_fs_entry(struct fsl_mc_io		*mc_io,
			 uint16_t			token,
			 uint8_t			tc_id,
			 const struct dpni_rule_cfg	*cfg);

/**
 * dpni_clear_fs_entries() - Clear all Flow Steering entries of a specific
 *			traffic class
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @tc_id:	Traffic class selection (0-7)
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_clear_fs_entries(struct fsl_mc_io	*mc_io,
			  uint16_t		token,
			  uint8_t		tc_id);

/**
 * dpni_set_vlan_insertion() - Enable/disable VLAN insertion for egress frames
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @en:		Set to '1' to enable; '0' to disable
 *
 * Requires that the 'DPNI_OPT_VLAN_MANIPULATION' option is set
 * at DPNI creation.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_vlan_insertion(struct fsl_mc_io *mc_io, uint16_t token, int en);

/**
 * dpni_set_vlan_removal() - Enable/disable VLAN removal for ingress frames
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @en:		Set to '1' to enable; '0' to disable
 *
 * Requires that the 'DPNI_OPT_VLAN_MANIPULATION' option is set
 * at DPNI creation.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_vlan_removal(struct fsl_mc_io *mc_io, uint16_t token, int en);

/**
 * dpni_set_ipr() - Enable/disable IP reassembly of ingress frames
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @en:		Set to '1' to enable; '0' to disable
 *
 * Requires that the 'DPNI_OPT_IPR' option is set at DPNI creation.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_ipr(struct fsl_mc_io *mc_io, uint16_t token, int en);

/**
 * dpni_set_ipf() - Enable/disable IP fragmentation of egress frames
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPNI object
 * @en:		Set to '1' to enable; '0' to disable
 *
 * Requires that the 'DPNI_OPT_IPF' option is set at DPNI
 * creation. Fragmentation is performed according to MTU value
 * set by dpni_set_mtu() function
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpni_set_ipf(struct fsl_mc_io *mc_io, uint16_t token, int en);

/** @} */

#endif /* __FSL_DPNI_H */

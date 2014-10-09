/* Copyright 2014 Freescale Semiconductor Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!
 *  @file    fsl_dpni.h
 *  @brief   Data Path Network Interface API
 *
 */
#ifndef __FSL_DPNI_H
#define __FSL_DPNI_H

#include "fsl_dpkg.h"

/*!
 * @Group grp_dpni	Data Path Network Interface API
 *
 * @brief	Contains initialization APIs and runtime control APIs for DPNI
 *
 * @{
 */

struct fsl_mc_io;

/*!
 * @name General DPNI macros
 */
#define DPNI_MAX_TC				8
/*!< Maximum number of traffic classes */
#define DPNI_MAX_DPBP				8
/*!< Maximum number of buffer pools per DPNI */

#define DPNI_ALL_TCS				(uint8_t)(-1)
/*!< All traffic classes considered; see dpni_set_rx_flow() */
#define DPNI_ALL_TC_FLOWS			(uint16_t)(-1)
/*!< All flows within traffic class considered; see dpni_set_rx_flow() */
#define DPNI_NEW_FLOW_ID			(uint16_t)(-1)
/*!< Generate new flow ID; see dpni_set_tx_flow() */
/* @} */

/**
 * @brief	Open a control session for the specified object
 *
 *		This function can be used to open a control session for an
 *		already created object; an object may have been declared in
 *		the DPL or by calling the dpni_create() function.
 *		This function returns a unique authentication token,
 *		associated with the specific object ID and the specific MC
 *		portal; this token must be used in all subsequent commands for
 *		this specific object.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]	dpni_id		DPNI unique ID
 * @param[out]	token		Returned token; use in subsequent API calls
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_open(struct fsl_mc_io *mc_io, int dpni_id, uint16_t *token);

/**
 * @brief	Close the control session of the object
 *
 *		After this function is called, no further operations are
 *		allowed on the object without opening a new control session.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_close(struct fsl_mc_io *mc_io, uint16_t token);

/*!
 * @name DPNI configuration options
 */
#define DPNI_OPT_ALLOW_DIST_KEY_PER_TC		0x00000001
/*!< Allow different distribution key profiles for different traffic classes;
 * if not set, a single key profile is assumed
 */
#define DPNI_OPT_TX_CONF_DISABLED		0x00000002
/*!< Disable all non-error transmit confirmation; error frames are reported
 * back to a common Tx error queue
 */
#define DPNI_OPT_PRIVATE_TX_CONF_ERROR_DISABLED	0x00000004
/*!< Disable per-sender private Tx confirmation/error queue */
#define DPNI_OPT_DIST_HASH			0x00000010
/*!< Support distribution based on hashed key;
 * allows statistical distribution over receive queues in a traffic class
 */
#define DPNI_OPT_DIST_FS			0x00000020
/*!< Support distribution based on flow steering;
 * allows explicit control of distribution over receive queues in a traffic
 * class
 */
#define DPNI_OPT_UNICAST_FILTER			0x00000080
/*!< Unicast filtering support */
#define DPNI_OPT_MULTICAST_FILTER		0x00000100
/*!< Multicast filtering support */
#define DPNI_OPT_VLAN_FILTER			0x00000200
/*!< VLAN filtering support */
#define DPNI_OPT_IPR				0x00000800
/*!< Support IP reassembly on received packets */
#define DPNI_OPT_IPF				0x00001000
/*!< Support IP fragmentation on transmitted packets */
#define DPNI_OPT_VLAN_MANIPULATION		0x00010000
/*!< VLAN manipulation support */
#define DPNI_OPT_QOS_MASK_SUPPORT		0x00020000
/*!< Support masking of QoS lookup keys */
#define DPNI_OPT_FS_MASK_SUPPORT		0x00040000
/*!< Support masking of Flow Steering lookup keys */
/* @} */

/**
 * @brief	Structure representing IP reassembly configuration
 */
struct dpni_ipr_cfg {
	uint16_t max_reass_frm_size;
	/*!< Maximum size of the reassembled frame */
	uint16_t min_frag_size_ipv4;
	/*!< Minimum fragment size of IPv4 fragments */
	uint16_t min_frag_size_ipv6;
	/*!< Minimum fragment size of IPv6 fragments */
	uint16_t max_open_frames_ipv4;
	/*!< Maximum concurrent IPv4 packets in reassembly process */
	uint16_t max_open_frames_ipv6;
	/*!< Maximum concurrent IPv6 packets in reassembly process */
};

/**
 * @brief	Structure representing DPNI configuration
 */
struct dpni_cfg {
	uint8_t mac_addr[6]; /*!< Primary MAC address */
	struct {
		uint64_t options;
		/*!< Mask of available options; use 'DPNI_OPT_<X>' values */
		enum net_prot start_hdr;
		/*!< Selects the packet starting header for parsing;
		 * 'NET_PROT_NONE' is treated as default: 'NET_PROT_ETH'
		 */
		uint8_t max_senders;
		/*!< Maximum number of different senders; used as the number
		 * of dedicated Tx flows; Non-power-of-2 values are rounded up
		 * to the next power-of-2 value as hardware demands it;
		 * '0' will be treated as '1'
		 */
		uint8_t max_tcs;
		/*!< Maximum number of traffic classes (for both Tx and Rx);
		 * '0' will e treated as '1'
		 */
		uint8_t max_dist_per_tc[DPNI_MAX_TC];
		/*!< Maximum distribution size per Rx traffic class;
		 * Must be set to the required value minus 1;
		 * i.e. 0->1, 1->2, ... ,255->256;
		 * Non-power-of-2 values are rounded up to the next
		 * power-of-2 value as hardware demands it
		 */
		uint8_t max_unicast_filters;
		/*!< Maximum number of unicast filters; '0' is treated
		 * as '16'
		 */
		uint8_t max_multicast_filters;
		/*!< Maximum number of multicast filters; '0' is treated
		 * as '64'
		 */
		uint8_t max_vlan_filters;
		/*!< Maximum number of VLAN filters; '0' is treated
		 * as '16'
		 */
		uint8_t max_qos_entries;
		/*!< if 'max_tcs > 1', declares the maximum entries in the QoS
		 * table; '0' is treated as '64'
		 */
		uint8_t max_qos_key_size;
		/*!< Maximum key size for the QoS look-up; '0' is treated
		 * as '24' which is enough for IPv4 5-tuple
		 */
		uint8_t max_dist_key_size;
		/*!< Maximum key size for the distribution; '0' is treated
		 * as '24' which is enough for IPv4 5-tuple
		 */
		struct dpni_ipr_cfg ipr_cfg;
		/*!< IP reassembly configuration */
	} adv;
	/*!< Advanced parameters; default is all zeros;
	 * use this structure to change default settings
	 */
};

/**
 * @brief	Create the DPNI object, allocate required resources and
 *		perform required initialization.
 *
 *		The object can be created either by declaring it in the
 *		DPL file, or by calling this function.
 *
 *		This function returns a unique authentication token,
 *		associated with the specific object ID and the specific MC
 *		portal; this token must be used in all subsequent calls to
 *		this specific object. For objects that are created using the
 *		DPL file, call dpni_open() function to get an authentication
 *		token first.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]	cfg	Configuration structure
 * @param[out]	token	Returned token; use in subsequent API calls
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_create(struct fsl_mc_io	*mc_io,
		const struct dpni_cfg	*cfg,
		uint16_t		*token);

/**
 * @brief	Destroy the DPNI object and release all its resources.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 *
 * @returns	'0' on Success; error code otherwise.
 */
int dpni_destroy(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Structure representing buffer pools configuration
 */
struct dpni_pools_cfg {
	uint8_t num_dpbp;
	/*!< Number of DPBPs */
	struct {
		int dpbp_id;
		/*!< DPBP object id */
		uint16_t buffer_size;
		/*!< buffer size */
	} pools[DPNI_MAX_DPBP];
	/*!< Array of buffer pools parameters; The number of valid entries
	 * must match 'num_dpbp' value
	 */
};

/**
 * @brief	Set buffer pools configuration (mandatory for DPNI operation)
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	cfg	Buffer pools configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 */
int dpni_set_pools(struct fsl_mc_io		*mc_io,
		   uint16_t			token,
		   const struct dpni_pools_cfg	*cfg);

/**
 * @brief	Enable the DPNI, allow sending and receiving frames.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_enable(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Disable the DPNI, stop sending and receiving frames.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_disable(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Check if the DPNI is enabled.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]  en	Returns '1' if object is enabled; '0' otherwise
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_is_enabled(struct fsl_mc_io *mc_io, uint16_t token, int *en);

/**
 * @brief	Reset the DPNI, returns the object to initial state.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_reset(struct fsl_mc_io *mc_io, uint16_t token);

/*!
 * @name DPNI IRQ Index and Events
 */
#define DPNI_IRQ_INDEX				0
/*!< IRQ index */
#define DPNI_IRQ_EVENT_LINK_CHANGED		0x00000001
/*!< IRQ event - indicates a change in link state */
/* @} */

/**
 * @brief	Set IRQ information for the DPNI to trigger an interrupt.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	irq_index	Identifies the interrupt index to configure
 * @param[in]	irq_addr	Address that must be written to
 *				signal a message-based interrupt
 * @param[in]	irq_val		Value to write into irq_addr address
 * @param[in]	user_irq_id	A user defined number associated with this IRQ
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_irq(struct fsl_mc_io	*mc_io,
		 uint16_t		token,
		 uint8_t		irq_index,
		 uint64_t		irq_addr,
		 uint32_t		irq_val,
		 int			user_irq_id);

/**
 * @brief	Get IRQ information from the DPNI.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]  type		Interrupt type: 0 represents message interrupt
 *				type (both irq_addr and irq_val are valid)
 * @param[out]	irq_addr	Address that must be written to
 *				signal the message-based interrupt
 * @param[in]	irq_val		Value to write into irq_addr address
 * @param[out]	user_irq_id	A user defined number associated with this IRQ
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_irq(struct fsl_mc_io	*mc_io,
		 uint16_t		token,
		 uint8_t		irq_index,
		 int			*type,
		 uint64_t		*irq_addr,
		 uint32_t		*irq_val,
		 int			*user_irq_id);

/**
 * @brief	Set overall interrupt state.
 *
 * Allows GPP software to control when interrupts are generated.
 * Each interrupt can have up to 32 causes.  The enable/disable control's the
 * overall interrupt state. if the interrupt is disabled no causes will cause
 * an interrupt.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[in]	en		Interrupt state: - enable = 1, disable = 0
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_irq_enable(struct fsl_mc_io	*mc_io,
			uint16_t		token,
			uint8_t			irq_index,
			uint8_t			en);

/**
 * @brief	Get overall interrupt state
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	en		Interrupt state - enable = 1, disable = 0
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_irq_enable(struct fsl_mc_io	*mc_io,
			uint16_t		token,
			uint8_t			irq_index,
			uint8_t			*en);

/**
 * @brief	Set interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[in]	mask		event mask to trigger interrupt;
 *				each bit:
 *					0 = ignore event
 *					1 = consider event for asserting IRQ
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_irq_mask(struct fsl_mc_io	*mc_io,
		      uint16_t		token,
		      uint8_t		irq_index,
		      uint32_t		mask);

/**
 * @brief	Get interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	mask		event mask to trigger interrupt
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_irq_mask(struct fsl_mc_io	*mc_io,
		      uint16_t		token,
		      uint8_t		irq_index,
		      uint32_t		*mask);

/**
 * @brief	Get the current status of any pending interrupts.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	status		interrupts status - one bit per cause:
 *					0 = no interrupt pending
 *					1 = interrupt pending
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_irq_status(struct fsl_mc_io	*mc_io,
			uint16_t		token,
			uint8_t			irq_index,
			uint32_t		*status);

/**
 * @brief	Clear a pending interrupt's status
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	status		bits to clear (W1C) - one bit per cause:
 *					0 = don't change
 *					1 = clear status bit
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_clear_irq_status(struct fsl_mc_io	*mc_io,
			  uint16_t		token,
			  uint8_t		irq_index,
			  uint32_t		status);

/**
 * @brief	Structure representing DPNI attributes
 */
struct dpni_attr {
	int id;
	/*!< DPNI object ID */
	struct {
		uint16_t major;
		/*!< DPNI major version */
		uint16_t minor;
		/*!< DPNI minor version */
	} version;
	/*!< DPNI version */
	enum net_prot start_hdr;
	/*!< Indicates the packet starting header for parsing */
	uint64_t options;
	/*!< Mask of available options; reflects the value as was given in
	 * object's creation
	 */
	uint8_t max_senders;
	/*!< Maximum number of different senders; used as the number
	 * of dedicated Tx flows;
	 */
	uint8_t max_tcs;
	/*!< Maximum number of traffic classes (for both Tx and Rx) */
	uint8_t max_dist_per_tc[DPNI_MAX_TC];
	/*!< Maximum distribution size per Rx traffic class;
	 * Set to the required value minus 1
	 */
	uint8_t max_unicast_filters;
	/*!< Maximum number of unicast filters */
	uint8_t max_multicast_filters;
	/*!< Maximum number of multicast filters */
	uint8_t max_vlan_filters;
	/*!< Maximum number of VLAN filters */
	uint8_t max_qos_entries;
	/*!< if 'max_tcs > 1', declares the maximum entries in QoS table */
	uint8_t max_qos_key_size;
	/*!< Maximum key size for the QoS look-up */
	uint8_t max_dist_key_size;
	/*!< Maximum key size for the distribution look-up */
	struct dpni_ipr_cfg ipr_cfg;
	/*!< IP reassembly configuration */
};

/**
 * @brief	Retrieve DPNI attributes.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[out]	attr		Object's attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_attributes(struct fsl_mc_io	*mc_io,
			uint16_t		token,
			struct dpni_attr	*attr);

/*!
 * @name	DPNI errors
 */
#define DPNI_ERROR_EOFHE	0x00020000
/*!< Extract out of frame header error */
#define DPNI_ERROR_FLE		0x00002000
/*!< Frame length error */
#define DPNI_ERROR_FPE		0x00001000
/*!< Frame physical error */
#define DPNI_ERROR_PHE		0x00000020
/*!< Parsing header error */
#define DPNI_ERROR_L3CE		0x00000004
/*!< Parser L3 checksum error */
#define DPNI_ERROR_L4CE		0x00000001
/*!< Parser L3 checksum error */

/*!
 * @brief	Defines DPNI behavior for errors
 */
enum dpni_error_action {
	DPNI_ERROR_ACTION_DISCARD,
	/*!< Discard the frame */
	DPNI_ERROR_ACTION_CONTINUE,
	/*!< Continue with the normal flow */
	DPNI_ERROR_ACTION_SEND_TO_ERROR_QUEUE
	/*!< Send the frame to the error queue */
};

/**
 * @brief	Structure representing DPNI errors treatment
 */
struct dpni_error_cfg {
	uint32_t errors;
	/*!< Errors mask; use 'DPNI_ERROR__<X>' */
	enum dpni_error_action error_action;
	/*!< The desired action for the errors mask */
	int set_frame_annotation;
	/*!< Set to '1' to mark the errors in frame annotation status (FAS);
	 * relevant only for the non-discard action
	 */
};

/**
 * @brief	Set errors behavior
 *
 *		this function may be called numerous times with different
 *		error masks
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	cfg	Errors configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_errors_behavior(struct fsl_mc_io		*mc_io,
			     uint16_t			token,
			     struct dpni_error_cfg	*cfg);

/*!
 * @name DPNI buffer layout modification options
 */
#define DPNI_BUF_LAYOUT_OPT_TIMESTAMP		0x00000001
/*!< Select to modify the time-stamp setting */
#define DPNI_BUF_LAYOUT_OPT_PARSER_RESULT	0x00000002
/*!< Select to modify the parser-result setting; not applicable for Tx */
#define DPNI_BUF_LAYOUT_OPT_FRAME_STATUS	0x00000004
/*!< Select to modify the frame-status setting */
#define DPNI_BUF_LAYOUT_OPT_PRIVATE_DATA_SIZE	0x00000008
/*!< Select to modify the private-data-size setting */
#define DPNI_BUF_LAYOUT_OPT_DATA_ALIGN		0x00000010
/*!< Select to modify the data-alignment setting */
/* @} */

/**
 * @brief	Structure representing DPNI buffer layout
 */
struct dpni_buffer_layout {
	uint32_t options;
	/*!< Flags representing the suggested modifications to the buffer
	 * layout; Use any combination of 'DPNI_BUF_LAYOUT_OPT_<X>' flags
	 */
	int pass_timestamp;
	/*!< Pass timestamp value */
	int pass_parser_result;
	/*!< Pass parser results */
	int pass_frame_status;
	/*!< Pass frame status */
	uint16_t private_data_size;
	/*!< Size kept for private data (in bytes) */
	uint16_t data_align;
	/*!< Data alignment */
};

/**
 * @brief	Retrieve Rx buffer layout attributes.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]	layout	Returns buffer layout attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_rx_buffer_layout(struct fsl_mc_io		*mc_io,
			      uint16_t			token,
			      struct dpni_buffer_layout	*layout);

/**
 * @brief	Set Rx buffer layout configuration.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	layout	Buffer layout configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 */
int dpni_set_rx_buffer_layout(struct fsl_mc_io			*mc_io,
			      uint16_t				token,
			      const struct dpni_buffer_layout	*layout);

/**
 * @brief	Retrieve Tx buffer layout attributes.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]	layout	Returns buffer layout attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_buffer_layout(struct fsl_mc_io		*mc_io,
			      uint16_t			token,
			      struct dpni_buffer_layout	*layout);

/**
 * @brief	Set Tx buffer layout configuration.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	layout	Buffer layout configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 */
int dpni_set_tx_buffer_layout(struct fsl_mc_io			*mc_io,
			      uint16_t				token,
			      const struct dpni_buffer_layout	*layout);

/**
 * @brief	Retrieve Tx confirmation buffer layout attributes.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]	layout	Returns buffer layout attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_conf_buffer_layout(struct fsl_mc_io		*mc_io,
				   uint16_t			token,
				   struct dpni_buffer_layout	*layout);

/**
 * @brief	Set Tx confirmation buffer layout configuration.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	layout	Buffer layout configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 */
int dpni_set_tx_conf_buffer_layout(struct fsl_mc_io		   *mc_io,
				   uint16_t			   token,
				   const struct dpni_buffer_layout *layout);

/**
 * @brief	Enable/disable L3 checksum validation
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	en	Set to '1' to enable; '0' to disable
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_l3_chksum_validation(struct fsl_mc_io	*mc_io,
				  uint16_t		token,
				  int			en);

/**
 * @brief	Get L3 checksum validation mode
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]	en	Returns '1' if enabled; '0' otherwise
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_l3_chksum_validation(struct fsl_mc_io	*mc_io,
				  uint16_t		token,
				  int			*en);

/**
 * @brief	Enable/disable L4 checksum validation
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	en	Set to '1' to enable; '0' to disable
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_l4_chksum_validation(struct fsl_mc_io	*mc_io,
				  uint16_t		token,
				  int			en);

/**
 * @brief	Get L4 checksum validation mode
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]	en	Returns '1' if enabled; '0' otherwise
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_l4_chksum_validation(struct fsl_mc_io	*mc_io,
				  uint16_t		token,
				  int			*en);

/**
 * @brief	Get the Queuing Destination ID (QDID) that should be used for
 *		enqueue operations
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]	qdid	Virtual QDID value that should be used as an argument
 *			in all enqueue operations
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_qdid(struct fsl_mc_io *mc_io, uint16_t token, uint16_t *qdid);

/**
 * @brief	Get the AIOP storage profile ID associated with the DPNI
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]	spid	aiop storage-profile ID
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Only relevant for DPNI that belongs to AIOP container.
 */
int dpni_get_spid(struct fsl_mc_io *mc_io, uint16_t token, uint16_t *spid);

/**
 * @brief	Get the Tx data offset (from start of buffer)
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[out]	data_offset	Tx data offset (from start of buffer)
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_data_offset(struct fsl_mc_io	*mc_io,
			    uint16_t		token,
			    uint16_t		*data_offset);

/**
 * @brief	DPNI counter types
 */
enum dpni_counter {
	DPNI_CNT_ING_FRAME,
	/*!< Counts ingress frames */
	DPNI_CNT_ING_BYTE,
	/*!< Counts ingress bytes */
	DPNI_CNT_ING_FRAME_DROP,
	/*!< Counts ingress frames dropped due to explicit 'drop' setting */
	DPNI_CNT_ING_FRAME_DISCARD,
	/*!< Counts ingress frames discarded due to errors */
	DPNI_CNT_ING_MCAST_FRAME,
	/*!< Counts ingress multicast frames */
	DPNI_CNT_ING_MCAST_BYTE,
	/*!< Counts ingress multicast bytes */
	DPNI_CNT_ING_BCAST_FRAME,
	/*!< Counts ingress broadcast frames */
	DPNI_CNT_ING_BCAST_BYTES,
	/*!< Counts ingress broadcast bytes */
	DPNI_CNT_EGR_FRAME,
	/*!< Counts egress frames */
	DPNI_CNT_EGR_BYTE,
	/*!< Counts egress bytes */
	DPNI_CNT_EGR_FRAME_DISCARD
	/*!< Counts egress frames discarded due to errors */
};

/**
 * @brief	Read a specific DPNI counter
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	counter	The requested counter
 * @param[out]	value	Counter's current value
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_counter(struct fsl_mc_io	*mc_io,
		     uint16_t		token,
		     enum dpni_counter	counter,
		     uint64_t		*value);

/**
 * @brief	Set (or clear) a specific DPNI counter
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	counter	The requested counter
 * @param[in]   value	New counter value; typically pass '0' for resetting
 *			the counter.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_counter(struct fsl_mc_io	*mc_io,
		     uint16_t		token,
		     enum dpni_counter	counter,
		     uint64_t		value);

/**
 * @brief	Return the link state (either up or down)
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]	up	Link state; returns '1' if link is up, '0' otherwise
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_link_state(struct fsl_mc_io *mc_io, uint16_t token, int *up);

/**
 * @brief	Set the maximum received frame length.
 *
 * @param[in]	mc_io			Pointer to MC portal's I/O object
 * @param[in]   token			Token of DPNI object
 * @param[in]	max_frame_length	Maximum received frame length (in
 *					bytes); frame is discarded if its
 *					length exceeds this value
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_max_frame_length(struct fsl_mc_io	*mc_io,
			      uint16_t		token,
			      uint16_t		max_frame_length);

/**
 * @brief	Get the maximum received frame length.
 *
 * @param[in]	mc_io			Pointer to MC portal's I/O object
 * @param[in]   token			Token of DPNI object
 * @param[out]	max_frame_length	Maximum received frame length (in
 *					bytes); frame is discarded if its
 *					length exceeds this value
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_max_frame_length(struct fsl_mc_io	*mc_io,
			      uint16_t		token,
			      uint16_t		*max_frame_length);

/**
 * @brief	Set the MTU for the interface.
 *
 *		MTU determines the maximum fragment size for performing IP
 *		fragmentation on egress packets.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	mtu	MTU length (in bytes)
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_mtu(struct fsl_mc_io *mc_io, uint16_t token, uint16_t mtu);

/**
 * @brief	Get the MTU.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]	mtu	MTU length (in bytes)
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_mtu(struct fsl_mc_io *mc_io, uint16_t token, uint16_t *mtu);

/**
 * @brief	Enable/disable multicast promiscuous mode
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	en	Set to '1' to enable; '0' to disable
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_multicast_promisc(struct fsl_mc_io	*mc_io,
			       uint16_t		token,
			       int		en);

/**
 * @brief	Get multicast promiscuous mode
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]	en	Returns '1' if enabled; '0' otherwise
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_multicast_promisc(struct fsl_mc_io	*mc_io,
			       uint16_t		token,
			       int		*en);

/**
 * @brief	Enable/disable unicast promiscuous mode
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	en	Set to '1' to enable; '0' to disable
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_unicast_promisc(struct fsl_mc_io *mc_io, uint16_t token, int en);

/**
 * @brief	Get unicast promiscuous mode
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]	en	Returns '1' if enabled; '0' otherwise
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_unicast_promisc(struct fsl_mc_io *mc_io, uint16_t token, int *en);

/**
 * @brief	Set the primary MAC address
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	mac_addr	MAC address to set as primary address
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_primary_mac_addr(struct fsl_mc_io	*mc_io,
			      uint16_t		token,
			      const uint8_t	mac_addr[6]);

/**
 * @brief	Get the primary MAC address
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[out]	mac_addr	MAC address
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_primary_mac_addr(struct fsl_mc_io	*mc_io,
			      uint16_t		token,
			      uint8_t		mac_addr[6]);

/**
 * @brief	Add MAC address filter
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	mac_addr	MAC address to add
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_add_mac_addr(struct fsl_mc_io	*mc_io,
		      uint16_t		token,
		      const uint8_t	mac_addr[6]);

/**
 * @brief	Remove MAC address filter
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	mac_addr	MAC address to remove
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_remove_mac_addr(struct fsl_mc_io	*mc_io,
			 uint16_t		token,
			 const uint8_t		mac_addr[6]);

/**
 * @brief	Clear all unicast and/or multicast MAC filters
 *
 *		The primary MAC address is not cleared by this operation.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]   unicast		Set to '1' to clear unicast addresses
 * @param[in]   multicast	Set to '1' to clear multicast addresses
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_clear_mac_filters(struct fsl_mc_io	*mc_io,
			   uint16_t		token,
			   int			unicast,
			   int			multicast);

/**
 * @brief	Enable/disable VLAN filtering mode
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	en	Set to '1' to enable; '0' to disable
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_vlan_filters(struct fsl_mc_io *mc_io, uint16_t token, int en);

/**
 * @brief	Add VLAN ID filter
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	vlan_id		VLAN ID to add
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_add_vlan_id(struct fsl_mc_io	*mc_io,
		     uint16_t		token,
		     uint16_t		vlan_id);

/**
 * @brief	Remove VLAN ID filter
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	vlan_id		VLAN ID to remove
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_remove_vlan_id(struct fsl_mc_io	*mc_io,
			uint16_t		token,
			uint16_t		vlan_id);

/**
 * @brief	Clear all VLAN filters
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_clear_vlan_filters(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Structure representing Tx traffic class configuration
 */
struct dpni_tx_tc_cfg {
	uint16_t depth_limit;
	/*!< Limit the depth of a queue to the given value; note, that this
	 * may result in frames being rejected from the queue;
	 * set to '0' to remove any limitation
	 */
};

/**
 * @brief	Set Tx traffic class configuration
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	tc_id	Traffic class selection (1-8)
 * @param[in]	cfg	Traffic class configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_tx_tc(struct fsl_mc_io		*mc_io,
		   uint16_t			token,
		   uint8_t			tc_id,
		   const struct dpni_tx_tc_cfg	*cfg);

/**
 * @brief	DPNI distribution mode
 */
enum dpni_dist_mode {
	DPNI_DIST_MODE_NONE,
	/*!< No distribution */
	DPNI_DIST_MODE_HASH,
	/*!< Use hash distribution; only relevant if
	 * the 'DPNI_OPT_DIST_HASH' option was set at DPNI creation
	 */
	DPNI_DIST_MODE_FS
	/*!< Use explicit flow steering; only relevant if
	 * the 'DPNI_OPT_DIST_FS' option was set at DPNI creation
	 */
};

/**
 * @brief   DPNI Flow Steering miss action
 */
enum dpni_fs_miss_action {
	DPNI_FS_MISS_DROP,
	/*!< In case of no-match, drop the frame  */
	DPNI_FS_MISS_EXPLICIT_FLOWID,
	/*!< In case of no-match, use explicit flow-id */
	DPNI_FS_MISS_HASH
	/*!< In case of no-match, distribute using hash */
};

/**
 * @brief	Structure representing Flow Steering table configuration
 */
struct dpni_fs_tbl_cfg {
	enum dpni_fs_miss_action miss_action;
	/*!< Miss action selection */
	uint16_t default_flow_id;
	/*!< Used when 'miss_action = DPNI_FS_MISS_EXPLICIT_FLOWID' */
};

/**
 * @brief	Structure representing Rx traffic class distribution
 *		configuration
 */
struct dpni_rx_tc_dist_cfg {
	uint8_t dist_size;
	/*!< Set the distribution size; Must be set to the required value
	 * minus 1, for example: 0->1, 1->2, ... ,255->256;
	 * Non-power-of-2 values are rounded up to the next power-of-2 value
	 * as HW demands it
	 */
	enum dpni_dist_mode dist_mode;
	/*!< Distribution mode */
	struct dpkg_profile_cfg *dist_key_cfg;
	/*!< Select the extractions to be used for the distribution key */
	struct dpni_fs_tbl_cfg fs_cfg;
	/*!< Flow Steering table configuration; only relevant if
	 * 'dist_mode = DPNI_DIST_MODE_FS'
	 */
};

/**
 * @brief	Set Rx traffic class distribution configuration
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	tc_id		Traffic class selection (1-8)
 * @param[in]	cfg		Traffic class distribution configuration
 * @param[in]	params_iova	I/O virtual address of zeroed 256 bytes of
 *				DMA-able memory
 *
 * @returns	'0' on Success; error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 */
int dpni_set_rx_tc_dist(struct fsl_mc_io			*mc_io,
			uint16_t				token,
			uint8_t					tc_id,
			const struct dpni_rx_tc_dist_cfg	*cfg,
			uint64_t				params_iova);

/**
 * @brief   DPNI destination types
 */
enum dpni_dest {
	DPNI_DEST_NONE,
	/*!< Unassigned destination; The queue is set in parked mode and does
	 * not generate FQDAN notifications; user is expected to dequeue from
	 * the queue based on polling or other user-defined method
	 */
	DPNI_DEST_DPIO,
	/*!< The queue is set in schedule mode and generates FQDAN
	 * notifications to the specified DPIO; user is expected to dequeue
	 * from the queue only after notification is received
	 */
	DPNI_DEST_DPCON
	/*!< The queue is set in schedule mode and does not generate FQDAN
	 * notifications, but is connected to the specified DPCON object;
	 * user is expected to dequeue from the DPCON channel
	 */
};

/**
 * @brief	Structure representing DPNI destination parameters
 */
struct dpni_dest_cfg {
	enum dpni_dest dest_type;
	/*!< Destination type */
	int dest_id;
	/*!< Either DPIO ID or DPCON ID, depending on the destination type */
	uint8_t priority;
	/*!< Priority selection within the DPIO or DPCON channel; valid values
	 * are 0-1 or 0-7, depending on the number of priorities in that
	 * channel; not relevant for 'DPNI_DEST_NONE' option
	 */
};

/*!
 * @name DPNI queue modification options
 */
#define DPNI_QUEUE_OPT_USER_CTX		0x00000001
/*!< Select to modify the user's context associated with the queue */
#define DPNI_QUEUE_OPT_DEST		0x00000002
/*!< Select to modify the queue's destination */
/* @} */

/**
 * @brief	Structure representing queue configuration
 */
struct dpni_queue_cfg {
	uint32_t options;
	/*!< Flags representing the suggested modifications to the queue;
	 * Use any combination of 'DPNI_QUEUE_OPT_<X>' flags
	 */
	uint64_t user_ctx;
	/*!< User context value provided in the frame descriptor of each
	 * dequeued frame;
	 * valid only if 'DPNI_QUEUE_OPT_USER_CTX' is contained in 'options'
	 */
	struct dpni_dest_cfg dest_cfg;
	/*!< Queue destination parameters;
	 * valid only if 'DPNI_QUEUE_OPT_DEST' is contained in 'options'
	 */
};

/**
 * @brief	Structure representing queue attributes
 */
struct dpni_queue_attr {
	uint64_t user_ctx;
	/*!< User context value provided in the frame descriptor of each
	 * dequeued frame
	 */
	struct dpni_dest_cfg dest_cfg;
	/*!< Queue destination configuration */
	uint32_t fqid;
	/*!< Virtual fqid value to be used for dequeue operations */
};

/*!
 * @name DPNI Tx flow modification options
 */
#define DPNI_TX_FLOW_OPT_TX_CONF_ERROR	0x00000001
/*!< Select to modify the settings for dedicate Tx confirmation/error */
#define DPNI_TX_FLOW_OPT_ONLY_TX_ERROR	0x00000002
/*!< Select to modify the Tx confirmation and/or error setting */
#define DPNI_TX_FLOW_OPT_QUEUE		0x00000004
/*!< Select to modify the queue configuration */
#define DPNI_TX_FLOW_OPT_L3_CHKSUM_GEN	0x00000010
/*!< Select to modify the L3 checksum generation setting */
#define DPNI_TX_FLOW_OPT_L4_CHKSUM_GEN	0x00000020
/*!< Select to modify the L4 checksum generation setting */
/* @} */

/**
 * @brief	Structure representing Tx flow configuration
 */
struct dpni_tx_flow_cfg {
	uint32_t options;
	/*!< Flags representing the suggested modifications to the Tx flow;
	 * Use any combination 'DPNI_TX_FLOW_OPT_<X>' flags
	 */
	struct {
		int use_default_queue;
		/*!< Set to '1' to use the common (default) Tx confirmation
		 * and error queue; Set to '0' to use the private Tx
		 * confirmation and error queue; valid only if
		 * 'DPNI_TX_FLOW_OPT_TX_CONF_ERROR' is contained in 'options'
		 */
		int errors_only;
		/*!< Set to '1' to report back only error frames; Set to '0'
		 * to confirm transmission/error for all transmitted frames;
		 * valid only if 'DPNI_TX_FLOW_OPT_ONLY_TX_ERROR' is
		 * contained in 'options' and 'use_default_queue = 0';
		 */
		struct dpni_queue_cfg queue_cfg;
		/*!< Queue configuration; valid only if
		 * 'DPNI_TX_FLOW_OPT_QUEUE' is contained in 'options'
		 */
	} conf_err_cfg;
	/*!< Tx confirmation and error configuration; these settings are
	 * ignored if 'DPNI_OPT_PRIVATE_TX_CONF_ERROR_DISABLED' was set at
	 * DPNI creation
	 */
	int l3_chksum_gen;
	/*!< Set to '1' to enable L3 checksum generation; '0' to disable;
	 * valid only if 'DPNI_TX_FLOW_OPT_L3_CHKSUM_GEN' is contained in
	 * 'options'
	 */
	int l4_chksum_gen;
	/*!< Set to '1' to enable L4 checksum generation; '0' to disable;
	 * valid only if 'DPNI_TX_FLOW_OPT_L4_CHKSUM_GEN' is contained in
	 * 'options'
	 */
};

/**
 * @brief	Set Tx flow configuration
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in,out] flow_id	Provides (or returns) the sender's flow ID;
 *				for each new sender set (*flow_id) to
 *				'DPNI_NEW_FLOW_ID' to generate a new flow_id;
 *				this ID should be used as the QDBIN argument
 *				in enqueue operations
 * @param[in]	cfg		Tx flow configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_tx_flow(struct fsl_mc_io			*mc_io,
		     uint16_t				token,
		     uint16_t				*flow_id,
		     const struct dpni_tx_flow_cfg	*cfg);

/**
 * @brief	Structure representing Tx flow attributes
 */
struct dpni_tx_flow_attr {
	struct {
		int use_default_queue;
		/*!< '1' if using common (default) Tx confirmation and error
		 * queue; '0' if using private Tx confirmation and error queue
		 */
		int errors_only;
		/*!< '1' if only error frames are reported back; '0' if all
		 * transmitted frames are confirmed
		 */
		struct dpni_queue_attr queue_attr;
		/*!< Queue attributes */
	} conf_err_attr;
	/*!< Tx confirmation and error attributes */
	int l3_chksum_gen;
	/*!< '1' if L3 checksum generation is enabled; '0' if disabled */
	int l4_chksum_gen;
	/*!< '1' if L4 checksum generation is enabled; '0' if disabled */
};

/**
 * @brief	Get Tx flow attributes
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	flow_id		The sender's flow ID, as returned by the
 *				dpni_set_tx_flow() function
 * @param[out]	attr		Tx flow attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_flow(struct fsl_mc_io		*mc_io,
		     uint16_t			token,
		     uint16_t			flow_id,
		     struct dpni_tx_flow_attr	*attr);

/**
 * @brief	Set Rx flow configuration
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	tc_id		Traffic class selection (1-8);
 *				use 'DPNI_ALL_TCS' to set all TCs and all flows
 * @param[in]	flow_id	Rx flow id within the traffic class; use
 *				'DPNI_ALL_TC_FLOWS' to set all flows within
 *				this tc_id; ignored if tc_id is set to
 *				'DPNI_ALL_TCS';
 * @param[in]	cfg		Rx flow configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_rx_flow(struct fsl_mc_io			*mc_io,
		     uint16_t				token,
		     uint8_t				tc_id,
		     uint16_t				flow_id,
		     const struct dpni_queue_cfg	*cfg);

/**
 * @brief	Get Rx flow attributes
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	tc_id		Traffic class selection (1-8)
 * @param[in]	flow_id	Rx flow id within the traffic class
 * @param[out]	attr		Rx flow attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_rx_flow(struct fsl_mc_io		*mc_io,
		     uint16_t			token,
		     uint8_t			tc_id,
		     uint16_t			flow_id,
		     struct dpni_queue_attr	*attr);

/**
 * @brief	Set Rx error queue configuration
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	cfg		Queue configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_rx_err_queue(struct fsl_mc_io		*mc_io,
			  uint16_t			token,
			  const struct dpni_queue_cfg	*cfg);

/**
 * @brief	Get Rx error queue attributes
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[out]	attr		Queue attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_rx_err_queue(struct fsl_mc_io		*mc_io,
			  uint16_t			token,
			  struct dpni_queue_attr	*attr);

/**
 * @brief	Set Tx confirmation and error queue configuration
 *
 *		If 'DPNI_OPT_TX_CONF_DISABLED' was selected at DPNI creation,
 *		only error frames are reported back - successfully transmitted
 *		frames are not confirmed. Otherwise, all transmitted frames
 *		are sent for confirmation.
 *
 *		If private Tx confirmation and error queues are used with this
 *		DPNI, then this queue serves all Tx flows that are configured
 *		with 'use_default_queue' option (see dpni_tx_flow_cfg).
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	cfg		Queue configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_tx_conf_err_queue(struct fsl_mc_io			*mc_io,
			       uint16_t				token,
			       const struct dpni_queue_cfg	*cfg);

/**
 * @brief	Get Tx confirmation and error queue attributes
 *
 *		If 'DPNI_OPT_TX_CONF_DISABLED' was selected at DPNI creation,
 *		only error frames are reported back - successfully transmitted
 *		frames are not confirmed. Otherwise, all transmitted frames
 *		are sent for confirmation.
 *
 *		If private Tx confirmation and error queues are used with this
 *		DPNI, then this queue serves all Tx flows that are configured
 *		with 'use_default_queue' option (see dpni_tx_flow_cfg).
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[out]	attr		Queue attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_conf_err_queue(struct fsl_mc_io		*mc_io,
			       uint16_t			token,
			       struct dpni_queue_attr	*attr);

/**
 * @brief	Structure representing QOS table configuration
 */
struct dpni_qos_tbl_cfg {
	struct dpkg_profile_cfg *qos_key_cfg;
	/*!< Selects key extractions to be used as the QoS criteria */
	int discard_on_miss;
	/*!< Set to '1' to discard frames in case of no match (miss);
	 * '0' to use the 'default_tc' in such cases
	 */
	uint8_t default_tc;
	/*!< Used in case of no-match and 'discard_on_miss'= 0 */
};

/**
 * @brief	Set QoS mapping table
 *
 *		This function and all QoS-related functions require that
 *		'max_tcs > 1' was set at DPNI creation.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	cfg		QoS table configuration
 * @param[in]	params_iova	I/O virtual address of zeroed 256 bytes of
 *				DMA-able memory
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_qos_table(struct fsl_mc_io			*mc_io,
		       uint16_t				token,
		       const struct dpni_qos_tbl_cfg	*cfg,
		       uint64_t				params_iova);

/**
 * @brief	Structure representing rule configuration for table lookup
 */
struct dpni_rule_cfg {
	uint64_t key_iova;
	/*!< I/O virtual address of the key (must be in DMA-able memory) */
	uint64_t mask_iova;
	/*!< I/O virtual address of the mask (must be in DMA-able memory) */
	uint8_t key_size;
	/*!< key and mask size (in bytes) */
};

/**
 * @brief	Add QoS mapping entry (to select a traffic class)
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	cfg		QoS rule to add
 * @param[in]	tc_id		Traffic class selection (1-8)
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_add_qos_entry(struct fsl_mc_io			*mc_io,
		       uint16_t				token,
		       const struct dpni_rule_cfg	*cfg,
		       uint8_t				tc_id);

/**
 * @brief	Remove QoS mapping entry
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	cfg		QoS rule to remove
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_remove_qos_entry(struct fsl_mc_io		*mc_io,
			  uint16_t			token,
			  const struct dpni_rule_cfg	*cfg);

/**
 * @brief	Clear all QoS mapping entries
 *
 *		Following this function call, all frames are directed to
 *		the default traffic class (0)
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_clear_qos_table(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Add Flow Steering entry for a specific traffic class (to
 *		select a flow ID)
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	tc_id		Traffic class selection (1-8)
 * @param[in]	cfg		Flow steering rule to add
 * @param[in]	flow_id		Flow id selection (must be smaller than the
 *				distribution size of the traffic class)
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_add_fs_entry(struct fsl_mc_io			*mc_io,
		      uint16_t				token,
		      uint8_t				tc_id,
		      const struct dpni_rule_cfg	*cfg,
		      uint16_t				flow_id);

/**
 * @brief	Remove Flow Steering entry from a specific traffic class
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	tc_id		Traffic class selection (1-8)
 * @param[in]	cfg		Flow steering rule to remove
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_remove_fs_entry(struct fsl_mc_io		*mc_io,
			 uint16_t			token,
			 uint8_t			tc_id,
			 const struct dpni_rule_cfg	*cfg);

/**
 * @brief	Clear all Flow Steering entries of a specific traffic class
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	tc_id		Traffic class selection (1-8)
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_clear_fs_entries(struct fsl_mc_io	*mc_io,
			  uint16_t		token,
			  uint8_t		tc_id);

/**
 * @brief	Enable/disable VLAN insertion for egress frames
 *
 *		Requires that the 'DPNI_OPT_VLAN_MANIPULATION' option is set
 *		at DPNI creation.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	en	Set to '1' to enable; '0' to disable
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_vlan_insertion(struct fsl_mc_io *mc_io, uint16_t token, int en);

/**
 * @brief	Enable/disable VLAN removal for ingress frames
 *
 *		Requires that the 'DPNI_OPT_VLAN_MANIPULATION' option is set
 *		at DPNI creation.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	en	Set to '1' to enable; '0' to disable
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_vlan_removal(struct fsl_mc_io *mc_io, uint16_t token, int en);

/**
 * @brief	Enable/disable IP reassembly of ingress frames
 *
 *		Requires that the 'DPNI_OPT_IPR' option is set at DPNI
 *		creation.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	en	Set to '1' to enable; '0' to disable
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_ipr(struct fsl_mc_io *mc_io, uint16_t token, int en);

/**
 * @brief	Enable/disable IP fragmentation of egress frames
 *
 *		Requires that the 'DPNI_OPT_IPF' option is set at DPNI
 *		creation. Fragmentation is performed according to MTU value
 *		set by dpni_set_mtu() function
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	en	Set to '1' to enable; '0' to disable
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_ipf(struct fsl_mc_io *mc_io, uint16_t token, int en);

/** @} */

#endif /* __FSL_DPNI_H */

/* Copyright 2013-2014 Freescale Semiconductor Inc.
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
/*!
 *  @file    fsl_dpsw.h
 *  @brief   Data Path DPSW API
 */

#ifndef __FSL_DPSW_H
#define __FSL_DPSW_H

#include "fsl_net.h"

struct fsl_mc_io;

/*!
 * @Group grp_dpsw	Data Path DPSW API
 *
 * @brief	Contains API for handling DPSW topology and functionality
 * @{
 */

/*!
 * @name DPSW general definitions
 */
#define DPSW_MAX_PRI		8
/*!< Maximum number of traffic class priorities */
#define DPSW_MAX_IF		64
/*!< Maximum number of interfaces */
/* @} */

/**
 * @brief	Open a control session for the specified object
 *
 *		This function can be used to open a control session for an
 *		already created object; an object may have been declared in
 *		the DPL or by calling the dpsw_create() function.
 *		This function returns a unique authentication token,
 *		associated with the specific object ID and the specific MC
 *		portal; this token must be used in all subsequent commands for
 *		this specific object
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]	dpsw_id		DPSW unique ID
 * @param[out]	token		Returned token; use in subsequent API calls
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpsw_open(struct fsl_mc_io *mc_io, int dpsw_id, uint16_t *token);

/**
 * @brief	Close the control session of the object
 *
 *		After this function is called, no further operations are
 *		allowed on the object without opening a new control session.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpsw_close(struct fsl_mc_io *mc_io, uint16_t token);

/*!
 * @name DPSW options
 */
#define DPSW_OPT_FLOODING_DIS	0x0000000000000001ULL
/*!< Disable flooding */
#define DPSW_OPT_BROADCAST_DIS	0x0000000000000002ULL
/*!< Disable Broadcast */
#define DPSW_OPT_MULTICAST_DIS	0x0000000000000004ULL
/*!< Disable Multicast */
#define DPSW_OPT_TC_DIS		0x0000000000000008ULL
/*!< Disable Traffic classes */
#define DPSW_OPT_CONTROL	0x0000000000000010ULL
/*!< Support control interface */
/* @} */

/**
 * @brief	DPSW configuration
 *
 */
struct dpsw_cfg {
	uint16_t num_ifs;
	/*!< Number of external and internal interfaces */
	struct {
		uint64_t options;
		/*!< Enable/Disable DPSW features (bitmap) */
		uint16_t max_vlans;
		/*!< Maximum Number of VLAN's; 0 - indicates default 16 */
		uint8_t max_fdbs;
		/*!< Maximum Number of FDB's; 0 - indicates default 16 */
		uint16_t max_fdb_entries;
		/*!< Number of FDB entries for default FDB table;
		 * 0 - indicates default 1024 entries.
		 */
		uint16_t fdb_aging_time;
		/*!< Default FDB aging time for default FDB table;
		 * 0 - indicates default 300 seconds
		 */
		uint16_t max_fdb_mc_groups;
		/*!< Number of multicast groups in each FDB table;
		 * 0 - indicates default 32
		 */
	} adv;
	/*!< Advanced parameters; default is all zeros;
	 * use this structure to change default settings
	 */
};

/**
 * @brief	Create the DPSW object, allocate required resources and
 *		perform required initialization.
 *
 *		The object can be created either by declaring it in the
 *		DPL file, or by calling this function.
 *
 *		This function returns a unique authentication token,
 *		associated with the specific object ID and the specific MC
 *		portal; this token must be used in all subsequent calls to
 *		this specific object. For objects that are created using the
 *		DPL file, call dpsw_open() function to get an authentication
 *		token first
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]	cfg	Configuration structure
 * @param[out]	token	Returned token; use in subsequent API calls
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpsw_create(struct fsl_mc_io *mc_io, const struct dpsw_cfg *cfg,
		uint16_t *token);

/**
 * @brief	Destroy the DPSW object and release all its resources.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPSW object
 *
 * @returns	'0' on Success; error code otherwise.
 */
int dpsw_destroy(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Enable DPSW functionality
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 *
 * @param[in]   token		Token of DPSW object
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_enable(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Disable DPSW functionality
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 *
 * @param[in]   token		Token of DPSW object
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_disable(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Check if the DPSW is enabled
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPSW object
 * @param[out]  en	Returns '1' if object is enabled; '0' otherwise
 *
 * @returns	'0' on Success; Error code otherwise
 */
int dpsw_is_enabled(struct fsl_mc_io *mc_io, uint16_t token, int *en);

/**
 * @brief	Reset the DPSW, returns the object to initial state.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPSW object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpsw_reset(struct fsl_mc_io *mc_io, uint16_t token);

/*!
 * @name DPSW IRQ Index and Events
 */
#define DPSW_IRQ_INDEX_IF		0x0000
#define DPSW_IRQ_INDEX_L2SW		0x0001

#define DPSW_IRQ_EVENT_LINK_CHANGED	0x0001
/*!< IRQ event - Indicates that the link state changed */
/* @} */

/**
 * @brief	Set IRQ information for the DPSW to trigger an interrupt.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	irq_index	Identifies the interrupt index to configure
 * @param[in]	irq_addr	Address that must be written to
 *				signal a message-based interrupt
 * @param[in]	irq_val		Value to write into irq_addr address
 * @param[out]	user_irq_id	A user defined number associated with this IRQ
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpsw_set_irq(struct fsl_mc_io	*mc_io,
		 uint16_t		token,
		 uint8_t		irq_index,
		 uint64_t		irq_addr,
		 uint32_t		irq_val,
		 int			user_irq_id);

/**
 * @brief	Get IRQ information from the DPSW
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]  type		Interrupt type: 0 represents message interrupt
 *				type (both irq_addr and irq_val are valid)
 * @param[out]	irq_addr	Address that must be written to
 *				signal the message-based interrupt
 * @param[in]	irq_val		Value to write into irq_addr address
 * @param[in]	user_irq_id	A user defined number associated with this IRQ
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpsw_get_irq(struct fsl_mc_io	*mc_io,
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
 * an interrupt
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPCI object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[in]	en		Interrupt state - enable = 1, disable = 0
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpsw_set_irq_enable(struct fsl_mc_io *mc_io, uint16_t token,
			uint8_t irq_index,
			uint8_t en);

/**
 * @brief	Get overall interrupt state
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	en		Interrupt state - enable = 1, disable = 0
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpsw_get_irq_enable(struct fsl_mc_io	*mc_io,
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
 * @param[in]   token		Token of DPCI object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[in]	mask		event mask to trigger interrupt;
 *				each bit:
 *					0 = ignore event
 *					1 = consider event for asserting irq
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpsw_set_irq_mask(struct fsl_mc_io		*mc_io,
		      uint16_t			token,
		      uint8_t			irq_index,
		      uint32_t			mask);

/**
 * @brief	Get interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	mask		event mask to trigger interrupt
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpsw_get_irq_mask(struct fsl_mc_io		*mc_io,
		      uint16_t			token,
		      uint8_t			irq_index,
		      uint32_t			*mask);

/**
 * @brief	Get the current status of any pending interrupts
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	status		interrupts status - one bit per cause:
 *					0 = no interrupt pending
 *					1 = interrupt pending
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpsw_get_irq_status(struct fsl_mc_io	*mc_io,
			uint16_t		token,
			uint8_t			irq_index,
			uint32_t		*status);

/**
 * @brief	Clear a pending interrupt's status
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPCI object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	status		bits to clear (W1C) - one bit per cause:
 *					0 = don't change
 *					1 = clear status bit
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpsw_clear_irq_status(struct fsl_mc_io	*mc_io,
			  uint16_t		token,
			  uint8_t		irq_index,
			  uint32_t		status);
/**
 * @brief	Structure representing DPSW attributes
 *
 */
struct dpsw_attr {
	int id;
	/*!< DPSW object ID */
	struct {
		uint16_t major;
		/*!< DPSW major version */
		uint16_t minor;
		/*!< DPSW minor version */
	} version;
	/*!< DPSW version */
	uint64_t options;
	/*!< Enable/Disable DPSW features */
	uint16_t max_vlans;
	/*!< Maximum Number of VLANs */
	uint8_t max_fdbs;
	/*!< Maximum Number of FDBs */
	uint16_t mem_size;
	/*!< DPSW frame storage memory size */
	uint16_t num_ifs;
	/*!< Number of interfaces */
	uint16_t num_vlans;
	/*!< Current number of VLANs */
	uint8_t num_fdbs;
	/*!< Current number of FDBs */
};

/**
 * @brief	Retrieve DPSW attributes
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[out]  attr		DPSW attributes
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_get_attributes(struct fsl_mc_io	*mc_io,
			uint16_t		token,
			struct dpsw_attr	*attr);

/**
 * @brief	Policer configuration mode
 *
 */
enum dpsw_policer_mode {
	DPSW_POLICER_DIS = 0,
	/*!< Disable Policer */
	DPSW_POLICER_EN
	/*!< Enable Policer */
};

/**
 * @brief	Policer configuration
 *
 */
struct dpsw_policer_cfg {
	enum dpsw_policer_mode mode;
	/*!< Enables/Disables policer (Ingress) */
};

/**
 * @brief	Enable/disable policer for DPSW
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	cfg		Configuration parameters

 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_set_policer(struct fsl_mc_io			*mc_io,
		     uint16_t				token,
		     const struct dpsw_policer_cfg	*cfg);

/**
 * @brief	Structure representing buffer depletion configuration
 *		parameters. Assuming only one buffer pool
 *		exist per switch.
 */
struct dpsw_buffer_depletion_cfg {
	uint32_t entrance_threshold;
	/*!<
	 * Entrance threshold, the number of buffers in a pool falls below a
	 * programmable depletion entry threshold
	 */
	uint32_t exit_threshold;
	/*!<
	 * Exit threshold. When the buffer pool's occupancy rises above a
	 * programmable depletion exit threshold, the buffer pool exits
	 * depletion
	 */
	uint64_t wr_addr;
	/*!<
	 * Address in GPP to write buffer depletion State Change Notification
	 * Message
	 */
};

/**
 * @brief	Configure thresholds for buffer depletion state
 *		and establish buffer depletion State Change Notification Message
 *		(CSCNM) from Congestion Point (CP) to GPP trusted software
 *		This configuration is used to trigger PFC request or congestion
 *		notification if enabled. It is assumed one buffer pool defined
 *		per switch.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	cfg		Configuration parameters

 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_set_buffer_depletion(struct fsl_mc_io				*mc_io,
			      uint16_t					token,
			      const struct dpsw_buffer_depletion_cfg	*cfg);

/**
 * @brief	Set target interface for reflected interfaces.
 *		Only one reflection receive interface is allowed per switch
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Id

 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_set_reflection_if(struct fsl_mc_io	*mc_io,
			   uint16_t		token,
			   uint16_t		if_id);

/**
 * @brief	Action selection for special/control frames
 *
 */
enum dpsw_action {
	DPSW_ACTION_DROP = 0,
	/*!< Drop frame */
	DPSW_ACTION_REDIRECT_TO_CTRL
	/*!< Redirect frame to control interface */
};

/*!
 * @name Precision Time Protocol (PTP) options
 */
#define DPSW_PTP_OPT_UPDATE_FCV	0x1
/*!< Indicate the need for UDP checksum update after PTP time correction
 * field has been filled in
 */
/* @} */

/**
 * @brief	Precision Time Protocol (PTP) configuration
 *
 */
struct dpsw_ptp_v2_cfg {
	int enable;
	/*!< Enable updating Correction time field in IEEE1588 V2 messages */
	uint16_t time_offset;
	/*!< Time correction field offset inside PTP from L2 start of the
	 * frame; PTP messages can be transported over different underlying
	 * protocols IEEE802.3, IPv4/UDP, Ipv6/UDP and others
	 */
	uint32_t options;
	/*!< Bitmap of additional options along with time correction;
	 * Select from 'DPSW_PTP_OPT_<X>'
	 */
};

/**
 * @brief	Function is used to define IEEE1588 V2 Precision Time Protocol
 *		(PTP) parameters for time correction
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	cfg		IEEE1588 V2 Configuration parameters

 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_set_ptp_v2(struct fsl_mc_io			*mc_io,
		    uint16_t				token,
		    const struct dpsw_ptp_v2_cfg	*cfg);

/**
 * @brief	Enable Disable flooding for particular interface
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[in]	en		1 - enable, 0 - disable

 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_flooding(struct fsl_mc_io	*mc_io,
			 uint16_t		token,
			 uint16_t		if_id,
			 int			en);

/**
 * @brief	Enable/disable broadcast for particular interface
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[in]	en		1 - enable, 0 - disable

 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_broadcast(struct fsl_mc_io	*mc_io,
			  uint16_t		token,
			  uint16_t		if_id,
			  int			en);

/**
 * @brief	Enable/disable multicast for particular interface
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[in]	en		1 - enable, 0 - disable

 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_multicast(struct fsl_mc_io	*mc_io,
			  uint16_t		token,
			  uint16_t		if_id,
			  int			en);

/**
 * @brief	Tag Contorl Information (TCI) configuration
 *
 */
struct dpsw_tci_cfg {
	uint8_t pcp;
	/*!< Priority Code Point (PCP): a 3-bit field which refers
	 * to the IEEE 802.1p priority
	 */
	uint8_t dei;
	/*!< Drop Eligible Indicator (DEI): a 1-bit field. May be used
	 * separately or in conjunction with PCP to indicate frames
	 * eligible to be dropped in the presence of congestion
	 */
	uint16_t vlan_id;
	/*!< VLAN Identifier (VID): a 12-bit field specifying the VLAN
	 * to which the frame belongs. The hexadecimal values
	 * of 0x000 and 0xFFF are reserved;
	 * all other values may be used as VLAN identifiers, allowing up
	 * to 4,094 VLANs
	 */
};

/**
 * @brief	Set default VLAN Tag Control Information (TCI)
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[in]	cfg		Tag Control Information Configuration

 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_tci(struct fsl_mc_io		*mc_io,
		    uint16_t			token,
		    uint16_t			if_id,
		    const struct dpsw_tci_cfg	*cfg);

/**
 * @brief	Get default VLAN Tag Control Information (TCI)
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[out]	cfg		Tag Control Information Configuration

 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_get_tci(struct fsl_mc_io		*mc_io,
		    uint16_t			token,
		    uint16_t			if_id,
		    struct dpsw_tci_cfg		*cfg);

/**
 * @brief	Spanning Tree Protocol (STP) states
 *
 */
enum dpsw_stp_state {
	DPSW_STP_STATE_BLOCKING = 0,
	/*!< Blocking state */
	DPSW_STP_STATE_LISTENING,
	/*!< Listening state */
	DPSW_STP_STATE_LEARNING,
	/*!< Learning state */
	DPSW_STP_STATE_FORWARDING
	/*!< Forwarding state */
};

/**
 * @brief	Spanning Tree Protocol (STP) Configuration
 *
 */
struct dpsw_stp_cfg {
	uint16_t vlan_id;
	/*!< VLAN ID STP state */
	enum dpsw_stp_state state;
	/*!< STP state */
};

/**
 * @brief	Function sets Spanning Tree Protocol (STP) state.
 *		The following STP states are supported -
 *		 blocking, listening, learning, forwarding and disabled.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[in]	cfg		STP State configuration parameters

 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_stp(struct fsl_mc_io		*mc_io,
		    uint16_t			token,
		    uint16_t			if_id,
		    const struct dpsw_stp_cfg	*cfg);

/**
 * @brief	Types of frames to accept
 *
 */
enum dpsw_accepted_frames {
	DPSW_ADMIT_ALL = 1,
	/*!< The device accepts VLAN tagged, untagged and
	 * priority tagged frames
	 **/
	DPSW_ADMIT_ONLY_VLAN_TAGGED = 3,
	/*!< The device discards untagged frames or
	 * Priority-Tagged frames received on this interface.
	 */
};

/**
 * @brief	Types of frames to accept configuration
 *
 */
struct dpsw_accepted_frames_cfg {
	enum dpsw_accepted_frames type;
	/*!< Defines ingress accepted frames */
	enum dpsw_action unaccept_act;
	/*!< When a frame is not accepted, it may be discarded or redirected
	 * to control interface depending on this mode
	 */
};

/**
 * @brief	When is admit_only_vlan_tagged- the device will discard untagged
 *		frames or Priority-Tagged frames received on this interface.
 *		When admit_only_untagged- untagged frames or Priority-Tagged
 *		frames received on this interface will be accepted and assigned
 *		to a VID based on the PVID and VID Set for this interface.
 *		When admit_all - the device will accept VLAN tagged, untagged
 *		and priority tagged frames.
 *		The default is admit_all
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[in]	cfg		Frame types configuration

 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_accepted_frames(struct fsl_mc_io			*mc_io,
				uint16_t				token,
				uint16_t				if_id,
				const struct dpsw_accepted_frames_cfg	*cfg);

/**
 * @brief	When this is accept (FALSE), the device will discard incoming
 *		frames for VLANs that do not include this interface in its
 *		Member set. When accept (TRUE), the interface will accept all
 *		incoming frames
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[in]	accept_all	Accept or drop frames having different VLAN

 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_accept_all_vlan(struct fsl_mc_io	*mc_io,
				uint16_t		token,
				uint16_t		if_id,
				int			accept_all);

/**
 * @brief	Counters types
 *
 */
enum dpsw_counter {
	DPSW_CNT_ING_FRAME,
	/*!< Counts ingress frames */
	DPSW_CNT_ING_BYTE,
	/*!< Counts ingress bytes */
	DPSW_CNT_ING_FLTR_FRAME,
	/*!< Counts filtered ingress frames */
	DPSW_CNT_ING_FRAME_DISCARD,
	/*!< Counts discarded ingress frame */
	DPSW_CNT_ING_MCAST_FRAME,
	/*!< Counts ingress multicast frames */
	DPSW_CNT_ING_MCAST_BYTE,
	/*!< Counts ingress multicast bytes */
	DPSW_CNT_ING_BCAST_FRAME,
	/*!< Counts ingress broadcast frames */
	DPSW_CNT_ING_BCAST_BYTES,
	/*!< Counts ingress broadcast bytes */
	DPSW_CNT_EGR_FRAME,
	/*!< Counts egress frames */
	DPSW_CNT_EGR_BYTE,
	/*!< Counts eEgress bytes */
	DPSW_CNT_EGR_FRAME_DISCARD
	/*!< Counts discarded egress frames */
};

/**
 * @brief	Get specific counter of particular interface
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[in]	type		Counter type
 *
 * @param[out]	counter	return value
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_get_counter(struct fsl_mc_io	*mc_io,
			uint16_t		token,
			uint16_t		if_id,
			enum dpsw_counter	type,
			uint64_t		*counter);

/**
 * @brief	Set specific counter of particular interface
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[in]	type		Counter type
 * @param[in]	counter		New counter value
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_counter(struct fsl_mc_io	*mc_io,
			uint16_t		token,
			uint16_t		if_id,
			enum dpsw_counter	type,
			uint64_t		counter);

/**
 * @brief	User priority
 *
 */
enum dpsw_user_priority {
	DPSW_UP_PCP = 0,
	/*!< Priority Code Point (PCP): a 3-bit field which
	 * refers to the IEEE 802.1p priority.
	 */
	DPSW_UP_PCP_DEI,
	/*!< Priority Code Point (PCP) combined with
	 * Drop Eligible Indicator (DEI)
	 */
	DPSW_UP_DSCP
	/*!< Differentiated services Code Point (DSCP): 6 bit
	 * field from IP header
	 */
};

/**
 * @brief	Mapping user priority into traffic class configuration
 *
 */
struct dpsw_tc_map_cfg {
	enum dpsw_user_priority user_priority;
	/*!< Source for user priority regeneration */
	uint8_t regenerated_priority[DPSW_MAX_PRI];
	/*!< The Regenerated User priority that the incoming
	 * User Priority is mapped to for this interface
	 */
};

/**
 * @brief	Function is used for mapping variety of frame fields (DSCP, PCP)
 *		to Traffic Class. Traffic class is a number
 *		in the range from 0 to 7
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 *
 * @param[in]	cfg		Traffic class mapping configuration
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_tc_set_map(struct fsl_mc_io			*mc_io,
		       uint16_t			token,
		       uint16_t			if_id,
		       const struct dpsw_tc_map_cfg	*cfg);

/**
 * @brief	Filter type for frames to reflect
 *
 */
enum dpsw_reflection_filter {
	DPSW_REFLECTION_FILTER_INGRESS_ALL = 0,
	/*!< Reflect all frames */
	DPSW_REFLECTION_FILTER_INGRESS_VLAN
	/*!< Reflect only frames belong to particular
	 * VLAN defined by vid parameter
	 */
};

/**
 * @brief	Structure representing reflection information
 *
 */
struct dpsw_reflection_cfg {
	enum dpsw_reflection_filter filter;
	/*!< Filter type for frames to reflect */
	uint16_t vlan_id;
	/*!< Vlan Id to reflect;
	 * valid only when filter type is DPSW_INGRESS_VLAN
	 */
};

/**
 * @brief	Identify interface to be reflected or mirrored
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[in]	cfg		Reflection configuration
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */

int dpsw_if_add_reflection(struct fsl_mc_io			*mc_io,
			   uint16_t				token,
			   uint16_t				if_id,
			   const struct dpsw_reflection_cfg	*cfg);

/**
 * @brief	Remove interface to be reflected or mirrored
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[in]	cfg		Reflection configuration
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_remove_reflection(struct fsl_mc_io			*mc_io,
			      uint16_t				token,
			      uint16_t				if_id,
			      const struct dpsw_reflection_cfg	*cfg);

/**
 * @brief	Metering and marking algorithms
 *
 */
enum dpsw_metering_algo {
	DPSW_METERING_ALGO_RFC2698 = 0,
	/*!< RFC 2698 */
	DPSW_METERING_ALGO_RFC4115
	/*!< RFC 4115 */
};

/**
 * @brief	Metering and marking modes
 *
 */
enum dpsw_metering_mode {
	DPSW_METERING_MODE_COLOR_BLIND = 0,
	/*!< Color blind mode */
	DPSW_METERING_MODE_COLOR_AWARE
	/*!< Color aware mode */
};

/**
 * @brief	Metering and marking configuration
 *
 */
struct dpsw_metering_cfg {
	enum dpsw_metering_algo algo;
	/*!< Implementation based on Metering and marking algorithm */
	uint32_t cir;
	/*!< Committed information rate (CIR) in bits/s */
	uint32_t eir;
	/*!< Excess information rate (EIR) in bits/s */
	uint32_t cbs;
	/*!< Committed burst size (CBS) in bytes */
	uint32_t ebs;
	/*!< Excess bust size (EBS) in bytes */
	enum dpsw_metering_mode mode;
	/*!< Color awareness mode */
};

/**
 * @brief	Function sets metering and marking algorithm (coloring)
 *		and provides corresponding parameters
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[in]	tc_id		Traffic class selection (1-8)
 * @param[in]	cfg		Metering and marking parameters
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_tc_set_metering_marking(struct fsl_mc_io			*mc_io,
				    uint16_t				token,
				    uint16_t				if_id,
				    uint8_t				tc_id,
				    const struct dpsw_metering_cfg	*cfg);

/**
 * @brief	Structure representing tag Protocol identifier
 *
 */
struct dpsw_custom_tpid_cfg {
	uint16_t tpid;
	/*!< An additional tag protocol identifier */
};

/**
 * @brief	API Configures a distinct Ethernet type value (or TPID value)
 *		to indicate a VLAN tag in addition to the common
 *		TPID values 0x8100 and 0x88A8.
 *		Two additional TPID's are supported
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	cfg		Tag Protocol identifier
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_add_custom_tpid(struct fsl_mc_io			*mc_io,
			 uint16_t				token,
			 const struct dpsw_custom_tpid_cfg	*cfg);

/**
 * @brief	Structure representing transmit rate configuration
 *
 */
struct dpsw_transmit_rate_cfg {
	uint64_t rate;
	/*!< Interface Transmit rate in bits per second */
};

/**
 * @brief	API sets interface transmit rate.
 *
 *		The setting mechanism is the same for internal and
 *		external (physical) interfaces.
 *		The rate set explicitly in bits per second.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[in]	cfg		Transmit rate configuration
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_transmit_rate(struct fsl_mc_io				*mc_io,
			      uint16_t					token,
			      uint16_t					if_id,
			      const struct dpsw_transmit_rate_cfg	*cfg);

/**
 * @brief	Transmission selection algorithm
 *
 */
enum dpsw_bw_algo {
	DPSW_BW_ALGO_STRICT_PRIORITY = 0,
	/*!< Strict priority algorithm */
	DPSW_BW_ALGO_CREDIT_BASED
	/*!< Credit based shaper algorithm */
};

/**
 * @brief	Structure representing class bandwidth configuration
 *
 */
struct dpsw_bandwidth_cfg {
	enum dpsw_bw_algo algo;
	/*!< Transmission selection algorithm */
	uint8_t delta_bandwidth;
	/*!< A percentage of the interface transmit rate; applied only when
	 * using credit-based shaper algorithm otherwise best effort algorithm
	 * is applied
	 */
};

/**
 * @brief	API sets a percentage of the interface transmit rate;
 *		 this is the bandwidth that can be reserved for use by the queue
 *		 associated with traffic class. A percentage is relevant
 *		 only when credit based shaping algorithm is selected for
 *		 traffic class otherwise best effort (strict priority)
 *		 algorithm is in place
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[in]	tc_id		Traffic class selection (1-8)
 * @param[in]	cfg		Traffic class bandwidth configuration
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_tc_set_bandwidth(struct fsl_mc_io			*mc_io,
			     uint16_t				token,
			     uint16_t				if_id,
			     uint8_t				tc_id,
			     const struct dpsw_bandwidth_cfg	*cfg);

/**
 * @brief	Enable Interface
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_enable(struct fsl_mc_io *mc_io, uint16_t token, uint16_t if_id);

/**
 * @brief	Disable Interface
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_disable(struct fsl_mc_io *mc_io, uint16_t token, uint16_t if_id);

/**
 * @brief	Structure representing congestion queue configuration
 *
 */
struct dpsw_queue_congestion_cfg {
	uint32_t entrance_threshold;
	/*!< Entrance threshold */
	uint32_t exit_threshold;
	/*!< Exit threshold */
	uint64_t wr_addr;
	/*!< Address to write Congestion State Change Notification Message */
};

/*!
 * @name PFC source trigger
 */
#define DPSW_PFC_TRIG_QUEUE_CNG		0x01
/*!< Trigger for PFC initiation is traffic class queue congestion */
#define DPSW_PFC_TRIG_BUFFER_DPL	0x02
/*!< Trigger for PFC initiation is buffer depletion */
/* @} */

/**
 * @brief	API is used to configure thresholds for traffic class queue
 *		 congestion state and to establish Congestion State Change
 *		 Notification Message (CSCNM) from Congestion Point (CP) to GPP
 *		 trusted software This configuration is used to trigger PFC
 *		 request or congestion notification if enabled
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[in]	tc_id		Traffic class Identifier
 * @param[in]	cfg		Queue congestion configuration
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_tc_set_queue_congestion(struct fsl_mc_io *mc_io,
				    uint16_t token,
				   uint16_t if_id,
				   uint8_t tc_id,
				   const struct dpsw_queue_congestion_cfg *cfg);

/**
 * @brief	Structure representing Priority Flow Control (PFC)
 *		configuration
 *
 */
struct dpsw_pfc_cfg {
	int receiver;
	/*!< Enable/Disable PFC receiver;
	 * PFC receiver is responsible for accepting
	 * PFC PAUSE messages and pausing transmission
	 * for indicated in message PFC quanta
	 */
	int initiator;
	/*!< Enable/Disable PFC initiator;
	 * PFC initiator is responsible for sending
	 * PFC request message when congestion has been
	 * detected on specified TC queue
	 */
	uint32_t initiator_trig;
	/*!< Bitmap defining Trigger source or sources
	 * for sending PFC request message out;
	 * DPSW_PFC_TRIG_QUEUE_CNG or DPSW_PFC_TRIG_BUFFER_DPL should be used
	 */
	uint16_t pause_quanta;
	/*!< Pause Quanta to indicate in PFC request
	 * message the amount of quanta time to pause
	 */
};

/**
 * @brief	Handles Priority Flow Control (PFC) configuration per
 *		Traffic Class (TC)
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[in]	tc_id		Traffic class Identifier
 * @param[in]	cfg		PFC configuration
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_tc_set_pfc(struct fsl_mc_io	*mc_io,
		       uint16_t		token,
		       uint16_t		if_id,
		       uint8_t			tc_id,
		       struct dpsw_pfc_cfg	*cfg);

/**
 * @brief	Structure representing Congestion Notification (CN)
 *		configuration
 */
struct dpsw_cn_cfg {
	int enable;
	/*!< Enable/disable Congestion State Change Notification
	 * Message (CSCNM) from Congestion Point (CP) to GPP
	 * trusted software
	 */
};

/**
 * @brief	API is used to enable/disable Congestion State Change
 *		Notification Message (CSCNM) from Congestion Point (CP)
 *		to GPP trusted software
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[in]	tc_id		Traffic class Identifier
 * @param[in]	cfg		Congestion notification description
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_tc_set_cn(struct fsl_mc_io		*mc_io,
		      uint16_t			token,
		      uint16_t			if_id,
		      uint8_t			tc_id,
		      const struct dpsw_cn_cfg	*cfg);

/**
 * @brief	Structure representing DPSW interface attributes
 */
struct dpsw_if_attr {
	uint8_t num_tcs;
	/*!< Number of traffic classes */
	uint64_t rate;
	/*!< Transmit rate in bits per second */
	uint64_t options;
	/*!< Interface configuration options (bitmap) */
	int enabled;
	/*! Indicates if interface is enabled */
	int accept_all_vlan;
	/*! The device discards/accepts incoming frames
	 * for VLANs that do not include this interface
	 */
	enum dpsw_accepted_frames admit_untagged;
	/*! When set to 'DPSW_ADMIT_ONLY_VLAN_TAGGED', the device discards
	 * untagged frames or priority-tagged frames received on this
	 * interface;
	 * When set to 'DPSW_ADMIT_ALL', untagged frames or priority-
	 * tagged frames received on this interface are accepted
	 */
};

/**
 * @brief	Function obtains attributes of interface
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[out]	attr		Interface attributes
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_get_attributes(struct fsl_mc_io	*mc_io,
			   uint16_t		token,
			   uint16_t		if_id,
			   struct dpsw_if_attr	*attr);

/**
 * @brief	Cipher Suite for MACSec
 */
enum dpsw_cipher_suite {
	DPSW_MACSEC_GCM_AES_128 = 0,
	/*!< 128 bit */
	DPSW_MACSEC_GCM_AES_256
	/*!< 256 bit */
};

/**
 * @brief	MACSec Configuration
 *
 */
struct dpsw_macsec_cfg {
	int enable;
	/*!< Enable MACSec */
	uint64_t sci;
	/*!< Secure Channel ID */
	enum dpsw_cipher_suite cipher_suite;
	/*!< Cipher Suite */
};

/**
 * @brief	Set MACSec configuration for physical port.
 *		Only point to point MACSec is supported
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[in]	cfg		MACSec configuration
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_macsec(struct fsl_mc_io			*mc_io,
		       uint16_t				token,
		       uint16_t			if_id,
		       const struct dpsw_macsec_cfg	*cfg);

/**
 * @brief	Set Maximum Receive frame length.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[in]	frame_length	Maximum Frame Length
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_max_frame_length(struct fsl_mc_io	*mc_io,
				 uint16_t		token,
				 uint16_t		if_id,
				 uint16_t		frame_length);

/**
 * @brief	Get Maximum Receive frame length.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[out]	frame_length	Maximum Frame Length
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_get_max_frame_length(struct fsl_mc_io	*mc_io,
				 uint16_t		token,
				 uint16_t		if_id,
				 uint16_t		*frame_length);

/**
 * @brief	Get Current Link state.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	if_id		Interface Identifier
 * @param[out]	state		Current link state
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_get_link_state(struct fsl_mc_io	*mc_io,
			   uint16_t		token,
			   uint16_t		if_id,
			   int			*state);

/**
 * @brief	VLAN Configuration
 *
 */
struct dpsw_vlan_cfg {
	uint16_t fdb_id;
	/*!< Forwarding Data base */
};

/**
 * @brief	Adding new VLAN to DPSW.
 *
 *		Only VLAN ID and FDB ID are required parameters here.
 *		12 bit VLAN ID is defined in IEEE802.1Q.
 *		Adding a duplicate VLAN ID is not allowed.
 *		FDB ID can be shared across multiple VLANs. Shared learning
 *		is obtained by calling dpsw_vlan_add for multiple VLAN IDs
 *		with same fdb_id
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	vlan_id		VLAN Identifier
 * @param[in]	cfg		VLAN configuration
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_add(struct fsl_mc_io		*mc_io,
		  uint16_t			token,
		  uint16_t			vlan_id,
		  const struct dpsw_vlan_cfg	*cfg);

/**
 * @brief	Set of VLAN Interfaces
 *
 */
struct dpsw_vlan_if_cfg {
	uint16_t num_ifs;
	/*!< The number of interfaces that are
	 * assigned to the egress list for this VLAN
	 */
	uint16_t if_id[DPSW_MAX_IF];
	/*!< The set of interfaces that are
	 * assigned to the egress list for this VLAN
	 */
};

/**
 * @brief	Adding a set of interfaces to an existing VLAN.
 *
 *		It adds only interfaces not belonging to this VLAN yet,
 *		otherwise an error is generated and an entire command is
 *		ignored. This function can be called numerous times always
 *		providing required interfaces delta.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	vlan_id		VLAN Identifier
 * @param[in]	cfg		Set of interfaces to add
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_add_if(struct fsl_mc_io			*mc_io,
		     uint16_t				token,
		     uint16_t				vlan_id,
		     const struct dpsw_vlan_if_cfg	*cfg);

/**
 * @brief	Defining a set of interfaces that should be transmitted as
 *		untagged.
 *
 *		These interfaces should already belong to this VLAN.
 *		By default all interfaces are transmitted as tagged.
 *		Providing un-existing interface or untagged interface that is
 *		configured untagged already generates an error and the entire
 *		command is ignored.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	vlan_id		VLAN Identifier
 * @param[in]	cfg		set of interfaces that should be
 *				transmitted as untagged
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_add_if_untagged(struct fsl_mc_io			*mc_io,
			      uint16_t				token,
			      uint16_t				vlan_id,
			      const struct dpsw_vlan_if_cfg	*cfg);

/**
 * @brief	Define a set of interfaces that should be included in flooding
 *		when frame with unknown destination unicast MAC arrived.
 *
 *		These interfaces should belong to this VLAN. By default all
 *		interfaces are included into flooding list. Providing
 *		un-existing interface or an interface that already in the
 *		flooding list generates an error and the entire command is
 *		ignored.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	vlan_id		VLAN Identifier
 * @param[in]	cfg		Set of interfaces that should be
 *				used for flooding
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_add_if_flooding(struct fsl_mc_io			*mc_io,
			      uint16_t				token,
			      uint16_t				vlan_id,
			      const struct dpsw_vlan_if_cfg	*cfg);

/**
 * @brief	Remove interfaces from an existing VLAN.
 *
 *		Interfaces must belong to this VLAN, otherwise an error
 *		is returned and an the command is ignored
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	vlan_id		VLAN Identifier
 * @param[in]	cfg		Set of interfaces that should be removed
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_remove_if(struct fsl_mc_io		*mc_io,
			uint16_t			token,
			uint16_t			vlan_id,
			const struct dpsw_vlan_if_cfg	*cfg);

/**
 * @brief	Define a set of interfaces that should be converted from
 *		transmitted as untagged to transmit as tagged.
 *
 *		Interfaces provided by API have to belong to this VLAN and
 *		configured untagged, otherwise an error is returned and the
 *		command is ignored
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	vlan_id		VLAN Identifier
 * @param[in]	cfg		set of interfaces that should be removed
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_remove_if_untagged(struct fsl_mc_io		*mc_io,
				 uint16_t			token,
				 uint16_t			vlan_id,
				 const struct dpsw_vlan_if_cfg	*cfg);

/**
 * @brief	Define a set of interfaces that should be removed from the
 *		flooding list.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	vlan_id		VLAN Identifier
 * @param[in]	cfg		set of interfaces used for flooding
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_remove_if_flooding(struct fsl_mc_io		*mc_io,
				 uint16_t			token,
				 uint16_t			vlan_id,
				 const struct dpsw_vlan_if_cfg	*cfg);

/**
 * @brief	Remove an entire VLAN
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	vlan_id		VLAN Identifier
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_remove(struct fsl_mc_io *mc_io, uint16_t token, uint16_t vlan_id);

/**
 * @brief	VLAN attributes
 *
 */
struct dpsw_vlan_attr {
	uint16_t fdb_id;
	/*!< Associated FDB ID */
	uint16_t num_ifs;
	/*!< Number of interfaces */
	uint16_t num_untagged_ifs;
	/*!< Number of untagged interfaces */
	uint16_t num_flooding_ifs;
	/*!< Number of flooding interfaces */
};

/**
 * @brief	Get VLAN attributes
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	vlan_id		VLAN Identifier
 * @param[out]  attr		DPSW attributes
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_get_attributes(struct fsl_mc_io		*mc_io,
			     uint16_t			token,
			     uint16_t			vlan_id,
			     struct dpsw_vlan_attr	*attr);

/**
 * @brief	Get interfaces belong to this VLAN
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	vlan_id		VLAN Identifier
 * @param[out]  cfg		Set of interfaces belong to this VLAN
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_get_if(struct fsl_mc_io		*mc_io,
		     uint16_t			token,
		     uint16_t			vlan_id,
		     struct dpsw_vlan_if_cfg	*cfg);

/**
 * @brief	Get interfaces used in flooding for this VLAN
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	vlan_id		VLAN Identifier
 * @param[out]  cfg		Set of flooding interfaces
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_get_if_flooding(struct fsl_mc_io		*mc_io,
			      uint16_t			token,
			      uint16_t			vlan_id,
			      struct dpsw_vlan_if_cfg	*cfg);

/**
 * @brief	Get interfaces that should be transmitted as untagged
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	vlan_id		VLAN Identifier
 * @param[out]  cfg		Set of untagged interfaces
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_get_if_untagged(struct fsl_mc_io		*mc_io,
			      uint16_t			token,
			      uint16_t			vlan_id,
			      struct dpsw_vlan_if_cfg	*cfg);

/**
 * @brief	FDB Configuration
 *
 */
struct dpsw_fdb_cfg {
	uint16_t num_fdb_entries;
	/*!< Number of FDB entries */
	uint16_t fdb_aging_time;
	/*!< Aging time in seconds */
};

/**
 * @brief	Add FDB to switch and Returns handle to FDB table for
 *		the reference
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[out]	fdb_id		Forwarding Database Identifier
 * @param[in]	cfg		FDB Configuration
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_add(struct fsl_mc_io		*mc_io,
		 uint16_t			token,
		 uint16_t			*fdb_id,
		 const struct dpsw_fdb_cfg	*cfg);

/**
 * @brief	Remove FDB from switch
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	fdb_id		Forwarding Database Identifier
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_remove(struct fsl_mc_io *mc_io, uint16_t token, uint16_t fdb_id);

/**
 * @brief	FDB Entry type - Static/Dynamic
 *
 */
enum dpsw_fdb_entry_type {
	DPSW_FDB_ENTRY_STATIC = 0,
	/*!< Static entry */
	DPSW_FDB_ENTRY_DINAMIC
	/*!< Dynamic entry */
};

/**
 * @brief	Structure representing unicast entry configuration
 *
 */
struct dpsw_fdb_unicast_cfg {
	enum dpsw_fdb_entry_type type;
	/*!< Select static or dynamic entry */
	uint8_t mac_addr[6];
	/*!< MAC address */
	uint16_t if_egress;
	/*!< Egress interface ID */
};

/**
 * @brief	Function adds an unicast entry into MAC lookup table
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	fdb_id		Forwarding Database Identifier
 * @param[in]	cfg		Unicast entry configuration
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_add_unicast(struct fsl_mc_io			*mc_io,
			 uint16_t				token,
			 uint16_t				fdb_id,
			 const struct dpsw_fdb_unicast_cfg	*cfg);

/**
 * @brief	Get unicast entry from MAC lookup table by
 *		unicast Ethernet address
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	fdb_id		Forwarding Database Identifier
 * @param[in,out] cfg		Unicast entry configuration
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_get_unicast(struct fsl_mc_io		*mc_io,
			 uint16_t			token,
			 uint16_t			fdb_id,
			 struct dpsw_fdb_unicast_cfg	*cfg);

/**
 * @brief	removes an entry from MAC lookup table
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	fdb_id		Forwarding Database Identifier
 * @param[in]	cfg		Unicast entry configuration
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_remove_unicast(struct fsl_mc_io			*mc_io,
			    uint16_t				token,
			    uint16_t				fdb_id,
			    const struct dpsw_fdb_unicast_cfg	*cfg);

/**
 * @brief	Structure representing multi-cast entry configuration
 *
 */
struct dpsw_fdb_multicast_cfg {
	enum dpsw_fdb_entry_type type;
	/*!< Select static or dynamic entry */
	uint8_t mac_addr[6];
	/*!< MAC address */
	uint16_t num_ifs;
	/*!< Number of external and internal interfaces */
	uint16_t if_id[DPSW_MAX_IF];
	/*!< Egress interface IDs */
};

/**
 * @brief	Add a set of egress interfaces to multi-cast group.
 *
 *		If group doesn't exist, it will be created.
 *		It adds only interfaces not belonging to this multicast group
 *		yet, otherwise error will be generated and the command is
 *		ignored.
 *		This function may be called numerous times always providing
 *		required interfaces delta.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	fdb_id		Forwarding Database Identifier
 * @param[in]	cfg		Multicast entry configuration
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_add_multicast(struct fsl_mc_io			*mc_io,
			   uint16_t				token,
			   uint16_t				fdb_id,
			   const struct dpsw_fdb_multicast_cfg	*cfg);

/**
 * @brief	Reading multi-cast group by multi-cast Ethernet address.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	fdb_id		Forwarding Database Identifier
 * @param[in,out] cfg		Multicast entry configuration
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_get_multicast(struct fsl_mc_io			*mc_io,
			   uint16_t				token,
			   uint16_t				fdb_id,
			   struct dpsw_fdb_multicast_cfg	*cfg);

/**
 * @brief	Removing interfaces from an existing multicast group.
 *
 *		Interfaces provided by this API have to exist in the group,
 *		otherwise an error will be returned and an entire command
 *		ignored. If there is no interface left in the group,
 *		an entire group is deleted
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	fdb_id		Forwarding Database Identifier
 * @param[in]	cfg		Multicast entry configuration
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_remove_multicast(struct fsl_mc_io *mc_io, uint16_t token,
			      uint16_t fdb_id,
			      const struct dpsw_fdb_multicast_cfg *cfg);

/**
 * @brief	Auto-learning modes
 *	NONE - SECURE LEARNING
 *	SMAC found	DMAC found	CTLU Action
 *	v		v	Forward frame to
 *						1.  DMAC destination
 *	-		v	Forward frame to
 *						1.  DMAC destination
 *						2.  Control interface
 *	v		-	Forward frame to
 *						1.  Flooding list of interfaces
 *	-		-	Forward frame to
 *						1.  Flooding list of interfaces
 *						2.  Control interface
 *	SECURE LEARING
 *	SMAC found	DMAC found	CTLU Action
 *	v		v		Forward frame to
 *						1.  DMAC destination
 *	-		v		Forward frame to
 *						1.  Control interface
 *	v		-		Forward frame to
 *						1.  Flooding list of interfaces
 *	-		-		Forward frame to
 *						1.  Control interface
 */
enum dpsw_fdb_learning_mode {
	DPSW_FDB_LEARNING_MODE_DIS = 0,
	/*!< Disable Auto-learning */
	DPSW_FDB_LEARNING_MODE_HW,
	/*!< Enable HW auto-Learning */
	DPSW_FDB_LEARNING_MODE_NON_SECURE,
	/*!< Enable None secure learning by CPU */
	DPSW_FDB_LEARNING_MODE_SECURE
	/*!< Enable secure learning by CPU */
};

/**
 * @brief        defines FDB learning mode
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	fdb_id		Forwarding Database Identifier
 * @param[in]	mode		learning mode
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_set_learning_mode(struct fsl_mc_io			*mc_io,
			       uint16_t				token,
			       uint16_t				fdb_id,
			       enum dpsw_fdb_learning_mode	mode);

/**
 * @brief	FDB Attributes
 */
struct dpsw_fdb_attr {
	uint16_t max_fdb_entries;
	/*!< Number of FDB entries */
	uint16_t fdb_aging_time;
	/*!< Aging time in seconds */
	enum dpsw_fdb_learning_mode learning_mode;
	/*!< Learning mode */
	uint16_t num_fdb_mc_groups;
	/*! Current number of multicast groups */
	uint16_t max_fdb_mc_groups;
	/*! Maximum number of multicast groups */
};

/**
 * @brief       Get FDB attributes
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSW object
 * @param[in]	fdb_id		Forwarding Database Identifier
 * @param[out]	attr		FDB attributes
 *
 * @returns	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_get_attributes(struct fsl_mc_io		*mc_io,
			    uint16_t			token,
			    uint16_t			fdb_id,
			    struct dpsw_fdb_attr	*attr);



/*! @} */

#endif /* __FSL_DPSW_H */

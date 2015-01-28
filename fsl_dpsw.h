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
#ifndef __FSL_DPSW_H
#define __FSL_DPSW_H

#include "fsl_net.h"

/* Data Path DPSW API
 * Contains API for handling DPSW topology and functionality
 */

struct fsl_mc_io;

/* DPSW general definitions */

/*!< Maximum number of traffic class priorities */
#define DPSW_MAX_PRIORITIES	8
/*!< Maximum number of interfaces */
#define DPSW_MAX_IF		64

/**
 * dpsw_open() - Open a control session for the specified object
 * @mc_io:	Pointer to MC portal's I/O object
 * @dpsw_id:	DPSW unique ID
 * @token:	Returned token; use in subsequent API calls
 *
 * This function can be used to open a control session for an
 * already created object; an object may have been declared in
 * the DPL or by calling the dpsw_create() function.
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent commands for
 * this specific object
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_open(struct fsl_mc_io *mc_io, int dpsw_id, uint16_t *token);

/**
 * dpsw_close() - Close the control session of the object
 * @mc_io	Pointer to MC portal's I/O object
 * @token	Token of DPSW object
 *
 * After this function is called, no further operations are
 * allowed on the object without opening a new control session.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_close(struct fsl_mc_io *mc_io, uint16_t token);

/* DPSW options */

/* Disable flooding */
#define DPSW_OPT_FLOODING_DIS	0x0000000000000001ULL
/* Disable Multicast */
#define DPSW_OPT_MULTICAST_DIS	0x0000000000000004ULL
/* Support control interface */
#define DPSW_OPT_CTRL		0x0000000000000010ULL

/**
 * struct dpsw_cfg - DPSW configuration
 * @num_ifs: Number of external and internal interfaces
 * @adv: Advanced parameters; default is all zeros;
 *		 use this structure to change default settings
 */
struct dpsw_cfg {
	uint16_t num_ifs;
	/**
	 * struct adv - Advanced parameters
	 * @options: Enable/Disable DPSW features (bitmap)
	 * @max_vlans: Maximum Number of VLAN's; 0 - indicates default 16
	 * @max_fdbs: Maximum Number of FDB's; 0 - indicates default 16
	 * @max_fdb_entries: Number of FDB entries for default FDB table;
	 *			0 - indicates default 1024 entries.
	 * @fdb_aging_time: Default FDB aging time for default FDB table;
	 *			0 - indicates default 300 seconds
	 * @max_fdb_mc_groups: Number of multicast groups in each FDB table;
	 *			0 - indicates default 32
	 * @ctrl_if_id: Control interface
	 */
	struct {
		uint64_t options;
		uint16_t max_vlans;
		uint8_t max_fdbs;
		uint16_t max_fdb_entries;
		uint16_t fdb_aging_time;
		uint16_t max_fdb_mc_groups;
		uint16_t ctrl_if_id;
	} adv;
};

/**
 * dpsw_create() - Create the DPSW object.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cfg:	Configuration structure
 * @token:	Returned token; use in subsequent API calls
 *
 * Create the DPSW object, allocate required resources and
 * perform required initialization.
 *
 * The object can be created either by declaring it in the
 * DPL file, or by calling this function.
 *
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent calls to
 * this specific object. For objects that are created using the
 * DPL file, call dpsw_open() function to get an authentication
 * token first
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_create(struct fsl_mc_io *mc_io,
		const struct dpsw_cfg *cfg,
		uint16_t *token);

/**
 * dpsw_destroy() - Destroy the DPSW object and release all its resources.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 *
 * Return:	'0' on Success; error code otherwise.
 */
int dpsw_destroy(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * dpsw_enable() - Enable DPSW functionality
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_enable(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * dpsw_disable() - Disable DPSW functionality
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_disable(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * dpsw_is_enabled() - Check if the DPSW is enabled
 *
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @en:		Returns '1' if object is enabled; '0' otherwise
 *
 * Return:	'0' on Success; Error code otherwise
 */
int dpsw_is_enabled(struct fsl_mc_io *mc_io, uint16_t token, int *en);

/**
 * dpsw_reset() - Reset the DPSW, returns the object to initial state.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_reset(struct fsl_mc_io *mc_io, uint16_t token);

/* DPSW IRQ Index and Events */

#define DPSW_IRQ_INDEX_IF		0x0000
#define DPSW_IRQ_INDEX_L2SW		0x0001

/*!< IRQ event - Indicates that the link state changed */
#define DPSW_IRQ_EVENT_LINK_CHANGED	0x0001

/**
 * dpsw_set_irq() - Set IRQ information for the DPSW to trigger an interrupt.
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @irq_index:	Identifies the interrupt index to configure
 * @irq_addr:	Address that must be written to
 *				signal a message-based interrupt
 * @irq_val:	Value to write into irq_addr address
 * @user_irq_id: A user defined number associated with this IRQ
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_set_irq(struct fsl_mc_io *mc_io,
		 uint16_t token,
		 uint8_t irq_index,
		 uint64_t irq_addr,
		 uint32_t irq_val,
		 int user_irq_id);

/**
 * @dpsw_get_irq() - Get IRQ information from the DPSW
 *
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @irq_index:	The interrupt index to configure
 * @type:		Returned interrupt type: 0 represents message interrupt
 *				type (both irq_addr and irq_val are valid)
 * @irq_addr:	Returned address that must be written to
 *				signal the message-based interrupt
 * @irq_val:	Value to write into irq_addr address
 * @user_irq_id: A user defined number associated with this IRQ
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_get_irq(struct fsl_mc_io *mc_io,
		 uint16_t token,
		 uint8_t irq_index,
		 int *type,
		 uint64_t *irq_addr,
		 uint32_t *irq_val,
		 int *user_irq_id);

/**
 * dpsw_set_irq_enable() - Set overall interrupt state.
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPCI object
 * @irq_index:	The interrupt index to configure
 * @en:			Interrupt state - enable = 1, disable = 0
 *
 * Allows GPP software to control when interrupts are generated.
 * Each interrupt can have up to 32 causes.  The enable/disable control's the
 * overall interrupt state. if the interrupt is disabled no causes will cause
 * an interrupt
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_set_irq_enable(struct fsl_mc_io *mc_io,
			uint16_t token,
			uint8_t irq_index,
			uint8_t en);

/**
 * dpsw_get_irq_enable() - Get overall interrupt state
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @irq_index:	The interrupt index to configure
 * @en:			Returned Interrupt state - enable = 1, disable = 0
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_get_irq_enable(struct fsl_mc_io *mc_io,
			uint16_t token,
			uint8_t irq_index,
			uint8_t *en);

/**
 * dpsw_set_irq_mask() - Set interrupt mask.
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPCI object
 * @irq_index:	The interrupt index to configure
 * @mask:		event mask to trigger interrupt;
 *				each bit:
 *					0 = ignore event
 *					1 = consider event for asserting irq
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_set_irq_mask(struct fsl_mc_io *mc_io,
		      uint16_t token,
		      uint8_t irq_index,
		      uint32_t mask);

/**
 * dpsw_get_irq_mask() - Get interrupt mask.
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @irq_index:	The interrupt index to configure
 * @mask:		Returned event mask to trigger interrupt
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_get_irq_mask(struct fsl_mc_io *mc_io,
		      uint16_t token,
		      uint8_t irq_index,
		      uint32_t *mask);

/**
 * dpsw_get_irq_status() - Get the current status of any pending interrupts
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @irq_index:	The interrupt index to configure
 * @status:		Returned interrupts status - one bit per cause:
 *					0 = no interrupt pending
 *					1 = interrupt pending
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_get_irq_status(struct fsl_mc_io *mc_io,
			uint16_t token,
			uint8_t irq_index,
			uint32_t *status);

/**
 * dpsw_clear_irq_status() - Clear a pending interrupt's status
 * @mc_io		Pointer to MC portal's I/O object
 * @token		Token of DPCI object
 * @irq_index	The interrupt index to configure
 * @status		bits to clear (W1C) - one bit per cause:
 *					0 = don't change
 *					1 = clear status bit
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_clear_irq_status(struct fsl_mc_io *mc_io,
			  uint16_t token,
			  uint8_t irq_index,
			  uint32_t status);
/**
 * struct dpsw_attr - Structure representing DPSW attributes
 * @id: DPSW object ID
 * @version: DPSW version
 * @options: Enable/Disable DPSW features
 * @max_vlans: Maximum Number of VLANs
 * @max_fdbs: Maximum Number of FDBs
 * @mem_size: DPSW frame storage memory size
 * @num_ifs: Number of interfaces
 * @num_vlans: Current number of VLANs
 * @num_fdbs: Current number of FDBs
 */
struct dpsw_attr {
	int id;
	/**
	 * struct version - DPSW version
	 * @major: DPSW major version
	 * @minor: DPSW minor version
	 */
	struct {
		uint16_t major;
		uint16_t minor;
	} version;
	uint64_t options;
	uint16_t max_vlans;
	uint8_t max_fdbs;
	uint16_t mem_size;
	uint16_t num_ifs;
	uint16_t num_vlans;
	uint8_t num_fdbs;
};

/**
 * dpsw_get_attributes() - Retrieve DPSW attributes
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @attr:		Returned DPSW attributes
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_get_attributes(struct fsl_mc_io *mc_io,
			uint16_t token,
			struct dpsw_attr *attr);

/**
 * enum dpsw_policer_mode - Policer configuration mode
 * @DPSW_POLICER_DIS: Disable Policer
 * @DPSW_POLICER_EN: Enable Policer
 */
enum dpsw_policer_mode {
	DPSW_POLICER_DIS = 0,
	DPSW_POLICER_EN = 1
};

/**
 * struct dpsw_policer_cfg - Policer configuration
 * @mode: Enables/Disables policer (Ingress)
 */
struct dpsw_policer_cfg {
	enum dpsw_policer_mode mode;
};

/**
 * dpsw_set_policer() - Enable/disable policer for DPSW
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @cfg:		Configuration parameters
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_set_policer(struct fsl_mc_io *mc_io,
		     uint16_t token,
		     const struct dpsw_policer_cfg *cfg);

/**
 * struct dpsw_buffer_depletion_cfg - buffer depletion configuration parameters.
 *			Assuming only one buffer pool exist per switch.
 * @entrance_threshold: Entrance threshold, the number of buffers in a pool
 *			falls below a programmable depletion entry threshold
 * @exit_threshold: Exit threshold. When the buffer pool's occupancy rises above
 *			a programmable depletion exit threshold, the buffer pool
 *			exits depletion
 * @wr_addr: Address in GPP to write buffer depletion State Change Notification
 *			Message
 */
struct dpsw_buffer_depletion_cfg {
	uint32_t entrance_threshold;
	uint32_t exit_threshold;
	uint64_t wr_addr;
};

/**
 * dpsw_set_buffer_depletion() - Configure thresholds for buffer depletion state
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @cfg:		Configuration parameters
 *
 * Configure thresholds for buffer depletion state
 * and establish buffer depletion State Change Notification Message
 * (CSCNM) from Congestion Point (CP) to GPP trusted software
 * This configuration is used to trigger PFC request or congestion
 * notification if enabled. It is assumed one buffer pool defined
 * per switch.
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_set_buffer_depletion(struct fsl_mc_io *mc_io,
			      uint16_t token,
			      const struct dpsw_buffer_depletion_cfg *cfg);

/**
 * dpsw_set_reflection_if() - Set target interface for reflected interfaces.
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @if_id:		Interface Id
 *
 *	Only one reflection receive interface is allowed per switch
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_set_reflection_if(struct fsl_mc_io *mc_io,
			   uint16_t token,
			   uint16_t if_id);

/**
 * dpsw_set_ctrl_if() - Set control interface id
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @if_id:	Interface Id

 * Return:      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_set_ctrl_if(struct fsl_mc_io *mc_io,
		     uint16_t token,
		     uint16_t if_id);

/**
 * dpsw_get_ctrl_if - Get control interface id
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @if_id:	Returned interface ID
 *
 * Return:      Completion status. '0' on Success; Error - when control
 *		interface disabled
 */
int dpsw_get_ctrl_if(struct fsl_mc_io *mc_io,
		     uint16_t token,
		     uint16_t *if_id);

/**
 * enum dpsw_action - Action selection for special/control frames
 * @DPSW_ACTION_DROP: Drop frame
 * @DPSW_ACTION_REDIRECT: Redirect frame to control port
 */
enum dpsw_action {
	DPSW_ACTION_DROP = 0,
	DPSW_ACTION_REDIRECT = 1
};

/* Precision Time Protocol (PTP) options */

/* Indicate the need for UDP checksum update after PTP time correction
 * field has been filled in
 */
#define DPSW_PTP_OPT_UPDATE_FCV	0x1

/**
 * struct dpsw_ptp_v2_cfg - Precision Time Protocol (PTP) configuration
 * @enable: Enable updating Correction time field in IEEE1588 V2 messages
 * @time_offset: Time correction field offset inside PTP from L2 start of the
 *			frame; PTP messages can be transported over different
 *			underlying protocols IEEE802.3, IPv4/UDP, Ipv6/UDP and
 *			others
 * @options: Bitmap of additional options along with time correction;
 *		 Select from 'DPSW_PTP_OPT_<X>'
 */
struct dpsw_ptp_v2_cfg {
	int enable;
	uint16_t time_offset;
	uint32_t options;
};

/**
 * dpsw_set_ptp_v2() - Define IEEE1588 V2 Precision Time Protocol
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @cfg:		IEEE1588 V2 Configuration parameters
 *
 * (PTP) parameters for time correction
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_set_ptp_v2(struct fsl_mc_io *mc_io,
		    uint16_t token,
		    const struct dpsw_ptp_v2_cfg *cfg);


/**
 * struct dpsw_link_cfg - Structure representing DPSW link configuration
 * @rate: Rate
 * @options: Mask of available options; use 'DPSW_LINK_OPT_<X>' values
 */
struct dpsw_link_cfg {
	uint64_t rate;
	uint64_t options;
};

/**
 * dpsw_if_set_link_cfg() - set the link configuration.
 * @mc_io: Pointer to MC portal's I/O object
 * @token: Token of DPSW object
 * @if_id: interface id
 * @cfg: Link configuration
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_if_set_link_cfg(struct fsl_mc_io *mc_io,
			 uint16_t token,
			 uint16_t if_id,
			 struct dpsw_link_cfg *cfg);
/**
 * struct dpsw_link_state - Structure representing DPSW link state
 * @rate: Rate
 * @options: Mask of available options; use 'DPSW_LINK_OPT_<X>' values
 * @up: 0 - covers two cases: down and disconnected, 1 - up
 */
struct dpsw_link_state {
	uint64_t rate;
	uint64_t options;
	int      up;
};

/**
 * dpsw_if_get_link_state - Return the link state
 * @mc_io: Pointer to MC portal's I/O object
 * @token: Token of DPSW object
 * @if_id: interface id
 * @state: link state
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpsw_if_get_link_state(struct fsl_mc_io *mc_io,
			   uint16_t token,
			   uint16_t if_id,
			   struct dpsw_link_state *state);

/**
 * dpsw_if_set_flooding() - Enable Disable flooding for particular interface
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @if_id:		Interface Identifier
 * @en:			1 - enable, 0 - disable

 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_flooding(struct fsl_mc_io *mc_io,
			 uint16_t token,
			 uint16_t if_id,
			 int en);

/**
 * dpsw_if_set_broadcast() - Enable/disable broadcast for particular interface
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @if_id:		Interface Identifier
 * @en:			1 - enable, 0 - disable

 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_broadcast(struct fsl_mc_io *mc_io,
			  uint16_t token,
			  uint16_t if_id,
			  int en);

/**
 * dpsw_if_set_multicast() - Enable/disable multicast for particular interface
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @if_id:		Interface Identifier
 * @en:			1 - enable, 0 - disable
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_multicast(struct fsl_mc_io *mc_io,
			  uint16_t token,
			  uint16_t if_id,
			  int en);

/**
 * struct dpsw_tci_cfg - Tag Contorl Information (TCI) configuration
 * @pcp: Priority Code Point (PCP): a 3-bit field which refers
 *		 to the IEEE 802.1p priority
 * @dei: Drop Eligible Indicator (DEI): a 1-bit field. May be used
 *		 separately or in conjunction with PCP to indicate frames
 *		 eligible to be dropped in the presence of congestion
 * @vlan_id: VLAN Identifier (VID): a 12-bit field specifying the VLAN
 *			to which the frame belongs. The hexadecimal values
 *			of 0x000 and 0xFFF are reserved;
 *			all other values may be used as VLAN identifiers,
 *			allowing up to 4,094 VLANs
 */
struct dpsw_tci_cfg {
	uint8_t pcp;
	uint8_t dei;
	uint16_t vlan_id;
};

/**
 * dpsw_if_set_tci() - Set default VLAN Tag Control Information (TCI)
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @if_id:		Interface Identifier
 * @cfg:		Tag Control Information Configuration

 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_tci(struct fsl_mc_io *mc_io,
		    uint16_t token,
		    uint16_t if_id,
		    const struct dpsw_tci_cfg *cfg);

/**
 * dpsw_if_get_tci() - Get default VLAN Tag Control Information (TCI)
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @if_id:		Interface Identifier
 * @cfg:		Tag Control Information Configuration

 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_get_tci(struct fsl_mc_io *mc_io,
		    uint16_t token,
		    uint16_t if_id,
		    struct dpsw_tci_cfg *cfg);

/**
 * enum dpsw_stp_state - Spanning Tree Protocol (STP) states
 * @DPSW_STP_STATE_BLOCKING: Blocking state
 * @DPSW_STP_STATE_LISTENING: Listening state
 * @DPSW_STP_STATE_LEARNING: Learning state
 * @DPSW_STP_STATE_FORWARDING: Forwarding state
 *
 */
enum dpsw_stp_state {
	DPSW_STP_STATE_BLOCKING = 0,
	DPSW_STP_STATE_LISTENING = 1,
	DPSW_STP_STATE_LEARNING = 2,
	DPSW_STP_STATE_FORWARDING = 3
};

/**
 * struct dpsw_stp_cfg - Spanning Tree Protocol (STP) Configuration
 * @vlan_id: VLAN ID STP state
 * @state: STP state
 */
struct dpsw_stp_cfg {
	uint16_t vlan_id;
	enum dpsw_stp_state state;
};

/**
 * dpsw_if_set_stp() - Function sets Spanning Tree Protocol (STP) state.
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @if_id:		Interface Identifier
 * @cfg:		STP State configuration parameters
 *
 * The following STP states are supported -
 * blocking, listening, learning, forwarding and disabled.
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_stp(struct fsl_mc_io *mc_io,
		    uint16_t token,
		    uint16_t if_id,
		    const struct dpsw_stp_cfg *cfg);

/**
 * enum dpsw_accepted_frames - Types of frames to accept
 * @DPSW_ADMIT_ALL: The device accepts VLAN tagged, untagged and
 *			priority tagged frames
 * @DPSW_ADMIT_ONLY_VLAN_TAGGED: The device discards untagged frames or
 *			Priority-Tagged frames received on this interface.
 *
 */
enum dpsw_accepted_frames {
	DPSW_ADMIT_ALL = 1,
	DPSW_ADMIT_ONLY_VLAN_TAGGED = 3
};

/**
 * struct dpsw_accepted_frames_cfg - Types of frames to accept configuration
 * @type: Defines ingress accepted frames
 * @unaccept_act: When a frame is not accepted, it may be discarded or
 *			redirected to control interface depending on this mode
 */
struct dpsw_accepted_frames_cfg {
	enum dpsw_accepted_frames type;
	enum dpsw_action unaccept_act;
};

/**
 * dpsw_if_set_accepted_frames()
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @if_id:		Interface Identifier
 * @cfg:		Frame types configuration
 *
 * When is admit_only_vlan_tagged- the device will discard untagged
 * frames or Priority-Tagged frames received on this interface.
 * When admit_only_untagged- untagged frames or Priority-Tagged
 * frames received on this interface will be accepted and assigned
 * to a VID based on the PVID and VID Set for this interface.
 * When admit_all - the device will accept VLAN tagged, untagged
 * and priority tagged frames.
 * The default is admit_all

 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_accepted_frames(struct fsl_mc_io *mc_io,
				uint16_t token,
				uint16_t if_id,
				const struct dpsw_accepted_frames_cfg *cfg);

/**
 * dpsw_if_set_accept_all_vlan()
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @if_id:		Interface Identifier
 * @accept_all:	Accept or drop frames having different VLAN
 *
 * When this is accept (FALSE), the device will discard incoming
 * frames for VLANs that do not include this interface in its
 * Member set. When accept (TRUE), the interface will accept all incoming frames
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_accept_all_vlan(struct fsl_mc_io *mc_io,
				uint16_t token,
				uint16_t if_id,
				int accept_all);

/**
 * enum dpsw_counter  - Counters types
 * @DPSW_CNT_ING_FRAME: Counts ingress frames
 * @DPSW_CNT_ING_BYTE: Counts ingress bytes
 * @DPSW_CNT_ING_FLTR_FRAME: Counts filtered ingress frames
 * @DPSW_CNT_ING_FRAME_DISCARD: Counts discarded ingress frame
 * @DPSW_CNT_ING_MCAST_FRAME: Counts ingress multicast frames
 * @DPSW_CNT_ING_MCAST_BYTE: Counts ingress multicast bytes
 * @DPSW_CNT_ING_BCAST_FRAME: Counts ingress broadcast frames
 * @DPSW_CNT_ING_BCAST_BYTES: Counts ingress broadcast bytes
 * @DPSW_CNT_EGR_FRAME: Counts egress frames
 * @DPSW_CNT_EGR_BYTE: Counts eEgress bytes
 * @DPSW_CNT_EGR_FRAME_DISCARD: Counts discarded egress frames
 *
 */
enum dpsw_counter {
	DPSW_CNT_ING_FRAME = 0x0,
	DPSW_CNT_ING_BYTE = 0x1,
	DPSW_CNT_ING_FLTR_FRAME = 0x2,
	DPSW_CNT_ING_FRAME_DISCARD = 0x3,
	DPSW_CNT_ING_MCAST_FRAME = 0x4,
	DPSW_CNT_ING_MCAST_BYTE = 0x5,
	DPSW_CNT_ING_BCAST_FRAME = 0x6,
	DPSW_CNT_ING_BCAST_BYTES = 0x7,
	DPSW_CNT_EGR_FRAME = 0x8,
	DPSW_CNT_EGR_BYTE = 0x9,
	DPSW_CNT_EGR_FRAME_DISCARD = 0xa
};

/**
 * dpsw_if_get_counter() - Get specific counter of particular interface
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @if_id:		Interface Identifier
 * @type:		Counter type
 * @counter:	return value
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_get_counter(struct fsl_mc_io *mc_io,
			uint16_t token,
			uint16_t if_id,
			enum dpsw_counter type,
			uint64_t *counter);

/**
 * dpsw_if_set_counter() - Set specific counter of particular interface
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @if_id:		Interface Identifier
 * @type:		Counter type
 * @counter:	New counter value
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_counter(struct fsl_mc_io *mc_io,
			uint16_t token,
			uint16_t if_id,
			enum dpsw_counter type,
			uint64_t counter);

/**
 * enum dpsw_priority_selector - User priority
 * @DPSW_UP_PCP: Priority Code Point (PCP): a 3-bit field which
 *				 refers to the IEEE 802.1p priority.
 * @DPSW_UP_DSCP: Differentiated services Code Point (DSCP): 6 bit
 *				field from IP header
 *
 */
enum dpsw_priority_selector {
	DPSW_UP_PCP = 0,
	DPSW_UP_DSCP = 1
};

/**
 * struct dpsw_tc_map_cfg - Mapping user priority into traffic class
 *				configuration
 * @priority_selector: Source for user priority regeneration
 * @tc_id: The Regenerated User priority that the incoming
 *				User Priority is mapped to for this interface
 *
 */
struct dpsw_tc_map_cfg {
	enum dpsw_priority_selector priority_selector;
	uint8_t tc_id[DPSW_MAX_PRIORITIES];
};

/**
 * dpsw_if_set_tc_map() - Function is used for mapping variety of frame fields
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @if_id:		Interface Identifier
 * @cfg:		Traffic class mapping configuration
 *
 * Function is used for mapping variety of frame fields (DSCP, PCP)
 * to Traffic Class. Traffic class is a number
 * in the range from 0 to 7
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_tc_map(struct fsl_mc_io *mc_io,
		       uint16_t token,
		       uint16_t if_id,
		       const struct dpsw_tc_map_cfg *cfg);

/**
 * enum dpsw_reflection_filter - Filter type for frames to reflect
 * @DPSW_REFLECTION_FILTER_INGRESS_ALL - Reflect all frames
 * @DPSW_REFLECTION_FILTER_INGRESS_VLAN - Reflect only frames belong to
 *			particular VLAN defined by vid parameter
 *
 */
enum dpsw_reflection_filter {
	DPSW_REFLECTION_FILTER_INGRESS_ALL = 0,
	DPSW_REFLECTION_FILTER_INGRESS_VLAN = 1
};

/**
 * struct dpsw_reflection_cfg - Structure representing reflection information
 * @filter: Filter type for frames to reflect
 * @vlan_id: Vlan Id to reflect; valid only when filter type is
 *		DPSW_INGRESS_VLAN
 */
struct dpsw_reflection_cfg {
	enum dpsw_reflection_filter filter;
	uint16_t vlan_id;
};

/**
 * dpsw_if_add_reflection() - Identify interface to be reflected or mirrored
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @if_id:		Interface Identifier
 * @cfg:		Reflection configuration
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_add_reflection(struct fsl_mc_io *mc_io,
			   uint16_t token,
			   uint16_t if_id,
			   const struct dpsw_reflection_cfg *cfg);

/**
 * dpsw_if_remove_reflection() - Remove interface to be reflected or mirrored
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @if_id:		Interface Identifier
 * @cfg:		Reflection configuration
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_remove_reflection(struct fsl_mc_io *mc_io,
			      uint16_t token,
			      uint16_t if_id,
			      const struct dpsw_reflection_cfg *cfg);

/**
 * enum dpsw_metering_algo - Metering and marking algorithms
 * @DPSW_METERING_ALGO_RFC2698: RFC 2698
 * @DPSW_METERING_ALGO_RFC4115: RFC 4115
 */
enum dpsw_metering_algo {
	DPSW_METERING_ALGO_RFC2698 = 0,
	DPSW_METERING_ALGO_RFC4115 = 1
};

/**
 * enum dpsw_metering_mode - Metering and marking modes
 * @DPSW_METERING_MODE_COLOR_BLIND: Color blind mode
 * @DPSW_METERING_MODE_COLOR_AWARE: Color aware mode
 */
enum dpsw_metering_mode {
	DPSW_METERING_MODE_COLOR_BLIND = 0,
	DPSW_METERING_MODE_COLOR_AWARE = 1
};

/**
 * struct dpsw_metering_cfg - Metering and marking configuration
 * @algo: Implementation based on Metering and marking algorithm
 * @cir: Committed information rate (CIR) in bits/s
 * @eir: Excess information rate (EIR) in bits/s
 * @cbs: Committed burst size (CBS) in bytes
 * @ebs: Excess bust size (EBS) in bytes
 * @mode: Color awareness mode
 *
 */
struct dpsw_metering_cfg {
	enum dpsw_metering_algo algo;
	uint32_t cir;
	uint32_t eir;
	uint32_t cbs;
	uint32_t ebs;
	enum dpsw_metering_mode mode;
};

/**
 * dpsw_if_tc_set_metering_marking() - Set metering and marking algorithm
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @if_id:		Interface Identifier
 * @tc_id:		Traffic class selection (1-8)
 * @cfg:		Metering and marking parameters
 *
 * Set metering and marking algorithm (coloring) and provides
 * corresponding parameters
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_tc_set_metering_marking(struct fsl_mc_io *mc_io,
				    uint16_t token,
				    uint16_t if_id,
				    uint8_t tc_id,
				    const struct dpsw_metering_cfg *cfg);

/**
 * struct dpsw_custom_tpid_cfg - Structure representing tag Protocol identifier
 * @tpid: An additional tag protocol identifier
 */
struct dpsw_custom_tpid_cfg {
	uint16_t tpid;
};

/**
 * dpsw_add_custom_tpid() - API Configures a distinct Ethernet type value
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @cfg:		Tag Protocol identifier
 *
 * API Configures a distinct Ethernet type value (or TPID value)
 * to indicate a VLAN tag in addition to the common
 * TPID values 0x8100 and 0x88A8.
 * Two additional TPID's are supported
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_add_custom_tpid(struct fsl_mc_io *mc_io,
			 uint16_t token,
			 const struct dpsw_custom_tpid_cfg *cfg);

/**
 * dpsw_remove_custom_tpid - API removes a distinct Ethernet type value
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @cfg:		Tag Protocol identifier
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_remove_custom_tpid(struct fsl_mc_io *mc_io,
			    uint16_t token,
			    const struct dpsw_custom_tpid_cfg *cfg);

/**
 * struct dpsw_transmit_rate_cfg - Transmit rate configuration
 * @rate: Interface Transmit rate in bits per second
 *
 */
struct dpsw_transmit_rate_cfg {
	uint64_t rate;
};

/**
 * dpsw_if_set_transmit_rate() - API sets interface transmit rate.
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @if_id:		Interface Identifier
 * @cfg:		Transmit rate configuration
 *
 * The setting mechanism is the same for internal and
 * external (physical) interfaces.
 * The rate set explicitly in bits per second.
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_transmit_rate(struct fsl_mc_io *mc_io,
			      uint16_t token,
			      uint16_t if_id,
			      const struct dpsw_transmit_rate_cfg *cfg);

/**
 * enum dpsw_bw_algo - Transmission selection algorithm
 * @DPSW_BW_ALGO_STRICT_PRIORITY: Strict priority algorithm
 * @DPSW_BW_ALGO_CREDIT_BASED: Credit based shaper algorithm
 */
enum dpsw_bw_algo {
	DPSW_BW_ALGO_STRICT_PRIORITY = 0,
	DPSW_BW_ALGO_CREDIT_BASED = 1
};

/**
 * struct dpsw_bandwidth_cfg - Class bandwidth configuration
 * @algo: Transmission selection algorithm
 * @delta_bandwidth: A percentage of the interface transmit rate; applied only
 *			when using credit-based shaper algorithm otherwise best
 *			effort algorithm is applied
 */
struct dpsw_bandwidth_cfg {
	enum dpsw_bw_algo algo;
	uint8_t delta_bandwidth;
};

/**
 * dpsw_if_tc_set_bandwidth() - Set a percentage of the interface transmit rate
 * @mc_io		Pointer to MC portal's I/O object
 * @token		Token of DPSW object
 * @if_id		Interface Identifier
 * @tc_id		Traffic class selection (1-8)
 * @cfg		Traffic class bandwidth configuration
 *
 *
 * API sets a percentage of the interface transmit rate;
 * this is the bandwidth that can be reserved for use by the queue
 * associated with traffic class. A percentage is relevant
 * only when credit based shaping algorithm is selected for
 * traffic class otherwise best effort (strict priority)
 * algorithm is in place
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_tc_set_bandwidth(struct fsl_mc_io *mc_io,
			     uint16_t token,
			     uint16_t if_id,
			     uint8_t tc_id,
			     const struct dpsw_bandwidth_cfg *cfg);

/**
 * dpsw_if_enable() - Enable Interface
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @if_id:		Interface Identifier
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_enable(struct fsl_mc_io *mc_io, uint16_t token, uint16_t if_id);

/**
 * dpsw_if_disable() - Disable Interface
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @if_id:		Interface Identifier
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_disable(struct fsl_mc_io *mc_io, uint16_t token, uint16_t if_id);

/**
 * dpsw_if_get_token - Obtains interface token
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @if_id:	Interface id
 * @if_token:	Interface token
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_get_token(struct fsl_mc_io *mc_io,
		      uint16_t token,
		      uint16_t if_id,
		      uint16_t *if_token);

/**
 * struct dpsw_queue_congestion_cfg  - Congestion queue configuration
 * @entrance_threshold: Entrance threshold
 * @exit_threshold: Exit threshold
 * @wr_addr: Address to write Congestion State Change Notification Message
 */
struct dpsw_queue_congestion_cfg {
	uint32_t entrance_threshold;
	uint32_t exit_threshold;
	uint64_t wr_addr;
};

/* PFC source trigger */

/* Trigger for PFC initiation is traffic class queue congestion */
#define DPSW_PFC_TRIG_QUEUE_CNG		0x01
/* Trigger for PFC initiation is buffer depletion */
#define DPSW_PFC_TRIG_BUFFER_DPL	0x02

/**
 * dpsw_if_tc_set_queue_congestion() - Configure thresholds for traffic class
 *					queue
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @if_id:		Interface Identifier
 * @tc_id:		Traffic class Identifier
 * @cfg:		Queue congestion configuration
 *
 * Configure thresholds for traffic class queue
 * congestion state and to establish Congestion State Change
 * Notification Message (CSCNM) from Congestion Point (CP) to GPP
 * trusted software This configuration is used to trigger PFC
 * request or congestion notification if enabled
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_tc_set_queue_congestion(struct fsl_mc_io *mc_io,
				    uint16_t token,
				    uint16_t if_id,
				    uint8_t tc_id,
				    const struct dpsw_queue_congestion_cfg
					*cfg);

/**
 * struct dpsw_pfc_cfg - Structure representing Priority Flow Control (PFC)
 *				configuration
 * @receiver: Enable/Disable PFC receiver;
 *				PFC receiver is responsible for accepting
 *				PFC PAUSE messages and pausing transmission
 *				for indicated in message PFC quanta
 * @initiator: Enable/Disable PFC initiator;
 *				PFC initiator is responsible for sending
 *				PFC request message when congestion has been
 *				detected on specified TC queue
 * @initiator_trig: Bitmap defining Trigger source or sources
 *			for sending PFC request message out;
 *			DPSW_PFC_TRIG_QUEUE_CNG or DPSW_PFC_TRIG_BUFFER_DPL
 *			should be used
 * @pause_quanta: Pause Quanta to indicate in PFC request
 *				message the amount of quanta time to pause
 *
 */
struct dpsw_pfc_cfg {
	int receiver;
	int initiator;
	uint32_t initiator_trig;
	uint16_t pause_quanta;
};

/**
 * dpsw_if_tc_set_pfc() - Handles Priority Flow Control (PFC) configuration per
 *							Traffic Class (TC)
 * @mc_io:		Pointer to MC portal's I/O object
 * @token:		Token of DPSW object
 * @if_id:		Interface Identifier
 * @tc_id:		Traffic class Identifier
 * @cfg:		PFC configuration
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_tc_set_pfc(struct fsl_mc_io *mc_io,
		       uint16_t token,
		       uint16_t if_id,
		       uint8_t tc_id,
		       struct dpsw_pfc_cfg *cfg);

/**
 * struct dpsw_cn_cfg - Structure representing Congestion Notification (CN)
 *		configuration
 * @enable: Enable/disable Congestion State Change Notification
 *			Message (CSCNM) from Congestion Point (CP) to GPP
 *			trusted software
 */
struct dpsw_cn_cfg {
	int enable;
};

/**
 * dpsw_if_tc_set_cn() - Enable/disable Congestion State Change Notification
 * @mc_io		Pointer to MC portal's I/O object
 * @token		Token of DPSW object
 * @if_id		Interface Identifier
 * @tc_id		Traffic class Identifier
 * @cfg		Congestion notification description
 *
 * Enable/disable Congestion State Change Notification
 * Message (CSCNM) from Congestion Point (CP) to GPP trusted software
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_tc_set_cn(struct fsl_mc_io *mc_io,
		      uint16_t token,
		      uint16_t if_id,
		      uint8_t tc_id,
		      const struct dpsw_cn_cfg *cfg);

/**
 * struct dpsw_if_attr - Structure representing DPSW interface attributes
 * @num_tcs: Number of traffic classes
 * @rate: Transmit rate in bits per second
 * @options: Interface configuration options (bitmap)
 * @enabled: Indicates if interface is enabled
 * @accept_all_vlan: The device discards/accepts incoming frames
 *		for VLANs that do not include this interface
 * @admit_untagged: When set to 'DPSW_ADMIT_ONLY_VLAN_TAGGED', the device
 *		discards untagged frames or priority-tagged frames received on
 *		this interface;
 *		When set to 'DPSW_ADMIT_ALL', untagged frames or priority-
 *		tagged frames received on this interface are accepted
 */
struct dpsw_if_attr {
	uint8_t num_tcs;
	uint64_t rate;
	uint64_t options;
	int enabled;
	int accept_all_vlan;
	enum dpsw_accepted_frames admit_untagged;
};

/**
 * dpsw_if_get_attributes() - Function obtains attributes of interface
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @if_id:	Interface Identifier
 * @attr:	Returned interface attributes
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_get_attributes(struct fsl_mc_io *mc_io,
			   uint16_t token,
			   uint16_t if_id,
			   struct dpsw_if_attr *attr);

/**
 * enum dpsw_cipher_suite - Cipher Suite for MACSec
 * @DPSW_MACSEC_GCM_AES_128: 128 bit
 * @DPSW_MACSEC_GCM_AES_256: 256 bit
 */
enum dpsw_cipher_suite {
	DPSW_MACSEC_GCM_AES_128 = 0,
	DPSW_MACSEC_GCM_AES_256 = 1
};

/**
 * struct dpsw_macsec_cfg - MACSec Configuration
 * @enable: Enable MACSec
 * @sci: Secure Channel ID
 * @cipher_suite: Cipher Suite
 *
 */
struct dpsw_macsec_cfg {
	int enable;
	uint64_t sci;
	enum dpsw_cipher_suite cipher_suite;
};

/**
 * dpsw_if_set_macsec() - Set MACSec configuration for physical port.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @if_id:	Interface Identifier
 * @cfg:	MACSec configuration
 *
 * Only point to point MACSec is supported
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_macsec(struct fsl_mc_io *mc_io,
		       uint16_t token,
		       uint16_t if_id,
		       const struct dpsw_macsec_cfg *cfg);

/**
 * dpsw_if_set_max_frame_length() - Set Maximum Receive frame length.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @if_id:	Interface Identifier
 * @frame_length: Maximum Frame Length
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_max_frame_length(struct fsl_mc_io *mc_io,
				 uint16_t token,
				 uint16_t if_id,
				 uint16_t frame_length);

/**
 * dpsw_if_get_max_frame_length() - Get Maximum Receive frame length.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @if_id:	Interface Identifier
 * @frame_length: Returned maximum Frame Length
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_get_max_frame_length(struct fsl_mc_io *mc_io,
				 uint16_t token,
				 uint16_t if_id,
				 uint16_t *frame_length);

/**
 * struct dpsw_vlan_cfg - VLAN Configuration
 * @fdb_id: Forwarding Data Base
 */
struct dpsw_vlan_cfg {
	uint16_t fdb_id;
};

/**
 * dpsw_vlan_add() - Adding new VLAN to DPSW.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @vlan_id:	VLAN Identifier
 * @cfg:	VLAN configuration
 *
 * Only VLAN ID and FDB ID are required parameters here.
 * 12 bit VLAN ID is defined in IEEE802.1Q.
 * Adding a duplicate VLAN ID is not allowed.
 * FDB ID can be shared across multiple VLANs. Shared learning
 * is obtained by calling dpsw_vlan_add for multiple VLAN IDs
 * with same fdb_id
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_add(struct fsl_mc_io *mc_io,
		  uint16_t token,
		  uint16_t vlan_id,
		  const struct dpsw_vlan_cfg *cfg);

/**
 * struct dpsw_vlan_if_cfg - Set of VLAN Interfaces
 * @num_ifs: The number of interfaces that are assigned to the egress
 *		list for this VLAN
 * @if_id: The set of interfaces that are
 *		assigned to the egress list for this VLAN
 */
struct dpsw_vlan_if_cfg {
	uint16_t num_ifs;
	uint16_t if_id[DPSW_MAX_IF];
};

/**
 * dpsw_vlan_add_if() - Adding a set of interfaces to an existing VLAN.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @vlan_id:	VLAN Identifier
 * @cfg:	Set of interfaces to add
 *
 * It adds only interfaces not belonging to this VLAN yet,
 * otherwise an error is generated and an entire command is
 * ignored. This function can be called numerous times always
 * providing required interfaces delta.
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_add_if(struct fsl_mc_io *mc_io,
		     uint16_t token,
		     uint16_t vlan_id,
		     const struct dpsw_vlan_if_cfg *cfg);

/**
 * dpsw_vlan_add_if_untagged() - Defining a set of interfaces that should be
 *				transmitted as untagged.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @vlan_id:	VLAN Identifier
 * @cfg:	set of interfaces that should be transmitted as untagged
 *
 * These interfaces should already belong to this VLAN.
 * By default all interfaces are transmitted as tagged.
 * Providing un-existing interface or untagged interface that is
 * configured untagged already generates an error and the entire
 * command is ignored.
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_add_if_untagged(struct fsl_mc_io *mc_io,
			      uint16_t token,
			      uint16_t vlan_id,
			      const struct dpsw_vlan_if_cfg *cfg);

/**
 * dpsw_vlan_add_if_flooding() - Define a set of interfaces that should be
 *			included in flooding when frame with unknown destination
 *			unicast MAC arrived.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @vlan_id:	VLAN Identifier
 * @cfg:	Set of interfaces that should be used for flooding
 *
 * These interfaces should belong to this VLAN. By default all
 * interfaces are included into flooding list. Providing
 * un-existing interface or an interface that already in the
 * flooding list generates an error and the entire command is
 * ignored.
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_add_if_flooding(struct fsl_mc_io *mc_io,
			      uint16_t token,
			      uint16_t vlan_id,
			      const struct dpsw_vlan_if_cfg *cfg);

/**
 * dpsw_vlan_remove_if() - Remove interfaces from an existing VLAN.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @vlan_id:	VLAN Identifier
 * @cfg:	Set of interfaces that should be removed
 *
 * Interfaces must belong to this VLAN, otherwise an error
 * is returned and an the command is ignored
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_remove_if(struct fsl_mc_io *mc_io,
			uint16_t token,
			uint16_t vlan_id,
			const struct dpsw_vlan_if_cfg *cfg);

/**
 * dpsw_vlan_remove_if_untagged() - Define a set of interfaces that should be
 *		converted from transmitted as untagged to transmit as tagged.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @vlan_id:	VLAN Identifier
 * @cfg:	set of interfaces that should be removed
 *
 * Interfaces provided by API have to belong to this VLAN and
 * configured untagged, otherwise an error is returned and the
 * command is ignored
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_remove_if_untagged(struct fsl_mc_io *mc_io,
				 uint16_t token,
				 uint16_t vlan_id,
				 const struct dpsw_vlan_if_cfg *cfg);

/**
 * dpsw_vlan_remove_if_flooding() - Define a set of interfaces that should be
 *			removed from the flooding list.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @vlan_id:	VLAN Identifier
 * @cfg:	set of interfaces used for flooding
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_remove_if_flooding(struct fsl_mc_io *mc_io,
				 uint16_t token,
				 uint16_t vlan_id,
				 const struct dpsw_vlan_if_cfg *cfg);

/**
 * dpsw_vlan_remove() - Remove an entire VLAN
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @vlan_id:	VLAN Identifier
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_remove(struct fsl_mc_io *mc_io, uint16_t token, uint16_t vlan_id);

/**
 * struct dpsw_vlan_attr - VLAN attributes
 * @fdb_id: Associated FDB ID
 * @num_ifs: Number of interfaces
 * @num_untagged_ifs: Number of untagged interfaces
 * @num_flooding_ifs: Number of flooding interfaces
 */
struct dpsw_vlan_attr {
	uint16_t fdb_id;
	uint16_t num_ifs;
	uint16_t num_untagged_ifs;
	uint16_t num_flooding_ifs;
};

/**
 * dpsw_vlan_get_attributes() - Get VLAN attributes
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @vlan_id:	VLAN Identifier
 * @attr:	Returned DPSW attributes
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_get_attributes(struct fsl_mc_io *mc_io,
			     uint16_t token,
			     uint16_t vlan_id,
			     struct dpsw_vlan_attr *attr);

/**
 * dpsw_vlan_get_if() - Get interfaces belong to this VLAN
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @vlan_id:	VLAN Identifier
 * @cfg:	Returned set of interfaces belong to this VLAN
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_get_if(struct fsl_mc_io *mc_io,
		     uint16_t token,
		     uint16_t vlan_id,
		     struct dpsw_vlan_if_cfg *cfg);

/**
 * dpsw_vlan_get_if_flooding() - Get interfaces used in flooding for this VLAN
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @vlan_id:	VLAN Identifier
 * @cfg:	Returned set of flooding interfaces
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_get_if_flooding(struct fsl_mc_io *mc_io,
			      uint16_t token,
			      uint16_t vlan_id,
			      struct dpsw_vlan_if_cfg *cfg);

/**
 * dpsw_vlan_get_if_untagged() - Get interfaces that should be transmitted as
 *				untagged
 * @mc_io	Pointer to MC portal's I/O object
 * @token	Token of DPSW object
 * @vlan_id	VLAN Identifier
 * @cfg:	Returned set of untagged interfaces
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_get_if_untagged(struct fsl_mc_io *mc_io,
			      uint16_t token,
			      uint16_t vlan_id,
			      struct dpsw_vlan_if_cfg *cfg);

/**
 * struct dpsw_fdb_cfg  - FDB Configuration
 * @num_fdb_entries: Number of FDB entries
 * @fdb_aging_time: Aging time in seconds
 */
struct dpsw_fdb_cfg {
	uint16_t num_fdb_entries;
	uint16_t fdb_aging_time;
};

/**
 * dpsw_fdb_add() - Add FDB to switch and Returns handle to FDB table for
 *		the reference
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @fdb_id:	Returned Forwarding Database Identifier
 * @cfg:	FDB Configuration
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_add(struct fsl_mc_io *mc_io,
		 uint16_t token,
		 uint16_t *fdb_id,
		 const struct dpsw_fdb_cfg *cfg);

/**
 * dpsw_fdb_remove() - Remove FDB from switch
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @fdb_id:	Forwarding Database Identifier
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_remove(struct fsl_mc_io *mc_io, uint16_t token, uint16_t fdb_id);

/**
 * enum dpsw_fdb_entry_type - FDB Entry type - Static/Dynamic
 * @DPSW_FDB_ENTRY_STATIC: Static entry
 * @DPSW_FDB_ENTRY_DINAMIC: Dynamic entry
 */
enum dpsw_fdb_entry_type {
	DPSW_FDB_ENTRY_STATIC = 0,
	DPSW_FDB_ENTRY_DINAMIC = 1
};

/**
 * struct dpsw_fdb_unicast_cfg - Unicast entry configuration
 * @type: Select static or dynamic entry
 * @mac_addr: MAC address
 * @if_egress: Egress interface ID
 */
struct dpsw_fdb_unicast_cfg {
	enum dpsw_fdb_entry_type type;
	uint8_t mac_addr[6];
	uint16_t if_egress;
};

/**
 * dpsw_fdb_add_unicast() - Function adds an unicast entry into MAC lookup table
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @fdb_id:	Forwarding Database Identifier
 * @cfg:	Unicast entry configuration
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_add_unicast(struct fsl_mc_io *mc_io,
			 uint16_t token,
			 uint16_t fdb_id,
			 const struct dpsw_fdb_unicast_cfg *cfg);

/**
 * dpsw_fdb_get_unicast() - Get unicast entry from MAC lookup table by
 *		unicast Ethernet address
 * @mc_io	Pointer to MC portal's I/O object
 * @token	Token of DPSW object
 * @fdb_id	Forwarding Database Identifier
 * @cfg		Returned unicast entry configuration
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_get_unicast(struct fsl_mc_io *mc_io,
			 uint16_t token,
			 uint16_t fdb_id,
			 struct dpsw_fdb_unicast_cfg *cfg);

/**
 * dpsw_fdb_remove_unicast() - removes an entry from MAC lookup table
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @fdb_id:	Forwarding Database Identifier
 * @cfg:	Unicast entry configuration
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_remove_unicast(struct fsl_mc_io *mc_io,
			    uint16_t token,
			    uint16_t fdb_id,
			    const struct dpsw_fdb_unicast_cfg *cfg);

/**
 * struct dpsw_fdb_multicast_cfg - Multi-cast entry configuration
 * @type: Select static or dynamic entry
 * @mac_addr: MAC address
 * @num_ifs: Number of external and internal interfaces
 * @if_id: Egress interface IDs
 */
struct dpsw_fdb_multicast_cfg {
	enum dpsw_fdb_entry_type type;
	uint8_t mac_addr[6];
	uint16_t num_ifs;
	uint16_t if_id[DPSW_MAX_IF];
};

/**
 * dpsw_fdb_add_multicast() - Add a set of egress interfaces to multi-cast group
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @fdb_id:	Forwarding Database Identifier
 * @cfg:	Multicast entry configuration
 *
 * If group doesn't exist, it will be created.
 * It adds only interfaces not belonging to this multicast group
 * yet, otherwise error will be generated and the command is
 * ignored.
 * This function may be called numerous times always providing
 * required interfaces delta.
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_add_multicast(struct fsl_mc_io *mc_io,
			   uint16_t token,
			   uint16_t fdb_id,
			   const struct dpsw_fdb_multicast_cfg *cfg);

/**
 * dpsw_fdb_get_multicast() - Reading multi-cast group by multi-cast Ethernet
 *				address.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @fdb_id:	Forwarding Database Identifier
 * @cfg:	Returned multicast entry configuration
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_get_multicast(struct fsl_mc_io *mc_io,
			   uint16_t token,
			   uint16_t fdb_id,
			   struct dpsw_fdb_multicast_cfg *cfg);

/**
 * dpsw_fdb_remove_multicast() - Removing interfaces from an existing multicast
 *				group.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @fdb_id:	Forwarding Database Identifier
 * @cfg:	Multicast entry configuration
 *
 * Interfaces provided by this API have to exist in the group,
 * otherwise an error will be returned and an entire command
 * ignored. If there is no interface left in the group,
 * an entire group is deleted
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_remove_multicast(struct fsl_mc_io *mc_io,
			      uint16_t token,
			      uint16_t fdb_id,
			      const struct dpsw_fdb_multicast_cfg *cfg);

/**
 * enum dpsw_fdb_learning_mode - Auto-learning modes
 * @DPSW_FDB_LEARNING_MODE_DIS: Disable Auto-learning
 * @DPSW_FDB_LEARNING_MODE_HW: Enable HW auto-Learning
 * @DPSW_FDB_LEARNING_MODE_NON_SECURE: Enable None secure learning by CPU
 * @DPSW_FDB_LEARNING_MODE_SECURE: Enable secure learning by CPU
 *
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
	DPSW_FDB_LEARNING_MODE_HW = 1,
	DPSW_FDB_LEARNING_MODE_NON_SECURE = 2,
	DPSW_FDB_LEARNING_MODE_SECURE = 3
};

/**
 * dpsw_fdb_set_learning_mode() - Define FDB learning mode
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @fdb_id:	Forwarding Database Identifier
 * @mode:	learning mode
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_set_learning_mode(struct fsl_mc_io *mc_io,
			       uint16_t token,
			       uint16_t fdb_id,
			       enum dpsw_fdb_learning_mode mode);

/**
 * struct dpsw_fdb_attr - FDB Attributes
 * @max_fdb_entries: Number of FDB entries
 * @fdb_aging_time: Aging time in seconds
 * @learning_mode: Learning mode
 * @num_fdb_mc_groups: Current number of multicast groups
 * @max_fdb_mc_groups: Maximum number of multicast groups
 */
struct dpsw_fdb_attr {
	uint16_t max_fdb_entries;
	uint16_t fdb_aging_time;
	enum dpsw_fdb_learning_mode learning_mode;
	uint16_t num_fdb_mc_groups;
	uint16_t max_fdb_mc_groups;
};

/**
 * dpsw_fdb_get_attributes() - Get FDB attributes
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @fdb_id:	Forwarding Database Identifier
 * @attr:	Returned FDB attributes
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_get_attributes(struct fsl_mc_io *mc_io,
			    uint16_t token,
			    uint16_t fdb_id,
			    struct dpsw_fdb_attr *attr);

/**
 * struct dpsw_acl_cfg - ACL Configuration
 * @max_entries: Number of FDB entries
 */
struct dpsw_acl_cfg {
	uint16_t	max_entries;
};

/**
 * struct dpsw_acl_fields - ACL fields.
 * @l2_dest_mac: Destination MAC address: BPDU, Multicast, Broadcast, Unicast,
 *			slow protocols, MVRP, STP
 * @l2_source_mac: Source MAC address
 * @l2_tpid: Layer 2 (Ethernet) protocol type, used to identify the following
 *		protocols: MPLS, PTP, PFC, ARP, Jumbo frames, LLDP, IEEE802.1ae,
 *		Q-in-Q, IPv4, IPv6, PPPoE
 * @l2_pcp_dei: indicate which protocol is encapsulated in the payload
 * @l2_vlan_id: layer 2 VLAN ID
 * @l2_ether_type: layer 2 Ethernet type
 * @l3_dscp: Layer 3 differentiated services code point
 * @l3_protocol: Tells the Network layer at the destination host, to which
 *		Protocol this packet belongs to. The following protocol are
 *		supported: ICMP, IGMP, IPv4 (encapsulation), TCP, IPv6
 *		(encapsulation), GRE, PTP
 * @l3_source_ip: Source IPv4 IP
 * @l3_dest_ip: Destination IPv4 IP
 * @l4_source_port: Source TCP/UDP Port
 * @l4_dest_port: Destination TCP/UDP Port
 */
struct dpsw_acl_fields {
	uint8_t         l2_dest_mac[6];
	uint8_t         l2_source_mac[6];
	uint16_t        l2_tpid;
	uint8_t         l2_pcp_dei;
	uint16_t        l2_vlan_id;
	uint16_t        l2_ether_type;
	uint8_t         l3_dscp;
	uint8_t         l3_protocol;
	uint32_t        l3_source_ip;
	uint32_t        l3_dest_ip;
	uint16_t        l4_source_port;
	uint16_t        l4_dest_port;
};

/**
 * struct dpsw_acl_key - ACL key
 * @match: Match fields
 * @mask: Mask: b'1 - valid, b'0 don't care
 */
struct dpsw_acl_key {
	struct dpsw_acl_fields  match;
	struct dpsw_acl_fields  mask;
};

/**
 * enum dpsw_acl_action
 * @DPSW_ACL_ACTION_DROP: Drop frame
 * @DPSW_ACL_ACTION_REDIRECT: Redirect to certain port
 * @DPSW_ACL_ACTION_ACCEPT: Accept frame
 */
enum dpsw_acl_action {
	DPSW_ACL_ACTION_DROP,            /*! Drop frame */
	DPSW_ACL_ACTION_REDIRECT,        /*! Redirect to certain port */
	DPSW_ACL_ACTION_ACCEPT           /*! Accept frame */
};

/**
 * struct dpsw_acl_result - ACL action
 * @action: Action should be taken when	ACL entry hit
 * @if_id:  Interface IDs to redirect frame. Valid only if redirect selected for
 *		 action
 */
struct dpsw_acl_result {
	enum dpsw_acl_action	action;
	uint16_t                if_id;
};

/**
 * struct dpsw_acl_entry_cfg - ACL entry
 * @key_iova: I/O virtual address of DMA-able memory filled with key after call
 *				to dpsw_acl_prepare_entry_cfg()
 * @result: Required action when entry hit occurs
 * @precedence: Precedence inside ACL 0 is lowest; This priority can not change
 *		during the lifetime of a Policy. It is user responsibility to
 *		space the priorities according to consequent rule additions.
 */
struct dpsw_acl_entry_cfg {
	uint64_t			key_iova;
	struct dpsw_acl_result  result;
	int                     precedence;
};
/**
 * dpsw_acl_add() - Adds ACL to L2 switch.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @acl_id	Returned ACL ID, for the future reference
 * @cfg		ACL configuration
 *
 * Create Access Control List. Multiple ACLs can be created and
 * co-exist in L2 switch
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_acl_add(struct fsl_mc_io *mc_io,
		 uint16_t token,
		 uint16_t *acl_id,
		 const struct dpsw_acl_cfg  *cfg);

/**
 * dpsw_acl_remove() - Removes ACL from L2 switch.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @acl_id:	ACL ID
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_acl_remove(struct fsl_mc_io *mc_io,
		    uint16_t token,
		    uint16_t acl_id);

/**
 * dpsw_acl_prepare_entry_cfg() - Set an entry to ACL.
 * @key:	key
 * @entry_cfg_buf: Zeroed 256 bytes of memory before mapping it to DMA
 *
 * This function has to be called before adding or removing acl_entry
 *
 */
void dpsw_acl_prepare_entry_cfg(const struct dpsw_acl_key *key,
				uint8_t *entry_cfg_buf);

/**
 * dpsw_acl_add_entry() - Adds an entry to ACL.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @acl_id:	ACL ID
 * @cfg:	entry configuration
 *
 * warning: This function has to be called after dpsw_acl_set_entry_cfg()
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_acl_add_entry(struct fsl_mc_io *mc_io,
		       uint16_t token,
		       uint16_t acl_id,
		       const struct dpsw_acl_entry_cfg *cfg);

/**
 * dpsw_acl_remove_entry() - Removes an entry from ACL.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @acl_id:	ACL ID
 * @cfg:	entry configuration
 *
 * warning: This function has to be called after dpsw_acl_set_entry_cfg()
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_acl_remove_entry(struct fsl_mc_io *mc_io,
			  uint16_t token,
			  uint16_t acl_id,
			  const struct dpsw_acl_entry_cfg *cfg);

/**
 * struct dpsw_acl_if_cfg - List of interfaces to Associate with ACL
 * @num_ifs: Number of interfaces
 * @if_id: List of interfaces
 */
struct dpsw_acl_if_cfg {
	uint16_t	num_ifs;
	uint16_t	if_id[DPSW_MAX_IF];
};
/**
 * dpsw_acl_add_if() - Associate interface/interfaces with ACL.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @acl_id:	ACL ID
 * @cfg:	interfaces list
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_acl_add_if(struct fsl_mc_io *mc_io,
		    uint16_t token,
		    uint16_t acl_id,
		    const struct dpsw_acl_if_cfg	*cfg);

/**
 * dpsw_acl_remove_if() - De-associate interface/interfaces from ACL.
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPSW object
 * @acl_id:	ACL ID
 * @cfg:	interfaces list
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_acl_remove_if(struct fsl_mc_io *mc_io,
		       uint16_t token,
		       uint16_t acl_id,
		       const struct dpsw_acl_if_cfg	*cfg);
#endif /* __FSL_DPSW_H */

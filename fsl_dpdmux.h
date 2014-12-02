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
 *  @file    fsl_dpdmux.h
 *  @brief   Data Path DPDMUX API
 */

#ifndef __FSL_DPDMUX_H
#define __FSL_DPDMUX_H

#include "fsl_net.h"

struct fsl_mc_io;

/*!
 * @Group grp_dpdmux	Data Path Demux API
 *
 * @brief	Contains API for handling DPDMUX topology and functionality
 * @{
 */

/**
 * @brief	Open a control session for the specified object
 *
 *		This function can be used to open a control session for an
 *		already created object; an object may have been declared in
 *		the DPL or by calling the dpdmux_create() function.
 *		This function returns a unique authentication token,
 *		associated with the specific object ID and the specific MC
 *		portal; this token must be used in all subsequent commands for
 *		this specific object.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]	dpdmux_id	DPDMUX unique ID
 * @param[out]	token		Returned token; use in subsequent API calls
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_open(struct fsl_mc_io *mc_io, int dpdmux_id, uint16_t *token);

/**
 * @brief	Close the control session of the object
 *
 *		After this function is called, no further operations are
 *		allowed on the object without opening a new control session.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPDMUX object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_close(struct fsl_mc_io *mc_io, uint16_t token);

/*!
 * @name DPDMUX general options
 */
#define DPDMUX_OPT_REPLICATION_DIS	0x0000000000000001ULL
/*!< Disable replication (no multicast or broadcast) */
#define DPDMUX_OPT_BRIDGE_EN		0x0000000000000002ULL
/*!< Enable bridging between internal interfaces */
/* @} */

/**
 * @brief	DPDMUX manipulation operations
 */
enum dpdmux_manip {
	DPDMUX_MANIP_NONE = 0x0,
	/*!< No manipulation on frames */
	DPDMUX_MANIP_ADD_REMOVE_S_VLAN = 0x1
	/*!< Add S-VLAN on egress, remove it on ingress */
};

/**
 * @brief	DPDMUX method options
 */
enum dpdmux_method {
	DPDMUX_METHOD_NONE = 0x0,
	/*!< no DPDMUX method */
	DPDMUX_METHOD_C_VLAN_MAC = 0x1,
	/*!< DPDMUX based on C-VLAN and MAC address */
	DPDMUX_METHOD_MAC = 0x2,
	/*!< DPDMUX based on MAC address */
	DPDMUX_METHOD_C_VLAN = 0x3,
	/*!< DPDMUX based on C-VLAN */
	DPDMUX_METHOD_S_VLAN = 0x4
	/*!< DPDMUX based on S-VLAN */
};

/**
 * @brief	DPDMUX configuration parameters
 */
struct dpdmux_cfg {
	enum dpdmux_method method;
	/*!< Defines the operation method for the DPDMUX address table */
	enum dpdmux_manip manip;
	/*!< Required manipulation operation */
	int control_if;
	/*!< The initial control interface */
	uint16_t num_ifs;
	/*!< Number of interfaces (excluding the uplink interface) */
	struct {
		uint64_t options;
		/*!< DPDMUX options - combination of 'DPDMUX_OPT_<X>' flags */
		uint16_t max_dmat_entries;
		/*!< Maximum entries in DPDMUX address table
		 * 0 - indicates default: 64 entries per interface.
		 */
		uint16_t max_mc_groups;
		/*!< Number of multicast groups in DPDMUX table
		 * 0 - indicates default: 32 multicast groups
		 */
	} adv;
	/*!< Advanced parameters; default is all zeros;
	 * use this structure to change default settings
	 */
};

/**
 * @brief	Create the DPDMUX object, allocate required resources and
 *		perform required initialization.
 *
 *		The object can be created either by declaring it in the
 *		DPL file, or by calling this function.
 *
 *		This function returns a unique authentication token,
 *		associated with the specific object ID and the specific MC
 *		portal; this token must be used in all subsequent calls to
 *		this specific object. For objects that are created using the
 *		DPL file, call dpdmux_open() function to get an authentication
 *		token first.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]	cfg	Configuration structure
 * @param[out]	token	Returned token; use in subsequent API calls
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_create(struct fsl_mc_io *mc_io, const struct dpdmux_cfg *cfg,
		  uint16_t *token);

/**
 * @brief	Destroy the DPDMUX object and release all its resources.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPDMUX object
 *
 * @returns	'0' on Success; error code otherwise.
 */
int dpdmux_destroy(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Enable DPDMUX functionality
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPDMUX object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_enable(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Disable DPDMUX functionality
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPDMUX object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_disable(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Check if the DPDMUX is enabled.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPDMUX object
 * @param[out]  en	Returns '1' if object is enabled; '0' otherwise
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_is_enabled(struct fsl_mc_io *mc_io, uint16_t token, int *en);

/**
 * @brief	Reset the DPDMUX, returns the object to initial state.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPDMUX object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_reset(struct fsl_mc_io *mc_io, uint16_t token);


/**
 * @brief	Set IRQ information for the DPDMUX to trigger an interrupt.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPDMUX object
 * @param[in]	irq_index	Identifies the interrupt index to configure
 * @param[in]	irq_addr	Address that must be written to
 *				signal a message-based interrupt
 * @param[in]	irq_val		Value to write into irq_addr address
 * @param[out]	user_irq_id	A user defined number associated with this IRQ
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_set_irq(struct fsl_mc_io	*mc_io,
		   uint16_t		token,
		   uint8_t		irq_index,
		   uint64_t		irq_addr,
		   uint32_t		irq_val,
		   int			user_irq_id);

/**
 * @brief	Get IRQ information from the DPDMUX.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPDMUX object
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
int dpdmux_get_irq(struct fsl_mc_io	*mc_io,
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
 * @param[in]   token		Token of DPDMUX object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[in]	en		Interrupt state - enable = 1, disable = 0
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_set_irq_enable(struct fsl_mc_io	*mc_io,
			  uint16_t		token,
			  uint8_t		irq_index,
			  uint8_t		en);

/**
 * @brief	Get overall interrupt state.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPDMUX object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	en		Interrupt state - enable = 1, disable = 0
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_get_irq_enable(struct fsl_mc_io	*mc_io,
			  uint16_t		token,
			  uint8_t		irq_index,
			  uint8_t		*en);

/**
 * @brief	Set interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPDMUX object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[in]	mask		event mask to trigger interrupt;
 *				each bit:
 *					0 = ignore event
 *					1 = consider event for asserting irq
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_set_irq_mask(struct fsl_mc_io	*mc_io,
			uint16_t		token,
			uint8_t			irq_index,
			uint32_t		mask);

/**
 * @brief	Get interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPDMUX object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	mask		event mask to trigger interrupt
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_get_irq_mask(struct fsl_mc_io	*mc_io,
			uint16_t		token,
			uint8_t			irq_index,
			uint32_t		*mask);

/**
 * @brief	Get the current status of any pending interrupts.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPDMUX object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	status		interrupts status - one bit per cause:
 *					0 = no interrupt pending
 *					1 = interrupt pending
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_get_irq_status(struct fsl_mc_io	*mc_io,
			  uint16_t		token,
			  uint8_t		irq_index,
			  uint32_t		*status);

/**
 * @brief	Clear a pending interrupt's status
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPDMUX object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	status		bits to clear (W1C) - one bit per cause:
 *					0 = don't change
 *					1 = clear status bit
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_clear_irq_status(struct fsl_mc_io	*mc_io,
			    uint16_t		token,
			    uint8_t		irq_index,
			    uint32_t		status);

/**
 * @brief	Structure representing DPDMUX attributes
 */
struct dpdmux_attr {
	int id;
	/*!< DPDMUX object ID */
	struct {
		uint16_t major;
		/*!< DPDMUX major version */
		uint16_t minor;
		/*!< DPDMUX minor version */
	} version;
	/*!< DPDMUX version */
	uint64_t options;
	/*!< Configuration options (bitmap) */
	enum dpdmux_method method;
	/*!< DPDMUX address table method */
	enum dpdmux_manip manip;
	/*!< DPDMUX manipulation type */
	uint16_t num_ifs;
	/*!< Number of interfaces (excluding the uplink interface) */
	uint16_t mem_size;
	/*!< DPDMUX frame storage memory size */
	int control_if;
	/*!< Control interface ID */
};

/**
 * @brief	Retrieve DPDMUX attributes
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPDMUX object
 * @param[in]	attr		Object's attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_get_attributes(struct fsl_mc_io	*mc_io,
			  uint16_t		token,
			  struct dpdmux_attr	*attr);

/**
 *
 * @brief	Set the maximum frame length in DPDMUX
 *
 * @param[in]	mc_io			Pointer to MC portal's I/O object
 * @param[in]   token			Token of DPDMUX object
 * @param[in]	max_frame_length	The required maximum frame length
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_ul_set_max_frame_length(struct fsl_mc_io	*mc_io,
				   uint16_t		token,
				   uint16_t		max_frame_length);

/**
 *
 * @brief	Set the interface to be the default interface
 *
 *		Default interface is selected when the frame does not match
 *		any entry in the Demux address table. This function can also
 *		clear the default interface selection by passing 'set = 0'.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPDMUX object
 * @param[in]	if_id		Interface ID (0 for uplink, or 1-num_ifs);
 *				Ignored if 'no_default_if = 1'
 * @param[in]	no_default_if	Set to '1' to clear default interface setting -
 *				consequently, frames with no match in the
 *				Demux address table are dropped;
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_set_default_if(struct fsl_mc_io	*mc_io,
			  uint16_t		token,
			  uint16_t		if_id,
			  int			no_default_if);

/**
 *
 * @brief	Get the default interface
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPDMUX object
 * @param[in]	if_id		Returns default interface ID (0 for uplink,
 *				or 1-num_ifs);
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_get_default_if(struct fsl_mc_io	*mc_io,
			  uint16_t		token,
			  uint16_t		*if_id);

/**
 * @brief	Counter types
 */
enum dpdmux_counter_type {
	DPDMUX_CNT_ING_FRAME,
	/*!< Counts ingress frames */
	DPDMUX_CNT_ING_BYTE,
	/*!< Counts ingress bytes */
	DPDMUX_CNT_ING_FLTR_FRAME,
	/*!< Counts filtered ingress frames */
	DPDMUX_CNT_ING_FRAME_DISCARD,
	/*!< Counts discarded ingress frames */
	DPDMUX_CNT_ING_MCAST_FRAME,
	/*!< Counts ingress multicast frames */
	DPDMUX_CNT_ING_MCAST_BYTE,
	/*!< Counts ingress multicast bytes */
	DPDMUX_CNT_ING_BCAST_FRAME,
	/*!< Counts ingress broadcast frames */
	DPDMUX_CNT_ING_BCAST_BYTES,
	/*!< Counts ingress broadcast bytes */
	DPDMUX_CNT_EGR_FRAME,
	/*!< Counts egress frames */
	DPDMUX_CNT_EGR_BYTE,
	/*!< Counts egress bytes */
	DPDMUX_CNT_EGR_FRAME_DISCARD
	/*!< Counts discarded egress frames */
};

/**
 * @brief	Obtain specific uplink interface counter
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPDMUX object
 * @Param[in]   type		Counter type
 * @Param[out]  counter		Current counter value
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_ul_get_counter(struct fsl_mc_io		*mc_io,
			  uint16_t			token,
			  enum dpdmux_counter_type	type,
			  uint64_t			*counter);

/**
 * @brief	Set uplink interface counter to specified value
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPDMUX object
 * @Param[in]   type		Counter type to set
 * @Param[in]   counter		New counter value
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_ul_set_counter(struct fsl_mc_io		*mc_io,
			  uint16_t			token,
			  enum dpdmux_counter_type	type,
			  uint64_t			counter);

/**
 * @brief	DPDMUX frame types
 */
enum dpdmux_accepted_frames_type {
	DPDMUX_ADMIT_ALL = 0,
	/*!< The device accepts VLAN tagged, untagged and
	 * priority-tagged frames
	 */
	DPDMUX_ADMIT_ONLY_VLAN_TAGGED,
	/*!< The device discards untagged frames or priority-tagged frames
	 * that are received on this interface
	 */
	DPDMUX_ADMIT_ONLY_UNTAGGED
	/*!< Untagged frames or priority-tagged frames received
	 * on this interface are accepted
	 */
};

/**
 * @brief	DPDMUX action for un-accepted frames
 */
enum dpdmux_action {
	DPDMUX_ACTION_DROP = 0,
	/*!< Drop un-accepted frames */
	DPDMUX_ACTION_REDIRECT_TO_CTRL = 1
	/*!< Redirect un-accepted frames to the control interface */
};

/**
 * @brief	Structure representing frame types configuration
 */
struct dpdmux_accepted_frames {
	enum dpdmux_accepted_frames_type type;
	/*!< Defines ingress accepted frames */
	enum dpdmux_action unaccept_act;
	/*!< Defines action on frames not accepted */
};

/**
 * @brief	Set the accepted frame types
 *
 *	if 'DPDMUX_ADMIT_ONLY_VLAN_TAGGED' is set - untagged frames or
 *	priority-tagged frames are discarded.
 *	if 'DPDMUX_ADMIT_ONLY_UNTAGGED' is set - untagged frames or
 *	priority-tagged frames are accepted.
 *	if 'DPDMUX_ADMIT_ALL' is set (default mode) - all VLAN tagged,
 *	untagged and priority-tagged frame are accepted;
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPDMUX object
 * @param[in]	if_id		Interface ID (0 for uplink, or 1-num_ifs);
 * @param[in]	cfg		Frame types configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_if_set_accepted_frames(struct fsl_mc_io			*mc_io,
				  uint16_t				token,
				  uint16_t				if_id,
				  const struct dpdmux_accepted_frames	*cfg);

/**
 * @brief	Structure representing frame types configuration
 */
struct dpdmux_if_attr {
	uint64_t rate;
	/*!< Configured interface rate (in bits per second) */
	int enabled;
	/*! Indicates if interface is enabled */
	enum dpdmux_accepted_frames_type accept_frame_type;
	/*!< Indicates type of accepted frames for the interface */
	int is_default;
	/*!< Indicates if configured as default interface */
};

/**
 * @brief	Obtain DPDMUX interface attributes
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPDMUX object
 * @param[in]	if_id		Interface ID (0 for uplink, or 1-num_ifs);
 * @param[out]	attr		Interface attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_if_get_attributes(struct fsl_mc_io		*mc_io,
			     uint16_t			token,
			     uint16_t			if_id,
			     struct dpdmux_if_attr	*attr);

/**
 * @brief	Structure representing L2 rule
 */
struct dpdmux_l2_rule {
	uint8_t mac_addr[6];
	/*!<  MAC address */
	uint16_t vlan_id;
	/*!< VLAN ID */
};

/**
 * @brief	Remove L2 rule from DPDMUX table
 *
 *		Function removes a L2 rule from DPDMUX table
 *		or adds an interface to an existing multicast address
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPDMUX object
 * @param[in]	if_id		Destination interface ID
 * @param[in]	rule		L2 rule
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_if_remove_l2_rule(struct fsl_mc_io			*mc_io,
			     uint16_t				token,
			     uint16_t				if_id,
			     const struct dpdmux_l2_rule	*rule);

/**
 * @brief	Add L2 rule into DPDMUX table
 *
 *		Function adds a L2 rule into DPDMUX table
 *		or adds an interface to an existing multicast address
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPDMUX object
 * @param[in]	if_id		Destination interface ID
 * @param[in]	rule		L2 rule
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpdmux_if_add_l2_rule(struct fsl_mc_io		*mc_io,
			  uint16_t			token,
			  uint16_t			if_id,
			  const struct dpdmux_l2_rule	*rule);

/*! @} */

#endif /* __FSL_DPDMUX_H */

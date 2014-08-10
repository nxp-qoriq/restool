/* Copyright 2013-2014 Freescale Semiconductor Inc.
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
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor "AS IS" AND ANY
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
/*!< Max number of traffic classes */
#define DPNI_MAX_UNICAST_FILTERS		16
/*!< Maximum number of unicast filters */
#define DPNI_MAX_MULTICAST_FILTERS		64
/*!< Maximum number of multicast filters */
#define DPNI_MAX_VLAN_FILTERS			16
/*!< Maximum number of VLAN filters */
#define DPNI_MAX_QOS_ENTRIES			64
/*!< Maximum number of QoS entries */
#define DPNI_MAX_DPBP				8
/*!< Maximum number of bm-pools */

#define DPNI_ALL_TCS				(uint8_t)(-1)
/*!< All traffic classes considered */
#define DPNI_ALL_TC_FLOWS			(uint16_t)(-1)
/*!< All flows within traffic classes considered */
#define DPNI_NEW_FLOW_ID			(uint16_t)(-1)
/*!< Generate new flow id */
#define DPNI_VFQID_NOT_VALID			(-1)
/*!< Invalid virtual FQID  */
/* @} */

/**
 * @brief   structure representing FLC parameters
 */
struct dpni_flc_cfg {
	int stash_en; /*!< either stash enabled or not */
	union {
		struct {
			uint8_t frame_annotation_size;
			/*!< Size of Frame  Annotation to be stashed */
			uint8_t frame_data_size;
			/*!< Size of Frame Data to be stashed. */
			uint8_t flow_context_size;
			/*!< Size of flow context to be stashed. */
			uint64_t flow_context_addr;
		/*!< 64/49 bit memory address containing the
		 flow context information to be stashed;
		 Must be cacheline-aligned */
		} stashing;
		uint64_t odp; /*!< value to be written for order-definition */
	} u;
};

/**
 *
 * @brief	Open object handle
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_id - DPNI unique ID
 * @param[out]	dpni_handle - Pointer to handle of the DPNI object
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_open(struct fsl_mc_io *mc_io, int dpni_id, uint16_t *dpni_handle);

/**
 *
 * @brief	Closes the object handle, no further operations on the object
 *		are allowed
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_close(struct fsl_mc_io *mc_io, uint16_t dpni_handle);

/*!
 * @name DPNI configuration options
 *
 */
#define DPNI_OPT_ALLOW_DIST_KEY_PER_TC		0x00000001
/*!< allow different dist-key per TC */
#define DPNI_OPT_TX_CONF_DISABLED		0x00000002
/*!< No Tx-confirmation at all */
#define DPNI_OPT_PRIVATE_TX_CONF_ERR_DISABLED	0x00000004
/*!< private tx-confirmation/err disable */
#define DPNI_OPT_QOS				0x00000008
/*!< QoS support */
#define DPNI_OPT_DIST_HASH			0x00000010
/*!< hash based distribution support */
#define DPNI_OPT_DIST_FS			0x00000020
/*!< flow-steering based distribution support */
#define DPNI_OPT_POLICING			0x00000040
/*!< policing support */
#define DPNI_OPT_UNICAST_FILTER			0x00000080
/*!< unicast filtering support */
#define DPNI_OPT_MULTICAST_FILTER		0x00000100
/*!< multicast filtering support */
#define DPNI_OPT_VLAN_FILTER			0x00000200
/*!< vlan filtering support */
#define DPNI_OPT_MACSEC				0x00000400
/*!< MACSEC support */
#define DPNI_OPT_IPR				0x00000800
/*!< IP-reassembly support */
#define DPNI_OPT_IPF				0x00001000
/*!< IP-fragmentation support */
#define DPNI_OPT_RSC				0x00002000
/*!< RSC support */
#define DPNI_OPT_GSO				0x00004000
/*!< GSO support */
#define DPNI_OPT_IPSEC				0x00008000
/*!< IPSec transport support */
#define DPNI_OPT_VLAN_MANIPULATION		0x00010000
/*!< vlan manipulation support */
/* @} */

/**
 * @brief	DPNI types
 *
 */
enum dpni_type {
	DPNI_TYPE_NI = 1, /*!< DPNI of type NI */
	DPNI_TYPE_NIC
/*!< DPNI of type NIC */
};

/**
 * @brief	Structure representing DPNI configuration
 */
struct dpni_cfg {
	enum dpni_type type; /*!< DPNI Type */
	uint8_t mac_addr[6]; /*!< Primary mac address */
	enum net_prot start_hdr;
	/*!< Valid only in the case 'type = DPNI_TYPE_NI';
	 * In NIC case will be set to NET_PROT_ETH */
	struct {
		uint64_t options;
		/*!< Mask of available options; use 'DPNI_OPT_XXX' */
		uint8_t max_senders;
		/*!< maximum number of different senders; will be used as the
		 * number of dedicated tx flows; '0' will be treated as '1' */
		uint8_t max_tcs;
		/*!< maximum number of traffic-classes;
		 will affect both Tx & Rx; '0' will e treated as '1' */
		uint16_t max_dist_per_tc[DPNI_MAX_TC];
		/*!< maximum distribution's size per Rx traffic-class;
		 represent the maximum DPIO objects that will be
		 referenced by this TC; '0' will e treated as '1' */
		uint8_t max_unicast_filters;
		/*!< maximum number of unicast filters; '0' will be treated
		 as 'DPNI_MAX_UNICAST_FILTERS' */
		uint8_t max_multicast_filters;
		/*!< maximum number of multicast filters; '0' will be treated
		 as 'DPNI_MAX_MULTICAST_FILTERS' */
		uint8_t max_vlan_filters;
		/*!< maximum number of vlan filters; '0' will be treated
		 as 'DPNI_MAX_VLAN_FILTERS' */
		uint8_t max_qos_entries;
		/*!< if 'max_tcs>1', declare the maximum entries for the
		 QoS table; '0' will be treated as
		 'DPNI_MAX_QOS_ENTRIES' */
		uint8_t max_qos_key_size;
		/*!< maximum key size for the QoS look-up; '0' will be treated
		 * as '24' which enough for IPv4 5-tuple */
		uint8_t max_dist_key_size;
	/*!< maximum key size for the distribution; '0' will be treated as
	 '24' which enough for IPv4 5-tuple */
	} adv; /*!< use this structure to change default settings */
};
/**
 * @brief	Open object handle, allocate resources and preliminary initialization -
 *		required before any operation on the object
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	cfg - Configuration structure
 * @param[out]	dpni_handle - Pointer to handle of the DPNI object
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Required before any operation on the object
 */
int dpni_create(struct fsl_mc_io *mc_io, const struct dpni_cfg *cfg,
		uint16_t *dpni_handle);

/**
 *
 * @brief	Free the DPNI object and all its resources.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 *
 * @returns	'0' on Success; error code otherwise.
 */
int dpni_destroy(struct fsl_mc_io *mc_io, uint16_t dpni_handle);

/*!
 * @name DPNI IRQ Index and Events
 */
#define DPNI_IRQ_INDEX				0
/*!< Irq index */
#define DPNI_IRQ_EVENT_LINK_CHANGED		0x00000001
/*!< irq event - Indicates that the link state changed */
/* @} */

/**
 * @brief	Sets IRQ information for the DPNI to trigger an interrupt.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]	dpni_handle	handle to the DPNI
 * @param[in]	irq_index	Identifies the interrupt index to configure.
 * @param[in]	irq_paddr	Physical IRQ address that must be written to
 *				signal a message-based interrupt
 * @param[in]	irq_val		Value to write into irq_paddr address
 * @param[in]	user_irq_id	A user defined number associated with this IRQ;
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_irq(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		 uint8_t irq_index,
		 uint64_t irq_paddr,
		 uint32_t irq_val,
		 int user_irq_id);

/**
 * @brief	Gets IRQ information from the DPNI.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]	dpni_handle	handle to the DPNI
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]  type		Interrupt type: 0 represents message interrupt
 *				type (both irq_paddr and irq_val are valid);
 * @param[out]	irq_paddr	Physical address that must be written in order
 *				to signal the message-based interrupt
 * @param[out]	irq_val		Value to write in order to signal the
 *				message-based interrupt
 * @param[out]	user_irq_id	A user defined number associated with this IRQ;
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_irq(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		 uint8_t irq_index,
		 int *type,
		 uint64_t *irq_paddr,
		 uint32_t *irq_val,
		 int *user_irq_id);

/**
 * @brief	Sets overall interrupt state.
 *
 * Allows GPP software to control when interrupts are generated.
 * Each interrupt can have up to 32 causes.  The enable/disable control's the
 * overall interrupt state. if the interrupt is disabled no causes will cause
 * an interrupt.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]	dpni_handle	handle to the DPNI
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[in]	enable_state	interrupt state - enable = 1, disable = 0.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_irq_enable(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			uint8_t irq_index,
			uint8_t enable_state);

/**
 * @brief	Gets overall interrupt state
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]	dpni_handle	handle to the DPNI
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	enable_state	interrupt state - enable = 1, disable = 0.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_irq_enable(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			uint8_t irq_index,
			uint8_t *enable_state);

/**
 * @brief	Sets interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]	dpni_handle	handle to the DPNI
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[in]	mask		event mask to trigger interrupt.
 *				each bit:
 *					0 = ignore event
 *					1 = consider event for asserting irq
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_irq_mask(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		      uint8_t irq_index, uint32_t mask);

/**
 * @brief	Gets interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]	dpni_handle	handle to the DPNI
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	mask		event mask to trigger interrupt
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_irq_mask(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		      uint8_t irq_index, uint32_t *mask);

/**
 * @brief	Gets the current status of any pending interrupts.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]	dpni_handle	handle to the DPNI
 * @param[in]	dpni		DPNI descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	status		interrupts status - one bit per cause
 *					0 = no interrupt pending
 *					1 = interrupt pending
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dpni_get_irq_status(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			uint8_t irq_index, uint32_t *status);

/**
 * @brief	Clears a pending interrupt's status
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]	dpni_handle	handle to the DPNI
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	status		bits to clear (W1C) - one bit per cause
 *					0 = don't change
 *					1 = clear status bit
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dpni_clear_irq_status(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			  uint8_t irq_index,
			  uint32_t status);

/**
 *
 * @brief	Enable/Disable transmission of Pause-Frames.
 *		Will only affect the underlying MAC if exist.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]   priority -  the PFC class of service; use '0xff'
 *		to indicate legacy pause support (i.e. no PFC).
 * @param[in]   pause_time - Pause quanta value used with transmitted
 *		pause frames. Each quanta represents a 512 bit-times;
 *		Note that '0' as an input here will be used
 *		as disabling the transmission of the pause-frames.
 * @param[in]   thresh_time - Pause Threshold quanta value used by the MAC to
 *		retransmit pause frame. if the situation causing
 *		a pause frame to be sent didn't finish when
 *		the timer reached the threshold quanta, the MAC
 *		will retransmit the pause frame.
 *		Each quanta represents a 512 bit-times.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_tx_pause_frames(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			     uint8_t priority,
			     uint16_t pause_time,
			     uint16_t thresh_time);

/**
 *
 * @brief	Enable/Disable ignoring of Pause-Frames.
 *		Will only affect the underlying MAC if exist.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]   enable - indicates whether to ignore the incoming pause
 *		frames or not.
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_rx_ignore_pause_frames(struct fsl_mc_io *mc_io,
				    uint16_t dpni_handle, int enable);

/**
 * @brief	Structure representing DPNI pools parameters
 */
struct dpni_pools_cfg {
	uint8_t num_dpbp; /*!< number of DPBPs */
	struct {
		uint16_t dpbp_id; /*!< DPBPs object id */
		uint16_t buffer_size; /*!< buffer size */
	} pools[DPNI_MAX_DPBP];
};

/**
 *
 * @brief	Set the pools
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	cfg - pools configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 *
 */
int dpni_set_pools(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		   const struct dpni_pools_cfg *cfg);

/**
 * @brief   DPNI destination types
 */
enum dpni_dest {
	DPNI_DEST_NONE = 0,
	/*!< unassigned destination; i.e. queues will be set in parked mode  */
	DPNI_DEST_DPIO,
	/*!< queues will generate notification to the dpio's channel;
	 i.e. will be set in schedule mode and FQDAN enable */
	DPNI_DEST_DPCON
/*!< queues won't generate notification, but will be connected to this
 channel object; i.e. will be set in schedule mode and FQDAN disable */
};

/**
 * @brief	Structure representing DPNI destination parameters
 */
struct dpni_dest_cfg {
	enum dpni_dest type; /*!< destination type */
	uint16_t id;
	/*!< either DPIO id or DPCON id depends on the channel type */
	uint8_t priority;
/*!< 0-1 or 0-7 (depends on the channel type) to select the priority(work-queue)
 within the channel (not relevant for the 'NONE' case) */
};

/**
 * @brief	Structure representing DPNI attach parameters
 */
struct dpni_attach_cfg {
	/* TODO - add struct ldpaa_flow_ctx	*flc; */
	uint64_t rx_user_ctx;
	/*!< User context; will be received with the FD in case of Rx
	 frame; can be override by calling 'dpni_set_rx_flow' */
	int dest_apply_all; /*!< in case 'dest_apply_all'=1, 'dest_cfg' will
	 affect on all receive queues, otherwise it will affect only
	 on rx-err/tx-err queues. */
	struct dpni_dest_cfg dest_cfg; /*!< destination settings; will be
	 applied according to the 'dest_apply_all' */
	uint64_t rx_err_user_ctx;
	/*!< User context; will be received with the FD in case of Rx
	 error frame */
	uint64_t tx_err_user_ctx;
	/*!< User context; will be received with the FD in case of Tx
	 error frame and 'DPNI_OPT_TX_CONF_DISABLED' is set.
	 if not set, tx-error frames will received with 'tx_conf_user_ctx' */
	uint64_t tx_conf_user_ctx;
/*!< User context; will be received with the FD in case of Tx
 confirmation frame; can be override by calling 'dpni_set_tx_flow' */
};

/**
 *
 * @brief	Attach the NI to application
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	cfg - Attach configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 *
 */
int dpni_attach(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		const struct dpni_attach_cfg *cfg);

/**
 *
 * @brief	Detach the NI from application
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 *
 */
int dpni_detach(struct fsl_mc_io *mc_io, uint16_t dpni_handle);

/**
 *
 * @brief	Enable the NI, will allow sending and receiving frames.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_enable(struct fsl_mc_io *mc_io, uint16_t dpni_handle);

/**
 *
 * @brief	Disable the NI, will disallow sending and receiving frames.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_disable(struct fsl_mc_io *mc_io, uint16_t dpni_handle);

/**
 *
 * @brief	Reset the NI, will return to initial state.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_reset(struct fsl_mc_io *mc_io, uint16_t dpni_handle);

/**
 * @brief	Structure representing attributes parameters
 */
struct dpni_attr {
	int id; /*!< DPNI ID */
	enum dpni_type type; /*!< DPNI Type */
	enum net_prot start_hdr;
	/*!< Valid only in the case 'type = DPNI_TYPE_NI';
	 * In NIC case will be set to NET_PROT_ETH */
	uint64_t options; /*!< reflect the value as was given in the
	 initialization phase */
	uint8_t max_senders; /*!< reflect the value as was given in the
	 initialization phase */
	uint8_t max_tcs; /*!< reflect the value as was given in the
	 initialization phase */
	uint16_t max_dist_per_tc[DPNI_MAX_TC]; /*!< reflect the value
	 as was given in the initialization phase */
	uint8_t max_unicast_filters;
	/*!< maximum number of unicast filters */
	uint8_t max_multicast_filters;
	/*!< maximum number of multicast filters */
	uint8_t max_vlan_filters;
	/*!< maximum number of vlan filters */
	uint8_t max_qos_entries;
	/*!< if 'max_tcs>1', declare the maximum entries for the  QoS table */
	uint8_t max_qos_key_size;
	/*!< maximum key size for the QoS look-up */
	uint8_t max_dist_key_size;
	/*!< maximum key size for the distribution look-up */
	struct {
		uint32_t major; /*!< DPNI major version*/
		uint32_t minor; /*!< DPNI minor version*/
	} version; /*!< DPNI version */
};

/**
 *
 * @brief	Retrieve the object's attributes.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[out]	attr - Object's attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_attributes(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			struct dpni_attr *attr);

/*!
 * @name DPNI buffer layout modification options
 *
 */
#define DPNI_LAYOUT_MOD_OPT_TIMESTAMP		0x00000001
/*!< Modify the time-stamp setting */
#define DPNI_LAYOUT_MOD_OPT_PARSER_RESULT	0x00000002
/*!< Modify the parser-result setting; Not applicable in TX */
#define DPNI_LAYOUT_MOD_OPT_FRAME_STATUS	0x00000004
/*!< Modify the frame-status setting */
#define DPNI_LAYOUT_MOD_OPT_PRIVATE_DATA_SIZE	0x00000008
/*!< Modify the private-data-size setting */
#define DPNI_LAYOUT_MOD_OPT_DATA_ALIGN		0x00000010
/*!< Modify the data-alignment setting */
/* @} */

/**
 * @brief	Structure representing DPNI buffer layout
 */
struct dpni_buffer_layout {
	uint32_t options;
	/*!< the flags that represent the modification that are required to be
	 done for the buffer layout; use 'DPNI_RX_LAYOUT_MOD_OPT_xxx' */
	int pass_timestamp;
	/*!< This option maybe used when 'options' set
	 with DPNI_RX_LAYOUT_MOD_OPT_TIMESTAMP */
	int pass_parser_result;
	/*!< This option maybe used when 'options' set
	 with DPNI_RX_LAYOUT_MOD_OPT_PARSER_RESULT */
	int pass_frame_status;
	/*!< This option maybe used when 'options' set
	 with DPNI_RX_LAYOUT_MOD_OPT_FRAME_STATUS */
	uint16_t private_data_size;
	/*!< This option maybe used when 'options' set
	 with DPNI_RX_LAYOUT_MOD_OPT_PRIVATE_DATA_SIZE */
	uint16_t data_align;
/*!< This option maybe used when 'options' set
 with DPNI_RX_LAYOUT_MOD_OPT_DATA_ALIGN */
};

/**
 *
 * @brief	Retrieve the RX buffer layout settings.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[out]	layout - buffer's layout
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_rx_buffer_layout(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			      struct dpni_buffer_layout *layout);

/**
 *
 * @brief	Set the RX buffer layout settings.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	layout - buffer's layout
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 */
int dpni_set_rx_buffer_layout(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			      const struct dpni_buffer_layout *layout);

/**
 *
 * @brief	Retrieve the TX buffer layout settings.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[out]	layout - buffer's layout
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_buffer_layout(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			      struct dpni_buffer_layout *layout);

/**
 *
 * @brief	Set the TX buffer layout settings.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	layout - buffer's layout
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 */
int dpni_set_tx_buffer_layout(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			      const struct dpni_buffer_layout *layout);

/**
 *
 * @brief	Retrieve the TX-Conf buffer layout settings.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[out]	layout - buffer's layout
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_conf_buffer_layout(struct fsl_mc_io *mc_io,
				   uint16_t dpni_handle,
				   struct dpni_buffer_layout *layout);

/**
 *
 * @brief	Set the TX-Conf buffer layout settings.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	layout - buffer's layout
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 */
int dpni_set_tx_conf_buffer_layout(struct fsl_mc_io *mc_io,
				   uint16_t dpni_handle,
				   const struct dpni_buffer_layout *layout);

/**
 *
 * @brief	Enable/disable L3 checksum validation
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	en - enable/disable checksum validation
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_l3_chksum_validation(struct fsl_mc_io *mc_io,
				  uint16_t dpni_handle, int en);

/**
 *
 * @brief	Get L3 checksum validation mode
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[out]	en - enable/disable checksum validation
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_l3_chksum_validation(struct fsl_mc_io *mc_io,
				  uint16_t dpni_handle, int *en);

/**
 *
 * @brief	Enable/disable L4 checksum validation
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	en - enable/disable checksum validation
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_l4_chksum_validation(struct fsl_mc_io *mc_io,
				  uint16_t dpni_handle, int en);

/**
 *
 * @brief	Get L4 checksum validation mode
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[out]	en - enable/disable checksum validation
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_l4_chksum_validation(struct fsl_mc_io *mc_io,
				  uint16_t dpni_handle, int *en);

/**
 *
 * @brief	Get the QDID used for enqueue
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[out]	qdid - Qdid used for qneueue
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpni_enable().
 */
int dpni_get_qdid(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		  uint16_t *qdid);

/**
 *
 * @brief	Get the AIOP's SPID that represent this NI
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[out]	spid - aiop's storage-profile
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Only relevant for AIOP.
 */
int dpni_get_spid(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		  uint16_t *spid);

/**
 *
 * @brief	Get the tx data offset
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[out]	data_offset - TX data offset
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_data_offset(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			    uint16_t *data_offset);

/**
 * @brief	DPNI Counter types
 *
 */
enum dpni_counter {
	DPNI_CNT_ING_FRAME = 1,
	/*!< Ingress frame count */
	DPNI_CNT_ING_BYTE,
	/*!< Ingress byte count */
	DPNI_CNT_ING_FRAME_DROP,
	/*!< Ingress frame dropped counter due to
	 explicit 'drop' setting */
	DPNI_CNT_ING_FRAME_DISCARD,
	/*!< Ingress frame discarded counter due to errors */
	DPNI_CNT_ING_MCAST_FRAME,
	/*!< Ingress multicast frame count */
	DPNI_CNT_ING_MCAST_BYTE,
	/*!< Ingress multicast byte count */
	DPNI_CNT_ING_BCAST_FRAME,
	/*!< Ingress broadcast frame count */
	DPNI_CNT_ING_BCAST_BYTES,
	/*!< Ingress broad bytes count */
	DPNI_CNT_EGR_FRAME,
	/*!< Egress frame count */
	DPNI_CNT_EGR_BYTE,
	/*!< Egress byte count */
	DPNI_CNT_EGR_FRAME_DISCARD
/*!< Egress frame discard counter due to errors */
};

/**
 *
 * @brief   Read one of the DPNI counters
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	counter - the requested counter
 * @param[out]	value - counter's current value
 *
 * @returns        '0' on Success; Error code otherwise.
 */
int dpni_get_counter(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		     enum dpni_counter counter,
		     uint64_t *value);

/**
 *
 * @brief   Write to one of the DPNI counters
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	counter - the requested counter
 * @param[in]   value - New counter value.
 *		typically '0' for resetting the counter.
 *
 * @returns        '0' on Success; Error code otherwise.
 */
int dpni_set_counter(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		     enum dpni_counter counter,
		     uint64_t value);

/**
 *
 * @brief	Return the link state, either up or down
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[out]	up - return '0' for down, '1' for up
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_link_state(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			int *up);

/**
 *
 * @brief	Set the maximum received frame length.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	mfl - MFL length
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_mfl(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		 uint16_t mfl);

/**
 *
 * @brief	Get the maximum received frame length.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[out]	mfl - MFL length
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_mfl(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		 uint16_t *mfl);

/**
 *
 * @brief	Set the MTU for this interface. Will have affect on IPF and
 *		conditionally on GSO.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	mtu - MTU length
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_mtu(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		 uint16_t mtu);

/**
 *
 * @brief	Get the MTU.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[out]	mtu - MTU length
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_mtu(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		 uint16_t *mtu);

/**
 *
 * @brief	Enable/Disable multicast promiscuous mode
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	en - '1' for enabling/'0' for disabling
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_multicast_promisc(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			       int en);

/**
 *
 * @brief	Get multicast promiscuous mode
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[out]	en - '1' for enabling/'0' for disabling
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_multicast_promisc(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			       int *en);

/**
 *
 * @brief	Set the primary mac address
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	addr - MAC address
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_primary_mac_addr(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			      const uint8_t addr[6]);

/**
 *
 * @brief	Get the primary mac address
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[out]	addr - MAC address
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_primary_mac_addr(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			      uint8_t addr[6]);

/**
 *
 * @brief	Add unicast/multicast filter address
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	addr - MAC address
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_add_mac_addr(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		      const uint8_t addr[6]);

/**
 *
 * @brief	Remove unicast/multicast filter address
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	addr - MAC address
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_remove_mac_addr(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			 const uint8_t addr[6]);

/**
 *
 * @brief	Clear the mac filter table
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_clear_mac_table(struct fsl_mc_io *mc_io, uint16_t dpni_handle);

/**
 *
 * @brief	Enable/Disable VLAN filtering mode
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	en - '1' for enabling/'0' for disabling
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_vlan_filters(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			  int en);

/**
 *
 * @brief	Add VLAN-id filter
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	vlan_id - VLAN ID
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_add_vlan_id(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		     uint16_t vlan_id);

/**
 *
 * @brief	Remove VLAN-id filter
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	vlan_id - VLAN ID
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_remove_vlan_id(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			uint16_t vlan_id);

/**
 *
 * @brief	Clear the VLAN filter table
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_clear_vlan_table(struct fsl_mc_io *mc_io, uint16_t dpni_handle);

/**
 * @brief	Structure representing DPNI TX TC parameters
 */
struct dpni_tx_tc_cfg {
	uint16_t depth_limit;
/*!<  if >0 than limit the depth of this queue which may result with
 * rejected frames */
/* TODO - support both bytes & frames??? if not what we prefer?? */
};

/**
 *
 * @brief	Set TX TC settings
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	tc_id - Traffic class id
 * @param[in]	cfg - TC parameters
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_tx_tc(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		   uint8_t tc_id,
		   const struct dpni_tx_tc_cfg *cfg);

/**
 * @brief	distribution mode
 */
enum dpni_dist_mode {
	DPNI_DIST_MODE_NONE = 0,/*!< no distribution */
	DPNI_DIST_MODE_HASH, /*!< hash-distribution */
	DPNI_DIST_MODE_FS
/*!< flow-steering distribution */
};

/**
 * @brief	Structure representing DPNI RX TC parameters
 */
struct dpni_rx_tc_cfg {
	uint16_t dist_size; /*!< set the distribution size */
	enum dpni_dist_mode dist_mode; /*!< distribution mode */
	struct dpkg_profile_cfg *extract_cfg;
/*!< define the extractions to be used for the distribution key */
/*	struct policing_cfg *params;*/
/*TODO - add struct ldpaa_flow_ctx	*flc;*/
};

/**
 *
 * @brief	Set RX TC settings
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	tc_id - Traffic class id
 * @param[in]	cfg - TC parameters
 *
 * @returns	'0' on Success; error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 *
 */
int dpni_set_rx_tc(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		   uint8_t tc_id,
		   const struct dpni_rx_tc_cfg *cfg);

/*!
 * @name DPNI Tx flow modification options
 *
 */
#define DPNI_TX_FLOW_MOD_OPT_TX_CONF_ERR	0x00000001
/*!< Modify the flow's settings for dedicate tx confirmation/error */
#define DPNI_TX_FLOW_MOD_OPT_ONLY_TX_ERR	0x00000002
/*!< Modify the tx confirmation/error behavior*/
#define DPNI_TX_FLOW_MOD_OPT_DEST		0x00000004
/*!< Modify the tx-confirmation/error queue destination parameters*/
#define DPNI_TX_FLOW_MOD_OPT_USER_CTX		0x00000008
/*!< Modify the tx-confirmation/error user-context*/
#define DPNI_TX_FLOW_MOD_OPT_L3_CHKSUM_GEN	0x00000010
/*!< Modify the flow's l3 checksum generation */
#define DPNI_TX_FLOW_MOD_OPT_L4_CHKSUM_GEN	0x00000020
/*!< Modify the flow's l4 checksum generation */
/* @} */

/**
 * @brief	Structure representing DPNI TX flow parameters
 */
struct dpni_tx_flow_cfg {
	uint32_t options;
	/*!< the flags that represent the modification that are required to be
	 done for this tx-flow; use 'DPNI_TX_FLOW_MOD_OPT_xxx' */
	int tx_conf_err;
	/*!< This option maybe used when 'options' set
	 with DPNI_TX_FLOW_MOD_OPT_TX_CONF_ERR; Prefer this flow to
	 have its private tx confirmation/error settings */
	int only_error_frames; /*!< This option maybe used when 'options' set
	 with DPNI_TX_FLOW_MOD_OPT_ONLY_TX_ERR and 'tx_conf_err' = 1;
	 if 'only_error_frames' = 1,  will send back only errors frames.
	 else send both confirmation and error frames */
	struct dpni_dest_cfg dest_cfg; /*!< This option maybe used
	 when 'options' set with DPNI_TX_FLOW_MOD_OPT_DEST; */
	uint64_t user_ctx;
	/*!< This option maybe used when 'options' set
	 with DPNI_TX_FLOW_MOD_OPT_USER_CTX; will be provided in case
	 of 'tx_conf_err'= 1 or enqueue-rejection condition ("lossless") */
	int l3_chksum_gen;
	/*!< This option maybe used when 'options' set
	 with DPNI_TX_FLOW_MOD_OPT_L3_CHKSUM_GEN; enable/disable checksum l3
	 generation */
	int l4_chksum_gen;
/*!< This option maybe used when 'options' set
 with DPNI_TX_FLOW_MOD_OPT_L4_CHKSUM_GEN; enable/disable checksum l4
 generation */
};

/**
 *
 * @brief	Set TX flow configuration
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in,out] flow_id - this id is the sender index; should be used as the
 *		third argument of the enqueue command (QDBIN);
 *		for each new sender (flow) use 'DPNI_NEW_FLOW_ID'.
 *		driver will provide back a new flow_id that should
 *		be used for succeeding calls.
 * @param[in]	cfg - flow configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_tx_flow(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		     uint16_t *flow_id,
		     const struct dpni_tx_flow_cfg *cfg);

/**
 *
 * @brief	Get TX flow configuration and fqid
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	flow_id - this id is the sender index
 * @param[out]	cfg - flow configuration
 * @param[out]	fqid - virtual fqid to be used for dequeue operations;
 *		if equal to 'DPNI_VFQID_NOT_VALID' means you need to
 *		call this function after you enable the NI.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_flow(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		     uint16_t flow_id,
		     struct dpni_tx_flow_cfg *cfg,
		     uint32_t *fqid);

/*!
 * @name DPNI Rx flow modification options
 *
 */
#define DPNI_RX_FLOW_MOD_OPT_USER_CTX		0x00000001
/*!< Modify the user's context parameters */
#define DPNI_RX_FLOW_MOD_OPT_DEST		0x00000002
/*!< Modify the destination parameters */
/* #define DPNI_RX_Q_MOD_OPT_FLC			0x00000004 */
/*!< Modify the flow-context (e.g. stashing) parameters */
/* @} */

/**
 * @brief	Structure representing DPNI RX flow parameters
 */
struct dpni_rx_flow_cfg {
	uint32_t options;
	/*!< the flags that represent the modification that are required to be
	 done for the rx-flow; use 'DPNI_RX_FLOW_MOD_OPT_xxx' */
	uint64_t user_ctx;
	/*!< This option maybe used when 'options' set
	 with DPNI_RX_FLOW_MOD_OPT_USER_CTX; will be provided
	 with each rx frame */
	struct dpni_dest_cfg dest_cfg; /*!< This option maybe used
	 when 'options' set with DPNI_RX_FLOW_MOD_OPT_DEST; */
/* TODO - add struct ldpaa_flow_ctx	*flc;*/
/*!< valid only in case of flow-steering */
};

/**
 *
 * @brief	Set RX flow configuration
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	tc_id - Traffic class id; use 'DPNI_ALL_TCS' for all TCs
 *		and flows
 * @param[in]	flow_id - flow id within the traffic class;
 *		use 'DPNI_ALL_TC_FLOWS' for all flows within this tc_id
 * @param[in]	cfg - flow configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_rx_flow(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		     uint8_t tc_id,
		     uint16_t flow_id,
		     const struct dpni_rx_flow_cfg *cfg);

/**
 *
 * @brief	Get TX flow configuration and fqid
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	tc_id - Traffic class id
 * @param[in]	flow_id - flow id within the traffic class;
 * @param[out]	cfg - flow configuration
 * @param[out]	fqid - virtual fqid to be used for dequeue operations;
 *		if equal to 'DPNI_VFQID_NOT_VALID' means you need to
 *		call this function after you enable the NI.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_rx_flow(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		     uint8_t tc_id,
		     uint16_t flow_id,
		     struct dpni_rx_flow_cfg *cfg,
		     uint32_t *fqid);

/**
 * @brief	Structure representing QOS table parameters
 */
struct dpni_qos_tbl_cfg {
	struct dpkg_profile_cfg *extract_cfg;
	/*!< define the extractions to be used as the QoS criteria */
	int drop_frame;
	/*!< '1' for dropping the frame in case of no match;
	 '0' for using the 'default_tc' */
	uint8_t default_tc;
/*!< will be used in case of no-match and 'drop_frame'= 0 */
};

/**
 *
 * @brief	Set QoS mapping table
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	cfg - QoS table configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_set_qos_table(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		       const struct dpni_qos_tbl_cfg *cfg);

/**
 *
 * @brief	Delete QoS mapping table
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_delete_qos_table(struct fsl_mc_io *mc_io, uint16_t dpni_handle);

/**
 * @brief	Structure representing DPNI key parameters
 */
struct dpni_key_cfg {
	uint8_t *key; /*!< A pointer to the key */
	uint8_t *mask;/*!< A pointer to the mask */
	uint8_t size; /*!< key/mask size */
};

/**
 *
 * @brief	Add QoS mapping entry
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	cfg - QoS key parameters
 * @param[in]	tc_id - Traffic class id
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_add_qos_entry(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		       const struct dpni_key_cfg *cfg,
		       uint8_t tc_id);

/**
 *
 * @brief	Remove QoS mapping entry
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	cfg - QoS key parameters
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_remove_qos_entry(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			  const struct dpni_key_cfg *cfg);

/**
 *
 * @brief	Clear all QoS mapping entries
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_clear_qos_table(struct fsl_mc_io *mc_io, uint16_t dpni_handle);

/**
 * @brief   DPNI Flow-Steering miss action
 */
enum dpni_fs_miss_action {
	DPNI_FS_MISS_DROP = 0,
	/*!< in case of no-match drop the frame  */
	DPNI_FS_MISS_EXPLICIT_FLOWID,
	/*!< in case of no-match go to explicit flow-id */
	DPNI_FS_MISS_HASH
/*!< in case of no-match do a hashing to select a flow */
};

/**
 * @brief	Structure representing FS table parameters
 */
struct dpni_fs_tbl_cfg {
	enum dpni_fs_miss_action miss_action; /*!< miss action mode */
	uint16_t default_flow_id;
/*!< will be used in case 'DPNI_FS_MISS_EXPLICIT_FLOWID' */
/* TODO - add mask to select a subset of the hash result */
};

/**
 *
 * @brief	Set FS mapping table for a specific traffic class
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	tc_id - Traffic class id
 * @param[in]	cfg - FS table configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_set_fs_table(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		      uint8_t tc_id,
		      const struct dpni_fs_tbl_cfg *cfg);

/**
 *
 * @brief	Delete FS mapping table
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	tc_id - Traffic class id
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_delete_fs_table(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			 uint8_t tc_id);

/**
 *
 * @brief	Add FS entry for a specific traffic-class
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	tc_id - Traffic class id
 * @param[in]	cfg - Key parameters
 * @param[in]	flow_id - Flow id
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_add_fs_entry(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		      uint8_t tc_id,
		      const struct dpni_key_cfg *cfg,
		      uint16_t flow_id);

/**
 *
 * @brief	Remove FS entry from a specific traffic-class
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	tc_id - Traffic class id
 * @param[in]	cfg - Key parameters
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_remove_fs_entry(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			 uint8_t tc_id,
			 const struct dpni_key_cfg *cfg);

/**
 *
 * @brief	Clear all FS entries
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpni_handle - handle to the DPNI
 * @param[in]	tc_id - Traffic class id
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_clear_fs_table(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			uint8_t tc_id);

/** @} */

#endif /* __FSL_DPNI_H */

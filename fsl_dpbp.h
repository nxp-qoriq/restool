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
 *  @file    fsl_dpbp.h
 *  @brief   Data Path Buffer Pool API
 */
#ifndef __FSL_DPBP_H
#define __FSL_DPBP_H

/*!
 * @Group grp_dpbp	Data Path Buffer Pool API
 *
 * @brief	Contains initialization APIs and runtime control APIs for DPBP
 * @{
 */

struct fsl_mc_io;

/*!
 * @name General DPBP macros
 */
#define DPBP_BPID_NOT_VALID			(-1)
/*!< Invalid BPID  */
/* @} */

/**
 * @brief	structure representing DPBP configuration
 */
struct dpbp_cfg {
	int tmp; /* place holder!!!! */
	/* TODO - need to add depletion parameters
 * (either here or in separate routine) */
};

/**
 * @brief	structure representing attr parameter
 */
struct dpbp_attr {
	int id; /*!< DPBP id*/
	uint16_t bpid; /*!< buffer pool id;
	 in case equal to DPBP_BPID_NOT_VALID the bpid isn't valid
	 and must not be used; Only after 'enable' bpid will be valid; */
	struct {
		uint32_t major; /*!< DPBP major version*/
		uint32_t minor; /*!< DPBP minor version*/
	} version; /*!< DPBP version */
};

/**
 * @brief	Open object handle, allocate resources and preliminary initialization -
 *		required before any operation on the object
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	cfg - Configuration structure
 * @param[out]	dpbp_handle - Pointer to handle of the DPBP object
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Required before any operation on the object
 */
int dpbp_create(struct fsl_mc_io *mc_io, const struct dpbp_cfg *cfg,
		uint16_t *dpbp_handle);

/**
 * @brief	Open object handle
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpbp_id - DPBP unique ID
 * @param[out]	dpbp_handle - Pointer to handle of the DPBP object
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpbp_open(struct fsl_mc_io *mc_io, int dpbp_id, uint16_t *dpbp_handle);

/**
 * @brief	Closes the object handle, no further operations on the object
 *		are allowed
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpbp_handle - handle of the DPBP object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_close(struct fsl_mc_io *mc_io, uint16_t dpbp_handle);

/**
 * @brief	Frees all allocated resources
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpbp_handle - handle of the DPBP object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_destroy(struct fsl_mc_io *mc_io, uint16_t dpbp_handle);

/**
 * @brief	Enable the dpbp
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpbp_handle - handle of the DPBP object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_enable(struct fsl_mc_io *mc_io, uint16_t dpbp_handle);

/**
 * @brief	Disable the dpbp
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpbp_handle - handle of the DPBP object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_disable(struct fsl_mc_io *mc_io, uint16_t dpbp_handle);

/**
 * @brief	Reset the dpbp, will return to initial state.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpbp_handle - handle of the DPBP object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_reset(struct fsl_mc_io *mc_io, uint16_t dpbp_handle);

/**
 * @brief	Retrieves the object's attributes.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpbp_handle - handle of the DPBP object
 * @param[out]	attr - Object's attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpbp_enable().
 */
int dpbp_get_attributes(struct fsl_mc_io *mc_io, uint16_t dpbp_handle,
			struct dpbp_attr *attr);

/**
 * @brief	Sets IRQ information for the DPBP to trigger an interrupt.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpbp_handle - handle of the DPBP object
 * @param[in]	irq_index	Identifies the interrupt index to configure.
 * @param[in]	irq_paddr	Physical IRQ address that must be written to
 *				signal a message-based interrupt
 * @param[in]	irq_val		Value to write into irq_paddr address
 * @param[in]	user_irq_id	A user defined number associated with this IRQ;
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_set_irq(struct fsl_mc_io *mc_io, uint16_t dpbp_handle,
		 uint8_t irq_index,
		 uint64_t irq_paddr,
		 uint32_t irq_val,
		 int user_irq_id);

/**
 * @brief	Gets IRQ information from the DPBP.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpbp_handle - handle of the DPBP object
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
int dpbp_get_irq(struct fsl_mc_io *mc_io, uint16_t dpbp_handle,
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
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpbp_handle - handle of the DPBP object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[in]	enable_state	interrupt state - enable = 1, disable = 0.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_set_irq_enable(struct fsl_mc_io *mc_io, uint16_t dpbp_handle,
			uint8_t irq_index,
			uint8_t enable_state);

/**
 * @brief	Gets overall interrupt state
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpbp_handle - handle of the DPBP object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	enable_state	interrupt state - enable = 1, disable = 0.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_get_irq_enable(struct fsl_mc_io *mc_io, uint16_t dpbp_handle,
			uint8_t irq_index,
			uint8_t *enable_state);

/**
 * @brief	Sets interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpbp_handle - handle of the DPBP object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[in]	mask		event mask to trigger interrupt.
 *				each bit:
 *					0 = ignore event
 *					1 = consider event for asserting irq
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_set_irq_mask(struct fsl_mc_io *mc_io, uint16_t dpbp_handle,
		      uint8_t irq_index, uint32_t mask);

/**
 * @brief	Gets interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpbp_handle - handle of the DPBP object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	mask		event mask to trigger interrupt
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_get_irq_mask(struct fsl_mc_io *mc_io, uint16_t dpbp_handle,
		      uint8_t irq_index, uint32_t *mask);

/**
 * @brief	Gets the current status of any pending interrupts.
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpbp_handle - handle of the DPBP object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	status		interrupts status - one bit per cause
 *					0 = no interrupt pending
 *					1 = interrupt pending
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dpbp_get_irq_status(struct fsl_mc_io *mc_io, uint16_t dpbp_handle,
			uint8_t irq_index, uint32_t *status);

/**
 * @brief	Clears a pending interrupt's status
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]	dpbp_handle - handle of the DPBP object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	status		bits to clear (W1C) - one bit per cause
 *					0 = don't change
 *					1 = clear status bit
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dpbp_clear_irq_status(struct fsl_mc_io *mc_io, uint16_t dpbp_handle,
			  uint8_t irq_index,
			  uint32_t status);

/** @} */

#endif /* __FSL_DPBP_H */

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
 *  @file    fsl_dpcon.h
 *  @brief   Data Path Concentrator API
 */
#ifndef __FSL_DPCON_H
#define __FSL_DPCON_H

/*!
 * @Group grp_dpcon	Data Path Concentrator API
 *
 * @brief	Contains initialization APIs and runtime control APIs for DPCON
 * @{
 */

struct fsl_mc_io;

/**
 * @brief	Open a control session for the specified object
 *
 *		This function can be used to open a control session for an
 *		already created object; an object may have been declared in
 *		the DPL or by calling the dpcon_create() function.
 *		This function returns a unique authentication token,
 *		associated with the specific object ID and the specific MC
 *		portal; this token must be used in all subsequent commands for
 *		this specific object.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]	dpcon_id	DPCON unique ID
 * @param[out]	token		Returned token; use in subsequent API calls
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpcon_open(struct fsl_mc_io *mc_io, int dpcon_id, uint16_t *token);

/**
 * @brief	Close the control session of the object
 *
 *		After this function is called, no further operations are
 *		allowed on the object without opening a new control session.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPCON object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpcon_close(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Structure representing DPCON configuration
 */
struct dpcon_cfg {
	uint8_t num_priorities;
	/*!< Number of priorities for the DPCON channel (1-8) */
};

/**
 * @brief	Create the DPCON object, allocate required resources and
 *		perform required initialization.
 *
 *		The object can be created either by declaring it in the
 *		DPL file, or by calling this function.
 *
 *		This function returns a unique authentication token,
 *		associated with the specific object ID and the specific MC
 *		portal; this token must be used in all subsequent calls to
 *		this specific object. For objects that are created using the
 *		DPL file, call dpcon_open() function to get an authentication
 *		token first.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]	cfg	Configuration structure
 * @param[out]	token	Returned token; use in subsequent API calls
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpcon_create(struct fsl_mc_io	*mc_io,
		 const struct dpcon_cfg *cfg,
		 uint16_t		*token);

/**
 * @brief	Destroy the DPCON object and release all its resources.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPCON object
 *
 * @returns	'0' on Success; error code otherwise.
 */
int dpcon_destroy(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Enable the DPCON
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPCON object
 *
 * @returns	'0' on Success; Error code otherwise
 */
int dpcon_enable(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Disable the DPCON
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPCON object
 *
 * @returns	'0' on Success; Error code otherwise
 */
int dpcon_disable(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Check if the DPCON is enabled.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPCON object
 * @param[out]  en	Returns '1' if object is enabled; '0' otherwise
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpcon_is_enabled(struct fsl_mc_io *mc_io, uint16_t token, int *en);

/**
 * @brief	Reset the DPCON, returns the object to initial state.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPCON object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpcon_reset(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Set IRQ information for the DPCON to trigger an interrupt.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPCON object
 * @param[in]	irq_index	Identifies the interrupt index to configure
 * @param[in]	irq_addr	Address that must be written to
 *				signal a message-based interrupt
 * @param[in]	irq_val		Value to write into irq_addr address
 * @param[out]	user_irq_id	A user defined number associated with this IRQ
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpcon_set_irq(struct fsl_mc_io	*mc_io,
		  uint16_t		token,
		 uint8_t		irq_index,
		 uint64_t		irq_addr,
		 uint32_t		irq_val,
		 int			user_irq_id);

/**
 * @brief	Get IRQ information from the DPCON.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPCON object
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
int dpcon_get_irq(struct fsl_mc_io	*mc_io,
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
 * @param[in]   token		Token of DPCON object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[in]	en		Interrupt state - enable = 1, disable = 0
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpcon_set_irq_enable(struct fsl_mc_io	*mc_io,
			 uint16_t		token,
			 uint8_t		irq_index,
			 uint8_t		en);

/**
 * @brief	Get overall interrupt state.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPCON object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	en		Interrupt state - enable = 1, disable = 0
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpcon_get_irq_enable(struct fsl_mc_io	*mc_io,
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
 * @param[in]   token		Token of DPCON object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[in]	mask		event mask to trigger interrupt;
 *				each bit:
 *					0 = ignore event
 *					1 = consider event for asserting irq
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpcon_set_irq_mask(struct fsl_mc_io	*mc_io,
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
 * @param[in]   token		Token of DPCON object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	mask		event mask to trigger interrupt
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpcon_get_irq_mask(struct fsl_mc_io	*mc_io,
		       uint16_t		token,
		       uint8_t		irq_index,
		       uint32_t		*mask);

/**
 * @brief	Get the current status of any pending interrupts.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPCON object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	status		interrupts status - one bit per cause:
 *					0 = no interrupt pending
 *					1 = interrupt pending
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpcon_get_irq_status(struct fsl_mc_io	*mc_io,
			 uint16_t		token,
			 uint8_t		irq_index,
			 uint32_t		*status);

/**
 * @brief	Clear a pending interrupt's status
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPCON object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	status		bits to clear (W1C) - one bit per cause:
 *					0 = don't change
 *					1 = clear status bit
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpcon_clear_irq_status(struct fsl_mc_io	*mc_io,
			   uint16_t		token,
			   uint8_t		irq_index,
			   uint32_t		status);

/**
 * @brief	Structure representing DPCON attributes
 */
struct dpcon_attr {
	int id;
	/*!< DPCON object ID */
	struct {
		uint16_t major;
		/*!< DPCON major version */
		uint16_t minor;
		/*!< DPCON minor version */
	} version;
	/*!< DPCON version */
	uint16_t qbman_ch_id;
	/*!< Channel ID to be used by dequeue operation; */
	uint8_t num_priorities;
	/*!< Number of priorities for the DPCON channel (1-8) */
};

/**
 * @brief	Retrieve DPCON attributes.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPCON object
 * @param[out]	attr	Object's attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpcon_get_attributes(struct fsl_mc_io	*mc_io,
			 uint16_t		token,
			 struct dpcon_attr	*attr);

/**
 * @brief	Structure representing notification parameters
 */
struct dpcon_notification_cfg {
	int dpio_id;
	/*!< DPIO object ID; must be configured with a notification channel */
	uint8_t priority;
	/*!< Priority selection within the DPIO channel; valid values
	 * are 0-7, depending on the number of priorities in that
	 * channel
	 */
	uint64_t user_ctx;
	/*!< User context value provided with each CDAN message */
};

/**
 * @brief	Set DPCON notification destination
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPCON object
 * @param[in]	cfg	Notification parameters
 *
 * @returns	'0' on Success; Error code otherwise
 */
int dpcon_set_notification(struct fsl_mc_io			*mc_io,
			   uint16_t				token,
			   struct dpcon_notification_cfg	*cfg);

/** @} */

#endif /* __FSL_DPCON_H */

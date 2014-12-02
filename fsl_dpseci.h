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
 *  @file    fsl_dpseci.h
 *  @brief   Data Path SEC Interface API
 *
 */
#ifndef __FSL_DPSECI_H
#define __FSL_DPSECI_H

/*!
 * @Group grp_dpseci	Data Path SEC Interface API
 *
 * @brief	Contains initialization APIs and runtime control APIs for DPSECI
 *
 * @{
 */

struct fsl_mc_io;

/*!
 * @name	General DPSECI macros
 */
#define DPSECI_PRIO_NUM		2
/*!< Maximum number of Tx/Rx priorities per DPSECI object */
#define DPSECI_ALL_QUEUES	(uint8_t)(-1)
/*!< All queues considered; see dpseci_set_rx_queue() */
/* @} */

/**
 * @brief	Open a control session for the specified object
 *
 *		This function can be used to open a control session for an
 *		already created object; an object may have been declared in
 *		the DPL or by calling the dpseci_create() function.
 *		This function returns a unique authentication token,
 *		associated with the specific object ID and the specific MC
 *		portal; this token must be used in all subsequent commands for
 *		this specific object.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]	dpseci_id	DPSECI unique ID
 * @param[out]	token		Returned token; use in subsequent API calls
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpseci_open(struct fsl_mc_io *mc_io, int dpseci_id, uint16_t *token);

/**
 * @brief	Close the control session of the object
 *
 *		After this function is called, no further operations are
 *		allowed on the object without opening a new control session.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSECI object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpseci_close(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Structure representing DPSECI configuration
 */
struct dpseci_cfg {
	uint8_t priorities[DPSECI_PRIO_NUM];
	/*!< Priorities for the SEC hardware processing; valid priorities are
	 * configured with values 1-8; the entry following last valid entry
	 * should be configured with 0
	 */
};

/**
 * @brief	Create the DPSECI object, allocate required resources and
 *		perform required initialization.
 *
 *		The object can be created either by declaring it in the
 *		DPL file, or by calling this function.
 *
 *		This function returns a unique authentication token,
 *		associated with the specific object ID and the specific MC
 *		portal; this token must be used in all subsequent calls to
 *		this specific object. For objects that are created using the
 *		DPL file, call dpseci_open() function to get an authentication
 *		token first.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]	cfg	Configuration structure
 * @param[out]	token	Returned token; use in subsequent API calls
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpseci_create(struct fsl_mc_io		*mc_io,
		  const struct dpseci_cfg	*cfg,
		  uint16_t			*token);

/**
 * @brief	Destroy the DPSECI object and release all its resources.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPSECI object
 *
 * @returns	'0' on Success; error code otherwise.
 */
int dpseci_destroy(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Enable the DPSECI, allow sending and receiving frames.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPSECI object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpseci_enable(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Disable the DPSECI, stop sending and receiving frames.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPSECI object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpseci_disable(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Check if the DPSECI is enabled.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPSECI object
 * @param[out]  en	Returns '1' if object is enabled; '0' otherwise
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpseci_is_enabled(struct fsl_mc_io *mc_io, uint16_t token, int *en);

/**
 * @brief	Reset the DPSECI, returns the object to initial state.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPSECI object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpseci_reset(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Get IRQ information from the DPSECI.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPSECI object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]  type		Interrupt type: 0 represents message interrupt
 *				type (both irq_addr and irq_val are valid)
 * @param[out]	irq_addr	Address that must be written to
 *				signal the message-based interrupt
 * @param[in]	irq_val		Value to write into irq_addr address
 * @param[in]	user_irq_id	A user defined number associated with this IRQ
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpseci_get_irq(struct fsl_mc_io	*mc_io,
		   uint16_t		token,
		   uint8_t		irq_index,
		   int			*type,
		   uint64_t		*irq_addr,
		   uint32_t		*irq_val,
		   int			*user_irq_id);

/**
 * @brief	Set IRQ information for the DPSECI to trigger an interrupt.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPSECI object
 * @param[in]	irq_index	Identifies the interrupt index to configure
 * @param[in]	irq_addr	Address that must be written to
 *				signal a message-based interrupt
 * @param[in]	irq_val		Value to write into irq_addr address
 * @param[out]	user_irq_id	A user defined number associated with this IRQ
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpseci_set_irq(struct fsl_mc_io	*mc_io,
		   uint16_t		token,
		   uint8_t		irq_index,
		   uint64_t		irq_addr,
		   uint32_t		irq_val,
		   int			user_irq_id);

/**
 * @brief	Set overall interrupt state.
 *
 * Allows GPP software to control when interrupts are generated.
 * Each interrupt can have up to 32 causes.  The enable/disable control's the
 * overall interrupt state. if the interrupt is disabled no causes will cause
 * an interrupt.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPSECI object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[in]	en		Interrupt state - enable = 1, disable = 0
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpseci_set_irq_enable(struct fsl_mc_io	*mc_io,
			  uint16_t		token,
			  uint8_t		irq_index,
			  uint8_t		en);

/**
 * @brief	Get overall interrupt state
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPSECI object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	en		Interrupt state - enable = 1, disable = 0
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpseci_get_irq_enable(struct fsl_mc_io	*mc_io,
			  uint16_t		token,
			  uint8_t		irq_index,
			  uint8_t		*en);

/**
 * @brief	Set interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPSECI object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[in]	mask		event mask to trigger interrupt;
 *				each bit:
 *					0 = ignore event
 *					1 = consider event for asserting IRQ
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpseci_set_irq_mask(struct fsl_mc_io	*mc_io,
			uint16_t		token,
			uint8_t			irq_index,
			uint32_t		mask);

/**
 * @brief	Get interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPSECI object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	mask		event mask to trigger interrupt
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpseci_get_irq_mask(struct fsl_mc_io	*mc_io,
			uint16_t		token,
			uint8_t			irq_index,
			uint32_t		*mask);

/**
 * @brief	Get the current status of any pending interrupts.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPSECI object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	status		interrupts status - one bit per cause:
 *					0 = no interrupt pending
 *					1 = interrupt pending
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpseci_get_irq_status(struct fsl_mc_io	*mc_io,
			  uint16_t		token,
			  uint8_t		irq_index,
			  uint32_t		*status);

/**
 * @brief	Clear a pending interrupt's status
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPSECI object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	status		bits to clear (W1C) - one bit per cause:
 *					0 = don't change
 *					1 = clear status bit
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpseci_clear_irq_status(struct fsl_mc_io	*mc_io,
			    uint16_t		token,
			    uint8_t		irq_index,
			    uint32_t		status);

/**
 * @brief	Structure representing DPSECI attributes
 */
struct dpseci_attr {
	int id;
	/*!< DPSECI object ID */
	struct {
		uint16_t major;
		/*!< DPSECI major version */
		uint16_t minor;
		/*!< DPSECI minor version */
	} version;
	/*!< DPSECI version */
	uint8_t num_of_priorities;
	/*!< number of priorities */
};

/**
 * @brief	Retrieve DPSECI attributes.
 *
 * @param[in]	mc_io	Pointer to MC portal's I/O object
 * @param[in]   token	Token of DPSECI object
 * @param[out]	attr	Object's attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpseci_get_attributes(struct fsl_mc_io	*mc_io,
			  uint16_t		token,
			  struct dpseci_attr	*attr);

/**
 * @brief	DPSECI destination types
 */
enum dpseci_dest {
	DPSECI_DEST_NONE,
	/*!< Unassigned destination; The queue is set in parked mode and does
	 * not generate FQDAN notifications; user is expected to dequeue from
	 * the queue based on polling or other user-defined method
	 */
	DPSECI_DEST_DPIO,
	/*!< The queue is set in schedule mode and generates FQDAN
	 * notifications to the specified DPIO; user is expected to dequeue
	 * from the queue only after notification is received
	 */
	DPSECI_DEST_DPCON
	/*!< The queue is set in schedule mode and does not generate FQDAN
	 * notifications, but is connected to the specified DPCON object;
	 * user is expected to dequeue from the DPCON channel
	 */
};

/**
 * @brief	Structure representing DPSECI destination parameters
 */
struct dpseci_dest_cfg {
	enum dpseci_dest dest_type;
	/*!< Destination type */
	int dest_id;
	/*!< Either DPIO ID or DPCON ID, depending on the destination type */
	uint8_t priority;
	/*!< Priority selection within the DPIO or DPCON channel; valid values
	 * are 0-1 or 0-7, depending on the number of priorities in that
	 * channel; not relevant for 'DPSECI_DEST_NONE' option
	 */
};

/*!
 * @name	DPSECI queue modification options
 */
#define DPSECI_QUEUE_OPT_USER_CTX	0x00000001
/*!< Select to modify the user's context associated with the queue */
#define DPSECI_QUEUE_OPT_DEST		0x00000002
/*!< Select to modify the queue's destination */
/* @} */

struct dpseci_rx_queue_cfg {
	uint32_t options;
	/*!< Flags representing the suggested modifications to the queue;
	 * Use any combination of 'DPSECI_QUEUE_OPT_<X>' flags
	 */
	uint64_t user_ctx;
	/*!< User context value provided in the frame descriptor of each
	 * dequeued frame;
	 * valid only if 'DPSECI_QUEUE_OPT_USER_CTX' is contained in 'options'
	 */
	struct dpseci_dest_cfg dest_cfg;
	/*!< Queue destination parameters;
	 * valid only if 'DPSECI_QUEUE_OPT_DEST' is contained in 'options'
	 */
};

/**
 * @brief	Set Rx queue configuration
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSECI object
 * @param[in]	priority	Select the queue relative to number of
 *				priorities configured at DPSECI creation; use
 *				DPSECI_ALL_QUEUES to configure all Rx queues
 *				identically.
 * @param[in]	cfg		Rx queue configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpseci_set_rx_queue(struct fsl_mc_io			*mc_io,
			uint16_t				token,
			uint8_t					priority,
			const struct dpseci_rx_queue_cfg	*cfg);

/**
 * @brief	Structure representing attributes of Rx queues
 */
struct dpseci_rx_queue_attr {
	uint64_t user_ctx;
	/*!< User context value provided in the frame descriptor of each
	 * dequeued frame
	 */
	struct dpseci_dest_cfg dest_cfg;
	/*!< Queue destination configuration */
	uint32_t fqid;
	/*!< Virtual FQID value to be used for dequeue operations */
};

/**
 * @brief	Retrieve Rx queue attributes.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSECI object
 * @param[in]   priority	Select the queue relative to number of
 *				priorities configured at DPSECI creation
 * @param[out]	attr		Rx queue attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpseci_get_rx_queue(struct fsl_mc_io		*mc_io,
			uint16_t			token,
			uint8_t				priority,
			struct dpseci_rx_queue_attr	*attr);

/**
 * @brief	Structure representing attributes of Tx queues
 */
struct dpseci_tx_queue_attr {
	uint32_t fqid;
	/*!< Virtual FQID to be used for sending frames to SEC hardware */
};

/**
 * @brief	Retrieve Tx queue attributes.
 *
 * @param[in]	mc_io		Pointer to MC portal's I/O object
 * @param[in]   token		Token of DPSECI object
 * @param[in]   priority	Select the queue relative to number of
 *				priorities configured at DPSECI creation
 * @param[out]	attr		Tx queue attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpseci_get_tx_queue(struct fsl_mc_io		*mc_io,
			uint16_t			token,
			uint8_t				priority,
			struct dpseci_tx_queue_attr	*attr);

/** @} */

#endif /* __FSL_DPSECI_H */

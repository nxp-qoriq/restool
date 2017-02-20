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
#ifndef __FSL_DPCI_H
#define __FSL_DPCI_H

/* Data Path Communication Interface API
 * Contains initialization APIs and runtime control APIs for DPCI
 */

struct fsl_mc_io;

/** General DPCI macros */

/**
 * Maximum number of Tx/Rx priorities per DPCI object
 */
#define DPCI_PRIO_NUM		2

/**
 * Indicates an invalid frame queue
 */
#define DPCI_FQID_NOT_VALID	(uint32_t)(-1)

/**
 * All queues considered; see dpci_set_rx_queue()
 */
#define DPCI_ALL_QUEUES		(uint8_t)(-1)

/**
 * dpci_open() - Open a control session for the specified object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @dpci_id:	DPCI unique ID
 * @token:	Returned token; use in subsequent API calls
 *
 * This function can be used to open a control session for an
 * already created object; an object may have been declared in
 * the DPL or by calling the dpci_create() function.
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent commands for
 * this specific object.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpci_open(struct fsl_mc_io *mc_io,
	      uint32_t		cmd_flags,
	      int		dpci_id,
	      uint16_t		*token);

/**
 * dpci_close() - Close the control session of the object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPCI object
 *
 * After this function is called, no further operations are
 * allowed on the object without opening a new control session.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpci_close(struct fsl_mc_io *mc_io,
	       uint32_t	cmd_flags,
	       uint16_t	token);

/**
 * struct dpci_cfg - Structure representing DPCI configuration
 * @num_of_priorities:	Number of receive priorities (queues) for the DPCI;
 *			note, that the number of transmit priorities (queues)
 *			is determined by the number of receive priorities of
 *			the peer DPCI object
 */
struct dpci_cfg {
	uint8_t num_of_priorities;
};

/**
 * dpci_create() - Create the DPCI object.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @cfg:	Configuration structure
 * @token:	Returned token; use in subsequent API calls
 *
 * Create the DPCI object, allocate required resources and perform required
 * initialization.
 *
 * The object can be created either by declaring it in the
 * DPL file, or by calling this function.
 *
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent calls to
 * this specific object. For objects that are created using the
 * DPL file, call dpci_open() function to get an authentication
 * token first.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpci_create(struct fsl_mc_io	*mc_io,
		uint32_t		cmd_flags,
		const struct dpci_cfg	*cfg,
		uint16_t		*token);

/**
 * dpci_destroy() - Destroy the DPCI object and release all its resources.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPCI object
 *
 * Return:	'0' on Success; error code otherwise.
 */
int dpci_destroy(struct fsl_mc_io	*mc_io,
		 uint32_t		cmd_flags,
		 uint16_t		token);

/** DPCI IRQ Index and Events */

/**
 * IRQ index
 */
#define DPCI_IRQ_INDEX				0

/**
 * IRQ event - indicates a change in link state
 */
#define DPCI_IRQ_EVENT_LINK_CHANGED		0x00000001
/**
 * IRQ event - indicates a connection event
 */
#define DPCI_IRQ_EVENT_CONNECTED                0x00000002
/**
 * IRQ event - indicates a disconnection event
 */
#define DPCI_IRQ_EVENT_DISCONNECTED             0x00000004

/**
 * struct dpci_irq_cfg - IRQ configuration
 * @addr:	Address that must be written to signal a message-based interrupt
 * @val:	Value to write into irq_addr address
 * @irq_num: A user defined number associated with this IRQ
 */
struct dpci_irq_cfg {
	     uint64_t		addr;
	     uint32_t		val;
	     int		irq_num;
};

/**
 * dpci_get_irq_mask() - Get interrupt mask.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPCI object
 * @irq_index:	The interrupt index to configure
 * @mask:	Returned event mask to trigger interrupt
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpci_get_irq_mask(struct fsl_mc_io *mc_io,
		      uint32_t		cmd_flags,
		      uint16_t		token,
		      uint8_t		irq_index,
		      uint32_t		*mask);

/**
 * dpci_get_irq_status() - Get the current status of any pending interrupts.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPCI object
 * @irq_index:	The interrupt index to configure
 * @status:	Returned interrupts status - one bit per cause:
 *					0 = no interrupt pending
 *					1 = interrupt pending
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpci_get_irq_status(struct fsl_mc_io	*mc_io,
			uint32_t		cmd_flags,
			uint16_t		token,
			uint8_t			irq_index,
			uint32_t		*status);

/**
 * struct dpci_attr - Structure representing DPCI attributes
 * @id:		DPCI object ID
 * @version:	DPCI version
 * @num_of_priorities:	Number of receive priorities
 */
struct dpci_attr {
	int id;
	/**
	 * struct version - Structure representing DPCI attributes
	 * @major:	DPCI major version
	 * @minor:	DPCI minor version
	 */
	struct {
		uint16_t major;
		uint16_t minor;
	} version;
	uint8_t num_of_priorities;
};

/**
 * dpci_get_attributes() - Retrieve DPCI attributes.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPCI object
 * @attr:	Returned object's attributes
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpci_get_attributes(struct fsl_mc_io	*mc_io,
			uint32_t		cmd_flags,
			uint16_t		token,
			struct dpci_attr	*attr);

/**
 * struct dpci_peer_attr - Structure representing the peer DPCI attributes
 * @peer_id:	DPCI peer id; if no peer is connected returns (-1)
 * @num_of_priorities:	The pper's number of receive priorities; determines the
 *			number of transmit priorities for the local DPCI object
 */
struct dpci_peer_attr {
	int peer_id;
	uint8_t num_of_priorities;
};

/**
 * dpci_get_peer_attributes() - Retrieve peer DPCI attributes.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPCI object
 * @attr:	Returned peer attributes
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpci_get_peer_attributes(struct fsl_mc_io		*mc_io,
			     uint32_t			cmd_flags,
			     uint16_t			token,
			     struct dpci_peer_attr	*attr);

/**
 * dpci_get_link_state() - Retrieve the DPCI link state.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPCI object
 * @up:		Returned link state; returns '1' if link is up, '0' otherwise
 *
 * DPCI can be connected to another DPCI, together they
 * create a 'link'. In order to use the DPCI Tx and Rx queues,
 * both objects must be enabled.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpci_get_link_state(struct fsl_mc_io *mc_io,
			uint32_t	cmd_flags,
			uint16_t	token,
			int		*up);
#endif /* __FSL_DPCI_H */

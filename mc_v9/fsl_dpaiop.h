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
#ifndef __FSL_DPAIOP_H
#define __FSL_DPAIOP_H

struct fsl_mc_io;

/* Data Path AIOP API
 * Contains initialization APIs and runtime control APIs for DPAIOP
 */

/**
 * dpaiop_open() - Open a control session for the specified object.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @dpaiop_id:	DPAIOP unique ID
 * @token:	Returned token; use in subsequent API calls
 *
 * This function can be used to open a control session for an
 * already created object; an object may have been declared in
 * the DPL or by calling the dpaiop_create function.
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent commands for
 * this specific object
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpaiop_open(struct fsl_mc_io *mc_io,
		uint32_t cmd_flags,
		int dpaiop_id,
		uint16_t *token);

/**
 * dpaiop_close() - Close the control session of the object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPAIOP object
 *
 * After this function is called, no further operations are
 * allowed on the object without opening a new control session.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpaiop_close(struct fsl_mc_io *mc_io, uint32_t cmd_flags, uint16_t token);

/**
 * struct dpaiop_cfg - Structure representing DPAIOP configuration
 * @aiop_id:		AIOP ID
 * @aiop_container_id:	AIOP container ID
 */
struct dpaiop_cfg {
	int aiop_id;
	int aiop_container_id;
};

/**
 * dpaiop_create() - Create the DPAIOP object.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @cfg:	Configuration structure
 * @token:	Returned token; use in subsequent API calls
 *
 * Create the DPAIOP object, allocate required resources and
 * perform required initialization.
 *
 * The object can be created either by declaring it in the
 * DPL file, or by calling this function.
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent calls to
 * this specific object. For objects that are created using the
 * DPL file, call dpaiop_open function to get an authentication
 * token first.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpaiop_create(struct fsl_mc_io	*mc_io,
		  uint32_t		cmd_flags,
		  const struct dpaiop_cfg	*cfg,
		uint16_t		*token);

/**
 * dpaiop_destroy() - Destroy the DPAIOP object and release all its resources.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPAIOP object
 *
 * Return:	'0' on Success; error code otherwise.
 */
int dpaiop_destroy(struct fsl_mc_io *mc_io, uint32_t cmd_flags, uint16_t token);

/**
 * dpaiop_get_irq_mask() - Get interrupt mask.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPAIOP object
 * @irq_index:	The interrupt index to configure
 * @mask:	Returned event mask to trigger interrupt
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpaiop_get_irq_mask(struct fsl_mc_io	*mc_io,
			uint32_t		cmd_flags,
			uint16_t		token,
			uint8_t			irq_index,
			uint32_t		*mask);

/**
 * dpaiop_get_irq_status() - Get the current status of any pending interrupts.
 *
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPAIOP object
 * @irq_index:	The interrupt index to configure
 * @status:	Returned interrupts status - one bit per cause:
 *			0 = no interrupt pending
 *			1 = interrupt pending
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpaiop_get_irq_status(struct fsl_mc_io	*mc_io,
			  uint32_t		cmd_flags,
			  uint16_t		token,
			  uint8_t		irq_index,
			  uint32_t		*status);

/**
 * struct dpaiop_attr - Structure representing DPAIOP attributes
 * @id:	AIOP ID
 * @version:	DPAIOP version
 */
struct dpaiop_attr {
	int id;
	/**
	 * struct version - Structure representing DPAIOP version
	 * @major:	DPAIOP major version
	 * @minor:	DPAIOP minor version
	 */
	struct {
		uint16_t major;
		uint16_t minor;
	} version;
};

/**
 * dpaiop_get_attributes - Retrieve DPAIOP attributes.
 *
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPAIOP object
 * @attr:	Returned object's attributes
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpaiop_get_attributes(struct fsl_mc_io	*mc_io,
			  uint32_t		cmd_flags,
			  uint16_t		token,
			  struct dpaiop_attr	*attr);

/**
 * struct dpaiop_sl_version - AIOP SL (Service Layer) version
 * @major: AIOP SL major version number
 * @minor: AIOP SL minor version number
 * @revision: AIOP SL revision number
 */
struct dpaiop_sl_version {
	uint32_t major;
	uint32_t minor;
	uint32_t revision;
};

/**
 * dpaiop_get_sl_version() - Get AIOP SL (Service Layer) version
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPAIOP object
 * @version:	AIOP SL version number
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpaiop_get_sl_version(struct fsl_mc_io		*mc_io,
			  uint32_t			cmd_flags,
			  uint16_t			token,
			  struct dpaiop_sl_version	*version);

/**
 * AIOP states
 *
 * AIOP internal states, can be retrieved by calling dpaiop_get_state() routine
 */

/**
 * AIOP reset successfully completed.
 */
#define DPAIOP_STATE_RESET_DONE      0x00000000
/**
 * AIOP reset is ongoing.
 */
#define DPAIOP_STATE_RESET_ONGOING   0x00000001

/**
 * AIOP image loading successfully completed.
 */
#define DPAIOP_STATE_LOAD_DONE       0x00000002
/**
 * AIOP image loading is ongoing.
 */
#define DPAIOP_STATE_LOAD_ONGIONG    0x00000004
/**
 * AIOP image loading completed with error.
 */
#define DPAIOP_STATE_LOAD_ERROR      0x00000008

/**
 * Boot process of AIOP cores is ongoing.
 */
#define DPAIOP_STATE_BOOT_ONGOING    0x00000010
/**
 * Boot process of AIOP cores completed with an error.
 */
#define DPAIOP_STATE_BOOT_ERROR      0x00000020
/**
 * AIOP cores are functional and running
 */
#define DPAIOP_STATE_RUNNING         0x00000040
/** @} */

/**
 * dpaiop_get_state() - Get AIOP state
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPAIOP object
 * @state:	AIOP state
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpaiop_get_state(struct fsl_mc_io	*mc_io,
		     uint32_t		cmd_flags,
		     uint16_t		token,
		     uint32_t		*state);

#endif /* __FSL_DPAIOP_H */

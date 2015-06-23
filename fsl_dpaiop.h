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

/* Data Path Buffer Pool API
 * Contains initialization APIs and runtime control APIs for DPAIOP
 */

struct fsl_mc_io;

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
 * dpaiop_reset() - Reset the DPAIOP, returns the object to initial state.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPAIOP object
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpaiop_reset(struct fsl_mc_io *mc_io, uint32_t cmd_flags, uint16_t token);

/**
 * struct dpaiop_irq_cfg - IRQ configuration
 * @addr:	Address that must be written to signal a message-based interrupt
 * @val:	Value to write into irq_addr address
 * @user_irq_id: A user defined number associated with this IRQ
 */
struct dpaiop_irq_cfg {
	     uint64_t		addr;
	     uint32_t		val;
	     int		user_irq_id;
};

/**
 * dpaiop_set_irq() - Set IRQ information for the DPAIOP to trigger an interrupt.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPAIOP object
 * @irq_index:	Identifies the interrupt index to configure
 * @irq_cfg:	IRQ configuration
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpaiop_set_irq(struct fsl_mc_io		*mc_io,
		   uint32_t			cmd_flags,
		   uint16_t			token,
		   uint8_t			irq_index,
		   struct dpaiop_irq_cfg	*irq_cfg);

/**
 * dpaiop_get_irq() - Get IRQ information from the DPAIOP.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPAIOP object
 * @irq_index:	The interrupt index to configure
 * @type:	Interrupt type: 0 represents message interrupt
 *		type (both irq_addr and irq_val are valid)
 * @irq_cfg:	IRQ attributes
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpaiop_get_irq(struct fsl_mc_io		*mc_io,
		   uint32_t			cmd_flags,
		   uint16_t			token,
		   uint8_t			irq_index,
		   int				*type,
		   struct dpaiop_irq_cfg	*irq_cfg);

/**
 * dpaiop_set_irq_enable() - Set overall interrupt state.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPAIOP object
 * @irq_index:	The interrupt index to configure
 * @en:	Interrupt state - enable = 1, disable = 0
 *
 * Allows GPP software to control when interrupts are generated.
 * Each interrupt can have up to 32 causes.  The enable/disable control's the
 * overall interrupt state. if the interrupt is disabled no causes will cause
 * an interrupt.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpaiop_set_irq_enable(struct fsl_mc_io	*mc_io,
			  uint32_t		cmd_flags,
			  uint16_t		token,
			  uint8_t		irq_index,
			  uint8_t		en);

/**
 * dpaiop_get_irq_enable() - Get overall interrupt state
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPAIOP object
 * @irq_index:	The interrupt index to configure
 * @en:		Returned interrupt state - enable = 1, disable = 0
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpaiop_get_irq_enable(struct fsl_mc_io	*mc_io,
			  uint32_t		cmd_flags,
			  uint16_t		token,
			  uint8_t		irq_index,
			  uint8_t		*en);

/**
 * dpaiop_set_irq_mask() - Set interrupt mask.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPAIOP object
 * @irq_index:	The interrupt index to configure
 * @mask:	Event mask to trigger interrupt;
 *			each bit:
 *				0 = ignore event
 *				1 = consider event for asserting IRQ
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpaiop_set_irq_mask(struct fsl_mc_io	*mc_io,
			uint32_t		cmd_flags,
			uint16_t		token,
			uint8_t			irq_index,
			uint32_t		mask);

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
 * dpaiop_clear_irq_status() - Clear a pending interrupt's status
 *
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPAIOP object
 * @irq_index:	The interrupt index to configure
 * @status:	Bits to clear (W1C) - one bit per cause:
 *					0 = don't change
 *					1 = clear status bit
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpaiop_clear_irq_status(struct fsl_mc_io	*mc_io,
			    uint32_t		cmd_flags,
			    uint16_t		token,
			    uint8_t		irq_index,
			    uint32_t		status);

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
 * struct dpaiop_load_cfg - AIOP load configuration
 * @options: AIOP load options
 * @img_iova:	I/O virtual address of AIOP ELF image
 * @img_size:	Size of AIOP ELF image in memory (in bytes)
 */
struct dpaiop_load_cfg {
	uint64_t options;
	uint64_t img_iova;
	uint32_t img_size;
};

/**
 * dpaiop_load_aiop() - Loads an image to AIOP tile
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPAIOP object
 * @cfg:	AIOP load configurations
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpaiop_load(struct fsl_mc_io *mc_io,
		uint32_t cmd_flags,
		uint16_t token,
		struct dpaiop_load_cfg *cfg);

#define DPAIOP_RUN_OPT_DEBUG                    0x0000000000000001ULL

/**
 * struct dpaiop_run_cfg - AIOP run configuration
 * @cores_mask: Mask of AIOP cores to run (core 0 in most significant bit)
 * @options: Execution options (currently none defined)
 * @args_iova:	I/O virtual address of AIOP arguments
 * @args_size:	Size of AIOP arguments in memory (in bytes)
 */
struct dpaiop_run_cfg {
	uint64_t cores_mask;
	uint64_t options;
	uint64_t args_iova;
	uint32_t args_size;
};

/**
 * dpaiop_run_aiop() - Starts AIOP tile execution
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPAIOP object
 * @cfg:	AIOP run configuration
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpaiop_run(struct fsl_mc_io			*mc_io,
	       uint32_t				cmd_flags,
	       uint16_t				token,
	       const struct dpaiop_run_cfg	*cfg);

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

#define DPAIOP_STATE_RESET_DONE      0x00000000
#define DPAIOP_STATE_RESET_ONGOING   0x00000001

#define DPAIOP_STATE_LOAD_DONE       0x00000002
#define DPAIOP_STATE_LOAD_ONGIONG    0x00000004
#define DPAIOP_STATE_LOAD_ERROR      0x00000008

#define DPAIOP_STATE_BOOT_ONGOING    0x00000010
#define DPAIOP_STATE_BOOT_ERROR      0x00000020
#define DPAIOP_STATE_RUNNING         0x00000040

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

/**
 * dpaiop_set_time_of_day() - Set AIOP internal time-of-day
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:  Token of DPAIOP object
 * @time_of_day:  Current number of milliseconds since the Epoch
 *
 * Return:  '0' on Success; Error code otherwise.
 */
int dpaiop_set_time_of_day(struct fsl_mc_io *mc_io,
			   uint32_t cmd_flags,
			   uint16_t token,
			   uint64_t time_of_day);

/**
 * dpaiop_get_time_of_day() - Get AIOP internal time-of-day
 * @mc_io:  Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:  Token of DPAIOP object
 * @time_of_day:  Current number of milliseconds since the Epoch
 *
 * Return:  '0' on Success; Error code otherwise.
 */
int dpaiop_get_time_of_day(struct fsl_mc_io *mc_io,
			   uint32_t cmd_flags,
			   uint16_t token,
			   uint64_t *time_of_day);

#endif /* __FSL_DPAIOP_H */

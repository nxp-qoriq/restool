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
#ifndef __FSL_DPDCEI_H
#define __FSL_DPDCEI_H

/* Data Path DCE Interface API
 * Contains initialization APIs and runtime control APIs for DPDCEI
 */

struct fsl_mc_io;

/** General DPDCEI macros */

/**
 * Indicates an invalid frame queue
 */
#define DPDCEI_FQID_NOT_VALID	(uint32_t)(-1)

/**
 * enum dpdcei_engine - DCE engine block
 * @DPDCEI_ENGINE_COMPRESSION: Engine compression
 * @DPDCEI_ENGINE_DECOMPRESSION: Engine decompression
 */
enum dpdcei_engine {
	DPDCEI_ENGINE_COMPRESSION,
	DPDCEI_ENGINE_DECOMPRESSION
};

/**
 * dpdcei_open() - Open a control session for the specified object
 * @mc_io: Pointer to MC portal's I/O object
 * @cmd_flags: Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDCEI object
 * @dpdcei_id: DPDCEI unique ID
 *
 * This function can be used to open a control session for an
 * already created object; an object may have been declared in
 * the DPL or by calling the dpdcei_create() function.
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent commands for
 * this specific object.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdcei_open(struct fsl_mc_io	*mc_io,
		uint32_t		cmd_flags,
		int			dpdcei_id,
		uint16_t		*token);

/**
 * dpdcei_close() - Close the control session of the object
 * @mc_io: Pointer to MC portal's I/O object
 * @cmd_flags: Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDCEI object
 *
 * After this function is called, no further operations are
 * allowed on the object without opening a new control session.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdcei_close(struct fsl_mc_io	*mc_io,
		 uint32_t		cmd_flags,
		 uint16_t		token);

/**
 * struct dpdcei_cfg - Structure representing DPDCEI configuration
 * @engine: compression or decompression engine to be selected
 * @priority: Priority for the DCE hardware processing (valid values 1-8).
 */
struct dpdcei_cfg {
	enum dpdcei_engine	engine;
	uint8_t		priority;
};

/**
 * dpdcei_create() - Create the DPDCEI object
 * @mc_io: Pointer to MC portal's I/O object
 * @cmd_flags: Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDCEI object
 * @cfg: configuration parameters
 *
 * Create the DPDCEI object, allocate required resources and
 * perform required initialization.
 *
 * The object can be created either by declaring it in the
 * DPL file, or by calling this function.
 *
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent calls to
 * this specific object. For objects that are created using the
 * DPL file, call dpdcei_open() function to get an authentication
 * token first.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdcei_create(struct fsl_mc_io		*mc_io,
		  uint32_t			cmd_flags,
		  const struct dpdcei_cfg	*cfg,
		  uint16_t			*token);

/**
 * dpdcei_destroy() - Destroy the DPDCEI object and release all its resources.
 * @mc_io: Pointer to MC portal's I/O object
 * @cmd_flags: Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDCEI object
 *
 * Return:	'0' on Success; error code otherwise.
 */
int dpdcei_destroy(struct fsl_mc_io	*mc_io,
		   uint32_t		cmd_flags,
		   uint16_t		token);

/**
 * dpdcei_get_irq_mask() - Get interrupt mask.
 * @mc_io: Pointer to MC portal's I/O object
 * @cmd_flags: Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:		Token of DPDCEI object
 * @irq_index:	The interrupt index to configure
 * @mask:		Returned event mask to trigger interrupt
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdcei_get_irq_mask(struct fsl_mc_io	*mc_io,
			uint32_t		cmd_flags,
			uint16_t		token,
			uint8_t		irq_index,
			uint32_t		*mask);

/**
 * dpdcei_get_irq_status() - Get the current status of any pending interrupts
 * @mc_io: Pointer to MC portal's I/O object
 * @cmd_flags: Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:		Token of DPDCEI object
 * @irq_index:	The interrupt index to configure
 * @status:		Returned interrupts status - one bit per cause:
 *					0 = no interrupt pending
 *					1 = interrupt pending
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdcei_get_irq_status(struct fsl_mc_io	*mc_io,
			  uint32_t		cmd_flags,
			  uint16_t		token,
			  uint8_t		irq_index,
			  uint32_t		*status);

/**
 * struct dpdcei_attr - Structure representing DPDCEI attributes
 * @id: DPDCEI object ID
 * @engine: DCE engine block
 * @version: DPDCEI version
 */
struct dpdcei_attr {
	int id;
	enum dpdcei_engine engine;
	/**
	 * struct version - DPDCEI version
	 * @major: DPDCEI major version
	 * @minor: DPDCEI minor version
	 */
	struct {
		uint16_t major;
		uint16_t minor;
	} version;
};

/**
 * dpdcei_get_attributes() - Retrieve DPDCEI attributes.
 * @mc_io: Pointer to MC portal's I/O object
 * @cmd_flags: Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDCEI object
 * @attr: Returned  object's attributes
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdcei_get_attributes(struct fsl_mc_io	*mc_io,
			  uint32_t		cmd_flags,
			  uint16_t		token,
			  struct dpdcei_attr	*attr);

#endif /* __FSL_DPDCEI_H */

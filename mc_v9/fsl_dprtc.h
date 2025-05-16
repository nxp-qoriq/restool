/* Copyright 2013-2016 Freescale Semiconductor Inc.
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
#ifndef __FSL_DPRTC_H
#define __FSL_DPRTC_H

/* Data Path Real Time Counter API
 * Contains initialization APIs and runtime control APIs for RTC
 */

struct fsl_mc_io;

/**
 * dprtc_open() - Open a control session for the specified object.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @dprtc_id:	DPRTC unique ID
 * @token:	Returned token; use in subsequent API calls
 *
 * This function can be used to open a control session for an
 * already created object; an object may have been declared in
 * the DPL or by calling the dprtc_create function.
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent commands for
 * this specific object
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dprtc_open(struct fsl_mc_io	*mc_io,
	       uint32_t		cmd_flags,
	      int		dprtc_id,
	      uint16_t		*token);

/**
 * dprtc_close() - Close the control session of the object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPRTC object
 *
 * After this function is called, no further operations are
 * allowed on the object without opening a new control session.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dprtc_close(struct fsl_mc_io	*mc_io,
		uint32_t		cmd_flags,
	       uint16_t	token);

/**
 * struct dprtc_cfg - Structure representing DPRTC configuration
 * @options:	place holder
 */
struct dprtc_cfg {
	uint32_t options;
};

/**
 * dprtc_create() - Create the DPRTC object.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @cfg:	Configuration structure
 * @token:	Returned token; use in subsequent API calls
 *
 * Create the DPRTC object, allocate required resources and
 * perform required initialization.
 *
 * The object can be created either by declaring it in the
 * DPL file, or by calling this function.
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent calls to
 * this specific object. For objects that are created using the
 * DPL file, call dprtc_open function to get an authentication
 * token first.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dprtc_create(struct fsl_mc_io	*mc_io,
		 uint32_t		cmd_flags,
		const struct dprtc_cfg	*cfg,
		uint16_t		*token);

/**
 * dprtc_destroy() - Destroy the DPRTC object and release all its resources.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPRTC object
 *
 * Return:	'0' on Success; error code otherwise.
 */
int dprtc_destroy(struct fsl_mc_io	*mc_io,
		  uint32_t		cmd_flags,
		 uint16_t		token);

/**
 * struct dprtc_attr - Structure representing DPRTC attributes
 * @id:		DPRTC object ID
 * @version:	DPRTC version
 */
struct dprtc_attr {
	int id;
	/**
	 * struct version - Structure representing DPRTC version
	 * @major:	DPRTC major version
	 * @minor:	DPRTC minor version
	 */
	struct {
		uint16_t major;
		uint16_t minor;
	} version;
};

/**
 * dprtc_get_attributes - Retrieve DPRTC attributes.
 *
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPRTC object
 * @attr:	Returned object's attributes
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dprtc_get_attributes(struct fsl_mc_io	*mc_io,
			 uint32_t	cmd_flags,
			uint16_t		token,
			struct dprtc_attr	*attr);

#endif /* __FSL_DPRTC_H */

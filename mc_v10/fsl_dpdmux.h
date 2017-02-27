/* Copyright 2013-2017 Freescale Semiconductor Inc.
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
#ifndef __FSL_DPDMUX_V10_H
#define __FSL_DPDMUX_V10_H

#include "../mc_v9/fsl_dpdmux.h"

struct fsl_mc_io;

/* Data Path Demux API
 * Contains API for handling DPDMUX topology and functionality
 */

/**
 * dpdmux_create() - Create the DPDMUX object
 * @mc_io:	Pointer to MC portal's I/O object
 * @dprc_token:	Parent container token; '0' for default container
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @cfg:	Configuration structure
 * @obj_id:	Returned object id
 *
 * Create the DPDMUX object, allocate required resources and
 * perform required initialization.
 *
 * The object can be created either by declaring it in the
 * DPL file, or by calling this function.
 *
 * The function accepts an authentication token of a parent
 * container that this object should be assigned to. The token
 * can be '0' so the object will be assigned to the default container.
 * The newly created object can be opened with the returned
 * object id and using the container's associated tokens and MC portals.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdmux_create_v10(struct fsl_mc_io *mc_io,
		      uint16_t dprc_token,
		      uint32_t cmd_flags,
		      const struct dpdmux_cfg_v9 *cfg,
		      uint32_t *obj_id);

/**
 * dpdmux_destroy() - Destroy the DPDMUX object and release all its resources.
 * @mc_io:	Pointer to MC portal's I/O object
 * @dprc_token: Parent container token; '0' for default container
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @object_id:	The object id; it must be a valid id within the container that
 * created this object;
 *
 * The function accepts the authentication token of the parent container that
 * created the object (not the one that currently owns the object). The object
 * is searched within parent using the provided 'object_id'.
 * All tokens to the object must be closed before calling destroy.
 *
 * Return:	'0' on Success; error code otherwise.
 */
int dpdmux_destroy_v10(struct fsl_mc_io *mc_io,
		       uint16_t dprc_token,
		       uint32_t cmd_flags,
		       uint32_t object_id);

/**
 * struct dpdmux_attr - Structure representing DPDMUX attributes
 * @id:		DPDMUX object ID
 * @version:	DPDMUX version
 * @options:	Configuration options (bitmap)
 * @method:	DPDMUX address table method
 * @manip:	DPDMUX manipulation type
 * @num_ifs:	Number of interfaces (excluding the uplink interface)
 * @mem_size:	DPDMUX frame storage memory size
 */
struct dpdmux_attr_v10 {
	int id;
	uint64_t options;
	enum dpdmux_method method;
	enum dpdmux_manip manip;
	uint16_t num_ifs;
	uint16_t mem_size;
};

/**
 * dpdmux_get_attributes() - Retrieve DPDMUX attributes
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDMUX object
 * @attr:	Returned object's attributes
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdmux_get_attributes_v10(struct fsl_mc_io *mc_io,
			      uint32_t cmd_flags,
			      uint16_t token,
			      struct dpdmux_attr_v10 *attr);

/**
 * dpaiop_get_version() - Get Data Path Demux version
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @majorVer:	Major version of data path demux object
 * @minorVer:	Minor version of data path demux object
 *
 * Return:  '0' on Success; Error code otherwise.
 */
int dpdmux_get_version_v10(struct fsl_mc_io *mc_io,
			   uint32_t cmd_flags,
			   uint16_t *majorVer,
			   uint16_t *minorVer);

#endif /* __FSL_DPDMUX_H */

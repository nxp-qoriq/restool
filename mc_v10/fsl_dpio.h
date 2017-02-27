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
#ifndef __FSL_DPIO_V10_H
#define __FSL_DPIO_V10_H

#include "../mc_v9/fsl_dpio.h"

/* Data Path I/O Portal API
 * Contains initialization APIs and runtime control APIs for DPIO
 */

struct fsl_mc_io;

/**
 * dpio_create() - Create the DPIO object.
 * @mc_io:	Pointer to MC portal's I/O object
 * @dprc_token:	Parent container token; '0' for default container
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @cfg:	Configuration structure
 * @obj_id:	Returned object id
 *
 * Create the DPIO object, allocate required resources and
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
int dpio_create_v10(struct fsl_mc_io *mc_io,
		    uint16_t dprc_token,
		    uint32_t cmd_flags,
		    const struct dpio_cfg *cfg,
		    uint32_t *obj_id);

/**
 * dpio_destroy() - Destroy the DPIO object and release all its resources.
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
 * Return:	'0' on Success; Error code otherwise
 */
int dpio_destroy_v10(struct fsl_mc_io *mc_io,
		     uint16_t dprc_token,
		     uint32_t cmd_flags,
		     uint32_t object_id);

/**
 * struct dpio_attr - Structure representing DPIO attributes
 * @id:			DPIO object ID
 * @version:		DPIO version
 * @qbman_portal_ce_offset: offset of the software portal cache-enabled area
 * @qbman_portal_ci_offset: offset of the software portal cache-inhibited area
 * @qbman_portal_id:	Software portal ID
 * @channel_mode:	Notification channel mode
 * @num_priorities:	Number of priorities for the notification channel (1-8);
 *			relevant only if 'channel_mode = DPIO_LOCAL_CHANNEL'
 * @qbman_version: QBMAN version
 */
struct dpio_attr_v10 {
	int id;
	uint64_t qbman_portal_ce_offset;
	uint64_t qbman_portal_ci_offset;
	uint16_t qbman_portal_id;
	enum dpio_channel_mode channel_mode;
	uint8_t num_priorities;
	uint32_t qbman_version;
	uint32_t clk;
};

/**
 * dpio_get_attributes() - Retrieve DPIO attributes
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPIO object
 * @attr:	Returned object's attributes
 *
 * Return:	'0' on Success; Error code otherwise
 */
int dpio_get_attributes_v10(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    struct dpio_attr_v10 *attr);

/**
 * dpio_get_version() - Get Data Path I/O version
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @majorVer:	Major version of data path i/o object
 * @minorVer:	Minor version of data path i/o object
 *
 * Return:  '0' on Success; Error code otherwise.
 */
int dpio_get_version_v10(struct fsl_mc_io *mc_io,
			 uint32_t cmd_flags,
			 uint16_t *majorVer,
			 uint16_t *minorVer);

#endif /* __FSL_DPIO_V10_H */

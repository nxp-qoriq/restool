/* Copyright 2013-2016 Freescale Semiconductor Inc.
 * Copyright 2017-2018 NXP
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
#ifndef __FSL_DPAIOP_V10_H
#define __FSL_DPAIOP_V10_H

struct fsl_mc_io;

/* Data Path AIOP API
 * Contains initialization APIs and runtime control APIs for DPAIOP
 */

int dpaiop_open_v10(struct fsl_mc_io *mc_io,
		    uint32_t cmd_flags,
		    int dpaiop_id,
		    uint16_t *token);

int dpaiop_close_v10(struct fsl_mc_io *mc_io,
		     uint32_t cmd_flags,
		     uint16_t token);

/**
 * struct dpaiop_cfg_v10 - Structure representing DPAIOP configuration
 * @aiop_id:		AIOP ID
 * @aiop_container_id:	AIOP container ID
 */
struct dpaiop_cfg_v10 {
	int aiop_id;
	int aiop_container_id;
};

int dpaiop_create_v10(struct fsl_mc_io *mc_io,
		      uint16_t dprc_token,
		      uint32_t cmd_flags,
		      const struct dpaiop_cfg_v10 *cfg,
		      uint32_t *obj_id);

int dpaiop_destroy_v10(struct fsl_mc_io *mc_io,
		       uint16_t dprc_token,
		       uint32_t cmd_flags,
		       uint32_t object_id);

int dpaiop_get_irq_mask_v10(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    uint8_t irq_index,
			    uint32_t *mask);

int dpaiop_get_irq_status_v10(struct fsl_mc_io *mc_io,
			      uint32_t cmd_flags,
			      uint16_t token,
			      uint8_t irq_index,
			      uint32_t *status);
/**
 * struct dpaiop_attr_v10 - Structure representing DPAIOP attributes
 * @id:	AIOP ID
 */
struct dpaiop_attr_v10 {
	int id;
};

int dpaiop_get_attributes_v10(struct fsl_mc_io *mc_io,
			      uint32_t cmd_flags,
			      uint16_t token,
			      struct dpaiop_attr_v10 *attr);

/**
 * struct dpaiop_sl_version - AIOP SL (Service Layer) version
 * @major:	AIOP SL major version number
 * @minor:	AIOP SL minor version number
 * @revision:	AIOP SL revision number
 */
struct dpaiop_sl_version_v10 {
	uint32_t major;
	uint32_t minor;
	uint32_t revision;
};

int dpaiop_get_sl_version_v10(struct fsl_mc_io *mc_io,
			      uint32_t cmd_flags,
			      uint16_t token,
			      struct dpaiop_sl_version_v10 *version);

/**
 * AIOP states
 *
 * AIOP internal states, can be retrieved by calling dpaiop_get_state() routine
 */

/**
 * AIOP reset successfully completed.
 */
#define DPAIOP_STATE_RESET_DONE		0x00000000
/**
 * AIOP reset is ongoing.
 */
#define DPAIOP_STATE_RESET_ONGOING	0x00000001

/**
 * AIOP image loading successfully completed.
 */
#define DPAIOP_STATE_LOAD_DONE		0x00000002
/**
 * AIOP image loading is ongoing.
 */
#define DPAIOP_STATE_LOAD_ONGIONG	0x00000004
/**
 * AIOP image loading completed with error.
 */
#define DPAIOP_STATE_LOAD_ERROR		0x00000008

/**
 * Boot process of AIOP cores is ongoing.
 */
#define DPAIOP_STATE_BOOT_ONGOING	0x00000010
/**
 * Boot process of AIOP cores completed with an error.
 */
#define DPAIOP_STATE_BOOT_ERROR		0x00000020
/**
 * AIOP cores are functional and running
 */
#define DPAIOP_STATE_RUNNING		0x00000040
/** @} */

int dpaiop_get_state_v10(struct fsl_mc_io *mc_io,
			 uint32_t cmd_flags,
			 uint16_t token,
			 uint32_t *state);

int dpaiop_get_api_version_v10(struct fsl_mc_io *mc_io,
			       uint32_t cmd_flags,
			       uint16_t *major_ver,
			       uint16_t *minor_ver);

#endif /* __FSL_DPAIOP_V10_H */

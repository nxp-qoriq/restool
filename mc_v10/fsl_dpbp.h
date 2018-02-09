/* Copyright 2013-2016 Freescale Semiconductor Inc.
 * Copyright 2017 NXP
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
#ifndef __FSL_DPBP_V10_H
#define __FSL_DPBP_V10_H

/*
 * Data Path Buffer Pool API
 * Contains initialization APIs and runtime control APIs for DPBP
 */

struct fsl_mc_io;

int dpbp_open_v10(struct fsl_mc_io *mc_io,
		  uint32_t cmd_flags,
		  int dpbp_id,
		  uint16_t *token);

int dpbp_close_v10(struct fsl_mc_io *mc_io,
		   uint32_t cmd_flags,
		   uint16_t token);

/**
 * struct dpbp_cfg - Structure representing DPBP configuration
 * @options:	place holder
 */
struct dpbp_cfg_v10 {
	uint32_t options;
};

int dpbp_create_v10(struct fsl_mc_io *mc_io,
		    uint16_t dprc_token,
		    uint32_t cmd_flags,
		    const struct dpbp_cfg_v10 *cfg,
		    uint32_t *obj_id);

int dpbp_destroy_v10(struct fsl_mc_io *mc_io,
		     uint16_t dprc_token,
		     uint32_t cmd_flags,
		     uint32_t obj_id);

int dpbp_get_irq_mask_v10(struct fsl_mc_io *mc_io,
			  uint32_t cmd_flags,
			  uint16_t token,
			  uint8_t irq_index,
			  uint32_t *mask);

int dpbp_get_irq_status_v10(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    uint8_t irq_index,
			    uint32_t *status);
/**
 * struct dpbp_attr - Structure representing DPBP attributes
 * @id:		DPBP object ID
 * @bpid:	Hardware buffer pool ID; should be used as an argument in
 *		acquire/release operations on buffers
 */
struct dpbp_attr_v10 {
	int id;
	uint16_t bpid;
};

int dpbp_get_attributes_v10(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    struct dpbp_attr_v10 *attr);

int dpbp_get_api_version_v10(struct fsl_mc_io *mc_io,
			     uint32_t cmd_flags,
			     uint16_t *major_ver,
			     uint16_t *minor_ver);

#endif /* __FSL_DPBP_V10_H */

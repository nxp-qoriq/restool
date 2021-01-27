/* Copyright 2013-2016 Freescale Semiconductor Inc.
 * Copyright 2017-2021 NXP
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

int dpdmux_open_v10(struct fsl_mc_io *mc_io,
		    uint32_t cmd_flags,
		    int  dpdmux_id,
		    uint16_t *token);

int dpdmux_close_v10(struct fsl_mc_io *mc_io,
		     uint32_t cmd_flags,
		     uint16_t token);

/*
 * DPDMUX options
 */

#define DPDMUX_OPT_CLS_MASK_SUPPORT		0x0000000000000020ULL
/**
 * Automatic max frame length - maximum frame length for spdmux interface will
 * be changed automatically by connected dpni objects.
 */
#define DPDMUX_OPT_AUTO_MAX_FRAME_LEN		0x0000000000000040ULL

/**
 * enum dpdmux_counter_type - Counter types
 * @DPDMUX_CNT_ING_FRAME: Counts ingress frames
 * @DPDMUX_CNT_ING_BYTE: Counts ingress bytes
 * @DPDMUX_CNT_ING_FLTR_FRAME: Counts filtered ingress frames
 * @DPDMUX_CNT_ING_FRAME_DISCARD: Counts discarded ingress frames
 * @DPDMUX_CNT_ING_MCAST_FRAME: Counts ingress multicast frames
 * @DPDMUX_CNT_ING_MCAST_BYTE: Counts ingress multicast bytes
 * @DPDMUX_CNT_ING_BCAST_FRAME: Counts ingress broadcast frames
 * @DPDMUX_CNT_ING_BCAST_BYTES: Counts ingress broadcast bytes
 * @DPDMUX_CNT_EGR_FRAME: Counts egress frames
 * @DPDMUX_CNT_EGR_BYTE: Counts egress bytes
 * @DPDMUX_CNT_EGR_FRAME_DISCARD: Counts discarded egress frames
 * @DPDMUX_CNT_ING_NO_BUFFER_DISCARD: Counts ingress no buffer discard frames
 */
enum dpdmux_counter_type {
	DPDMUX_CNT_ING_FRAME = 0x0,
	DPDMUX_CNT_ING_BYTE = 0x1,
	DPDMUX_CNT_ING_FLTR_FRAME = 0x2,
	DPDMUX_CNT_ING_FRAME_DISCARD = 0x3,
	DPDMUX_CNT_ING_MCAST_FRAME = 0x4,
	DPDMUX_CNT_ING_MCAST_BYTE = 0x5,
	DPDMUX_CNT_ING_BCAST_FRAME = 0x6,
	DPDMUX_CNT_ING_BCAST_BYTES = 0x7,
	DPDMUX_CNT_EGR_FRAME = 0x8,
	DPDMUX_CNT_EGR_BYTE = 0x9,
	DPDMUX_CNT_EGR_FRAME_DISCARD = 0xa,
	DPDMUX_CNT_ING_NO_BUFFER_DISCARD = 0xb,
};

/**
 * struct dpdmux_cfg_v10 - DPDMUX configuration parameters
 * @method: Defines the operation method for the DPDMUX address table
 * @manip: Required manipulation operation
 * @num_ifs: Number of interfaces (excluding the uplink interface)
 * @default_if: Default interface number (different from uplink, maximum value num_ifs)
 * @adv: Advanced parameters; default is all zeros;
 *	 use this structure to change default settings
 */
struct dpdmux_cfg_v10 {
	enum dpdmux_method method;
	enum dpdmux_manip manip;
	uint16_t num_ifs;
	uint16_t default_if;
	/**
	 * struct adv - Advanced parameters
	 * @options: DPDMUX options - combination of 'DPDMUX_OPT_<X>' flags
	 * @max_dmat_entries: Maximum entries in DPDMUX address table
	 *		0 - indicates default: 64 entries per interface.
	 * @max_mc_groups: Number of multicast groups in DPDMUX table
	 *		0 - indicates default: 32 multicast groups
	 * @max_vlan_ids: max vlan ids allowed in the system -
	 *		relevant only case of working in mac+vlan method.
	 *		0 - indicates default 16 vlan ids.
	 */
	struct {
		uint64_t options;
		uint16_t max_dmat_entries;
		uint16_t max_mc_groups;
		uint16_t max_vlan_ids;
		uint16_t mem_size;
	} adv;
};

int dpdmux_create_v10(struct fsl_mc_io *mc_io,
		      uint16_t dprc_token,
		      uint32_t cmd_flags,
		      const struct dpdmux_cfg_v10 *cfg,
		      uint32_t *obj_id);

int dpdmux_destroy_v10(struct fsl_mc_io *mc_io,
		       uint16_t dprc_token,
		       uint32_t cmd_flags,
		       uint32_t object_id);

int dpdmux_get_irq_mask_v10(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    uint8_t irq_index,
			    uint32_t *mask);

int dpdmux_get_irq_status_v10(struct fsl_mc_io *mc_io,
			      uint32_t cmd_flags,
			      uint16_t token,
			      uint8_t irq_index,
			      uint32_t *status);

/**
 * struct dpdmux_attr_v10 - Structure representing DPDMUX attributes
 * @id: DPDMUX object ID
 * @options: Configuration options (bitmap)
 * @method: DPDMUX address table method
 * @manip: DPDMUX manipulation type
 * @num_ifs: Number of interfaces (excluding the uplink interface)
 * @mem_size: DPDMUX frame storage memory size
 * @default_if: Default interface number (different from uplink, maximum value num_ifs)
 */
struct dpdmux_attr_v10 {
	int id;
	uint64_t options;
	enum dpdmux_method method;
	enum dpdmux_manip manip;
	uint16_t num_ifs;
	uint16_t mem_size;
	uint16_t default_if;
};

int dpdmux_get_attributes_v10(struct fsl_mc_io *mc_io,
			      uint32_t cmd_flags,
			      uint16_t token,
			      struct dpdmux_attr_v10 *attr);

int dpdmux_get_api_version_v10(struct fsl_mc_io *mc_io,
			       uint32_t cmd_flags,
			       uint16_t *major_ver,
			       uint16_t *minor_ver);

int dpdmux_if_get_counter(struct fsl_mc_io *mc_io,
			  uint32_t cmd_flags,
			  uint16_t token,
			  uint16_t if_id,
			  enum dpdmux_counter_type counter_type,
			  uint64_t *counter);

int dpdmux_get_max_frame_length(struct fsl_mc_io *mc_io,
				uint32_t cmd_flags,
				uint16_t token,
				uint16_t if_id,
				uint16_t *max_frame_length);

#endif /* __FSL_DPDMUX_H */

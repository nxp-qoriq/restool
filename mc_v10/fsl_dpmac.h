/* Copyright 2013-2016 Freescale Semiconductor Inc.
 * Copyright 2017-2020 NXP
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
#ifndef __FSL_DPMAC_V10_H
#define __FSL_DPMAC_V10_H

#include "../mc_v9/fsl_dpmac.h"

/* Data Path MAC API
 * Contains initialization APIs and runtime control APIs for DPMAC
 */

struct fsl_mc_io;

int dpmac_open_v10(struct fsl_mc_io *mc_io,
		   uint32_t cmd_flags,
		   int dpmac_id,
		   uint16_t *token);

int dpmac_close_v10(struct fsl_mc_io *mc_io,
		    uint32_t cmd_flags,
		    uint16_t token);

int dpmac_create_v10(struct fsl_mc_io *mc_io,
		     uint16_t dprc_token,
		     uint32_t cmd_flags,
		     const struct dpmac_cfg *cfg,
		     uint32_t *obj_id);

int dpmac_destroy_v10(struct fsl_mc_io *mc_io,
		      uint16_t dprc_token,
		      uint32_t cmd_flags,
		      uint32_t object_id);

int dpmac_get_irq_mask_v10(struct fsl_mc_io *mc_io,
			   uint32_t cmd_flags,
			   uint16_t token,
			   uint8_t irq_index,
			   uint32_t *mask);

int dpmac_get_irq_status_v10(struct fsl_mc_io *mc_io,
			     uint32_t cmd_flags,
			     uint16_t token,
			     uint8_t irq_index,
			     uint32_t *status);

int dpmac_get_mac_addr_v10(struct fsl_mc_io *mc_io,
			   uint32_t cmd_flags,
			   uint16_t token,
			   uint8_t mac_addr[6]);

/**
 * @brief»      Inter-Frame Gap mode
 *
 * LAN/WAN uses different Inter-Frame Gap mode
 */
enum dpmac_ifg_mode {
	DPMAC_IFG_MODE_FIXED,
	/*!< IFG length represents number of octets in steps of 4 */
	DPMAC_IFG_MODE_STRECHED
	/*!< IFG length represents the stretch factor */
};

/**
 * @brief Structure representing Inter-Frame Gap mode configuration
 */
struct dpmac_ifg_cfg {
	enum dpmac_ifg_mode ipg_mode; /*!< WAN/LAN mode */
	uint8_t ipg_length; /*!< IPG Length, default value is 0xC */
};

/* serdes sfi/custom settings feature internals
 * @SERDES_CFG_DEFAULT: the default configuration.
 * @SERDES_CFG_SFI: default operating mode for XFI interfaces
 * @SERDES_CFG_CUSTOM: It allows the user to manually configure the type of equalization,
 *»     amplitude, preq and post1q settings. Can be used with all interfaces except RGMII.
 */
enum serdes_eq_cfg_mode {
	SERDES_CFG_DEFAULT = 0,
	SERDES_CFG_SFI,
	SERDES_CFG_CUSTOM,
};

/**
 * struct serdes_eq_settings - Structure  SerDes equalization settings
 * cfg: serdes sfi/custom/default settings feature internals
 * @eq_type: Number of levels of TX equalization
 * @sgn_preq: Precursor sign indicating direction of eye closure
 * @eq_preq: Drive strength of TX full swing transition bit to precursor
 * @sgn_post1q: First post-cursor sign indicating direction of eye closure
 * @eq_post1q: Drive strength of full swing transition bit to first post-cursor
 * @eq_amp_red: Overall transmit amplitude reduction
 */
struct serdes_eq_settings {
	enum serdes_eq_cfg_mode cfg;
	int eq_type;
	int sgn_preq;
	int eq_preq;
	int sgn_post1q;
	int eq_post1q;
	int eq_amp_red;
};

/*
 * @DPMAC_FEC_NONE: RS-FEC (enabled by default) is disabled
 * @DPMAC_FEC_RS: RS-FEC (Clause 91) mode configured
 * @DPMAC_FEC_FC: FC-FEC (Clause 74) mode configured (not yet supported)
 */
enum dpmac_fec_mode {
	DPMAC_FEC_NONE,
	DPMAC_FEC_RS,
	DPMAC_FEC_FC,
};

/**
 * struct dpmac_attr_v10 - Structure representing DPMAC attributes
 * @id:		DPMAC object ID
 * @max_rate:	Maximum supported rate - in Mbps
 * @eth_if:	Ethernet interface
 * @link_type:	link type
 */
struct dpmac_attr_v10 {
	uint16_t id;
	uint32_t max_rate;
	enum dpmac_eth_if eth_if;
	enum dpmac_link_type link_type;
	enum dpmac_fec_mode fec_mode;
	struct serdes_eq_settings serdes_cfg;
	struct dpmac_ifg_cfg ifg_cfg;
	uint16_t lni;
	uint16_t ceetm_id;
};

int dpmac_get_attributes_v10(struct fsl_mc_io *mc_io,
			     uint32_t cmd_flags,
			     uint16_t token,
			     struct dpmac_attr_v10 *attr);

int dpmac_get_counter_v10(struct fsl_mc_io *mc_io,
			  uint32_t cmd_flags,
			  uint16_t token,
			  enum dpmac_counter  type,
			  uint64_t *counter);

int dpmac_get_api_version_v10(struct fsl_mc_io *mc_io,
			      uint32_t cmd_flags,
			      uint16_t *major_ver,
			      uint16_t *minor_ver);

#endif /* __FSL_DPMAC_H */

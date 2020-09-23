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
#ifndef __FSL_DPSW_V10_H
#define __FSL_DPSW_V10_H

#include "../mc_v9/fsl_dpsw.h"

/* Data Path L2-Switch API
 * Contains API for handling DPSW topology and functionality
 */

struct fsl_mc_io;

int dpsw_open_v10(struct fsl_mc_io *mc_io,
		  uint32_t cmd_flags,
		  int dpsw_id,
		  uint16_t *token);

int dpsw_close_v10(struct fsl_mc_io *mc_io,
		   uint32_t cmd_flags,
		   uint16_t token);

/**
 * DPSW options
 */

/**
 * Disable link aggregation
 */
#define DPSW_OPT_LAG_DIS		0x0000000000000100ULL
/**
 * Each interface will use own buffer pool
 */
#define DPSW_OPT_BP_PER_IF		0x0000000000000080ULL

/*
 * VLAN miss action is reprogrammed to look-up in FDB
 */
#define DPSW_OPT_VLAN_MISS		0x0000000000000200ULL

/**
 * struct dpsw_cfg - DPSW configuration
 * @num_ifs: Number of external and internal interfaces
 * @adv: Advanced parameters; default is all zeros;
 *		 use this structure to change default settings
 */
struct dpsw_cfg_v10 {
	uint16_t num_ifs;
	/**
	 * struct adv - Advanced parameters
	 * @options: Enable/Disable DPSW features (bitmap)
	 * @max_vlans: Maximum Number of VLAN's; 0 - indicates default 16
	 * @max_meters_per_if: Number of meters per interface
	 * @max_fdbs: Maximum Number of FDB's; 0 - indicates default 16
	 * @max_fdb_entries: Number of FDB entries for default FDB table;
	 *			0 - indicates default 1024 entries.
	 * @fdb_aging_time: Default FDB aging time for default FDB table;
	 *			0 - indicates default 300 seconds
	 * @max_fdb_mc_groups: Number of multicast groups in each FDB table;
	 *			0 - indicates default 32
	 * @component_type: Indicates the component type of this bridge
	 */
	struct {
		uint64_t options;
		uint16_t max_vlans;
		uint8_t max_meters_per_if;
		uint8_t max_fdbs;
		uint16_t max_fdb_entries;
		uint16_t fdb_aging_time;
		uint16_t max_fdb_mc_groups;
		enum dpsw_component_type component_type;
	} adv;
};

int dpsw_create_v10(struct fsl_mc_io *mc_io,
		    uint16_t dprc_token,
		    uint32_t cmd_flags,
		    const struct dpsw_cfg_v10 *cfg,
		    uint32_t *obj_id);

int dpsw_destroy_v10(struct fsl_mc_io *mc_io,
		     uint16_t dprc_token,
		     uint32_t cmd_flags,
		     uint32_t object_id);

int dpsw_get_irq_mask_v10(struct fsl_mc_io *mc_io,
			  uint32_t cmd_flags,
			  uint16_t token,
			  uint8_t irq_index,
			  uint32_t *mask);

int dpsw_get_irq_status_v10(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    uint8_t irq_index,
			    uint32_t *status);

/**
 * struct dpsw_attr_v10 - Structure representing DPSW attributes
 * @id: DPSW object ID
 * @options: Enable/Disable DPSW features
 * @max_vlans: Maximum Number of VLANs
 * @max_meters_per_if:  Number of meters per interface
 * @max_fdbs: Maximum Number of FDBs
 * @max_fdb_entries: Number of FDB entries for default FDB table;
 *			0 - indicates default 1024 entries.
 * @fdb_aging_time: Default FDB aging time for default FDB table;
 *			0 - indicates default 300 seconds
 * @max_fdb_mc_groups: Number of multicast groups in each FDB table;
 *			0 - indicates default 32
 * @mem_size: DPSW frame storage memory size
 * @num_ifs: Number of interfaces
 * @num_vlans: Current number of VLANs
 * @num_fdbs: Current number of FDBs
 * @component_type: Component type of this bridge
 */
struct dpsw_attr_v10 {
	int id;
	uint64_t options;
	uint16_t max_vlans;
	uint8_t max_meters_per_if;
	uint8_t max_fdbs;
	uint16_t max_fdb_entries;
	uint16_t fdb_aging_time;
	uint16_t max_fdb_mc_groups;
	uint16_t num_ifs;
	uint16_t mem_size;
	uint16_t num_vlans;
	uint8_t num_fdbs;
	enum dpsw_component_type component_type;
};

int dpsw_get_attributes_v10(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    struct dpsw_attr_v10 *attr);

int dpsw_get_api_version_v10(struct fsl_mc_io *mc_io,
			     uint32_t cmd_flags,
			     uint16_t *major_ver,
			     uint16_t *minor_ver);

enum dpsw_congestion_unit {
	DPSW_TAILDROP_DROP_UNIT_BYTE = 0,
	DPSW_TAILDROP_DROP_UNIT_FRAMES,
	DPSW_TAILDROP_DROP_UNIT_BUFFERS
};

/**
 * struct dpsw_taildrop_cfg - interface taildrop configuration
 * @enable - enable (1 ) or disable (0) taildrop
 * @units - taildrop units
 * @threshold - taildtop threshold
 */
struct dpsw_taildrop_cfg {
	char enable;
	enum dpsw_congestion_unit units;
	uint32_t threshold;
};

int dpsw_if_set_taildrop(struct fsl_mc_io *mc_io, uint32_t cmd_flags, uint16_t token,
			 uint16_t if_id, uint8_t tc, struct dpsw_taildrop_cfg *cfg);

int dpsw_if_get_taildrop(struct fsl_mc_io *mc_io, uint32_t cmd_flags, uint16_t token,
			 uint16_t if_id, uint8_t tc, struct dpsw_taildrop_cfg *cfg);

#endif /* __FSL_DPSW_H */

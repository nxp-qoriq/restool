/* Copyright 2013-2015 Freescale Semiconductor Inc.
 * Copyright 2018-2019 NXP
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
#ifndef __FSL_DPDBG_H
#define __FSL_DPDBG_H

#include "fsl_dpdbg_cmd.h"
#include "fsl_dpkg.h"
#include "fsl_dpmac.h"
#include "fsl_dpni.h"

/** @addtogroup dpdbg Data Path Debug API
 * Contains initialization APIs and runtime control APIs for DPDBG
 * @{
 */

struct fsl_mc_io;

int dpdbg_open_v10(struct fsl_mc_io	*mc_io,
	       uint32_t cmd_flags,
	       int dpdbg_id,
	       uint16_t *token);

int dpdbg_close_v10(struct fsl_mc_io *mc_io,
		uint32_t cmd_flags,
		uint16_t token);

/**
 * struct dpdbg_cfg - Structure representing DPDBG configuration
 * @dpdbg_id:		DPDBG ID
 * @dpdbg_container_id:	DPDBG container ID
 */
struct dpdbg_cfg {
	int dpdbg_id;
	int dpdbg_container_id;
};

int dpdbg_create_v10(struct fsl_mc_io *mc_io,
		  uint16_t dprc_token,
		  uint32_t cmd_flags,
		  const struct dpdbg_cfg *cfg,
		  uint32_t *obj_id);

int dpdbg_destroy_v10(struct fsl_mc_io *mc_io,
		   uint16_t dprc_token,
		   uint32_t cmd_flags,
		   uint32_t object_id);

/**
 * struct dpdbg_attr - Structure representing DPDBG attributes
 * @id:		DPDBG object ID
 */
struct dpdbg_attr {
	int id;
};

int dpdbg_get_attributes_v10(struct fsl_mc_io *mc_io,
			 uint32_t cmd_flags,
			 uint16_t token,
			 struct dpdbg_attr *attr);

int dpdbg_dump_v10(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			int obj_id,
			char obj_type[16]);

int dpdbg_set_v10(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			int state,
			char module[16]);

int dpdbg_get_api_version_v10(struct fsl_mc_io *mc_io,
			   uint32_t cmd_flags,
			   uint16_t *major_ver,
			   uint16_t *minor_ver);

#define DBG_CTLU_EIOP_EGRESS 0 /*!< CTLU for EIOP Egress */
#define DBG_CTLU_EIOP_INGRESS 1 /*!< CTLU for EIOP Ingress */
#define DBG_CTLU_AIOP 2 /*!< CTLU for AIOP */
#define DBG_CTLU_AIOP_MFLU 3 /*!< MFLU for AIOP */

/* CTLU profiling counters */
struct ctlu_profiling_counters {
	uint32_t rule_lookups;
	uint32_t rule_hits;
	uint32_t entry_lookups;
	uint32_t entry_hits;
	uint32_t cache_accesses;
	uint32_t cache_hits;
	uint32_t cache_updates;
	uint32_t memory_accesses;
};

int dpdbg_get_ctlu_profiling_counters(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t ctlu_type,
			struct ctlu_profiling_counters *counters);

struct ctlu_profiling_options {
	uint8_t enable_profiling_counters; /* enable/disable CTLU profiling */
	uint8_t enable_profiling_for_tid; /* set profiling for a table id */
	uint16_t table_id; /* valid iff enable_profiling_for_tid is 'true' */
};

int dpdbg_set_ctlu_profiling_counters(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t ctlu_type,
			struct ctlu_profiling_options *options);

#endif /* __FSL_DPDBG_H */

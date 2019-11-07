/* Copyright 2013-2016 Freescale Semiconductor Inc.
 * Copyright 2018-2019 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 * * Neither the name of the above-listed copyright holders nor the
 * names of any contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
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
#ifndef _FSL_DPDBG_CMD_H
#define _FSL_DPDBG_CMD_H

#include "fsl_dpkg.h"

/* DPDBG Version */
#define DPDBG_VER_MAJOR				1
#define DPDBG_VER_MINOR				0

/* Command versioning */
#define DPDBG_CMD_BASE_VERSION			1
#define DPDBG_CMD_ID_OFFSET				4

#define DPDBG_CMD(id)	((id << DPDBG_CMD_ID_OFFSET) | DPDBG_CMD_BASE_VERSION)

/* Command IDs */
#define DPDBG_CMDID_CLOSE				DPDBG_CMD(0x800)
#define DPDBG_CMDID_OPEN				DPDBG_CMD(0x80f)
#define DPDBG_CMDID_CREATE				DPDBG_CMD(0x90f)
#define DPDBG_CMDID_DESTROY				DPDBG_CMD(0x98f)
#define DPDBG_CMDID_GET_API_VERSION		DPDBG_CMD(0xa0f)
#define DPDBG_CMDID_GET_ATTR			DPDBG_CMD(0x004)
#define DPDBG_CMDID_DUMP                DPDBG_CMD(0x130)
#define DPDBG_CMDID_SET                 DPDBG_CMD(0x140)

#define DPDBG_CMDID_SET_CTLU_PROFILING		DPDBG_CMD(0x152)
#define DPDBG_CMDID_GET_CTLU_PROFILING		DPDBG_CMD(0x153)

/* Macros for accessing command fields smaller than 1byte */
#define DPDBG_MASK(field)        \
	GENMASK(DPDBG_##field##_SHIFT + DPDBG_##field##_SIZE - 1, \
		DPDBG_##field##_SHIFT)

#define dpdbg_set_field(var, field, val) \
	((var) |= (((val) << DPDBG_##field##_SHIFT) & DPDBG_MASK(field)))

#pragma pack(push, 1)

struct dpdbg_cmd_open {
	uint32_t dpdbg_id;
};

struct dpdbg_cmd_create {
	uint32_t dpdbg_id;
	uint32_t dpdbg_container_id;
};

struct dpdbg_cmd_destroy {
	uint32_t dpdbg_id;
};

struct dpdbg_rsp_get_attributes {
	uint32_t id;
};

#define OBJ_TYPE_LENGTH    16
#define MODULE_NAME_LENGTH 16

struct dpdbg_cmd_dump {
	uint32_t id;
	uint8_t type[OBJ_TYPE_LENGTH];
};

struct dpdbg_cmd_set {
	uint32_t state;
	uint8_t module[MODULE_NAME_LENGTH];
};

struct dpdbg_rsp_get_api_version {
	uint16_t major;
	uint16_t minor;
};

#define DPDBG_CTLU_TYPE_SHIFT	0
#define DPDBG_CTLU_TYPE_SIZE	3
#define DPDBG_OPT_ENABLE_PROF_SHIFT	6
#define DPDBG_OPT_ENABLE_PROF_SIZE	1
#define DPDBG_OPT_ENABLE_PROF_TID_SHIFT	7
#define DPDBG_OPT_ENABLE_PROF_TID_SIZE	1

struct dpdbg_cmd_set_ctlu_profiling_counters {
	/* from LSB: only the last 3 bits */
	uint8_t ctlu_type;
	/* from LSB: only the last 1 bit */
	uint8_t opt_enable_prof_ctrs;
	/* from LSB: only the last 1 bits,
	 * valid if opt_enable_prof_ctrs == true
	 */
	uint8_t opt_enable_prof_for_tid;
	/* valid only if opt_enable_prof_for_tid && opt_enable_prof_ctrs */
	uint16_t table_id;
};

struct dpdbg_cmd_get_ctlu_profiling_counters {
	/* from LSB: only the last 3 bits */
	uint8_t ctlu_type;
};

struct dpdbg_rsp_get_ctlu_profiling_counters {
	uint32_t rule_lookups;
	uint32_t rule_hits;
	uint32_t entry_lookups;
	uint32_t entry_hits;
	uint32_t cache_accesses;
	uint32_t cache_hits;
	uint32_t cache_updates;
	uint32_t memory_accesses;
};

#pragma pack(pop)

#endif /* _FSL_DPDBG_CMD_H */

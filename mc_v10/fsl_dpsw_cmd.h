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
#ifndef __FSL_DPSW_CMD_H
#define __FSL_DPSW_CMD_H

/* DPSW Version */
#define DPSW_VER_MAJOR		8
#define DPSW_VER_MINOR		2

/* Command versioning */
#define DPSW_CMD_BASE_VERSION	1
#define DPSW_CMD_VERSION_2	2
#define DPSW_CMD_ID_OFFSET	4

#define DPSW_CMD(id)	((id << DPSW_CMD_ID_OFFSET) | DPSW_CMD_BASE_VERSION)
#define DPSW_CMD_V2(id)	(((id) << DPSW_CMD_ID_OFFSET) | DPSW_CMD_VERSION_2)

/* Command IDs */
#define DPSW_CMDID_CLOSE                        DPSW_CMD(0x800)
#define DPSW_CMDID_OPEN                         DPSW_CMD(0x802)
#define DPSW_CMDID_CREATE                       DPSW_CMD(0x902)
#define DPSW_CMDID_DESTROY                      DPSW_CMD(0x982)
#define DPSW_CMDID_GET_API_VERSION              DPSW_CMD(0xa02)
#define DPSW_CMDID_GET_ATTR                     DPSW_CMD(0x004)
#define DPSW_CMDID_GET_IRQ_MASK                 DPSW_CMD(0x015)
#define DPSW_CMDID_GET_IRQ_STATUS               DPSW_CMD(0x016)

#define DPSW_CMDID_IF_SET_TAILDROP		DPSW_CMD(0x0A8)
#define DPSW_CMDID_IF_GET_TAILDROP		DPSW_CMD(0x0A9)

#define DPSW_CMDID_IF_GET_COUNTER               DPSW_CMD_V2(0x034)
#define DPSW_CMDID_IF_GET_MAX_FRAME_LENGTH      DPSW_CMD(0x045)

/* Macros for accessing command fields smaller than 1byte */
#define DPSW_MASK(field)        \
	GENMASK(DPSW_##field##_SHIFT + DPSW_##field##_SIZE - 1, \
		DPSW_##field##_SHIFT)
#define dpsw_set_field(var, field, val) \
	((var) |= (((val) << DPSW_##field##_SHIFT) & DPSW_MASK(field)))
#define dpsw_get_field(var, field)      \
	(((var) & DPSW_MASK(field)) >> DPSW_##field##_SHIFT)
#define dpsw_set_bit(var, bit, val) \
	((var) |= (((uint64_t)(val) << (bit)) & GENMASK((bit), (bit))))
#define dpsw_get_bit(var, bit) \
	(((var)  >> bit) & GENMASK(0, 0))

#pragma pack(push, 1)
struct dpsw_cmd_open {
	uint32_t dpsw_id;
};

#define DPSW_COMPONENT_TYPE_SHIFT	0
#define DPSW_COMPONENT_TYPE_SIZE	4

struct dpsw_cmd_create {
	/* cmd word 0 */
	uint16_t num_ifs;
	uint8_t max_fdbs;
	uint8_t max_meters_per_if;
	/* from LSB: only the first 4 bits */
	uint8_t component_type;
	uint8_t pad[3];
	/* cmd word 1 */
	uint16_t max_vlans;
	uint16_t max_fdb_entries;
	uint16_t fdb_aging_time;
	uint16_t max_fdb_mc_groups;
	/* cmd word 2 */
	uint64_t options;
};

struct dpsw_cmd_destroy {
	uint32_t dpsw_id;
};

#define DPSW_ENABLE_SHIFT		0
#define DPSW_ENABLE_SIZE		1

struct dpsw_cmd_get_irq_mask {
	uint32_t pad;
	uint8_t irq_index;
};

struct dpsw_rsp_get_irq_mask {
	uint32_t mask;
};

struct dpsw_cmd_get_irq_status {
	uint32_t status;
	uint8_t irq_index;
};

struct dpsw_rsp_get_irq_status {
	uint32_t status;
};

#define DPSW_COMPONENT_TYPE_SHIFT	0
#define DPSW_COMPONENT_TYPE_SIZE	4

struct dpsw_rsp_get_attr {
	/* cmd word 0 */
	uint16_t num_ifs;
	uint8_t max_fdbs;
	uint8_t num_fdbs;
	uint16_t max_vlans;
	uint16_t num_vlans;
	/* cmd word 1 */
	uint16_t max_fdb_entries;
	uint16_t fdb_aging_time;
	uint32_t dpsw_id;
	/* cmd word 2 */
	uint16_t mem_size;
	uint16_t max_fdb_mc_groups;
	uint8_t max_meters_per_if;
	/* from LSB only the ffirst 4 bits */
	uint8_t component_type;
	uint16_t pad;
	/* cmd word 3 */
	uint64_t options;
};


struct dpsw_rsp_get_api_version {
	uint16_t version_major;
	uint16_t version_minor;
};

struct dpsw_cmd_set_taildrop {
	uint16_t pad1;
	uint8_t tc;
	uint8_t pad2;
	uint16_t if_id;
	uint16_t pad3;
	uint16_t oal_en;
	uint8_t units;
	uint8_t pad4;
	uint32_t threshold;
};

struct dpsw_cmd_get_taildrop {
	uint16_t pad1;
	uint8_t tc;
	uint8_t pad2;
	uint16_t if_id;
};

struct dpsw_rsp_get_taildrop {
	uint16_t pad1;
	uint16_t pad2;
	uint16_t if_id;
	uint16_t pad3;
	uint16_t oal_en;
	uint8_t units;
	uint8_t pad4;
	uint32_t threshold;
};

#define DPSW_COUNTER_TYPE_SHIFT		0
#define DPSW_COUNTER_TYPE_SIZE		5

struct dpsw_cmd_if_get_counter {
	uint16_t if_id;
	/* from LSB: type:5 */
	uint8_t type;
};

struct dpsw_rsp_if_get_counter {
	uint64_t pad;
	uint64_t counter;
};

struct dpsw_cmd_if_get_max_frame_length {
	uint16_t if_id;
};

struct dpsw_rsp_if_get_max_frame_length {
	uint16_t pad;
	uint16_t frame_length;
};

#pragma pack(pop)
#endif /* __FSL_DPSW_CMD_H */

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
#ifndef _FSL_DPDMUX_CMD_H
#define _FSL_DPDMUX_CMD_H

/* DPDMUX Version */
#define DPDMUX_VER_MAJOR		6
#define DPDMUX_VER_MINOR		5

#define DPDMUX_CMD_BASE_VERSION		1
#define DPDMUX_CMD_VERSION_2		2
#define DPDMUX_CMD_VERSION_3		3
#define DPDMUX_CMD_VERSION_4		4
#define DPDMUX_CMD_ID_OFFSET		4

#define DPDMUX_CMD(id)	((id << DPDMUX_CMD_ID_OFFSET) | DPDMUX_CMD_BASE_VERSION)
#define DPDMUX_CMD_V2(id)	(((id) << DPDMUX_CMD_ID_OFFSET) | DPDMUX_CMD_VERSION_2)
#define DPDMUX_CMD_V3(id)       (((id) << DPDMUX_CMD_ID_OFFSET) | DPDMUX_CMD_VERSION_3)
#define DPDMUX_CMD_V4(id)       (((id) << DPDMUX_CMD_ID_OFFSET) | DPDMUX_CMD_VERSION_4)

/* Command IDs */
#define DPDMUX_CMDID_CLOSE			DPDMUX_CMD(0x800)
#define DPDMUX_CMDID_OPEN			DPDMUX_CMD(0x806)
#define DPDMUX_CMDID_CREATE			DPDMUX_CMD_V4(0x906)
#define DPDMUX_CMDID_DESTROY			DPDMUX_CMD(0x986)
#define DPDMUX_CMDID_GET_API_VERSION		DPDMUX_CMD(0xa06)

#define DPDMUX_CMDID_GET_ATTR			DPDMUX_CMD_V2(0x004)

#define DPDMUX_CMDID_GET_IRQ_MASK		DPDMUX_CMD(0x015)
#define DPDMUX_CMDID_GET_IRQ_STATUS		DPDMUX_CMD(0x016)

#define DPDMUX_CMDID_IF_GET_COUNTER		DPDMUX_CMD(0x0b2)

#define DPDMUX_CMDID_GET_MAX_FRAME_LENGTH	DPDMUX_CMD(0x0a2)

#define DPDMUX_MASK(field)        \
	GENMASK(DPDMUX_##field##_SHIFT + DPDMUX_##field##_SIZE - 1, \
		DPDMUX_##field##_SHIFT)
#define dpdmux_set_field(var, field, val) \
	((var) |= (((val) << DPDMUX_##field##_SHIFT) & DPDMUX_MASK(field)))
#define dpdmux_get_field(var, field)      \
	(((var) & DPDMUX_MASK(field)) >> DPDMUX_##field##_SHIFT)

#pragma pack(push, 1)

struct dpdmux_cmd_if_get_counter {
	uint16_t if_id;
	uint8_t counter_type;
};

struct dpdmux_rsp_if_get_counter {
	uint64_t pad;
	uint64_t counter;
};

struct dpdmux_cmd_open {
	uint32_t dpdmux_id;
};

struct dpdmux_cmd_create {
	uint8_t method;
	uint8_t manip;
	uint16_t num_ifs;
	uint16_t default_if;
	uint16_t pad;

	uint16_t adv_max_dmat_entries;
	uint16_t adv_max_mc_groups;
	uint16_t adv_max_vlan_ids;
	uint16_t mem_size;

	uint64_t options;
};

struct dpdmux_cmd_destroy {
	uint32_t dpdmux_id;
};

struct dpdmux_cmd_get_irq_mask {
	uint32_t pad;
	uint8_t irq_index;
};

struct dpdmux_rsp_get_irq_mask {
	uint32_t mask;
};

struct dpdmux_cmd_get_irq_status {
	uint32_t status;
	uint8_t irq_index;
};

struct dpdmux_rsp_get_irq_status {
	uint32_t status;
};

struct dpdmux_rsp_get_attr {
	uint8_t method;
	uint8_t manip;
	uint16_t num_ifs;
	uint16_t mem_size;
	uint16_t default_if;

	uint64_t pad1;

	uint32_t id;
	uint32_t pad2;

	uint64_t options;
};

struct dpdmux_rsp_get_api_version {
	uint16_t major;
	uint16_t minor;
};

struct dpdmux_cmd_get_max_frame_len {
	uint16_t if_id;
};

struct dpdmux_rsp_get_max_frame_len {
	uint16_t max_len;
};

#pragma pack(pop)
#endif /* _FSL_DPDMUX_CMD_H */


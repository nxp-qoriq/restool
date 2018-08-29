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
#ifndef _FSL_DPNI_CMD_v10_H
#define _FSL_DPNI_CMD_v10_H

/* DPNI Version */
#define DPNI_VER_MAJOR				7
#define DPNI_VER_MINOR				4

#define DPNI_CMD_BASE_VERSION			1
#define DPNI_CMD_VERSION_2			2
#define DPNI_CMD_ID_OFFSET			4

#define DPNI_CMD(id)	(((id) << DPNI_CMD_ID_OFFSET) | DPNI_CMD_BASE_VERSION)
#define DPNI_CMD_V2(id)	(((id) << DPNI_CMD_ID_OFFSET) | DPNI_CMD_VERSION_2)

/* Command IDs */
#define DPNI_CMDID_OPEN				DPNI_CMD(0x801)
#define DPNI_CMDID_CLOSE			DPNI_CMD(0x800)
#define DPNI_CMDID_CREATE			DPNI_CMD(0x901)
#define DPNI_CMDID_DESTROY			DPNI_CMD(0x981)
#define DPNI_CMDID_GET_API_VERSION		DPNI_CMD(0xa01)
#define DPNI_CMDID_GET_ATTR			DPNI_CMD_V2(0x004)
#define DPNI_CMDID_SET_PRIM_MAC			DPNI_CMD(0x224)
#define DPNI_CMDID_GET_PRIM_MAC			DPNI_CMD(0x225)
#define DPNI_CMDID_GET_STATISTICS		DPNI_CMD_V2(0x25D)
#define DPNI_CMDID_GET_LINK_STATE		DPNI_CMD(0x215)
#define DPNI_CMDID_GET_IRQ_MASK			DPNI_CMD(0x015)
#define DPNI_CMDID_GET_IRQ_STATUS		DPNI_CMD(0x016)

/* Macros for accessing command fields smaller than 1byte */
#define DPNI_MASK(field)	\
	GENMASK(DPNI_##field##_SHIFT + DPNI_##field##_SIZE - 1, \
		DPNI_##field##_SHIFT)
#define dpni_set_field(var, field, val)	\
	((var) |= (((val) << DPNI_##field##_SHIFT) & DPNI_MASK(field)))
#define dpni_get_field(var, field)	\
	(((var) & DPNI_MASK(field)) >> DPNI_##field##_SHIFT)

#pragma pack(push, 1)

struct dpni_cmd_open {
	uint32_t dpni_id;
};

struct dpni_cmd_create {
	uint32_t options;
	uint8_t num_queues;
	uint8_t num_tcs;
	uint8_t mac_filter_entries;
	uint8_t pad1;
	uint8_t vlan_filter_entries;
	uint8_t pad2;
	uint8_t qos_entries;
	uint8_t pad3;
	uint16_t fs_entries;
};

struct dpni_cmd_destroy {
	uint32_t dpsw_id;
};

struct dpni_rsp_get_api_version {
	uint16_t major;
	uint16_t minor;
};

struct dpni_rsp_get_attr {
	/* response word 0 */
	uint32_t options;
	uint8_t num_queues;
	uint8_t num_rx_tcs;
	uint8_t mac_filter_entries;
	uint8_t num_tx_tcs;
	/* response word 1 */
	uint8_t vlan_filter_entries;
	uint8_t pad1;
	uint8_t qos_entries;
	uint8_t pad2;
	uint16_t fs_entries;
	uint16_t pad3;
	/* response word 2 */
	uint8_t qos_key_size;
	uint8_t fs_key_size;
	uint16_t wriop_version;
};

struct dpni_cmd_set_primary_mac_addr {
	uint16_t pad;
	uint8_t mac_addr[6];
};

struct dpni_rsp_get_primary_mac_addr {
	uint16_t pad;
	uint8_t mac_addr[6];
};

struct dpni_cmd_get_statistics {
	uint8_t page_number;
	uint8_t param;
};

struct dpni_rsp_get_statistics {
	uint64_t counter[7];
};

#define DPNI_LINK_STATE_SHIFT		0
#define DPNI_LINK_STATE_SIZE		1

struct dpni_rsp_get_link_state {
	uint32_t pad0;
	/* from LSB: up:1 */
	uint8_t flags;
	uint8_t pad1[3];
	uint32_t rate;
	uint32_t pad2;
	uint64_t options;
};

struct dpni_cmd_get_irq_mask {
	uint32_t pad;
	uint8_t irq_index;
};

struct dpni_rsp_get_irq_mask {
	uint32_t mask;
};

struct dpni_cmd_get_irq_status {
	uint32_t status;
	uint8_t irq_index;
};

struct dpni_rsp_get_irq_status {
	uint32_t status;
};

#pragma pack(pop)
#endif /* _FSL_DPNI_CMD_v10_H */

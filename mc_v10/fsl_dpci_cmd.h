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
#ifndef _FSL_DPCI_CMD_H
#define _FSL_DPCI_CMD_H

/* DPCI Version */
#define DPCI_VER_MAJOR			3
#define DPCI_VER_MINOR			4

#define DPCI_CMD_BASE_VERSION		1
#define DPCI_CMD_BASE_VERSION_V2	2
#define DPCI_CMD_ID_OFFSET		4

#define DPCI_CMD_V1(id) ((id << DPCI_CMD_ID_OFFSET) | DPCI_CMD_BASE_VERSION)
#define DPCI_CMD_V2(id) ((id << DPCI_CMD_ID_OFFSET) | DPCI_CMD_BASE_VERSION_V2)

/* Command IDs */
#define DPCI_CMDID_CLOSE		DPCI_CMD_V1(0x800)
#define DPCI_CMDID_OPEN			DPCI_CMD_V1(0x807)
#define DPCI_CMDID_CREATE		DPCI_CMD_V2(0x907)
#define DPCI_CMDID_DESTROY		DPCI_CMD_V1(0x987)
#define DPCI_CMDID_GET_API_VERSION	DPCI_CMD_V1(0xa07)
#define DPCI_CMDID_GET_ATTR		DPCI_CMD_V1(0x004)
#define DPCI_CMDID_GET_PEER_ATTR	DPCI_CMD_V1(0x0e2)
#define DPCI_CMDID_GET_IRQ_MASK		DPCI_CMD_V1(0x015)
#define DPCI_CMDID_GET_IRQ_STATUS	DPCI_CMD_V1(0x016)
#define DPCI_CMDID_GET_LINK_STATE	DPCI_CMD_V1(0x0e1)

/* Macros for accessing command fields smaller than 1byte */
#define DPCI_MASK(field)        \
	GENMASK(DPCI_##field##_SHIFT + DPCI_##field##_SIZE - 1, \
		DPCI_##field##_SHIFT)
#define dpci_set_field(var, field, val) \
	((var) |= (((val) << DPCI_##field##_SHIFT) & DPCI_MASK(field)))
#define dpci_get_field(var, field)      \
	(((var) & DPCI_MASK(field)) >> DPCI_##field##_SHIFT)

#pragma pack(push, 1)
struct dpci_cmd_open {
	uint32_t dpci_id;
};

struct dpci_cmd_create {
	uint8_t num_of_priorities;
	uint8_t pad[15];
	uint32_t options;
};

struct dpci_cmd_destroy {
	uint32_t dpci_id;
};

struct dpci_cmd_get_irq_mask {
	uint32_t pad;
	uint8_t irq_index;
};

struct dpci_rsp_get_irq_mask {
	uint32_t mask;
};

struct dpci_cmd_get_irq_status {
	uint32_t status;
	uint8_t irq_index;
};

struct dpci_rsp_get_irq_status {
	uint32_t status;
};

struct dpci_rsp_get_attr {
	uint32_t id;
	uint16_t pad;
	uint8_t num_of_priorities;
};

struct dpci_rsp_get_peer_attr {
	uint32_t id;
	uint32_t pad;
	uint8_t num_of_priorities;
};

#define DPCI_UP_SHIFT	0
#define DPCI_UP_SIZE	1

struct dpci_rsp_get_link_state {
	/* only the LSB bit */
	uint8_t up;
};

struct dpci_rsp_get_api_version {
	uint16_t major;
	uint16_t minor;
};

#pragma pack(pop)
#endif /* _FSL_DPCI_CMD_H */

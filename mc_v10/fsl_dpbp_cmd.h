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
#ifndef _FSL_DPBP_CMD_V10_H
#define _FSL_DPBP_CMD_V10_H

/* DPBP Version */
#define DPBP_VER_MAJOR				3
#define DPBP_VER_MINOR				3

/* Command versioning */
#define DPBP_CMD_BASE_VERSION			1
#define DPBP_CMD_ID_OFFSET			4

#define DPBP_CMD(id)	((id << DPBP_CMD_ID_OFFSET) | DPBP_CMD_BASE_VERSION)

/* Command IDs */
#define DPBP_CMDID_CLOSE		DPBP_CMD(0x800)
#define DPBP_CMDID_OPEN			DPBP_CMD(0x804)
#define DPBP_CMDID_CREATE		DPBP_CMD(0x904)
#define DPBP_CMDID_DESTROY		DPBP_CMD(0x984)
#define DPBP_CMDID_GET_API_VERSION	DPBP_CMD(0xa04)

#define DPBP_CMDID_GET_ATTR		DPBP_CMD(0x004)

#define DPBP_CMDID_GET_IRQ_MASK		DPBP_CMD(0x015)
#define DPBP_CMDID_GET_IRQ_STATUS	DPBP_CMD(0x016)

#pragma pack(push, 1)
struct dpbp_cmd_open {
	uint32_t dpbp_id;
};

struct dpbp_cmd_destroy {
	uint32_t object_id;
};

struct dpbp_cmd_get_irq_mask {
	uint32_t pad;
	uint8_t irq_index;
};

struct dpbp_rsp_get_irq_mask {
	uint32_t mask;
};

struct dpbp_cmd_get_irq_status {
	uint32_t status;
	uint8_t irq_index;
};

struct dpbp_rsp_get_irq_status {
	uint32_t status;
};

struct dpbp_rsp_get_attributes {
	uint16_t pad;
	uint16_t bpid;
	uint32_t id;
};

struct dpbp_rsp_get_api_version {
	uint16_t major;
	uint16_t minor;
};

#pragma pack(pop)
#endif /* _FSL_DPBP_CMD_V10_H */

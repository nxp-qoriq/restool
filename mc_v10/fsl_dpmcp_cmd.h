/* Copyright 2013-2016 Freescale Semiconductor Inc.
 * Copyright 2017-2022 NXP
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
#ifndef _FSL_DPMCP_CMD_H
#define _FSL_DPMCP_CMD_H

/* Minimal supported DPMCP Version */
#define DPMCP_VER_MAJOR		4
#define DPMCP_VER_MINOR		1

/* Command versioning */
#define DPMCP_CMD_BASE_VERSION		1
#define DPMCP_CMD_VERSION_2		2
#define DPMCP_CMD_ID_OFFSET		4

#define DPMCP_CMD(id)		((id << DPMCP_CMD_ID_OFFSET) | DPMCP_CMD_BASE_VERSION)
#define DPMCP_CMD_V2(id)	(((id) << DPMCP_CMD_ID_OFFSET) | DPMCP_CMD_VERSION_2)

/* Command IDs */
#define DPMCP_CMDID_CLOSE		DPMCP_CMD(0x800)
#define DPMCP_CMDID_OPEN		DPMCP_CMD(0x80b)
#define DPMCP_CMDID_CREATE		DPMCP_CMD_V2(0x90b)
#define DPMCP_CMDID_DESTROY		DPMCP_CMD(0x98b)
#define DPMCP_CMDID_GET_API_VERSION	DPMCP_CMD(0xa0b)
#define DPMCP_CMDID_GET_ATTR		DPMCP_CMD_V2(0x004)
#define DPMCP_CMDID_GET_IRQ_MASK	DPMCP_CMD(0x015)
#define DPMCP_CMDID_GET_IRQ_STATUS	DPMCP_CMD(0x016)

#pragma pack(push, 1)
struct dpmcp_cmd_open {
	uint32_t dpmcp_id;
};

struct dpmcp_cmd_create {
	uint32_t portal_id;
	uint32_t options;
};

struct dpmcp_cmd_destroy {
	uint32_t object_id;
};

struct dpmcp_cmd_get_irq_mask {
	uint32_t pad;
	uint8_t irq_index;
};

struct dpmcp_rsp_get_irq_mask {
	uint32_t mask;
};

struct dpmcp_cmd_get_irq_status {
	uint32_t status;
	uint8_t irq_index;
};

struct dpmcp_rsp_get_irq_status {
	uint32_t status;
};

struct dpmcp_rsp_get_attributes {
	uint32_t options;
	uint32_t id;
};

struct dpmcp_rsp_get_api_version {
	uint16_t major;
	uint16_t minor;
};
#pragma pack(pop)
#endif /* _FSL_DPMCP_CMD_H */

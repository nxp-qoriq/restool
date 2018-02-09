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
#ifndef _FSL_DPAIOP_CMD_V10_H
#define _FSL_DPAIOP_CMD_V10_H

/* DPAIOP Version */
#define DPAIOP_VER_MAJOR		2
#define DPAIOP_VER_MINOR		3

#define DPAIOP_CMD_BASE_VERSION		1
#define DPAIOP_CMD_ID_OFFSET		4

#define DPAIOP_CMD(id)	((id << DPAIOP_CMD_ID_OFFSET) | DPAIOP_CMD_BASE_VERSION)

/* Command IDs */
#define DPAIOP_CMDID_CLOSE		DPAIOP_CMD(0x800)
#define DPAIOP_CMDID_OPEN		DPAIOP_CMD(0x80a)
#define DPAIOP_CMDID_CREATE		DPAIOP_CMD(0x90a)
#define DPAIOP_CMDID_DESTROY		DPAIOP_CMD(0x98a)
#define DPAIOP_CMDID_GET_API_VERSION	DPAIOP_CMD(0xa0a)

#define DPAIOP_CMDID_GET_ATTR		DPAIOP_CMD(0x004)

#define DPAIOP_CMDID_GET_IRQ_MASK	DPAIOP_CMD(0x015)
#define DPAIOP_CMDID_GET_IRQ_STATUS	DPAIOP_CMD(0x016)

#define DPAIOP_CMDID_GET_SL_VERSION	DPAIOP_CMD(0x282)
#define DPAIOP_CMDID_GET_STATE		DPAIOP_CMD(0x283)

#pragma pack(push, 1)
struct dpaiop_cmd_open {
	uint32_t dpaiop_id;
};

struct dpaiop_cmd_create {
	uint32_t aiop_id;
	uint32_t aiop_container_id;
};

struct dpaiop_cmd_destroy {
	uint32_t dpaiop_id;
};

struct dpaiop_cmd_get_irq_mask {
	uint32_t pad;
	uint8_t irq_index;
};

struct dpaiop_rsp_get_irq_mask {
	uint32_t mask;
};

struct dpaiop_cmd_irq_status {
	uint32_t status;
	uint8_t irq_index;
};

struct dpaiop_rsp_get_irq_status {
	uint32_t status;
};

struct dpaiop_rsp_get_attributes {
	uint32_t id;
};

struct dpaiop_rsp_get_sl_version {
	uint32_t version_major;
	uint32_t version_minor;
	uint32_t revision;
};

struct dpaiop_rsp_get_state {
	uint32_t state;
};

struct dpaiop_rsp_get_api_version {
	uint16_t major;
	uint16_t minor;
};
#pragma pack(pop)
#endif /* _FSL_DPAIOP_CMD_V10_H */

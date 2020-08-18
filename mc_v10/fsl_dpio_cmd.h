/* Copyright 2013-2016 Freescale Semiconductor Inc.
 * Copyright 2017-2020 NXP
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
#ifndef _FSL_DPIO_CMD_H
#define _FSL_DPIO_CMD_H

/* DPIO Version */
#define DPIO_VER_MAJOR			4
#define DPIO_VER_MINOR			4

#define DPIO_CMD_BASE_VERSION		1
#define DPIO_CMD_VERSION_2		2
#define DPIO_CMD_ID_OFFSET		4

#define DPIO_CMD(id)	(((id) << DPIO_CMD_ID_OFFSET) | DPIO_CMD_BASE_VERSION)
#define DPIO_CMD_V2(id)	(((id) << DPIO_CMD_ID_OFFSET) | DPIO_CMD_VERSION_2)

/* Command IDs */
#define DPIO_CMDID_CLOSE				DPIO_CMD(0x800)
#define DPIO_CMDID_OPEN					DPIO_CMD(0x803)
#define DPIO_CMDID_CREATE				DPIO_CMD_V2(0x903)
#define DPIO_CMDID_DESTROY				DPIO_CMD(0x983)
#define DPIO_CMDID_GET_API_VERSION			DPIO_CMD(0xa03)
#define DPIO_CMDID_GET_ATTR				DPIO_CMD_V2(0x004)
#define DPIO_CMDID_GET_IRQ_MASK				DPIO_CMD(0x015)
#define DPIO_CMDID_GET_IRQ_STATUS			DPIO_CMD(0x016)

/* Macros for accessing command fields smaller than 1byte */
#define DPIO_MASK(field)        \
	GENMASK(DPIO_##field##_SHIFT + DPIO_##field##_SIZE - 1, \
		DPIO_##field##_SHIFT)
#define dpio_set_field(var, field, val) \
	((var) |= (((val) << DPIO_##field##_SHIFT) & DPIO_MASK(field)))
#define dpio_get_field(var, field)      \
	(((var) & DPIO_MASK(field)) >> DPIO_##field##_SHIFT)

#pragma pack(push, 1)
struct dpio_cmd_open {
	uint32_t dpio_id;
};

#define DPIO_CHANNEL_MODE_SHIFT		0
#define DPIO_CHANNEL_MODE_SIZE		2

struct dpio_cmd_create {
	uint16_t pad1;
	/* from LSB: channel_mode:2 */
	uint8_t channel_mode;
	uint8_t pad2;
	uint8_t num_priorities;
	uint8_t pad3[3];
	uint32_t options;
};

struct dpio_cmd_destroy {
	uint32_t dpio_id;
};

struct dpio_cmd_get_irq {
	uint32_t pad;
	uint8_t irq_index;
};

struct dpio_rsp_get_irq_mask {
	uint32_t mask;
};

struct dpio_cmd_irq_status {
	uint32_t status;
	uint8_t irq_index;
};

struct dpio_rsp_get_irq_status {
	uint32_t status;
};

#define DPIO_ATTR_CHANNEL_MODE_SHIFT	0
#define DPIO_ATTR_CHANNEL_MODE_SIZE	4

struct dpio_rsp_get_attr {
	uint32_t id;
	uint16_t qbman_portal_id;
	uint8_t num_priorities;
	/* from LSB: channel_mode:4 */
	uint8_t channel_mode;
	uint64_t qbman_portal_ce_offset;
	uint64_t qbman_portal_ci_offset;
	uint32_t qbman_version;
	uint32_t options;
	uint32_t clk;
};

struct dpio_rsp_get_api_version {
	uint16_t major;
	uint16_t minor;
};

#pragma pack(pop)
#endif /* _FSL_DPIO_CMD_H */

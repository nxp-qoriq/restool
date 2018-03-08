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
#ifndef _FSL_DPDCEI_CMD_H
#define _FSL_DPDCEI_CMD_H

/* DPDCEI Version */
#define DPDCEI_VER_MAJOR		2
#define DPDCEI_VER_MINOR		3

#define DPDCEI_CMD_BASE_VERSION		1
#define DPDCEI_CMD_VERSION_2		2
#define DPDCEI_CMD_ID_OFFSET		4

#define DPDCEI_CMD(id)	((id << DPDCEI_CMD_ID_OFFSET) | DPDCEI_CMD_BASE_VERSION)
#define DPDCEI_CMD_V2(id) \
			(((id) << DPDCEI_CMD_ID_OFFSET) | DPDCEI_CMD_VERSION_2)

/* Command IDs */
#define DPDCEI_CMDID_CLOSE		DPDCEI_CMD(0x800)
#define DPDCEI_CMDID_OPEN		DPDCEI_CMD(0x80D)
#define DPDCEI_CMDID_CREATE		DPDCEI_CMD(0x90D)
#define DPDCEI_CMDID_DESTROY		DPDCEI_CMD(0x98D)
#define DPDCEI_CMDID_GET_API_VERSION	DPDCEI_CMD(0xa0D)

#define DPDCEI_CMDID_GET_ATTR		DPDCEI_CMD_V2(0x004)

#define DPDCEI_CMDID_GET_IRQ_MASK	DPDCEI_CMD(0x015)
#define DPDCEI_CMDID_GET_IRQ_STATUS	DPDCEI_CMD(0x016)

/* Macros for accessing command fields smaller than 1byte */
#define DPDCEI_MASK(field)        \
	GENMASK(DPDCEI_##field##_SHIFT + DPDCEI_##field##_SIZE - 1, \
		DPDCEI_##field##_SHIFT)
#define dpdcei_set_field(var, field, val) \
	((var) |= (((val) << DPDCEI_##field##_SHIFT) & DPDCEI_MASK(field)))
#define dpdcei_get_field(var, field)      \
	(((var) & DPDCEI_MASK(field)) >> DPDCEI_##field##_SHIFT)

#pragma pack(push, 1)
struct dpdcei_cmd_open {
	uint32_t dpdcei_id;
};

struct dpdcei_cmd_create {
	uint8_t pad;
	uint8_t engine;
	uint8_t priority;
};

struct dpdcei_cmd_destroy {
	uint32_t dpdcei_id;
};

struct dpdcei_cmd_get_irq_mask {
	uint32_t pad;
	uint8_t irq_index;
};

struct dpdcei_rsp_get_irq_mask {
	uint32_t mask;
};

struct dpdcei_cmd_irq_status {
	uint32_t status;
	uint8_t irq_index;
};

struct dpdcei_rsp_get_irq_status {
	uint32_t status;
};

struct dpdcei_rsp_get_attr {
	uint32_t id;
	uint8_t dpdcei_engine;
	uint8_t pad[3];
	uint64_t dce_version;
};

struct dpdcei_rsp_get_api_version {
	uint16_t major;
	uint16_t minor;
};
#pragma pack(pop)
#endif /* _FSL_DPDCEI_CMD_H */

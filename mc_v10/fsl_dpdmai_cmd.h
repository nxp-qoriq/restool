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
#ifndef _FSL_DPDMAI_CMD_H
#define _FSL_DPDMAI_CMD_H

/* DPDMAI Version */
#define DPDMAI_VER_MAJOR		3
#define DPDMAI_VER_MINOR		2

/* Command versioning */
#define DPDMAI_CMD_BASE_VERSION		1
#define DPDMAI_CMD_VERSION_2		2
#define DPDMAI_CMD_ID_OFFSET		4

#define DPDMAI_CMD(id)	((id << DPDMAI_CMD_ID_OFFSET) | DPDMAI_CMD_BASE_VERSION)
#define DPDMAI_CMD_V2(id)	((id << DPDMAI_CMD_ID_OFFSET) | DPDMAI_CMD_VERSION_2)

/* Command IDs */
#define DPDMAI_CMDID_CLOSE		DPDMAI_CMD(0x800)
#define DPDMAI_CMDID_OPEN		DPDMAI_CMD(0x80E)
#define DPDMAI_CMDID_CREATE		DPDMAI_CMD_V2(0x90E)
#define DPDMAI_CMDID_DESTROY		DPDMAI_CMD(0x98E)
#define DPDMAI_CMDID_GET_API_VERSION	DPDMAI_CMD(0xa0E)

#define DPDMAI_CMDID_GET_ATTR		DPDMAI_CMD_V2(0x004)

#define DPDMAI_CMDID_GET_IRQ_MASK	DPDMAI_CMD(0x015)
#define DPDMAI_CMDID_GET_IRQ_STATUS	DPDMAI_CMD(0x016)

/* Macros for accessing command fields smaller than 1byte */
#define DPDMAI_MASK(field)        \
	GENMASK(DPDMAI_##field##_SHIFT + DPDMAI_##field##_SIZE - 1, \
		DPDMAI_##field##_SHIFT)
#define dpdmai_set_field(var, field, val) \
	((var) |= (((val) << DPDMAI_##field##_SHIFT) & DPDMAI_MASK(field)))
#define dpdmai_get_field(var, field)      \
	(((var) & DPDMAI_MASK(field)) >> DPDMAI_##field##_SHIFT)

#pragma pack(push, 1)
struct dpdmai_cmd_open {
	uint32_t dpdmai_id;
};

struct dpdmai_cmd_create {
	uint8_t num_queues;
	uint8_t priorities[2];
};

struct dpdmai_cmd_destroy {
	uint32_t dpdmai_id;
};

struct dpdmai_cmd_get_irq_mask {
	uint32_t pad;
	uint8_t irq_index;
};

struct dpdmai_rsp_get_irq_mask {
	uint32_t mask;
};

struct dpdmai_cmd_get_irq_status {
	uint32_t status;
	uint8_t irq_index;
};

struct dpdmai_rsp_get_irq_status {
	uint32_t status;
};

struct dpdmai_rsp_get_attr {
	uint32_t id;
	uint8_t num_of_priorities;
	uint8_t num_of_queues;
};

struct dpdmai_rsp_get_api_version {
	uint16_t major;
	uint16_t minor;
};
#pragma pack(pop)
#endif /* _FSL_DPDMAI_CMD_H */

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
#ifndef _FSL_DPSECI_CMD_H
#define _FSL_DPSECI_CMD_H

/* DPSECI Version */
#define DPSECI_VER_MAJOR		5
#define DPSECI_VER_MINOR		1

/* Command versioning */
#define DPSECI_CMD_BASE_VERSION		1
#define DPSECI_CMD_BASE_VERSION_V2	2
#define DPSECI_CMD_ID_OFFSET		4

#define DPSECI_CMD_V1(id)	((id << DPSECI_CMD_ID_OFFSET) | DPSECI_CMD_BASE_VERSION)
#define DPSECI_CMD_V2(id)	((id << DPSECI_CMD_ID_OFFSET) | DPSECI_CMD_BASE_VERSION_V2)

/* Command IDs */
#define DPSECI_CMDID_CLOSE		DPSECI_CMD_V1(0x800)
#define DPSECI_CMDID_OPEN		DPSECI_CMD_V1(0x809)
#define DPSECI_CMDID_CREATE		DPSECI_CMD_V2(0x909)
#define DPSECI_CMDID_DESTROY		DPSECI_CMD_V1(0x989)
#define DPSECI_CMDID_GET_API_VERSION	DPSECI_CMD_V1(0xa09)
#define DPSECI_CMDID_GET_ATTR		DPSECI_CMD_V1(0x004)
#define DPSECI_CMDID_GET_IRQ_MASK	DPSECI_CMD_V1(0x015)
#define DPSECI_CMDID_GET_IRQ_STATUS	DPSECI_CMD_V1(0x016)
#define DPSECI_CMDID_GET_TX_QUEUE	DPSECI_CMD_V1(0x197)

/* Macros for accessing command fields smaller than 1byte */
#define DPSECI_MASK(field)        \
	GENMASK(DPSECI_##field##_SHIFT + DPSECI_##field##_SIZE - 1, \
		DPSECI_##field##_SHIFT)
#define dpseci_set_field(var, field, val) \
	((var) |= (((val) << DPSECI_##field##_SHIFT) & DPSECI_MASK(field)))
#define dpseci_get_field(var, field)      \
	(((var) & DPSECI_MASK(field)) >> DPSECI_##field##_SHIFT)

#pragma pack(push, 1)
struct dpseci_cmd_open {
	uint32_t dpseci_id;
};

struct dpseci_cmd_create {
	uint8_t priorities[8];
	uint8_t num_tx_queues;
	uint8_t num_rx_queues;
	uint8_t pad[6];
	uint32_t options;
};

struct dpseci_cmd_destroy {
	uint32_t dpseci_id;
};

struct dpseci_cmd_get_irq {
	uint32_t pad;
	uint8_t irq_index;
};

struct dpseci_rsp_get_irq_mask {
	uint32_t mask;
};

struct dpseci_cmd_irq_status {
	uint32_t status;
	uint8_t irq_index;
};

struct dpseci_rsp_get_irq_status {
	uint32_t status;
};

struct dpseci_rsp_get_attr {
	uint32_t id;
	uint32_t pad;
	uint8_t num_tx_queues;
	uint8_t num_rx_queues;
	uint8_t pad1[6];
	uint32_t options;
};

struct dpseci_cmd_get_queue {
	uint8_t pad[5];
	uint8_t queue;
};

struct dpseci_rsp_get_tx_queue {
	uint32_t pad;
	uint32_t fqid;
	uint8_t priority;
};

struct dpseci_rsp_get_api_version {
	uint16_t major;
	uint16_t minor;
};

#pragma pack(pop)
#endif /* _FSL_DPSECI_CMD_H */

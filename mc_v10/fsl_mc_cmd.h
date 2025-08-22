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
#ifndef __FSL_MC_CMD_H
#define __FSL_MC_CMD_H

#include "fsl_mc_sys.h"
#include <endian.h>

#define MC_CMD_NUM_OF_PARAMS	7

#define u64	uint64_t
#define u32	uint32_t
#define u16	uint16_t
#define u8	uint8_t

#define cpu_to_le64	htole64
#define cpu_to_le32	htole32
#define cpu_to_le16	htole16

#define le64_to_cpu	le64toh
#define le32_to_cpu	le32toh
#define le16_to_cpu	le16toh

#ifndef BITS_PER_LONG
#define BITS_PER_LONG (sizeof(void *) * 8)
#endif

#define GENMASK(h, l) \
	(((~(unsigned long)0) - ((unsigned long)1 << (l)) + 1) & \
	 (~(unsigned long)0 >> (BITS_PER_LONG - 1 - (h))))

struct mc_cmd_header {
	union {
		struct {
			uint8_t src_id;
			uint8_t flags_hw;
			uint8_t status;
			uint8_t flags_sw;
			uint16_t token;
			uint16_t cmd_id;
		};
		uint32_t word[2];
	};
};

struct mc_command {
	uint64_t header;
	uint64_t params[MC_CMD_NUM_OF_PARAMS];
};

struct mc_rsp_create {
	uint32_t object_id;
};

enum mc_cmd_status {
	MC_CMD_STATUS_OK = 0x0, /* Completed successfully */
	MC_CMD_STATUS_READY = 0x1, /* Ready to be processed */
	MC_CMD_STATUS_AUTH_ERR = 0x3, /* Authentication error */
	MC_CMD_STATUS_NO_PRIVILEGE = 0x4, /* No privilege */
	MC_CMD_STATUS_DMA_ERR = 0x5, /* DMA or I/O error */
	MC_CMD_STATUS_CONFIG_ERR = 0x6, /* Configuration error */
	MC_CMD_STATUS_TIMEOUT = 0x7, /* Operation timed out */
	MC_CMD_STATUS_NO_RESOURCE = 0x8, /* No resources */
	MC_CMD_STATUS_NO_MEMORY = 0x9, /* No memory available */
	MC_CMD_STATUS_BUSY = 0xA, /* Device is busy */
	MC_CMD_STATUS_UNSUPPORTED_OP = 0xB, /* Unsupported operation */
	MC_CMD_STATUS_INVALID_STATE = 0xC /* Invalid state */
};

/*
 * MC command flags
 */

/* High priority flag */
#define MC_CMD_FLAG_PRI		0x80
/* Command completion flag */
#define MC_CMD_FLAG_INTR_DIS	0x01

#define MC_CMD_HDR_FLAGS_MASK	0xFF00FF00

static inline uint64_t mc_encode_cmd_header(uint16_t cmd_id,
					    uint32_t cmd_flags,
					    uint16_t token)
{
	uint64_t header = 0;
	struct mc_cmd_header *hdr = (struct mc_cmd_header *)&header;

	hdr->cmd_id = cpu_to_le16(cmd_id);
	hdr->token = cpu_to_le16(token);
	hdr->status = MC_CMD_STATUS_READY;
	hdr->word[0] |= cpu_to_le32(cmd_flags & MC_CMD_HDR_FLAGS_MASK);

	return header;
}

static inline uint16_t mc_cmd_hdr_read_token(struct mc_command *cmd)
{
	struct mc_cmd_header *hdr = (struct mc_cmd_header *)&cmd->header;
	uint16_t token = le16_to_cpu(hdr->token);

	return token;
}

static inline uint32_t mc_cmd_read_object_id(struct mc_command *cmd)
{
	struct mc_rsp_create *rsp_params;

	rsp_params = (struct mc_rsp_create *)cmd->params;
	return le32_to_cpu(rsp_params->object_id);
}

static inline enum mc_cmd_status mc_cmd_read_status(struct mc_command *cmd)
{
	struct mc_cmd_header *hdr = (struct mc_cmd_header *)&cmd->header;
	uint8_t status = hdr->status;

	return (enum mc_cmd_status)status;
}

#endif /* __FSL_MC_CMD_H */

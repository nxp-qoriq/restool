/*
 * Freescale Layerscape Management Complex commands common declarations
 *
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#ifndef __FSL_MC_CMD_COMMON_H
#define __FSL_MC_CMD_COMMON_H

#define MC_CMD_NUM_OF_PARAMS	7

struct mc_cmd_data {
	uint64_t params[MC_CMD_NUM_OF_PARAMS];
};

struct mc_command {
	uint64_t header;
	struct mc_cmd_data data;
};

enum mc_cmd_status {
	MC_CMD_STATUS_OK = 0x0, /**< passed */
	MC_CMD_STATUS_READY = 0x1, /**< Ready to be processed */
	MC_CMD_STATUS_AUTH_ERR = 0x3, /**< Authentication error */
	MC_CMD_STATUS_NO_PRIVILEGE = 0x4,
	MC_CMD_STATUS_DMA_ERR = 0x5,
	MC_CMD_STATUS_CONFIG_ERR = 0x6,
	MC_CMD_STATUS_TIMEOUT = 0x7,
	MC_CMD_STATUS_NO_RESOURCE = 0x8,
	MC_CMD_STATUS_NO_MEMORY = 0x9,
	MC_CMD_STATUS_BUSY = 0xA,
	MC_CMD_STATUS_UNSUPPORTED_OP = 0xB,
	MC_CMD_STATUS_INVALID_STATE = 0xC
};

/*
 * MC command priorities
 */
enum mc_cmd_priorities {
	CMDIF_PRI_LOW =		0,  /* Low Priority command indication */
	CMDIF_PRI_HIGH =	1   /* High Priority command indication */
};

static inline uint64_t mc_encode_cmd_header(
	uint16_t cmd_id,
	uint8_t cmd_size,
	uint8_t priority,
	uint16_t auth_id)
{
	uint64_t header;
	header = (cmd_id & 0xfffULL) << 52;
	header |= (auth_id & 0x3ffULL) << 38;
	header |= (cmd_size & 0x3fULL) << 31;
	header |= (priority & 0x1ULL) << 15;
	header |= (MC_CMD_STATUS_READY & 0xff) << 16;
	return header;
}
#endif /* __FSL_MC_CMD_COMMON_H */

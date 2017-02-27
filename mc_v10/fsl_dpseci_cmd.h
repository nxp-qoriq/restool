/* Copyright 2013-2017 Freescale Semiconductor Inc.
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
#define DPSECI_VER_MINOR		0

/* Command IDs */
#define DPSECI_CMDID_CREATE_V1		((0x909 << 4) | (0x1))
#define DPSECI_CMDID_CREATE_V2		((0x909 << 4) | (0x2))
#define DPSECI_CMDID_DESTROY		((0x989 << 4) | (0x1))
#define DPSECI_CMDID_GET_VERSION	((0xa09 << 4) | (0x1))
#define DPSECI_CMDID_GET_ATTR		((0x004 << 4) | (0x1))

/*                cmd, param, offset, width, type, arg_name */
#define DPSECI_CMD_CREATE_V1(cmd, cfg) \
do { \
	MC_CMD_OP(cmd, 0, 0,  8,  uint8_t,  cfg->priorities[0]);\
	MC_CMD_OP(cmd, 0, 8,  8,  uint8_t,  cfg->priorities[1]);\
	MC_CMD_OP(cmd, 0, 16, 8,  uint8_t,  cfg->priorities[2]);\
	MC_CMD_OP(cmd, 0, 24, 8,  uint8_t,  cfg->priorities[3]);\
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  cfg->priorities[4]);\
	MC_CMD_OP(cmd, 0, 40, 8,  uint8_t,  cfg->priorities[5]);\
	MC_CMD_OP(cmd, 0, 48, 8,  uint8_t,  cfg->priorities[6]);\
	MC_CMD_OP(cmd, 0, 56, 8,  uint8_t,  cfg->priorities[7]);\
	MC_CMD_OP(cmd, 1, 0,  8,  uint8_t,  cfg->num_tx_queues);\
	MC_CMD_OP(cmd, 1, 8,  8,  uint8_t,  cfg->num_rx_queues);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPSECI_CMD_CREATE_V2(cmd, cfg) \
do { \
	MC_CMD_OP(cmd, 0, 0,  8,  uint8_t,  cfg->priorities[0]);\
	MC_CMD_OP(cmd, 0, 8,  8,  uint8_t,  cfg->priorities[1]);\
	MC_CMD_OP(cmd, 0, 16, 8,  uint8_t,  cfg->priorities[2]);\
	MC_CMD_OP(cmd, 0, 24, 8,  uint8_t,  cfg->priorities[3]);\
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  cfg->priorities[4]);\
	MC_CMD_OP(cmd, 0, 40, 8,  uint8_t,  cfg->priorities[5]);\
	MC_CMD_OP(cmd, 0, 48, 8,  uint8_t,  cfg->priorities[6]);\
	MC_CMD_OP(cmd, 0, 56, 8,  uint8_t,  cfg->priorities[7]);\
	MC_CMD_OP(cmd, 1, 0,  8,  uint8_t,  cfg->num_tx_queues);\
	MC_CMD_OP(cmd, 1, 8,  8,  uint8_t,  cfg->num_rx_queues);\
	MC_CMD_OP(cmd, 2, 0,  32, uint32_t, cfg->options);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPSECI_RSP_GET_ATTR(cmd, attr) \
do { \
	MC_RSP_OP(cmd, 0, 0,  32, int,	    attr->id); \
	MC_RSP_OP(cmd, 1, 0,  8,  uint8_t,  attr->num_tx_queues); \
	MC_RSP_OP(cmd, 1, 8,  8,  uint8_t,  attr->num_rx_queues); \
} while (0)

/*                cmd, param, offset, width, type,      arg_name */
#define DPSECI_RSP_GET_VERSION(cmd, major, minor) \
do { \
	MC_RSP_OP(cmd, 0, 0,  16, uint16_t, major);\
	MC_RSP_OP(cmd, 0, 16, 16, uint16_t, minor);\
} while (0)

#endif /* _FSL_DPSECI_CMD_H */

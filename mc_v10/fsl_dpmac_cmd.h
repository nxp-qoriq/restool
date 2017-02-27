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
#ifndef _FSL_DPMAC_CMD_H
#define _FSL_DPMAC_CMD_H

/* DPMAC Version */
#define DPMAC_VER_MAJOR			4
#define DPMAC_VER_MINOR			2

/* Command IDs */
#define DPMAC_CMDID_CREATE		((0x90c << 4) | (0x1))
#define DPMAC_CMDID_DESTROY		((0x98c << 4) | (0x1))
#define DPMAC_CMDID_GET_VERSION		((0xa0c << 4) | (0x1))
#define DPMAC_CMDID_GET_ATTR		((0x004 << 4) | (0x1))

/*                cmd, param, offset, width, type, arg_name */
#define DPMAC_CMD_CREATE(cmd, cfg) \
	MC_CMD_OP(cmd, 0, 0,  16, uint16_t,      cfg->mac_id)

/*                cmd, param, offset, width, type,	arg_name */
#define DPMAC_RSP_GET_ATTRIBUTES(cmd, attr) \
do { \
	MC_RSP_OP(cmd, 0, 0,  16, uint16_t,		attr->phy_id);\
	MC_RSP_OP(cmd, 0, 16, 16, uint16_t,		attr->id);\
	MC_RSP_OP(cmd, 0, 32, 32, uint32_t,		attr->max_rate);\
	MC_RSP_OP(cmd, 1, 0,  8,  uint8_t,		attr->eth_if);\
	MC_RSP_OP(cmd, 1, 8,  8,  uint8_t,		attr->link_type);\
} while (0)

/*                cmd, param, offset, width, type,      arg_name */
#define DPMAC_RSP_GET_VERSION(cmd, major, minor) \
do { \
	MC_RSP_OP(cmd, 0, 0,  16, uint16_t, major);\
	MC_RSP_OP(cmd, 0, 16, 16, uint16_t, minor);\
} while (0)

#endif /* _FSL_DPMAC_CMD_H */

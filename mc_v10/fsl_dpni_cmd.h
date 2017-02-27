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
#ifndef _FSL_DPNI_CMD_v10_H
#define _FSL_DPNI_CMD_v10_H

/* DPNI Version */
#define DPNI_VER_MAJOR			7
#define DPNI_VER_MINOR			0

/* Command IDs */
#define DPNI_CMDID_CREATE		((0x901 << 4) | (0x1))
#define DPNI_CMDID_DESTROY		((0x981 << 4) | (0x1))
#define DPNI_CMDID_GET_VERSION		((0xa01 << 4) | (0x1))
#define DPNI_CMDID_GET_ATTR		((0x004 << 4) | (0x1))
#define DPNI_CMDID_SET_PRIM_MAC		((0x224 << 4) | (0x1))
#define DPNI_CMDID_GET_STATISTICS	((0x25D << 4) | (0x1))

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_CREATE(cmd, cfg) \
do { \
	MC_CMD_OP(cmd, 0,  0, 32, uint32_t,  (cfg)->options); \
	MC_CMD_OP(cmd, 0, 32,  8,  uint8_t,  (cfg)->num_queues); \
	MC_CMD_OP(cmd, 0, 40,  8,  uint8_t,  (cfg)->num_tcs); \
	MC_CMD_OP(cmd, 0, 48,  8,  uint8_t,  (cfg)->mac_filter_entries); \
	MC_CMD_OP(cmd, 1,  0,  8,  uint8_t,  (cfg)->vlan_filter_entries); \
	MC_CMD_OP(cmd, 1, 16,  8,  uint8_t,  (cfg)->qos_entries); \
	MC_CMD_OP(cmd, 1, 32, 16, uint16_t,  (cfg)->fs_entries); \
} while (0)

#define DPNI_RSP_GET_ATTR(cmd, attr) \
do { \
	MC_RSP_OP(cmd, 0,  0, 32, uint32_t, (attr)->options); \
	MC_RSP_OP(cmd, 0, 32,  8, uint8_t,  (attr)->num_queues); \
	MC_RSP_OP(cmd, 0, 40,  8, uint8_t,  (attr)->num_tcs); \
	MC_RSP_OP(cmd, 0, 48,  8, uint8_t,  (attr)->mac_filter_entries); \
	MC_RSP_OP(cmd, 1,  0,  8, uint8_t, (attr)->vlan_filter_entries); \
	MC_RSP_OP(cmd, 1, 16,  8, uint8_t,  (attr)->qos_entries); \
	MC_RSP_OP(cmd, 1, 32, 16, uint16_t, (attr)->fs_entries); \
	MC_RSP_OP(cmd, 2,  0,  8, uint8_t,  (attr)->qos_key_size); \
	MC_RSP_OP(cmd, 2,  8,  8, uint8_t,  (attr)->fs_key_size); \
} while (0)

#define DPNI_CMD_GET_STATISTICS(cmd, page) \
do { \
	MC_CMD_OP(cmd, 0, 0, 8, uint8_t, page); \
} while (0)

#define DPNI_RSP_GET_STATISTICS(cmd, stat) \
do { \
	MC_RSP_OP(cmd, 0, 0, 64, uint64_t, (stat)->raw.counter0); \
	MC_RSP_OP(cmd, 1, 0, 64, uint64_t, (stat)->raw.counter1); \
	MC_RSP_OP(cmd, 2, 0, 64, uint64_t, (stat)->raw.counter2); \
	MC_RSP_OP(cmd, 3, 0, 64, uint64_t, (stat)->raw.counter3); \
	MC_RSP_OP(cmd, 4, 0, 64, uint64_t, (stat)->raw.counter4); \
	MC_RSP_OP(cmd, 5, 0, 64, uint64_t, (stat)->raw.counter5); \
	MC_RSP_OP(cmd, 6, 0, 64, uint64_t, (stat)->raw.counter6); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_PRIMARY_MAC_ADDR(cmd, mac_addr) \
do { \
	MC_CMD_OP(cmd, 0, 16, 8,  uint8_t,  mac_addr[5]); \
	MC_CMD_OP(cmd, 0, 24, 8,  uint8_t,  mac_addr[4]); \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  mac_addr[3]); \
	MC_CMD_OP(cmd, 0, 40, 8,  uint8_t,  mac_addr[2]); \
	MC_CMD_OP(cmd, 0, 48, 8,  uint8_t,  mac_addr[1]); \
	MC_CMD_OP(cmd, 0, 56, 8,  uint8_t,  mac_addr[0]); \
} while (0)

#define DPNI_RSP_GET_VERSION(cmd, major, minor) \
do { \
	MC_RSP_OP(cmd, 0, 0,  16, uint16_t, major);\
	MC_RSP_OP(cmd, 0, 16, 16, uint16_t, minor);\
} while (0)

#endif /* _FSL_DPNI_CMD_v10_H */

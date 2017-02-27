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
#ifndef __FSL_DPSW_CMD_H
#define __FSL_DPSW_CMD_H

/* DPSW Version */
#define DPSW_VER_MAJOR			8
#define DPSW_VER_MINOR			0

/* Command IDs */
#define DPSW_CMDID_CREATE		((0x902 << 4) | (0x1))
#define DPSW_CMDID_DESTROY		((0x982 << 4) | (0x1))
#define DPSW_CMDID_GET_VERSION		((0xa02 << 4) | (0x1))
#define DPSW_CMDID_GET_ATTR		((0x004 << 4) | (0x1))

/*                cmd, param, offset, width, type, arg_name */
#define DPSW_CMD_CREATE(cmd, cfg) \
do { \
	MC_CMD_OP(cmd, 0, 0,  16, uint16_t, cfg->num_ifs);\
	MC_CMD_OP(cmd, 0, 16,  8, uint8_t,  cfg->adv.max_fdbs);\
	MC_CMD_OP(cmd, 0, 24,  8, uint8_t,  cfg->adv.max_meters_per_if);\
	MC_CMD_OP(cmd, 0, 32,  4, enum dpsw_component_type,  \
			cfg->adv.component_type);\
	MC_CMD_OP(cmd, 1, 0,  16, uint16_t, cfg->adv.max_vlans);\
	MC_CMD_OP(cmd, 1, 16, 16, uint16_t, cfg->adv.max_fdb_entries);\
	MC_CMD_OP(cmd, 1, 32, 16, uint16_t, cfg->adv.fdb_aging_time);\
	MC_CMD_OP(cmd, 1, 48, 16, uint16_t, cfg->adv.max_fdb_mc_groups);\
	MC_CMD_OP(cmd, 2, 0,  64, uint64_t, cfg->adv.options);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPSW_RSP_GET_ATTR(cmd, attr) \
do { \
	MC_RSP_OP(cmd, 0, 0,  16, uint16_t, attr->num_ifs);\
	MC_RSP_OP(cmd, 0, 16, 8,  uint8_t,  attr->max_fdbs);\
	MC_RSP_OP(cmd, 0, 24, 8,  uint8_t,  attr->num_fdbs);\
	MC_RSP_OP(cmd, 0, 32, 16, uint16_t, attr->max_vlans);\
	MC_RSP_OP(cmd, 0, 48, 16, uint16_t, attr->num_vlans);\
	MC_RSP_OP(cmd, 1, 0, 16, uint16_t, attr->max_fdb_entries);\
	MC_RSP_OP(cmd, 1, 16, 16, uint16_t, attr->fdb_aging_time);\
	MC_RSP_OP(cmd, 1, 32,  32, int,	 attr->id);\
	MC_RSP_OP(cmd, 2, 0, 16, uint16_t, attr->mem_size);\
	MC_RSP_OP(cmd, 2, 16, 16, uint16_t, attr->max_fdb_mc_groups);\
	MC_RSP_OP(cmd, 2, 32,  8,  uint8_t, attr->max_meters_per_if);\
	MC_RSP_OP(cmd, 2, 40,  4,  enum dpsw_component_type, \
			attr->component_type);\
	MC_RSP_OP(cmd, 3, 0,  64, uint64_t, attr->options);\
} while (0)

/*                cmd, param, offset, width, type,      arg_name */
#define DPSW_RSP_GET_VERSION(cmd, major, minor) \
do { \
	MC_RSP_OP(cmd, 0, 0,  16, uint16_t, major);\
	MC_RSP_OP(cmd, 0, 16, 16, uint16_t, minor);\
} while (0)

#endif /* __FSL_DPSW_CMD_H */

/* Copyright 2013-2015 Freescale Semiconductor Inc.
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
#ifndef _FSL_DPDMUX_CMD_V9_H
#define _FSL_DPDMUX_CMD_V9_H

/* DPDMUX Version */
#define DPDMUX_VER_MAJOR		5
#define DPDMUX_VER_MINOR		0

/* Command IDs */
#define DPDMUX_CMDID_CLOSE		0x800
#define DPDMUX_CMDID_OPEN		0x806
#define DPDMUX_CMDID_CREATE		0x906
#define DPDMUX_CMDID_DESTROY		0x900

#define DPDMUX_CMDID_GET_ATTR		0x004

#define DPDMUX_CMDID_GET_IRQ_MASK	0x015
#define DPDMUX_CMDID_GET_IRQ_STATUS	0x016

#define DPDMUX_CMDID_IF_GET_ATTR	0x0a8

/*                cmd, param, offset, width, type, arg_name */
#define DPDMUX_CMD_OPEN(cmd, dpdmux_id) \
	MC_CMD_OP(cmd, 0, 0,  32,  int,	dpdmux_id)

/*                cmd, param, offset, width, type, arg_name */
#define DPDMUX_CMD_CREATE_V9(cmd, cfg) \
do { \
	MC_CMD_OP(cmd, 0, 0,  8,  enum dpdmux_method, cfg->method);\
	MC_CMD_OP(cmd, 0, 8,  8,  enum dpdmux_manip, cfg->manip);\
	MC_CMD_OP(cmd, 0, 16, 16, uint16_t, cfg->num_ifs);\
	MC_CMD_OP(cmd, 1, 0,  16, uint16_t, cfg->adv.max_dmat_entries);\
	MC_CMD_OP(cmd, 1, 16, 16, uint16_t, cfg->adv.max_mc_groups);\
	MC_CMD_OP(cmd, 1, 32, 16, uint16_t, cfg->adv.max_vlan_ids);\
	MC_CMD_OP(cmd, 2, 0,  64, uint64_t, cfg->adv.options);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPDMUX_CMD_GET_IRQ_MASK(cmd, irq_index) \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index)

/*                cmd, param, offset, width, type, arg_name */
#define DPDMUX_RSP_GET_IRQ_MASK(cmd, mask) \
	MC_RSP_OP(cmd, 0, 0,  32, uint32_t, mask)

/*                cmd, param, offset, width, type, arg_name */
#define DPDMUX_CMD_GET_IRQ_STATUS(cmd, irq_index, status) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, uint32_t, status);\
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPDMUX_RSP_GET_IRQ_STATUS(cmd, status) \
	MC_RSP_OP(cmd, 0, 0,  32, uint32_t, status) \

#define DPDMUX_RSP_GET_ATTR_V9(cmd, attr) \
do { \
	MC_RSP_OP(cmd, 0, 0,  8,  enum dpdmux_method, attr->method);\
	MC_RSP_OP(cmd, 0, 8,  8,  enum dpdmux_manip, attr->manip);\
	MC_RSP_OP(cmd, 0, 16, 16, uint16_t, attr->num_ifs);\
	MC_RSP_OP(cmd, 0, 32, 16, uint16_t, attr->mem_size);\
	MC_RSP_OP(cmd, 2, 0,  32, int,	    attr->id);\
	MC_RSP_OP(cmd, 3, 0,  64, uint64_t, attr->options);\
	MC_RSP_OP(cmd, 4, 0,  16, uint16_t, attr->version.major);\
	MC_RSP_OP(cmd, 4, 16, 16, uint16_t, attr->version.minor);\
} while (0)

#endif /* _FSL_DPDMUX_CMD_V9_H */

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
#ifndef _FSL_DPNI_CMD_V9_H
#define _FSL_DPNI_CMD_V9_H

/* DPNI Version */
#define DPNI_VER_MAJOR				6
#define DPNI_VER_MINOR				0

/* Command IDs */
#define DPNI_CMDID_OPEN				0x801
#define DPNI_CMDID_CLOSE			0x800
#define DPNI_CMDID_CREATE			0x901
#define DPNI_CMDID_DESTROY			0x900

#define DPNI_CMDID_GET_ATTR			0x004

#define DPNI_CMDID_GET_IRQ_MASK			0x015
#define DPNI_CMDID_GET_IRQ_STATUS		0x016

#define DPNI_CMDID_GET_LINK_STATE		0x215
#define DPNI_CMDID_GET_PRIM_MAC			0x225

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_OPEN(cmd, dpni_id) \
	MC_CMD_OP(cmd,	 0,	0,	32,	int,	dpni_id)

#define DPNI_PREP_EXTENDED_CFG(ext, cfg) \
do { \
	MC_PREP_OP(ext, 0, 0,   16, uint16_t, cfg->tc_cfg[0].max_dist); \
	MC_PREP_OP(ext, 0, 16,  16, uint16_t, cfg->tc_cfg[0].max_fs_entries); \
	MC_PREP_OP(ext, 0, 32,  16, uint16_t, cfg->tc_cfg[1].max_dist); \
	MC_PREP_OP(ext, 0, 48,  16, uint16_t, cfg->tc_cfg[1].max_fs_entries); \
	MC_PREP_OP(ext, 1, 0,   16, uint16_t, cfg->tc_cfg[2].max_dist); \
	MC_PREP_OP(ext, 1, 16,  16, uint16_t, cfg->tc_cfg[2].max_fs_entries); \
	MC_PREP_OP(ext, 1, 32,  16, uint16_t, cfg->tc_cfg[3].max_dist); \
	MC_PREP_OP(ext, 1, 48,  16, uint16_t, cfg->tc_cfg[3].max_fs_entries); \
	MC_PREP_OP(ext, 2, 0,   16, uint16_t, cfg->tc_cfg[4].max_dist); \
	MC_PREP_OP(ext, 2, 16,  16, uint16_t, cfg->tc_cfg[4].max_fs_entries); \
	MC_PREP_OP(ext, 2, 32,  16, uint16_t, cfg->tc_cfg[5].max_dist); \
	MC_PREP_OP(ext, 2, 48,  16, uint16_t, cfg->tc_cfg[5].max_fs_entries); \
	MC_PREP_OP(ext, 3, 0,   16, uint16_t, cfg->tc_cfg[6].max_dist); \
	MC_PREP_OP(ext, 3, 16,  16, uint16_t, cfg->tc_cfg[6].max_fs_entries); \
	MC_PREP_OP(ext, 3, 32,  16, uint16_t, cfg->tc_cfg[7].max_dist); \
	MC_PREP_OP(ext, 3, 48,  16, uint16_t, cfg->tc_cfg[7].max_fs_entries); \
	MC_PREP_OP(ext, 4, 0,   16, uint16_t, \
		   cfg->ipr_cfg.max_open_frames_ipv4); \
	MC_PREP_OP(ext, 4, 16,  16, uint16_t, \
		   cfg->ipr_cfg.max_open_frames_ipv6); \
	MC_PREP_OP(ext, 4, 32,  16, uint16_t, \
		   cfg->ipr_cfg.max_reass_frm_size); \
	MC_PREP_OP(ext, 5, 0,   16, uint16_t, \
		   cfg->ipr_cfg.min_frag_size_ipv4); \
	MC_PREP_OP(ext, 5, 16,  16, uint16_t, \
		   cfg->ipr_cfg.min_frag_size_ipv6); \
} while (0)

#define DPNI_EXT_EXTENDED_CFG(ext, cfg) \
do { \
	MC_EXT_OP(ext, 0, 0,   16, uint16_t, cfg->tc_cfg[0].max_dist); \
	MC_EXT_OP(ext, 0, 16,  16, uint16_t, cfg->tc_cfg[0].max_fs_entries); \
	MC_EXT_OP(ext, 0, 32,  16, uint16_t, cfg->tc_cfg[1].max_dist); \
	MC_EXT_OP(ext, 0, 48,  16, uint16_t, cfg->tc_cfg[1].max_fs_entries); \
	MC_EXT_OP(ext, 1, 0,   16, uint16_t, cfg->tc_cfg[2].max_dist); \
	MC_EXT_OP(ext, 1, 16,  16, uint16_t, cfg->tc_cfg[2].max_fs_entries); \
	MC_EXT_OP(ext, 1, 32,  16, uint16_t, cfg->tc_cfg[3].max_dist); \
	MC_EXT_OP(ext, 1, 48,  16, uint16_t, cfg->tc_cfg[3].max_fs_entries); \
	MC_EXT_OP(ext, 2, 0,   16, uint16_t, cfg->tc_cfg[4].max_dist); \
	MC_EXT_OP(ext, 2, 16,  16, uint16_t, cfg->tc_cfg[4].max_fs_entries); \
	MC_EXT_OP(ext, 2, 32,  16, uint16_t, cfg->tc_cfg[5].max_dist); \
	MC_EXT_OP(ext, 2, 48,  16, uint16_t, cfg->tc_cfg[5].max_fs_entries); \
	MC_EXT_OP(ext, 3, 0,   16, uint16_t, cfg->tc_cfg[6].max_dist); \
	MC_EXT_OP(ext, 3, 16,  16, uint16_t, cfg->tc_cfg[6].max_fs_entries); \
	MC_EXT_OP(ext, 3, 32,  16, uint16_t, cfg->tc_cfg[7].max_dist); \
	MC_EXT_OP(ext, 3, 48,  16, uint16_t, cfg->tc_cfg[7].max_fs_entries); \
	MC_EXT_OP(ext, 4, 0,   16, uint16_t, \
		  cfg->ipr_cfg.max_open_frames_ipv4); \
	MC_EXT_OP(ext, 4, 16,  16, uint16_t, \
		  cfg->ipr_cfg.max_open_frames_ipv6); \
	MC_EXT_OP(ext, 4, 32,  16, uint16_t, \
		  cfg->ipr_cfg.max_reass_frm_size); \
	MC_EXT_OP(ext, 5, 0,   16, uint16_t, \
		  cfg->ipr_cfg.min_frag_size_ipv4); \
	MC_EXT_OP(ext, 5, 16,  16, uint16_t, \
		  cfg->ipr_cfg.min_frag_size_ipv6); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_CREATE_V9(cmd, cfg) \
do { \
	MC_CMD_OP(cmd, 0, 0,	8,  uint8_t,  cfg->adv.max_tcs); \
	MC_CMD_OP(cmd, 0, 8,	8,  uint8_t,  cfg->adv.max_senders); \
	MC_CMD_OP(cmd, 0, 16,	8,  uint8_t,  cfg->mac_addr[5]); \
	MC_CMD_OP(cmd, 0, 24,	8,  uint8_t,  cfg->mac_addr[4]); \
	MC_CMD_OP(cmd, 0, 32,	8,  uint8_t,  cfg->mac_addr[3]); \
	MC_CMD_OP(cmd, 0, 40,	8,  uint8_t,  cfg->mac_addr[2]); \
	MC_CMD_OP(cmd, 0, 48,	8,  uint8_t,  cfg->mac_addr[1]); \
	MC_CMD_OP(cmd, 0, 56,	8,  uint8_t,  cfg->mac_addr[0]); \
	MC_CMD_OP(cmd, 1, 0,	32, uint32_t, cfg->adv.options); \
	MC_CMD_OP(cmd, 2, 0,	8,  uint8_t,  cfg->adv.max_unicast_filters); \
	MC_CMD_OP(cmd, 2, 8,	8,  uint8_t,  cfg->adv.max_multicast_filters); \
	MC_CMD_OP(cmd, 2, 16,	8,  uint8_t,  cfg->adv.max_vlan_filters); \
	MC_CMD_OP(cmd, 2, 24,	8,  uint8_t,  cfg->adv.max_qos_entries); \
	MC_CMD_OP(cmd, 2, 32,	8,  uint8_t,  cfg->adv.max_qos_key_size); \
	MC_CMD_OP(cmd, 2, 48,	8,  uint8_t,  cfg->adv.max_dist_key_size); \
	MC_CMD_OP(cmd, 2, 56,	8,  enum net_prot, cfg->adv.start_hdr); \
	MC_CMD_OP(cmd, 4, 48,	8,  uint8_t, cfg->adv.max_policers); \
	MC_CMD_OP(cmd, 4, 56,	8,  uint8_t, cfg->adv.max_congestion_ctrl); \
	MC_CMD_OP(cmd, 5, 0,	64, uint64_t, cfg->adv.ext_cfg_iova); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_GET_IRQ_MASK(cmd, irq_index) \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_IRQ_MASK(cmd, mask) \
	MC_RSP_OP(cmd, 0, 0,  32, uint32_t,  mask)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_GET_IRQ_STATUS(cmd, irq_index, status) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, uint32_t, status);\
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_IRQ_STATUS(cmd, status) \
	MC_RSP_OP(cmd, 0, 0,  32, uint32_t,  status)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_GET_ATTR_V9(cmd, attr) \
	MC_CMD_OP(cmd, 6, 0,  64, uint64_t, attr->ext_cfg_iova)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_ATTR_V9(cmd, attr) \
do { \
	MC_RSP_OP(cmd, 0, 0,  32, int,	    attr->id);\
	MC_RSP_OP(cmd, 0, 32, 8,  uint8_t,  attr->max_tcs); \
	MC_RSP_OP(cmd, 0, 40, 8,  uint8_t,  attr->max_senders); \
	MC_RSP_OP(cmd, 0, 48, 8,  enum net_prot, attr->start_hdr); \
	MC_RSP_OP(cmd, 1, 0,  32, uint32_t, attr->options); \
	MC_RSP_OP(cmd, 2, 0,  8,  uint8_t,  attr->max_unicast_filters); \
	MC_RSP_OP(cmd, 2, 8,  8,  uint8_t,  attr->max_multicast_filters);\
	MC_RSP_OP(cmd, 2, 16, 8,  uint8_t,  attr->max_vlan_filters); \
	MC_RSP_OP(cmd, 2, 24, 8,  uint8_t,  attr->max_qos_entries); \
	MC_RSP_OP(cmd, 2, 32, 8,  uint8_t,  attr->max_qos_key_size); \
	MC_RSP_OP(cmd, 2, 40, 8,  uint8_t,  attr->max_dist_key_size); \
	MC_RSP_OP(cmd, 4, 48, 8,  uint8_t, attr->max_policers); \
	MC_RSP_OP(cmd, 4, 56, 8,  uint8_t, attr->max_congestion_ctrl); \
	MC_RSP_OP(cmd, 5, 32, 16, uint16_t, attr->version.major);\
	MC_RSP_OP(cmd, 5, 48, 16, uint16_t, attr->version.minor);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_LINK_STATE(cmd, state) \
do { \
	MC_RSP_OP(cmd, 0, 32,  1, int,      state->up);\
	MC_RSP_OP(cmd, 1, 0,  32, uint32_t, state->rate);\
	MC_RSP_OP(cmd, 2, 0,  64, uint64_t, state->options);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_PRIMARY_MAC_ADDR(cmd, mac_addr) \
do { \
	MC_RSP_OP(cmd, 0, 16, 8,  uint8_t,  mac_addr[5]); \
	MC_RSP_OP(cmd, 0, 24, 8,  uint8_t,  mac_addr[4]); \
	MC_RSP_OP(cmd, 0, 32, 8,  uint8_t,  mac_addr[3]); \
	MC_RSP_OP(cmd, 0, 40, 8,  uint8_t,  mac_addr[2]); \
	MC_RSP_OP(cmd, 0, 48, 8,  uint8_t,  mac_addr[1]); \
	MC_RSP_OP(cmd, 0, 56, 8,  uint8_t,  mac_addr[0]); \
} while (0)

#endif /* _FSL_DPNI_CMD_V9_H */

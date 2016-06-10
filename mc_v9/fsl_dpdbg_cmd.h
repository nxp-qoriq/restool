/* Copyright 2013-2015 Freescale Semiconductor Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 * * Neither the name of the above-listed copyright holders nor the
 * names of any contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
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
#ifndef _FSL_DPDBG_CMD_H
#define _FSL_DPDBG_CMD_H

/* DPDBG Version */
#define DPDBG_VER_MAJOR				1
#define DPDBG_VER_MINOR				0

/* Command IDs */
#define DPDBG_CMDID_CLOSE				0x800
#define DPDBG_CMDID_OPEN				0x80F

#define DPDBG_CMDID_GET_ATTR				0x004

#define DPDBG_CMDID_GET_DPNI_INFO			0x130
#define DPDBG_CMDID_GET_DPNI_PRIV_TX_CONF_FQID		0x131
#define DPDBG_CMDID_GET_DPCON_INFO			0x132
#define DPDBG_CMDID_GET_DPBP_INFO			0x133
#define DPDBG_CMDID_GET_DPCI_FQID			0x134

#define DPDBG_CMDID_SET_CTLU_GLOBAL_MARKING		0x135
#define DPDBG_CMDID_SET_DPNI_RX_MARKING			0x136
#define DPDBG_CMDID_SET_DPNI_TX_CONF_MARKING		0x137
#define DPDBG_CMDID_SET_DPIO_MARKING			0x138

#define DPDBG_CMDID_SET_CTLU_GLOBAL_TRACE		0x140
#define DPDBG_CMDID_SET_DPIO_TRACE			0x141
#define DPDBG_CMDID_SET_DPNI_RX_TRACE			0x142
#define DPDBG_CMDID_SET_DPNI_TX_TRACE			0x143
#define DPDBG_CMDID_SET_DPCON_TRACE			0x145
#define DPDBG_CMDID_SET_DPSECI_TRACE			0x146

#define DPDBG_CMDID_GET_DPMAC_COUNTER			0x150
#define DPDBG_CMDID_GET_DPNI_COUNTER			0x151

/*                cmd, param, offset, width, type, arg_name */
#define DPDBG_CMD_OPEN(cmd, dpdbg_id) \
	MC_CMD_OP(cmd, 0, 0,  32, int,	    dpdbg_id)

/*                cmd, param, offset, width, type,	arg_name */
#define DPDBG_RSP_GET_ATTRIBUTES(cmd, attr) \
do { \
	MC_RSP_OP(cmd, 0, 32, 32, int,	    attr->id);\
	MC_RSP_OP(cmd, 1, 0,  16, uint16_t, attr->version.major);\
	MC_RSP_OP(cmd, 1, 16, 16, uint16_t, attr->version.minor);\
} while (0)

/*                cmd, param, offset, width, type,	arg_name */
#define	DPDBG_CMD_GET_DPNI_INFO(cmd, dpni_id) \
	MC_CMD_OP(cmd, 0, 0,  32, int,	    dpni_id)

/*                cmd, param, offset, width, type,	arg_name */
#define	DPDBG_RSP_GET_DPNI_INFO(cmd, info) \
do { \
	MC_RSP_OP(cmd, 1, 0,  32, uint32_t, info->qdid);\
	MC_RSP_OP(cmd, 1, 32, 8,  uint8_t,  info->max_senders);\
	MC_RSP_OP(cmd, 2, 0,  32, uint32_t, info->err_fqid);\
	MC_RSP_OP(cmd, 2, 32, 32, uint32_t, info->tx_conf_fqid);\
} while (0)

/*                cmd, param, offset, width, type,	arg_name */
#define	DPDBG_CMD_GET_DPNI_PRIV_TX_CONF_FQID(cmd, dpni_id, sender_id) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, int,	    dpni_id);\
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  sender_id);\
} while (0)

/*                cmd, param, offset, width, type,	arg_name */
#define	DPDBG_RSP_GET_DPNI_PRIV_TX_CONF_FQID(cmd, fqid) \
	MC_RSP_OP(cmd, 1, 0,  32, uint32_t, fqid)

/*                cmd, param, offset, width, type,	arg_name */
#define	DPDBG_CMD_GET_DPCON_INFO(cmd, dpcon_id) \
	MC_CMD_OP(cmd, 0, 0,  32, int,	    dpcon_id)

/*                cmd, param, offset, width, type,	arg_name */
#define	DPDBG_RSP_GET_DPCON_INFO(cmd, info) \
	MC_RSP_OP(cmd, 1, 0,  16, uint16_t,  info->ch_id)

/*                cmd, param, offset, width, type,	arg_name */
#define	DPDBG_CMD_GET_DPBP_INFO(cmd, dpbp_id) \
	MC_CMD_OP(cmd, 0, 0,  32, int,	    dpbp_id)

/*                cmd, param, offset, width, type,	arg_name */
#define	DPDBG_RSP_GET_DPBP_INFO(cmd, info) \
	MC_RSP_OP(cmd, 1, 0,  16, uint16_t,  info->bpid)

/*                cmd, param, offset, width, type,	arg_name */
#define	DPDBG_CMD_GET_DPCI_FQID(cmd, dpci_id, priority) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, int,	    dpci_id);\
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  priority);\
} while (0)
/*                cmd, param, offset, width, type,	arg_name */
#define	DPDBG_RSP_GET_DPCI_FQID(cmd, fqid) \
	MC_RSP_OP(cmd, 1, 0,  32, uint32_t,  fqid)

/*                cmd, param, offset, width, type,	arg_name */
#define DPDBG_CMD_SET_CTLU_GLOBAL_MARKING(cmd, marking, cfg) \
do { \
	MC_CMD_OP(cmd, 0, 0,  8,  uint8_t,  marking);\
	MC_CMD_OP(cmd, 0, 8,  8,  uint8_t,  cfg->key_size); \
	MC_CMD_OP(cmd, 1, 0,  64, uint64_t, cfg->key_iova); \
	MC_CMD_OP(cmd, 2, 0,  64, uint64_t, cfg->mask_iova); \
	MC_CMD_OP(cmd, 3, 0,  64, uint64_t, cfg->rule_iova); \
} while (0)

#define DPDBG_CMD_SET_DPNI_RX_MARKING(cmd, dpni_id, cfg) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, int,      dpni_id);\
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  cfg->tc_id);\
	MC_CMD_OP(cmd, 0, 48, 16, uint16_t, cfg->flow_id);\
	MC_CMD_OP(cmd, 1, 0,  16, uint16_t, cfg->dpbp_id);\
	MC_CMD_OP(cmd, 1, 16, 8,  uint8_t,  cfg->marking);\
} while (0)

#define DPDBG_CMD_SET_DPNI_TX_CONF_MARKING(cmd, dpni_id, sender_id, marking) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, int,      dpni_id);\
	MC_CMD_OP(cmd, 0, 48, 16, uint16_t, sender_id);\
	MC_CMD_OP(cmd, 1, 16, 8,  uint8_t,  marking);\
} while (0)

#define DPDBG_CMD_SET_DPIO_MARKING(cmd, dpio_id, marking) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, int,      dpio_id);\
	MC_CMD_OP(cmd, 1, 16, 8,  uint8_t,  marking);\
} while (0)

/*                cmd, param, offset, width, type,	arg_name */
#define DPDBG_CMD_SET_CTLU_GLOBAL_TRACE(cmd, cfg) \
do { \
	MC_CMD_OP(cmd, 0, 8,  8,  uint8_t,  cfg->key_size); \
	MC_CMD_OP(cmd, 1, 0,  64, uint64_t, cfg->key_iova); \
	MC_CMD_OP(cmd, 2, 0,  64, uint64_t, cfg->mask_iova); \
	MC_CMD_OP(cmd, 3, 0,  64, uint64_t, cfg->rule_iova); \
} while (0)

/*                cmd, param, offset, width, type,	arg_name */
#define DPDBG_CMD_SET_DPIO_TRACE(cmd, dpio_id, trace_point) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, int,      dpio_id);\
	MC_CMD_OP(cmd, 1, 0,  4,  enum dpdbg_verbosity_level,  \
					  trace_point[0].verbosity); \
	MC_CMD_OP(cmd, 1, 4,  4,  enum dpdbg_dpio_trace_type, \
					  trace_point[0].enqueue_type); \
	MC_CMD_OP(cmd, 1, 8,  8, uint8_t, trace_point[0].marking); \
	MC_CMD_OP(cmd, 1, 32, 4,  enum dpdbg_verbosity_level,  \
					  trace_point[1].verbosity); \
	MC_CMD_OP(cmd, 1, 36, 4,  enum dpdbg_dpio_trace_type, \
					  trace_point[1].enqueue_type); \
	MC_CMD_OP(cmd, 1, 40,  8, uint8_t, trace_point[1].marking); \
} while (0)

/*                cmd, param, offset, width, type,	arg_name */
#define DPDBG_CMD_SET_DPNI_RX_TRACE(cmd, dpni_id, trace_cfg) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, int,      dpni_id);\
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  trace_cfg->tc_id);\
	MC_CMD_OP(cmd, 0, 48, 16, uint16_t, trace_cfg->flow_id);\
	MC_CMD_OP(cmd, 1, 0,  16, uint16_t, trace_cfg->dpbp_id);\
	MC_CMD_OP(cmd, 1, 16, 8,  uint8_t,  trace_cfg->marking);\
} while (0)

/*                cmd, param, offset, width, type,	arg_name */
#define DPDBG_CMD_SET_DPNI_TX_TRACE(cmd, dpni_id, sender_id, trace_cfg) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, int,      dpni_id);\
	MC_CMD_OP(cmd, 0, 48, 16, uint16_t, sender_id);\
	MC_CMD_OP(cmd, 1, 16, 8,  uint8_t,  trace_cfg->marking);\
} while (0)

/*                cmd, param, offset, width, type,	arg_name */
#define DPDBG_CMD_SET_DPCON_TRACE(cmd, dpcon_id, trace_point) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, int,      dpcon_id);\
	MC_CMD_OP(cmd, 1, 0,  4,  enum dpdbg_verbosity_level,  \
					  trace_point[0].verbosity); \
	MC_CMD_OP(cmd, 1, 8,  8, uint8_t, trace_point[0].marking); \
	MC_CMD_OP(cmd, 1, 32, 4,  enum dpdbg_verbosity_level,  \
					  trace_point[1].verbosity); \
	MC_CMD_OP(cmd, 1, 40,  8, uint8_t, trace_point[1].marking); \
} while (0)

/*                cmd, param, offset, width, type,	arg_name */
#define DPDBG_CMD_SET_DPSECI_TRACE(cmd, dpseci_id, trace_point) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, int,      dpseci_id);\
	MC_CMD_OP(cmd, 1, 0,  4,  enum dpdbg_verbosity_level,  \
					  trace_point[0].verbosity); \
	MC_CMD_OP(cmd, 1, 8,  8, uint8_t, trace_point[0].marking); \
	MC_CMD_OP(cmd, 1, 32, 4,  enum dpdbg_verbosity_level,  \
					  trace_point[1].verbosity); \
	MC_CMD_OP(cmd, 1, 40,  8, uint8_t, trace_point[1].marking); \
} while (0)

/*                cmd, param, offset, width, type,	arg_name */
#define DPDBG_CMD_GET_DPMAC_COUNTER(cmd, dpmac_id, counter_type) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, int,      dpmac_id);\
	MC_CMD_OP(cmd, 0, 32, 16, enum dpmac_counter, counter_type);\
} while (0)

/*                cmd, param, offset, width, type,	arg_name */
#define DPDBG_RSP_GET_DPMAC_COUNTER(cmd, counter) \
	MC_RSP_OP(cmd, 1, 0,  64, uint64_t,  counter)

/*                cmd, param, offset, width, type,	arg_name */
#define DPDBG_CMD_GET_DPNI_COUNTER(cmd, dpni_id, counter_type) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, int,      dpni_id);\
	MC_CMD_OP(cmd, 0, 32, 16, enum dpni_counter, counter_type);\
} while (0)

/*                cmd, param, offset, width, type,	arg_name */
#define DPDBG_RSP_GET_DPNI_COUNTER(cmd, counter) \
	MC_RSP_OP(cmd, 1, 0,  64, uint64_t,  counter)

#endif /* _FSL_DPDBG_CMD_H */

/* Copyright 2013-2014 Freescale Semiconductor Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//*
 @File          fsl_dpni_cmd.h

 @Description   defines dpni portal commands

 @Cautions      None.
 *//***************************************************************************/

#ifndef _FSL_DPNI_CMD_H
#define _FSL_DPNI_CMD_H

#define MC_CMD_EXTRACT_DATA_PARAMS		25
struct extract_data {
	uint64_t params[MC_CMD_EXTRACT_DATA_PARAMS];
};

/* DPNI Version */
#define DPNI_VER_MAJOR				1
#define DPNI_VER_MINOR				1

/* cmd IDs */
#define MC_CMDID_CLOSE				0x800
#define MC_DPNI_CMDID_OPEN			0x801
#define MC_DPNI_CMDID_CREATE			0x901

#define DPNI_CMDID_GET_L3_CHKSUM_VALIDATION	0x120
#define DPNI_CMDID_SET_L3_CHKSUM_VALIDATION	0x121
#define DPNI_CMDID_GET_L4_CHKSUM_VALIDATION	0x122
#define DPNI_CMDID_SET_L4_CHKSUM_VALIDATION	0x123
#define DPNI_CMDID_DESTROY			0x124
#define DPNI_CMDID_ATTACH			0x125
#define DPNI_CMDID_DETACH			0x126
#define DPNI_CMDID_SET_POOLS			0x127

#define DPNI_CMDID_SET_TX_TC			0x129
#define DPNI_CMDID_SET_RX_TC			0x12A
#define DPNI_CMDID_SET_TX_FLOW			0x12B
#define DPNI_CMDID_GET_TX_FLOW			0x12C
#define DPNI_CMDID_RESET			0x12D
#define DPNI_CMDID_GET_ATTR			0x12E
#define DPNI_CMDID_GET_QDID			0x12F
#define DPNI_CMDID_ENABLE			0x130
#define DPNI_CMDID_DISABLE			0x131
#define DPNI_CMDID_GET_COUNTER			0x132
#define DPNI_CMDID_SET_COUNTER			0x133
#define DPNI_CMDID_GET_LINK_STATE		0x134
#define DPNI_CMDID_SET_MFL			0x135
#define DPNI_CMDID_SET_MTU			0x136
#define DPNI_CMDID_SET_MCAST_PROMISC		0x137
#define DPNI_CMDID_GET_MCAST_PROMISC		0x138
#define DPNI_CMDID_SET_PRIM_MAC			0x139
#define DPNI_CMDID_ADD_MAC_ADDR			0x13A
#define DPNI_CMDID_REMOVE_MAC_ADDR		0x13B
#define DPNI_CMDID_CLR_MAC_TBL			0x13C
#define DPNI_CMDID_ADD_VLAN_ID			0x13D
#define DPNI_CMDID_REMOVE_VLAN_ID		0x13E
#define DPNI_CMDID_CLR_VLAN_TBL			0x13F
#define DPNI_CMDID_SET_QOS_TBL			0x140
#define DPNI_CMDID_DELETE_QOS_TBL		0x141
#define DPNI_CMDID_ADD_QOS_ENT			0x142
#define DPNI_CMDID_REMOVE_QOS_ENT		0x143
#define DPNI_CMDID_CLR_QOS_TBL			0x144
#define DPNI_CMDID_SET_FS_TBL			0x146
#define DPNI_CMDID_DELETE_FS_TBL		0x147
#define DPNI_CMDID_ADD_FS_ENT			0x148
#define DPNI_CMDID_REMOVE_FS_ENT		0x149
#define DPNI_CMDID_CLR_FS_TBL			0x14A
#define DPNI_CMDID_SET_IRQ			0x14B
#define DPNI_CMDID_SET_TX_PAUSE_FRAMES		0x14C
#define DPNI_CMDID_SET_RX_IGNORE_PAUSE_FRAMES	0x14D
#define DPNI_CMDID_SET_VLAN_FILTERS		0x14E
#define DPNI_CMDID_SET_TX_CONF_BUFFER_LAYOUT	0x14F

#define DPNI_CMDID_GET_TX_DATA_OFFSET		0x150
#define DPNI_CMDID_GET_PRIM_MAC			0x151
#define DPNI_CMDID_GET_MFL			0x152
#define DPNI_CMDID_GET_MTU			0x153
#define DPNI_CMDID_GET_RX_BUFFER_LAYOUT		0x154
#define DPNI_CMDID_SET_RX_BUFFER_LAYOUT		0x155

#define DPNI_CMDID_GET_IRQ			0x156
#define DPNI_CMDID_SET_IRQ_ENABLE		0x157
#define DPNI_CMDID_GET_IRQ_ENABLE		0x158
#define DPNI_CMDID_SET_IRQ_MASK			0x159
#define DPNI_CMDID_GET_IRQ_MASK			0x15A
#define DPNI_CMDID_GET_IRQ_STATUS		0x15B
#define DPNI_CMDID_CLEAR_IRQ_STATUS		0x15C
#define DPNI_CMDID_GET_TX_BUFFER_LAYOUT		0x15D
#define DPNI_CMDID_SET_TX_BUFFER_LAYOUT		0x15E
#define DPNI_CMDID_GET_TX_CONF_BUFFER_LAYOUT	0x15F

#define DPNI_CMDID_GET_SPID			0x160
#define DPNI_CMDID_SET_RX_FLOW			0x161
#define DPNI_CMDID_GET_RX_FLOW			0x162

/* cmd sizes */
#define MC_CMD_OPEN_SIZE			8
#define MC_CMD_CLOSE_SIZE			0
#define DPNI_CMDSZ_CREATE			(8 * 6)
#define DPNI_CMDSZ_DESTROY			0
#define DPNI_CMDSZ_RESET			0
#define DPNI_CMDSZ_ATTACH			(8 * 7)
#define DPNI_CMDSZ_DETACH			0
#define DPNI_CMDSZ_SET_POOLS			(8 * 5)
#define DPNI_CMDSZ_SET_TX_TC			8
#define DPNI_CMDSZ_SET_RX_TC			8
#define DPNI_CMDSZ_SET_TX_FLOW			(8 * 3)
#define DPNI_CMDSZ_GET_TX_FLOW			(8 * 4)
#define DPNI_CMDSZ_SET_RX_FLOW			(8 * 3)
#define DPNI_CMDSZ_GET_RX_FLOW			(8 * 3)
#define DPNI_CMDSZ_GET_CFG			(8 * 2)
#define DPNI_CMDSZ_GET_ATTR			(8 * 5)
#define DPNI_CMDSZ_GET_QDID			8
#define DPNI_CMDSZ_GET_SPID			8
#define DPNI_CMDSZ_GET_RX_BUFFER_LAYOUT		(8 * 2)
#define DPNI_CMDSZ_SET_RX_BUFFER_LAYOUT		(8 * 2)
#define DPNI_CMDSZ_GET_TX_BUFFER_LAYOUT		(8 * 2)
#define DPNI_CMDSZ_SET_TX_BUFFER_LAYOUT		(8 * 2)
#define DPNI_CMDSZ_GET_TX_CONF_BUFFER_LAYOUT	(8 * 2)
#define DPNI_CMDSZ_SET_TX_CONF_BUFFER_LAYOUT	(8 * 2)
#define DPNI_CMDSZ_ENABLE			0
#define DPNI_CMDSZ_DISABLE			0
#define DPNI_CMDSZ_GET_TX_DATA_OFFSET		8
#define DPNI_CMDSZ_GET_LINK_STATE		8
#define DPNI_CMDSZ_SET_MFL			8
#define DPNI_CMDSZ_GET_MFL			8
#define DPNI_CMDSZ_SET_MTU			8
#define DPNI_CMDSZ_GET_MTU			8
#define DPNI_CMDSZ_SET_MCAST_PROMISC		8
#define DPNI_CMDSZ_GET_MCAST_PROMISC		8
#define DPNI_CMDSZ_MODIFY_PRIM_MAC		8
#define DPNI_CMDSZ_ADD_MAC_ADDR			8
#define DPNI_CMDSZ_REMOVE_MAC_ADDR		8
#define DPNI_CMDSZ_CLR_MAC_TBL			0
#define DPNI_CMDSZ_ADD_VLAN_ID			8
#define DPNI_CMDSZ_REMOVE_VLAN_ID		8
#define DPNI_CMDSZ_CLR_VLAN_TBL			0
#define DPNI_CMDSZ_SET_QOS_TBL			(8 * 7)
#define DPNI_CMDSZ_DELETE_QOS_TBL		0
#define DPNI_CMDSZ_ADD_QOS_ENT			8
#define DPNI_CMDSZ_REMOVE_QOS_ENT		0
#define DPNI_CMDSZ_CLR_QOS_TBL			0
#define DPNI_CMDSZ_SET_FS_TBL			8
#define DPNI_CMDSZ_DELETE_FS_TBL		8
#define DPNI_CMDSZ_ADD_FS_ENT			8
#define DPNI_CMDSZ_REMOVE_FS_ENT		8
#define DPNI_CMDSZ_CLR_FS_TBL			8
#define DPNI_CMDSZ_GET_MFL			8
#define DPNI_CMDSZ_SET_IRQ			(8 * 3)
#define DPNI_CMDSZ_SET_TX_PAUSE_FRAMES		8
#define DPNI_CMDSZ_SET_RX_IGNORE_PAUSE_FRAMES	8
#define DPNI_CMDSZ_SET_VLAN_FILTERS		8
#define DPNI_CMDSZ_GET_COUNTER			(8 * 2)
#define DPNI_CMDSZ_SET_COUNTER			(8 * 2)
#define DPNI_CMDSZ_SET_PRIM_MAC			8
#define DPNI_CMDSZ_GET_PRIM_MAC			8
#define DPNI_CMDSZ_GET_IRQ			(8 * 3)
#define DPNI_CMDSZ_SET_IRQ_ENABLE		8
#define DPNI_CMDSZ_GET_IRQ_ENABLE		8
#define DPNI_CMDSZ_SET_IRQ_MASK			8
#define DPNI_CMDSZ_GET_IRQ_MASK			8
#define DPNI_CMDSZ_GET_IRQ_STATUS		8
#define DPNI_CMDSZ_CLEAR_IRQ_STATUS		8
#define DPNI_CMDSZ_GET_L3_CHKSUM_VALIDATION	8
#define DPNI_CMDSZ_SET_L3_CHKSUM_VALIDATION	8
#define DPNI_CMDSZ_GET_L4_CHKSUM_VALIDATION	8
#define DPNI_CMDSZ_SET_L4_CHKSUM_VALIDATION	8


/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_OPEN(_OP) \
	_OP(0,  0,	32,	int,			dpni_id)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_CREATE(_OP) \
do { \
	_OP(0,	0,	8,	uint8_t,		cfg->adv.max_tcs); \
	_OP(0,  8,	8,	uint8_t,		cfg->adv.max_senders); \
	_OP(0,  16,	8,	uint8_t,		cfg->mac_addr[0]); \
	_OP(0,  24,	8,	uint8_t,		cfg->mac_addr[1]); \
	_OP(0,  32,	8,	uint8_t,		cfg->mac_addr[2]); \
	_OP(0,  40,	8,	uint8_t,		cfg->mac_addr[3]); \
	_OP(0,  48,	8,	uint8_t,		cfg->mac_addr[4]); \
	_OP(0,  56,	8,	uint8_t,		cfg->mac_addr[5]); \
	_OP(1,  0,	64,	uint64_t,		cfg->adv.options); \
	_OP(2,  0,	8,	uint8_t, \
					cfg->adv.max_unicast_filters); \
	_OP(2,  8,	8,	uint8_t, \
					cfg->adv.max_multicast_filters); \
	_OP(2,  16,	8,	uint8_t, \
					cfg->adv.max_vlan_filters); \
	_OP(2,  24,	8,	uint8_t, \
					cfg->adv.max_qos_entries); \
	_OP(2,  32,	8,	uint8_t, \
					cfg->adv.max_qos_key_size); \
	_OP(2,  48,	8,	uint8_t, \
					cfg->adv.max_dist_key_size); \
	_OP(3,  0,	4,	enum dpni_type,		cfg->type); \
	_OP(4,  0,	16,	uint16_t, \
					cfg->adv.max_dist_per_tc[0]); \
	_OP(4,  16,	16,	uint16_t, \
					cfg->adv.max_dist_per_tc[1]); \
	_OP(4,  32,	16,	uint16_t, \
					cfg->adv.max_dist_per_tc[2]); \
	_OP(4,  48,	16,	uint16_t, \
					cfg->adv.max_dist_per_tc[3]); \
	_OP(5,  0,	16,	uint16_t, \
					cfg->adv.max_dist_per_tc[4]); \
	_OP(5,  16,	16,	uint16_t, \
					cfg->adv.max_dist_per_tc[5]); \
	_OP(5,  32,	16,	uint16_t, \
					cfg->adv.max_dist_per_tc[6]); \
	_OP(5,  48,	16,	uint16_t, \
					cfg->adv.max_dist_per_tc[7]);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_SET_IRQ(_OP) \
do { \
	_OP(0,	0,	8,	uint8_t,		irq_index); \
	_OP(0,  32,	32,	uint32_t,		irq_val); \
	_OP(1,  0,	64,	uint64_t,		irq_paddr); \
	_OP(2,  0,	32,	int,			user_irq_id); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_SET_TX_PAUSE_FRAMES(_OP) \
do { \
	_OP(0,	0,	8,	uint8_t,		priority); \
	_OP(0,  16,	16,	uint16_t,		pause_time); \
	_OP(0,  32,	16,	uint16_t,		thresh_time); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_SET_RX_IGNORE_PAUSE_FRAMES(_OP) \
	_OP(0,	0,	1,	int,			enable)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_ATTACH(_OP) \
do { \
	_OP(0,	0,	16,	uint16_t,	cfg->dest_cfg.id); \
	_OP(0,  48,	8,	uint8_t,	cfg->dest_cfg.priority); \
	_OP(0,  56,	1,	int,		cfg->dest_apply_all); \
	_OP(0,  57,	2,	enum dpni_dest,	cfg->dest_cfg.type); \
	_OP(1,	0,	64,	uint64_t,	cfg->rx_user_ctx); \
	_OP(2,	0,	64,	uint64_t,	cfg->rx_err_user_ctx); \
	_OP(3,	0,	64,	uint64_t,	cfg->tx_err_user_ctx);\
	_OP(4,	0,	64,	uint64_t,	cfg->tx_conf_user_ctx); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_RSP_GET_ATTR(_OP) \
do { \
	_OP(0,  0,	32,	int,		attr->id);\
	_OP(0,	32,	8,	uint8_t,	attr->max_tcs); \
	_OP(0,	40,	8,	uint8_t,	attr->max_senders); \
	_OP(0,	48,	8,	enum net_prot,	attr->start_hdr); \
	_OP(0,	56,	2,	enum dpni_type,	attr->type); \
	_OP(1,	0,	64,	uint64_t,	attr->options); \
	_OP(2,	0,	16,	uint16_t,	attr->max_dist_per_tc[0]); \
	_OP(2,	16,	16,	uint16_t,	attr->max_dist_per_tc[1]); \
	_OP(2,	32,	16,	uint16_t,	attr->max_dist_per_tc[2]); \
	_OP(2,	48,	16,	uint16_t,	attr->max_dist_per_tc[3]); \
	_OP(2,	0,	16,	uint16_t,	attr->max_dist_per_tc[4]); \
	_OP(2,	16,	16,	uint16_t,	attr->max_dist_per_tc[5]); \
	_OP(2,	32,	16,	uint16_t,	attr->max_dist_per_tc[6]); \
	_OP(2,	48,	16,	uint16_t,	attr->max_dist_per_tc[7]); \
	_OP(3,	0,	8,	uint8_t,	attr->max_unicast_filters); \
	_OP(3,	8,	8,	uint8_t,	attr->max_multicast_filters); \
	_OP(3,	16,	8,	uint8_t,	attr->max_vlan_filters); \
	_OP(3,	24,	8,	uint8_t,	attr->max_qos_entries); \
	_OP(3,	32,	8,	uint8_t,	attr->max_qos_key_size); \
	_OP(3,	40,	8,	uint8_t,	attr->max_dist_key_size); \
	_OP(4,  0,	32,	uint32_t,		attr->version.major);\
	_OP(4,  32,	32,	uint32_t,		attr->version.minor);\
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPNI_RSP_GET_RX_BUFFER_LAYOUT(_OP) \
do { \
	_OP(0,	0,	16,	uint16_t,	layout->private_data_size); \
	_OP(0,	16,	16,	uint16_t,	layout->data_align); \
	_OP(0,	32,	32,	uint32_t,	layout->options); \
	_OP(1,	0,	1,	int,		layout->pass_timestamp); \
	_OP(1,	1,	1,	int,		layout->pass_parser_result); \
	_OP(1,	2,	1,	int,		layout->pass_frame_status); \
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPNI_CMD_SET_RX_BUFFER_LAYOUT(_OP) \
do { \
	_OP(0,	0,	16,	uint16_t,	layout->private_data_size); \
	_OP(0,	16,	16,	uint16_t,	layout->data_align); \
	_OP(0,	32,	32,	uint32_t,	layout->options); \
	_OP(1,	0,	1,	int,		layout->pass_timestamp); \
	_OP(1,	1,	1,	int,		layout->pass_parser_result); \
	_OP(1,	2,	1,	int,		layout->pass_frame_status); \
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPNI_RSP_GET_TX_BUFFER_LAYOUT(_OP) \
do { \
	_OP(0,	0,	16,	uint16_t,	layout->private_data_size); \
	_OP(0,	16,	16,	uint16_t,	layout->data_align); \
	_OP(0,	32,	32,	uint32_t,	layout->options); \
	_OP(1,	0,	1,	int,		layout->pass_timestamp); \
	_OP(1,	1,	1,	int,		layout->pass_parser_result); \
	_OP(1,	2,	1,	int,		layout->pass_frame_status); \
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPNI_CMD_SET_TX_BUFFER_LAYOUT(_OP) \
do { \
	_OP(0,	0,	16,	uint16_t,	layout->private_data_size); \
	_OP(0,	16,	16,	uint16_t,	layout->data_align); \
	_OP(0,	32,	32,	uint32_t,	layout->options); \
	_OP(1,	0,	1,	int,		layout->pass_timestamp); \
	_OP(1,	1,	1,	int,		layout->pass_parser_result); \
	_OP(1,	2,	1,	int,		layout->pass_frame_status); \
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPNI_RSP_GET_TX_CONF_BUFFER_LAYOUT(_OP) \
do { \
	_OP(0,	0,	16,	uint16_t,	layout->private_data_size); \
	_OP(0,	16,	16,	uint16_t,	layout->data_align); \
	_OP(0,	32,	32,	uint32_t,	layout->options); \
	_OP(1,	0,	1,	int,		layout->pass_timestamp); \
	_OP(1,	1,	1,	int,		layout->pass_parser_result); \
	_OP(1,	2,	1,	int,		layout->pass_frame_status); \
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPNI_CMD_SET_TX_CONF_BUFFER_LAYOUT(_OP) \
do { \
	_OP(0,	0,	16,	uint16_t,	layout->private_data_size); \
	_OP(0,	16,	16,	uint16_t,	layout->data_align); \
	_OP(0,	32,	32,	uint32_t,	layout->options); \
	_OP(1,	0,	1,	int,		layout->pass_timestamp); \
	_OP(1,	1,	1,	int,		layout->pass_parser_result); \
	_OP(1,	2,	1,	int,		layout->pass_frame_status); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_RSP_GET_QDID(_OP) \
	_OP(0,	0,	16,	uint16_t,		qdid)

/*	param, offset, width,	type,			arg_name */
#define DPNI_RSP_GET_SPID(_OP) \
	_OP(0,	0,	16,	uint16_t,		spid)

/*	param, offset, width,	type,			arg_name */
#define DPNI_RSP_GET_TX_DATA_OFFSET(_OP) \
	_OP(0,	0,	16,	uint16_t,		data_offset)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_GET_COUNTER(_OP) \
	_OP(0,	0,	16,	enum dpni_counter,	 counter)

/*	param, offset, width,	type,			arg_name */
#define DPNI_RSP_GET_COUNTER(_OP) \
	_OP(1,	0,	64,	uint64_t,		value)

/*	param, offset, width,	type,			arg_name */
#define DPNI_RSP_GET_LINK_STATE(_OP) \
	_OP(0,	0,	1,	int,			up)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_SET_COUNTER(_OP) \
do { \
	_OP(0,	0,	16,	enum dpni_counter,	counter); \
	_OP(1,	0,	64,	uint64_t,		value); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_SET_MFL(_OP) \
	_OP(0,	0,	16,	uint16_t,		mfl)

/*	param, offset, width,	type,			arg_name */
#define DPNI_RSP_GET_MFL(_OP) \
	_OP(0,	0,	16,	uint16_t,		mfl)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_SET_MTU(_OP) \
	_OP(0,	0,	16,	uint16_t,		mtu)

/*	param, offset, width,	type,			arg_name */
#define DPNI_RSP_GET_MTU(_OP) \
	_OP(0,	0,	16,	uint16_t,		mtu)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_SET_MULTICAST_PROMISC(_OP) \
	_OP(0,	0,	1,	int,			en)

/*	param, offset, width,	type,			arg_name */
#define DPNI_RSP_GET_MULTICAST_PROMISC(_OP) \
	_OP(0,	0,	1,	int,			en)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_SET_PRIMARY_MAC_ADDR(_OP) \
do { \
	_OP(0,	16,	8,	uint8_t,		addr[0]); \
	_OP(0,	24,	8,	uint8_t,		addr[1]); \
	_OP(0,	32,	8,	uint8_t,		addr[2]); \
	_OP(0,	40,	8,	uint8_t,		addr[3]); \
	_OP(0,	48,	8,	uint8_t,		addr[4]); \
	_OP(0,	56,	8,	uint8_t,		addr[5]); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_RSP_GET_PRIMARY_MAC_ADDR(_OP) \
do { \
	_OP(0,	16,	8,	uint8_t,		addr[0]); \
	_OP(0,	24,	8,	uint8_t,		addr[1]); \
	_OP(0,	32,	8,	uint8_t,		addr[2]); \
	_OP(0,	40,	8,	uint8_t,		addr[3]); \
	_OP(0,	48,	8,	uint8_t,		addr[4]); \
	_OP(0,	56,	8,	uint8_t,		addr[5]); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_ADD_MAC_ADDR(_OP) \
do { \
	_OP(0,	16,	8,	uint8_t,		addr[0]); \
	_OP(0,	24,	8,	uint8_t,		addr[1]); \
	_OP(0,	32,	8,	uint8_t,		addr[2]); \
	_OP(0,	40,	8,	uint8_t,		addr[3]); \
	_OP(0,	48,	8,	uint8_t,		addr[4]); \
	_OP(0,	56,	8,	uint8_t,		addr[5]); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_REMOVE_MAC_ADDR(_OP) \
do { \
	_OP(0,	16,	8,	uint8_t,		addr[0]); \
	_OP(0,	24,	8,	uint8_t,		addr[1]); \
	_OP(0,	32,	8,	uint8_t,		addr[2]); \
	_OP(0,	40,	8,	uint8_t,		addr[3]); \
	_OP(0,	48,	8,	uint8_t,		addr[4]); \
	_OP(0,	56,	8,	uint8_t,		addr[5]); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_ADD_VLAN_ID(_OP) \
	_OP(0,	32,	16,	uint16_t,		vlan_id)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_REMOVE_VLAN_ID(_OP) \
	_OP(0,	32,	16,	uint16_t,		vlan_id)

/*	param, offset, width,	type,		arg_name */
#define DPNI_CMD_SET_POOLS(_OP) \
do { \
	_OP(0,	0,	8,	uint8_t,	cfg->num_dpbp); \
	_OP(0,	16,	16,	uint16_t,	cfg->pools[0].dpbp_id); \
	_OP(0,	32,	16,	uint16_t,	cfg->pools[0].buffer_size); \
	_OP(1,	0,	16,	uint16_t,	cfg->pools[1].dpbp_id); \
	_OP(1,	16,	16,	uint16_t,	cfg->pools[1].buffer_size); \
	_OP(1,	32,	16,	uint16_t,	cfg->pools[2].dpbp_id); \
	_OP(1,	48,	16,	uint16_t,	cfg->pools[2].buffer_size); \
	_OP(2,	0,	16,	uint16_t,	cfg->pools[3].dpbp_id); \
	_OP(2,	16,	16,	uint16_t,	cfg->pools[3].buffer_size); \
	_OP(2,	32,	16,	uint16_t,	cfg->pools[4].dpbp_id); \
	_OP(2,	48,	16,	uint16_t,	cfg->pools[4].buffer_size); \
	_OP(3,	0,	16,	uint16_t,	cfg->pools[5].dpbp_id); \
	_OP(3,	16,	16,	uint16_t,	cfg->pools[5].buffer_size); \
	_OP(3,	32,	16,	uint16_t,	cfg->pools[6].dpbp_id); \
	_OP(3,	48,	16,	uint16_t,	cfg->pools[6].buffer_size); \
	_OP(4,	0,	16,	uint16_t,	cfg->pools[7].dpbp_id); \
	_OP(4,	16,	16,	uint16_t,	cfg->pools[7].buffer_size); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_SET_TX_TC(_OP) \
do { \
	_OP(0,	0,	16,	uint16_t,		cfg->depth_limit); \
	_OP(0,	16,	8,	uint8_t,		tc_id); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_SET_RX_TC(_OP) \
do { \
	_OP(0,	0,	16,	uint16_t,		cfg->dist_size); \
	_OP(0,	16,	8,	uint8_t,		tc_id); \
	_OP(0,	24,	4,	enum dpni_dist_mode,	cfg->dist_mode); \
	_OP(6,	0,	64,	uint64_t,		ext_paddr); \
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPNI_CMD_SET_TX_FLOW(_OP) \
do { \
	_OP(0,   0,	16,	uint16_t,	cfg->dest_cfg.id);\
	_OP(0,   16,	8,	uint8_t,	cfg->dest_cfg.priority);\
	_OP(0,   24,	1,	int,		cfg->only_error_frames);\
	_OP(0,   25,	2,	enum dpni_dest,	cfg->dest_cfg.type);\
	_OP(0,   27,	1,	int,		cfg->l3_chksum_gen);\
	_OP(0,   28,	1,	int,		cfg->l4_chksum_gen);\
	_OP(0,   48,	16,	uint16_t,	*flow_id);\
	_OP(1,   0,	64,	uint64_t,	cfg->user_ctx);\
	_OP(2,   0,	32,	uint32_t,	cfg->options);\
	_OP(2,   32,	32,	int,		cfg->tx_conf_err);\
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPNI_RSP_SET_TX_FLOW(_OP) \
	_OP(0,   48,	16,	uint16_t,	*flow_id)

/*	param, offset, width,	type,		arg_name */
#define DPNI_CMD_GET_TX_FLOW(_OP) \
	_OP(0,   48,	16,	uint16_t,	flow_id)

/*	param, offset, width,	type,		arg_name */
#define DPNI_RSP_GET_TX_FLOW(_OP) \
do { \
	_OP(0,   0,	16,	uint16_t,	cfg->dest_cfg.id);\
	_OP(0,   16,	8,	uint8_t,	cfg->dest_cfg.priority);\
	_OP(0,   24,	1,	int,		cfg->only_error_frames);\
	_OP(0,   25,	2,	enum dpni_dest,	cfg->dest_cfg.type);\
	_OP(0,   27,	1,	int,		cfg->l3_chksum_gen);\
	_OP(0,   28,	1,	int,		cfg->l4_chksum_gen);\
	_OP(1,   0,	64,	uint64_t,	cfg->user_ctx);\
	_OP(2,   0,	32,	uint32_t,	cfg->options);\
	_OP(2,   32,	32,	int,		cfg->tx_conf_err);\
	_OP(3,   0,	32,	uint32_t,	*fqid);\
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPNI_CMD_SET_RX_FLOW(_OP) \
do { \
	_OP(0,	0,	16,	uint16_t,	cfg->dest_cfg.id); \
	_OP(0,	16,	8,	uint8_t,	tc_id); \
	_OP(0,	24,	16,	uint8_t,	cfg->dest_cfg.priority);\
	_OP(0,	48,	16,	uint16_t,	flow_id); \
	_OP(1,	0,	64,	uint64_t,	cfg->user_ctx); \
	_OP(2,	0,	32,	uint32_t,	cfg->options); \
	_OP(2,	32,	2,	enum dpni_dest,	cfg->dest_cfg.type); \
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPNI_CMD_GET_RX_FLOW(_OP) \
do { \
	_OP(0,	16,	8,	uint8_t,	tc_id); \
	_OP(0,	48,	16,	uint16_t,	flow_id); \
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPNI_RSP_GET_RX_FLOW(_OP) \
do { \
	_OP(0,	0,	16,	uint16_t,	cfg->dest_cfg.id); \
	_OP(0,	24,	16,	uint8_t,	cfg->dest_cfg.priority);\
	_OP(1,	0,	64,	uint64_t,	cfg->user_ctx); \
	_OP(2,	0,	32,	uint32_t,	cfg->options); \
	_OP(2,	32,	2,	enum dpni_dest,	cfg->dest_cfg.type); \
	_OP(3,	0,	32,	uint32_t,	*fqid); \
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPNI_CMD_SET_QOS_TABLE(_OP) \
do { \
	_OP(0,	0,	32,	int,		cfg->drop_frame); \
	_OP(0,	32,	8,	uint8_t,	cfg->default_tc); \
	_OP(6,	0,	64,	uint64_t,	ext_paddr); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_ADD_QOS_ENTRY(_OP) \
do { \
	_OP(0,	16,	8,	uint8_t,		tc_id); \
	_OP(0,	24,	8,	uint8_t,		cfg->size); \
	_OP(1,	0,	64,	uint64_t,		key_paddr); \
	_OP(2,	0,	64,	uint64_t,		mask_paddr); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_REMOVE_QOS_ENTRY(_OP) \
do { \
	_OP(0,	24,	8,	uint8_t,		cfg->size); \
	_OP(1,	0,	64,	uint64_t,		key_paddr); \
	_OP(2,	0,	64,	uint64_t,		mask_paddr); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_SET_FS_TABLE(_OP) \
do { \
	_OP(0,	0,	4,	enum dpni_fs_miss_action, cfg->miss_action); \
	_OP(0,	16,	8,	uint8_t,		tc_id); \
	_OP(0,	48,	16,	uint16_t,		cfg->default_flow_id);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_DELETE_FS_TABLE(_OP) \
	_OP(0,	16,	8,	uint8_t,		tc_id)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_ADD_FS_ENTRY(_OP) \
do { \
	_OP(0,	16,	8,	uint8_t,		tc_id); \
	_OP(0,	48,	16,	uint16_t,		flow_id); \
	_OP(0,	24,	8,	uint8_t,		cfg->size); \
	_OP(1,	0,	64,	uint64_t,		key_paddr); \
	_OP(2,	0,	64,	uint64_t,		mask_paddr); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_REMOVE_FS_ENTRY(_OP) \
do { \
	_OP(0,	16,	8,	uint8_t,		tc_id); \
	_OP(0,	24,	8,	uint8_t,		cfg->size); \
	_OP(1,	0,	64,	uint64_t,		key_paddr); \
	_OP(2,	0,	64,	uint64_t,		mask_paddr); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_CLEAR_FS_TABLE(_OP) \
	_OP(0,	16,	8,	uint8_t,		tc_id)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_GET_IRQ(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPNI_RSP_GET_IRQ(_OP) \
do { \
	_OP(0,	0,	32,	uint32_t,		irq_val); \
	_OP(1,	0,	64,	uint64_t,		irq_paddr); \
	_OP(2,  0,	32,	int,			user_irq_id); \
	_OP(2,	32,	32,	int,			type); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_SET_IRQ_ENABLE(_OP) \
do { \
	_OP(0,	0,	8,	uint8_t,		enable_state); \
	_OP(0,	32,	8,	uint8_t,		irq_index);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_GET_IRQ_ENABLE(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPNI_RSP_GET_IRQ_ENABLE(_OP) \
	_OP(0,	0,	8,	uint8_t,		enable_state)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_SET_IRQ_MASK(_OP) \
do { \
	_OP(0,	0,	32,	uint32_t,		mask); \
	_OP(0,	32,	8,	uint8_t,		irq_index);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_GET_IRQ_MASK(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPNI_RSP_GET_IRQ_MASK(_OP) \
	_OP(0,	0,	32,	uint32_t,		mask)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_GET_IRQ_STATUS(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPNI_RSP_GET_IRQ_STATUS(_OP) \
	_OP(0,	0,	32,	uint32_t,		status)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_CLEAR_IRQ_STATUS(_OP) \
do { \
	_OP(0,	0,	32,	uint32_t,		status); \
	_OP(0,	32,	8,	uint8_t,		irq_index);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_SET_VLAN_FILTERS(_OP) \
	_OP(0,	0,	1,	int,			en)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_SET_L3_CHKSUM_VALIDATION(_OP) \
	_OP(0,	0,	1,	int,			en)

/*	param, offset, width,	type,			arg_name */
#define DPNI_RSP_GET_L3_CHKSUM_VALIDATION(_OP) \
	_OP(0,	0,	1,	int,			en)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_SET_L4_CHKSUM_VALIDATION(_OP) \
	_OP(0,	0,	1,	int,			en)

/*	param, offset, width,	type,			arg_name */
#define DPNI_RSP_GET_L4_CHKSUM_VALIDATION(_OP) \
	_OP(0,	0,	1,	int,			en)

#endif /* _FSL_DPNI_CMD_H */

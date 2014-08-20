/* Copyright 2014 Freescale Semiconductor Inc.
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
 @File          fsl_dpio_cmd.h

 @Description   defines dprc portal commandsF

 @Cautions      None.
 *//***************************************************************************/

#ifndef _FSL_DPIO_CMD_H
#define _FSL_DPIO_CMD_H

/* cmd IDs */
#define MC_CMDID_CLOSE					0x800
#define MC_DPIO_CMDID_OPEN				0x803
#define MC_DPIO_CMDID_CREATE				0x903

#define DPIO_CMDID_RESET				0x112
#define DPIO_CMDID_DESTROY				0x113
#define DPIO_CMDID_ENABLE				0x114
#define DPIO_CMDID_DISABLE				0x115
#define DPIO_CMDID_GET_ATTR				0x116
#define DPIO_CMDID_SET_IRQ				0x117
#define DPIO_CMDID_GET_IRQ				0x118
#define DPIO_CMDID_SET_IRQ_ENABLE			0x119
#define DPIO_CMDID_GET_IRQ_ENABLE			0x11A
#define DPIO_CMDID_SET_IRQ_MASK				0x11B
#define DPIO_CMDID_GET_IRQ_MASK				0x11C
#define DPIO_CMDID_GET_IRQ_STATUS			0x11D
#define DPIO_CMDID_CLEAR_IRQ_STATUS			0x11E

/* cmd sizes */
#define MC_CMD_OPEN_SIZE				8
#define MC_CMD_CLOSE_SIZE				0
#define DPIO_CMDSZ_RESET				0
#define DPIO_CMDSZ_CREATE				8
#define DPIO_CMDSZ_DESTROY				0
#define DPIO_CMDSZ_ENABLE				0
#define DPIO_CMDSZ_DISABLE				0
#define DPIO_CMDSZ_GET_ATTR				(8 * 4)
#define DPIO_CMDSZ_SET_IRQ				(8 * 3)
#define DPIO_CMDSZ_GET_IRQ				(8 * 3)
#define DPIO_CMDSZ_SET_IRQ_ENABLE			8
#define DPIO_CMDSZ_GET_IRQ_ENABLE			8
#define DPIO_CMDSZ_SET_IRQ_MASK				8
#define DPIO_CMDSZ_GET_IRQ_MASK				8
#define DPIO_CMDSZ_GET_IRQ_STATUS			8
#define DPIO_CMDSZ_CLEAR_IRQ_STATUS			8

/*	param, offset, width,	type,			arg_name */
#define DPIO_CMD_OPEN(_OP) \
	_OP(0,  0,	32,	int,			dpio_id)

/*	param, offset, width,	type,		arg_name */
#define DPIO_CMD_CREATE(_OP) \
do { \
	_OP(0,  16,	2,	enum dpio_channel_mode,	cfg->channel_mode);\
	_OP(0,  32,	8,	uint8_t,		cfg->num_priorities);\
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPIO_RSP_GET_ATTR(_OP) \
do { \
	_OP(0,  0,	32,	int,		attr->id);\
	_OP(0,  32,	16,	uint16_t,	attr->qbman_portal_id);\
	_OP(0,  48,	8,	uint8_t,	attr->num_priorities);\
	_OP(0,  56,	4,	enum dpio_channel_mode,	attr->channel_mode);\
	_OP(1,  0,	64,	uint64_t,	attr->qbman_portal_ce_paddr);\
	_OP(2,  0,	64,	uint64_t,	attr->qbman_portal_ci_paddr);\
	_OP(3,  0,	32,	uint32_t,		attr->version.major);\
	_OP(3,  32,	32,	uint32_t,		attr->version.minor);\
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPIO_CMD_SET_IRQ(_OP) \
do { \
	_OP(0,  0,	8,	uint8_t,	irq_index);\
	_OP(0,  32,	32,	uint32_t,	irq_val);\
	_OP(1,  0,	64,	uint64_t,	irq_paddr);\
	_OP(2,  0,	32,	int,		user_irq_id); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPIO_CMD_GET_IRQ(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPIO_RSP_GET_IRQ(_OP) \
do { \
	_OP(0,	0,	32,	uint32_t,		irq_val); \
	_OP(1,	0,	64,	uint64_t,		irq_paddr); \
	_OP(2,  0,	32,	int,			user_irq_id); \
	_OP(2,	32,	32,	int,			type); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPIO_CMD_SET_IRQ_ENABLE(_OP) \
do { \
	_OP(0,	0,	8,	uint8_t,		enable_state); \
	_OP(0,	32,	8,	uint8_t,		irq_index);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPIO_CMD_GET_IRQ_ENABLE(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPIO_RSP_GET_IRQ_ENABLE(_OP) \
	_OP(0,	0,	8,	uint8_t,		enable_state)

/*	param, offset, width,	type,			arg_name */
#define DPIO_CMD_SET_IRQ_MASK(_OP) \
do { \
	_OP(0,	0,	32,	uint32_t,		mask); \
	_OP(0,	32,	8,	uint8_t,		irq_index);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPIO_CMD_GET_IRQ_MASK(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPIO_RSP_GET_IRQ_MASK(_OP) \
	_OP(0,	0,	32,	uint32_t,		mask)

/*	param, offset, width,	type,			arg_name */
#define DPIO_CMD_GET_IRQ_STATUS(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPIO_RSP_GET_IRQ_STATUS(_OP) \
	_OP(0,	0,	32,	uint32_t,		status)

/*	param, offset, width,	type,			arg_name */
#define DPIO_CMD_CLEAR_IRQ_STATUS(_OP) \
do { \
	_OP(0,	0,	32,	uint32_t,		status); \
	_OP(0,	32,	8,	uint8_t,		irq_index);\
} while (0)

#endif /* _FSL_DPIO_CMD_H */

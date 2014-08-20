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
 @File          fsl_dpmng_cmd.h

 @Description   defines portal commands

 @Cautions      None.
 *//***************************************************************************/

#ifndef __FSL_DPMNG_CMD_H
#define __FSL_DPMNG_CMD_H

/* Command IDs */
#define DPMNG_CMDID_GET_VERSION			0x831
#define DPMNG_CMDID_RESET_AIOP			0x832
#define DPMNG_CMDID_LOAD_AIOP			0x833
#define DPMNG_CMDID_RUN_AIOP			0x834

/* Command sizes */
#define DPMNG_CMDSZ_GET_VERSION			(8 * 2)
#define DPMNG_CMDSZ_RESET_AIOP			8
#define DPMNG_CMDSZ_LOAD_AIOP			(8 * 2)
#define DPMNG_CMDSZ_RUN_AIOP			(8 * 2)

/*	param, offset, width,	type,		arg_name */
#define DPMNG_RSP_GET_VERSION(_OP) \
do { \
	_OP(0,	0,	32,	uint32_t,	mc_ver_info->revision); \
	_OP(0,	32,	32,	uint32_t,	mc_ver_info->major); \
	_OP(1,	0,	8,	uint32_t,	mc_ver_info->minor); \
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPMNG_CMD_RESET_AIOP(_OP) \
	_OP(0,	0,	32,	int,		aiop_tile_id)

/*	param, offset, width,	type,		arg_name */
#define DPMNG_CMD_LOAD_AIOP(_OP) \
do { \
	_OP(0,	0,	32,	int,		aiop_tile_id); \
	_OP(0,	32,	32,	int,		img_size); \
	_OP(1,	0,	64,	uint64_t,	img_paddr); \
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPMNG_CMD_RUN_AIOP(_OP) \
do { \
	_OP(0,	0,	32,	int,		aiop_tile_id); \
	_OP(0,	32,	32,	uint32_t,	cores_mask); \
	_OP(1,	0,	64,	uint64_t,	options); \
} while (0)

#endif /* __FSL_DPMNG_CMD_H */

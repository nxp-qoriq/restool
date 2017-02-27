/* Copyright 2013-2015 Freescale Semiconductor Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * * Neither the name of the above-listed copyright holders nor the
 *   names of any contributors may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
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
#ifndef _FSL_DPRC_CMD_H
#define _FSL_DPRC_CMD_H

/* DPRC Version */
#define DPRC_VER_MAJOR				5
#define DPRC_VER_MINOR				0

/* Command IDs */
#define DPRC_CMDID_CLOSE			0x800
#define DPRC_CMDID_OPEN				0x805
#define DPRC_CMDID_CREATE			0x905

#define DPRC_CMDID_GET_ATTR			0x004

#define DPRC_CMDID_GET_IRQ_MASK			0x015
#define DPRC_CMDID_GET_IRQ_STATUS		0x016

#define DPRC_CMDID_CREATE_CONT			0x151
#define DPRC_CMDID_DESTROY_CONT			0x152
#define DPRC_CMDID_GET_CONT_ID			0x830
#define DPRC_CMDID_ASSIGN			0x157
#define DPRC_CMDID_UNASSIGN			0x158
#define DPRC_CMDID_GET_OBJ_COUNT		0x159
#define DPRC_CMDID_GET_OBJ			0x15A
#define DPRC_CMDID_GET_RES_COUNT		0x15B
#define DPRC_CMDID_GET_RES_IDS			0x15C
#define DPRC_CMDID_SET_OBJ_LABEL		0x161

#define DPRC_CMDID_CONNECT			0x167
#define DPRC_CMDID_DISCONNECT			0x168
#define DPRC_CMDID_GET_POOL			0x169
#define DPRC_CMDID_GET_POOL_COUNT		0x16A

#define DPRC_CMDID_GET_CONNECTION		0x16C

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_RSP_GET_CONTAINER_ID(cmd, container_id) \
	MC_RSP_OP(cmd, 0, 0,  32,  int,	    container_id)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_CMD_OPEN(cmd, container_id) \
	MC_CMD_OP(cmd, 0, 0,  32, int,	    container_id)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_CMD_CREATE_CONTAINER(cmd, cfg) \
do { \
	MC_CMD_OP(cmd, 0, 32, 16, uint16_t, cfg->icid); \
	MC_CMD_OP(cmd, 0, 0,  32, uint32_t, cfg->options); \
	MC_CMD_OP(cmd, 1, 32, 32, int,	    cfg->portal_id); \
	MC_CMD_OP(cmd, 2, 0,  8,  char,	    cfg->label[0]);\
	MC_CMD_OP(cmd, 2, 8,  8,  char,	    cfg->label[1]);\
	MC_CMD_OP(cmd, 2, 16, 8,  char,	    cfg->label[2]);\
	MC_CMD_OP(cmd, 2, 24, 8,  char,	    cfg->label[3]);\
	MC_CMD_OP(cmd, 2, 32, 8,  char,	    cfg->label[4]);\
	MC_CMD_OP(cmd, 2, 40, 8,  char,	    cfg->label[5]);\
	MC_CMD_OP(cmd, 2, 48, 8,  char,	    cfg->label[6]);\
	MC_CMD_OP(cmd, 2, 56, 8,  char,	    cfg->label[7]);\
	MC_CMD_OP(cmd, 3, 0,  8,  char,	    cfg->label[8]);\
	MC_CMD_OP(cmd, 3, 8,  8,  char,	    cfg->label[9]);\
	MC_CMD_OP(cmd, 3, 16, 8,  char,	    cfg->label[10]);\
	MC_CMD_OP(cmd, 3, 24, 8,  char,	    cfg->label[11]);\
	MC_CMD_OP(cmd, 3, 32, 8,  char,	    cfg->label[12]);\
	MC_CMD_OP(cmd, 3, 40, 8,  char,	    cfg->label[13]);\
	MC_CMD_OP(cmd, 3, 48, 8,  char,	    cfg->label[14]);\
	MC_CMD_OP(cmd, 3, 56, 8,  char,	    cfg->label[15]);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_RSP_CREATE_CONTAINER(cmd, child_container_id, child_portal_offset)\
do { \
	MC_RSP_OP(cmd, 1, 0,  32, int,	   child_container_id); \
	MC_RSP_OP(cmd, 2, 0,  64, uint64_t, child_portal_offset);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_CMD_DESTROY_CONTAINER(cmd, child_container_id) \
	MC_CMD_OP(cmd, 0, 0,  32, int,	    child_container_id)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_CMD_RESET_CONTAINER(cmd, child_container_id) \
	MC_CMD_OP(cmd, 0, 0,  32, int,	    child_container_id)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_CMD_GET_IRQ_MASK(cmd, irq_index) \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_RSP_GET_IRQ_MASK(cmd, mask) \
	MC_RSP_OP(cmd, 0, 0,  32, uint32_t, mask)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_CMD_GET_IRQ_STATUS(cmd, irq_index) \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_RSP_GET_IRQ_STATUS(cmd, status) \
	MC_RSP_OP(cmd, 0, 0,  32, uint32_t, status)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_RSP_GET_ATTRIBUTES(cmd, attr) \
do { \
	MC_RSP_OP(cmd, 0, 0,  32, int,	    attr->container_id); \
	MC_RSP_OP(cmd, 0, 32, 16, uint16_t, attr->icid); \
	MC_RSP_OP(cmd, 1, 0,  32, uint32_t, attr->options);\
	MC_RSP_OP(cmd, 1, 32, 32, int,      attr->portal_id); \
	MC_RSP_OP(cmd, 2, 0,  16, uint16_t, attr->version.major);\
	MC_RSP_OP(cmd, 2, 16, 16, uint16_t, attr->version.minor);\
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPRC_CMD_ASSIGN(cmd, container_id, res_req) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, int,      container_id); \
	MC_CMD_OP(cmd, 0, 32, 32, uint32_t, res_req->options);\
	MC_CMD_OP(cmd, 1, 0,  32, uint32_t, res_req->num); \
	MC_CMD_OP(cmd, 1, 32, 32, int,	    res_req->id_base_align); \
	MC_CMD_OP(cmd, 2, 0,  8,  char,	    res_req->type[0]);\
	MC_CMD_OP(cmd, 2, 8,  8,  char,	    res_req->type[1]);\
	MC_CMD_OP(cmd, 2, 16, 8,  char,	    res_req->type[2]);\
	MC_CMD_OP(cmd, 2, 24, 8,  char,	    res_req->type[3]);\
	MC_CMD_OP(cmd, 2, 32, 8,  char,	    res_req->type[4]);\
	MC_CMD_OP(cmd, 2, 40, 8,  char,	    res_req->type[5]);\
	MC_CMD_OP(cmd, 2, 48, 8,  char,	    res_req->type[6]);\
	MC_CMD_OP(cmd, 2, 56, 8,  char,	    res_req->type[7]);\
	MC_CMD_OP(cmd, 3, 0,  8,  char,	    res_req->type[8]);\
	MC_CMD_OP(cmd, 3, 8,  8,  char,	    res_req->type[9]);\
	MC_CMD_OP(cmd, 3, 16, 8,  char,	    res_req->type[10]);\
	MC_CMD_OP(cmd, 3, 24, 8,  char,	    res_req->type[11]);\
	MC_CMD_OP(cmd, 3, 32, 8,  char,	    res_req->type[12]);\
	MC_CMD_OP(cmd, 3, 40, 8,  char,	    res_req->type[13]);\
	MC_CMD_OP(cmd, 3, 48, 8,  char,	    res_req->type[14]);\
	MC_CMD_OP(cmd, 3, 56, 8,  char,	    res_req->type[15]);\
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPRC_CMD_UNASSIGN(cmd, child_container_id, res_req) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, int,      child_container_id); \
	MC_CMD_OP(cmd, 0, 32, 32, uint32_t, res_req->options);\
	MC_CMD_OP(cmd, 1, 0,  32, uint32_t, res_req->num); \
	MC_CMD_OP(cmd, 1, 32, 32, int,	    res_req->id_base_align); \
	MC_CMD_OP(cmd, 2, 0,  8,  char,	    res_req->type[0]);\
	MC_CMD_OP(cmd, 2, 8,  8,  char,	    res_req->type[1]);\
	MC_CMD_OP(cmd, 2, 16, 8,  char,	    res_req->type[2]);\
	MC_CMD_OP(cmd, 2, 24, 8,  char,	    res_req->type[3]);\
	MC_CMD_OP(cmd, 2, 32, 8,  char,	    res_req->type[4]);\
	MC_CMD_OP(cmd, 2, 40, 8,  char,	    res_req->type[5]);\
	MC_CMD_OP(cmd, 2, 48, 8,  char,	    res_req->type[6]);\
	MC_CMD_OP(cmd, 2, 56, 8,  char,	    res_req->type[7]);\
	MC_CMD_OP(cmd, 3, 0,  8,  char,	    res_req->type[8]);\
	MC_CMD_OP(cmd, 3, 8,  8,  char,	    res_req->type[9]);\
	MC_CMD_OP(cmd, 3, 16, 8,  char,	    res_req->type[10]);\
	MC_CMD_OP(cmd, 3, 24, 8,  char,	    res_req->type[11]);\
	MC_CMD_OP(cmd, 3, 32, 8,  char,	    res_req->type[12]);\
	MC_CMD_OP(cmd, 3, 40, 8,  char,	    res_req->type[13]);\
	MC_CMD_OP(cmd, 3, 48, 8,  char,	    res_req->type[14]);\
	MC_CMD_OP(cmd, 3, 56, 8,  char,	    res_req->type[15]);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_RSP_GET_POOL_COUNT(cmd, pool_count) \
	MC_RSP_OP(cmd, 0, 0,  32, int,	    pool_count)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_CMD_GET_POOL(cmd, pool_index) \
	MC_CMD_OP(cmd,	  0,	0,	32,	int,	pool_index)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_RSP_GET_POOL(cmd, type) \
do { \
	MC_RSP_OP(cmd, 1, 0,  8,  char,     type[0]);\
	MC_RSP_OP(cmd, 1, 8,  8,  char,	    type[1]);\
	MC_RSP_OP(cmd, 1, 16, 8,  char,	    type[2]);\
	MC_RSP_OP(cmd, 1, 24, 8,  char,	    type[3]);\
	MC_RSP_OP(cmd, 1, 32, 8,  char,	    type[4]);\
	MC_RSP_OP(cmd, 1, 40, 8,  char,	    type[5]);\
	MC_RSP_OP(cmd, 1, 48, 8,  char,	    type[6]);\
	MC_RSP_OP(cmd, 1, 56, 8,  char,	    type[7]);\
	MC_RSP_OP(cmd, 2, 0,  8,  char,	    type[8]);\
	MC_RSP_OP(cmd, 2, 8,  8,  char,	    type[9]);\
	MC_RSP_OP(cmd, 2, 16, 8,  char,	    type[10]);\
	MC_RSP_OP(cmd, 2, 24, 8,  char,	    type[11]);\
	MC_RSP_OP(cmd, 2, 32, 8,  char,	    type[12]);\
	MC_RSP_OP(cmd, 2, 40, 8,  char,	    type[13]);\
	MC_RSP_OP(cmd, 2, 48, 8,  char,     type[14]);\
	MC_RSP_OP(cmd, 2, 56, 8,  char,	    type[15]);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_RSP_GET_OBJ_COUNT(cmd, obj_count) \
	MC_RSP_OP(cmd, 0, 32, 32, int,      obj_count)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_CMD_GET_OBJ(cmd, obj_index) \
	MC_CMD_OP(cmd, 0, 0,  32, int,	    obj_index)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_RSP_GET_OBJ(cmd, obj_desc) \
do { \
	MC_RSP_OP(cmd, 0, 32, 32, int,	    obj_desc->id); \
	MC_RSP_OP(cmd, 1, 0,  16, uint16_t, obj_desc->vendor); \
	MC_RSP_OP(cmd, 1, 16, 8,  uint8_t,  obj_desc->irq_count); \
	MC_RSP_OP(cmd, 1, 24, 8,  uint8_t,  obj_desc->region_count); \
	MC_RSP_OP(cmd, 1, 32, 32, uint32_t, obj_desc->state);\
	MC_RSP_OP(cmd, 2, 0,  16, uint16_t, obj_desc->ver_major);\
	MC_RSP_OP(cmd, 2, 16, 16, uint16_t, obj_desc->ver_minor);\
	MC_RSP_OP(cmd, 3, 0,  8,  char,	    obj_desc->type[0]);\
	MC_RSP_OP(cmd, 3, 8,  8,  char,	    obj_desc->type[1]);\
	MC_RSP_OP(cmd, 3, 16, 8,  char,	    obj_desc->type[2]);\
	MC_RSP_OP(cmd, 3, 24, 8,  char,	    obj_desc->type[3]);\
	MC_RSP_OP(cmd, 3, 32, 8,  char,	    obj_desc->type[4]);\
	MC_RSP_OP(cmd, 3, 40, 8,  char,	    obj_desc->type[5]);\
	MC_RSP_OP(cmd, 3, 48, 8,  char,	    obj_desc->type[6]);\
	MC_RSP_OP(cmd, 3, 56, 8,  char,	    obj_desc->type[7]);\
	MC_RSP_OP(cmd, 4, 0,  8,  char,	    obj_desc->type[8]);\
	MC_RSP_OP(cmd, 4, 8,  8,  char,	    obj_desc->type[9]);\
	MC_RSP_OP(cmd, 4, 16, 8,  char,	    obj_desc->type[10]);\
	MC_RSP_OP(cmd, 4, 24, 8,  char,	    obj_desc->type[11]);\
	MC_RSP_OP(cmd, 4, 32, 8,  char,	    obj_desc->type[12]);\
	MC_RSP_OP(cmd, 4, 40, 8,  char,	    obj_desc->type[13]);\
	MC_RSP_OP(cmd, 4, 48, 8,  char,	    obj_desc->type[14]);\
	MC_RSP_OP(cmd, 4, 56, 8,  char,	    obj_desc->type[15]);\
	MC_RSP_OP(cmd, 5, 0,  8,  char,	    obj_desc->label[0]);\
	MC_RSP_OP(cmd, 5, 8,  8,  char,	    obj_desc->label[1]);\
	MC_RSP_OP(cmd, 5, 16, 8,  char,	    obj_desc->label[2]);\
	MC_RSP_OP(cmd, 5, 24, 8,  char,	    obj_desc->label[3]);\
	MC_RSP_OP(cmd, 5, 32, 8,  char,	    obj_desc->label[4]);\
	MC_RSP_OP(cmd, 5, 40, 8,  char,	    obj_desc->label[5]);\
	MC_RSP_OP(cmd, 5, 48, 8,  char,	    obj_desc->label[6]);\
	MC_RSP_OP(cmd, 5, 56, 8,  char,	    obj_desc->label[7]);\
	MC_RSP_OP(cmd, 6, 0,  8,  char,	    obj_desc->label[8]);\
	MC_RSP_OP(cmd, 6, 8,  8,  char,	    obj_desc->label[9]);\
	MC_RSP_OP(cmd, 6, 16, 8,  char,	    obj_desc->label[10]);\
	MC_RSP_OP(cmd, 6, 24, 8,  char,	    obj_desc->label[11]);\
	MC_RSP_OP(cmd, 6, 32, 8,  char,	    obj_desc->label[12]);\
	MC_RSP_OP(cmd, 6, 40, 8,  char,	    obj_desc->label[13]);\
	MC_RSP_OP(cmd, 6, 48, 8,  char,	    obj_desc->label[14]);\
	MC_RSP_OP(cmd, 6, 56, 8,  char,	    obj_desc->label[15]);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_CMD_GET_RES_COUNT(cmd, type) \
do { \
	MC_CMD_OP(cmd, 1, 0,  8,  char,	    type[0]);\
	MC_CMD_OP(cmd, 1, 8,  8,  char,	    type[1]);\
	MC_CMD_OP(cmd, 1, 16, 8,  char,	    type[2]);\
	MC_CMD_OP(cmd, 1, 24, 8,  char,	    type[3]);\
	MC_CMD_OP(cmd, 1, 32, 8,  char,	    type[4]);\
	MC_CMD_OP(cmd, 1, 40, 8,  char,	    type[5]);\
	MC_CMD_OP(cmd, 1, 48, 8,  char,	    type[6]);\
	MC_CMD_OP(cmd, 1, 56, 8,  char,	    type[7]);\
	MC_CMD_OP(cmd, 2, 0,  8,  char,	    type[8]);\
	MC_CMD_OP(cmd, 2, 8,  8,  char,	    type[9]);\
	MC_CMD_OP(cmd, 2, 16, 8,  char,	    type[10]);\
	MC_CMD_OP(cmd, 2, 24, 8,  char,	    type[11]);\
	MC_CMD_OP(cmd, 2, 32, 8,  char,	    type[12]);\
	MC_CMD_OP(cmd, 2, 40, 8,  char,	    type[13]);\
	MC_CMD_OP(cmd, 2, 48, 8,  char,	    type[14]);\
	MC_CMD_OP(cmd, 2, 56, 8,  char,	    type[15]);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_RSP_GET_RES_COUNT(cmd, res_count) \
	MC_RSP_OP(cmd, 0, 0,  32, int,	    res_count)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_CMD_GET_RES_IDS(cmd, range_desc, type) \
do { \
	MC_CMD_OP(cmd, 0, 42, 7,  enum dprc_iter_status, \
					    range_desc->iter_status); \
	MC_CMD_OP(cmd, 1, 0,  32, int,	    range_desc->base_id); \
	MC_CMD_OP(cmd, 1, 32, 32, int,	    range_desc->last_id);\
	MC_CMD_OP(cmd, 2, 0,  8,  char,	    type[0]);\
	MC_CMD_OP(cmd, 2, 8,  8,  char,	    type[1]);\
	MC_CMD_OP(cmd, 2, 16, 8,  char,	    type[2]);\
	MC_CMD_OP(cmd, 2, 24, 8,  char,	    type[3]);\
	MC_CMD_OP(cmd, 2, 32, 8,  char,	    type[4]);\
	MC_CMD_OP(cmd, 2, 40, 8,  char,     type[5]);\
	MC_CMD_OP(cmd, 2, 48, 8,  char,	    type[6]);\
	MC_CMD_OP(cmd, 2, 56, 8,  char,	    type[7]);\
	MC_CMD_OP(cmd, 3, 0,  8,  char,	    type[8]);\
	MC_CMD_OP(cmd, 3, 8,  8,  char,	    type[9]);\
	MC_CMD_OP(cmd, 3, 16, 8,  char,	    type[10]);\
	MC_CMD_OP(cmd, 3, 24, 8,  char,	    type[11]);\
	MC_CMD_OP(cmd, 3, 32, 8,  char,	    type[12]);\
	MC_CMD_OP(cmd, 3, 40, 8,  char,	    type[13]);\
	MC_CMD_OP(cmd, 3, 48, 8,  char,	    type[14]);\
	MC_CMD_OP(cmd, 3, 56, 8,  char,	    type[15]);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_RSP_GET_RES_IDS(cmd, range_desc) \
do { \
	MC_RSP_OP(cmd, 0, 42, 7,  enum dprc_iter_status, \
					    range_desc->iter_status);\
	MC_RSP_OP(cmd, 1, 0,  32, int,	    range_desc->base_id); \
	MC_RSP_OP(cmd, 1, 32, 32, int,	    range_desc->last_id);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_CMD_SET_OBJ_LABEL(cmd, obj_type, obj_id, label) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, int,      obj_id); \
	MC_CMD_OP(cmd, 1, 0,  8,  char,	    label[0]);\
	MC_CMD_OP(cmd, 1, 8,  8,  char,	    label[1]);\
	MC_CMD_OP(cmd, 1, 16, 8,  char,	    label[2]);\
	MC_CMD_OP(cmd, 1, 24, 8,  char,	    label[3]);\
	MC_CMD_OP(cmd, 1, 32, 8,  char,	    label[4]);\
	MC_CMD_OP(cmd, 1, 40, 8,  char,	    label[5]);\
	MC_CMD_OP(cmd, 1, 48, 8,  char,	    label[6]);\
	MC_CMD_OP(cmd, 1, 56, 8,  char,	    label[7]);\
	MC_CMD_OP(cmd, 2, 0,  8,  char,	    label[8]);\
	MC_CMD_OP(cmd, 2, 8,  8,  char,	    label[9]);\
	MC_CMD_OP(cmd, 2, 16, 8,  char,	    label[10]);\
	MC_CMD_OP(cmd, 2, 24, 8,  char,	    label[11]);\
	MC_CMD_OP(cmd, 2, 32, 8,  char,	    label[12]);\
	MC_CMD_OP(cmd, 2, 40, 8,  char,	    label[13]);\
	MC_CMD_OP(cmd, 2, 48, 8,  char,	    label[14]);\
	MC_CMD_OP(cmd, 2, 56, 8,  char,	    label[15]);\
	MC_CMD_OP(cmd, 3, 0,  8,  char,	    obj_type[0]);\
	MC_CMD_OP(cmd, 3, 8,  8,  char,	    obj_type[1]);\
	MC_CMD_OP(cmd, 3, 16, 8,  char,	    obj_type[2]);\
	MC_CMD_OP(cmd, 3, 24, 8,  char,	    obj_type[3]);\
	MC_CMD_OP(cmd, 3, 32, 8,  char,	    obj_type[4]);\
	MC_CMD_OP(cmd, 3, 40, 8,  char,	    obj_type[5]);\
	MC_CMD_OP(cmd, 3, 48, 8,  char,	    obj_type[6]);\
	MC_CMD_OP(cmd, 3, 56, 8,  char,	    obj_type[7]);\
	MC_CMD_OP(cmd, 4, 0,  8,  char,	    obj_type[8]);\
	MC_CMD_OP(cmd, 4, 8,  8,  char,	    obj_type[9]);\
	MC_CMD_OP(cmd, 4, 16, 8,  char,	    obj_type[10]);\
	MC_CMD_OP(cmd, 4, 24, 8,  char,	    obj_type[11]);\
	MC_CMD_OP(cmd, 4, 32, 8,  char,	    obj_type[12]);\
	MC_CMD_OP(cmd, 4, 40, 8,  char,	    obj_type[13]);\
	MC_CMD_OP(cmd, 4, 48, 8,  char,	    obj_type[14]);\
	MC_CMD_OP(cmd, 4, 56, 8,  char,	    obj_type[15]);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_CMD_CONNECT(cmd, endpoint1, endpoint2, cfg) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, int,      endpoint1->id); \
	MC_CMD_OP(cmd, 0, 32, 16, uint16_t, endpoint1->if_id); \
	MC_CMD_OP(cmd, 1, 0,  32, int,	    endpoint2->id); \
	MC_CMD_OP(cmd, 1, 32, 16, uint16_t, endpoint2->if_id); \
	MC_CMD_OP(cmd, 2, 0,  8,  char,     endpoint1->type[0]); \
	MC_CMD_OP(cmd, 2, 8,  8,  char,	    endpoint1->type[1]); \
	MC_CMD_OP(cmd, 2, 16, 8,  char,	    endpoint1->type[2]); \
	MC_CMD_OP(cmd, 2, 24, 8,  char,	    endpoint1->type[3]); \
	MC_CMD_OP(cmd, 2, 32, 8,  char,	    endpoint1->type[4]); \
	MC_CMD_OP(cmd, 2, 40, 8,  char,	    endpoint1->type[5]); \
	MC_CMD_OP(cmd, 2, 48, 8,  char,	    endpoint1->type[6]); \
	MC_CMD_OP(cmd, 2, 56, 8,  char,	    endpoint1->type[7]); \
	MC_CMD_OP(cmd, 3, 0,  8,  char,	    endpoint1->type[8]); \
	MC_CMD_OP(cmd, 3, 8,  8,  char,	    endpoint1->type[9]); \
	MC_CMD_OP(cmd, 3, 16, 8,  char,	    endpoint1->type[10]); \
	MC_CMD_OP(cmd, 3, 24, 8,  char,	    endpoint1->type[11]); \
	MC_CMD_OP(cmd, 3, 32, 8,  char,     endpoint1->type[12]); \
	MC_CMD_OP(cmd, 3, 40, 8,  char,	    endpoint1->type[13]); \
	MC_CMD_OP(cmd, 3, 48, 8,  char,	    endpoint1->type[14]); \
	MC_CMD_OP(cmd, 3, 56, 8,  char,	    endpoint1->type[15]); \
	MC_CMD_OP(cmd, 4, 0,  32, uint32_t, cfg->max_rate); \
	MC_CMD_OP(cmd, 4, 32, 32, uint32_t, cfg->committed_rate); \
	MC_CMD_OP(cmd, 5, 0,  8,  char,	    endpoint2->type[0]); \
	MC_CMD_OP(cmd, 5, 8,  8,  char,	    endpoint2->type[1]); \
	MC_CMD_OP(cmd, 5, 16, 8,  char,	    endpoint2->type[2]); \
	MC_CMD_OP(cmd, 5, 24, 8,  char,	    endpoint2->type[3]); \
	MC_CMD_OP(cmd, 5, 32, 8,  char,	    endpoint2->type[4]); \
	MC_CMD_OP(cmd, 5, 40, 8,  char,	    endpoint2->type[5]); \
	MC_CMD_OP(cmd, 5, 48, 8,  char,	    endpoint2->type[6]); \
	MC_CMD_OP(cmd, 5, 56, 8,  char,	    endpoint2->type[7]); \
	MC_CMD_OP(cmd, 6, 0,  8,  char,	    endpoint2->type[8]); \
	MC_CMD_OP(cmd, 6, 8,  8,  char,	    endpoint2->type[9]); \
	MC_CMD_OP(cmd, 6, 16, 8,  char,	    endpoint2->type[10]); \
	MC_CMD_OP(cmd, 6, 24, 8,  char,	    endpoint2->type[11]); \
	MC_CMD_OP(cmd, 6, 32, 8,  char,	    endpoint2->type[12]); \
	MC_CMD_OP(cmd, 6, 40, 8,  char,	    endpoint2->type[13]); \
	MC_CMD_OP(cmd, 6, 48, 8,  char,	    endpoint2->type[14]); \
	MC_CMD_OP(cmd, 6, 56, 8,  char,	    endpoint2->type[15]); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_CMD_DISCONNECT(cmd, endpoint) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, int,	    endpoint->id); \
	MC_CMD_OP(cmd, 0, 32, 16, uint16_t, endpoint->if_id); \
	MC_CMD_OP(cmd, 1, 0,  8,  char,	    endpoint->type[0]); \
	MC_CMD_OP(cmd, 1, 8,  8,  char,	    endpoint->type[1]); \
	MC_CMD_OP(cmd, 1, 16, 8,  char,	    endpoint->type[2]); \
	MC_CMD_OP(cmd, 1, 24, 8,  char,	    endpoint->type[3]); \
	MC_CMD_OP(cmd, 1, 32, 8,  char,	    endpoint->type[4]); \
	MC_CMD_OP(cmd, 1, 40, 8,  char,	    endpoint->type[5]); \
	MC_CMD_OP(cmd, 1, 48, 8,  char,	    endpoint->type[6]); \
	MC_CMD_OP(cmd, 1, 56, 8,  char,	    endpoint->type[7]); \
	MC_CMD_OP(cmd, 2, 0,  8,  char,	    endpoint->type[8]); \
	MC_CMD_OP(cmd, 2, 8,  8,  char,	    endpoint->type[9]); \
	MC_CMD_OP(cmd, 2, 16, 8,  char,	    endpoint->type[10]); \
	MC_CMD_OP(cmd, 2, 24, 8,  char,	    endpoint->type[11]); \
	MC_CMD_OP(cmd, 2, 32, 8,  char,	    endpoint->type[12]); \
	MC_CMD_OP(cmd, 2, 40, 8,  char,	    endpoint->type[13]); \
	MC_CMD_OP(cmd, 2, 48, 8,  char,	    endpoint->type[14]); \
	MC_CMD_OP(cmd, 2, 56, 8,  char,	    endpoint->type[15]); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_CMD_GET_CONNECTION(cmd, endpoint1) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, int,      endpoint1->id); \
	MC_CMD_OP(cmd, 0, 32, 16, uint16_t, endpoint1->if_id); \
	MC_CMD_OP(cmd, 1, 0,  8,  char,     endpoint1->type[0]); \
	MC_CMD_OP(cmd, 1, 8,  8,  char,	    endpoint1->type[1]); \
	MC_CMD_OP(cmd, 1, 16, 8,  char,	    endpoint1->type[2]); \
	MC_CMD_OP(cmd, 1, 24, 8,  char,	    endpoint1->type[3]); \
	MC_CMD_OP(cmd, 1, 32, 8,  char,	    endpoint1->type[4]); \
	MC_CMD_OP(cmd, 1, 40, 8,  char,	    endpoint1->type[5]); \
	MC_CMD_OP(cmd, 1, 48, 8,  char,	    endpoint1->type[6]); \
	MC_CMD_OP(cmd, 1, 56, 8,  char,	    endpoint1->type[7]); \
	MC_CMD_OP(cmd, 2, 0,  8,  char,	    endpoint1->type[8]); \
	MC_CMD_OP(cmd, 2, 8,  8,  char,	    endpoint1->type[9]); \
	MC_CMD_OP(cmd, 2, 16, 8,  char,	    endpoint1->type[10]); \
	MC_CMD_OP(cmd, 2, 24, 8,  char,	    endpoint1->type[11]); \
	MC_CMD_OP(cmd, 2, 32, 8,  char,     endpoint1->type[12]); \
	MC_CMD_OP(cmd, 2, 40, 8,  char,	    endpoint1->type[13]); \
	MC_CMD_OP(cmd, 2, 48, 8,  char,	    endpoint1->type[14]); \
	MC_CMD_OP(cmd, 2, 56, 8,  char,	    endpoint1->type[15]); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPRC_RSP_GET_CONNECTION(cmd, endpoint2, state) \
do { \
	MC_RSP_OP(cmd, 3, 0,  32, int,	    endpoint2->id); \
	MC_RSP_OP(cmd, 3, 32, 16, uint16_t, endpoint2->if_id); \
	MC_RSP_OP(cmd, 4, 0,  8,  char,	    endpoint2->type[0]); \
	MC_RSP_OP(cmd, 4, 8,  8,  char,	    endpoint2->type[1]); \
	MC_RSP_OP(cmd, 4, 16, 8,  char,	    endpoint2->type[2]); \
	MC_RSP_OP(cmd, 4, 24, 8,  char,	    endpoint2->type[3]); \
	MC_RSP_OP(cmd, 4, 32, 8,  char,	    endpoint2->type[4]); \
	MC_RSP_OP(cmd, 4, 40, 8,  char,	    endpoint2->type[5]); \
	MC_RSP_OP(cmd, 4, 48, 8,  char,	    endpoint2->type[6]); \
	MC_RSP_OP(cmd, 4, 56, 8,  char,	    endpoint2->type[7]); \
	MC_RSP_OP(cmd, 5, 0,  8,  char,	    endpoint2->type[8]); \
	MC_RSP_OP(cmd, 5, 8,  8,  char,	    endpoint2->type[9]); \
	MC_RSP_OP(cmd, 5, 16, 8,  char,	    endpoint2->type[10]); \
	MC_RSP_OP(cmd, 5, 24, 8,  char,	    endpoint2->type[11]); \
	MC_RSP_OP(cmd, 5, 32, 8,  char,	    endpoint2->type[12]); \
	MC_RSP_OP(cmd, 5, 40, 8,  char,	    endpoint2->type[13]); \
	MC_RSP_OP(cmd, 5, 48, 8,  char,	    endpoint2->type[14]); \
	MC_RSP_OP(cmd, 5, 56, 8,  char,	    endpoint2->type[15]); \
	MC_RSP_OP(cmd, 6, 0,  32, int,	    state); \
} while (0)

#endif /* _FSL_DPRC_CMD_H */

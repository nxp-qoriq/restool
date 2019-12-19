/*
 * Copyright 2013-2016 Freescale Semiconductor Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the above-listed copyright holders nor the
 *       names of any contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
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

/* Minimal supported DPRC Version */
#define DPRC_VER_MAJOR			6
#define DPRC_VER_MINOR			4

/* Command versioning */
#define DPRC_CMD_BASE_VERSION			1
#define DPRC_CMD_VERSION_2			2
#define DPRC_CMD_ID_OFFSET			4

#define DPRC_CMD(id)	((id << DPRC_CMD_ID_OFFSET) | DPRC_CMD_BASE_VERSION)
#define DPRC_CMD_V2(id)	(((id) << DPRC_CMD_ID_OFFSET) | DPRC_CMD_VERSION_2)

/* Command IDs */
#define DPRC_CMDID_CLOSE                        DPRC_CMD(0x800)
#define DPRC_CMDID_OPEN                         DPRC_CMD(0x805)
#define DPRC_CMDID_GET_API_VERSION              DPRC_CMD(0xa05)
#define DPRC_CMDID_GET_ATTR                     DPRC_CMD(0x004)
#define DPRC_CMDID_CREATE_CONT                  DPRC_CMD_V2(0x151)
#define DPRC_CMDID_DESTROY_CONT                 DPRC_CMD(0x152)
#define DPRC_CMDID_GET_IRQ_MASK                 DPRC_CMD(0x015)
#define DPRC_CMDID_GET_IRQ_STATUS               DPRC_CMD(0x016)
#define DPRC_CMDID_GET_CONT_ID                  DPRC_CMD(0x830)
#define DPRC_CMDID_ASSIGN                       DPRC_CMD(0x157)
#define DPRC_CMDID_UNASSIGN                     DPRC_CMD(0x158)
#define DPRC_CMDID_GET_OBJ_COUNT                DPRC_CMD(0x159)
#define DPRC_CMDID_GET_OBJ                      DPRC_CMD(0x15A)
#define DPRC_CMDID_GET_RES_COUNT                DPRC_CMD(0x15B)
#define DPRC_CMDID_GET_RES_IDS                  DPRC_CMD(0x15C)
#define DPRC_CMDID_SET_OBJ_LABEL                DPRC_CMD(0x161)
#define DPRC_CMDID_SET_LOCKED                   DPRC_CMD(0x16B)

#define DPRC_CMDID_CONNECT                      DPRC_CMD(0x167)
#define DPRC_CMDID_DISCONNECT                   DPRC_CMD(0x168)
#define DPRC_CMDID_GET_POOL                     DPRC_CMD(0x169)
#define DPRC_CMDID_GET_POOL_COUNT               DPRC_CMD(0x16A)

#define DPRC_CMDID_GET_CONNECTION               DPRC_CMD(0x16C)

/* Macros for accessing command fields smaller than 1byte */
#define DPRC_MASK(field)        \
	GENMASK(DPRC_##field##_SHIFT + DPRC_##field##_SIZE - 1, \
		DPRC_##field##_SHIFT)
#define dprc_set_field(var, field, val) \
	((var) |= (((val) << DPRC_##field##_SHIFT) & DPRC_MASK(field)))
#define dprc_get_field(var, field)      \
	(((var) & DPRC_MASK(field)) >> DPRC_##field##_SHIFT)

#pragma pack(push, 1)
struct dprc_cmd_open {
	uint32_t container_id;
};

struct dprc_cmd_create_container {
	uint32_t options;
	uint32_t icid;
	uint32_t pad1;
	uint32_t portal_id;
	uint8_t label[16];
};

struct dprc_rsp_create_container {
	uint64_t pad0;
	uint32_t child_container_id;
	uint32_t pad1;
	uint64_t child_portal_addr;
};

struct dprc_cmd_destroy_container {
	uint32_t child_container_id;
};

struct dprc_cmd_set_locked {
	uint8_t locked;
	uint8_t pad[3];
	uint32_t child_container_id;
};

struct dprc_rsp_get_attributes {
	uint32_t container_id;
	uint32_t icid;
	uint32_t options;
	uint32_t portal_id;
};

struct dprc_cmd_assign {
	uint32_t container_id;
	uint32_t options;
	uint32_t num;
	uint32_t id_base_align;
	uint8_t type[16];
};

struct dprc_cmd_unassign {
	uint32_t child_container_id;
	uint32_t options;
	uint32_t num;
	uint32_t id_base_align;
	uint8_t type[16];
};

struct dprc_rsp_get_pool_count {
	uint32_t pool_count;
};

struct dprc_cmd_get_pool {
	uint32_t pool_index;
};

struct dprc_rsp_get_pool {
	uint64_t pad;
	uint8_t type[16];
};

struct dprc_rsp_get_obj_count {
	uint32_t pad;
	uint32_t obj_count;
};

struct dprc_cmd_get_obj {
	uint32_t obj_index;
};

struct dprc_rsp_get_obj {
	uint32_t pad0;
	uint32_t id;
	uint16_t vendor;
	uint8_t irq_count;
	uint8_t region_count;
	uint32_t state;
	uint16_t version_major;
	uint16_t version_minor;
	uint16_t flags;
	uint16_t pad1;
	uint8_t type[16];
	uint8_t label[16];
};

struct dprc_cmd_get_res_count {
	uint64_t pad;
	uint8_t type[16];
};

struct dprc_rsp_get_res_count {
	uint32_t res_count;
};

#define DPRC_ITER_STATUS_LO_SHIFT	2
#define DPRC_ITER_STATUS_LO_SIZE	6

#define DPRC_ITER_STATUS_HI_SHIFT	0
#define DPRC_ITER_STATUS_HI_SIZE	1

struct dprc_cmd_get_res_ids {
	uint8_t pad0[5];
	uint8_t iter_status_lo;
	uint8_t iter_status_hi;
	uint8_t pad1;
	uint32_t base_id;
	uint32_t last_id;
	uint8_t type[16];
};

struct dprc_rsp_get_res_ids {
	uint8_t pad0[5];
	uint8_t iter_status_lo;
	uint8_t iter_status_hi;
	uint8_t pad1;
	uint32_t base_id;
	uint32_t last_id;
};

struct dprc_cmd_set_obj_label {
	uint32_t obj_id;
	uint32_t pad;
	uint8_t label[16];
	uint8_t obj_type[16];
};

struct dprc_cmd_get_irq_mask {
	uint32_t pad;
	uint8_t irq_index;
};

struct dprc_rsp_get_irq_mask {
	uint32_t mask;
};

struct dprc_cmd_get_irq_status {
	uint32_t status;
	uint8_t irq_index;
};

struct dprc_rsp_get_irq_status {
	uint32_t status;
};

struct dprc_cmd_connect {
	uint32_t ep1_id;
	uint16_t ep1_interface_id;
	uint16_t pad0;

	uint32_t ep2_id;
	uint16_t ep2_interface_id;
	uint16_t pad1;

	uint8_t ep1_type[16];

	uint32_t max_rate;
	uint32_t committed_rate;

	uint8_t ep2_type[16];
};

struct dprc_cmd_disconnect {
	uint32_t id;
	uint32_t interface_id;
	uint8_t type[16];
};

struct dprc_cmd_get_connection {
	uint32_t ep1_id;
	uint16_t ep1_interface_id;
	uint16_t pad;

	uint8_t ep1_type[16];
};

struct dprc_rsp_get_connection {
	uint64_t pad[3];
	uint32_t ep2_id;
	uint16_t ep2_interface_id;
	uint16_t pad1;
	uint8_t ep2_type[16];
	uint32_t state;
};

struct dprc_rsp_get_api_version {
	uint16_t major;
	uint16_t minor;
};
#pragma pack(pop)
#endif /* _FSL_DPRC_CMD_H */

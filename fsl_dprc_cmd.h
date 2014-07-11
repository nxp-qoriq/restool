/**************************************************************************//*
 @File          fsl_dprc_cmd.h

 @Description   defines dprc portal commands

 @Cautions      None.
 *//***************************************************************************/

#ifndef _FSL_DPRC_CMD_H
#define _FSL_DPRC_CMD_H

/* Command IDs */
#define MC_CMDID_CLOSE				0x800
#define MC_DPRC_CMDID_OPEN			0x805
#define MC_DPRC_CMDID_CREATE			0x905

#define DPRC_CMDID_CREATE_CONT			0x151
#define DPRC_CMDID_DESTROY_CONT			0x152
#define DPRC_CMDID_GET_CONT_ID			0x830
#define DPRC_CMDID_RESET_CONT			0x154
#define DPRC_CMDID_SET_RES_QUOTA		0x155
#define DPRC_CMDID_GET_RES_QUOTA		0x156
#define DPRC_CMDID_ASSIGN			0x157
#define DPRC_CMDID_UNASSIGN			0x158
#define DPRC_CMDID_GET_OBJ_COUNT		0x159
#define DPRC_CMDID_GET_OBJECT			0x15A
#define DPRC_CMDID_GET_RES_COUNT		0x15B
#define DPRC_CMDID_GET_RES_IDS			0x15C
#define DPRC_CMDID_GET_ATTR			0x15D
#define DPRC_CMDID_GET_OBJ_REG			0x15E
#define DPRC_CMDID_SET_IRQ			0x15F
#define DPRC_CMDID_GET_IRQ			0x160
#define DPRC_CMDID_SET_IRQ_ENABLE		0x161
#define DPRC_CMDID_GET_IRQ_ENABLE		0x162
#define DPRC_CMDID_SET_IRQ_MASK			0x163
#define DPRC_CMDID_GET_IRQ_MASK			0x164
#define DPRC_CMDID_GET_IRQ_STATUS		0x165
#define DPRC_CMDID_CLEAR_IRQ_STATUS		0x166
#define DPRC_CMDID_CONNECT			0x167
#define DPRC_CMDID_DISCONNECT			0x168
#define DPRC_CMDID_GET_POOL			0x169
#define DPRC_CMDID_GET_POOL_COUNT		0x16A
#define DPRC_CMDID_GET_PORTAL_PADDR		0x16B

/* Command sizes */
#define MC_CMD_OPEN_SIZE			8
#define MC_CMD_CLOSE_SIZE			0
#define DPRC_CMDSZ_CREATE_CONT			(8 * 2)
#define DPRC_CMDSZ_DESTROY_CONT			8
#define DPRC_CMDSZ_GET_CONT_ID			8
#define DPRC_CMDSZ_RESET_CONT			8
#define DPRC_CMDSZ_SET_RES_QUOTA		(8 * 3)
#define DPRC_CMDSZ_GET_RES_QUOTA		(8 * 3)
#define DPRC_CMDSZ_ASSIGN			(8 * 4)
#define DPRC_CMDSZ_UNASSIGN			(8 * 4)
#define DPRC_CMDSZ_GET_OBJ_COUNT		0
#define DPRC_CMDSZ_GET_OBJECT			(8 * 5)
#define DPRC_CMDSZ_GET_RES_COUNT		8
#define DPRC_CMDSZ_GET_RES_IDS			(8 * 4)
#define DPRC_CMDSZ_GET_ATTR			(8 * 3)
#define DPRC_CMDSZ_GET_OBJ_REG			(8 * 5)
#define DPRC_CMDSZ_SET_IRQ			(8 * 3)
#define DPRC_CMDSZ_GET_IRQ			(8 * 3)
#define DPRC_CMDSZ_SET_IRQ_ENABLE		8
#define DPRC_CMDSZ_GET_IRQ_ENABLE		8
#define DPRC_CMDSZ_SET_IRQ_MASK			8
#define DPRC_CMDSZ_GET_IRQ_MASK			8
#define DPRC_CMDSZ_GET_IRQ_STATUS		8
#define DPRC_CMDSZ_CLEAR_IRQ_STATUS		8
#define DPRC_CMDSZ_CONNECT			(8 * 7)
#define DPRC_CMDSZ_DISCONNECT			(8 * 3)
#define DPRC_CMDSZ_GET_POOL			(8 * 3)
#define DPRC_CMDSZ_GET_POOL_COUNT		8
#define DPRC_CMDSZ_GET_PORTAL_PADDR		(8 * 2)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_OPEN(_OP) \
	_OP(0,  0,	32,	int,			container_id)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_GET_CONTAINER_ID(_OP) \
	_OP(0,  32,	32,	uint32_t,		portal_id)

/*	param, offset, width,	type,			arg_name */
#define DPRC_RSP_GET_CONTAINER_ID(_OP) \
	_OP(0,  0,	32,	int,			container_id)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_CREATE_CONTAINER(_OP) \
do { \
	_OP(0,	32,	16,	uint16_t,		cfg->icid); \
	_OP(0,  0,	32,	uint32_t,		cfg->options); \
	_OP(1,  32,	32,	int,			cfg->portal_id); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPRC_RSP_CREATE_CONTAINER(_OP) \
do { \
	_OP(1,  0,	32,	int,			child_container_id); \
	_OP(2,  0,	64,	uint64_t,		child_portal_paddr);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_DESTROY_CONTAINER(_OP) \
	_OP(0,	0,	32,	int,			child_container_id)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_RESET_CONTAINER(_OP) \
	_OP(0,	0,	32,	int,			child_container_id)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_SET_RES_QUOTA(_OP) \
do { \
	_OP(0,	0,	32,	int,			child_container_id); \
	_OP(0,  32,	16,	uint16_t,		quota);\
	_OP(1,  0,	8,	char,			type[0]);\
	_OP(1,  8,	8,	char,			type[1]);\
	_OP(1,  16,	8,	char,			type[2]);\
	_OP(1,  24,	8,	char,			type[3]);\
	_OP(1,  32,	8,	char,			type[4]);\
	_OP(1,  40,	8,	char,			type[5]);\
	_OP(1,  48,	8,	char,			type[6]);\
	_OP(1,  56,	8,	char,			type[7]);\
	_OP(2,  0,	8,	char,			type[8]);\
	_OP(2,  8,	8,	char,			type[9]);\
	_OP(2,  16,	8,	char,			type[10]);\
	_OP(2,  24,	8,	char,			type[11]);\
	_OP(2,  32,	8,	char,			type[12]);\
	_OP(2,  40,	8,	char,			type[13]);\
	_OP(2,  48,	8,	char,			type[14]);\
	_OP(2,  56,	8,	char,			type[15]);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_GET_RES_QUOTA(_OP) \
do { \
	_OP(0,	0,	32,	int,			child_container_id); \
	_OP(1,  0,	8,	char,			type[0]);\
	_OP(1,  8,	8,	char,			type[1]);\
	_OP(1,  16,	8,	char,			type[2]);\
	_OP(1,  24,	8,	char,			type[3]);\
	_OP(1,  32,	8,	char,			type[4]);\
	_OP(1,  40,	8,	char,			type[5]);\
	_OP(1,  48,	8,	char,			type[6]);\
	_OP(1,  56,	8,	char,			type[7]);\
	_OP(2,  0,	8,	char,			type[8]);\
	_OP(2,  8,	8,	char,			type[9]);\
	_OP(2,  16,	8,	char,			type[10]);\
	_OP(2,  24,	8,	char,			type[11]);\
	_OP(2,  32,	8,	char,			type[12]);\
	_OP(2,  40,	8,	char,			type[13]);\
	_OP(2,  48,	8,	char,			type[14]);\
	_OP(2,  56,	8,	char,			type[15]);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPRC_RSP_GET_RES_QUOTA(_OP) \
	_OP(0,	32,	16,	uint16_t,		quota)

/*	param, offset, width,	type,		arg_name */
#define DPRC_CMD_ASSIGN(_OP) \
do { \
	_OP(0,	0,	32,	int,		container_id); \
	_OP(0,	32,	32,	uint32_t,	res_req->options);\
	_OP(1,	0,	32,	uint32_t,	res_req->num); \
	_OP(1,	32,	32,	int,		res_req->id_base_align); \
	_OP(2,	0,	8,	char,		res_req->type[0]);\
	_OP(2,	8,	8,	char,		res_req->type[1]);\
	_OP(2,	16,	8,	char,		res_req->type[2]);\
	_OP(2,	24,	8,	char,		res_req->type[3]);\
	_OP(2,	32,	8,	char,		res_req->type[4]);\
	_OP(2,	40,	8,	char,		res_req->type[5]);\
	_OP(2,	48,	8,	char,		res_req->type[6]);\
	_OP(2,	56,	8,	char,		res_req->type[7]);\
	_OP(3,	0,	8,	char,		res_req->type[8]);\
	_OP(3,	8,	8,	char,		res_req->type[9]);\
	_OP(3,	16,	8,	char,		res_req->type[10]);\
	_OP(3,	24,	8,	char,		res_req->type[11]);\
	_OP(3,	32,	8,	char,		res_req->type[12]);\
	_OP(3,	40,	8,	char,		res_req->type[13]);\
	_OP(3,	48,	8,	char,		res_req->type[14]);\
	_OP(3,	56,	8,	char,		res_req->type[15]);\
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPRC_CMD_UNASSIGN(_OP) \
do { \
	_OP(0,	0,	32,	int,		child_container_id); \
	_OP(0,	32,	32,	uint32_t,	res_req->options);\
	_OP(1,	0,	32,	uint32_t,	res_req->num); \
	_OP(1,	32,	32,	int,		res_req->id_base_align); \
	_OP(2,	0,	8,	char,		res_req->type[0]);\
	_OP(2,	8,	8,	char,		res_req->type[1]);\
	_OP(2,	16,	8,	char,		res_req->type[2]);\
	_OP(2,	24,	8,	char,		res_req->type[3]);\
	_OP(2,	32,	8,	char,		res_req->type[4]);\
	_OP(2,	40,	8,	char,		res_req->type[5]);\
	_OP(2,	48,	8,	char,		res_req->type[6]);\
	_OP(2,	56,	8,	char,		res_req->type[7]);\
	_OP(3,	0,	8,	char,		res_req->type[8]);\
	_OP(3,	8,	8,	char,		res_req->type[9]);\
	_OP(3,	16,	8,	char,		res_req->type[10]);\
	_OP(3,	24,	8,	char,		res_req->type[11]);\
	_OP(3,	32,	8,	char,		res_req->type[12]);\
	_OP(3,	40,	8,	char,		res_req->type[13]);\
	_OP(3,	48,	8,	char,		res_req->type[14]);\
	_OP(3,	56,	8,	char,		res_req->type[15]);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPRC_RSP_GET_OBJ_COUNT(_OP) \
	_OP(0,	32,	32,	int,			obj_count)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_GET_OBJECT(_OP) \
	_OP(0,	0,	32,	int,			obj_index)

/*	param, offset, width,	type,		arg_name */
#define DPRC_RSP_GET_OBJECT(_OP) \
do { \
	_OP(0,	32,	32,	int,		obj_desc->id); \
	_OP(1,	0,	16,	uint16_t,	obj_desc->vendor); \
	_OP(1,	16,	8,	uint8_t,	obj_desc->irq_count); \
	_OP(1,	24,	8,	uint8_t,	obj_desc->region_count); \
	_OP(1,	32,	32,	uint32_t,	obj_desc->state);\
	_OP(2,	0,	32,	uint32_t,	obj_desc->ver_minor); \
	_OP(2,	32,	32,	uint32_t,	obj_desc->ver_major); \
	_OP(3,  0,	8,	char,		obj_desc->type[0]);\
	_OP(3,  8,	8,	char,		obj_desc->type[1]);\
	_OP(3,  16,	8,	char,		obj_desc->type[2]);\
	_OP(3,  24,	8,	char,		obj_desc->type[3]);\
	_OP(3,  32,	8,	char,		obj_desc->type[4]);\
	_OP(3,  40,	8,	char,		obj_desc->type[5]);\
	_OP(3,  48,	8,	char,		obj_desc->type[6]);\
	_OP(3,  56,	8,	char,		obj_desc->type[7]);\
	_OP(4,  0,	8,	char,		obj_desc->type[8]);\
	_OP(4,  8,	8,	char,		obj_desc->type[9]);\
	_OP(4,  16,	8,	char,		obj_desc->type[10]);\
	_OP(4,  24,	8,	char,		obj_desc->type[11]);\
	_OP(4,  32,	8,	char,		obj_desc->type[12]);\
	_OP(4,  40,	8,	char,		obj_desc->type[13]);\
	_OP(4,  48,	8,	char,		obj_desc->type[14]);\
	_OP(4,  56,	8,	char,		obj_desc->type[15]);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_GET_RES_COUNT(_OP) \
do { \
	_OP(1,  0,	8,	char,			type[0]);\
	_OP(1,  8,	8,	char,			type[1]);\
	_OP(1,  16,	8,	char,			type[2]);\
	_OP(1,  24,	8,	char,			type[3]);\
	_OP(1,  32,	8,	char,			type[4]);\
	_OP(1,  40,	8,	char,			type[5]);\
	_OP(1,  48,	8,	char,			type[6]);\
	_OP(1,  56,	8,	char,			type[7]);\
	_OP(2,  0,	8,	char,			type[8]);\
	_OP(2,  8,	8,	char,			type[9]);\
	_OP(2,  16,	8,	char,			type[10]);\
	_OP(2,  24,	8,	char,			type[11]);\
	_OP(2,  32,	8,	char,			type[12]);\
	_OP(2,  40,	8,	char,			type[13]);\
	_OP(2,  48,	8,	char,			type[14]);\
	_OP(2,  56,	8,	char,			type[15]);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPRC_RSP_GET_RES_COUNT(_OP) \
	_OP(0,	0,	32,	int,			res_count)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_GET_RES_IDS(_OP) \
do { \
	_OP(0,	42,	7,	enum dprc_iter_status,	\
						range_desc->iter_status); \
	_OP(1,	0,	32,	int,			range_desc->base_id); \
	_OP(1,	32,	32,	int,			range_desc->last_id);\
	_OP(2,  0,	8,	char,			type[0]);\
	_OP(2,  8,	8,	char,			type[1]);\
	_OP(2,  16,	8,	char,			type[2]);\
	_OP(2,  24,	8,	char,			type[3]);\
	_OP(2,  32,	8,	char,			type[4]);\
	_OP(2,  40,	8,	char,			type[5]);\
	_OP(2,  48,	8,	char,			type[6]);\
	_OP(2,  56,	8,	char,			type[7]);\
	_OP(3,  0,	8,	char,			type[8]);\
	_OP(3,  8,	8,	char,			type[9]);\
	_OP(3,  16,	8,	char,			type[10]);\
	_OP(3,  24,	8,	char,			type[11]);\
	_OP(3,  32,	8,	char,			type[12]);\
	_OP(3,  40,	8,	char,			type[13]);\
	_OP(3,  48,	8,	char,			type[14]);\
	_OP(3,  56,	8,	char,			type[15]);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPRC_RSP_GET_RES_IDS(_OP) \
do { \
	_OP(0,	42,	7,	enum dprc_iter_status,	\
						range_desc->iter_status);\
	_OP(1,	0,	32,	int,			range_desc->base_id); \
	_OP(1,	32,	32,	int,			range_desc->last_id);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPRC_RSP_GET_ATTRIBUTES(_OP) \
do { \
	_OP(0,	0,	32,	int,			attr->container_id); \
	_OP(0,	32,	16,	uint16_t,		attr->icid); \
	_OP(0,	48,	16,	uint16_t,		attr->portal_id); \
	_OP(1,	0,	32,	uint32_t,		attr->options);\
	_OP(2,  0,	32,	uint32_t,		attr->version.major);\
	_OP(2,  32,	32,	uint32_t,		attr->version.minor);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_GET_OBJ_REGION(_OP) \
do { \
	_OP(0,	0,	32,	int,			obj_id); \
	_OP(0,	48,	8,	uint8_t,		region_index);\
	_OP(3,  0,	8,	char,			obj_type[0]);\
	_OP(3,  8,	8,	char,			obj_type[1]);\
	_OP(3,  16,	8,	char,			obj_type[2]);\
	_OP(3,  24,	8,	char,			obj_type[3]);\
	_OP(3,  32,	8,	char,			obj_type[4]);\
	_OP(3,  40,	8,	char,			obj_type[5]);\
	_OP(3,  48,	8,	char,			obj_type[6]);\
	_OP(3,  56,	8,	char,			obj_type[7]);\
	_OP(4,  0,	8,	char,			obj_type[8]);\
	_OP(4,  8,	8,	char,			obj_type[9]);\
	_OP(4,  16,	8,	char,			obj_type[10]);\
	_OP(4,  24,	8,	char,			obj_type[11]);\
	_OP(4,  32,	8,	char,			obj_type[12]);\
	_OP(4,  40,	8,	char,			obj_type[13]);\
	_OP(4,  48,	8,	char,			obj_type[14]);\
	_OP(4,  56,	8,	char,			obj_type[15]);\
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPRC_RSP_GET_OBJ_REGION(_OP) \
do { \
	_OP(1,	0,	64,	uint64_t,	region_desc->base_paddr);\
	_OP(2,	0,	32,	uint32_t,	region_desc->size); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_SET_IRQ(_OP) \
do { \
	_OP(0,	32,	8,	uint8_t,		irq_index); \
	_OP(0,	0,	32,	uint32_t,		irq_val); \
	_OP(1,	0,	64,	uint64_t,		irq_paddr);\
	_OP(2,  0,	32,	int,			user_irq_id); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_GET_IRQ(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPRC_RSP_GET_IRQ(_OP) \
do { \
	_OP(0,	0,	32,	uint32_t,		irq_val); \
	_OP(1,	0,	64,	uint64_t,		irq_paddr);\
	_OP(2,  0,	32,	int,			user_irq_id); \
	_OP(2,	32,	32,	int,			type); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_SET_IRQ_ENABLE(_OP) \
do { \
	_OP(0,	0,	8,	uint8_t,		enable_state); \
	_OP(0,	32,	8,	uint8_t,		irq_index);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_GET_IRQ_ENABLE(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPRC_RSP_GET_IRQ_ENABLE(_OP) \
	_OP(0,	0,	8,	uint8_t,		enable_state)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_SET_IRQ_MASK(_OP) \
do { \
	_OP(0,	0,	32,	uint32_t,		mask); \
	_OP(0,	32,	8,	uint8_t,		irq_index);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_GET_IRQ_MASK(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPRC_RSP_GET_IRQ_MASK(_OP) \
	_OP(0,	0,	32,	uint32_t,		mask)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_GET_IRQ_STATUS(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPRC_RSP_GET_IRQ_STATUS(_OP) \
	_OP(0,	0,	32,	uint32_t,		status)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_CLEAR_IRQ_STATUS(_OP) \
do { \
	_OP(0,	0,	32,	uint32_t,		status); \
	_OP(0,	32,	8,	uint8_t,		irq_index);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_CONNECT(_OP) \
do { \
	_OP(0,	0,	32,	int,		endpoint1->id); \
	_OP(0,	32,	32,	int,		endpoint1->interface_id); \
	_OP(1,	0,	32,	int,		endpoint2->id); \
	_OP(1,	32,	32,	int,		endpoint2->interface_id); \
	_OP(2,	0,	8,	char,		endpoint1->type[0]); \
	_OP(2,	8,	8,	char,		endpoint1->type[1]); \
	_OP(2,	16,	8,	char,		endpoint1->type[2]); \
	_OP(2,	24,	8,	char,		endpoint1->type[3]); \
	_OP(2,	32,	8,	char,		endpoint1->type[4]); \
	_OP(2,	40,	8,	char,		endpoint1->type[5]); \
	_OP(2,	48,	8,	char,		endpoint1->type[6]); \
	_OP(2,	56,	8,	char,		endpoint1->type[7]); \
	_OP(3,	0,	8,	char,		endpoint1->type[8]); \
	_OP(3,	8,	8,	char,		endpoint1->type[9]); \
	_OP(3,	16,	8,	char,		endpoint1->type[10]); \
	_OP(3,	24,	8,	char,		endpoint1->type[11]); \
	_OP(3,	32,	8,	char,		endpoint1->type[12]); \
	_OP(3,	40,	8,	char,		endpoint1->type[13]); \
	_OP(3,	48,	8,	char,		endpoint1->type[14]); \
	_OP(3,	56,	8,	char,		endpoint1->type[15]); \
	_OP(5,	0,	8,	char,		endpoint2->type[0]); \
	_OP(5,	8,	8,	char,		endpoint2->type[1]); \
	_OP(5,	16,	8,	char,		endpoint2->type[2]); \
	_OP(5,	24,	8,	char,		endpoint2->type[3]); \
	_OP(5,	32,	8,	char,		endpoint2->type[4]); \
	_OP(5,	40,	8,	char,		endpoint2->type[5]); \
	_OP(5,	48,	8,	char,		endpoint2->type[6]); \
	_OP(5,	56,	8,	char,		endpoint2->type[7]); \
	_OP(6,	0,	8,	char,		endpoint2->type[8]); \
	_OP(6,	8,	8,	char,		endpoint2->type[9]); \
	_OP(6,	16,	8,	char,		endpoint2->type[10]); \
	_OP(6,	24,	8,	char,		endpoint2->type[11]); \
	_OP(6,	32,	8,	char,		endpoint2->type[12]); \
	_OP(6,	40,	8,	char,		endpoint2->type[13]); \
	_OP(6,	48,	8,	char,		endpoint2->type[14]); \
	_OP(6,	56,	8,	char,		endpoint2->type[15]); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_DISCONNECT(_OP) \
do { \
	_OP(0,	0,	32,	int,		endpoint->id); \
	_OP(0,	32,	32,	int,		endpoint->interface_id); \
	_OP(1,	0,	8,	char,		endpoint->type[0]); \
	_OP(1,	8,	8,	char,		endpoint->type[1]); \
	_OP(1,	16,	8,	char,		endpoint->type[2]); \
	_OP(1,	24,	8,	char,		endpoint->type[3]); \
	_OP(1,	32,	8,	char,		endpoint->type[4]); \
	_OP(1,	40,	8,	char,		endpoint->type[5]); \
	_OP(1,	48,	8,	char,		endpoint->type[6]); \
	_OP(1,	56,	8,	char,		endpoint->type[7]); \
	_OP(2,	0,	8,	char,		endpoint->type[8]); \
	_OP(2,	8,	8,	char,		endpoint->type[9]); \
	_OP(2,	16,	8,	char,		endpoint->type[10]); \
	_OP(2,	24,	8,	char,		endpoint->type[11]); \
	_OP(2,	32,	8,	char,		endpoint->type[12]); \
	_OP(2,	40,	8,	char,		endpoint->type[13]); \
	_OP(2,	48,	8,	char,		endpoint->type[14]); \
	_OP(2,	56,	8,	char,		endpoint->type[15]); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_GET_POOL(_OP) \
	_OP(0,	0,	32,	int,		pool_index)

/*	param, offset, width,	type,			arg_name */
#define DPRC_RSP_GET_POOL(_OP) \
do { \
	_OP(1,	0,	8,	char,		type[0]);\
	_OP(1,	8,	8,	char,		type[1]);\
	_OP(1,	16,	8,	char,		type[2]);\
	_OP(1,	24,	8,	char,		type[3]);\
	_OP(1,	32,	8,	char,		type[4]);\
	_OP(1,	40,	8,	char,		type[5]);\
	_OP(1,	48,	8,	char,		type[6]);\
	_OP(1,	56,	8,	char,		type[7]);\
	_OP(2,	0,	8,	char,		type[8]);\
	_OP(2,	8,	8,	char,		type[9]);\
	_OP(2,	16,	8,	char,		type[10]);\
	_OP(2,	24,	8,	char,		type[11]);\
	_OP(2,	32,	8,	char,		type[12]);\
	_OP(2,	40,	8,	char,		type[13]);\
	_OP(2,	48,	8,	char,		type[14]);\
	_OP(2,	56,	8,	char,		type[15]);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPRC_RSP_GET_POOL_COUNT(_OP) \
	_OP(0,	0,	32,	int,		pool_count)

/*	param, offset, width,	type,			arg_name */
#define DPRC_CMD_GET_PORTAL_PADDR(_OP) \
	_OP(0,	0,	32,	int,		portal_id)

/*	param, offset, width,	type,			arg_name */
#define DPRC_RSP_GET_PORTAL_PADDR(_OP) \
	_OP(1,	0,	64,	uint64_t,		portal_addr)
#endif /* _FSL_DPRC_CMD_H */

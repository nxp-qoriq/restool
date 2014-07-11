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

/* Copyright 2014 Freescale Semiconductor, Inc. */
/*!
 *  @file    fsl_mc_dpmng_commands.h
 *  @brief   Management Complex (MC) Data Path Management commands
 */
#ifndef _FSL_MC_DPMNG_COMMANDS_H
#define _FSL_MC_DPMNG_COMMANDS_H

#include <fsl_mc_cmd_common.h>

/**
 * @brief	Management Complex firmware version information
 */
struct mc_version {
	uint32_t major;
	/*!<
	 * Major version number: incremented on API compatibility changes
	 */
	uint32_t minor;
	/*!<
	 * Minor version number: incremented on API additions (backward
	 * compatible); reset when major version is incremented.
	 */
	uint32_t revision;
	/*!<
	 * Internal revision number: incremented on implementation changes
	 * and/or bug fixes that have no impact on API
	 */
};

/*
 * Retrieves the Management Complex firmware version information
 */
#define DPMNG_CMDID_GET_VERSION		0x831
#define DPMNG_CMDSZ_GET_VERSION		(8 * 2)

static inline void build_cmd_mc_get_version(struct mc_command *cmd)
{
	cmd->header = mc_encode_cmd_header(
				DPMNG_CMDID_GET_VERSION,
				DPMNG_CMDSZ_GET_VERSION,
				CMDIF_PRI_LOW,
				0);

	memset(&cmd->data, 0, sizeof(cmd->data));
}

static inline void parse_resp_mc_get_version(
	struct mc_command *cmd,
	struct mc_version *mc_ver_info)
{
	mc_ver_info->revision = lower_32_bits(cmd->data.params[0]);
	mc_ver_info->major = upper_32_bits(cmd->data.params[0]);
	mc_ver_info->minor = cmd->data.params[1] & 0xff;
}

#endif /* _FSL_MC_DPMNG_COMMANDS_H */

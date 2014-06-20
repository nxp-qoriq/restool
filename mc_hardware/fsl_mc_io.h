/*
 * Freescale Layerscape Management Complex I/O functions
 *
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#ifndef __FSL_MC_IO_H
#define __FSL_MC_IO_H

#include <fsl_mc_cmd_common.h>

/**
 * @brief	Writes a command to a Management Complex (MC) portal
 *
 * @param[in]	portal_regs	Pointer to an MC portal's MMIO registers
 * @param[in]	cmd		Pointer to a filled command
 *
 * @returns	'0' on Success; Error code otherwise.
 */
static inline int mc_write_command(
	volatile struct mc_command *portal_regs,
	struct mc_command *cmd)
{
	int i;

	/* copy command parameters into the portal MMIO registers */
	for (i = 0; i < MC_CMD_NUM_OF_PARAMS; i++)
		writeq(cmd->data.params[i], &portal_regs->data.params[i]);

	/* submit the command by writing the header */
	writeq(cmd->header, &portal_regs->header);
	return 0;
}

/**
 * @brief	Reads the response for the last MC command from a Management
 *		Complex (MC) portal
 *
 * @param[in]	portal_regs	Pointer to an MC portal's MMIO registers
 * @param[out]	response	Pointer to command response buffer
 *
 * @returns	MC_CMD_STATUS_OK on Success; Error code otherwise.
 */
static inline enum mc_cmd_status mc_read_response(
	volatile struct mc_command *portal_regs,
	struct mc_command *response)
{
	int i;
	enum mc_cmd_status status;

	/* Copy command response header from MC portal MMIO space: */
	response->header = readq(&portal_regs->header);
	status = (response->header >> 16) & 0xff;

	if (status != MC_CMD_STATUS_OK)
		goto out;

	/* Copy command response data from MC portal MMIO space: */
	for (i = 0; i < MC_CMD_NUM_OF_PARAMS; i++)
		response->data.params[i] =
			readq(&portal_regs->data.params[i]);

out:
	return status;
}

#endif /* __FSL_MC_IO_H */

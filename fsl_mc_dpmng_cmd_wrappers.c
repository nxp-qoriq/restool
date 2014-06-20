/*
 * Freescale Layerscape MC DPMNG command wrappers
 *
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 * Author: German Rivera <German.Rivera@freescale.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#include "fsl_mc_cmd_wrappers.h"
#include <stdint.h>
#include <string.h>
#include "fsl_mc_io_wrapper.h"

int mc_get_version(
	struct mc_portal_wrapper *mc_portal,
	struct mc_version *mc_ver_info)
{
	int error;
	struct mc_command cmd;

	build_cmd_mc_get_version(&cmd);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	parse_resp_mc_get_version(&cmd, mc_ver_info);
	return 0;
}


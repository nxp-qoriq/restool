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
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
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
#include "fsl_mc_sys.h"
#include "fsl_mc_cmd.h"
#include "fsl_dpmng.h"
#include "fsl_dpmng_cmd.h"

int mc_get_version(struct fsl_mc_io *mc_io, struct mc_version *mc_ver_info)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPMNG_CMDID_GET_VERSION,
					  MC_CMD_PRI_LOW, 0);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPMNG_RSP_GET_VERSION(cmd, mc_ver_info);

	return 0;
}

int dpmng_reset_aiop(struct fsl_mc_io *mc_io, int container_id,
		     int aiop_tile_id)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPMNG_CMDID_RESET_AIOP,
					  MC_CMD_PRI_LOW, 0);
	DPMNG_CMD_RESET_AIOP(cmd, container_id, aiop_tile_id);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpmng_load_aiop(struct fsl_mc_io *mc_io,
		    int container_id,
		    int aiop_tile_id,
		    uint64_t img_iova,
		    uint32_t img_size)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPMNG_CMDID_LOAD_AIOP,
					  MC_CMD_PRI_LOW,
					  0);
	DPMNG_CMD_LOAD_AIOP(cmd, container_id, aiop_tile_id, img_size,
			    img_iova);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpmng_run_aiop(struct fsl_mc_io *mc_io,
		   int container_id,
		   int aiop_tile_id,
		   const struct dpmng_aiop_run_cfg *cfg)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPMNG_CMDID_RUN_AIOP,
					  MC_CMD_PRI_LOW,
					  0);
	DPMNG_CMD_RUN_AIOP(cmd, container_id, aiop_tile_id, cfg);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpmng_reset_mc_portal(struct fsl_mc_io *mc_io)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPMNG_CMDID_RESET_MC_PORTAL,
					  MC_CMD_PRI_LOW,
					  0);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

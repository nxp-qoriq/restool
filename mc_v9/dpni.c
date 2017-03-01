/* Copyright 2013-2015 Freescale Semiconductor Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * * Neither the name of the above-listed copyright holders nor the
 * names of any contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
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
#include <errno.h>
#include "fsl_mc_sys.h"
#include "fsl_mc_cmd.h"
#include "fsl_dpni.h"
#include "fsl_dpni_cmd.h"

int dpni_open(struct fsl_mc_io *mc_io,
	      uint32_t cmd_flags,
	      int dpni_id,
	      uint16_t *token)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_OPEN,
					  cmd_flags,
					  0);
	DPNI_CMD_OPEN(cmd, dpni_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	*token = MC_CMD_HDR_READ_TOKEN(cmd.header);

	return 0;
}

int dpni_close(struct fsl_mc_io *mc_io,
	       uint32_t cmd_flags,
	       uint16_t token)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_CLOSE,
					  cmd_flags,
					  token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_prepare_extended_cfg(const struct dpni_extended_cfg	*cfg,
			      uint8_t			*ext_cfg_buf)
{
	uint64_t *ext_params = (uint64_t *)ext_cfg_buf;

	DPNI_PREP_EXTENDED_CFG(ext_params, cfg);

	return 0;
}

int dpni_extract_extended_cfg(struct dpni_extended_cfg	*cfg,
			      const uint8_t		*ext_cfg_buf)
{
	uint64_t *ext_params = (uint64_t *)ext_cfg_buf;

	DPNI_EXT_EXTENDED_CFG(ext_params, cfg);

	return 0;
}

int dpni_create_v9(struct fsl_mc_io *mc_io,
		   uint32_t cmd_flags,
		   const struct dpni_cfg_v9 *cfg,
		   const struct dpni_extended_cfg *ext_cfg,
		   uint16_t *token)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_CREATE,
					  cmd_flags,
					  0);
	DPNI_CMD_CREATE_V9(cmd, cfg);

	MC_CMD_OP(cmd, 3, 0, 8, uint8_t, ext_cfg->tc_cfg[0].max_dist);
	MC_CMD_OP(cmd, 3, 8, 8, uint8_t, ext_cfg->tc_cfg[1].max_dist);
	MC_CMD_OP(cmd, 3, 16, 8, uint8_t, ext_cfg->tc_cfg[2].max_dist);
	MC_CMD_OP(cmd, 3, 24, 8, uint8_t, ext_cfg->tc_cfg[3].max_dist);
	MC_CMD_OP(cmd, 3, 32, 8, uint8_t, ext_cfg->tc_cfg[4].max_dist);
	MC_CMD_OP(cmd, 3, 40, 8, uint8_t, ext_cfg->tc_cfg[5].max_dist);
	MC_CMD_OP(cmd, 3, 48, 8, uint8_t, ext_cfg->tc_cfg[6].max_dist);
	MC_CMD_OP(cmd, 3, 56, 8, uint8_t, ext_cfg->tc_cfg[7].max_dist);
	MC_CMD_OP(cmd, 4, 0, 16, uint16_t, ext_cfg->ipr_cfg.max_reass_frm_size);
	MC_CMD_OP(cmd, 4, 16, 16, uint16_t, ext_cfg->ipr_cfg.min_frag_size_ipv4);
	MC_CMD_OP(cmd, 4, 32, 16, uint16_t, ext_cfg->ipr_cfg.min_frag_size_ipv6);
	MC_CMD_OP(cmd, 5, 0, 16, uint16_t, ext_cfg->ipr_cfg.max_open_frames_ipv4);
	MC_CMD_OP(cmd, 5, 16, 16, uint16_t, ext_cfg->ipr_cfg.max_open_frames_ipv6);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	*token = MC_CMD_HDR_READ_TOKEN(cmd.header);

	return 0;
}

int dpni_destroy(struct fsl_mc_io *mc_io,
		 uint32_t cmd_flags,
		 uint16_t token)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_DESTROY,
					  cmd_flags,
					  token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_irq_status_v9(struct fsl_mc_io *mc_io,
			   uint32_t cmd_flags,
			   uint16_t token,
			   uint8_t irq_index,
			   uint32_t *status)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_IRQ_STATUS,
					  cmd_flags,
					  token);
	DPNI_CMD_GET_IRQ_STATUS(cmd, irq_index, *status);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPNI_RSP_GET_IRQ_STATUS(cmd, *status);

	return 0;
}

int dpni_get_attributes_v9(struct fsl_mc_io *mc_io,
			   uint32_t cmd_flags,
			   uint16_t token,
			   struct dpni_attr_v9 *attr,
			   struct dpni_extended_cfg *ext_cfg)
{
	struct mc_command cmd = { 0 };
	int err;
	int i;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_ATTR,
					  cmd_flags,
					  token);
	DPNI_CMD_GET_ATTR_V9(cmd, attr);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPNI_RSP_GET_ATTR_V9(cmd, attr);

	MC_RSP_OP(cmd, 3, 0,  8,  uint8_t, ext_cfg->tc_cfg[0].max_dist);
	MC_RSP_OP(cmd, 3, 8,  8,  uint8_t, ext_cfg->tc_cfg[1].max_dist);
	MC_RSP_OP(cmd, 3, 16, 8,  uint8_t, ext_cfg->tc_cfg[2].max_dist);
	MC_RSP_OP(cmd, 3, 24, 8,  uint8_t, ext_cfg->tc_cfg[3].max_dist);
	MC_RSP_OP(cmd, 3, 32, 8,  uint8_t, ext_cfg->tc_cfg[4].max_dist);
	MC_RSP_OP(cmd, 3, 40, 8,  uint8_t, ext_cfg->tc_cfg[5].max_dist);
	MC_RSP_OP(cmd, 3, 48, 8,  uint8_t, ext_cfg->tc_cfg[6].max_dist);
	MC_RSP_OP(cmd, 3, 56, 8,  uint8_t, ext_cfg->tc_cfg[7].max_dist);
	MC_RSP_OP(cmd, 4, 0,  16, uint16_t, ext_cfg->ipr_cfg.max_reass_frm_size);
	MC_RSP_OP(cmd, 4, 16, 16, uint16_t, ext_cfg->ipr_cfg.min_frag_size_ipv4);
	MC_RSP_OP(cmd, 4, 32, 16, uint16_t, ext_cfg->ipr_cfg.min_frag_size_ipv6);
	MC_RSP_OP(cmd, 5, 0,  16, uint16_t, ext_cfg->ipr_cfg.max_open_frames_ipv4);
	MC_RSP_OP(cmd, 5, 16, 16, uint16_t, ext_cfg->ipr_cfg.max_open_frames_ipv6);

	if (attr->options & DPNI_OPT_DIST_FS)
		for (i = 0; i < 7; i++)
			ext_cfg->tc_cfg[i].max_fs_entries = ext_cfg->tc_cfg[i].max_dist;

	return 0;
}

int dpni_get_primary_mac_addr(struct fsl_mc_io *mc_io,
			      uint32_t cmd_flags,
			      uint16_t token,
			      uint8_t mac_addr[6])
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_PRIM_MAC,
					  cmd_flags,
					  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPNI_RSP_GET_PRIMARY_MAC_ADDR(cmd, mac_addr);

	return 0;
}

int dpni_get_link_state(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			struct dpni_link_state *state)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_LINK_STATE,
					  cmd_flags,
					  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPNI_RSP_GET_LINK_STATE(cmd, state);

	return 0;
}

int dpni_get_irq_mask(struct fsl_mc_io *mc_io,
		      uint32_t cmd_flags,
		      uint16_t token,
		      uint8_t irq_index,
		      uint32_t *mask)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_IRQ_MASK,
					  cmd_flags,
					  token);
	DPNI_CMD_GET_IRQ_MASK(cmd, irq_index);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPNI_RSP_GET_IRQ_MASK(cmd, *mask);

	return 0;
}

int dpni_get_irq_status(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t irq_index,
			uint32_t *status)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_IRQ_STATUS,
					  cmd_flags,
					  token);
	DPNI_CMD_GET_IRQ_STATUS(cmd, irq_index, *status);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPNI_RSP_GET_IRQ_STATUS(cmd, *status);

	return 0;
}


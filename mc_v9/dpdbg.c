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
#include "fsl_dpdbg.h"
#include "fsl_dpdbg_cmd.h"

int dpdbg_open(struct fsl_mc_io *mc_io,
	       uint32_t cmd_flags,
	       int dpdbg_id,
	       uint16_t *token)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_OPEN,
					  cmd_flags,
					  0);
	DPDBG_CMD_OPEN(cmd, dpdbg_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	*token = MC_CMD_HDR_READ_TOKEN(cmd.header);

	return err;
}

int dpdbg_close(struct fsl_mc_io *mc_io,
		uint32_t cmd_flags,
		uint16_t token)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_CLOSE, cmd_flags,
					  token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpdbg_get_attributes(struct fsl_mc_io *mc_io,
			 uint32_t cmd_flags,
			 uint16_t token,
			 struct dpdbg_attr *attr)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_GET_ATTR,
					  cmd_flags,
					  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPDBG_RSP_GET_ATTRIBUTES(cmd, attr);

	return 0;
}

int dpdbg_get_dpni_info(struct fsl_mc_io		*mc_io,
			uint32_t			cmd_flags,
			uint16_t			token,
			int				dpni_id,
			struct dpdbg_dpni_info		*info)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_GET_DPNI_INFO,
					  cmd_flags,
					  token);
	DPDBG_CMD_GET_DPNI_INFO(cmd, dpni_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPDBG_RSP_GET_DPNI_INFO(cmd, info);

	return 0;
}

int dpdbg_get_dpni_priv_tx_conf_fqid(struct fsl_mc_io	*mc_io,
				     uint32_t		cmd_flags,
				     uint16_t		token,
				     int		dpni_id,
				     uint8_t		sender_id,
				     uint32_t		*fqid)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(
					DPDBG_CMDID_GET_DPNI_PRIV_TX_CONF_FQID,
					cmd_flags,
					token);
	DPDBG_CMD_GET_DPNI_PRIV_TX_CONF_FQID(cmd, dpni_id, sender_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPDBG_RSP_GET_DPNI_PRIV_TX_CONF_FQID(cmd, *fqid);

	return 0;
}

int dpdbg_get_dpcon_info(struct fsl_mc_io		*mc_io,
			 uint32_t			cmd_flags,
			 uint16_t			token,
			 int				dpcon_id,
			 struct dpdbg_dpcon_info	*info)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_GET_DPCON_INFO,
					  cmd_flags,
					  token);
	DPDBG_CMD_GET_DPCON_INFO(cmd, dpcon_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPDBG_RSP_GET_DPCON_INFO(cmd, info);

	return 0;
}

int dpdbg_get_dpbp_info(struct fsl_mc_io		*mc_io,
			uint32_t			cmd_flags,
			uint16_t			token,
			int				dpbp_id,
			struct dpdbg_dpbp_info		*info)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_GET_DPBP_INFO,
					  cmd_flags,
					  token);
	DPDBG_CMD_GET_DPBP_INFO(cmd, dpbp_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPDBG_RSP_GET_DPBP_INFO(cmd, info);

	return 0;
}

int dpdbg_get_dpci_fqid(struct fsl_mc_io	*mc_io,
			uint32_t		cmd_flags,
			uint16_t		token,
			int			dpci_id,
			uint8_t			priority,
			uint32_t		*fqid)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_GET_DPBP_INFO,
					  cmd_flags,
					  token);
	DPDBG_CMD_GET_DPCI_FQID(cmd, dpci_id, priority);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPDBG_RSP_GET_DPCI_FQID(cmd, *fqid);

	return 0;
}

int dpdbg_prepare_ctlu_global_rule(struct dpkg_profile_cfg	*dpkg_rule,
				   uint8_t			*rule_buf)
{
	int i, j;
	int offset = 0;
	int param = 1;
	uint64_t *params = (uint64_t *)rule_buf;

	if (!rule_buf || !dpkg_rule)
		return -EINVAL;

	params[0] |= mc_enc(0, 8, dpkg_rule->num_extracts);
	params[0] = __cpu_to_le64(params[0]);

	if (dpkg_rule->num_extracts >= DPKG_MAX_NUM_OF_EXTRACTS)
		return -EINVAL;

	for (i = 0; i < dpkg_rule->num_extracts; i++) {
		switch (dpkg_rule->extracts[i].type) {
		case DPKG_EXTRACT_FROM_HDR:
			params[param] |= mc_enc(0, 8,
				dpkg_rule->extracts[i].extract.from_hdr.prot);
			params[param] |= mc_enc(8, 4,
				dpkg_rule->extracts[i].extract.from_hdr.type);
			params[param] |= mc_enc(16, 8,
				dpkg_rule->extracts[i].extract.from_hdr.size);
			params[param] |= mc_enc(24, 8,
				dpkg_rule->extracts[i].extract.from_hdr.offset);
			params[param] |= mc_enc(32, 32,
				dpkg_rule->extracts[i].extract.from_hdr.field);
			params[param] = __cpu_to_le64(params[param]);
			param++;
			params[param] |= mc_enc(0, 8,
				dpkg_rule->extracts[i].extract.
				from_hdr.hdr_index);
			break;
		case DPKG_EXTRACT_FROM_DATA:
			params[param] |= mc_enc(16, 8,
				dpkg_rule->extracts[i].extract.from_data.size);
			params[param] |= mc_enc(24, 8,
				dpkg_rule->extracts[i].extract.
				from_data.offset);
			params[param] = __cpu_to_le64(params[param]);
			param++;
			break;
		case DPKG_EXTRACT_FROM_PARSE:
			params[param] |= mc_enc(16, 8,
				dpkg_rule->extracts[i].extract.from_parse.size);
			params[param] |= mc_enc(24, 8,
				dpkg_rule->extracts[i].extract.
				from_parse.offset);
			params[param] = __cpu_to_le64(params[param]);
			param++;
			break;
		default:
			return -EINVAL;
		}
		params[param] |= mc_enc(
			24, 8, dpkg_rule->extracts[i].num_of_byte_masks);
		params[param] |= mc_enc(32, 4, dpkg_rule->extracts[i].type);
		params[param] = __cpu_to_le64(params[param]);
		param++;
		for (offset = 0, j = 0;
			j < DPKG_NUM_OF_MASKS;
			offset += 16, j++) {
			params[param] |= mc_enc(
				(offset), 8,
				dpkg_rule->extracts[i].masks[j].mask);
			params[param] |= mc_enc(
				(offset + 8), 8,
				dpkg_rule->extracts[i].masks[j].offset);
		}
		params[param] = __cpu_to_le64(params[param]);
		param++;
	}
	return 0;
}

int dpdbg_set_ctlu_global_marking(struct fsl_mc_io		*mc_io,
				  uint32_t			cmd_flags,
				  uint16_t			token,
				  uint8_t			marking,
				  struct dpdbg_rule_cfg		*cfg)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_SET_CTLU_GLOBAL_MARKING,
					  cmd_flags,
					  token);
	DPDBG_CMD_SET_CTLU_GLOBAL_MARKING(cmd, marking, cfg);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpdbg_set_dpni_rx_marking(struct fsl_mc_io			*mc_io,
			      uint32_t				cmd_flags,
			      uint16_t				token,
			      int				dpni_id,
			      struct dpdbg_dpni_rx_marking_cfg	*cfg)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_SET_DPNI_RX_MARKING,
					  cmd_flags,
					  token);
	DPDBG_CMD_SET_DPNI_RX_MARKING(cmd, dpni_id, cfg);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpdbg_set_dpni_tx_conf_marking(struct fsl_mc_io		*mc_io,
				   uint32_t			cmd_flags,
				   uint16_t			token,
				   int				dpni_id,
				   uint16_t			sender_id,
				   uint8_t			marking)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_SET_DPNI_TX_CONF_MARKING,
					  cmd_flags,
					  token);
	DPDBG_CMD_SET_DPNI_TX_CONF_MARKING(cmd, dpni_id, sender_id, marking);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpdbg_set_dpio_marking(struct fsl_mc_io	*mc_io,
			   uint32_t		cmd_flags,
			   uint16_t		 token,
			   int			 dpio_id,
			   uint8_t		 marking)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_SET_DPIO_MARKING,
					  cmd_flags,
					  token);
	DPDBG_CMD_SET_DPIO_MARKING(cmd, dpio_id, marking);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpdbg_set_ctlu_global_trace(struct fsl_mc_io	*mc_io,
				uint32_t		cmd_flags,
				uint16_t		token,
				struct dpdbg_rule_cfg	*cfg)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_SET_CTLU_GLOBAL_TRACE,
					  cmd_flags,
					  token);
	DPDBG_CMD_SET_CTLU_GLOBAL_TRACE(cmd, cfg);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpdbg_set_dpio_trace(struct fsl_mc_io	*mc_io,
			 uint32_t		cmd_flags,
			 uint16_t		token,
			 int			dpio_id,
			 struct dpdbg_dpio_trace_cfg
				 trace_point[DPDBG_NUM_OF_DPIO_TRACE_POINTS])
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_SET_DPIO_TRACE,
					  cmd_flags,
					  token);
	DPDBG_CMD_SET_DPIO_TRACE(cmd, dpio_id, trace_point);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpdbg_set_dpni_rx_trace(struct fsl_mc_io		*mc_io,
			    uint32_t			cmd_flags,
			    uint16_t			token,
			    int			dpni_id,
			    struct dpdbg_dpni_rx_trace_cfg *trace_cfg)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_SET_DPNI_RX_TRACE,
					  cmd_flags,
					  token);
	DPDBG_CMD_SET_DPNI_RX_TRACE(cmd, dpni_id, trace_cfg);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpdbg_set_dpni_tx_trace(struct fsl_mc_io			*mc_io,
			    uint32_t				cmd_flags,
			    uint16_t				token,
			    int					dpni_id,
			    uint16_t				sender_id,
			    struct dpdbg_dpni_tx_trace_cfg	*trace_cfg)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_SET_DPNI_TX_TRACE,
					  cmd_flags,
					  token);
	DPDBG_CMD_SET_DPNI_TX_TRACE(cmd, dpni_id, sender_id, trace_cfg);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpdbg_set_dpcon_trace(struct fsl_mc_io		*mc_io,
			  uint32_t			cmd_flags,
			  uint16_t			token,
			  int				dpcon_id,
			  struct dpdbg_dpcon_trace_cfg
				  trace_point[DPDBG_NUM_OF_DPCON_TRACE_POINTS])
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_SET_DPCON_TRACE,
					  cmd_flags,
					  token);
	DPDBG_CMD_SET_DPCON_TRACE(cmd, dpcon_id, trace_point);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpdbg_set_dpseci_trace(struct fsl_mc_io		*mc_io,
			   uint32_t			cmd_flags,
			   uint16_t			token,
			   int				dpseci_id,
			   struct dpdbg_dpseci_trace_cfg
				  trace_point[DPDBG_NUM_OF_DPSECI_TRACE_POINTS])
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_SET_DPSECI_TRACE,
					  cmd_flags,
					  token);
	DPDBG_CMD_SET_DPSECI_TRACE(cmd, dpseci_id, trace_point);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpdbg_get_dpmac_counter(struct fsl_mc_io	*mc_io,
			    uint32_t		cmd_flags,
			    uint16_t		 token,
			    int		 dpmac_id,
			    enum dpmac_counter	 counter_type,
			    uint64_t		 *counter)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_GET_DPMAC_COUNTER,
					  cmd_flags,
					  token);
	DPDBG_CMD_GET_DPMAC_COUNTER(cmd, dpmac_id, counter_type);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPDBG_RSP_GET_DPMAC_COUNTER(cmd, *counter);

	return 0;
}

int dpdbg_get_dpni_counter(struct fsl_mc_io	*mc_io,
			   uint32_t		cmd_flags,
			   uint16_t		token,
			   int			dpni_id,
			   enum dpni_counter	counter_type,
			   uint64_t		*counter)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_GET_DPNI_COUNTER,
					  cmd_flags,
					  token);
	DPDBG_CMD_GET_DPMAC_COUNTER(cmd, dpni_id, counter_type);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPDBG_RSP_GET_DPNI_COUNTER(cmd, *counter);

	return 0;
}

/* Copyright 2013-2016 Freescale Semiconductor Inc.
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
#include "../mc_v8/fsl_mc_sys.h"
#include "fsl_mc_cmd.h"
#include "fsl_dpni.h"
#include "fsl_dpni_cmd.h"

int dpni_create_v10(struct fsl_mc_io	*mc_io,
		uint16_t	dprc_token,
		uint32_t	cmd_flags,
		const struct dpni_cfg_v10	*cfg,
		uint32_t	*obj_id)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_CREATE,
					  cmd_flags,
					  dprc_token);
	DPNI_CMD_CREATE(cmd, cfg);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	CMD_CREATE_RSP_GET_OBJ_ID_PARAM0(cmd, *obj_id);

	return 0;
}

int dpni_destroy_v10(struct fsl_mc_io	*mc_io,
		uint16_t	dprc_token,
		uint32_t	cmd_flags,
		uint32_t	object_id)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_DESTROY,
					  cmd_flags,
					  dprc_token);
	/* set object id to destroy */
	CMD_DESTROY_SET_OBJ_ID_PARAM0(cmd, object_id);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_attributes_v10(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			struct dpni_attr_v10 *attr)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_ATTR,
					  cmd_flags,
					  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPNI_RSP_GET_ATTR(cmd, attr);

	return 0;
}
int dpni_add_qos_entry_v10(struct fsl_mc_io *mc_io,
		       uint32_t cmd_flags,
		       uint16_t token,
		       const struct dpni_rule_cfg *cfg,
			   char has_index,
		       uint8_t tc_id,
			   uint16_t index)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_ADD_QOS_ENT,
					  cmd_flags,
					  token);
	DPNI_CMD_ADD_QOS_ENTRY(cmd, cfg, has_index, tc_id, index);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_add_fs_entry_v10(struct fsl_mc_io *mc_io,
		      uint32_t cmd_flags,
		      uint16_t token,
			  char has_index,
		      uint8_t tc_id,
			  uint16_t index,
		      const struct dpni_rule_cfg *cfg,
		      uint16_t flow_id)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_ADD_FS_ENT,
					  cmd_flags,
					  token);
	DPNI_CMD_ADD_FS_ENTRY(cmd, cfg, has_index, tc_id, index, flow_id);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_version_v10(struct fsl_mc_io *mc_io,
			   uint32_t cmd_flags,
			   uint16_t* majorVer,
			   uint16_t* minorVer)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_VERSION,
					cmd_flags,
					0);

	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	DPNI_RSP_GET_VERSION(cmd, *majorVer, *minorVer);

	return 0;
}

int dpni_set_queue_v10(struct fsl_mc_io *mc_io,
		     uint32_t cmd_flags,
		     uint16_t token,
			 enum dpni_queue_type_v10 type,
			 uint8_t tc,
			 uint8_t index,
		     const struct dpni_queue_v10 *queue)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_QUEUE,
					  cmd_flags,
					  token);
	DPNI_CMD_SET_QUEUE(cmd, type, tc, index, queue);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_queue_v10(struct fsl_mc_io *mc_io,
		     uint32_t cmd_flags,
		     uint16_t token,
			 enum dpni_queue_type_v10 type,
			 uint8_t tc,
			 uint8_t index,
			 struct dpni_queue_v10 *queue)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_QUEUE,
					  cmd_flags,
					  token);
	DPNI_CMD_GET_QUEUE(cmd, type, tc, index);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPNI_RSP_GET_QUEUE(cmd, queue);

	return 0;
}

int dpni_get_statistics_v10(struct fsl_mc_io *mc_io,
		     uint32_t cmd_flags,
		     uint16_t token,
			 uint8_t page,
			 union dpni_statistics_v10 *stat)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_STATISTICS,
					  cmd_flags,
					  token);
	DPNI_CMD_GET_STATISTICS(cmd, page);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPNI_RSP_GET_STATISTICS(cmd, stat);

	return 0;
}


int dpni_reset_statistics_v10(struct fsl_mc_io *mc_io,
		     uint32_t cmd_flags,
		     uint16_t token)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_RESET_STATISTICS,
					  cmd_flags,
					  token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_set_taildrop_v10(struct fsl_mc_io *mc_io,
		     uint32_t cmd_flags,
		     uint16_t token,
			 enum dpni_congestion_point cg_point,
			 enum dpni_queue_type_v10 q_type,
			 uint8_t tc,
			 uint8_t q_index,
			 struct dpni_taildrop_v10 *taildrop)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_TAILDROP,
					  cmd_flags,
					  token);
	DPNI_CMD_SET_TAILDROP(cmd, cg_point, q_type, tc, q_index, taildrop);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_taildrop_v10(struct fsl_mc_io *mc_io,
		     uint32_t cmd_flags,
		     uint16_t token,
			 enum dpni_congestion_point cg_point,
			 enum dpni_queue_type_v10 q_type,
			 uint8_t tc,
			 uint8_t q_index,
			 struct dpni_taildrop_v10 *taildrop)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_TAILDROP,
					  cmd_flags,
					  token);
	DPNI_CMD_GET_TAILDROP(cmd, cg_point, q_type, tc, q_index);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPNI_RSP_GET_TAILDROP(cmd, taildrop);

	return 0;
}

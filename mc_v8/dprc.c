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
#include "fsl_mc_sys.h"
#include "fsl_mc_cmd.h"
#include "fsl_dprc.h"
#include "fsl_dprc_cmd.h"

int dprc_get_container_id(struct fsl_mc_io *mc_io,
			  uint32_t cmd_flags,
			  int *container_id)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_CONT_ID,
					  cmd_flags,
					  0);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPRC_RSP_GET_CONTAINER_ID(cmd, *container_id);

	return 0;
}

int dprc_open(struct fsl_mc_io *mc_io,
	      uint32_t cmd_flags,
	      int container_id,
	      uint16_t *token)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_OPEN, cmd_flags,
					  0);
	DPRC_CMD_OPEN(cmd, container_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	*token = MC_CMD_HDR_READ_TOKEN(cmd.header);

	return 0;
}

int dprc_close(struct fsl_mc_io *mc_io,
	       uint32_t cmd_flags,
	       uint16_t token)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_CLOSE, cmd_flags,
					  token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dprc_create_container(struct fsl_mc_io *mc_io,
			  uint32_t cmd_flags,
			  uint16_t token,
			  struct dprc_cfg *cfg,
			  int *child_container_id,
			  uint64_t *child_portal_paddr)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	DPRC_CMD_CREATE_CONTAINER(cmd, cfg);

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_CREATE_CONT,
					  cmd_flags,
					  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPRC_RSP_CREATE_CONTAINER(cmd, *child_container_id,
				  *child_portal_paddr);

	return 0;
}

int dprc_destroy_container(struct fsl_mc_io *mc_io,
			   uint32_t cmd_flags,
			   uint16_t token,
			   int child_container_id)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_DESTROY_CONT,
					  cmd_flags,
					  token);
	DPRC_CMD_DESTROY_CONTAINER(cmd, child_container_id);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dprc_reset_container(struct fsl_mc_io *mc_io,
			 uint32_t cmd_flags,
			 uint16_t token,
			 int child_container_id)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_RESET_CONT,
					  cmd_flags,
					  token);
	DPRC_CMD_RESET_CONTAINER(cmd, child_container_id);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dprc_get_irq(struct fsl_mc_io *mc_io,
		 uint32_t cmd_flags,
		 uint16_t token,
		 uint8_t irq_index,
		 int *type,
		 struct dprc_irq_cfg *irq_cfg)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_IRQ,
					  cmd_flags,
					  token);
	DPRC_CMD_GET_IRQ(cmd, irq_index);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPRC_RSP_GET_IRQ(cmd, *type, irq_cfg);

	return 0;
}

int dprc_set_irq(struct fsl_mc_io *mc_io,
		 uint32_t cmd_flags,
		 uint16_t token,
		 uint8_t irq_index,
		 struct dprc_irq_cfg *irq_cfg)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_SET_IRQ,
					  cmd_flags,
					  token);
	DPRC_CMD_SET_IRQ(cmd, irq_index, irq_cfg);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dprc_get_irq_enable(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t irq_index,
			uint8_t *en)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_IRQ_ENABLE,
					  cmd_flags,
					  token);
	DPRC_CMD_GET_IRQ_ENABLE(cmd, irq_index);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPRC_RSP_GET_IRQ_ENABLE(cmd, *en);

	return 0;
}

int dprc_set_irq_enable(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t irq_index,
			uint8_t en)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_SET_IRQ_ENABLE,
					  cmd_flags,
					  token);
	DPRC_CMD_SET_IRQ_ENABLE(cmd, irq_index, en);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dprc_get_irq_mask(struct fsl_mc_io *mc_io,
		      uint32_t cmd_flags,
		      uint16_t token,
		      uint8_t irq_index,
		      uint32_t *mask)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_IRQ_MASK,
					  cmd_flags,
					  token);
	DPRC_CMD_GET_IRQ_MASK(cmd, irq_index);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPRC_RSP_GET_IRQ_MASK(cmd, *mask);

	return 0;
}

int dprc_set_irq_mask(struct fsl_mc_io *mc_io,
		      uint32_t cmd_flags,
		      uint16_t token,
		      uint8_t irq_index,
		      uint32_t mask)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_SET_IRQ_MASK,
					  cmd_flags,
					  token);
	DPRC_CMD_SET_IRQ_MASK(cmd, irq_index, mask);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dprc_get_irq_status(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t irq_index,
			uint32_t *status)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_IRQ_STATUS,
					  cmd_flags,
					  token);
	DPRC_CMD_GET_IRQ_STATUS(cmd, irq_index);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPRC_RSP_GET_IRQ_STATUS(cmd, *status);

	return 0;
}

int dprc_clear_irq_status(struct fsl_mc_io *mc_io,
			  uint32_t cmd_flags,
			  uint16_t token,
			  uint8_t irq_index,
			  uint32_t status)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_CLEAR_IRQ_STATUS,
					  cmd_flags,
					  token);
	DPRC_CMD_CLEAR_IRQ_STATUS(cmd, irq_index, status);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dprc_get_attributes(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			struct dprc_attributes *attr)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_ATTR,
					  cmd_flags,
					  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPRC_RSP_GET_ATTRIBUTES(cmd, attr);

	return 0;
}

int dprc_set_res_quota(struct fsl_mc_io *mc_io,
		       uint32_t cmd_flags,
		       uint16_t token,
		       int child_container_id,
		       char *type,
		       uint16_t quota)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_SET_RES_QUOTA,
					  cmd_flags,
					  token);
	DPRC_CMD_SET_RES_QUOTA(cmd, child_container_id, type, quota);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dprc_get_res_quota(struct fsl_mc_io *mc_io,
		       uint32_t cmd_flags,
		       uint16_t token,
		       int child_container_id,
		       char *type,
		       uint16_t *quota)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_RES_QUOTA,
					  cmd_flags,
					  token);
	DPRC_CMD_GET_RES_QUOTA(cmd, child_container_id, type);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPRC_RSP_GET_RES_QUOTA(cmd, *quota);

	return 0;
}

int dprc_assign(struct fsl_mc_io *mc_io,
		uint32_t cmd_flags,
		uint16_t token,
		int container_id,
		struct dprc_res_req *res_req)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_ASSIGN,
					  cmd_flags,
					  token);
	DPRC_CMD_ASSIGN(cmd, container_id, res_req);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dprc_unassign(struct fsl_mc_io *mc_io,
		  uint32_t cmd_flags,
		  uint16_t token,
		  int child_container_id,
		  struct dprc_res_req *res_req)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_UNASSIGN,
					  cmd_flags,
					  token);
	DPRC_CMD_UNASSIGN(cmd, child_container_id, res_req);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dprc_get_pool_count(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			int *pool_count)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_POOL_COUNT,
					  cmd_flags,
					  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPRC_RSP_GET_POOL_COUNT(cmd, *pool_count);

	return 0;
}

int dprc_get_pool(struct fsl_mc_io *mc_io,
		  uint32_t cmd_flags,
		  uint16_t token,
		  int pool_index,
		  char *type)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_POOL,
					  cmd_flags,
					  token);
	DPRC_CMD_GET_POOL(cmd, pool_index);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPRC_RSP_GET_POOL(cmd, type);

	return 0;
}

int dprc_get_obj_count(struct fsl_mc_io *mc_io,
		       uint32_t cmd_flags,
		       uint16_t token,
		       int *obj_count)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_OBJ_COUNT,
					  cmd_flags,
					  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPRC_RSP_GET_OBJ_COUNT(cmd, *obj_count);

	return 0;
}

int dprc_get_obj(struct fsl_mc_io *mc_io,
		 uint32_t cmd_flags,
		 uint16_t token,
		 int obj_index,
		 struct dprc_obj_desc *obj_desc)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_OBJ,
					  cmd_flags,
					  token);
	DPRC_CMD_GET_OBJ(cmd, obj_index);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPRC_RSP_GET_OBJ(cmd, obj_desc);

	return 0;
}

int dprc_get_obj_desc(struct fsl_mc_io *mc_io,
		      uint32_t cmd_flags,
		      uint16_t token,
		      char *obj_type,
		      int obj_id,
		      struct dprc_obj_desc *obj_desc)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_OBJ_DESC,
					  cmd_flags,
					  token);
	DPRC_CMD_GET_OBJ_DESC(cmd, obj_type, obj_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPRC_RSP_GET_OBJ_DESC(cmd, obj_desc);

	return 0;
}

int dprc_set_obj_irq(struct fsl_mc_io *mc_io,
		     uint32_t cmd_flags,
		     uint16_t token,
		     char *obj_type,
		     int obj_id,
		     uint8_t irq_index,
		     struct dprc_irq_cfg *irq_cfg)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_SET_OBJ_IRQ,
					  cmd_flags,
					  token);
	DPRC_CMD_SET_OBJ_IRQ(cmd,
			     obj_type,
			     obj_id,
			     irq_index,
			     irq_cfg);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dprc_get_obj_irq(struct fsl_mc_io	*mc_io,
		     uint32_t		cmd_flags,
		     uint16_t		token,
		     char		*obj_type,
		     int		obj_id,
		     uint8_t		irq_index,
		     int		*type,
		     struct dprc_irq_cfg *irq_cfg)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_OBJ_IRQ,
					  cmd_flags,
					  token);
	DPRC_CMD_GET_OBJ_IRQ(cmd, obj_type, obj_id, irq_index);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPRC_RSP_GET_OBJ_IRQ(cmd, *type, irq_cfg);

	return 0;
}

int dprc_get_res_count(struct fsl_mc_io *mc_io,
		       uint32_t cmd_flags,
		       uint16_t token,
		       char *type,
		       int *res_count)
{
	struct mc_command cmd = { 0 };
	int err;

	*res_count = 0;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_RES_COUNT,
					  cmd_flags,
					  token);
	DPRC_CMD_GET_RES_COUNT(cmd, type);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPRC_RSP_GET_RES_COUNT(cmd, *res_count);

	return 0;
}

int dprc_get_res_ids(struct fsl_mc_io *mc_io,
		     uint32_t cmd_flags,
		     uint16_t token,
		     char *type,
		     struct dprc_res_ids_range_desc *range_desc)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_RES_IDS,
					  cmd_flags,
					  token);
	DPRC_CMD_GET_RES_IDS(cmd, range_desc, type);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPRC_RSP_GET_RES_IDS(cmd, range_desc);

	return 0;
}

int dprc_get_obj_region(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			char *obj_type,
			int obj_id,
			uint8_t region_index,
			struct dprc_region_desc *region_desc)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_OBJ_REG,
					  cmd_flags,
					  token);
	DPRC_CMD_GET_OBJ_REGION(cmd, obj_type, obj_id, region_index);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPRC_RSP_GET_OBJ_REGION(cmd, region_desc);

	return 0;
}

int dprc_set_obj_label(struct fsl_mc_io *mc_io,
		       uint32_t cmd_flags,
		       uint16_t  token,
		       char *obj_type,
		       int  obj_id,
		       char *label)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_SET_OBJ_LABEL,
					  cmd_flags,
					  token);
	DPRC_CMD_SET_OBJ_LABEL(cmd, obj_type, obj_id, label);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dprc_connect(struct fsl_mc_io *mc_io,
		 uint32_t cmd_flags,
		 uint16_t token,
		 const struct dprc_endpoint *endpoint1,
		 const struct dprc_endpoint *endpoint2,
		 const struct dprc_connection_cfg *cfg)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_CONNECT,
					  cmd_flags,
					  token);
	DPRC_CMD_CONNECT(cmd, endpoint1, endpoint2, cfg);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dprc_disconnect(struct fsl_mc_io *mc_io,
		    uint32_t cmd_flags,
		    uint16_t token,
		    const struct dprc_endpoint *endpoint)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_DISCONNECT,
					  cmd_flags,
					  token);
	DPRC_CMD_DISCONNECT(cmd, endpoint);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dprc_get_connection(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			const struct dprc_endpoint *endpoint1,
			struct dprc_endpoint *endpoint2,
			int *state)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_CONNECTION,
					  cmd_flags,
					  token);
	DPRC_CMD_GET_CONNECTION(cmd, endpoint1);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPRC_RSP_GET_CONNECTION(cmd, endpoint2, *state);

	return 0;
}

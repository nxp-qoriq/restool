/*
 * Copyright 2013-2016 Freescale Semiconductor Inc.
 * Copyright 2017-2018 NXP
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
#include "fsl_mc_sys.h"
#include "fsl_mc_cmd.h"
#include "fsl_dprc.h"
#include "fsl_dprc_cmd.h"


/**
 * dprc_open() - Open DPRC object for use
 * @mc_io:		Pointer to MC portal's I/O object
 * @cmd_flags:		Command flags; one or more of 'MC_CMD_FLAG_'
 * @container_id:	Container ID to open
 * @token:		Returned token of DPRC object
 *
 * Return:	'0' on Success; Error code otherwise.
 *
 * @warning	Required before any operation on the object.
 */
int dprc_open(struct fsl_mc_io *mc_io,
	      uint32_t cmd_flags,
	      int container_id,
	      uint16_t *token)
{
	struct mc_command cmd = { 0 };
	struct dprc_cmd_open *cmd_params;
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_OPEN, cmd_flags,
					  0);
	cmd_params = (struct dprc_cmd_open *)cmd.params;
	cmd_params->container_id = cpu_to_le32(container_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	*token = mc_cmd_hdr_read_token(&cmd);

	return 0;
}

/**
 * dprc_close() - Close the control session of the object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPRC object
 *
 * After this function is called, no further operations are
 * allowed on the object without opening a new control session.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
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

/**
 * dprc_create_container() - Create child container
 * @mc_io:		Pointer to MC portal's I/O object
 * @cmd_flags:		Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:		Token of DPRC object
 * @cfg:		Child container configuration
 * @child_container_id:	Returned child container ID
 * @child_portal_offset:Returned child portal offset from MC portal base
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dprc_create_container(struct fsl_mc_io *mc_io,
			  uint32_t cmd_flags,
			  uint16_t token,
			  struct dprc_cfg *cfg,
			  int *child_container_id,
			  uint64_t *child_portal_offset)
{
	struct mc_command cmd = { 0 };
	struct dprc_cmd_create_container *cmd_params;
	struct dprc_rsp_create_container *rsp_params;
	int err, i;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_CREATE_CONT,
					  cmd_flags, token);
	cmd_params = (struct dprc_cmd_create_container *)cmd.params;
	cmd_params->options = cpu_to_le32(cfg->options);
	cmd_params->icid = cpu_to_le32(cfg->icid);
	cmd_params->portal_id = cpu_to_le32(cfg->portal_id);
	for (i = 0; i < 16; i++)
		cmd_params->label[i] = cfg->label[i];

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dprc_rsp_create_container *)cmd.params;
	*child_container_id = le32_to_cpu(rsp_params->child_container_id);
	*child_portal_offset = le64_to_cpu(rsp_params->child_portal_addr);

	return 0;
}

/**
 * dprc_destroy_container() - Destroy child container.
 * @mc_io:		Pointer to MC portal's I/O object
 * @cmd_flags:		Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:		Token of DPRC object
 * @child_container_id:	ID of the container to destroy
 *
 * This function terminates the child container, so following this call the
 * child container ID becomes invalid.
 *
 * Notes:
 * - All resources and objects of the destroyed container are returned to the
 * parent container or destroyed if were created be the destroyed container.
 * - This function destroy all the child containers of the specified
 *   container prior to destroying the container itself.
 *
 * warning: Only the parent container is allowed to destroy a child policy
 *		Container 0 can't be destroyed
 *
 * Return:	'0' on Success; Error code otherwise.
 *
 */
int dprc_destroy_container(struct fsl_mc_io *mc_io,
			   uint32_t cmd_flags,
			   uint16_t token,
			   int child_container_id)
{
	struct mc_command cmd = { 0 };
	struct dprc_cmd_destroy_container *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_DESTROY_CONT,
					  cmd_flags, token);
	cmd_params = (struct dprc_cmd_destroy_container *)cmd.params;
	cmd_params->child_container_id = cpu_to_le32(child_container_id);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dprc_get_attributes() - Obtains container attributes
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPRC object
 * @attributes	Returned container attributes
 *
 * Return:     '0' on Success; Error code otherwise.
 */
int dprc_get_attributes(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			struct dprc_attributes *attr)
{
	struct mc_command cmd = { 0 };
	struct dprc_rsp_get_attributes *rsp_params;
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
	rsp_params = (struct dprc_rsp_get_attributes *)cmd.params;
	attr->container_id = le32_to_cpu(rsp_params->container_id);
	attr->icid = le32_to_cpu(rsp_params->icid);
	attr->options = le32_to_cpu(rsp_params->options);
	attr->portal_id = le32_to_cpu(rsp_params->portal_id);

	return 0;
}

/**
 * dprc_set_locked() 	- Set locked the child container
 * @mc_io:		- Pointer to MC portal's I/O object
 * @cmd_flags:		- Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:		- Token of DPRC object
 * @locked:		- locked bit
 * 				1 = locked
 * 				0 = unlocked
 * @child_container_id:	- ID of the container to reset
 *
 * Locking is done by a parent (this DPRC) to one of its child
 * containers.
 *
 * This command will lock the child_container_id and the entire
 * hierarchy that belongs to it down.
 * That means the containers will not be longer allowed to:
 * - create/destroy objects
 * - create/destroy child containers
 * - assign/unassign objects to/from its child containers
 * - lock/unlock a child container
 *
 * The use case is for DPAA2 direct assignment. It will permit the host
 * to create objects in its container and after that to move them in the guest container.
 * The guest container will be locked by its parent and will not be able to mount a denial
 * of service attack by creating multiple objects.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dprc_set_locked(struct fsl_mc_io *mc_io,
			 uint32_t cmd_flags,
			 uint16_t token,
			 uint8_t locked,
			 int child_container_id)
{
	struct mc_command cmd = { 0 };
	struct dprc_cmd_set_locked *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_SET_LOCKED,
					  cmd_flags, token);
	cmd_params = (struct dprc_cmd_set_locked *)cmd.params;
	cmd_params->child_container_id = cpu_to_le32(child_container_id);
	cmd_params->locked = locked;

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dprc_assign() - Assigns objects or resource to a child container.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPRC object
 * @container_id: ID of the child container
 * @res_req:	Describes the type and amount of resources to
 *			assign to the given container
 *
 * Assignment is usually done by a parent (this DPRC) to one of its child
 * containers.
 *
 * According to the DPRC allocation policy, the assigned resources may be taken
 * (allocated) from the container's ancestors, if not enough resources are
 * available in the container itself.
 *
 * The type of assignment depends on the dprc_res_req options, as follows:
 * - DPRC_RES_REQ_OPT_EXPLICIT: indicates that assigned resources should have
 *   the explicit base ID specified at the id_base_align field of res_req.
 * - DPRC_RES_REQ_OPT_ALIGNED: indicates that the assigned resources should be
 *   aligned to the value given at id_base_align field of res_req.
 * - DPRC_RES_REQ_OPT_PLUGGED: Relevant only for object assignment,
 *   and indicates that the object must be set to the plugged state.
 *
 * A container may use this function with its own ID in order to change a
 * object state to plugged or unplugged.
 *
 * If IRQ information has been set in the child DPRC, it will signal an
 * interrupt following every change in its object assignment.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dprc_assign(struct fsl_mc_io *mc_io,
		uint32_t cmd_flags,
		uint16_t token,
		int container_id,
		struct dprc_res_req *res_req)
{
	struct mc_command cmd = { 0 };
	struct dprc_cmd_assign *cmd_params;
	int i;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_ASSIGN,
					  cmd_flags, token);
	cmd_params = (struct dprc_cmd_assign *)cmd.params;
	cmd_params->container_id = cpu_to_le32(container_id);
	cmd_params->options = cpu_to_le32(res_req->options);
	cmd_params->num = cpu_to_le32(res_req->num);
	cmd_params->id_base_align = cpu_to_le32(res_req->id_base_align);
	for (i = 0; i < 16; i++)
		cmd_params->type[i] = res_req->type[i];

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dprc_unassign() - Un-assigns objects or resources from a child container
 *		and moves them into this (parent) DPRC.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPRC object
 * @child_container_id:	ID of the child container
 * @res_req:	Describes the type and amount of resources to un-assign from
 *		the child container
 *
 * Un-assignment of objects can succeed only if the object is not in the
 * plugged or opened state.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dprc_unassign(struct fsl_mc_io *mc_io,
		  uint32_t cmd_flags,
		  uint16_t token,
		  int child_container_id,
		  struct dprc_res_req *res_req)
{
	struct mc_command cmd = { 0 };
	struct dprc_cmd_unassign *cmd_params;
	int i;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_UNASSIGN,
					  cmd_flags,
					  token);
	cmd_params = (struct dprc_cmd_unassign *)cmd.params;
	cmd_params->child_container_id = cpu_to_le32(child_container_id);
	cmd_params->options = cpu_to_le32(res_req->options);
	cmd_params->num = cpu_to_le32(res_req->num);
	cmd_params->id_base_align = cpu_to_le32(res_req->id_base_align);
	for (i = 0; i < 16; i++)
		cmd_params->type[i] = res_req->type[i];

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dprc_get_pool_count() - Get the number of dprc's pools
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @mc_io:	Pointer to MC portal's I/O object
 * @token:	Token of DPRC object
 * @pool_count:	Returned number of resource pools in the dprc
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dprc_get_pool_count(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			int *pool_count)
{
	struct mc_command cmd = { 0 };
	struct dprc_rsp_get_pool_count *rsp_params;
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_POOL_COUNT,
					  cmd_flags, token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dprc_rsp_get_pool_count *)cmd.params;
	*pool_count = le32_to_cpu(rsp_params->pool_count);

	return 0;
}

/**
 * dprc_get_pool() - Get the type (string) of a certain dprc's pool
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPRC object
 * @pool_index;	Index of the pool to be queried (< pool_count)
 * @type:	The type of the pool
 *
 * The pool types retrieved one by one by incrementing
 * pool_index up to (not including) the value of pool_count returned
 * from dprc_get_pool_count(). dprc_get_pool_count() must
 * be called prior to dprc_get_pool().
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dprc_get_pool(struct fsl_mc_io *mc_io,
		  uint32_t cmd_flags,
		  uint16_t token,
		  int pool_index,
		  char *type)
{
	struct mc_command cmd = { 0 };
	struct dprc_cmd_get_pool *cmd_params;
	struct dprc_rsp_get_pool *rsp_params;
	int err, i;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_POOL,
					  cmd_flags,
					  token);
	cmd_params = (struct dprc_cmd_get_pool *)cmd.params;
	cmd_params->pool_index = cpu_to_le32(pool_index);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dprc_rsp_get_pool *)cmd.params;
	for (i = 0; i < 16; i++)
		type[i] = rsp_params->type[i];

	return 0;
}

/**
 * dprc_get_obj_count() - Obtains the number of objects in the DPRC
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPRC object
 * @obj_count:	Number of objects assigned to the DPRC
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dprc_get_obj_count(struct fsl_mc_io *mc_io,
		       uint32_t cmd_flags,
		       uint16_t token,
		       int *obj_count)
{
	struct mc_command cmd = { 0 };
	struct dprc_rsp_get_obj_count *rsp_params;
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_OBJ_COUNT,
					  cmd_flags, token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dprc_rsp_get_obj_count *)cmd.params;
	*obj_count = le32_to_cpu(rsp_params->obj_count);

	return 0;
}

/**
 * dprc_get_obj() - Get general information on an object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPRC object
 * @obj_index:	Index of the object to be queried (< obj_count)
 * @obj_desc:	Returns the requested object descriptor
 *
 * The object descriptors are retrieved one by one by incrementing
 * obj_index up to (not including) the value of obj_count returned
 * from dprc_get_obj_count(). dprc_get_obj_count() must
 * be called prior to dprc_get_obj().
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dprc_get_obj(struct fsl_mc_io *mc_io,
		 uint32_t cmd_flags,
		 uint16_t token,
		 int obj_index,
		 struct dprc_obj_desc *obj_desc)
{
	struct mc_command cmd = { 0 };
	struct dprc_cmd_get_obj *cmd_params;
	struct dprc_rsp_get_obj *rsp_params;
	int err, i;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_OBJ,
					  cmd_flags,
					  token);
	cmd_params = (struct dprc_cmd_get_obj *)cmd.params;
	cmd_params->obj_index = cpu_to_le32(obj_index);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dprc_rsp_get_obj *)cmd.params;
	obj_desc->id = le32_to_cpu(rsp_params->id);
	obj_desc->vendor = le16_to_cpu(rsp_params->vendor);
	obj_desc->irq_count = rsp_params->irq_count;
	obj_desc->region_count = rsp_params->region_count;
	obj_desc->state = le32_to_cpu(rsp_params->state);
	obj_desc->ver_major = le16_to_cpu(rsp_params->version_major);
	obj_desc->ver_minor = le16_to_cpu(rsp_params->version_minor);
	obj_desc->flags = le16_to_cpu(rsp_params->flags);
	for (i = 0; i < 16; i++) {
		obj_desc->type[i] = rsp_params->type[i];
		obj_desc->label[i] = rsp_params->label[i];
	}

	return 0;
}

/**
 * dprc_get_res_count() - Obtains the number of free resources that are assigned
 *		to this container, by pool type
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPRC object
 * @type:	pool type
 * @res_count:	Returned number of free resources of the given
 *			resource type that are assigned to this DPRC
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dprc_get_res_count(struct fsl_mc_io *mc_io,
		       uint32_t cmd_flags,
		       uint16_t token,
		       char *type,
		       int *res_count)
{
	struct mc_command cmd = { 0 };
	struct dprc_cmd_get_res_count *cmd_params;
	struct dprc_rsp_get_res_count *rsp_params;
	int err, i;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_RES_COUNT,
					  cmd_flags, token);
	cmd_params = (struct dprc_cmd_get_res_count *)cmd.params;
	for (i = 0; i < 16; i++)
		cmd_params->type[i] = type[i];

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dprc_rsp_get_res_count *)cmd.params;
	*res_count = le32_to_cpu(rsp_params->res_count);

	return 0;
}

/**
 * dprc_get_res_ids() - Obtains IDs of free resources in the container
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPRC object
 * @type:	pool type
 * @range_desc:	range descriptor
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dprc_get_res_ids(struct fsl_mc_io *mc_io,
		     uint32_t cmd_flags,
		     uint16_t token,
		     char *type,
		     struct dprc_res_ids_range_desc *range_desc)
{
	struct mc_command cmd = { 0 };
	struct dprc_cmd_get_res_ids *cmd_params;
	struct dprc_rsp_get_res_ids *rsp_params;
	uint8_t iter_lo, iter_hi;
	int err, i;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_RES_IDS,
					  cmd_flags, token);
	cmd_params = (struct dprc_cmd_get_res_ids *)cmd.params;
	cmd_params->base_id = cpu_to_le32(range_desc->base_id);
	cmd_params->last_id = cpu_to_le32(range_desc->last_id);
	dprc_set_field(cmd_params->iter_status_lo,
		       ITER_STATUS_LO,
		       range_desc->iter_status & 0x3F);
	dprc_set_field(cmd_params->iter_status_hi,
		       ITER_STATUS_HI,
		       range_desc->iter_status & 0x40);
	for (i = 0; i < 16; i++)
		cmd_params->type[i] = type[i];

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dprc_rsp_get_res_ids *)cmd.params;
	iter_lo = dprc_get_field(rsp_params->iter_status_lo, ITER_STATUS_LO);
	iter_hi = dprc_get_field(rsp_params->iter_status_hi, ITER_STATUS_HI);
	range_desc->iter_status = (iter_hi << DPRC_ITER_STATUS_LO_SIZE) | iter_lo;
	range_desc->base_id = le32_to_cpu(rsp_params->base_id);
	range_desc->last_id = le32_to_cpu(rsp_params->last_id);

	return 0;
}

/**
 * dprc_set_obj_label() - Set object label.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPRC object
 * @obj_type:	Object's type
 * @obj_id:	Object's ID
 * @label:	The required label. The maximum length is 16 chars.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dprc_set_obj_label(struct fsl_mc_io *mc_io,
		       uint32_t cmd_flags,
		       uint16_t  token,
		       char *obj_type,
		       int  obj_id,
		       char *label)
{
	struct mc_command cmd = { 0 };
	struct dprc_cmd_set_obj_label *cmd_params;
	int i;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_SET_OBJ_LABEL,
					  cmd_flags,
					  token);
	cmd_params = (struct dprc_cmd_set_obj_label *)cmd.params;
	cmd_params->obj_id = cpu_to_le32(obj_id);
	for (i = 0; i < 16; i++) {
		cmd_params->label[i] = label[i];
		cmd_params->obj_type[i] = obj_type[i];
	}

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dprc_get_irq_mask() - Get interrupt mask.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPRC object
 * @irq_index:	The interrupt index to configure
 * @mask:	Returned event mask to trigger interrupt
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dprc_get_irq_mask(struct fsl_mc_io *mc_io,
		      uint32_t cmd_flags,
		      uint16_t token,
		      uint8_t irq_index,
		      uint32_t *mask)
{
	struct mc_command cmd = { 0 };
	struct dprc_cmd_get_irq_mask *cmd_params;
	struct dprc_rsp_get_irq_mask *rsp_params;
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_IRQ_MASK,
					  cmd_flags, token);
	cmd_params = (struct dprc_cmd_get_irq_mask *)cmd.params;
	cmd_params->irq_index = irq_index;

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dprc_rsp_get_irq_mask *)cmd.params;
	*mask = le32_to_cpu(rsp_params->mask);

	return 0;
}
/**
 * dprc_get_irq_status() - Get the current status of any pending interrupts.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPRC object
 * @irq_index:	The interrupt index to configure
 * @status:	Returned interrupts status - one bit per cause:
 *			0 = no interrupt pending
 *			1 = interrupt pending
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dprc_get_irq_status(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t irq_index,
			uint32_t *status)
{
	struct mc_command cmd = { 0 };
	struct dprc_cmd_get_irq_status *cmd_params;
	struct dprc_rsp_get_irq_status *rsp_params;
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_IRQ_STATUS,
					  cmd_flags, token);
	cmd_params = (struct dprc_cmd_get_irq_status *)cmd.params;
	cmd_params->status = cpu_to_le32(*status);
	cmd_params->irq_index = irq_index;

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dprc_rsp_get_irq_status *)cmd.params;
	*status = le32_to_cpu(rsp_params->status);

	return 0;
}



/**
 * dprc_connect() - Connect two endpoints to create a network link between them
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPRC object
 * @endpoint1:	Endpoint 1 configuration parameters
 * @endpoint2:	Endpoint 2 configuration parameters
 * @cfg:	Connection configuration. The connection configuration
 *		is ignored for connections made to DPMAC objects, where
 *		rate is retrieved from the MAC configuration.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dprc_connect(struct fsl_mc_io *mc_io,
		 uint32_t cmd_flags,
		 uint16_t token,
		 const struct dprc_endpoint *endpoint1,
		 const struct dprc_endpoint *endpoint2,
		 const struct dprc_connection_cfg *cfg)
{
	struct mc_command cmd = { 0 };
	struct dprc_cmd_connect *cmd_params;
	int i;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_CONNECT,
					  cmd_flags,
					  token);
	cmd_params = (struct dprc_cmd_connect *)cmd.params;
	cmd_params->ep1_id = cpu_to_le32(endpoint1->id);
	cmd_params->ep1_interface_id = cpu_to_le16(endpoint1->if_id);
	cmd_params->ep2_id = cpu_to_le32(endpoint2->id);
	cmd_params->ep2_interface_id = cpu_to_le16(endpoint2->if_id);
	cmd_params->max_rate = cpu_to_le32(cfg->max_rate);
	cmd_params->committed_rate = cpu_to_le32(cfg->committed_rate);
	for (i = 0; i < 16; i++) {
		cmd_params->ep1_type[i] = endpoint1->type[i];
		cmd_params->ep2_type[i] = endpoint2->type[i];
	}

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dprc_disconnect() - Disconnect one endpoint to remove its network connection
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPRC object
 * @endpoint:	Endpoint configuration parameters
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dprc_disconnect(struct fsl_mc_io *mc_io,
		    uint32_t cmd_flags,
		    uint16_t token,
		    const struct dprc_endpoint *endpoint)
{
	struct mc_command cmd = { 0 };
	struct dprc_cmd_disconnect *cmd_params;
	int i;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_DISCONNECT,
					  cmd_flags,
					  token);
	cmd_params = (struct dprc_cmd_disconnect *)cmd.params;
	cmd_params->id = cpu_to_le32(endpoint->id);
	cmd_params->interface_id = cpu_to_le32(endpoint->if_id);
	for (i = 0; i < 16; i++)
		cmd_params->type[i] = endpoint->type[i];

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dprc_get_connection() - Get connected endpoint and link status if connection
 *			exists.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPRC object
 * @endpoint1:	Endpoint 1 configuration parameters
 * @endpoint2:	Returned endpoint 2 configuration parameters
 * @state:	Returned link state:
 *		1 - link is up;
 *		0 - link is down;
 *		-1 - no connection (endpoint2 information is irrelevant)
 *
 * Return:     '0' on Success; -ENAVAIL if connection does not exist.
 */
int dprc_get_connection(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			const struct dprc_endpoint *endpoint1,
			struct dprc_endpoint *endpoint2,
			int *state)
{
	struct mc_command cmd = { 0 };
	struct dprc_cmd_get_connection *cmd_params;
	struct dprc_rsp_get_connection *rsp_params;
	int err, i;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_CONNECTION,
					  cmd_flags,
					  token);
	cmd_params = (struct dprc_cmd_get_connection *)cmd.params;
	cmd_params->ep1_id = cpu_to_le32(endpoint1->id);
	cmd_params->ep1_interface_id = cpu_to_le16(endpoint1->if_id);
	for (i = 0; i < 16; i++)
		cmd_params->ep1_type[i] = endpoint1->type[i];

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dprc_rsp_get_connection *)cmd.params;
	endpoint2->id = le32_to_cpu(rsp_params->ep2_id);
	endpoint2->if_id = le16_to_cpu(rsp_params->ep2_interface_id);
	*state = le32_to_cpu(rsp_params->state);
	for (i = 0; i < 16; i++)
		endpoint2->type[i] = rsp_params->ep2_type[i];

	return 0;
}

/**
 * dprc_get_api_version - Get Data Path Resource Container API version
 * @mc_io:	Pointer to Mc portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @major_ver:	Major version of Data Path Resource Container API
 * @minor_ver:	Minor version of Data Path Resource Container API
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dprc_get_api_version(struct fsl_mc_io *mc_io,
			 uint32_t cmd_flags,
			 uint16_t *major_ver,
			 uint16_t *minor_ver)
{
	struct dprc_rsp_get_api_version *rsp_params;
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_API_VERSION,
					  cmd_flags, 0);

	/* send command to mc */
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dprc_rsp_get_api_version *)cmd.params;
	*major_ver = le16_to_cpu(rsp_params->major);
	*minor_ver = le16_to_cpu(rsp_params->minor);

	return 0;
}

/**
 * dprc_get_container_id - Get container ID associated with a given portal.
 * @mc_io:		Pointer to Mc portal's I/O object
 * @cmd_flags:		Command flags; one or more of 'MC_CMD_FLAG_'
 * @container_id:	Requested container id
 *
 * Return:	'0' on Success; Error code otherwise.
 */
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
	*container_id = (int)mc_cmd_read_object_id(&cmd);

	return 0;
}

/* Copyright 2013-2016 Freescale Semiconductor Inc.
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
#include "fsl_dpsw.h"
#include "fsl_dpsw_cmd.h"

/**
 * dpsw_open_v10() - Open a control session for the specified object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @dpsw_id:	DPSW unique ID
 * @token:	Returned token; use in subsequent API calls
 *
 * This function can be used to open a control session for an
 * already created object; an object may have been declared in
 * the DPL or by calling the dpsw_create() function.
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent commands for
 * this specific object
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_open_v10(struct fsl_mc_io *mc_io,
		  uint32_t cmd_flags,
		  int dpsw_id,
		  uint16_t *token)
{
	struct mc_command cmd = { 0 };
	struct dpsw_cmd_open *cmd_params;
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_OPEN,
					  cmd_flags,
					  0);
	cmd_params = (struct dpsw_cmd_open *)cmd.params;
	cmd_params->dpsw_id = cpu_to_le32(dpsw_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	*token = mc_cmd_hdr_read_token(&cmd);

	return 0;
}

/**
 * dpsw_close_v10() - Close the control session of the object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 *
 * After this function is called, no further operations are
 * allowed on the object without opening a new control session.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_close_v10(struct fsl_mc_io *mc_io,
		   uint32_t cmd_flags,
		   uint16_t token)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_CLOSE,
					  cmd_flags,
					  token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_create_v10() - Create the DPSW object.
 * @mc_io:	Pointer to MC portal's I/O object
 * @dprc_token:	Parent container token; '0' for default container
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @cfg:	Configuration structure
 * @obj_id:	Returned object id
 *
 * Create the DPSW object, allocate required resources and
 * perform required initialization.
 *
 * The object can be created either by declaring it in the
 * DPL file, or by calling this function.
 *
 * The function accepts an authentication token of a parent
 * container that this object should be assigned to. The token
 * can be '0' so the object will be assigned to the default container.
 * The newly created object can be opened with the returned
 * object id and using the container's associated tokens and MC portals.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_create_v10(struct fsl_mc_io *mc_io,
		    uint16_t dprc_token,
		    uint32_t cmd_flags,
		    const struct dpsw_cfg_v10 *cfg,
		    uint32_t *obj_id)
{
	struct mc_command cmd = { 0 };
	struct dpsw_cmd_create *cmd_params;
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_CREATE,
					  cmd_flags,
					  dprc_token);
	cmd_params = (struct dpsw_cmd_create *)cmd.params;
	cmd_params->num_ifs = cpu_to_le16(cfg->num_ifs);
	cmd_params->max_fdbs = cfg->adv.max_fdbs;
	cmd_params->max_meters_per_if = cfg->adv.max_meters_per_if;
	cmd_params->max_vlans = cpu_to_le16(cfg->adv.max_vlans);
	cmd_params->max_fdb_entries = cpu_to_le16(cfg->adv.max_fdb_entries);
	cmd_params->fdb_aging_time = cpu_to_le16(cfg->adv.fdb_aging_time);
	cmd_params->max_fdb_mc_groups = cpu_to_le16(cfg->adv.max_fdb_mc_groups);
	cmd_params->options = cpu_to_le64(cfg->adv.options);
	dpsw_set_field(cmd_params->component_type,
		       COMPONENT_TYPE,
		       cfg->adv.component_type);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	*obj_id = mc_cmd_read_object_id(&cmd);

	return 0;
}

/**
 * dpsw_destroy_v10() - Destroy the DPSW object and release all its resources.
 * @mc_io:	Pointer to MC portal's I/O object
 * @dprc_token: Parent container token; '0' for default container
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @object_id:	The object id; it must be a valid id within the container that
 * created this object;
 *
 * The function accepts the authentication token of the parent container that
 * created the object (not the one that currently owns the object). The object
 * is searched within parent using the provided 'object_id'.
 * All tokens to the object must be closed before calling destroy.
 *
 * Return:	'0' on Success; error code otherwise.
 */
int dpsw_destroy_v10(struct fsl_mc_io *mc_io,
		     uint16_t dprc_token,
		     uint32_t cmd_flags,
		     uint32_t object_id)
{
	struct mc_command cmd = { 0 };
	struct dpsw_cmd_destroy *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_DESTROY,
					  cmd_flags,
					  dprc_token);
	cmd_params = (struct dpsw_cmd_destroy *)cmd.params;
	cmd_params->dpsw_id = cpu_to_le32(object_id);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_get_irq_mask_v10() - Get interrupt mask.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @irq_index:	The interrupt index to configure
 * @mask:	Returned event mask to trigger interrupt
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_get_irq_mask_v10(struct fsl_mc_io *mc_io,
			  uint32_t cmd_flags,
			  uint16_t token,
			  uint8_t irq_index,
			  uint32_t *mask)
{
	struct mc_command cmd = { 0 };
	struct dpsw_cmd_get_irq_mask *cmd_params;
	struct dpsw_rsp_get_irq_mask *rsp_params;
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_GET_IRQ_MASK,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_get_irq_mask *)cmd.params;
	cmd_params->irq_index = irq_index;

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dpsw_rsp_get_irq_mask *)cmd.params;
	*mask = le32_to_cpu(rsp_params->mask);

	return 0;
}

/**
 * dpsw_get_irq_status_v10() - Get the current status of any pending interrupts
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @irq_index:	The interrupt index to configure
 * @status:	Returned interrupts status - one bit per cause:
 *			0 = no interrupt pending
 *			1 = interrupt pending
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_get_irq_status_v10(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    uint8_t irq_index,
			    uint32_t *status)
{
	struct mc_command cmd = { 0 };
	struct dpsw_cmd_get_irq_status *cmd_params;
	struct dpsw_rsp_get_irq_status *rsp_params;
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_GET_IRQ_STATUS,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_get_irq_status *)cmd.params;
	cmd_params->status = cpu_to_le32(*status);
	cmd_params->irq_index = irq_index;

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dpsw_rsp_get_irq_status *)cmd.params;
	*status = le32_to_cpu(rsp_params->status);

	return 0;
}

/**
 * dpsw_get_attributes_v10() - Retrieve DPSW attributes
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @attr:	Returned DPSW attributes
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_get_attributes_v10(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    struct dpsw_attr_v10 *attr)
{
	struct mc_command cmd = { 0 };
	struct dpsw_rsp_get_attr *rsp_params;
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_GET_ATTR,
					  cmd_flags,
					  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dpsw_rsp_get_attr *)cmd.params;
	attr->num_ifs = le16_to_cpu(rsp_params->num_ifs);
	attr->max_fdbs = rsp_params->max_fdbs;
	attr->num_fdbs = rsp_params->num_fdbs;
	attr->max_vlans = le16_to_cpu(rsp_params->max_vlans);
	attr->num_vlans = le16_to_cpu(rsp_params->num_vlans);
	attr->max_fdb_entries = le16_to_cpu(rsp_params->max_fdb_entries);
	attr->fdb_aging_time = le16_to_cpu(rsp_params->fdb_aging_time);
	attr->id = le32_to_cpu(rsp_params->dpsw_id);
	attr->mem_size = le16_to_cpu(rsp_params->mem_size);
	attr->max_fdb_mc_groups = le16_to_cpu(rsp_params->max_fdb_mc_groups);
	attr->max_meters_per_if = rsp_params->max_meters_per_if;
	attr->options = le64_to_cpu(rsp_params->options);
	attr->component_type = dpsw_get_field(rsp_params->component_type,
					      COMPONENT_TYPE);

	return 0;
}

/**
 * dpsw_get_api_version_v10() - Get Data Path Switch API version
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @major_ver:	Major version of data path switch API
 * @minor_ver:	Minor version of data path switch API
 *
 * Return:  '0' on Success; Error code otherwise.
 */
int dpsw_get_api_version_v10(struct fsl_mc_io *mc_io,
			     uint32_t cmd_flags,
			     uint16_t *major_ver,
			     uint16_t *minor_ver)
{
	struct mc_command cmd = { 0 };
	struct dpsw_rsp_get_api_version *rsp_params;
	int err;

	cmd.header = mc_encode_cmd_header(DPSW_CMDID_GET_API_VERSION,
					cmd_flags,
					0);

	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	rsp_params = (struct dpsw_rsp_get_api_version *)cmd.params;
	*major_ver = le16_to_cpu(rsp_params->version_major);
	*minor_ver = le16_to_cpu(rsp_params->version_minor);

	return 0;
}

/**
 * dpsw_if_set_taildrop() - enable taildrop for egress interface queues.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @if_id:	Interface Identifier
 * @tc_id:  Traffic class Id
 * @cfg: Taildrop configuration
 */
int dpsw_if_set_taildrop(struct fsl_mc_io *mc_io, uint32_t cmd_flags, uint16_t token,
			 uint16_t if_id, uint8_t tc_id, struct dpsw_taildrop_cfg *cfg)
{
	struct dpsw_cmd_set_taildrop *cmd_params;
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_IF_SET_TAILDROP,
					  cmd_flags, token);

	cmd_params = (struct dpsw_cmd_set_taildrop *)cmd.params;
	cmd_params->if_id = cpu_to_le16(if_id);
	cmd_params->tc = tc_id;
	cmd_params->units = cfg->units;
	cmd_params->threshold = cpu_to_le32(cfg->threshold);
	dpsw_set_field(cmd_params->oal_en, ENABLE, (!!cfg->enable));

	return mc_send_command(mc_io, &cmd);;
}

/**
 * dpsw_if_get_taildrop() - get current taildrop configuration.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @if_id:	Interface Identifier
 * @tc_id:  Traffic class Id
 * @cfg: Taildrop configuration
 */
int dpsw_if_get_taildrop(struct fsl_mc_io *mc_io, uint32_t cmd_flags, uint16_t token,
			 uint16_t if_id, uint8_t tc_id, struct dpsw_taildrop_cfg *cfg)
{
	struct dpsw_cmd_get_taildrop *cmd_params;
	struct dpsw_rsp_get_taildrop *rsp_params;
	struct mc_command cmd = {0};
	int err = 0;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_IF_GET_TAILDROP,
					  cmd_flags, token);

	cmd_params = (struct dpsw_cmd_get_taildrop *)cmd.params;
	cmd_params->if_id = cpu_to_le16(if_id);
	cmd_params->tc = tc_id;

	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dpsw_rsp_get_taildrop *)cmd.params;
	cfg->threshold = le32_to_cpu(rsp_params->threshold);
	cfg->units = rsp_params->units;
	cfg->enable = dpsw_get_field(rsp_params->oal_en, ENABLE);

	return err;
}


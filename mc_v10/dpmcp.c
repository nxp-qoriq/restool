/* Copyright 2013-2016 Freescale Semiconductor Inc.
 * Copyright 2017 NXP
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
#include "fsl_dpmcp.h"
#include "fsl_dpmcp_cmd.h"

/**
 * dpmcp_open_v10() - Open a control session for the specified object.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @dpmcp_id:	DPMCP unique ID
 * @token:	Returned token; use in subsequent API calls
 *
 * This function can be used to open a control session for an
 * already created object; an object may have been declared in
 * the DPL or by calling the dpmcp_create function.
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent commands for
 * this specific object
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpmcp_open_v10(struct fsl_mc_io *mc_io,
		   uint32_t cmd_flags,
		   int dpmcp_id,
		   uint16_t *token)
{
	struct mc_command cmd = { 0 };
	struct dpmcp_cmd_open *cmd_params;
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPMCP_CMDID_OPEN,
					  cmd_flags, 0);
	cmd_params = (struct dpmcp_cmd_open *)cmd.params;
	cmd_params->dpmcp_id = cpu_to_le32(dpmcp_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	*token = mc_cmd_hdr_read_token(&cmd);

	return err;
}

/**
 * dpmcp_close_v10() - Close the control session of the object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPMCP object
 *
 * After this function is called, no further operations are
 * allowed on the object without opening a new control session.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpmcp_close_v10(struct fsl_mc_io *mc_io,
		    uint32_t cmd_flags,
		    uint16_t token)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPMCP_CMDID_CLOSE,
					  cmd_flags, token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpmcp_create_v10() - Create the DPMCP object.
 * @mc_io:	Pointer to MC portal's I/O object
 * @dprc_token:	Parent container token; '0' for default container
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @cfg:	Configuration structure
 * @obj_id:	Returned object id; use in subsequent API calls
 *
 * Create the DPMCP object, allocate required resources and
 * perform required initialization.
 *
 * The object can be created either by declaring it in the
 * DPL file, or by calling this function.

 * This function accepts an authentication token of a parent
 * container that this object should be assigned to and returns
 * an object id. This object_id will be used in all subsequent calls to
 * this specific object.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpmcp_create_v10(struct fsl_mc_io *mc_io,
		     uint16_t dprc_token,
		     uint32_t cmd_flags,
		     const struct dpmcp_cfg *cfg,
		     uint32_t *obj_id)
{
	struct mc_command cmd = { 0 };
	struct dpmcp_cmd_create *cmd_params;

	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPMCP_CMDID_CREATE,
					  cmd_flags, dprc_token);
	cmd_params = (struct dpmcp_cmd_create *)cmd.params;
	cmd_params->portal_id = cpu_to_le32(cfg->portal_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	*obj_id = mc_cmd_read_object_id(&cmd);

	return 0;
}

/**
 * dpmcp_destroy_v10() - Destroy the DPMCP object and release all its resources.
 * @mc_io:	Pointer to MC portal's I/O object
 * @dprc_token:	Parent container token; '0' for default container
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @obj_id:	ID of DPMCP object
 *
 * Return:	'0' on Success; error code otherwise.
 */
int dpmcp_destroy_v10(struct fsl_mc_io *mc_io,
		      uint16_t dprc_token,
		      uint32_t cmd_flags,
		      uint32_t obj_id)
{
	struct mc_command cmd = { 0 };
	struct dpmcp_cmd_destroy *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPMCP_CMDID_DESTROY,
					  cmd_flags, dprc_token);
	cmd_params = (struct dpmcp_cmd_destroy *)cmd.params;
	cmd_params->object_id = cpu_to_le32(obj_id);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpmcp_get_irq_mask_v10() - Get interrupt mask.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPMCP object
 * @irq_index:	The interrupt index to configure
 * @mask:	Returned event mask to trigger interrupt
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpmcp_get_irq_mask_v10(struct fsl_mc_io *mc_io,
			   uint32_t cmd_flags,
			   uint16_t token,
			   uint8_t irq_index,
			   uint32_t *mask)
{
	struct mc_command cmd = { 0 };
	struct dpmcp_cmd_get_irq_mask *cmd_params;
	struct dpmcp_rsp_get_irq_mask *rsp_params;

	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPMCP_CMDID_GET_IRQ_MASK,
					  cmd_flags, token);
	cmd_params = (struct dpmcp_cmd_get_irq_mask *)cmd.params;
	cmd_params->irq_index = irq_index;

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dpmcp_rsp_get_irq_mask *)cmd.params;
	*mask = le32_to_cpu(rsp_params->mask);

	return 0;
}

/**
 * dpmcp_get_irq_status_v10() - Get the current status of any pending interrupts.
 *
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPMCP object
 * @irq_index:	The interrupt index to configure
 * @status:	Returned interrupts status - one bit per cause:
 *			0 = no interrupt pending
 *			1 = interrupt pending
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpmcp_get_irq_status_v10(struct fsl_mc_io *mc_io,
			     uint32_t cmd_flags,
			     uint16_t token,
			     uint8_t irq_index,
			     uint32_t *status)
{
	struct mc_command cmd = { 0 };
	struct dpmcp_cmd_get_irq_status *cmd_params;
	struct dpmcp_rsp_get_irq_status *rsp_params;
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPMCP_CMDID_GET_IRQ_STATUS,
					  cmd_flags, token);
	cmd_params = (struct dpmcp_cmd_get_irq_status *)cmd.params;
	cmd_params->status = cpu_to_le32(*status);
	cmd_params->irq_index = irq_index;

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dpmcp_rsp_get_irq_status *)cmd.params;
	*status = le32_to_cpu(rsp_params->status);

	return 0;
}

/**
 * dpmcp_get_attributes_v10() - Retrieve DPMCP attributes.
 *
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPMCP object
 * @attr:	Returned object's attributes
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpmcp_get_attributes_v10(struct fsl_mc_io *mc_io,
			     uint32_t cmd_flags,
			     uint16_t token,
			     struct dpmcp_attr_v10 *attr)
{
	struct mc_command cmd = { 0 };
	struct dpmcp_rsp_get_attributes *rsp_params;
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPMCP_CMDID_GET_ATTR,
					  cmd_flags, token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dpmcp_rsp_get_attributes *)cmd.params;
	attr->id = le32_to_cpu(rsp_params->id);

	return 0;
}

/**
 * dpmcp_get_api_version_v10() - Get Data Path Management Command Portal API version
 * @mc_io:	Pointer to Mc portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @major_ver:	Major version of Data Path Management Command Portal API
 * @minor_ver:	Minor version of Data Path Management Command Portal API
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpmcp_get_api_version_v10(struct fsl_mc_io *mc_io,
			      uint32_t cmd_flags,
			      uint16_t *major_ver,
			      uint16_t *minor_ver)
{
	struct dpmcp_rsp_get_api_version *rsp_params;
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPMCP_CMDID_GET_API_VERSION,
					  cmd_flags, 0);

	/* send command to mc */
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dpmcp_rsp_get_api_version *)cmd.params;
	*major_ver = le16_to_cpu(rsp_params->major);
	*minor_ver = le16_to_cpu(rsp_params->minor);

	return 0;
}

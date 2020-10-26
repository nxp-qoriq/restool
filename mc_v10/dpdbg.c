/* Copyright 2013-2015 Freescale Semiconductor Inc.
 * Copyright 2019 NXP
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
#include <string.h>
#include "fsl_mc_sys.h"
#include "fsl_mc_cmd.h"
#include "fsl_dpdbg.h"
#include "fsl_dpdbg_cmd.h"

/** @addtogroup dpdbg
 * @{
 */

/**
 * dpdbg_open_v10() - Open a control session for the specified object.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @dpdbg_id:	DPDBG unique ID
 * @token:	Returned token; use in subsequent API calls
 *
 * This function can be used to open a control session for an
 * already created object;
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent commands for
 * this specific object
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_open_v10(struct fsl_mc_io *mc_io,
	       uint32_t cmd_flags,
	       int dpdbg_id,
	       uint16_t *token)
{
	struct dpdbg_cmd_open *cmd_params;
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_OPEN,
					  cmd_flags,
					  0);
	cmd_params = (struct dpdbg_cmd_open *)cmd.params;
	cmd_params->dpdbg_id = cpu_to_le64(dpdbg_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	*token = mc_cmd_hdr_read_token(&cmd);

	return err;
}

/**
 * dpdbg_close_v10() - Close the control session of the object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 *
 * After this function is called, no further operations are
 * allowed on the object without opening a new control session.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_close_v10(struct fsl_mc_io *mc_io,
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

/**
 * dpdbg_create_v10() - Create the DPDBG object.
 * @mc_io:	Pointer to MC portal's I/O object
 * @dprc_token:	Parent container token; '0' for default container
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @cfg:	Configuration structure
 * @obj_id:	Returned object id
 *
 * Create the DPDBG object, allocate required resources and
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
int dpdbg_create_v10(struct fsl_mc_io *mc_io,
		  uint16_t dprc_token,
		  uint32_t cmd_flags,
		  const struct dpdbg_cfg *cfg,
		  uint32_t *obj_id)
{
	struct dpdbg_cmd_create *cmd_params;
	struct mc_command cmd = { 0 };
	int err;

	(void)(cfg); /* unused */

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_CREATE,
					  cmd_flags,
					  dprc_token);
	cmd_params = (struct dpdbg_cmd_create *)cmd.params;
	cmd_params->dpdbg_id = cpu_to_le32(cfg->dpdbg_id);
	cmd_params->dpdbg_container_id = cpu_to_le32(cfg->dpdbg_container_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	*obj_id = mc_cmd_read_object_id(&cmd);

	return 0;
}

/**
 * dpdbg_destroy_v10() - Destroy the DPDBG object and release all its resources.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 *
 * The function accepts the authentication token of the parent container that
 * created the object (not the one that currently owns the object). The object
 * is searched within parent using the provided 'object_id'.
 * All tokens to the object must be closed before calling destroy.
 *
 * Return:	'0' on Success; error code otherwise.
 */
int dpdbg_destroy_v10(struct fsl_mc_io *mc_io,
		   uint16_t dprc_token,
		   uint32_t cmd_flags,
		   uint32_t object_id)
{
	struct dpdbg_cmd_destroy *cmd_params;
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_DESTROY,
					  cmd_flags,
					  dprc_token);
	cmd_params = (struct dpdbg_cmd_destroy *)cmd.params;
	cmd_params->dpdbg_id = cpu_to_le32(object_id);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpdbg_get_attributes_v10 - Retrieve DPDBG attributes.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @attr:	Returned object's attributes
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_get_attributes_v10(struct fsl_mc_io *mc_io,
			 uint32_t cmd_flags,
			 uint16_t token,
			 struct dpdbg_attr *attr)
{
	struct dpdbg_rsp_get_attributes *rsp_params;
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
	rsp_params = (struct dpdbg_rsp_get_attributes *)cmd.params;
	attr->id = le32_to_cpu(rsp_params->id);

	return 0;
}

/**
 * dpdbg_dump_v10 - Get informations about an existing object.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @obj_id:	ID of required object
 * @obj_type: Type of required object
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_dump_v10(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			int obj_id,
			char obj_type[OBJ_TYPE_LENGTH])
{
	struct dpdbg_cmd_dump *cmd_params;
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_DUMP, cmd_flags, token);
	cmd_params = (struct dpdbg_cmd_dump *)cmd.params;
	cmd_params->id = cpu_to_le32(obj_id);
	strncpy((char *)(cmd_params->type), (char *)obj_type, OBJ_TYPE_LENGTH - 1);
	cmd_params->type[OBJ_TYPE_LENGTH - 1] = '\0';

	/* send command to MC */
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	return 0;
}

/**
 * dpdbg_set_v10 - Set state of a module.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @state:	New state for required module
 * @module: Required module
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_set_v10(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			int state,
			char module[MODULE_NAME_LENGTH])
{
	struct dpdbg_cmd_set *cmd_params;
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_SET, cmd_flags, token);
	cmd_params = (struct dpdbg_cmd_set *)cmd.params;
	cmd_params->state = cpu_to_le32(state);
	strncpy((char *)(cmd_params->module),
			(char *)module,
			MODULE_NAME_LENGTH - 1);
	cmd_params->module[MODULE_NAME_LENGTH - 1] = '\0';

	/* send command to MC */
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	return 0;
}

/**
 * dpdbg_get_api_version_v10() - Get Data Path Debug API version
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @majorVer:	Major version of data path debug API
 * @minorVer:	Minor version of data path debug API
 *
 * Return:  '0' on Success; Error code otherwise.
 */
int dpdbg_get_api_version_v10(struct fsl_mc_io *mc_io,
			   uint32_t cmd_flags,
			   uint16_t *major_ver,
			   uint16_t *minor_ver)
{
	struct dpdbg_rsp_get_api_version *rsp_params;
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_GET_API_VERSION,
					cmd_flags,
					0);

	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	rsp_params = (struct dpdbg_rsp_get_api_version *)cmd.params;
	*major_ver = le16_to_cpu(rsp_params->major);
	*minor_ver = le16_to_cpu(rsp_params->minor);

	return 0;
}

/**
 * dpdbg_set_ctlu_profiling_counters() - set ctlu profiling options;
 * resets the existing profiling configuration: enable/disable profiling
 * or change the target table to profile in the selected CTLU type.
 *
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @ctlu_type: the ctlu type: EIOP EGRESS/INGRESS, AIOP or AIOP MFLU
 * @options: ctlu profiling options
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_set_ctlu_profiling_counters(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t ctlu_type,
			struct ctlu_profiling_options *options)
{
	struct mc_command cmd = {0};
	struct dpdbg_cmd_set_ctlu_profiling_counters *cmd_params;

	(void)(cmd_flags);

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_SET_CTLU_PROFILING,
					  0,
					  token);
	cmd_params =
		(struct dpdbg_cmd_set_ctlu_profiling_counters *) cmd.params;
	dpdbg_set_field(cmd_params->ctlu_type, CTLU_TYPE, ctlu_type);
	dpdbg_set_field(cmd_params->opt_enable_prof_ctrs, OPT_ENABLE_PROF,
			options->enable_profiling_counters);
	dpdbg_set_field(cmd_params->opt_enable_prof_for_tid,
			OPT_ENABLE_PROF_TID,
			options->enable_profiling_for_tid);
	cmd_params->table_id = options->table_id;

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpdbg_get_ctlu_profiling_counters() - get ctlu profiling counters
 *
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @ctlu_type: the ctlu type: EIOP EGRESS/INGRESS, AIOP or AIOP MFLU
 * @counters: The profiling counters
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_get_ctlu_profiling_counters(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t ctlu_type,
			struct ctlu_profiling_counters *counters)
{
	struct mc_command cmd = {0};
	struct dpdbg_cmd_get_ctlu_profiling_counters *cmd_params;
	struct dpdbg_rsp_get_ctlu_profiling_counters *rsp_params;
	int err = 0;

	(void)(cmd_flags);

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPDBG_CMDID_SET_CTLU_PROFILING,
					  0,
					  token);
	cmd_params =
		(struct dpdbg_cmd_get_ctlu_profiling_counters *) cmd.params;
	dpdbg_set_field(cmd_params->ctlu_type, CTLU_TYPE, ctlu_type);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dpdbg_rsp_get_ctlu_profiling_counters *)cmd.params;
	counters->cache_accesses = le32_to_cpu(rsp_params->cache_accesses);
	counters->cache_hits = le32_to_cpu(rsp_params->cache_hits);
	counters->cache_updates = le32_to_cpu(rsp_params->cache_updates);
	counters->entry_hits = le32_to_cpu(rsp_params->entry_hits);
	counters->entry_lookups = le32_to_cpu(rsp_params->entry_lookups);
	counters->rule_hits = le32_to_cpu(rsp_params->rule_hits);
	counters->rule_lookups = le32_to_cpu(rsp_params->rule_lookups);
	counters->memory_accesses = le32_to_cpu(rsp_params->memory_accesses);

	return err;
}

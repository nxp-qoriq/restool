/*
 * Freescale Layerscape MC DPRC command wrappers
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

int dprc_get_container_id(
	struct mc_portal_wrapper *mc_portal,
	uint32_t *container_id)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_get_container_id(&cmd);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	parse_resp_dprc_get_container_id(&cmd, container_id);
	return 0;
}

int dprc_open(
	struct mc_portal_wrapper *mc_portal,
	int container_id,
	uint16_t *dprc_handle)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_open(&cmd, container_id);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	parse_resp_dprc_open(&cmd, dprc_handle);
	return 0;
}

int dprc_close(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_close(&cmd, dprc_handle);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	return 0;
}

int dprc_create_container(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	struct dprc_cfg *cfg,
	uint32_t *child_container_id,
	uint64_t *child_portal_paddr)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_create_container(&cmd, dprc_handle, cfg);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	parse_resp_dprc_create_container(&cmd, child_container_id,
					 child_portal_paddr);
	return 0;
}

int dprc_destroy_container(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint32_t child_container_id)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_destroy_container(&cmd, dprc_handle, child_container_id);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	return 0;
}

int dprc_reset_container(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint32_t child_container_id)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_reset_container(&cmd, dprc_handle, child_container_id);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	return 0;
}

int dprc_assign(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint32_t child_container_id,
	struct dprc_res_req *res_req)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_assign(&cmd, dprc_handle, child_container_id, res_req);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	return 0;
}

int dprc_unassign(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	int child_container_id,
	struct dprc_res_req *res_req)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_unassign(&cmd, dprc_handle, child_container_id, res_req);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	return 0;
}

int dprc_get_device_count(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint32_t *dev_count)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_get_device_count(&cmd, dprc_handle);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	parse_resp_dprc_get_device_count(&cmd, dev_count);
	return 0;
}

int dprc_get_device(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint16_t dev_index,
	struct dprc_dev_desc *dev_desc)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_get_device(&cmd, dprc_handle, dev_index);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	parse_resp_dprc_get_device(&cmd, dev_desc);
	return 0;
}

int dprc_get_res_count(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	char *res_type,
	uint32_t *res_count)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_get_res_count(&cmd, dprc_handle, res_type);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	parse_resp_dprc_get_res_count(&cmd, res_count);
	return 0;
}

int dprc_get_res_ids(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	char *res_type,
	struct dprc_res_ids_range_desc *range_desc)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_get_res_ids(&cmd, dprc_handle, res_type, range_desc);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	parse_resp_dprc_get_res_ids(&cmd, range_desc);
	return 0;
}

int dprc_get_attributes(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	struct dprc_attributes *attributes)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_get_attributes(&cmd, dprc_handle);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	parse_resp_dprc_get_attributes(&cmd, attributes);
	return 0;
}

int dprc_get_dev_region(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	char *dev_type,
	uint16_t dev_id,
	uint8_t region_index,
	struct dprc_region_desc *region_desc)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_get_dev_region(&cmd, dprc_handle, dev_type, dev_id,
				      region_index);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	parse_resp_dprc_get_dev_region(&cmd, region_desc);
	return 0;
}

int dprc_set_irq(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint8_t irq_index,
	uint64_t irq_paddr,
	uint32_t irq_val)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_set_irq(&cmd, dprc_handle, irq_index, irq_paddr,
			       irq_val);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	return 0;
}

int dprc_get_irq(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint8_t irq_index,
	uint32_t *type,
	uint64_t *irq_paddr,
	uint32_t *irq_val)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_get_irq(&cmd, dprc_handle, irq_index);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	parse_resp_dprc_get_irq(&cmd, type, irq_paddr, irq_val);
	return 0;
}

int dprc_set_irq_enable(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint8_t irq_index,
	uint8_t enable_state)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_set_irq_enable(&cmd, dprc_handle, irq_index,
				      enable_state);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	return 0;
}

int dprc_get_irq_enable(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint8_t irq_index,
	uint8_t *enable_state)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_get_irq_enable(&cmd, dprc_handle, irq_index);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	parse_resp_dprc_get_irq_enable(&cmd, enable_state);
	return 0;
}

int dprc_set_irq_mask(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint8_t irq_index,
	uint32_t mask)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_set_irq_mask(&cmd, dprc_handle, irq_index, mask);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	return 0;
}

int dprc_get_irq_mask(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint8_t irq_index,
	uint32_t *mask)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_get_irq_mask(&cmd, dprc_handle, irq_index);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	parse_resp_dprc_get_irq_mask(&cmd, mask);
	return 0;
}

int dprc_get_irq_status(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint8_t irq_index,
	uint32_t *status)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_get_irq_status(&cmd, dprc_handle, irq_index);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	parse_resp_dprc_get_irq_status(&cmd, status);
	return 0;
}

int dprc_clear_irq_status(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint8_t irq_index,
	uint32_t status)
{
	int error;
	struct mc_command cmd;

	build_cmd_dprc_clear_irq_status(&cmd, dprc_handle, irq_index, status);
	error = mc_portal_wrapper_send_command(mc_portal, &cmd);
	if (error < 0)
		return error;

	return 0;
}



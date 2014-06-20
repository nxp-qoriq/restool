/*
 * Freescale Layerscape Management Complex (MC) command wrappers
 *
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#ifndef _FSL_MC_CMD_WRAPPERS_H
#define _FSL_MC_CMD_WRAPPERS_H

#include <string.h>
#include "fsl_mc_common_wrapper.h"
#include <fsl_mc_dpmng_commands.h>
#include <fsl_mc_dprc_commands.h>

struct mc_portal_wrapper;

int mc_get_version(
	struct mc_portal_wrapper *mc_portal,
	struct mc_version *mc_ver_info);

int dprc_get_container_id(
	struct mc_portal_wrapper *mc_portal,
	uint32_t *container_id);

int dprc_open(
	struct mc_portal_wrapper *mc_portal,
	int container_id,
	uint16_t *dprc_handle);

int dprc_close(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle);

int dprc_create_container(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	struct dprc_cfg *cfg,
	uint32_t *child_container_id,
	uint64_t *child_portal_paddr);

int dprc_destroy_container(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint32_t child_container_id);

int dprc_reset_container(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint32_t child_container_id);

int dprc_assign(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint32_t child_container_id,
	struct dprc_res_req *res_req);

int dprc_unassign(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	int child_container_id,
	struct dprc_res_req *res_req);

int dprc_get_device_count(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint32_t *dev_count);

int dprc_get_device(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint16_t dev_index,
	struct dprc_dev_desc *dev_desc);

int dprc_get_res_count(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	char *res_type,
	uint32_t *res_count);

int dprc_get_res_ids(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	char *res_type,
	struct dprc_res_ids_range_desc *range_desc);

int dprc_get_attributes(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	struct dprc_attributes *attributes);

int dprc_get_dev_region(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	char *dev_type,
	uint16_t dev_id,
	uint8_t region_index,
	struct dprc_region_desc *region_desc);

int dprc_set_irq(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint8_t irq_index,
	uint64_t irq_paddr,
	uint32_t irq_val);

int dprc_get_irq(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint8_t irq_index,
	uint32_t *type,
	uint64_t *irq_paddr,
	uint32_t *irq_val);

int dprc_set_irq_enable(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint8_t irq_index,
	uint8_t enable_state);

int dprc_get_irq_enable(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint8_t irq_index,
	uint8_t *enable_state);

int dprc_set_irq_mask(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint8_t irq_index,
	uint32_t mask);

int dprc_get_irq_mask(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint8_t irq_index,
	uint32_t *mask);

int dprc_get_irq_status(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint8_t irq_index,
	uint32_t *status);

int dprc_clear_irq_status(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle,
	uint8_t irq_index,
	uint32_t status);

#endif /* _FSL_MC_CMD_WRAPPERS_H */


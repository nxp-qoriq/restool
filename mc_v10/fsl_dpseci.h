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
#ifndef __FSL_DPSECI_V10_H
#define __FSL_DPSECI_V10_H

#include "../mc_v9/fsl_dpseci.h"

/* Data Path SEC Interface API
 * Contains initialization APIs and runtime control APIs for DPSECI
 */

struct fsl_mc_io;

int dpseci_open_v10(struct fsl_mc_io *mc_io,
		    uint32_t cmd_flags,
		    int dpseci_id,
		    uint16_t *token);

int dpseci_close_v10(struct fsl_mc_io *mc_io,
		     uint32_t cmd_flags,
		     uint16_t token);

/**
 * Enable the Congestion Group support
 */
#define DPSECI_OPT_HAS_CG				0x000020

/**
 * Enable the Order Restoration support
 */
#define DPSECI_OPT_HAS_OPR				0x000040

/**
 * Order Point Records are shared for the entire DPSECI
 */
#define DPSECI_OPT_OPR_SHARED				0x000080

/**
 * struct dpseci_cfg_v10 - Structure representing DPSECI configuration
 * @options: Any combination of the following options:
 *		DPSECI_OPT_HAS_CG
 *		DPSECI_OPT_HAS_OPR
 *		DPSECI_OPT_OPR_SHARED
 * @num_tx_queues: num of queues towards the SEC
 * @num_rx_queues: num of queues back from the SEC
 * @priorities: Priorities for the SEC hardware processing;
 *		each place in the array is the priority of the tx queue
 *		towards the SEC,
 *		valid priorities are configured with values 1-8;
 */
struct dpseci_cfg_v10 {
	uint32_t options;
	uint8_t num_tx_queues;
	uint8_t num_rx_queues;
	uint8_t priorities[DPSECI_PRIO_NUM];
};

int dpseci_create_v10(struct fsl_mc_io *mc_io,
		      uint16_t dprc_token,
		      uint32_t cmd_flags,
		      const struct dpseci_cfg_v10 *cfg,
		      uint32_t *obj_id);

int dpseci_destroy_v10(struct fsl_mc_io *mc_io,
		       uint16_t dprc_token,
		       uint32_t cmd_flags,
		       uint32_t object_id);

int dpseci_get_irq_mask_v10(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    uint8_t irq_index,
			    uint32_t *mask);

int dpseci_get_irq_status_v10(struct fsl_mc_io *mc_io,
			      uint32_t cmd_flags,
			      uint16_t token,
			      uint8_t irq_index,
			      uint32_t *status);

/**
 * struct dpseci_attr_v10 - Structure representing DPSECI attributes
 * @id: DPSECI object ID
 * @num_tx_queues: number of queues towards the SEC
 * @num_rx_queues: number of queues back from the SEC
 * @options: Any combination of the following options:
 *		DPSECI_OPT_HAS_CG
 *		DPSECI_OPT_HAS_OPR
 *		DPSECI_OPT_OPR_SHARED
 */
struct dpseci_attr_v10 {
	int id;
	uint8_t num_tx_queues;
	uint8_t num_rx_queues;
	uint32_t options;
};

int dpseci_get_attributes_v10(struct fsl_mc_io *mc_io,
			      uint32_t cmd_flags,
			      uint16_t token,
			      struct dpseci_attr_v10 *attr);
/**
 * struct dpseci_tx_queue_attr_v10 - Structure representing attributes of Tx queues
 * @fqid: Virtual FQID to be used for sending frames to SEC hardware
 * @priority: SEC hardware processing priority for the queue
 */
struct dpseci_tx_queue_attr_v10 {
	uint32_t fqid;
	uint8_t priority;
};

int dpseci_get_tx_queue_v10(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    uint8_t queue,
			    struct dpseci_tx_queue_attr_v10 *attr);

int dpseci_get_api_version_v10(struct fsl_mc_io *mc_io,
			       uint32_t cmd_flags,
			       uint16_t *major_ver,
			       uint16_t *minor_ver);

#endif /* __FSL_DPSECI_H */

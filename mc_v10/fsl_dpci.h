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
#ifndef __FSL_DPCI_V10_H
#define __FSL_DPCI_V10_H

/* Data Path Communication Interface API
 * Contains initialization APIs and runtime control APIs for DPCI
 */

struct fsl_mc_io;

/** General DPCI macros */

/**
 * Maximum number of Tx/Rx priorities per DPCI object
 */
#define DPCI_PRIO_NUM		4

/**
 * Indicates an invalid frame queue
 */
#define DPCI_FQID_NOT_VALID	(uint32_t)(-1)

/**
 * All queues considered; see dpci_set_rx_queue()
 */
#define DPCI_ALL_QUEUES		(uint8_t)(-1)

int dpci_open_v10(struct fsl_mc_io *mc_io,
		  uint32_t cmd_flags,
		  int dpci_id,
		  uint16_t *token);

int dpci_close_v10(struct fsl_mc_io *mc_io,
		   uint32_t cmd_flags,
		   uint16_t token);

/**
 * Enable the Order Restoration support
 */
#define DPCI_OPT_HAS_OPR					0x000040

/**
 * Order Point Records are shared for the entire DPCI
 */
#define DPCI_OPT_OPR_SHARED					0x000080

/**
 * struct dpci_cfg - Structure representing DPCI configuration
 * @options: Any combination of the following options:
 *		DPCI_OPT_HAS_OPR
 *		DPCI_OPT_OPR_SHARED
 * @num_of_priorities:	Number of receive priorities (queues) for the DPCI;
 *			note, that the number of transmit priorities (queues)
 *			is determined by the number of receive priorities of
 *			the peer DPCI object
 */
struct dpci_cfg_v10 {
	uint32_t options;
	uint8_t num_of_priorities;
};

int dpci_create_v10(struct fsl_mc_io *mc_io,
		    uint16_t dprc_token,
		    uint32_t cmd_flags,
		    const struct dpci_cfg_v10 *cfg,
		    uint32_t *obj_id);

int dpci_destroy_v10(struct fsl_mc_io *mc_io,
		     uint16_t dprc_token,
		     uint32_t cmd_flags,
		     uint32_t object_id);


int dpci_get_irq_mask_v10(struct fsl_mc_io *mc_io,
			  uint32_t cmd_flags,
			  uint16_t token,
			  uint8_t irq_index,
			  uint32_t *mask);

int dpci_get_irq_status_v10(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    uint8_t irq_index,
			    uint32_t *status);

/**
 * struct dpci_attr - Structure representing DPCI attributes
 * @id:			DPCI object ID
 * @num_of_priorities:	Number of receive priorities
 */
struct dpci_attr_v10 {
	int id;
	uint8_t num_of_priorities;
};

int dpci_get_attributes_v10(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    struct dpci_attr_v10 *attr);

/**
 * struct dpci_peer_attr - Structure representing the peer DPCI attributes
 * @peer_id:		DPCI peer id; if no peer is connected returns (-1)
 * @num_of_priorities:	The pper's number of receive priorities; determines the
 *			number of transmit priorities for the local DPCI object
 */
struct dpci_peer_attr_v10 {
	int peer_id;
	uint8_t num_of_priorities;
};

int dpci_get_peer_attributes_v10(struct fsl_mc_io *mc_io,
				 uint32_t cmd_flags,
				 uint16_t token,
				 struct dpci_peer_attr_v10 *attr);

int dpci_get_link_state_v10(struct fsl_mc_io *mc_io,
			    uint32_t cmd_flags,
			    uint16_t token,
			    int *up);

int dpci_get_api_version_v10(struct fsl_mc_io *mc_io,
			     uint32_t cmd_flags,
			     uint16_t *major_ver,
			     uint16_t *minor_ver);
#endif /* __FSL_DPCI_V10_H */

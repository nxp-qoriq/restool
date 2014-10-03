/* Copyright 2014 Freescale Semiconductor Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <errno.h>
#include "fsl_mc_sys.h"
#include "fsl_mc_cmd.h"
#include "fsl_dpni.h"
#include "fsl_dpni_cmd.h"

#define CMD_PREP(_param, _offset, _width, _type, _arg) \
	(cmd.params[_param] |= u64_enc((_offset), (_width), (_arg)))

#define RSP_READ(_param, _offset, _width, _type, _arg) \
	(*(_arg) = (_type)u64_dec(cmd.params[_param], (_offset), (_width)))

#define RSP_READ_STRUCT(_param, _offset, _width, _type, _arg) \
	(_arg = (_type)u64_dec(cmd.params[_param], (_offset), (_width)))

#if 0
static int build_extract_cfg_extention(struct dpkg_profile_cfg *cfg,
				       struct extract_data *data)
{
	int i, j;
	int offset = 0;
	int param = 1;
	struct {
		enum net_prot prot;
		enum dpkg_extract_from_hdr_type type;
		uint32_t field;
		uint8_t size;
		uint8_t offset;
		uint8_t hdr_index;
		enum dpkg_extract_from_context_type src;
		uint8_t constant;
		uint8_t num_of_repeats;
	} u_cfg[DPKG_MAX_NUM_OF_EXTRACTS];

	for (i = 0; i < DPKG_MAX_NUM_OF_EXTRACTS; i++) {
		switch (cfg->extracts[i].type) {
		case DPKG_EXTRACT_FROM_HDR:
			u_cfg[i].prot = cfg->extracts[i].extract.from_hdr.prot;
			u_cfg[i].type = cfg->extracts[i].extract.from_hdr.type;
			u_cfg[i].field =
				cfg->extracts[i].extract.from_hdr.field;
			u_cfg[i].size = cfg->extracts[i].extract.from_hdr.size;
			u_cfg[i].offset =
				cfg->extracts[i].extract.from_hdr.offset;
			u_cfg[i].hdr_index =
				cfg->extracts[i].extract.from_hdr.hdr_index;
			break;
		case DPKG_EXTRACT_FROM_DATA:
			u_cfg[i].size =
				cfg->extracts[i].extract.from_data.size;
			u_cfg[i].offset =
				cfg->extracts[i].extract.from_data.offset;
			break;
		case DPKG_EXTRACT_FROM_CONTEXT:
			u_cfg[i].src =
				cfg->extracts[i].extract.from_context.src;
			u_cfg[i].size =
				cfg->extracts[i].extract.from_context.size;
			u_cfg[i].offset =
				cfg->extracts[i].extract.from_context.offset;
			break;
		case DPKG_EXTRACT_CONSTANT:
			u_cfg[i].constant =
				cfg->extracts[i].extract.constant.constant;
			u_cfg[i].num_of_repeats =
				cfg->extracts[i].
					extract.constant.num_of_repeats;
			break;
		default:
			return -EINVAL;
		}
	}
	data->params[0] |= u64_enc(0, 8, cfg->num_extracts);
	data->params[0] = cpu_to_le64(data->params[0]);

	for (i = 0; i < DPKG_MAX_NUM_OF_EXTRACTS; i++) {
		data->params[param] |= u64_enc(0, 8, u_cfg[i].prot);
		data->params[param] |= u64_enc(8, 4, u_cfg[i].type);
		data->params[param] |= u64_enc(12, 4, u_cfg[i].src);
		data->params[param] |= u64_enc(16, 8, u_cfg[i].size);
		data->params[param] |= u64_enc(24, 8, u_cfg[i].offset);
		data->params[param] |= u64_enc(32, 32, u_cfg[i].field);
		data->params[param] = cpu_to_le64(data->params[param]);
		param++;
		data->params[param] |= u64_enc(0, 8, u_cfg[i].hdr_index);
		data->params[param] |= u64_enc(8, 8, u_cfg[i].constant);
		data->params[param] |= u64_enc(16, 8, u_cfg[i].num_of_repeats);
		data->params[param] |= u64_enc(
			24, 8, cfg->extracts[i].num_of_byte_masks);
		data->params[param] |= u64_enc(32, 4, cfg->extracts[i].type);
		data->params[param] = cpu_to_le64(data->params[param]);
		param++;
		for (j = 0; j < 4; j++) {
			data->params[param] |= u64_enc(
				(offset), 8, cfg->extracts[i].masks[j].mask);
			data->params[param] |= u64_enc(
				(offset + 8), 8,
				cfg->extracts[i].masks[j].offset);
			offset += 16;
		}
		data->params[param] = cpu_to_le64(data->params[param]);
		param++;
	}

	return 0;
}
#endif

int dpni_create(struct fsl_mc_io *mc_io, const struct dpni_cfg *cfg,
		uint16_t *dpni_handle)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(MC_DPNI_CMDID_CREATE,
					  DPNI_CMDSZ_CREATE,
					  MC_CMD_PRI_LOW, 0);

	DPNI_CMD_CREATE(CMD_PREP);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		*dpni_handle = MC_CMD_HDR_READ_AUTHID(cmd.header);

	return err;
}

int dpni_open(struct fsl_mc_io *mc_io, int dpni_id, uint16_t *dpni_handle)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(MC_DPNI_CMDID_OPEN,
					  MC_CMD_OPEN_SIZE,
					  MC_CMD_PRI_LOW, 0);

	DPNI_CMD_OPEN(CMD_PREP);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		*dpni_handle = MC_CMD_HDR_READ_AUTHID(cmd.header);

	return err;
}

int dpni_close(struct fsl_mc_io *mc_io, uint16_t dpni_handle)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(MC_CMDID_CLOSE,
					  MC_CMD_CLOSE_SIZE,
					  MC_CMD_PRI_HIGH,
					  dpni_handle);

	return mc_send_command(mc_io, &cmd);
}

int dpni_destroy(struct fsl_mc_io *mc_io, uint16_t dpni_handle)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_DESTROY,
					  DPNI_CMDSZ_DESTROY,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	return mc_send_command(mc_io, &cmd);
}

int dpni_set_pools(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		   const struct dpni_pools_cfg *cfg)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_POOLS,
					  DPNI_CMDSZ_SET_POOLS,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_POOLS(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_set_irq(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		 uint8_t irq_index,
		 uint64_t irq_paddr,
		 uint32_t irq_val,
		 int user_irq_id)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_IRQ,
					  DPNI_CMDSZ_SET_IRQ,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_IRQ(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_set_tx_pause_frames(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			     uint8_t priority,
			     uint16_t pause_time,
			     uint16_t thresh_time)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_TX_PAUSE_FRAMES,
					  DPNI_CMDSZ_SET_TX_PAUSE_FRAMES,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_TX_PAUSE_FRAMES(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_set_rx_ignore_pause_frames(struct fsl_mc_io *mc_io,
				    uint16_t dpni_handle, int enable)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_RX_IGNORE_PAUSE_FRAMES,
					  DPNI_CMDSZ_SET_RX_IGNORE_PAUSE_FRAMES,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_RX_IGNORE_PAUSE_FRAMES(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_attach(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		const struct dpni_attach_cfg *cfg)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_ATTACH,
					  DPNI_CMDSZ_ATTACH,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_ATTACH(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_detach(struct fsl_mc_io *mc_io, uint16_t dpni_handle)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_DETACH,
					  DPNI_CMDSZ_DETACH,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	return mc_send_command(mc_io, &cmd);
}

int dpni_enable(struct fsl_mc_io *mc_io, uint16_t dpni_handle)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_ENABLE,
					  DPNI_CMDSZ_ENABLE,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	return mc_send_command(mc_io, &cmd);
}

int dpni_disable(struct fsl_mc_io *mc_io, uint16_t dpni_handle)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_DISABLE,
					  DPNI_CMDSZ_DISABLE,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	return mc_send_command(mc_io, &cmd);
}

int dpni_reset(struct fsl_mc_io *mc_io, uint16_t dpni_handle)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_RESET,
					  DPNI_CMDSZ_RESET,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	return mc_send_command(mc_io, &cmd);
}

int dpni_get_attributes(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			struct dpni_attr *attr)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_ATTR,
					  DPNI_CMDSZ_GET_ATTR,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_ATTR(RSP_READ_STRUCT);

	return err;
}

int dpni_get_rx_buffer_layout(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			      struct dpni_buffer_layout *layout)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_RX_BUFFER_LAYOUT,
					  DPNI_CMDSZ_GET_RX_BUFFER_LAYOUT,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_RX_BUFFER_LAYOUT(RSP_READ_STRUCT);

	return err;
}

int dpni_set_rx_buffer_layout(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			      const struct dpni_buffer_layout *layout)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_RX_BUFFER_LAYOUT,
					  DPNI_CMDSZ_SET_RX_BUFFER_LAYOUT,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_RX_BUFFER_LAYOUT(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_get_tx_buffer_layout(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			      struct dpni_buffer_layout *layout)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_TX_BUFFER_LAYOUT,
					  DPNI_CMDSZ_GET_TX_BUFFER_LAYOUT,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_TX_BUFFER_LAYOUT(RSP_READ_STRUCT);

	return err;
}

int dpni_set_tx_buffer_layout(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			      const struct dpni_buffer_layout *layout)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_TX_BUFFER_LAYOUT,
					  DPNI_CMDSZ_SET_TX_BUFFER_LAYOUT,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_TX_BUFFER_LAYOUT(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_get_tx_conf_buffer_layout(struct fsl_mc_io *mc_io,
				   uint16_t dpni_handle,
				   struct dpni_buffer_layout *layout)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_TX_CONF_BUFFER_LAYOUT,
					  DPNI_CMDSZ_GET_TX_CONF_BUFFER_LAYOUT,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_TX_CONF_BUFFER_LAYOUT(RSP_READ_STRUCT);

	return err;
}

int dpni_set_tx_conf_buffer_layout(struct fsl_mc_io *mc_io,
				   uint16_t dpni_handle,
				   const struct dpni_buffer_layout *layout)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_TX_CONF_BUFFER_LAYOUT,
					  DPNI_CMDSZ_SET_TX_CONF_BUFFER_LAYOUT,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_TX_CONF_BUFFER_LAYOUT(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_get_l3_chksum_validation(struct fsl_mc_io *mc_io,
				  uint16_t dpni_handle, int *en)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_L3_CHKSUM_VALIDATION,
					  DPNI_CMDSZ_GET_L3_CHKSUM_VALIDATION,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_L3_CHKSUM_VALIDATION(RSP_READ);

	return err;
}

int dpni_set_l3_chksum_validation(struct fsl_mc_io *mc_io,
				  uint16_t dpni_handle, int en)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_L3_CHKSUM_VALIDATION,
					  DPNI_CMDSZ_SET_L3_CHKSUM_VALIDATION,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_L3_CHKSUM_VALIDATION(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_get_l4_chksum_validation(struct fsl_mc_io *mc_io,
				  uint16_t dpni_handle, int *en)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_L4_CHKSUM_VALIDATION,
					  DPNI_CMDSZ_GET_L4_CHKSUM_VALIDATION,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_L4_CHKSUM_VALIDATION(RSP_READ);

	return err;
}

int dpni_set_l4_chksum_validation(struct fsl_mc_io *mc_io,
				  uint16_t dpni_handle, int en)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_L4_CHKSUM_VALIDATION,
					  DPNI_CMDSZ_SET_L4_CHKSUM_VALIDATION,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_L4_CHKSUM_VALIDATION(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_get_qdid(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		  uint16_t *qdid)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_QDID,
					  DPNI_CMDSZ_GET_QDID,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_QDID(RSP_READ);

	return err;
}

int dpni_get_spid(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		  uint16_t *spid)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_SPID,
					  DPNI_CMDSZ_GET_SPID,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_SPID(RSP_READ);

	return err;
}

int dpni_get_tx_data_offset(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			    uint16_t *data_offset)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_TX_DATA_OFFSET,
					  DPNI_CMDSZ_GET_TX_DATA_OFFSET,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_TX_DATA_OFFSET(RSP_READ);

	return err;
}

int dpni_get_counter(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		     enum dpni_counter counter,
		     uint64_t *value)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_COUNTER,
					  DPNI_CMDSZ_GET_COUNTER,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_GET_COUNTER(CMD_PREP);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_COUNTER(RSP_READ);

	return err;
}

int dpni_set_counter(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		     enum dpni_counter counter,
		     uint64_t value)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_COUNTER,
					  DPNI_CMDSZ_GET_COUNTER,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_COUNTER(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_get_link_state(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			int *up)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_LINK_STATE,
					  DPNI_CMDSZ_GET_LINK_STATE,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_LINK_STATE(RSP_READ);

	return err;
}

int dpni_set_mfl(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		 uint16_t mfl)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_MFL,
					  DPNI_CMDSZ_SET_MFL,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_MFL(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_get_mfl(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		 uint16_t *mfl)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_MFL,
					  DPNI_CMDSZ_GET_MFL,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_MFL(RSP_READ);

	return err;
}

int dpni_set_mtu(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		 uint16_t mtu)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_MTU,
					  DPNI_CMDSZ_SET_MTU,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_MTU(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_get_mtu(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		 uint16_t *mtu)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_MTU,
					  DPNI_CMDSZ_GET_MTU,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_MTU(RSP_READ);
	return err;
}

int dpni_set_multicast_promisc(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			       int en)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_MCAST_PROMISC,
					  DPNI_CMDSZ_SET_MCAST_PROMISC,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_MULTICAST_PROMISC(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_get_multicast_promisc(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			       int *en)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_MCAST_PROMISC,
					  DPNI_CMDSZ_GET_MCAST_PROMISC,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_MULTICAST_PROMISC(RSP_READ);

	return err;
}

int dpni_set_primary_mac_addr(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			      const uint8_t addr[6])
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_PRIM_MAC,
					  DPNI_CMDSZ_SET_PRIM_MAC,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_PRIMARY_MAC_ADDR(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_get_primary_mac_addr(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			      uint8_t addr[6])
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_PRIM_MAC,
					  DPNI_CMDSZ_GET_PRIM_MAC,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_PRIMARY_MAC_ADDR(RSP_READ_STRUCT);

	return err;
}

int dpni_add_mac_addr(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		      const uint8_t addr[6])
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_ADD_MAC_ADDR,
					  DPNI_CMDSZ_ADD_MAC_ADDR,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_ADD_MAC_ADDR(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_remove_mac_addr(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			 const uint8_t addr[6])
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_REMOVE_MAC_ADDR,
					  DPNI_CMDSZ_REMOVE_MAC_ADDR,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_REMOVE_MAC_ADDR(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_clear_mac_table(struct fsl_mc_io *mc_io, uint16_t dpni_handle)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_CLR_MAC_TBL,
					  DPNI_CMDSZ_CLR_MAC_TBL,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	return mc_send_command(mc_io, &cmd);
}

int dpni_add_vlan_id(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		     uint16_t vlan_id)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_ADD_VLAN_ID,
					  DPNI_CMDSZ_ADD_VLAN_ID,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_ADD_VLAN_ID(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_remove_vlan_id(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			uint16_t vlan_id)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_REMOVE_VLAN_ID,
					  DPNI_CMDSZ_REMOVE_VLAN_ID,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_REMOVE_VLAN_ID(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_clear_vlan_table(struct fsl_mc_io *mc_io, uint16_t dpni_handle)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_CLR_VLAN_TBL,
					  DPNI_CMDSZ_CLR_VLAN_TBL,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	return mc_send_command(mc_io, &cmd);
}

int dpni_set_tx_tc(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		   uint8_t tc_id,
		   const struct dpni_tx_tc_cfg *cfg)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_TX_TC,
					  DPNI_CMDSZ_SET_TX_TC,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_TX_TC(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

#if 0
int dpni_set_rx_tc(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		   uint8_t tc_id,
		   const struct dpni_rx_tc_cfg *cfg)
{
	struct mc_command cmd = { 0 };
	struct extract_data ext_data = { { 0 } };
	uint64_t ext_paddr = virt_to_phys(&ext_data);
	int err;

	err = build_extract_cfg_extention(cfg->extract_cfg, &ext_data);
	if (err)
		return err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_RX_TC,
					  DPNI_CMDSZ_SET_RX_TC,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_RX_TC(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}
#endif

int dpni_set_tx_flow(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		     uint16_t *flow_id,
		     const struct dpni_tx_flow_cfg *cfg)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_TX_FLOW,
					  DPNI_CMDSZ_SET_TX_FLOW,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_TX_FLOW(CMD_PREP);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_SET_TX_FLOW(RSP_READ_STRUCT);

	return err;
}

int dpni_get_tx_flow(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		     uint16_t flow_id,
		     struct dpni_tx_flow_cfg *cfg,
		     uint32_t *fqid)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_TX_FLOW,
					  DPNI_CMDSZ_GET_TX_FLOW,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_GET_TX_FLOW(CMD_PREP);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_TX_FLOW(RSP_READ_STRUCT);

	return err;
}

int dpni_set_rx_flow(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		     uint8_t tc_id,
		     uint16_t flow_id,
		     const struct dpni_rx_flow_cfg *cfg)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_RX_FLOW,
					  DPNI_CMDSZ_SET_RX_FLOW,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_RX_FLOW(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_get_rx_flow(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		     uint8_t tc_id,
		     uint16_t flow_id,
		     struct dpni_rx_flow_cfg *cfg,
		     uint32_t *fqid)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_RX_FLOW,
					  DPNI_CMDSZ_GET_RX_FLOW,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_GET_RX_FLOW(CMD_PREP);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_RX_FLOW(RSP_READ_STRUCT);

	return err;
}

#if 0
int dpni_set_qos_table(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		       const struct dpni_qos_tbl_cfg *cfg)
{
	struct mc_command cmd = { 0 };
	struct extract_data ext_data = { { 0 } };
	uint64_t ext_paddr = virt_to_phys(&ext_data);
	int err;

	err = build_extract_cfg_extention(cfg->extract_cfg, &ext_data);
	if (err)
		return err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_QOS_TBL,
					  DPNI_CMDSZ_SET_QOS_TBL,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_QOS_TABLE(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}
#endif

int dpni_delete_qos_table(struct fsl_mc_io *mc_io, uint16_t dpni_handle)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_DELETE_QOS_TBL,
					  DPNI_CMDSZ_DELETE_QOS_TBL,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	return mc_send_command(mc_io, &cmd);
}

#if 0
int dpni_add_qos_entry(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		       const struct dpni_key_cfg *cfg,
		       uint8_t tc_id)
{
	struct mc_command cmd = { 0 };
	uint64_t key_paddr, mask_paddr = 0;

	key_paddr = virt_to_phys(cfg->key);
	if (cfg->mask)
		mask_paddr = virt_to_phys(cfg->mask);

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_ADD_QOS_ENT,
					  DPNI_CMDSZ_ADD_QOS_ENT,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_ADD_QOS_ENTRY(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_remove_qos_entry(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			  const struct dpni_key_cfg *cfg)
{
	struct mc_command cmd = { 0 };
	uint64_t key_paddr, mask_paddr = 0;

	key_paddr = virt_to_phys(cfg->key);
	if (cfg->mask)
		mask_paddr = virt_to_phys(cfg->mask);

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_REMOVE_QOS_ENT,
					  DPNI_CMDSZ_REMOVE_QOS_ENT,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_REMOVE_QOS_ENTRY(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}
#endif

int dpni_clear_qos_table(struct fsl_mc_io *mc_io, uint16_t dpni_handle)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_CLR_QOS_TBL,
					  DPNI_CMDSZ_CLR_QOS_TBL,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	return mc_send_command(mc_io, &cmd);
}

int dpni_set_fs_table(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		      uint8_t tc_id,
		      const struct dpni_fs_tbl_cfg *cfg)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_FS_TBL,
					  DPNI_CMDSZ_SET_FS_TBL,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_FS_TABLE(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_delete_fs_table(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			 uint8_t tc_id)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_DELETE_FS_TBL,
					  DPNI_CMDSZ_DELETE_FS_TBL,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_DELETE_FS_TABLE(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

#if 0
int dpni_add_fs_entry(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		      uint8_t tc_id,
		      const struct dpni_key_cfg *cfg,
		      uint16_t flow_id)
{
	struct mc_command cmd = { 0 };
	uint64_t key_paddr, mask_paddr = 0;

	key_paddr = virt_to_phys(cfg->key);
	if (cfg->mask)
		mask_paddr = virt_to_phys(cfg->mask);

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_ADD_FS_ENT,
					  DPNI_CMDSZ_ADD_FS_ENT,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_ADD_FS_ENTRY(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_remove_fs_entry(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			 uint8_t tc_id,
			 const struct dpni_key_cfg *cfg)
{
	struct mc_command cmd = { 0 };
	uint64_t key_paddr, mask_paddr = 0;

	key_paddr = virt_to_phys(cfg->key);
	if (cfg->mask)
		mask_paddr = virt_to_phys(cfg->mask);

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_REMOVE_FS_ENT,
					  DPNI_CMDSZ_REMOVE_FS_ENT,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_REMOVE_FS_ENTRY(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}
#endif

int dpni_clear_fs_table(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			uint8_t tc_id)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_CLR_FS_TBL,
					  DPNI_CMDSZ_CLR_FS_TBL,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_CLEAR_FS_TABLE(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_get_irq(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		 uint8_t irq_index,
		 int *type,
		 uint64_t *irq_paddr,
		 uint32_t *irq_val,
		 int *user_irq_id)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_IRQ,
					  DPNI_CMDSZ_GET_IRQ,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_GET_IRQ(CMD_PREP);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_IRQ(RSP_READ);

	return err;
}

int dpni_get_irq_enable(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			uint8_t irq_index,
			uint8_t *enable_state)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_IRQ_ENABLE,
					  DPNI_CMDSZ_GET_IRQ_ENABLE,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_GET_IRQ_ENABLE(CMD_PREP);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_IRQ_ENABLE(RSP_READ);

	return err;
}

int dpni_set_irq_enable(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			uint8_t irq_index,
			uint8_t enable_state)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_IRQ_ENABLE,
					  DPNI_CMDSZ_SET_IRQ_ENABLE,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_IRQ_ENABLE(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_get_irq_mask(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		      uint8_t irq_index, uint32_t *mask)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_IRQ_MASK,
					  DPNI_CMDSZ_GET_IRQ_MASK,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_GET_IRQ_MASK(CMD_PREP);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_IRQ_MASK(RSP_READ);

	return err;
}

int dpni_set_irq_mask(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
		      uint8_t irq_index, uint32_t mask)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_IRQ_MASK,
					  DPNI_CMDSZ_SET_IRQ_MASK,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_IRQ_MASK(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_get_irq_status(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			uint8_t irq_index, uint32_t *status)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_IRQ_STATUS,
					  DPNI_CMDSZ_GET_IRQ_STATUS,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_GET_IRQ_STATUS(CMD_PREP);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_IRQ_STATUS(RSP_READ);

	return err;
}

int dpni_clear_irq_status(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			  uint8_t irq_index,
			  uint32_t status)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_CLEAR_IRQ_STATUS,
					  DPNI_CMDSZ_CLEAR_IRQ_STATUS,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_CLEAR_IRQ_STATUS(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpni_set_vlan_filters(struct fsl_mc_io *mc_io, uint16_t dpni_handle,
			  int en)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_VLAN_FILTERS,
					  DPNI_CMDSZ_SET_VLAN_FILTERS,
					  MC_CMD_PRI_LOW,
					  dpni_handle);

	DPNI_CMD_SET_VLAN_FILTERS(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

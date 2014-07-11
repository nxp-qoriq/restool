#include "fsl_mc_sys.h"
#include "fsl_mc_cmd.h"
#include "fsl_dprc.h"
#include "fsl_dprc_cmd.h"

#define CMD_PREP(_param, _offset, _width, _type, _arg) \
	(cmd.params[_param] |= u64_enc((_offset), (_width), (_arg)))

#define RSP_READ(_param, _offset, _width, _type, _arg) \
	(*(_arg) = (_type)u64_dec(cmd.params[_param], (_offset), (_width)))

#define RSP_READ_STRUCT(_param, _offset, _width, _type, _arg) \
	(_arg = (_type)u64_dec(cmd.params[_param], (_offset), (_width)))

int dprc_get_container_id(void *mc_io, int *container_id)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_CONT_ID,
					  DPRC_CMDSZ_GET_CONT_ID,
					  MC_CMD_PRI_LOW, 0);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPRC_RSP_GET_CONTAINER_ID(RSP_READ);

	return err;
}

int dprc_open(void *mc_io, int container_id, uint16_t *dprc_handle)
{
	int err;
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(MC_DPRC_CMDID_OPEN,
					  MC_CMD_OPEN_SIZE,
					  MC_CMD_PRI_LOW, 0);

	DPRC_CMD_OPEN(CMD_PREP);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		*dprc_handle = MC_CMD_HDR_READ_AUTHID(cmd.header);

	return err;
}

int dprc_close(void *mc_io, uint16_t dprc_handle)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(MC_CMDID_CLOSE,
					  MC_CMD_CLOSE_SIZE,
					  MC_CMD_PRI_HIGH,
					  dprc_handle);

	return mc_send_command(mc_io, &cmd);
}

int dprc_create_container(void *mc_io, uint16_t dprc_handle,
			  struct dprc_cfg *cfg,
			  int *child_container_id,
			  uint64_t *child_portal_paddr)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_CREATE_CONT,
					  DPRC_CMDSZ_CREATE_CONT,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_CREATE_CONTAINER(CMD_PREP);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPRC_RSP_CREATE_CONTAINER(RSP_READ);

	return err;
}

int dprc_destroy_container(void *mc_io, uint16_t dprc_handle,
			   int child_container_id)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_DESTROY_CONT,
					  DPRC_CMDSZ_DESTROY_CONT,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_DESTROY_CONTAINER(CMD_PREP);
	return mc_send_command(mc_io, &cmd);
}

int dprc_reset_container(void *mc_io, uint16_t dprc_handle,
			 int child_container_id)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_RESET_CONT,
					  DPRC_CMDSZ_RESET_CONT,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_RESET_CONTAINER(CMD_PREP);
	return mc_send_command(mc_io, &cmd);
}

int dprc_set_res_quota(void *mc_io, uint16_t dprc_handle,
		       int child_container_id,
	char *type,
	uint16_t quota)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_SET_RES_QUOTA,
					  DPRC_CMDSZ_SET_RES_QUOTA,
					  MC_CMD_PRI_LOW,
					  dprc_handle);
	DPRC_CMD_SET_RES_QUOTA(CMD_PREP);
	return mc_send_command(mc_io, &cmd);
}

int dprc_get_res_quota(void *mc_io, uint16_t dprc_handle,
		       int child_container_id,
		       char *type,
		       uint16_t *quota)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_RES_QUOTA,
					  DPRC_CMDSZ_GET_RES_QUOTA,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_GET_RES_QUOTA(CMD_PREP);
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPRC_RSP_GET_RES_QUOTA(RSP_READ);

	return err;
}

int dprc_assign(void *mc_io, uint16_t dprc_handle,
		int container_id,
		struct dprc_res_req *res_req)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_ASSIGN,
					  DPRC_CMDSZ_ASSIGN,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_ASSIGN(CMD_PREP);
	return mc_send_command(mc_io, &cmd);
}

int dprc_unassign(void *mc_io, uint16_t dprc_handle, int child_container_id,
		  struct dprc_res_req *res_req)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_UNASSIGN,
					  DPRC_CMDSZ_UNASSIGN,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_UNASSIGN(CMD_PREP);
	return mc_send_command(mc_io, &cmd);
}

int dprc_get_obj_count(void *mc_io, uint16_t dprc_handle, int *obj_count)
{
	int err;
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_OBJ_COUNT,
					  DPRC_CMDSZ_GET_OBJ_COUNT,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPRC_RSP_GET_OBJ_COUNT(RSP_READ);

	return err;
}

int dprc_get_obj(void *mc_io, uint16_t dprc_handle,
		 int obj_index,
		 struct dprc_obj_desc *obj_desc)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_OBJECT,
					  DPRC_CMDSZ_GET_OBJECT,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_GET_OBJECT(CMD_PREP);
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPRC_RSP_GET_OBJECT(RSP_READ_STRUCT);

	return err;
}

int dprc_get_res_count(void *mc_io, uint16_t dprc_handle, char *type,
		       int *res_count)
{
	struct mc_command cmd = { 0 };
	int err;

	*res_count = 0;

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_RES_COUNT,
					  DPRC_CMDSZ_GET_RES_COUNT,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_GET_RES_COUNT(CMD_PREP);
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPRC_RSP_GET_RES_COUNT(RSP_READ);

	return err;
}

int dprc_get_res_ids(void *mc_io, uint16_t dprc_handle,
		     char *type,
		     struct dprc_res_ids_range_desc *range_desc)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_RES_IDS,
					  DPRC_CMDSZ_GET_RES_IDS,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_GET_RES_IDS(CMD_PREP);
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPRC_RSP_GET_RES_IDS(RSP_READ_STRUCT);

	return err;
}

int dprc_get_attributes(void *mc_io, uint16_t dprc_handle,
			struct dprc_attributes *attr)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_ATTR,
					  DPRC_CMDSZ_GET_ATTR,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPRC_RSP_GET_ATTRIBUTES(RSP_READ_STRUCT);

	return err;
}

int dprc_get_obj_region(void *mc_io, uint16_t dprc_handle,
			char *obj_type,
			int obj_id,
			uint8_t region_index,
			struct dprc_region_desc *region_desc)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_OBJ_REG,
					  DPRC_CMDSZ_GET_OBJ_REG,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_GET_OBJ_REGION(CMD_PREP);
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPRC_RSP_GET_OBJ_REGION(RSP_READ_STRUCT);

	return err;
}

int dprc_get_irq(void *mc_io, uint16_t dprc_handle,
		 uint8_t irq_index,
		 int *type,
		 uint64_t *irq_paddr,
		 uint32_t *irq_val,
		 int *user_irq_id)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_IRQ,
					  DPRC_CMDSZ_GET_IRQ,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_GET_IRQ(CMD_PREP);
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPRC_RSP_GET_IRQ(RSP_READ);

	return err;
}

int dprc_set_irq(void *mc_io, uint16_t dprc_handle,
		 uint8_t irq_index,
		 uint64_t irq_paddr,
		 uint32_t irq_val,
		 int user_irq_id)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_SET_IRQ,
					  DPRC_CMDSZ_SET_IRQ,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_SET_IRQ(CMD_PREP);
	return mc_send_command(mc_io, &cmd);
}

int dprc_get_irq_enable(void *mc_io, uint16_t dprc_handle,
			uint8_t irq_index,
			uint8_t *enable_state)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_IRQ_ENABLE,
					  DPRC_CMDSZ_GET_IRQ_ENABLE,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_GET_IRQ_ENABLE(CMD_PREP);
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPRC_RSP_GET_IRQ_ENABLE(RSP_READ);

	return err;
}

int dprc_set_irq_enable(void *mc_io, uint16_t dprc_handle,
			uint8_t irq_index,
			uint8_t enable_state)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_SET_IRQ_ENABLE,
					  DPRC_CMDSZ_SET_IRQ_ENABLE,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_SET_IRQ_ENABLE(CMD_PREP);
	return mc_send_command(mc_io, &cmd);
}

int dprc_get_irq_mask(void *mc_io, uint16_t dprc_handle, uint8_t irq_index,
		      uint32_t *mask)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_IRQ_MASK,
					  DPRC_CMDSZ_GET_IRQ_MASK,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_GET_IRQ_MASK(CMD_PREP);
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPRC_RSP_GET_IRQ_MASK(RSP_READ);

	return err;
}

int dprc_set_irq_mask(void *mc_io, uint16_t dprc_handle, uint8_t irq_index,
		      uint32_t mask)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_SET_IRQ_MASK,
					  DPRC_CMDSZ_SET_IRQ_MASK,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_SET_IRQ_MASK(CMD_PREP);
	return mc_send_command(mc_io, &cmd);
}

int dprc_get_irq_status(void *mc_io, uint16_t dprc_handle, uint8_t irq_index,
			uint32_t *status)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_IRQ_STATUS,
					  DPRC_CMDSZ_GET_IRQ_STATUS,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_GET_IRQ_STATUS(CMD_PREP);
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPRC_RSP_GET_IRQ_STATUS(RSP_READ);

	return err;
}

int dprc_clear_irq_status(void *mc_io, uint16_t dprc_handle,
			  uint8_t irq_index, uint32_t status)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_CLEAR_IRQ_STATUS,
					  DPRC_CMDSZ_CLEAR_IRQ_STATUS,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_CLEAR_IRQ_STATUS(CMD_PREP);
	return mc_send_command(mc_io, &cmd);
}

int dprc_get_pool_count(void *mc_io, uint16_t dprc_handle, int *pool_count)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_POOL_COUNT,
					  DPRC_CMDSZ_GET_POOL_COUNT,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPRC_RSP_GET_POOL_COUNT(RSP_READ);

	return err;
}

int dprc_get_pool(void *mc_io, uint16_t dprc_handle, int pool_index,
		  char *type)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_POOL,
					  DPRC_CMDSZ_GET_POOL,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_GET_POOL(CMD_PREP);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPRC_RSP_GET_POOL(RSP_READ_STRUCT);

	return err;
}

int dprc_get_portal_paddr(void *mc_io, uint16_t dprc_handle,
			  int portal_id, uint64_t *portal_addr)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_GET_PORTAL_PADDR,
					  DPRC_CMDSZ_GET_PORTAL_PADDR,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_GET_PORTAL_PADDR(CMD_PREP);
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPRC_RSP_GET_PORTAL_PADDR(RSP_READ);

	return err;
}

int dprc_connect(void *mc_io, uint16_t dprc_handle,
		 struct dprc_endpoint *endpoint1,
		 struct dprc_endpoint *endpoint2)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_CONNECT,
					  DPRC_CMDSZ_CONNECT,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_CONNECT(CMD_PREP);
	return mc_send_command(mc_io, &cmd);
}

int dprc_disconnect(void *mc_io, uint16_t dprc_handle,
		    struct dprc_endpoint *endpoint)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPRC_CMDID_DISCONNECT,
					  DPRC_CMDSZ_DISCONNECT,
					  MC_CMD_PRI_LOW,
					  dprc_handle);

	DPRC_CMD_DISCONNECT(CMD_PREP);
	return mc_send_command(mc_io, &cmd);
}

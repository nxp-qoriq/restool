#include "fsl_mc_sys.h"
#include "fsl_mc_cmd.h"
#include "fsl_dpmng.h"
#include "fsl_dpmng_cmd.h"

#define CMD_PREP(_param, _offset, _width, _type, _arg) \
	(cmd.params[_param] |= u64_enc((_offset), (_width), (_arg)))

#define RSP_READ(_param, _offset, _width, _type, _arg) \
	(*(_arg) = (_type)u64_dec(cmd.params[_param], (_offset), (_width)))

#define RSP_READ_STRUCT(_param, _offset, _width, _type, _arg) \
	(_arg = (_type)u64_dec(cmd.params[_param], (_offset), (_width)))

int mc_get_version(void *mc_io, struct mc_version *mc_ver_info)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPMNG_CMDID_GET_VERSION,
					  DPMNG_CMDSZ_GET_VERSION,
					  MC_CMD_PRI_LOW, 0);

	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPMNG_RSP_GET_VERSION(RSP_READ_STRUCT);

	return err;
}

int dpmng_reset_aiop(void *mc_io, int aiop_tile_id)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPMNG_CMDID_RESET_AIOP,
					  DPMNG_CMDSZ_RESET_AIOP,
					  MC_CMD_PRI_LOW, 0);

	DPMNG_CMD_RESET_AIOP(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

#if 0
int dpmng_load_aiop(void *mc_io,
		    int aiop_tile_id,
	uint8_t *img_addr,
	int img_size)
{
	struct mc_command cmd = { 0 };
	uint64_t img_paddr = virt_to_phys(img_addr);

	cmd.header = mc_encode_cmd_header(DPMNG_CMDID_LOAD_AIOP,
					  DPMNG_CMDSZ_LOAD_AIOP,
					  MC_CMD_PRI_LOW, 0);

	DPMNG_CMD_LOAD_AIOP(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}

int dpmng_run_aiop(void *mc_io,
		   int aiop_tile_id,
		   uint32_t cores_mask,
		   uint64_t options)
{
	struct mc_command cmd = { 0 };

	cmd.header = mc_encode_cmd_header(DPMNG_CMDID_RUN_AIOP,
					  DPMNG_CMDSZ_RUN_AIOP,
					  MC_CMD_PRI_LOW, 0);
	DPMNG_CMD_RUN_AIOP(CMD_PREP);

	return mc_send_command(mc_io, &cmd);
}
#endif

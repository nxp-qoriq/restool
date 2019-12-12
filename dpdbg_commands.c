/* Copyright 2014-2016 Freescale Semiconductor Inc.
 * Copyright 2019 NXP Semiconductors.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <getopt.h>
#include "restool.h"
#include "utils.h"
#include "mc_v10/fsl_dpdbg.h"

enum mc_cmd_status mc_status;

enum dpdbg_info_options {
	INFO_OPT_HELP = 0,
};

static struct option dpdbg_info_options[] = {
	[INFO_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpdbg_info_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

enum dpdbg_dump_options {
	DUMP_OPT_HELP = 0,
	DUMP_OPT_MEM,
	DUMP_OPT_OBJECT,
};

static struct option dpdbg_dump_options[] = {
	[DUMP_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	[DUMP_OPT_MEM] = {
		.name = "memory",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	[DUMP_OPT_OBJECT] = {
		.name = "object",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpdbg_dump_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

enum dpdbg_set_options {
	SET_OPT_HELP = 0,
	SET_OPT_CONSOLE,
	SET_OPT_LOG,
	SET_OPT_LEVEL,
	SET_OPT_TIMESTAMP,
	SET_OPT_UART,
};

static struct option dpdbg_set_options[] = {
	[SET_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	[SET_OPT_CONSOLE] = {
		.name = "console",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[SET_OPT_LOG] = {
		.name = "log",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[SET_OPT_LEVEL] = {
		.name = "level",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[SET_OPT_TIMESTAMP] = {
		.name = "timestamp",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[SET_OPT_UART] = {
		.name = "uart",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpdbg_set_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

enum dpdbg_create_options {
	CREATE_OPT_HELP = 0,
};

static struct option dpdbg_create_options[] = {
	[CREATE_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpdbg_create_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

enum dpdbg_destroy_options {
	DESTROY_OPT_HELP = 0,
};

static struct option dpdbg_destroy_options[] = {
	[DESTROY_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpdbg_destroy_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);


static int cmd_dpdbg_help(void)
{
	static const char help_msg[] =
		"\n"
		"Usage: restool dpdbg <command> [--help] [ARGS...]\n"
		"Where <command> can be:\n"
		"    help - display all commands informations\n"
		"    info - displays detailed information about a DPDBG object.\n"
		"    create - create DPDBG object.\n"
		"    destroy - destroy DPDBG object.\n"
		"    dump - displays in MC console information about MC objects or memory usage.\n"
		"    set - set MC modules on or off.\n"
		"\n"
		"For command-specific help, use the --help option of each command.\n"
		"\n";

	printf(help_msg);
	return 0;
}

static int cmd_dpdbg_info(void)
{
	static const char usage_msg[] = "\nUsage: restool dpdbg info\n\n";

	uint32_t obj_id = 0;
	int error;
	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	bool found = false;
	uint16_t dpdbg_handle;
	struct dpdbg_attr dpdbg_attr;
	bool dpdbg_opened = false;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_HELP)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_HELP);
		error = 0;
		return error;
	}

	if (restool.obj_name != NULL) {
		ERROR_PRINTF("Unexpected argument: \'%s\'\n\n",
				restool.obj_name);
		puts(usage_msg);
		return -EINVAL;
	}

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
					restool.root_dprc_handle,
					0,
					obj_id,
					"dpdbg",
					&target_obj_desc,
					&target_parent_dprc_id,
					&found);
	if (error < 0)
		return error;

	if (strcmp(target_obj_desc.type, "dpdbg")) {
		printf("DPDBG object does not exist\n");
		return -EINVAL;
	}

	error = dpdbg_open_v10(&restool.mc_io, 0, obj_id, &dpdbg_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status),
				mc_status);
		goto out;
	}
	dpdbg_opened = true;

	if (dpdbg_handle == 0) {
		DEBUG_PRINTF("dpdbg_open() returned invalid handle (auth 0)\n");
		error = -ENOENT;
		goto out;
	}

	memset(&dpdbg_attr, 0, sizeof(dpdbg_attr));
	error = dpdbg_get_attributes_v10(&restool.mc_io,
			0,
			dpdbg_handle,
			&dpdbg_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status),
				mc_status);
		goto out;
	}

	printf("dpdbg id: %d\n", dpdbg_attr.id);
	printf("plugged state: %splugged\n",
			(target_obj_desc.state & DPRC_OBJ_STATE_PLUGGED)
			? ""
			: "un");
	print_obj_label(&target_obj_desc);

	error = 0;
out:
	if (dpdbg_opened) {
		int error2;

		error2 = dpdbg_close_v10(&restool.mc_io, 0, dpdbg_handle);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
					mc_status_to_string(mc_status),
					mc_status);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int dump(char *obj_type, uint32_t obj_id)
{
	uint32_t dpdbg_id = 0;
	uint16_t dpdbg_handle = 0;
	int error = 0;
	bool dpdbg_opened = false;

	error = dpdbg_open_v10(&restool.mc_io, 0, dpdbg_id, &dpdbg_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpdbg_opened = true;

	if (dpdbg_handle == 0) {
		DEBUG_PRINTF("dpdbg_open() returned invalid handle (auth 0)\n");
		error = -ENOENT;
		goto out;
	}

	error = dpdbg_dump_v10(&restool.mc_io,
			0,
			dpdbg_handle,
			obj_id,
			obj_type);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	error = 0;
out:
	if (dpdbg_opened) {
		int error2;

		error2 = dpdbg_close_v10(&restool.mc_io, 0, dpdbg_handle);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
					mc_status_to_string(mc_status),
					mc_status);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int cmd_dpdbg_dump(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpdbg dump --<option=[object]>\n"
		"where <option> can be:\n"
		"    memory - without other arguments\n"
		"    object - with object name and id as argument\n"
		"\n"
		"EXAMPLE:\n"
		"Display information about dpni.1:\n"
		"   $ restool dpdbg dump --object=dpni.5\n"
		"Display information about MC memory:\n"
		"   $ restool dpdbg dump --memory\n"

		"\n";

	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	bool found = false;
	int error = 0;
	uint32_t dpdbg_id = 0;
	uint32_t obj_id = -1;
	char obj_type[OBJ_TYPE_MAX_LENGTH + 1];

	if (restool.cmd_option_mask & ONE_BIT_MASK(DUMP_OPT_HELP)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(DUMP_OPT_HELP);
		return 0;
	}

	if (restool.obj_name != NULL) {
		ERROR_PRINTF("Unexpected argument: \'%s\'\n\n",
				restool.obj_name);
		puts(usage_msg);
		return -EINVAL;
	}

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
					restool.root_dprc_handle,
					0,
					dpdbg_id,
					"dpdbg",
					&target_obj_desc,
					&target_parent_dprc_id,
					&found);
	if (error < 0)
		return error;

	if (strcmp(target_obj_desc.type, "dpdbg")) {
		printf("DPDBG object does not exist\n");
		return -EINVAL;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(DUMP_OPT_MEM)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(DUMP_OPT_MEM);
		strcpy(obj_type, "mem");
		error = dump(obj_type, obj_id);
	} else if (restool.cmd_option_mask & ONE_BIT_MASK(DUMP_OPT_OBJECT)) {
		int n;

		restool.cmd_option_mask &= ~ONE_BIT_MASK(DUMP_OPT_OBJECT);
		n = sscanf(restool.cmd_option_args[DUMP_OPT_OBJECT],
				"%" STRINGIFY(OBJ_TYPE_MAX_LENGTH) "[a-z].%u",
				obj_type,
				&obj_id);
		if (n != 2) {
			ERROR_PRINTF("Invalid MC object name: %s\n",
				restool.cmd_option_args[DUMP_OPT_OBJECT]);
			puts(usage_msg);
			return -EINVAL;
		}
		error = dump(obj_type, obj_id);
	} else {
		ERROR_PRINTF("--option missing\n");
		puts(usage_msg);
		return -EINVAL;
	}

	return error;
}

static int set(char *module, uint32_t state)
{
	uint32_t dpdbg_id = 0;
	uint16_t dpdbg_handle = 0;
	int error = 0;
	bool dpdbg_opened = false;

	error = dpdbg_open_v10(&restool.mc_io, 0, dpdbg_id, &dpdbg_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpdbg_opened = true;

	if (dpdbg_handle == 0) {
		DEBUG_PRINTF("dpdbg_open() returned invalid handle (auth 0)\n");
		error = -ENOENT;
		goto out;
	}


	error = dpdbg_set_v10(&restool.mc_io, 0, dpdbg_handle, state, module);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	error = 0;

out:
	if (dpdbg_opened) {
		int error2;

		error2 = dpdbg_close_v10(&restool.mc_io, 0, dpdbg_handle);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
					mc_status_to_string(mc_status),
					mc_status);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int cmd_dpdbg_set(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpdbg set --<module=state>\n"
		"where <module> can be:\n"
		"    console - enable/disable mc_console, state is 1 or 0\n"
		"    log - enable/disable log printing, state is 1 or 0\n"
		"    timestamp - enable/disable timestamp printing, state is 1 or 0\n"
		"    level - change logging level from 0 to 5\n"
		"    uart - change uart ID of mc console from 0 to 4\n"
		"    1 = ON, 0 = OFF\n"
		"\n"
		"EXAMPLE:\n"
		"Set logging off:\n"
		"   $ restool dpdbg set --log=0\n"
		"Change logging level:\n"
		"   $ restool dpdbg set --level=0\n"
		"\n";

	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	bool found = false;
	int error = 0;
	uint32_t dpdbg_id = 0;
	uint32_t state = -1;

	if (restool.cmd_option_mask & ONE_BIT_MASK(SET_OPT_HELP)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(SET_OPT_HELP);
		return 0;
	}

	if (restool.obj_name != NULL) {
		ERROR_PRINTF("Unexpected argument: \'%s\'\n\n",
				restool.obj_name);
		puts(usage_msg);
		return -EINVAL;
	}

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
					restool.root_dprc_handle,
					0,
					dpdbg_id,
					"dpdbg",
					&target_obj_desc,
					&target_parent_dprc_id,
					&found);
	if (error < 0)
		return error;

	if (strcmp(target_obj_desc.type, "dpdbg")) {
		printf("dpdbg.0 does not exist\n");
		return -EINVAL;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(SET_OPT_CONSOLE)) {
		int n;
		char *module = "console";

		restool.cmd_option_mask &= ~ONE_BIT_MASK(SET_OPT_CONSOLE);
		n = sscanf(restool.cmd_option_args[SET_OPT_CONSOLE],
				"%u",
				&state);

		if (n != 1) {
			ERROR_PRINTF("Invalid MC console state: %s\n",
				restool.cmd_option_args[SET_OPT_CONSOLE]);
			puts(usage_msg);
			return -EINVAL;
		}

		error = set(module, state);

	} else if (restool.cmd_option_mask & ONE_BIT_MASK(SET_OPT_LOG)) {
		int n;
		char *module = "log_mode";

		restool.cmd_option_mask &= ~ONE_BIT_MASK(SET_OPT_LOG);
		n = sscanf(restool.cmd_option_args[SET_OPT_LOG],
				"%u",
				&state);

		if (n != 1) {
			ERROR_PRINTF("Invalid MC log state: %s\n",
					restool.cmd_option_args[SET_OPT_LOG]);
			puts(usage_msg);
			return -EINVAL;
		}

		error = set(module, state);

	} else if (restool.cmd_option_mask & ONE_BIT_MASK(SET_OPT_TIMESTAMP)) {
		int n;
		char *module = "timestamp";

		restool.cmd_option_mask &= ~ONE_BIT_MASK(SET_OPT_TIMESTAMP);
		n = sscanf(restool.cmd_option_args[SET_OPT_TIMESTAMP],
				"%u",
				&state);

		if (n != 1) {
			ERROR_PRINTF("Invalid MC timestamp state: %s\n",
				restool.cmd_option_args[SET_OPT_TIMESTAMP]);
			puts(usage_msg);
			return -EINVAL;
		}

		error = set(module, state);

	} else if (restool.cmd_option_mask & ONE_BIT_MASK(SET_OPT_LEVEL)) {
		int n;
		char *module = "log_level";

		restool.cmd_option_mask &= ~ONE_BIT_MASK(SET_OPT_LEVEL);
		n = sscanf(restool.cmd_option_args[SET_OPT_LEVEL],
				"%u",
				&state);

		if (n != 1) {
			ERROR_PRINTF("Invalid MC logging level: %s\n",
					restool.cmd_option_args[SET_OPT_LEVEL]);
			puts(usage_msg);
			return -EINVAL;
		}

		error = set(module, state);

	} else if (restool.cmd_option_mask & ONE_BIT_MASK(SET_OPT_UART)) {
		int n;
		char *module = "uart_id";

		restool.cmd_option_mask &= ~ONE_BIT_MASK(SET_OPT_UART);
		n = sscanf(restool.cmd_option_args[SET_OPT_UART],
				"%u",
				&state);

		if (n != 1) {
			ERROR_PRINTF("Invalid MC uart ID: %s\n",
					restool.cmd_option_args[SET_OPT_UART]);
			puts(usage_msg);
			return -EINVAL;
		}

		error = set(module, state);

	} else {
		ERROR_PRINTF("--option missing\n");
		puts(usage_msg);
		return -EINVAL;
	}

	return error;
}

static int cmd_dpdbg_destroy(void)
{
	static const char usage_msg[] = "\nUsage: restool dpdbg destroy\n\n";
	int error;
	uint32_t dpdbg_id = 0;

	if (restool.cmd_option_mask & ONE_BIT_MASK(DESTROY_OPT_HELP)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(DESTROY_OPT_HELP);
		return 0;
	}

	if (restool.obj_name != NULL) {
		ERROR_PRINTF("Unexpected argument: \'%s\'\n\n",
				restool.obj_name);
		puts(usage_msg);
		return -EINVAL;
	}

	if (in_use(restool.obj_name, "destroyed"))
		return -EBUSY;

	if (!find_obj("dpdbg", dpdbg_id))
		return -EINVAL;

	error = dpdbg_destroy_v10(&restool.mc_io,
			restool.root_dprc_handle,
			0,
			dpdbg_id);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status),
				mc_status);
	}

	printf("dpdbg.%u is destroyed\n", dpdbg_id);

	return error;
}

static int cmd_dpdbg_create(void)
{
	static const char usage_msg[] = "\nUsage: restool dpdbg create\n\n";
	int error;
	struct dpdbg_cfg dpdbg_cfg;
	uint32_t dpdbg_id;

	if (restool.mc_fw_version.major < 10 ||
			(restool.mc_fw_version.major >= 10 &&
			restool.mc_fw_version.minor < 20)) {
		ERROR_PRINTF("Unavailable feature\n");
		ERROR_PRINTF("MC version must be grater than 10.20.0\n");
		ERROR_PRINTF("Actual MC firmware version: %u.%u.%u\n",
				restool.mc_fw_version.major,
				restool.mc_fw_version.minor,
				restool.mc_fw_version.revision);

		return 1;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_HELP)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_HELP);
		return 0;
	}

	if (restool.obj_name != NULL) {
		ERROR_PRINTF("Unexpected argument: \'%s\'\n\n",
				restool.obj_name);
		puts(usage_msg);
		return -EINVAL;
	}

	dpdbg_cfg.dpdbg_container_id = restool.root_dprc_id;
	dpdbg_cfg.dpdbg_id = 0;

	error = dpdbg_create_v10(&restool.mc_io,
			restool.root_dprc_handle,
			0,
			&dpdbg_cfg,
			&dpdbg_id);
	if (error) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status), mc_status);
		return error;
	}

	print_new_obj("dpdbg", dpdbg_id, NULL);

	return 0;
}


struct object_command dpdbg_commands[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpdbg_help },

	{ .cmd_name = "info",
	  .options = dpdbg_info_options,
	  .cmd_func = cmd_dpdbg_info },

	{ .cmd_name = "dump",
	  .options = dpdbg_dump_options,
	  .cmd_func = cmd_dpdbg_dump },

	{ .cmd_name = "set",
	  .options = dpdbg_set_options,
	  .cmd_func = cmd_dpdbg_set },

	{ .cmd_name = "create",
	  .options = dpdbg_create_options,
	  .cmd_func = cmd_dpdbg_create },

	{ .cmd_name = "destroy",
	  .options = dpdbg_destroy_options,
	  .cmd_func = cmd_dpdbg_destroy },

	{ .cmd_name = NULL },
};


/*
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 * Author: Lijun Pan <Lijun.Pan@freescale.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation  and/or other materials provided with the distribution.
 * 3. Neither the names of the copyright holders nor the names of any
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
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
#include <sys/ioctl.h>
#include "restool.h"
#include "utils.h"
#include "mc_v8/fsl_dpseci.h"

enum mc_cmd_status mc_status;

/**
 * dpseci info command options
 */
enum dpseci_info_options {
	INFO_OPT_HELP = 0,
	INFO_OPT_VERBOSE,
};

static struct option dpseci_info_options[] = {
	[INFO_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	[INFO_OPT_VERBOSE] = {
		.name = "verbose",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpseci_info_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpseci create command options
 */
enum dpseci_create_options {
	CREATE_OPT_HELP = 0,
	CREATE_OPT_NUM_QUEUES,
	CREATE_OPT_PRIORITIES,
};

static struct option dpseci_create_options[] = {
	[CREATE_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_NUM_QUEUES] = {
		.name = "num-queues",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_PRIORITIES] = {
		.name = "priorities",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpseci_create_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpseci destroy command options
 */
enum dpseci_destroy_options {
	DESTROY_OPT_HELP = 0,
};

static struct option dpseci_destroy_options[] = {
	[DESTROY_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpseci_destroy_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static const struct flib_ops dpseci_ops = {
	.obj_open = dpseci_open,
	.obj_close = dpseci_close,
	.obj_get_irq_mask = dpseci_get_irq_mask,
	.obj_get_irq_status = dpseci_get_irq_status,
};

static int cmd_dpseci_help(void)
{
	static const char help_msg[] =
		"\n"
		"Usage: restool dpseci <command> [--help] [ARGS...]\n"
		"Where <command> can be:\n"
		"   info - displays detailed information about a DPSECI object.\n"
		"   create - creates a new child DPSECI under the root DPRC.\n"
		"   destroy - destroys a child DPSECI under the root DPRC.\n"
		"\n"
		"For command-specific help, use the --help option of each command.\n"
		"\n";

	printf(help_msg);
	return 0;
}

static int print_dpseci_attr(uint32_t dpseci_id,
			struct dprc_obj_desc *target_obj_desc)
{
	uint16_t dpseci_handle;
	int error;
	struct dpseci_attr dpseci_attr;
	bool dpseci_opened = false;
	struct dpseci_tx_queue_attr tx_attr;
	uint8_t *priorities;

	error = dpseci_open(&restool.mc_io, 0, dpseci_id, &dpseci_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpseci_opened = true;
	if (0 == dpseci_handle) {
		DEBUG_PRINTF(
			"dpseci_open() returned invalid handle (auth 0) for dpseci.%u\n",
			dpseci_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpseci_attr, 0, sizeof(dpseci_attr));
	error = dpseci_get_attributes(&restool.mc_io, 0, dpseci_handle,
					&dpseci_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpseci_id == (uint32_t)dpseci_attr.id);

	printf("dpseci version: %u.%u\n", dpseci_attr.version.major,
	       dpseci_attr.version.minor);
	printf("dpseci id: %d\n", dpseci_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	printf("number of transmit queues: %u\n", dpseci_attr.num_tx_queues);
	printf("number of receive queues: %u\n", dpseci_attr.num_rx_queues);

	priorities = malloc(dpseci_attr.num_tx_queues * sizeof(*priorities));
	if (priorities == NULL) {
		ERROR_PRINTF("malloc failed\n");
		error = -errno;
		goto out;
	}

	for (int i = 0; i < dpseci_attr.num_tx_queues; i++) {
		error = dpseci_get_tx_queue(&restool.mc_io, 0, dpseci_handle,
					    i, &tx_attr);

		if (error < 0) {
			mc_status = flib_error_to_mc_status(error);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
			free(priorities);
			goto out;
		}

		priorities[i] = tx_attr.priority;
	}
	printf("tx priorities: ");
	for (int i = 0; i < dpseci_attr.num_tx_queues-1; i++)
		printf("%d,", priorities[i]);

	printf("%d\n", priorities[dpseci_attr.num_tx_queues-1]);

	free(priorities);

	print_obj_label(target_obj_desc);

	error = 0;

out:
	if (dpseci_opened) {
		int error2;

		error2 = dpseci_close(&restool.mc_io, 0, dpseci_handle);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int print_dpseci_info(uint32_t dpseci_id)
{
	int error;
	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	bool found = false;

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
				restool.root_dprc_handle, 0, dpseci_id,
				"dpseci", &target_obj_desc,
				&target_parent_dprc_id, &found);
	if (error < 0)
		goto out;

	if (strcmp(target_obj_desc.type, "dpseci")) {
		printf("dpseci.%d does not exist\n", dpseci_id);
		return -EINVAL;
	}

	error = print_dpseci_attr(dpseci_id, &target_obj_desc);
	if (error < 0)
		goto out;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		error = print_obj_verbose(&target_obj_desc, &dpseci_ops);
	}

out:
	return error;
}

static int cmd_dpseci_info(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpseci info <dpseci-object> [--verbose]\n"
		"\n"
		"OPTIONS:\n"
		"--verbose\n"
		"   Shows extended/verbose information about the object\n"
		"\n"
		"EXAMPLE:\n"
		"Display information about dpseci.5:\n"
		"   $ restool dpseci info dpseci.5\n"
		"\n";

	uint32_t obj_id;
	int error;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_HELP)) {
		printf(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_HELP);
		error = 0;
		goto out;
	}

	if (restool.obj_name == NULL) {
		ERROR_PRINTF("<object> argument missing\n");
		printf(usage_msg);
		error = -EINVAL;
		goto out;
	}

	error = parse_object_name(restool.obj_name, "dpseci", &obj_id);
	if (error < 0)
		goto out;

	error = print_dpseci_info(obj_id);
out:
	return error;
}

static int parse_dpseci_priorities(char *priorities_str, uint8_t *priorities,
					int num)
{
	char *cursor = NULL;
	char *endptr;
	char *prio_str = strtok_r(priorities_str, ",", &cursor);
	int i = 0;
	long val;

	while (prio_str != NULL) {
		if (i >= num) {
			ERROR_PRINTF("Only supports %d priorities.\n", num);
			return -EINVAL;
		}

		errno = 0;
		val = strtol(prio_str, &endptr, 0);

		if (STRTOL_ERROR(prio_str, endptr, val, errno) ||
		    (val < 1 || val > 8)) {
			ERROR_PRINTF("Invalid priority value.\n");
			return -EINVAL;
		}

		priorities[i] = (uint8_t)val;
		prio_str = strtok_r(NULL, ",", &cursor);
		++i;
	}

	if (num != i) {
		ERROR_PRINTF("Please set %d priorities\n", num);
		return -EINVAL;
	}

	return 0;
}

static int cmd_dpseci_create(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpseci create --num-queues=<count> --priorities=<pri1,pri2,...>\n"
		"   --num-queues=<number of rx/tx queues>, ranges from 1 to 8\n"
		"   --priorities=<priority1,priority2, ...,priority8>\n"
		"      DPSECI supports num-queues priorities that can be individually set.\n"
		"      if --num-queues=3, then --priorities=X,Y,Z\n"
		"      Valid values for <priorityN> are 1-8.\n"
		"   --num-queues and --priorities must both be specified\n"
		"\n"
		"EXAMPLE:\n"
		"Create a DPSECI with 2 rx/tx queues, 2,4 priorities:\n"
		"   $ restool dpseci create --num-queues=2 --priorities=2,4\n"
		"\n";

	int error;
	struct dpseci_cfg dpseci_cfg = { 0 };
	uint16_t dpseci_handle;
	struct dpseci_attr dpseci_attr;
	long val;
	char *str;
	char *endptr;

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_HELP)) {
		printf(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_HELP);
		return 0;
	}

	if (restool.obj_name != NULL) {
		ERROR_PRINTF("Unexpected argument: \'%s\'\n\n",
			     restool.obj_name);
		printf(usage_msg);
		return -EINVAL;
	}

	if ((restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_NUM_QUEUES)) &&
	    (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_PRIORITIES))) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_NUM_QUEUES);
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_PRIORITIES);
		errno = 0;
		str = restool.cmd_option_args[CREATE_OPT_NUM_QUEUES];
		val = strtol(str, &endptr, 0);
		if (STRTOL_ERROR(str, endptr, val, errno) ||
		    (val < 1 || val > DPSECI_PRIO_NUM)) {
			ERROR_PRINTF("Invalid number of queues range\n");
			return -EINVAL;
		}
		dpseci_cfg.num_tx_queues = val;
		dpseci_cfg.num_rx_queues = val;

		error = parse_dpseci_priorities(
			restool.cmd_option_args[CREATE_OPT_PRIORITIES],
			dpseci_cfg.priorities, val);
		if (error < 0) {
			DEBUG_PRINTF(
				"parse_dpseci_priorities() failed with error %d, cannot get priorities.\n",
				error);
			return error;
		}
	} else {
		ERROR_PRINTF("--num-queues and --priorities must be set\n");
		printf(usage_msg);
		return -EINVAL;
	}

	error = dpseci_create(&restool.mc_io, 0, &dpseci_cfg, &dpseci_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	memset(&dpseci_attr, 0, sizeof(struct dpseci_attr));
	error = dpseci_get_attributes(&restool.mc_io, 0, dpseci_handle,
					&dpseci_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	print_new_obj("dpseci", dpseci_attr.id, NULL);

	error = dpseci_close(&restool.mc_io, 0, dpseci_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	return 0;
}

static int cmd_dpseci_destroy(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpseci destroy <dpseci-object>\n"
		"   e.g. restool dpseci destroy dpseci.9\n"
		"\n";

	int error;
	int error2;
	uint32_t dpseci_id;
	uint16_t dpseci_handle;
	bool dpseci_opened = false;

	if (restool.cmd_option_mask & ONE_BIT_MASK(DESTROY_OPT_HELP)) {
		printf(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(DESTROY_OPT_HELP);
		return 0;
	}

	if (restool.obj_name == NULL) {
		ERROR_PRINTF("<object> argument missing\n");
		printf(usage_msg);
		error = -EINVAL;
		goto out;
	}

	if (in_use(restool.obj_name, "destroyed")) {
		error = -EBUSY;
		goto out;
	}

	error = parse_object_name(restool.obj_name, "dpseci", &dpseci_id);
	if (error < 0)
		goto out;

	if (!find_obj("dpseci", dpseci_id)) {
		error = -EINVAL;
		goto out;
	}

	error = dpseci_open(&restool.mc_io, 0, dpseci_id, &dpseci_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpseci_opened = true;
	if (0 == dpseci_handle) {
		DEBUG_PRINTF(
			"dpseci_open() returned invalid handle (auth 0) for dpseci.%u\n",
			dpseci_id);
		error = -ENOENT;
		goto out;
	}

	error = dpseci_destroy(&restool.mc_io, 0, dpseci_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpseci_opened = false;
	printf("dpseci.%u is destroyed\n", dpseci_id);

out:
	if (dpseci_opened) {
		error2 = dpseci_close(&restool.mc_io, 0, dpseci_handle);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

struct object_command dpseci_commands[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpseci_help },

	{ .cmd_name = "info",
	  .options = dpseci_info_options,
	  .cmd_func = cmd_dpseci_info },

	{ .cmd_name = "create",
	  .options = dpseci_create_options,
	  .cmd_func = cmd_dpseci_create },

	{ .cmd_name = "destroy",
	  .options = dpseci_destroy_options,
	  .cmd_func = cmd_dpseci_destroy },

	{ .cmd_name = NULL },
};


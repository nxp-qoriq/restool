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
#include <limits.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include "resman.h"
#include "utils.h"
#include "fsl_dpdmux.h"

#define ALL_DPDMUX_OPTS (		\
	DPDMUX_OPT_REPLICATION_DIS |	\
	DPDMUX_OPT_BRIDGE_EN)

enum mc_cmd_status mc_status;

/**
 * dpdmux info command options
 */
enum dpdmux_info_options {
	INFO_OPT_HELP = 0,
	INFO_OPT_VERBOSE,
};

static struct option dpdmux_info_options[] = {
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

C_ASSERT(ARRAY_SIZE(dpdmux_info_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpdmux create command options
 */
enum dpdmux_create_options {
	CREATE_OPT_HELP = 0,
	CREATE_OPT_NUM_IFS,
	CREATE_OPT_CONTROL_IF,
	CREATE_OPT_METHOD,
	CREATE_OPT_MANIP,
	CREATE_OPT_OPTIONS,
	CREATE_OPT_MAX_DMAT_ENTRIES,
	CREATE_OPT_MAX_MC_GROUPS,
};

static struct option dpdmux_create_options[] = {
	[CREATE_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_NUM_IFS] = {
		.name = "num-ifs",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_CONTROL_IF] = {
		.name = "control-if",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_METHOD] = {
		.name = "method",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MANIP] = {
		.name = "manip",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_OPTIONS] = {
		.name = "options",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_DMAT_ENTRIES] = {
		.name = "max-dmat-entries",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_MC_GROUPS] = {
		.name = "max-mc-groups",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpdmux_create_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpdmux destroy command options
 */
enum dpdmux_destroy_options {
	DESTROY_OPT_HELP = 0,
};

static struct option dpdmux_destroy_options[] = {
	[DESTROY_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpdmux_destroy_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static const struct flib_ops dpdmux_ops = {
	.obj_open = dpdmux_open,
	.obj_close = dpdmux_close,
	.obj_get_irq_mask = dpdmux_get_irq_mask,
	.obj_get_irq_status = dpdmux_get_irq_status,
};

static int cmd_dpdmux_help(void)
{
	static const char help_msg[] =
		"\n"
		"resman dpdmux <command> [--help] [ARGS...]\n"
		"Where <command> can be:\n"
		"   info - displays detailed information about a DPDMUX object.\n"
		"   create - creates a new child DPDMUX under the root DPRC.\n"
		"   destroy - destroys a child DPDMUX under the root DPRC.\n"
		"\n"
		"For command-specific help, use the --help option of each command.\n"
		"\n";

	printf(help_msg);
	return 0;
}

static void print_dpdmux_options(uint64_t options)
{
	if (options == 0 || (options & ~ALL_DPDMUX_OPTS) != 0) {
		printf("\tUnrecognized options found...\n");
		return;
	}

	if (options & DPDMUX_OPT_REPLICATION_DIS)
		printf("\tDPDMUX_OPT_REPLICATION_DIS\n");

	if (options & DPDMUX_OPT_BRIDGE_EN)
		printf("\tDPDMUX_OPT_BRIDGE_EN\n");
}

static void print_dpdmux_method(enum dpdmux_method method)
{
	printf("DPDMUX address table method: ");
	switch (method) {
	case DPDMUX_METHOD_NONE:
		printf("DPDMUX_METHOD_NONE\n");
		break;
	case DPDMUX_METHOD_C_VLAN_MAC:
		printf("DPDMUX_METHOD_C_VLAN_MAC\n");
		break;
	case DPDMUX_METHOD_MAC:
		printf("DPDMUX_METHOD_MAX\n");
		break;
	case DPDMUX_METHOD_C_VLAN:
		printf("DPDMUX_METHOD_C_VLAN\n");
		break;
	case DPDMUX_METHOD_S_VLAN:
		printf("DPDMUX_METHOD_S_VLAN\n");
		break;
	default:
		assert(false);
		break;
	}
}

static void print_dpdmux_manip(enum dpdmux_manip manip)
{
	printf("DPDMUX manipulation type: ");
	switch (manip) {
	case DPDMUX_MANIP_NONE:
		printf("DPDMUX_MANIP_NONE\n");
		break;
	case DPDMUX_MANIP_ADD_REMOVE_S_VLAN:
		printf("DPDMUX_MANIP_ADD_REMOVE_S_VLAN\n");
		break;
	default:
		assert(false);
		break;
	}
}

static int print_dpdmux_attr(uint32_t dpdmux_id,
			struct dprc_obj_desc *target_obj_desc)
{
	uint16_t dpdmux_handle;
	int error;
	struct dpdmux_attr dpdmux_attr;
	bool dpdmux_opened = false;

	error = dpdmux_open(&resman.mc_io, dpdmux_id, &dpdmux_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpdmux_opened = true;
	if (0 == dpdmux_handle) {
		ERROR_PRINTF(
			"dpdmux_open() returned invalid handle (auth 0) for dpdmux.%u\n",
			dpdmux_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpdmux_attr, 0, sizeof(dpdmux_attr));
	error = dpdmux_get_attributes(&resman.mc_io, dpdmux_handle,
					&dpdmux_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpdmux_id == (uint32_t)dpdmux_attr.id);

	printf("dpdmux version: %u.%u\n", dpdmux_attr.version.major,
	       dpdmux_attr.version.minor);
	printf("dpdmux id: %d\n", dpdmux_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	printf("dpdmux_attr.options value is: %#llx\n",
	       (unsigned long long)dpdmux_attr.options);
	print_dpdmux_options(dpdmux_attr.options);
	print_dpdmux_method(dpdmux_attr.method);
	print_dpdmux_manip(dpdmux_attr.manip);
	printf("number of interfaces (excluding the uplink interface): %u\n",
		(uint32_t)dpdmux_attr.num_ifs);
	printf("DPDMUX frame storage memory size: %u\n",
		(uint32_t)dpdmux_attr.mem_size);
	printf("control interface ID: %u\n",
	       (uint32_t)dpdmux_attr.control_if);

	error = 0;

out:
	if (dpdmux_opened) {
		int error2;

		error2 = dpdmux_close(&resman.mc_io, dpdmux_handle);
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

static int print_dpdmux_info(uint32_t dpdmux_id)
{
	int error;
	struct dprc_obj_desc target_obj_desc;
	uint16_t target_parent_dprc_handle;
	bool found = false;

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(resman.root_dprc_handle, 0, dpdmux_id,
				"dpdmux", &target_obj_desc,
				&target_parent_dprc_handle, &found);
	if (error < 0)
		goto out;

	if (strcmp(target_obj_desc.type, "dpdmux")) {
		printf("dpdmux.%d does not exist\n", dpdmux_id);
		return -EINVAL;
	}

	error = print_dpdmux_attr(dpdmux_id, &target_obj_desc);
	if (error < 0)
		goto out;

	if (resman.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) {
		resman.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		error = print_obj_verbose(&target_obj_desc, &dpdmux_ops);
	}

out:
	return error;
}

static int cmd_dpdmux_info(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: resman dpdmux info <dpdmux-object> [--verbose]\n"
		"   e.g. resman dpdmux info dpdmux.7\n"
		"\n"
		"--verbose\n"
		"   Shows extended/verbose information about the object\n"
		"   e.g. resman dpdmux info dpdmux.7 --verbose\n"
		"\n";

	uint32_t obj_id;
	int error;

	if (resman.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_HELP)) {
		printf(usage_msg);
		resman.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_HELP);
		error = 0;
		goto out;
	}

	if (resman.obj_name == NULL) {
		ERROR_PRINTF("<object> argument missing\n");
		printf(usage_msg);
		error = -EINVAL;
		goto out;
	}

	error = parse_object_name(resman.obj_name, "dpdmux", &obj_id);
	if (error < 0)
		goto out;

	error = print_dpdmux_info(obj_id);
out:
	return error;
}

#define OPTION_MAP_ENTRY(_option)	{#_option, _option}

static int parse_dpdmux_create_options(char *options_str, uint64_t *options)
{
	static const struct {
		const char *str;
		uint64_t value;
	} options_map[] = {
		OPTION_MAP_ENTRY(DPDMUX_OPT_REPLICATION_DIS),
		OPTION_MAP_ENTRY(DPDMUX_OPT_BRIDGE_EN),
	};

	char *cursor = NULL;
	char *opt_str = strtok_r(options_str, ",", &cursor);
	uint64_t options_mask = 0;

	while (opt_str != NULL) {
		unsigned int i;

		for (i = 0; i < ARRAY_SIZE(options_map); ++i) {
			if (strcmp(opt_str, options_map[i].str) == 0) {
				options_mask |= options_map[i].value;
				break;
			}
		}

		if (i == ARRAY_SIZE(options_map)) {
			ERROR_PRINTF("Invalid option: '%s'\n", opt_str);
			return -EINVAL;
		}

		opt_str = strtok_r(NULL, ",", &cursor);
	}

	*options = options_mask;

	return 0;
}

static int parse_dpdmux_manip(char *manip_str, enum dpdmux_manip *manip)
{
	if (strcmp(manip_str, "DPDMUX_MANIP_NONE") == 0) {
		*manip = DPDMUX_MANIP_NONE;
		return 0;
	}

	if (strcmp(manip_str, "DPDMUX_MANIP_ADD_REMOVE_S_VLAN") == 0) {
		*manip = DPDMUX_MANIP_ADD_REMOVE_S_VLAN;
		return 0;
	}

	printf("Invalid dpdmux manip input.\n");
	return -EINVAL;
}


static int parse_dpdmux_method(char *method_str, enum dpdmux_method *method)
{
	if (strcmp(method_str, "DPDMUX_METHOD_NONE") == 0) {
		*method = DPDMUX_METHOD_NONE;
		return 0;
	}

	if (strcmp(method_str, "DPDMUX_METHOD_C_VLAN_MAC") == 0) {
		*method = DPDMUX_METHOD_C_VLAN_MAC;
		return 0;
	}

	if (strcmp(method_str, "DPDMUX_METHOD_MAC") == 0) {
		*method = DPDMUX_METHOD_MAC;
		return 0;
	}

	if (strcmp(method_str, "DPDMUX_METHOD_C_VLAN") == 0) {
		*method = DPDMUX_METHOD_C_VLAN;
		return 0;
	}

	if (strcmp(method_str, "DPDMUX_METHOD_S_VLAN") == 0) {
		*method = DPDMUX_METHOD_S_VLAN;
		return 0;
	}

	printf("Invalid dpdmux method input.\n");
	return -EINVAL;
}

static int create_dpdmux(const char *usage_msg)
{
	int error;
	struct dpdmux_cfg dpdmux_cfg;
	uint16_t dpdmux_handle;
	long val;
	char *str;
	char *endptr;
	struct dpdmux_attr dpdmux_attr;

	if (resman.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_HELP)) {
		printf(usage_msg);
		resman.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_HELP);
		return 0;
	}

	if (resman.obj_name != NULL) {
		ERROR_PRINTF("Unexpected argument: \'%s\'\n\n",
			     resman.obj_name);
		printf(usage_msg);
		return -EINVAL;
	}

	if (resman.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_OPTIONS)) {
		resman.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_OPTIONS);
		error = parse_dpdmux_create_options(
				resman.cmd_option_args[CREATE_OPT_OPTIONS],
				&dpdmux_cfg.adv.options);
		if (error < 0) {
			DEBUG_PRINTF(
				"parse_dpdmux_create_options() failed with error %d, cannot get options-mask\n",
				error);
			return error;
		}
	} else {
		dpdmux_cfg.adv.options = 0;
	}

	if (resman.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_METHOD)) {
		resman.cmd_option_mask &=
				~ONE_BIT_MASK(CREATE_OPT_METHOD);
		error = parse_dpdmux_method(
			resman.cmd_option_args[CREATE_OPT_METHOD],
			&dpdmux_cfg.method);
		if (error < 0) {
			DEBUG_PRINTF(
				"parse_dpdmux_method() failed with error %d, cannot get dpdmux method\n",
				error);
			return error;
		}
	} else {
		dpdmux_cfg.method = DPDMUX_METHOD_NONE;
	}

	if (resman.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_MANIP)) {
		resman.cmd_option_mask &=
				~ONE_BIT_MASK(CREATE_OPT_MANIP);
		error = parse_dpdmux_manip(
				resman.cmd_option_args[CREATE_OPT_MANIP],
				&dpdmux_cfg.manip);
		if (error < 0) {
			DEBUG_PRINTF(
				"parse_dpdmux_manip() failed with error %d, cannot get dpdmux manip\n",
				error);
			return error;
		}
	} else {
		dpdmux_cfg.manip = DPDMUX_MANIP_NONE;
	}

	if (resman.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_NUM_IFS)) {
		resman.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_NUM_IFS);
		errno = 0;
		str = resman.cmd_option_args[CREATE_OPT_NUM_IFS];
		val = strtol(str, &endptr, 0);

		if (STRTOL_ERROR(str, endptr, val, errno) ||
		    (val < 0 || val > UINT16_MAX)) {
			ERROR_PRINTF("Invalid number of interfaces\n");
			return -EINVAL;
		}

		dpdmux_cfg.num_ifs = val;
	} else {
		ERROR_PRINTF("--num-ifs option missing\n");
		printf(usage_msg);
		return -EINVAL;
	}

	if (resman.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_CONTROL_IF)) {
		resman.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_CONTROL_IF);
		errno = 0;
		str = resman.cmd_option_args[CREATE_OPT_CONTROL_IF];
		val = strtol(str, &endptr, 0);

		if (STRTOL_ERROR(str, endptr, val, errno) ||
		    (val < 0 || val > UINT8_MAX)) {
			ERROR_PRINTF("Invalid control interface\n");
			return -EINVAL;
		}

		dpdmux_cfg.control_if = val;
	} else {
		ERROR_PRINTF("--control-if option missing\n");
		printf(usage_msg);
		return -EINVAL;
	}

	if (resman.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_DMAT_ENTRIES)) {
		resman.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_DMAT_ENTRIES);
		errno = 0;
		str = resman.cmd_option_args[CREATE_OPT_MAX_DMAT_ENTRIES];
		val = strtol(str, &endptr, 0);

		if (STRTOL_ERROR(str, endptr, val, errno) ||
		    (val < 0 || val > UINT16_MAX)) {
			ERROR_PRINTF("Invalid max DPDMUX address table\n");
			return -EINVAL;
		}

		dpdmux_cfg.adv.max_dmat_entries = val;
	} else {
		dpdmux_cfg.adv.max_dmat_entries = 0;
	}

	if (resman.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_MC_GROUPS)) {
		resman.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_MC_GROUPS);
		errno = 0;
		str = resman.cmd_option_args[CREATE_OPT_MAX_MC_GROUPS];
		val = strtol(str, &endptr, 0);

		if (STRTOL_ERROR(str, endptr, val, errno) ||
		    (val < 0 || val > UINT16_MAX)) {
			ERROR_PRINTF(
			"Invalid max multicast group in DPDMUX table\n");
			return -EINVAL;
		}

		dpdmux_cfg.adv.max_mc_groups = val;
	} else {
		dpdmux_cfg.adv.max_mc_groups = 0;
	}

	error = dpdmux_create(&resman.mc_io, &dpdmux_cfg, &dpdmux_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	memset(&dpdmux_attr, 0, sizeof(struct dpdmux_attr));
	error = dpdmux_get_attributes(&resman.mc_io, dpdmux_handle,
					&dpdmux_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	printf("dpdmux.%d is created in dprc.1\n", dpdmux_attr.id);

	error = dpdmux_close(&resman.mc_io, dpdmux_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	return 0;
}

static int cmd_dpdmux_create(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: resman dpdmux create --num-ifs=<number> --control-if=<interface-num> [OPTIONS]\n"
		"\n"
		"ARGUMETNS:\n"
		"--num-ifs=<number>\n"
		"   Number of virtual interfaces (include the uplink interface)\n"
		"--control-if=<interface-num>\n"
		"   The initial control interface\n"
		"\n"
		"OPTIONS:\n"
		"--method=<dmat-method>\n"
		"   Where <dmat-method> defins the method of the DMux address table.\n"
		"   A valid value is one of the following:\n"
		"	DPDMUX_METHOD_NONE\n"
		"	DPDMUX_METHOD_C_VLAN_MAC\n"
		"	DPDMUX_METHOD_MAC\n"
		"	DPDMUX_METHOD_C_VLAN\n"
		"	DPDMUX_METHOD_S_VLAN\n"
		"--manip=<manip>\n"
		"   Where <manip> defines the DMux required manipulation operation.\n"
		"   A valid value is one of the following:\n"
		"	DPDMUX_MANIP_NONE\n"
		"	DPDMUX_MANIP_ADD_REMOVE_S_VLAN\n"
		"--options=<options-mask>\n"
		"   Where <options-mask> is a comma separated list of DPDMUX options:\n"
		"	DPDMUX_OPT_REPLICATION_DIS\n"
		"	DPDMUX_OPT_BRIDGE_EN\n"
		"   Default is 0\n"
		"--max-dmat-entries=<number>\n"
		"   Max entries in DMux address table. Default is 64.\n"
		"--max-mc-groups=<number>\n"
		"   Number of multicast groups in DMAT. Default is 32 groups.\n"
		"\n";

	return create_dpdmux(usage_msg);
}

static int cmd_dpdmux_destroy(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: resman dpdmux destroy <dpdmux-object>\n"
		"   e.g. resman dpdmux destroy dpdmux.9\n"
		"\n";

	int error;
	int error2;
	uint32_t dpdmux_id;
	uint16_t dpdmux_handle;
	bool dpdmux_opened = false;


	if (resman.cmd_option_mask & ONE_BIT_MASK(DESTROY_OPT_HELP)) {
		printf(usage_msg);
		resman.cmd_option_mask &= ~ONE_BIT_MASK(DESTROY_OPT_HELP);
		return 0;
	}

	if (resman.obj_name == NULL) {
		ERROR_PRINTF("<object> argument missing\n");
		printf(usage_msg);
		error = -EINVAL;
		goto out;
	}

	error = parse_object_name(resman.obj_name, "dpdmux", &dpdmux_id);
	if (error < 0)
		goto out;

	error = dpdmux_open(&resman.mc_io, dpdmux_id, &dpdmux_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpdmux_opened = true;
	if (0 == dpdmux_handle) {
		ERROR_PRINTF(
			"dpdmux_open() returned invalid handle (auth 0) for dpdmux.%u\n",
			dpdmux_id);
		error = -ENOENT;
		goto out;
	}

	error = dpdmux_destroy(&resman.mc_io, dpdmux_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpdmux_opened = false;

out:
	if (dpdmux_opened) {
		error2 = dpdmux_close(&resman.mc_io, dpdmux_handle);
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

struct object_command dpdmux_commands[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpdmux_help },

	{ .cmd_name = "info",
	  .options = dpdmux_info_options,
	  .cmd_func = cmd_dpdmux_info },

	{ .cmd_name = "create",
	  .options = dpdmux_create_options,
	  .cmd_func = cmd_dpdmux_create },

	{ .cmd_name = "destroy",
	  .options = dpdmux_destroy_options,
	  .cmd_func = cmd_dpdmux_destroy },

	{ .cmd_name = NULL },
};


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
#include "restool.h"
#include "utils.h"
#include "fsl_dpsw.h"

#define ALL_DPSW_OPTS (			\
	DPSW_OPT_FLOODING_DIS |		\
	DPSW_OPT_MULTICAST_DIS |	\
	DPSW_OPT_CTRL)

enum mc_cmd_status mc_status;

/**
 * dpsw info command options
 */
enum dpsw_info_options {
	INFO_OPT_HELP = 0,
	INFO_OPT_VERBOSE,
};

static struct option dpsw_info_options[] = {
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

C_ASSERT(ARRAY_SIZE(dpsw_info_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpsw create command options
 */
enum dpsw_create_options {
	CREATE_OPT_HELP = 0,
	CREATE_OPT_NUM_IFS,
	CREATE_OPT_OPTIONS,
	CREATE_OPT_MAX_VLANS,
	CREATE_OPT_MAX_FDBS,
	CREATE_OPT_MAX_FDB_ENTRIES,
	CREATE_OPT_FDB_AGING_TIME,
	CREATE_OPT_MAX_FDB_MC_GROUPS,
};

static struct option dpsw_create_options[] = {
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

	[CREATE_OPT_OPTIONS] = {
		.name = "options",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_VLANS] = {
		.name = "max-vlans",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_FDBS] = {
		.name = "max-fdbs",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_FDB_ENTRIES] = {
		.name = "max-fdb-entries",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_FDB_AGING_TIME] = {
		.name = "fdb-aging-time",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	[CREATE_OPT_MAX_FDB_MC_GROUPS] = {
		.name = "max-fdb-mc-groups",
		.has_arg = 1,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpsw_create_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpsw destroy command options
 */
enum dpsw_destroy_options {
	DESTROY_OPT_HELP = 0,
};

static struct option dpsw_destroy_options[] = {
	[DESTROY_OPT_HELP] = {
		.name = "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpsw_destroy_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static const struct flib_ops dpsw_ops = {
	.obj_open = dpsw_open,
	.obj_close = dpsw_close,
	.obj_get_irq_mask = dpsw_get_irq_mask,
	.obj_get_irq_status = dpsw_get_irq_status,
};

static int cmd_dpsw_help(void)
{
	static const char help_msg[] =
		"\n"
		"restool dpsw <command> [--help] [ARGS...]\n"
		"Where <command> can be:\n"
		"   info - displays detailed information about a DPSW object.\n"
		"   create - creates a new child DPSW under the root DPRC.\n"
		"   destroy - destroys a child DPSW under the root DPRC.\n"
		"\n"
		"For command-specific help, use the --help option of each command.\n"
		"\n";

	printf(help_msg);
	return 0;
}

static void print_dpsw_options(uint64_t options)
{
	if (options == 0 || (options & ~ALL_DPSW_OPTS) != 0) {
		printf("\tUnrecognized options found...\n");
		return;
	}

	if (options & DPSW_OPT_FLOODING_DIS)
		printf("\tDPSW_OPT_FLOODING_DIS\n");

	if (options & DPSW_OPT_MULTICAST_DIS)
		printf("\tDPSW_OPT_MULTICAST_DIS\n");

	if (options & DPSW_OPT_CTRL)
		printf("\tDPSW_OPT_CTRL\n");
}

static int print_dpsw_endpoint(uint32_t target_id,
				uint32_t target_parent_dprc_id,
				uint16_t num_ifs)
{
	struct dprc_endpoint endpoint1;
	struct dprc_endpoint endpoint2;
	uint16_t target_parent_dprc_handle;
	int state;
	int error = 0;
	int k;

	printf("endpoints:\n");
	for (k = 0; k < num_ifs; ++k) {
		memset(&endpoint1, 0, sizeof(struct dprc_endpoint));
		memset(&endpoint2, 0, sizeof(struct dprc_endpoint));
		memcpy(endpoint1.type, "dpsw", 5);
		endpoint1.id = target_id;
		endpoint1.interface_id = k;
		if (target_parent_dprc_id == restool.root_dprc_id)
			target_parent_dprc_handle = restool.root_dprc_handle;
		else {
			error = open_dprc(target_parent_dprc_id,
					&target_parent_dprc_handle);
			if (error < 0)
				return error;
		}
		error = dprc_get_connection(&restool.mc_io,
					target_parent_dprc_handle,
					&endpoint1,
					&endpoint2,
					&state);

		if (error == 0 && state == -1) {
			printf("\tinterface %d: No object associated\n", k);
		} else if (error == 0) {
			if (strcmp(endpoint2.type, "dpsw") == 0 ||
			    strcmp(endpoint2.type, "dpdmux") == 0) {
				printf("\tinterface %d: %s.%d.%d",
					k, endpoint2.type, endpoint2.id,
					endpoint2.interface_id);
			} else if (0 == endpoint2.interface_id) {
				printf("\tinterface %d: %s.%d",
					k, endpoint2.type, endpoint2.id);
			}

			if (1 == state)
				printf(", link is up\n");
			else if (0 == state)
				printf(", link is down\n");
			else
				printf(", link is in error state\n");

		} else {
			mc_status = flib_error_to_mc_status(error);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status), mc_status);
			return error;
		}

		if (target_parent_dprc_id != restool.root_dprc_id)
			return dprc_close(&restool.mc_io,
					target_parent_dprc_handle);
	}

	return 0;
}

static int print_dpsw_attr(uint32_t dpsw_id,
			struct dprc_obj_desc *target_obj_desc,
			uint32_t target_parent_dprc_id)
{
	uint16_t dpsw_handle;
	int error;
	struct dpsw_attr dpsw_attr;
	bool dpsw_opened = false;

	error = dpsw_open(&restool.mc_io, dpsw_id, &dpsw_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpsw_opened = true;
	if (0 == dpsw_handle) {
		DEBUG_PRINTF(
			"dpsw_open() returned invalid handle (auth 0) for dpsw.%u\n",
			dpsw_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpsw_attr, 0, sizeof(dpsw_attr));
	error = dpsw_get_attributes(&restool.mc_io, dpsw_handle, &dpsw_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(dpsw_id == (uint32_t)dpsw_attr.id);

	printf("dpsw version: %u.%u\n", dpsw_attr.version.major,
	       dpsw_attr.version.minor);
	printf("dpsw id: %d\n", dpsw_attr.id);
	printf("plugged state: %splugged\n",
		(target_obj_desc->state & DPRC_OBJ_STATE_PLUGGED) ? "" : "un");
	print_dpsw_endpoint(dpsw_id, target_parent_dprc_id,
				dpsw_attr.num_ifs);
	printf("dpsw_attr.options value is: %#llx\n",
	       (unsigned long long)dpsw_attr.options);
	print_dpsw_options(dpsw_attr.options);
	printf("max VLANs: %u\n", (uint32_t)dpsw_attr.max_vlans);
	printf("max FDBs: %u\n", (uint32_t)dpsw_attr.max_fdbs);
	printf("DPSW frame storage memory size: %u\n",
	       (uint32_t)dpsw_attr.mem_size);
	printf("number of interfaces: %u\n", (uint32_t)dpsw_attr.num_ifs);
	printf("current number of VLANs: %u\n", (uint32_t)dpsw_attr.num_vlans);
	printf("current number of FDBs: %u\n", (uint32_t)dpsw_attr.num_fdbs);
	print_obj_label(target_obj_desc);

	error = 0;

out:
	if (dpsw_opened) {
		int error2;

		error2 = dpsw_close(&restool.mc_io, dpsw_handle);
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

static int print_dpsw_info(uint32_t dpsw_id)
{
	int error;
	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	bool found = false;

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
				restool.root_dprc_handle, 0, dpsw_id,
				"dpsw", &target_obj_desc,
				&target_parent_dprc_id, &found);
	if (error < 0)
		goto out;

	if (strcmp(target_obj_desc.type, "dpsw")) {
		printf("dpsw.%d does not exist\n", dpsw_id);
		return -EINVAL;
	}

	error = print_dpsw_attr(dpsw_id, &target_obj_desc,
				target_parent_dprc_id);
	if (error < 0)
		goto out;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		error = print_obj_verbose(&target_obj_desc, &dpsw_ops);
	}

out:
	return error;
}

static int cmd_dpsw_info(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpsw info <dpsw-object> [--verbose]\n"
		"   e.g. restool dpsw info dpsw.2\n"
		"\n"
		"--verbose\n"
		"   Shows extended/verbose information about the object\n"
		"   e.g. restool dpsw info dpsw.2 --verbose\n"
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

	error = parse_object_name(restool.obj_name, "dpsw", &obj_id);
	if (error < 0)
		goto out;

	error = print_dpsw_info(obj_id);
out:
	return error;
}

#define OPTION_MAP_ENTRY(_option)	{#_option, _option}

static int parse_dpsw_create_options(char *options_str, uint64_t *options)
{
	static const struct {
		const char *str;
		uint64_t value;
	} options_map[] = {
		OPTION_MAP_ENTRY(DPSW_OPT_FLOODING_DIS),
		OPTION_MAP_ENTRY(DPSW_OPT_MULTICAST_DIS),
		OPTION_MAP_ENTRY(DPSW_OPT_CTRL),
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

static int create_dpsw(const char *usage_msg)
{
	int error;
	struct dpsw_cfg dpsw_cfg = {0};
	uint16_t dpsw_handle;
	long val;
	char *str;
	char *endptr;
	struct dpsw_attr dpsw_attr;

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

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_NUM_IFS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_NUM_IFS);
		errno = 0;
		str = restool.cmd_option_args[CREATE_OPT_NUM_IFS];
		val = strtol(str, &endptr, 0);

		if (STRTOL_ERROR(str, endptr, val, errno) ||
		    (val < 0 || val > UINT16_MAX)) {
			ERROR_PRINTF("Invalid number of interfaces\n");
			return -EINVAL;
		}

		dpsw_cfg.num_ifs = (uint16_t)val;
	} else {
		dpsw_cfg.num_ifs = 4; /* Todo: default value not defined */
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_OPTIONS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_OPTIONS);
		error = parse_dpsw_create_options(
				restool.cmd_option_args[CREATE_OPT_OPTIONS],
				&dpsw_cfg.adv.options);
		if (error < 0) {
			DEBUG_PRINTF(
				"parse_dpsw_create_options() failed with error %d, cannot get options-mask\n",
				error);
			return error;
		}
	} else { /* Todo: default option may change with spec */
		dpsw_cfg.adv.options = DPSW_OPT_FLOODING_DIS;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_MAX_VLANS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_MAX_VLANS);
		errno = 0;
		str = restool.cmd_option_args[CREATE_OPT_MAX_VLANS];
		val = strtol(str, &endptr, 0);

		if (STRTOL_ERROR(str, endptr, val, errno) ||
		    (val < 0 || val > UINT16_MAX)) {
			ERROR_PRINTF("Invalid max vlans\n");
			return -EINVAL;
		}

		dpsw_cfg.adv.max_vlans = (uint16_t)val;
	} else {
		dpsw_cfg.adv.max_vlans = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_FDBS)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_FDBS);
		errno = 0;
		str = restool.cmd_option_args[CREATE_OPT_MAX_FDBS];
		val = strtol(str, &endptr, 0);

		if (STRTOL_ERROR(str, endptr, val, errno) ||
		    (val < 0 || val > UINT8_MAX)) {
			ERROR_PRINTF("Invalid max FDB\n");
			return -EINVAL;
		}

		dpsw_cfg.adv.max_fdbs = (uint8_t)val;
	} else {
		dpsw_cfg.adv.max_fdbs = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_FDB_ENTRIES)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_FDB_ENTRIES);
		errno = 0;
		str = restool.cmd_option_args[CREATE_OPT_MAX_FDB_ENTRIES];
		val = strtol(str, &endptr, 0);

		if (STRTOL_ERROR(str, endptr, val, errno) ||
		    (val < 0 || val > UINT16_MAX)) {
			ERROR_PRINTF("Invalid number of FDB entries\n");
			return -EINVAL;
		}

		dpsw_cfg.adv.max_fdb_entries = (uint16_t)val;
	} else {
		dpsw_cfg.adv.max_fdb_entries = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_FDB_AGING_TIME)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_FDB_AGING_TIME);
		errno = 0;
		str = restool.cmd_option_args[CREATE_OPT_FDB_AGING_TIME];
		val = strtol(str, &endptr, 0);

		if (STRTOL_ERROR(str, endptr, val, errno) ||
		    (val < 0 || val > UINT16_MAX)) {
			ERROR_PRINTF("Invalid FDB aging time\n");
			return -EINVAL;
		}

		dpsw_cfg.adv.fdb_aging_time = (uint16_t)val;
	} else {
		dpsw_cfg.adv.fdb_aging_time = 0;
	}

	if (restool.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_FDB_MC_GROUPS)) {
		restool.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_FDB_MC_GROUPS);
		errno = 0;
		str = restool.cmd_option_args[CREATE_OPT_MAX_FDB_MC_GROUPS];
		val = strtol(str, &endptr, 0);

		if (STRTOL_ERROR(str, endptr, val, errno) ||
		    (val < 0 || val > UINT16_MAX)) {
			ERROR_PRINTF(
				"Invalid number of multicast groups in each FDB table\n");
			return -EINVAL;
		}

		dpsw_cfg.adv.max_fdb_mc_groups = (uint16_t)val;
	} else {
		dpsw_cfg.adv.max_fdb_mc_groups = 0;
	}

	error = dpsw_create(&restool.mc_io, &dpsw_cfg, &dpsw_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}

	memset(&dpsw_attr, 0, sizeof(struct dpsw_attr));
	error = dpsw_get_attributes(&restool.mc_io, dpsw_handle, &dpsw_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	printf("dpsw.%d is created under dprc.1\n", dpsw_attr.id);

	error = dpsw_close(&restool.mc_io, dpsw_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		return error;
	}
	return 0;
}

static int cmd_dpsw_create(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpsw create [OPTIONS]\n"
		"   e.g. create a DPSW object with all default options:\n"
		"	restool dpsw create\n"
		"\n"
		"OPTIONS:\n"
		"if options are not specified, create DPSW by default options\n"
		"--num-ifs=<number>\n"
		"	Number of external and internal interfaces.\n"
		"--options=<options-mask>\n"
		"   Where <options-mask> is a comma separated list of DPSW options:\n"
		"	DPSW_OPT_FLOODING_DIS\n"
		"	DPSW_OPT_MULTICAST_DIS\n"
		"	DPSW_OPT_CTRL\n"
		"--max-vlans=<number>\n"
		"	Maximum number of VLAN's. Default is 16.\n"
		"--max-fdbs=<number>\n"
		"	Maximum Number of FDB's. Default is 16.\n"
		"--max-fdb-entries=<number>\n"
		"	Number of FDB entries. Default is 1024;\n"
		"--fdb-aging-time=<number>\n"
		"	Default FDB aging time in seconds. Default is 300 seconds.\n"
		"--max-fdb-mc-groups=<number>\n"
		"	Number of multicast groups in each FDB table. Default is 32.\n"
		"\n";

	return create_dpsw(usage_msg);
}

static int cmd_dpsw_destroy(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dpsw destroy <dpsw-object>\n"
		"   e.g. restool dpsw destroy dpsw.8\n"
		"\n";

	int error;
	int error2;
	uint32_t dpsw_id;
	uint16_t dpsw_handle;
	bool dpsw_opened = false;

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

	error = parse_object_name(restool.obj_name, "dpsw", &dpsw_id);
	if (error < 0)
		goto out;

	error = dpsw_open(&restool.mc_io, dpsw_id, &dpsw_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpsw_opened = true;
	if (0 == dpsw_handle) {
		DEBUG_PRINTF(
			"dpsw_open() returned invalid handle (auth 0) for dpsw.%u\n",
			dpsw_id);
		error = -ENOENT;
		goto out;
	}

	error = dpsw_destroy(&restool.mc_io, dpsw_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpsw_opened = false;
	printf("dpsw.%u is destroyed\n", dpsw_id);

out:
	if (dpsw_opened) {
		error2 = dpsw_close(&restool.mc_io, dpsw_handle);
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

struct object_command dpsw_commands[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpsw_help },

	{ .cmd_name = "info",
	  .options = dpsw_info_options,
	  .cmd_func = cmd_dpsw_info },

	{ .cmd_name = "create",
	  .options = dpsw_create_options,
	  .cmd_func = cmd_dpsw_create },

	{ .cmd_name = "destroy",
	  .options = dpsw_destroy_options,
	  .cmd_func = cmd_dpsw_destroy },

	{ .cmd_name = NULL },
};


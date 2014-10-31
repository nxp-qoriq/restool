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
#include <stdbool.h>
#include <errno.h>
#include <assert.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include "resman.h"
#include "utils.h"
#include "fsl_dpsw.h"
#include "fsl_dprc.h"

#define ALL_DPSW_OPTS (			\
	DPSW_OPT_FLOODING_DIS |		\
	DPSW_OPT_BROADCAST_DIS |	\
	DPSW_OPT_MULTICAST_DIS |	\
	DPSW_OPT_TC_DIS |		\
	DPSW_OPT_CONTROL)

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

static int cmd_dpsw_help(void)
{
	static const char help_msg[] =
		"\n"
		"resman dpsw <command> [--help] [ARGS...]\n"
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

	if (options & DPSW_OPT_BROADCAST_DIS)
		printf("\tDPSW_OPT_BROADCAST_DIS\n");

	if (options & DPSW_OPT_MULTICAST_DIS)
		printf("\tDPSW_OPT_MULTICAST_DIS\n");

	if (options & DPSW_OPT_TC_DIS)
		printf("\tDPSW_OPT_TC_DIS\n");

	if (options & DPSW_OPT_CONTROL)
		printf("\tDPSW_OPT_CONTROL\n");
}

static int print_dpsw_attr(uint32_t dpsw_id)
{
	uint16_t dpsw_handle;
	int error;
	struct dpsw_attr dpsw_attr;
	bool dpsw_opened = false;

	error = dpsw_open(&resman.mc_io, dpsw_id, &dpsw_handle);
	if (error < 0) {
		ERROR_PRINTF("dpsw_open() failed for dpsw.%u with error %d\n",
			     dpsw_id, error);
		goto out;
	}
	dpsw_opened = true;
	if (0 == dpsw_handle) {
		ERROR_PRINTF(
			"dpsw_open() returned invalid handle (auth 0) for dpsw.%u\n",
			dpsw_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpsw_attr, 0, sizeof(dpsw_attr));
	error = dpsw_get_attributes(&resman.mc_io, dpsw_handle, &dpsw_attr);
	if (error < 0) {
		ERROR_PRINTF("dpsw_get_attributes() failed with error: %d\n",
			     error);
		goto out;
	}
	assert(dpsw_id == (uint32_t)dpsw_attr.id);

	printf("dpsw version: %u.%u\n", dpsw_attr.version.major,
	       dpsw_attr.version.minor);
	printf("dpsw id: %d\n", dpsw_attr.id);
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

	error = 0;

out:
	if (dpsw_opened) {
		int error2;

		error2 = dpsw_close(&resman.mc_io, dpsw_handle);
		if (error2 < 0) {
			ERROR_PRINTF("dpsw_close() failed with error %d\n",
				     error2);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int print_dpsw_verbose(uint16_t dprc_handle,
			      int nesting_level, uint32_t target_id)
{
	int num_child_devices;
	int error = 0;
	uint32_t irq_mask;
	uint32_t irq_status;

	assert(nesting_level <= MAX_DPRC_NESTING);

	error = dprc_get_obj_count(&resman.mc_io,
				   dprc_handle,
				   &num_child_devices);
	if (error < 0) {
		ERROR_PRINTF("dprc_get_object_count() failed with error %d\n",
			     error);
		goto out;
	}

	for (int i = 0; i < num_child_devices; i++) {
		struct dprc_obj_desc obj_desc;
		uint16_t child_dprc_handle;
		uint16_t dpsw_handle;
		int error2;

		error = dprc_get_obj(
				&resman.mc_io,
				dprc_handle,
				i,
				&obj_desc);
		if (error < 0) {
			ERROR_PRINTF(
				"dprc_get_object(%u) failed with error %d\n",
				i, error);
			goto out;
		}

		if (strcmp(obj_desc.type, "dpsw") == 0 &&
		    target_id == (uint32_t)obj_desc.id) {
			printf("plugged state: %splugged\n",
			       (obj_desc.state & DPRC_OBJ_STATE_PLUGGED) ?
			       "" : "un");
			printf("number of mappable regions: %u\n",
			       obj_desc.region_count);
			printf("number of interrupts: %u\n",
			       obj_desc.irq_count);

			error = dpsw_open(&resman.mc_io, target_id,
					  &dpsw_handle);
			if (error < 0) {
				ERROR_PRINTF(
					"dpsw_open() failed for dpsw.%u with error %d\n",
					target_id, error);
				goto out;
			}

			for (int j = 0; j < obj_desc.irq_count; j++) {
				dpsw_get_irq_mask(&resman.mc_io,
					dpsw_handle, j, &irq_mask);
				printf(
					"interrupt %d's mask: %#x\n",
					j, irq_mask);
				dpsw_get_irq_status(&resman.mc_io,
					dpsw_handle, j, &irq_status);
				(irq_status == 0) ?
				printf(
					"interrupt %d's status: %#x - no interrupt pending.\n",
				j, irq_status) :
				(irq_status == 1) ?
				printf(
					"interrupt %d's status: %#x - interrupt pending.\n",
					j, irq_status) :
				printf(
					"interrupt %d's status: %#x - error status.\n",
					j, irq_status);
			}

			error2 = dpsw_close(&resman.mc_io, dpsw_handle);
			if (error2 < 0) {
				ERROR_PRINTF(
					"dpsw_close() failed with error %d\n",
					error2);
				if (error == 0)
					error = error2;
			}
			goto out;
		} else if (strcmp(obj_desc.type, "dprc") == 0) {
			error = open_dprc(obj_desc.id, &child_dprc_handle);
			if (error < 0)
				goto out;

			error = print_dpsw_verbose(child_dprc_handle,
					  nesting_level + 1, target_id);

			error2 = dprc_close(&resman.mc_io, child_dprc_handle);
			if (error2 < 0) {
				ERROR_PRINTF(
					"dprc_close() failed with error %d\n",
					error2);
				if (error == 0)
					error = error2;

				goto out;
			}
		} else {
			continue;
		}
	}

out:
	return error;
}

static int print_dpsw_info(uint32_t dpsw_id)
{
	int error;

	error = print_dpsw_attr(dpsw_id);
	if (error < 0)
		goto out;

	if (resman.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) {
		resman.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		error = print_dpsw_verbose(resman.root_dprc_handle, 0, dpsw_id);
	}

out:
	return error;
}

static int cmd_dpsw_info(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: resman dpsw info <dpsw-object> [--verbose]\n"
		"   e.g. resman dpsw info dpsw.2\n"
		"\n"
		"--verbose\n"
		"   Shows extended/verbose information about the object\n"
		"   e.g. resman dpsw info dpsw.2 --verbose\n"
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

	error = parse_object_name(resman.obj_name, "dpsw", &obj_id);
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
		OPTION_MAP_ENTRY(DPSW_OPT_BROADCAST_DIS),
		OPTION_MAP_ENTRY(DPSW_OPT_MULTICAST_DIS),
		OPTION_MAP_ENTRY(DPSW_OPT_TC_DIS),
		OPTION_MAP_ENTRY(DPSW_OPT_CONTROL),
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
	struct dpsw_cfg dpsw_cfg;
	uint16_t dpsw_handle;
	long val;
	char *str;
	char *endptr;
	struct dpsw_attr dpsw_attr;

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

		dpsw_cfg.num_ifs = val;
	} else {
		dpsw_cfg.num_ifs = 0; /* Todo: default value not defined */
	}

	if (resman.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_OPTIONS)) {
		resman.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_OPTIONS);
		error = parse_dpsw_create_options(
				resman.cmd_option_args[CREATE_OPT_OPTIONS],
				&dpsw_cfg.adv.options);
		if (error < 0) {
			ERROR_PRINTF(
				"parse_dpsw_create_options() failed with error %d, cannot get options-mask\n",
				error);
			return error;
		}
	} else { /* Todo: default option may change with spec */
		dpsw_cfg.adv.options = DPSW_OPT_FLOODING_DIS |
				       DPSW_OPT_TC_DIS;
	}

	if (resman.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_MAX_VLANS)) {
		resman.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_MAX_VLANS);
		errno = 0;
		str = resman.cmd_option_args[CREATE_OPT_MAX_VLANS];
		val = strtol(str, &endptr, 0);

		if (STRTOL_ERROR(str, endptr, val, errno) ||
		    (val < 0 || val > UINT16_MAX)) {
			ERROR_PRINTF("Invalid max vlans\n");
			return -EINVAL;
		}

		dpsw_cfg.adv.max_vlans = val;
	} else {
		dpsw_cfg.adv.max_vlans = 0;
	}

	if (resman.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_FDBS)) {
		resman.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_FDBS);
		errno = 0;
		str = resman.cmd_option_args[CREATE_OPT_MAX_FDBS];
		val = strtol(str, &endptr, 0);

		if (STRTOL_ERROR(str, endptr, val, errno) ||
		    (val < 0 || val > UINT8_MAX)) {
			ERROR_PRINTF("Invalid max FDB\n");
			return -EINVAL;
		}

		dpsw_cfg.adv.max_fdbs = val;
	} else {
		dpsw_cfg.adv.max_fdbs = 0;
	}

	if (resman.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_FDB_ENTRIES)) {
		resman.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_FDB_ENTRIES);
		errno = 0;
		str = resman.cmd_option_args[CREATE_OPT_MAX_FDB_ENTRIES];
		val = strtol(str, &endptr, 0);

		if (STRTOL_ERROR(str, endptr, val, errno) ||
		    (val < 0 || val > UINT16_MAX)) {
			ERROR_PRINTF("Invalid number of FDB entries\n");
			return -EINVAL;
		}

		dpsw_cfg.adv.max_fdb_entries = val;
	} else {
		dpsw_cfg.adv.max_fdb_entries = 0;
	}

	if (resman.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_FDB_AGING_TIME)) {
		resman.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_FDB_AGING_TIME);
		errno = 0;
		str = resman.cmd_option_args[CREATE_OPT_FDB_AGING_TIME];
		val = strtol(str, &endptr, 0);

		if (STRTOL_ERROR(str, endptr, val, errno) ||
		    (val < 0 || val > UINT16_MAX)) {
			ERROR_PRINTF("Invalid FDB aging time\n");
			return -EINVAL;
		}

		dpsw_cfg.adv.fdb_aging_time = val;
	} else {
		dpsw_cfg.adv.fdb_aging_time = 0;
	}

	if (resman.cmd_option_mask &
	    ONE_BIT_MASK(CREATE_OPT_MAX_FDB_MC_GROUPS)) {
		resman.cmd_option_mask &=
			~ONE_BIT_MASK(CREATE_OPT_MAX_FDB_MC_GROUPS);
		errno = 0;
		str = resman.cmd_option_args[CREATE_OPT_MAX_FDB_MC_GROUPS];
		val = strtol(str, &endptr, 0);

		if (STRTOL_ERROR(str, endptr, val, errno) ||
		    (val < 0 || val > UINT16_MAX)) {
			ERROR_PRINTF(
				"Invalid number of multicast groups in each FDB table\n");
			return -EINVAL;
		}

		dpsw_cfg.adv.max_fdb_mc_groups = val;
	} else {
		dpsw_cfg.adv.max_fdb_mc_groups = 0;
	}

	error = dpsw_create(&resman.mc_io, &dpsw_cfg, &dpsw_handle);
	if (error < 0) {
		ERROR_PRINTF("dpsw_create() failed with error %d\n", error);
		return error;
	}

	memset(&dpsw_attr, 0, sizeof(struct dpsw_attr));
	error = dpsw_get_attributes(&resman.mc_io, dpsw_handle, &dpsw_attr);
	if (error < 0) {
		ERROR_PRINTF("dpsw_get_attributes() failed with error: %d\n",
			     error);
		return error;
	}
	printf("dpsw.%d is created in dprc.1\n", dpsw_attr.id);

	error = dpsw_close(&resman.mc_io, dpsw_handle);
	if (error < 0) {
		ERROR_PRINTF("dpsw_close() failed with error %d\n", error);
		return error;
	}
	return 0;
}

static int cmd_dpsw_create(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: resman dpsw create [OPTIONS]\n"
		"   e.g. create a DPSW object with all default options:\n"
		"	resman dpsw create\n"
		"\n"
		"OPTIONS:\n"
		"if options are not specified, create DPSW by default options\n"
		"--num-ifs=<number>\n"
		"	Number of external and internal interfaces.\n"
		"--options=<options-mask>\n"
		"   Where <options-mask> is a comma separated list of DPSW options:\n"
		"	DPSW_OPT_FLOODING_DIS\n"
		"	DPSW_OPT_BROADCAST_DIS\n"
		"	DPSW_OPT_MULTICAST_DIS\n"
		"	DPSW_OPT_TC_DIS\n"
		"	DPSW_OPT_CONTROL\n"
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
		"Usage: resman dpsw destroy <dpsw-object>\n"
		"   e.g. resman dpsw destroy dpsw.8\n"
		"\n";

	int error;
	int error2;
	uint32_t dpsw_id;
	uint16_t dpsw_handle;
	bool dpsw_opened = false;

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

	error = parse_object_name(resman.obj_name, "dpsw", &dpsw_id);
	if (error < 0)
		goto out;

	error = dpsw_open(&resman.mc_io, dpsw_id, &dpsw_handle);
	if (error < 0) {
		ERROR_PRINTF("dpsw_open() failed for dpsw.%u with error %d\n",
			     dpsw_id, error);
		goto out;
	}
	dpsw_opened = true;
	if (0 == dpsw_handle) {
		ERROR_PRINTF(
			"dpsw_open() returned invalid handle (auth 0) for dpsw.%u\n",
			dpsw_id);
		error = -ENOENT;
		goto out;
	}

	error = dpsw_destroy(&resman.mc_io, dpsw_handle);
	if (error < 0) {
		ERROR_PRINTF("dpsw_destroy() failed with error %d\n", error);
		goto out;
	}
	dpsw_opened = false;

out:
	if (dpsw_opened) {
		error2 = dpsw_close(&resman.mc_io, dpsw_handle);
		if (error2 < 0) {
			ERROR_PRINTF("dpsw_close() failed with error %d\n",
				     error2);
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


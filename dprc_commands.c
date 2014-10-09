/*
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 * Author: German Rivera <German.Rivera@freescale.com>
 * 	   Lijun Pan <Lijun.Pan@freescale.com>
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
#include <stdbool.h>
#include <errno.h>
#include <assert.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include "resman.h"
#include "utils.h"
#include "fsl_dprc.h"

#define ALL_DPRC_OPTS (				\
	DPRC_CFG_OPT_SPAWN_ALLOWED |		\
	DPRC_CFG_OPT_ALLOC_ALLOWED |		\
	DPRC_CFG_OPT_OBJ_CREATE_ALLOWED |	\
	DPRC_CFG_OPT_TOPOLOGY_CHANGES_ALLOWED |	\
	DPRC_CFG_OPT_IOMMU_BYPASS |		\
	DPRC_CFG_OPT_AIOP)

/**
 * Maximum level of nesting of DPRCs
 */
#define MAX_DPRC_NESTING	    16

/**
 * dprc list command options
 */
enum dprc_list_options {
	LIST_OPT_HELP = 0,
};

static struct option dprc_list_options[] = {
	[LIST_OPT_HELP] = {
		.name = "help",
	},
};

/**
 * dprc show command options
 */
enum dprc_show_options {
	SHOW_OPT_HELP = 0,
	SHOW_OPT_RESOURCES,
	SHOW_OPT_RES_TYPE,
	SHOW_OPT_VERBOSE,
};

static struct option dprc_show_options[] = {
	[SHOW_OPT_HELP] = {
		.name = "help",
	},

	[SHOW_OPT_RESOURCES] = {
		.name = "resources",
	},

	[SHOW_OPT_RES_TYPE] = {
		.name = "resource-type",
		.has_arg = 1,
	},

	[SHOW_OPT_VERBOSE] = {
		.name = "verbose",
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dprc_show_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dpio show command options
 */
enum dprc_info_options {
	INFO_OPT_HELP = 0,
	INFO_OPT_VERBOSE,
};

static struct option dprc_info_options[] = {
	[INFO_OPT_HELP] = {
		.name = "help",
	},

	[INFO_OPT_VERBOSE] = {
		.name = "verbose",
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dprc_info_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dprc create command options
 */
enum dprc_create_child_options {
	CREATE_OPT_HELP = 0,
	CREATE_OPT_OPTIONS,
};

static struct option dprc_create_child_options[] = {
	[CREATE_OPT_HELP] = {
		.name = "help",
	},

	[CREATE_OPT_OPTIONS] = {
		.name = "options",
		.has_arg = 1,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dprc_create_child_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dprc destroy command options
 */
enum dprc_destroy_options {
	DESTROY_OPT_HELP = 0,
};

static struct option dprc_destroy_options[] = {
	[DESTROY_OPT_HELP] = {
		.name = "help",
	},
};

C_ASSERT(ARRAY_SIZE(dprc_destroy_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dprc assign command options
 */
enum dprc_assign_options {
	ASSIGN_OPT_HELP = 0,
	ASSIGN_OPT_OBJECT,
	ASSIGN_OPT_TARGET,
	ASSIGN_OPT_RES_TYPE,
	ASSIGN_OPT_COUNT,
	ASSIGN_OPT_PLUGGED,
};

static struct option dprc_assign_options[] = {
	[ASSIGN_OPT_HELP] = {
		.name = "help",
	},

	[ASSIGN_OPT_OBJECT] = {
		.name = "object",
		.has_arg = 1,
	},

	[ASSIGN_OPT_TARGET] = {
		.name = "target",
		.has_arg = 1,
	},

	[ASSIGN_OPT_RES_TYPE] = {
		.name = "resource-type",
		.has_arg = 1,
	},

	[ASSIGN_OPT_COUNT] = {
		.name = "count",
		.has_arg = 1,
	},

	[ASSIGN_OPT_PLUGGED] = {
		.name = "plugged",
		.has_arg = 1,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dprc_assign_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dprc set-quota command options
 */
enum dprc_set_quota_options {
	SET_QUOTA_OPT_HELP = 0,
	SET_QUOTA_OPT_RES_TYPE,
	SET_QUOTA_OPT_COUNT,
	SET_QUOTA_OPT_CHILD,
};

static struct option dprc_set_quota_options[] = {
	[SET_QUOTA_OPT_HELP] = {
		.name = "help",
	},

	[SET_QUOTA_OPT_RES_TYPE] = {
		.name = "resource-type",
		.has_arg = 1,
	},

	[SET_QUOTA_OPT_COUNT] = {
		.name = "count",
		.has_arg = 1,
	},

	[SET_QUOTA_OPT_CHILD] = {
		.name = "child-container",
		.has_arg = 1,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dprc_set_quota_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dprc connect command options
 */
enum dprc_connect_options {
	CONNECT_OPT_HELP = 0,
	CONNECT_OPT_ENDPOINT1,
	CONNECT_OPT_ENDPOINT2,
};

static struct option dprc_connect_options[] = {
	[CONNECT_OPT_HELP] = {
		.name = "help",
	},

	[CONNECT_OPT_ENDPOINT1] = {
		.name = "endpoint1",
		.has_arg = 1,
	},

	[CONNECT_OPT_ENDPOINT2] = {
		.name = "endpoint2",
		.has_arg = 1,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dprc_connect_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dprc disconnect command options
 */
enum dprc_disconnect_options {
	DISCONNECT_OPT_HELP = 0,
	DISCONNECT_OPT_ENDPOINT,
};

static struct option dprc_disconnect_options[] = {
	[DISCONNECT_OPT_HELP] = {
		.name = "help",
	},

	[DISCONNECT_OPT_ENDPOINT] = {
		.name = "endpoint",
		.has_arg = 1,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dprc_disconnect_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static int cmd_dprc_help(void)
{
	static const char help_msg[] =
		"\n"
		"resman dprc <command>\n"
		"Where <command> can be:\n"
		"   list - lists all containers (DPRC objects) in the system.\n"
		"   show - displays the contents (objects and resources) of a DPRC object.\n"
		"   info - displays detailed information about a DPRC object.\n"
		"   create - creates a new child DPRC under the specified parent.\n"
		"   destroy - destroys a child DPRC under the specified parent.\n"
		"   assign - moves an object or resource from a parent container to a target container.\n"
		"   unassign - moves an object or resource from a target container to a parent container.\n"
		"   set-quota - sets quota policies for a child container, specifying the number of\n"
		"		resources a child may allocate from its parent container\n"
		"   connect - connects 2 objects, creating a link between them.\n"
		"   disconnect - removes the link between two objects. Either endpoint can be specified\n"
		"		 as the target of the operation.\n"
		"\n"
		"For command-specific help, use the --help option of each command.\n"
		"\n";

	printf(help_msg);
	return 0;
}

/**
 * Lists nested DPRCs inside a given DPRC, recursively
 */
static int list_dprc(uint32_t dprc_id, uint16_t dprc_handle,
		     int nesting_level, bool show_non_dprc_objects)
{
	int num_child_devices;
	int error = 0;

	assert(nesting_level <= MAX_DPRC_NESTING);

	for (int i = 0; i < nesting_level; i++)
		printf("  ");

	printf("dprc.%u\n", dprc_id);

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

		if (strcmp(obj_desc.type, "dprc") != 0) {
			if (show_non_dprc_objects) {
				for (int i = 0; i < nesting_level + 1; i++)
					printf("  ");

				printf("%s.%u\n", obj_desc.type, obj_desc.id);
			}

			continue;
		}

		error = open_dprc(obj_desc.id, &child_dprc_handle);
		if (error < 0)
			goto out;

		error = list_dprc(obj_desc.id, child_dprc_handle,
				  nesting_level + 1, show_non_dprc_objects);

		error2 = dprc_close(&resman.mc_io, child_dprc_handle);
		if (error2 < 0) {
			ERROR_PRINTF("dprc_close() failed with error %d\n",
				     error2);
			if (error == 0)
				error = error2;

			goto out;
		}
	}

out:
	return error;
}

static int cmd_dprc_list(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: resman dprc list\n"
		"\n";

	if (resman.cmd_option_mask & ONE_BIT_MASK(LIST_OPT_HELP)) {
		printf(usage_msg);
		resman.cmd_option_mask &= ~ONE_BIT_MASK(LIST_OPT_HELP);
		return 0;
	}

	if (resman.obj_name != NULL) {
		ERROR_PRINTF("Unexpected argument: \'%s\'\n\n", resman.obj_name);
		printf(usage_msg);
		return -EINVAL;
	}

	return list_dprc(resman.root_dprc_id, resman.root_dprc_handle, 0, false);
}

static int show_one_resource_type(uint16_t dprc_handle,
				      const char *mc_res_type)
{
	int res_count;
	int res_discovered_count;
	struct dprc_res_ids_range_desc range_desc;
	int error;

	error = dprc_get_res_count(&resman.mc_io, dprc_handle,
				   (char *)mc_res_type, &res_count);
	if (error < 0) {
		ERROR_PRINTF("dprc_get_res_count() failed: %d\n", error);
		goto out;
	}

	if (res_count == 0) {
		printf("Don't have any %s resource\n", mc_res_type);
		goto out;
	}

	memset(&range_desc, 0, sizeof(struct dprc_res_ids_range_desc));
	res_discovered_count = 0;
	do {
		int id;

		error = dprc_get_res_ids(&resman.mc_io, dprc_handle,
					 (char *)mc_res_type, &range_desc);
		if (error < 0) {
			ERROR_PRINTF("dprc_get_res_ids() failed: %d\n", error);
			goto out;
		}

		if (range_desc.base_id == range_desc.last_id)
			printf("%s.%d\n", mc_res_type, range_desc.base_id);
		else
			printf("%s.%d - %s.%d\n",
			       mc_res_type, range_desc.base_id,
			       mc_res_type, range_desc.last_id);

		for (id = range_desc.base_id; id <= range_desc.last_id; id++) {
			res_discovered_count++;
		}
	} while (res_discovered_count < res_count &&
		 range_desc.iter_status != DPRC_ITER_STATUS_LAST);
out:
	return error;
}

static int show_one_resource_type_count(uint16_t dprc_handle,
				      const char *mc_res_type)
{
	int res_count = -1;
	int error;

	error = dprc_get_res_count(&resman.mc_io, dprc_handle,
				   (char *)mc_res_type, &res_count);
	if (error < 0) {
		ERROR_PRINTF("dprc_get_res_count() failed for resource type \'%s\' (error: %d)\n",
			     mc_res_type, error);
		goto out;
	}

	assert(res_count >= 0);
	printf("%s: %d\n" ,mc_res_type, res_count);
out:
	return error;
}

/**
 * List resources of all types found in the container specified by dprc_handle
 */
static int show_mc_resources(uint16_t dprc_handle)
{
	int pool_count;
	char res_type[RES_TYPE_MAX_LENGTH + 1];
	int error;
	int ret_error = 0;

	error = dprc_get_pool_count(&resman.mc_io, dprc_handle,
				    &pool_count);
	if (error < 0) {
		ERROR_PRINTF("dprc_get_pool_count() failed: %d\n", error);
		goto out;
	}

	assert(pool_count > 0);
	for (int i = 0; i < pool_count; i++) {
		memset(res_type, 0, sizeof(res_type));
		error = dprc_get_pool(&resman.mc_io, dprc_handle,
				      i, res_type);

		/* check for buffer overrun: */
		assert(res_type[sizeof(res_type) - 1] == '\0');

		if (error < 0) {
			ERROR_PRINTF("dprc_get_pool() failed for pool index %d (error: %d)\n",
				     i, error);
			if (ret_error == 0)
				ret_error = error;

			continue;
		}

		DEBUG_PRINTF("pool index %d: ", i);
		error = show_one_resource_type_count(dprc_handle, res_type);
		if (error < 0) {
			if (ret_error == 0)
				ret_error = error;

			continue;
		}
	}
out:
	return ret_error;
}

static int show_mc_objects(uint16_t dprc_handle, const char *dprc_name)
{
	int num_child_devices;
	int error;
	bool verbose = false;

	if (resman.cmd_option_mask & ONE_BIT_MASK(SHOW_OPT_VERBOSE)) {
		resman.cmd_option_mask &= ~ONE_BIT_MASK(SHOW_OPT_VERBOSE);
		verbose = true;
	}

	error = dprc_get_obj_count(&resman.mc_io,
				   dprc_handle,
				   &num_child_devices);
	if (error < 0) {
		ERROR_PRINTF("dprc_get_object_count() failed with error %d\n",
			     error);
		goto out;
	}

	printf("%s contains %u objects%c\n", dprc_name, num_child_devices,
	       num_child_devices == 0 ? '.' : ':');

	for (int i = 0; i < num_child_devices; i++) {
		struct dprc_obj_desc obj_desc;

		error = dprc_get_obj(&resman.mc_io,
				     dprc_handle,
				     i,
				     &obj_desc);
		if (error < 0) {
			ERROR_PRINTF(
				"dprc_get_object(%u) failed with error %d\n",
				i, error);
			goto out;
		}

		if (strcmp(obj_desc.type, "dprc") == 0 || !verbose) {
			printf("%s.%u\n", obj_desc.type, obj_desc.id);
		} else {
			printf("%s.%u\t(%splugged)\n", obj_desc.type,
			       obj_desc.id,
			       (obj_desc.state & DPRC_OBJ_STATE_PLUGGED) ?
					"" : "un");
		}
	}

	error = 0;
out:
	return error;
}

static int cmd_dprc_show(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: resman dprc show <container>\n"
		"	resman dprc show <container> --resources\n"
		"	resman dprc show <container> --type=<resource-type>\n"
		"\n"
		"\n";

	uint32_t dprc_id;
	uint16_t dprc_handle;
	const char *dprc_name;
	int error;
	bool dprc_opened = false;
	const char *res_type;

	if (resman.cmd_option_mask & ONE_BIT_MASK(SHOW_OPT_HELP)) {
		printf(usage_msg);
		resman.cmd_option_mask &= ~ONE_BIT_MASK(SHOW_OPT_HELP);
		error = 0;
		goto out;
	}

	if (resman.obj_name == NULL) {
		ERROR_PRINTF("<object> argument missing\n");
		printf(usage_msg);
		error = -EINVAL;
		goto out;
	}

	dprc_name = resman.obj_name;
	if (strcmp(dprc_name, "mc.global") == 0)
		dprc_name = "dprc.0";

	error = parse_object_name(dprc_name, "dprc", &dprc_id);
	if (error < 0)
		goto out;

	if (dprc_id != resman.root_dprc_id) {
		error = open_dprc(dprc_id, &dprc_handle);
		if (error < 0)
			goto out;

		dprc_opened = true;
	} else {
		dprc_handle = resman.root_dprc_handle;
	}

	if (resman.cmd_option_mask & ONE_BIT_MASK(SHOW_OPT_RESOURCES)) {
		resman.cmd_option_mask &= ~ONE_BIT_MASK(SHOW_OPT_RESOURCES);
		error = show_mc_resources(dprc_handle);
	} else if (resman.cmd_option_mask & ONE_BIT_MASK(SHOW_OPT_RES_TYPE)) {
		assert(resman.cmd_option_args[SHOW_OPT_RES_TYPE] != NULL);
		res_type = resman.cmd_option_args[SHOW_OPT_RES_TYPE];
		resman.cmd_option_mask &= ~ONE_BIT_MASK(SHOW_OPT_RES_TYPE);
		error = show_one_resource_type(dprc_handle, res_type);
	} else {
		error = show_mc_objects(dprc_handle, dprc_name);
	}
out:
	if (dprc_opened) {
		int error2;

		error2 = dprc_close(&resman.mc_io, dprc_handle);
		if (error2 < 0) {
			ERROR_PRINTF("dprc_close() failed with error %d\n",
				     error2);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static void print_dprc_options(uint64_t options)
{
	if (options == 0 || (options & ~ALL_DPRC_OPTS) != 0) {
		printf("\tUnrecognized options found...\n");
		return;
	}

	if (options & DPRC_CFG_OPT_SPAWN_ALLOWED)
		printf("\tDPRC_CFG_OPT_SPAWN_ALLOWED\n");

	if (options & DPRC_CFG_OPT_ALLOC_ALLOWED)
		printf("\tDPRC_CFG_OPT_ALLOC_ALLOWED\n");

	if (options & DPRC_CFG_OPT_OBJ_CREATE_ALLOWED)
		printf("\tDPRC_CFG_OPT_OBJ_CREATE_ALLOWED\n");

	if (options & DPRC_CFG_OPT_TOPOLOGY_CHANGES_ALLOWED)
		printf("\tDPRC_CFG_OPT_TOPOLOGY_CHANGES_ALLOWED\n");

	if (options & DPRC_CFG_OPT_IOMMU_BYPASS)
		printf("\tDPRC_CFG_OPT_IOMMU_BYPASS\n");

	if (options & DPRC_CFG_OPT_AIOP)
		printf("\tDPRC_CFG_OPT_AIOP\n");
}

static int print_dprc_attr(uint32_t dprc_id)
{
	uint16_t dprc_handle;
	int error;
	struct dprc_attributes dprc_attr;
	bool dprc_opened = false;

	if (dprc_id != resman.root_dprc_id) {
		error = open_dprc(dprc_id, &dprc_handle);
		if (error < 0)
			goto out;

		dprc_opened = true;
	} else {
		dprc_handle = resman.root_dprc_handle;
	}

	memset(&dprc_attr, 0, sizeof(dprc_attr));
	error = dprc_get_attributes(&resman.mc_io, dprc_handle, &dprc_attr);
	if (error < 0) {
		ERROR_PRINTF("dprc_get_attributes() failed: %d\n", error);
		goto out;
	}

	assert(dprc_id == (uint32_t)dprc_attr.container_id);
	printf(
		"container id: %d\n"
		"icid: %u\n"
		"portal id: %d\n"
		"version: %u.%u\n"
		"dprc options: %#llx\n",
		dprc_attr.container_id,
		dprc_attr.icid,
		dprc_attr.portal_id,
		dprc_attr.version.major,
		dprc_attr.version.minor,
		(unsigned long long)dprc_attr.options);
	print_dprc_options(dprc_attr.options);

	error = 0;
out:
	if (dprc_opened) {
		int error2;

		error2 = dprc_close(&resman.mc_io, dprc_handle);
		if (error2 < 0) {
			ERROR_PRINTF("dprc_close() failed with error %d\n",
				     error2);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int print_dprc_verbose(uint16_t dprc_handle,
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

		if (strcmp(obj_desc.type, "dprc") != 0)
			continue;

		if (target_id == (uint32_t)obj_desc.id) {
			printf("number of mappable regions: %u\n",obj_desc.region_count);
			printf("number of interrupts: %u\n", obj_desc.irq_count);

			error = open_dprc(obj_desc.id, &child_dprc_handle);
			if (error < 0)
				goto out;

			for (int j = 0; j < obj_desc.irq_count; j++) {
				dprc_get_irq_mask(&resman.mc_io, child_dprc_handle, j, &irq_mask);
				printf("interrupt %d's mask: %#x\n", j, irq_mask);
				dprc_get_irq_status(&resman.mc_io, child_dprc_handle, j, &irq_status);
				(irq_status == 0) ?
				printf("interrupt %d's status: %#x - no interrupt pending.\n", j, irq_status) :
				(irq_status == 1) ?
				printf("interrupt %d's status: %#x - interrupt pending.\n", j, irq_status) :
				printf("interrupt %d's status: %#x - error status.\n", j, irq_status);
			}

			error2 = dprc_close(&resman.mc_io, child_dprc_handle);
			if (error2 < 0) {
				ERROR_PRINTF("dprc_close() failed with error %d\n",
					     error2);
				if (error == 0)
					error = error2;
			}
			goto out;
		}

		error = open_dprc(obj_desc.id, &child_dprc_handle);
		if (error < 0)
			goto out;

		error = print_dprc_verbose(child_dprc_handle,
					  nesting_level + 1, target_id);

		error2 = dprc_close(&resman.mc_io, child_dprc_handle);
		if (error2 < 0) {
			ERROR_PRINTF("dprc_close() failed with error %d\n",
				     error2);
			if (error == 0)
				error = error2;

			goto out;
		}
	}

out:
	return error;
}

static int print_dprc_info(uint32_t dprc_id)
{
	int error;

	error = print_dprc_attr(dprc_id);
	if (error < 0)
		goto out;

	if ((resman.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) &&
	    dprc_id == resman.root_dprc_id) {
		resman.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		printf("root dprc doesn't have verbose info to display\n");
		goto out;
	}

	if ((resman.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) &&
	    dprc_id != resman.root_dprc_id) {
		resman.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		error = print_dprc_verbose(resman.root_dprc_handle, 0, dprc_id);
		goto out;
	}
out:
	return error;
}

static int cmd_dprc_info(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: resman dprc info <dprc-object> [--verbose]\n"
		"\n"
		"--verbose\n"
		"   Shows extended/verbose information about the object \n"
		"\n";

	uint32_t dprc_id;
	const char *dprc_name;
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

	dprc_name = resman.obj_name;
	if (strcmp(dprc_name, "mc.global") == 0)
		dprc_name = "dprc.0";

	error = parse_object_name(dprc_name, "dprc", &dprc_id);
	if (error < 0)
		goto out;

	error = print_dprc_info(dprc_id);
out:
	return error;
}

/**
 * Create a DPRC object in the MC, as a child of the container
 * referred by 'dprc_handle'.
 */
static int create_child_dprc(uint16_t dprc_handle, uint64_t options)
{
	int error;
	int error2;
	struct dprc_cfg cfg;
	int child_dprc_id;
	uint64_t mc_portal_phys_addr;
	int32_t portal_id;
	bool portal_allocated = false;
	bool child_dprc_created = false;

	assert(dprc_handle != 0);
	error = ioctl(resman.mc_io.fd, RESMAN_ALLOCATE_MC_PORTAL,
		      &portal_id);
	if (error == -1) {
		error = -errno;
		ERROR_PRINTF(
			"ioctl(RESMAN_ALLOCATE_MC_PORTAL) failed with error %d\n",
			error);
		goto error;
	}

	portal_allocated = true;
	DEBUG_PRINTF("ioctl returned portal_id: %u\n", portal_id);

	cfg.icid = DPRC_GET_ICID_FROM_POOL;
	cfg.portal_id = portal_id;
	cfg.options = options;
	error = dprc_create_container(
			&resman.mc_io,
			dprc_handle,
			&cfg,
			&child_dprc_id,
			&mc_portal_phys_addr);
	if (error < 0) {
		ERROR_PRINTF("dprc_create_container() failed: %d\n", error);
		goto error;
	}

	child_dprc_created = true;
	printf("dprc.%u object created (using MC portal id %u, portal addr %#llx)\n",
	       child_dprc_id, portal_id, (unsigned long long)mc_portal_phys_addr);

	return 0;
error:
	if (child_dprc_created) {
		error2 = dprc_destroy_container(&resman.mc_io, dprc_handle,
						child_dprc_id);
		if (error2 < 0) {
			ERROR_PRINTF(
			    "dprc_destroy_container() failed with error %d\n",
			    error2);
		}
	}

	if (portal_allocated) {
		error2 = ioctl(resman.mc_io.fd, RESMAN_FREE_MC_PORTAL,
			       portal_id);
		if (error2 == -1) {
			error2 = -errno;
			ERROR_PRINTF(
				"ioctl(RESMAN_FREE_MC_PORTAL) failed with error %d\n",
				error2);
		}
	}

	return error;
}

#define OPTION_MAP_ENTRY(_option)   { #_option, _option }

static int parse_create_options(char *options_str, uint64_t *options)
{
	static const struct {
		const char *str;
		uint64_t value;
	} options_map[] = {
		OPTION_MAP_ENTRY(DPRC_CFG_OPT_SPAWN_ALLOWED),
		OPTION_MAP_ENTRY(DPRC_CFG_OPT_ALLOC_ALLOWED),
		OPTION_MAP_ENTRY(DPRC_CFG_OPT_OBJ_CREATE_ALLOWED),
		OPTION_MAP_ENTRY(DPRC_CFG_OPT_TOPOLOGY_CHANGES_ALLOWED),
		OPTION_MAP_ENTRY(DPRC_CFG_OPT_AIOP),
	};

	char *cursor = NULL;
	char *opt_str = strtok_r(options_str, ",", &cursor);
	uint64_t options_mask = 0;

	while (opt_str != NULL) {
		unsigned int i;

		for (i = 0; i < ARRAY_SIZE(options_map); i++) {
			if (strcmp(opt_str, options_map[i].str) == 0) {
				options_mask |= options_map[i].value;
				break;
			}
		}

		if (i == ARRAY_SIZE(options_map)) {
			ERROR_PRINTF("Invalid option: \'%s\'\n", opt_str);
			return -EINVAL;
		}

		opt_str = strtok_r(NULL, ",", &cursor);
	}

	*options = options_mask;
	return 0;
}

static int cmd_dprc_create_child(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: resman dprc create <parent-container> [--options=<options-mask>]\n"
		"\n"
		"--options=<options-mask>\n"
		"   Where <options-mask> is a comma separated list of DPRC options:\n"
		"	DPRC_CFG_OPT_SPAWN_ALLOWED\n"
		"	DPRC_CFG_OPT_ALLOC_ALLOWED\n"
		"	DPRC_CFG_OPT_OBJ_CREATE_ALLOWED\n"
		"	DPRC_CFG_OPT_TOPOLOGY_CHANGES_ALLOWED\n"
		"	DPRC_CFG_OPT_AIOP\n"
		"\n";

	uint16_t dprc_handle;
	int error;
	bool dprc_opened = false;
	uint32_t dprc_id;
	uint64_t options;

	if (resman.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_HELP)) {
		printf(usage_msg);
		resman.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_HELP);
		error = 0;
		goto out;
	}

	if (resman.obj_name == NULL) {
		ERROR_PRINTF("<object> argument missing\n");
		printf(usage_msg);
		error = -EINVAL;
		goto out;
	}

	error = parse_object_name(resman.obj_name,
				  "dprc", &dprc_id);
	if (error < 0)
		goto out;

	if (dprc_id != resman.root_dprc_id) {
		error = open_dprc(dprc_id, &dprc_handle);
		if (error < 0)
			goto out;

		dprc_opened = true;
	} else {
		dprc_handle = resman.root_dprc_handle;
	}

	if (resman.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_OPTIONS)) {
		resman.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_OPTIONS);
		error = parse_create_options(resman.cmd_option_args[CREATE_OPT_OPTIONS],
					     &options);
		if (error < 0)
			goto out;
	} else {
		options = DPRC_CFG_OPT_SPAWN_ALLOWED | DPRC_CFG_OPT_ALLOC_ALLOWED;
	}

	error = create_child_dprc(dprc_handle, options);
out:
	if (dprc_opened) {
		int error2;

		error2 = dprc_close(&resman.mc_io, dprc_handle);
		if (error2 < 0) {
			ERROR_PRINTF("dprc_close() failed with error %d\n",
				     error2);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int destroy_child_dprc(uint16_t parent_dprc_handle, int child_dprc_id)
{
	int error;
	uint16_t child_dprc_handle;
	struct dprc_attributes dprc_attr;
	bool dprc_opened = false;

	assert(parent_dprc_handle != 0);

	/*
	 * Before destroying the child container, get its MC portal id.
	 * We need to notify the fsl_mc_resman kernel driver that this
	 * MC portal has become available for reallocation, once we destroy it:
	 */

	error = open_dprc(child_dprc_id, &child_dprc_handle);
	if (error < 0)
		goto error;

	dprc_opened = true;
	memset(&dprc_attr, 0, sizeof(dprc_attr));
	error = dprc_get_attributes(&resman.mc_io, child_dprc_handle, &dprc_attr);
	if (error < 0) {
		ERROR_PRINTF("dprc_get_attributes() failed: %d\n", error);
		goto error;
	}

	assert(child_dprc_id == dprc_attr.container_id);
	dprc_opened = false;
	error = dprc_close(&resman.mc_io, child_dprc_handle);
	if (error < 0) {
		ERROR_PRINTF("dprc_close() failed with error %d\n", error);
		goto error;
	}

	/*
	 * Destroy child container in the MC:
	 */
	error = dprc_destroy_container(&resman.mc_io, parent_dprc_handle,
					child_dprc_id);
	if (error < 0) {
		ERROR_PRINTF(
		    "dprc_destroy_container() failed with error %d\n",
		    error);

		goto error;
	}

	printf("dprc.%u object destroyed\n", child_dprc_id);

	/*
	 * Tell the fsl_mc_resman kernel driver that the
	 * MC portal that was allocated for the destroyed child
	 * container can now be freed.
	 */
	error = ioctl(resman.mc_io.fd, RESMAN_FREE_MC_PORTAL,
		       dprc_attr.portal_id);
	if (error == -1) {
		error = -errno;
		ERROR_PRINTF(
			"ioctl(RESMAN_FREE_MC_PORTAL) failed with error %d\n",
			error);
		goto error;
	}

	DEBUG_PRINTF("Freed MC portal id %u\n", dprc_attr.portal_id);
	return 0;
error:
	if (dprc_opened) {
		int error2;

		error2 = dprc_close(&resman.mc_io, child_dprc_handle);
		if (error2 < 0) {
			ERROR_PRINTF(
				"dprc_close() failed with error %d\n", error2);
		}
	}

	return error;
}

static int cmd_dprc_destroy_child(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: resman dprc destroy <container>\n"
		"\n"
		"NOTE: <container> cannot be the root container (dprc.1)\n"
		"\n";

	int error;
	uint32_t dprc_id;

	if (resman.cmd_option_mask & ONE_BIT_MASK(DESTROY_OPT_HELP)) {
		printf(usage_msg);
		resman.cmd_option_mask &= ~ONE_BIT_MASK(DESTROY_OPT_HELP);
		error = 0;
		goto out;
	}

	if (resman.obj_name == NULL) {
		ERROR_PRINTF("<object> argument missing\n");
		error = -EINVAL;
		goto out;
	}

	error = parse_object_name(resman.obj_name,
				  "dprc", &dprc_id);
	if (error < 0)
		goto out;

	if (dprc_id == resman.root_dprc_id) {
		ERROR_PRINTF("The root DPRC (%s) cannot be destroyed\n",
			     resman.obj_name);
		error = -EINVAL;
		goto out;
	}

	/*
	 * TODO: Do we need to open the parent of the DPRC being destroyed
	 * in order to use the parent's handle in he dprc_destroy_container()
	 * call?
	 */
#if 0
	error = open_dprc(dprc_id, &dprc_handle);
	if (error < 0)
		goto out;

	dprc_opened = true;
#endif

	error = destroy_child_dprc(resman.root_dprc_handle, dprc_id);
out:
	return error;
}

static int do_dprc_assign_or_unassign(const char *usage_msg, bool do_assign)
{
	uint16_t dprc_handle;
	int error;
	bool dprc_opened = false;
	uint32_t parent_dprc_id;
	uint32_t target_dprc_id;
	struct dprc_res_req res_req;

	if (resman.cmd_option_mask & ONE_BIT_MASK(ASSIGN_OPT_HELP)) {
		printf(usage_msg);
		resman.cmd_option_mask &= ~ONE_BIT_MASK(ASSIGN_OPT_HELP);
		error = 0;
		goto out;
	}

	if (resman.obj_name == NULL) {
		ERROR_PRINTF("<parent-container> argument missing\n");
		printf(usage_msg);
		error = -EINVAL;
		goto out;
	}

	memset(&res_req, 0, sizeof(res_req));
	error = parse_object_name(resman.obj_name,
				  "dprc", &parent_dprc_id);
	if (error < 0)
		goto out;

	if (parent_dprc_id != resman.root_dprc_id) {
		error = open_dprc(parent_dprc_id, &dprc_handle);
		if (error < 0)
			goto out;

		dprc_opened = true;
	} else {
		dprc_handle = resman.root_dprc_handle;
	}

	if (resman.cmd_option_mask & ONE_BIT_MASK(ASSIGN_OPT_TARGET)) {
		resman.cmd_option_mask &= ~ONE_BIT_MASK(ASSIGN_OPT_TARGET);
		assert(resman.cmd_option_args[ASSIGN_OPT_TARGET] != NULL);
		error = parse_object_name(resman.cmd_option_args[ASSIGN_OPT_TARGET],
					  "dprc", &target_dprc_id);
		if (error < 0)
			goto out;
	} else {
		target_dprc_id = parent_dprc_id;
	}

	if (resman.cmd_option_mask & ONE_BIT_MASK(ASSIGN_OPT_RES_TYPE)) {
		resman.cmd_option_mask &= ~ONE_BIT_MASK(ASSIGN_OPT_RES_TYPE);
		assert(resman.cmd_option_args[ASSIGN_OPT_RES_TYPE] != NULL);
		strcpy(res_req.type, resman.cmd_option_args[ASSIGN_OPT_RES_TYPE]);
		if (strcmp(res_req.type, "mcp") == 0) {
			ERROR_PRINTF("resource type '%s' not supported\n",
				     res_req.type);
			error = -ENOTSUP;
			goto out;
		}

		if (!(resman.cmd_option_mask & ONE_BIT_MASK(ASSIGN_OPT_COUNT))) {
			ERROR_PRINTF("--count option missing\n");
			printf(usage_msg);
			error = -EINVAL;
			goto out;
		}

		assert(resman.cmd_option_args[ASSIGN_OPT_COUNT] != NULL);
		resman.cmd_option_mask &= ~ONE_BIT_MASK(ASSIGN_OPT_COUNT);
		res_req.num = atoi(resman.cmd_option_args[ASSIGN_OPT_COUNT]);
		if (res_req.num <= 0) {
			ERROR_PRINTF("Invalid --count arg: %s\n",
				     resman.cmd_option_args[ASSIGN_OPT_COUNT]);
			error = -EINVAL;
			goto out;
		}

		res_req.options = 0;
		res_req.id_base_align = 0;
	} else if (resman.cmd_option_mask & ONE_BIT_MASK(ASSIGN_OPT_OBJECT)) {
		int n;
		int state;

		resman.cmd_option_mask &= ~ONE_BIT_MASK(ASSIGN_OPT_OBJECT);
		assert(resman.cmd_option_args[ASSIGN_OPT_OBJECT] != NULL);

		n = sscanf(resman.cmd_option_args[ASSIGN_OPT_OBJECT],
			   "%" STRINGIFY(OBJ_TYPE_MAX_LENGTH) "s.%d",
			   res_req.type, &res_req.id_base_align);
		if (n != 2) {
			ERROR_PRINTF("Invalid --object arg: \'%s\'\n",
				     resman.cmd_option_args[ASSIGN_OPT_OBJECT]);

			error = -EINVAL;
			goto out;
		}

		res_req.options = DPRC_RES_REQ_OPT_EXPLICIT;
		if (!(resman.cmd_option_mask & ONE_BIT_MASK(ASSIGN_OPT_PLUGGED))) {
			ERROR_PRINTF("--plugged option missing\n");
			printf(usage_msg);
			error = -EINVAL;
			goto out;
		}

		resman.cmd_option_mask &= ~ONE_BIT_MASK(ASSIGN_OPT_PLUGGED);
		assert(resman.cmd_option_args[ASSIGN_OPT_PLUGGED] != NULL);
		state = atoi(resman.cmd_option_args[ASSIGN_OPT_PLUGGED]);
		if (state < 0 || state > 1) {
			ERROR_PRINTF("Invalid --plugged arg: \'%s\'\n",
			     resman.cmd_option_args[ASSIGN_OPT_PLUGGED]);
			error = -EINVAL;
			goto out;
		}

		if (state == 1)
			res_req.options |= DPRC_RES_REQ_OPT_PLUGGED;
	} else {
		ERROR_PRINTF("Invalid command line\n");
		printf(usage_msg);
		error = -EINVAL;
		goto out;
	}

	if (do_assign) {
		error = dprc_assign(&resman.mc_io,
				    dprc_handle,
				    target_dprc_id,
				    &res_req);
		if (error < 0)
			ERROR_PRINTF("dprc_assign() failed: %d\n", error);
	} else {
		error = dprc_unassign(&resman.mc_io,
				      dprc_handle,
				      target_dprc_id,
				      &res_req);
		if (error < 0)
			ERROR_PRINTF("dprc_unassign() failed: %d\n", error);
	}
out:
	if (dprc_opened) {
		int error2;

		error2 = dprc_close(&resman.mc_io, dprc_handle);
		if (error2 < 0) {
			ERROR_PRINTF("dprc_close() failed with error %d\n",
				     error2);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int cmd_dprc_assign(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: resman dprc assign <parent-container> --object=<object> [--target=<container>] --plugged=<state>\n"
		"	resman dprc assign <parent-container> --resource-type=<type> --count=<number> [--target=<container>]\n"
		"\n"
		"--object=<object>\n"
		"   Specifies the object to assign to the target container\n"
		"--target=<container>\n"
		"   Specifies the destination container for the operation.\n"
		"   Valid values are any child container. The target container\n"
		"   may be the same as the parent container, allowing assign to self.\n"
		"   Indeed, if this option is not specified, the default target is\n"
		"   <parent-container> itself.\n"
		"--plugged=<state>\n"
		"   Specifies the plugged state of the object (valid values are 0 or 1)\n"
		"--resource-type=<type>\n"
		"   String specifying the resource type to assign (e.g, \'mcp\', \'fq\', \'cg\', etc)\n"
		"--count=<number>\n"
		"   Number of resources to assign.\n"
		"\n";

	return do_dprc_assign_or_unassign(usage_msg, true);
}

static int cmd_dprc_unassign(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: resman dprc unassign <container> --object=<object> [--target=<container>] --plugged=<state>\n"
		"	resman dprc unassign <container> --resource-type <type> --count <number> --target <container>\n"
		"\n"
		"--object=<object>\n"
		"   Specifies the object to unassign from the target container\n"
		"--target=<container>\n"
		"   Specifies the destination container for the operation.\n"
		"   Valid values are any child container. The target container\n"
		"   may be the same as the parent container, allowing unassign to self.\n"
		"   Indeed, if this option is not specified, the default target is\n"
		"   <parent-container> itself.\n"
		"--plugged=<state>\n"
		"   Specifies the plugged state of the object (valid values are 0 or 1)\n"
		"--resource-type=<type>\n"
		"   String specifying the resource type to unassign (e.g, \'mcp\', \'fq\', \'cg\', etc)\n"
		"--count=<number>\n"
		"   Number of resources to unassign.\n"
		"\n";

	return do_dprc_assign_or_unassign(usage_msg, false);
}

static int cmd_dprc_set_quota(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: resman dprc set-quota <parent-container> --resource-type=<type> --count=<number>\n"
		"						 --child-container=<container>\n"
		"\n"
		"--resource-type=<type>\n"
		"   String specifying the resource type to set the quota for (e.g 'mcp', 'fq', 'cg', etc)\n"
		"--count=<number>\n"
		"   Max number of resources the child is able to allocate\n"
		"--child-container=<container>\n"
		"   Child container for which the quota is being set\n"
		"\n";

	uint16_t dprc_handle;
	int error;
	bool dprc_opened = false;
	uint32_t parent_dprc_id;
	uint32_t child_dprc_id;
	char *res_type;
	int quota;

	if (resman.cmd_option_mask & ONE_BIT_MASK(ASSIGN_OPT_HELP)) {
		printf(usage_msg);
		resman.cmd_option_mask &= ~ONE_BIT_MASK(ASSIGN_OPT_HELP);
		error = 0;
		goto out;
	}

	if (resman.obj_name == NULL) {
		ERROR_PRINTF("<parent-container> argument missing\n");
		printf(usage_msg);
		error = -EINVAL;
		goto out;
	}

	error = parse_object_name(resman.obj_name,
				  "dprc", &parent_dprc_id);
	if (error < 0)
		goto out;

	if (parent_dprc_id != resman.root_dprc_id) {
		error = open_dprc(parent_dprc_id, &dprc_handle);
		if (error < 0)
			goto out;

		dprc_opened = true;
	} else {
		dprc_handle = resman.root_dprc_handle;
	}

	if (!(resman.cmd_option_mask & ONE_BIT_MASK(SET_QUOTA_OPT_RES_TYPE))) {
		ERROR_PRINTF("--resource-type option missing\n");
		printf(usage_msg);
		error = -EINVAL;
		goto out;
	}

	resman.cmd_option_mask &= ~ONE_BIT_MASK(SET_QUOTA_OPT_RES_TYPE);
	assert(resman.cmd_option_args[SET_QUOTA_OPT_RES_TYPE] != NULL);
	res_type = resman.cmd_option_args[SET_QUOTA_OPT_RES_TYPE];

	if (!(resman.cmd_option_mask & ONE_BIT_MASK(SET_QUOTA_OPT_COUNT))) {
		ERROR_PRINTF("--count option missing\n");
		printf(usage_msg);
		error = -EINVAL;
		goto out;
	}

	assert(resman.cmd_option_args[SET_QUOTA_OPT_COUNT] != NULL);
	resman.cmd_option_mask &= ~ONE_BIT_MASK(SET_QUOTA_OPT_COUNT);
	quota = atoi(resman.cmd_option_args[SET_QUOTA_OPT_COUNT]);
	if (quota <= 0 || quota > UINT16_MAX) {
		ERROR_PRINTF("Invalid --count arg: %s\n",
			     resman.cmd_option_args[SET_QUOTA_OPT_COUNT]);
		error = -ERANGE;
		goto out;
	}

	if (!(resman.cmd_option_mask & ONE_BIT_MASK(SET_QUOTA_OPT_CHILD))) {
		ERROR_PRINTF("--child-container option missing\n");
		printf(usage_msg);
		error = -EINVAL;
		goto out;
	}

	resman.cmd_option_mask &= ~ONE_BIT_MASK(SET_QUOTA_OPT_CHILD);
	assert(resman.cmd_option_args[SET_QUOTA_OPT_CHILD] != NULL);
	error = parse_object_name(resman.cmd_option_args[SET_QUOTA_OPT_CHILD],
				  "dprc", &child_dprc_id);
	if (error < 0)
		goto out;

	error = dprc_set_res_quota(&resman.mc_io,
				   dprc_handle,
				   child_dprc_id,
				   res_type,
				   quota);

	if (error < 0)
		ERROR_PRINTF("dprc_set_res_quota() failed: %d\n", error);
out:
	if (dprc_opened) {
		int error2;

		error2 = dprc_close(&resman.mc_io, dprc_handle);
		if (error2 < 0) {
			ERROR_PRINTF("dprc_close() failed with error %d\n",
				     error2);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int parse_endpoint(char *endpoint_str, struct dprc_endpoint *endpoint)
{
	int n;

	memset(endpoint, 0, sizeof(*endpoint));

	n = sscanf(endpoint_str,
		   "%" STRINGIFY(OBJ_TYPE_MAX_LENGTH) "s.%d.%d",
		   endpoint->type, &endpoint->id, &endpoint->interface_id);

	if (n < 2)
		return -EINVAL;

	if (n == 2)
		assert(endpoint->interface_id == 0);

	return 0;
}


static int cmd_dprc_connect(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: resman dprc connect <container> --endpoint1=<object> --endpoint2=<object>\n"
		"\n"
		"--endpoint1=<object>\n"
		"   Specifies the first endpoint object.\n"
		"--endpoint2=<object>\n"
		"   Specifies the second endpoint object.\n"
		"\n";

	uint16_t dprc_handle;
	int error;
	bool dprc_opened = false;
	uint32_t parent_dprc_id;
	struct dprc_endpoint endpoint1;
	struct dprc_endpoint endpoint2;

	if (resman.cmd_option_mask & ONE_BIT_MASK(CONNECT_OPT_HELP)) {
		printf(usage_msg);
		resman.cmd_option_mask &= ~ONE_BIT_MASK(CONNECT_OPT_HELP);
		error = 0;
		goto out;
	}

	if (resman.obj_name == NULL) {
		ERROR_PRINTF("<parent-container> argument missing\n");
		printf(usage_msg);
		error = -EINVAL;
		goto out;
	}

	error = parse_object_name(resman.obj_name,
				  "dprc", &parent_dprc_id);
	if (error < 0)
		goto out;

	if (parent_dprc_id != resman.root_dprc_id) {
		error = open_dprc(parent_dprc_id, &dprc_handle);
		if (error < 0)
			goto out;

		dprc_opened = true;
	} else {
		dprc_handle = resman.root_dprc_handle;
	}

	if (!(resman.cmd_option_mask & ONE_BIT_MASK(CONNECT_OPT_ENDPOINT1))) {
		ERROR_PRINTF("--endpoint1 option missing\n");
		printf(usage_msg);
		error = -EINVAL;
		goto out;
	}

	resman.cmd_option_mask &= ~ONE_BIT_MASK(CONNECT_OPT_ENDPOINT1);
	assert(resman.cmd_option_args[CONNECT_OPT_ENDPOINT1] != NULL);
	error = parse_endpoint(resman.cmd_option_args[CONNECT_OPT_ENDPOINT1], &endpoint1);
	if (error < 0) {
		ERROR_PRINTF("Invalid --endpoint1 arg: '%s'\n",
			     resman.cmd_option_args[CONNECT_OPT_ENDPOINT1]);
		goto out;
	}

	if (!(resman.cmd_option_mask & ONE_BIT_MASK(CONNECT_OPT_ENDPOINT2))) {
		ERROR_PRINTF("--endpoint2 option missing\n");
		printf(usage_msg);
		error = -EINVAL;
		goto out;
	}

	resman.cmd_option_mask &= ~ONE_BIT_MASK(CONNECT_OPT_ENDPOINT2);
	assert(resman.cmd_option_args[CONNECT_OPT_ENDPOINT2] != NULL);
	error = parse_endpoint(resman.cmd_option_args[CONNECT_OPT_ENDPOINT2], &endpoint2);
	if (error < 0) {
		ERROR_PRINTF("Invalid --endpoint2 arg: '%s'\n",
			     resman.cmd_option_args[CONNECT_OPT_ENDPOINT1]);
		goto out;
	}

	error = dprc_connect(&resman.mc_io,
			     dprc_handle,
			     &endpoint1,
			     &endpoint2);

	if (error < 0)
		ERROR_PRINTF("dprc_connect() failed: %d\n", error);
out:
	if (dprc_opened) {
		int error2;

		error2 = dprc_close(&resman.mc_io, dprc_handle);
		if (error2 < 0) {
			ERROR_PRINTF("dprc_close() failed with error %d\n",
				     error2);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int cmd_dprc_disconnect(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: resman dprc disconnect <container> --endpoint=<object>\n"
		"\n"
		"--endpoint=<object>\n"
		"   Specifies either endpoint of a connection.\n"
		"\n";

	uint16_t dprc_handle;
	int error;
	bool dprc_opened = false;
	uint32_t parent_dprc_id;
	struct dprc_endpoint endpoint;

	if (resman.cmd_option_mask & ONE_BIT_MASK(DISCONNECT_OPT_HELP)) {
		printf(usage_msg);
		resman.cmd_option_mask &= ~ONE_BIT_MASK(DISCONNECT_OPT_HELP);
		error = 0;
		goto out;
	}

	if (resman.obj_name == NULL) {
		ERROR_PRINTF("<parent-container> argument missing\n");
		printf(usage_msg);
		error = -EINVAL;
		goto out;
	}

	error = parse_object_name(resman.obj_name,
				  "dprc", &parent_dprc_id);
	if (error < 0)
		goto out;

	if (parent_dprc_id != resman.root_dprc_id) {
		error = open_dprc(parent_dprc_id, &dprc_handle);
		if (error < 0)
			goto out;

		dprc_opened = true;
	} else {
		dprc_handle = resman.root_dprc_handle;
	}

	if (!(resman.cmd_option_mask & ONE_BIT_MASK(DISCONNECT_OPT_ENDPOINT))) {
		ERROR_PRINTF("--endpoint option missing\n");
		printf(usage_msg);
		error = -EINVAL;
		goto out;
	}

	resman.cmd_option_mask &= ~ONE_BIT_MASK(DISCONNECT_OPT_ENDPOINT);
	assert(resman.cmd_option_args[DISCONNECT_OPT_ENDPOINT] != NULL);
	error = parse_endpoint(resman.cmd_option_args[DISCONNECT_OPT_ENDPOINT], &endpoint);
	if (error < 0) {
		ERROR_PRINTF("Invalid --endpoint arg: '%s'\n",
			     resman.cmd_option_args[DISCONNECT_OPT_ENDPOINT]);
		goto out;
	}

	error = dprc_disconnect(&resman.mc_io,
				dprc_handle,
				&endpoint);

	if (error < 0)
		ERROR_PRINTF("dprc_disconnect() failed: %d\n", error);
out:
	if (dprc_opened) {
		int error2;

		error2 = dprc_close(&resman.mc_io, dprc_handle);
		if (error2 < 0) {
			ERROR_PRINTF("dprc_close() failed with error %d\n",
				     error2);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

/**
 * DPRC command table
 */
struct object_command dprc_commands[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dprc_help },

	{ .cmd_name = "list",
	  .options = dprc_list_options,
	  .cmd_func = cmd_dprc_list },

	{ .cmd_name = "show",
	  .options = dprc_show_options,
	  .cmd_func = cmd_dprc_show },

	{ .cmd_name = "info",
	  .options = dprc_info_options,
	  .cmd_func = cmd_dprc_info },

	{ .cmd_name = "create",
	  .options = dprc_create_child_options,
	  .cmd_func = cmd_dprc_create_child },

	{ .cmd_name = "destroy",
	  .options = dprc_destroy_options,
	  .cmd_func = cmd_dprc_destroy_child },

	{ .cmd_name = "assign",
	  .options = dprc_assign_options,
	  .cmd_func = cmd_dprc_assign },

	{ .cmd_name = "unassign",
	  .options = dprc_assign_options,
	  .cmd_func = cmd_dprc_unassign },

	{ .cmd_name = "set-quota",
	  .options = dprc_set_quota_options,
	  .cmd_func = cmd_dprc_set_quota },

	{ .cmd_name = "connect",
	  .options = dprc_connect_options,
	  .cmd_func = cmd_dprc_connect },

	{ .cmd_name = "disconnect",
	  .options = dprc_disconnect_options,
	  .cmd_func = cmd_dprc_disconnect },

	{ .cmd_name = NULL },
};


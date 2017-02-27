/* Copyright 2014-2017 Freescale Semiconductor Inc.
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
#include <math.h>
#include <sys/ioctl.h>
#include "restool.h"
#include "utils.h"
#include "dprc_commands_generate_dpl.h"

#define ALL_DPRC_OPTS (				\
	DPRC_CFG_OPT_SPAWN_ALLOWED |		\
	DPRC_CFG_OPT_ALLOC_ALLOWED |		\
	DPRC_CFG_OPT_OBJ_CREATE_ALLOWED |	\
	DPRC_CFG_OPT_TOPOLOGY_CHANGES_ALLOWED |	\
	DPRC_CFG_OPT_AIOP |			\
	DPRC_CFG_OPT_IRQ_CFG_ALLOWED)

enum mc_cmd_status mc_status;

/**
 * dprc sync command options
 */
enum dprc_sync_options {
	SYNC_OPT_HELP = 0,
};

static struct option dprc_sync_options[] = {
	[SYNC_OPT_HELP] = {
		.name = "help",
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dprc_sync_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);


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

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dprc_list_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dprc show command options
 */
enum dprc_show_options {
	SHOW_OPT_HELP = 0,
	SHOW_OPT_RESOURCES,
	SHOW_OPT_RES_TYPE,
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

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dprc_show_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dprc info command options
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
	CREATE_OPT_LABEL,
};

static struct option dprc_create_child_options[] = {
	[CREATE_OPT_HELP] = {
		.name = "help",
	},

	[CREATE_OPT_OPTIONS] = {
		.name = "options",
		.has_arg = 1,
	},

	[CREATE_OPT_LABEL] = {
		.name = "label",
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

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dprc_destroy_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

/**
 * dprc assign command options
 */
enum dprc_assign_options {
	ASSIGN_OPT_HELP = 0,
	ASSIGN_OPT_OBJECT,
	ASSIGN_OPT_CHILD,
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

	[ASSIGN_OPT_CHILD] = {
		.name = "child",
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
 * dprc set-label command options
 */
enum dprc_set_label_options {
	SET_LABEL_OPT_HELP = 0,
	SET_LABEL_OPT_LABEL,
};

static struct option dprc_set_label_options[] = {
	[SET_LABEL_OPT_HELP] = {
		.name = "help",
	},

	[SET_LABEL_OPT_LABEL] = {
		.name = "label",
		.has_arg = 1,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dprc_set_label_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

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

/**
 * dpl generate command options
 */
enum dpl_generate_options {
	GENERATE_OPT_HELP = 0,
};

struct option dpl_generate_options[] = {
	[GENERATE_OPT_HELP] = {
		.name = "help",
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpl_generate_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static const struct flib_ops dprc_ops = {
	.obj_open = dprc_open,
	.obj_close = dprc_close,
	.obj_get_irq_mask = dprc_get_irq_mask,
	.obj_get_irq_status = dprc_get_irq_status,
};

static struct option_entry options_map[] = {
	OPTION_MAP_ENTRY(DPRC_CFG_OPT_SPAWN_ALLOWED),
	OPTION_MAP_ENTRY(DPRC_CFG_OPT_ALLOC_ALLOWED),
	OPTION_MAP_ENTRY(DPRC_CFG_OPT_OBJ_CREATE_ALLOWED),
	OPTION_MAP_ENTRY(DPRC_CFG_OPT_TOPOLOGY_CHANGES_ALLOWED),
	OPTION_MAP_ENTRY(DPRC_CFG_OPT_AIOP),
	OPTION_MAP_ENTRY(DPRC_CFG_OPT_IRQ_CFG_ALLOWED),
};
static unsigned int options_num = ARRAY_SIZE(options_map);

static int cmd_dprc_help(void)
{
	static const char help_msg[] =
		"\n"
		"Usage: restool dprc <command> [--help] [ARGS...]\n"
		"Where <command> can be:\n"
		"   sync         - synchronize the objects in MC with MC bus.\n"
		"   list         - lists all containers (DPRC objects) in the system.\n"
		"   show         - displays the object contents of a DPRC object.\n"
		"   info         - displays detailed information about a DPRC object.\n"
		"   create       - creates a new child DPRC under the specified parent.\n"
		"   destroy      - destroys a child DPRC under the specified parent.\n"
		"   assign       - moves an object from a parent container to a child container.\n"
		"                  change an object's plugged state\n"
		"   unassign     - moves an object from a child container to a parent container.\n"
		"   set-label    - sets label/alias for any objects except root container.\n"
		"   connect      - connects 2 objects, creating a link between them.\n"
		"   disconnect   - removes the link between two objects. Either endpoint can\n"
		"		   be specified as the target of the operation.\n"
		"   generate-dpl - generate DPL syntax for the specified container\n"
		"\n"
		"For command-specific help, use the --help option of each command.\n"
		"\n";

	printf(help_msg);
	return 0;
}

static int cmd_dprc_sync(void)
{
	int error;

	static const char usage_msg[] =
		"\n"
		"Usage: restool dprc sync\n"
		"\n";

	if (restool.cmd_option_mask & ONE_BIT_MASK(SYNC_OPT_HELP)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(SYNC_OPT_HELP);
		return 0;
	}

	if (restool.obj_name != NULL) {
		ERROR_PRINTF(
			"Unexpected argument: \'%s\'\n\n", restool.obj_name);
		puts(usage_msg);
		return -EINVAL;
	}

	DEBUG_PRINTF("calling sytem()\n");
	error = system("echo 1 > /sys/bus/fsl-mc/rescan");
	if (error == -1) {
		error = -errno;
		DEBUG_PRINTF(
			"system() failed with error %d\n",
			error);
	}

	return error;
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

	error = dprc_get_obj_count(&restool.mc_io, 0,
				   dprc_handle,
				   &num_child_devices);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	for (int i = 0; i < num_child_devices; i++) {
		struct dprc_obj_desc obj_desc;
		uint16_t child_dprc_handle;
		int error2;

		error = dprc_get_obj(
				&restool.mc_io, 0,
				dprc_handle,
				i,
				&obj_desc);
		if (error < 0) {
			DEBUG_PRINTF(
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

		error2 = dprc_close(&restool.mc_io, 0, child_dprc_handle);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
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
		"Usage: restool dprc list\n"
		"\n";

	if (restool.cmd_option_mask & ONE_BIT_MASK(LIST_OPT_HELP)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(LIST_OPT_HELP);
		return 0;
	}

	if (restool.obj_name != NULL) {
		ERROR_PRINTF(
			"Unexpected argument: \'%s\'\n\n", restool.obj_name);
		puts(usage_msg);
		return -EINVAL;
	}

	return list_dprc(
		restool.root_dprc_id, restool.root_dprc_handle, 0, false);
}

static int show_one_resource_type(uint16_t dprc_handle,
				      const char *mc_res_type)
{
	int res_count;
	int res_discovered_count;
	struct dprc_res_ids_range_desc range_desc;
	int error;

	error = dprc_get_res_count(&restool.mc_io, 0, dprc_handle,
				   (char *)mc_res_type, &res_count);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
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

		error = dprc_get_res_ids(&restool.mc_io, 0, dprc_handle,
					 (char *)mc_res_type, &range_desc);
		if (error < 0) {
			mc_status = flib_error_to_mc_status(error);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
			goto out;
		}

		if (range_desc.base_id == range_desc.last_id)
			printf("%s.%d\n", mc_res_type, range_desc.base_id);
		else
			printf("%s.%d - %s.%d\n",
			       mc_res_type, range_desc.base_id,
			       mc_res_type, range_desc.last_id);

		for (id = range_desc.base_id; id <= range_desc.last_id; id++)
			res_discovered_count++;

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

	error = dprc_get_res_count(&restool.mc_io, 0, dprc_handle,
				   (char *)mc_res_type, &res_count);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	assert(res_count >= 0);
	printf("%s: %d\n", mc_res_type, res_count);
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

	error = dprc_get_pool_count(&restool.mc_io, 0, dprc_handle,
				    &pool_count);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	assert(pool_count >= 0);
	if (0 == pool_count) {
		printf("Don't have any resource in current dprc container.\n");
		return 0;
	}
	for (int i = 0; i < pool_count; i++) {
		memset(res_type, 0, sizeof(res_type));
		error = dprc_get_pool(&restool.mc_io, 0, dprc_handle,
				      i, res_type);

		/* check for buffer overrun: */
		assert(res_type[sizeof(res_type) - 1] == '\0');

		if (error < 0) {
			DEBUG_PRINTF(
				"dprc_get_pool() failed for pool index %d (error: %d)\n",
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
	int width;
	int labelen;
	char plug_stat[10] = {'\0'};
	struct dprc_obj_desc obj_desc;

	error = dprc_get_obj_count(&restool.mc_io, 0,
				   dprc_handle,
				   &num_child_devices);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	printf("%s contains %u objects%c\n", dprc_name, num_child_devices,
	       num_child_devices == 0 ? '.' : ':');
	printf("object\t\tlabel\t\tplugged-state\n");

	for (int i = 0; i < num_child_devices; i++) {
		plug_stat[0] = '\0';
		memset(&obj_desc, 0, sizeof(obj_desc));
		error = dprc_get_obj(&restool.mc_io, 0,
				     dprc_handle,
				     i,
				     &obj_desc);
		if (error < 0) {
			DEBUG_PRINTF(
				"dprc_get_object(%u) failed with error %d\n",
				i, error);
			goto out;
		}
		assert(strlen(obj_desc.label) <= MC_OBJ_LABEL_MAX_LENGTH);

		if (obj_desc.id < 0)
			width = strlen(obj_desc.type) + 1 +
				(2 + (int)log10(0 - obj_desc.id));
		else if (obj_desc.id == 0)
			width = strlen(obj_desc.type) + 1 + 1;
		else
			width = strlen(obj_desc.type) + 1 +
				(1 + (int)log10(obj_desc.id));

		labelen = strlen(obj_desc.label);

		DEBUG_PRINTF("%s.%d name length=%d\n",
				obj_desc.type, obj_desc.id, width);
		DEBUG_PRINTF("label \"%s\" length=%d\n",
				obj_desc.label, labelen);

		if (strcmp(obj_desc.label, "dprc") == 0)
			plug_stat[0] = '\0';
		else if (obj_desc.state & DPRC_OBJ_STATE_PLUGGED)
			strncpy(plug_stat, "plugged", 9);
		else
			strncpy(plug_stat, "unplugged", 9);
		plug_stat[9] = '\0';

		if (width < 8 && labelen < 8)
			printf("%s.%d\t\t%s\t\t%s\n",
			obj_desc.type, obj_desc.id, obj_desc.label, plug_stat);
		else if (width < 8 && labelen >= 8)
			printf("%s.%d\t\t%s\t%s\n",
			obj_desc.type, obj_desc.id, obj_desc.label, plug_stat);
		else if (width >= 8 && labelen < 8)
			printf("%s.%d\t%s\t\t%s\n",
			obj_desc.type, obj_desc.id, obj_desc.label, plug_stat);
		else
			printf("%s.%d\t%s\t%s\n",
			obj_desc.type, obj_desc.id, obj_desc.label, plug_stat);
	}

	error = 0;
out:
	return error;
}

static int cmd_dprc_show(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dprc show <container>\n"
		"\n";

	uint32_t dprc_id;
	uint16_t dprc_handle;
	const char *dprc_name;
	int error;
	bool dprc_opened = false;
	const char *res_type;

	if (restool.cmd_option_mask & ONE_BIT_MASK(SHOW_OPT_HELP)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(SHOW_OPT_HELP);
		error = 0;
		goto out;
	}

	if (restool.obj_name == NULL) {
		ERROR_PRINTF("<object> argument missing\n");
		puts(usage_msg);
		error = -EINVAL;
		goto out;
	}

	dprc_name = restool.obj_name;
	if (strcmp(dprc_name, "mc.global") == 0)
		dprc_name = "dprc.0";

	error = parse_object_name(dprc_name, "dprc", &dprc_id);
	if (error < 0)
		goto out;

	if (dprc_id != restool.root_dprc_id) {
		error = open_dprc(dprc_id, &dprc_handle);
		if (error < 0)
			goto out;

		dprc_opened = true;
	} else {
		dprc_handle = restool.root_dprc_handle;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(SHOW_OPT_RESOURCES)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(SHOW_OPT_RESOURCES);
		error = show_mc_resources(dprc_handle);
	} else if (restool.cmd_option_mask & ONE_BIT_MASK(SHOW_OPT_RES_TYPE)) {
		assert(restool.cmd_option_args[SHOW_OPT_RES_TYPE] != NULL);
		error = check_resource_type(
			restool.cmd_option_args[SHOW_OPT_RES_TYPE]);
		if (error < 0) {
			puts(usage_msg);
			goto out;
		}
		res_type = restool.cmd_option_args[SHOW_OPT_RES_TYPE];
		restool.cmd_option_mask &= ~ONE_BIT_MASK(SHOW_OPT_RES_TYPE);
		error = show_one_resource_type(dprc_handle, res_type);
	} else {
		error = show_mc_objects(dprc_handle, dprc_name);
	}
out:
	if (dprc_opened) {
		int error2;

		error2 = dprc_close(&restool.mc_io, 0, dprc_handle);
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

static void print_dprc_options(uint64_t options)
{
	if ((options & ~ALL_DPRC_OPTS) != 0) {
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

	if (options & DPRC_CFG_OPT_AIOP)
		printf("\tDPRC_CFG_OPT_AIOP\n");

	if (options & DPRC_CFG_OPT_IRQ_CFG_ALLOWED)
		printf("\tDPRC_CFG_OPT_IRQ_CFG_ALLOWED\n");
}

static int print_dprc_attr(uint32_t dprc_id,
			   struct dprc_obj_desc *target_obj_desc)
{
	uint16_t dprc_handle;
	int error;
	struct dprc_attributes dprc_attr;
	bool dprc_opened = false;

	if (dprc_id != restool.root_dprc_id) {
		error = open_dprc(dprc_id, &dprc_handle);
		if (error < 0)
			goto out;

		dprc_opened = true;
	} else {
		dprc_handle = restool.root_dprc_handle;
	}

	memset(&dprc_attr, 0, sizeof(dprc_attr));
	error = dprc_get_attributes(&restool.mc_io, 0, dprc_handle, &dprc_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
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
	print_obj_label(target_obj_desc);

	error = 0;

out:
	if (dprc_opened) {
		int error2;

		error2 = dprc_close(&restool.mc_io, 0, dprc_handle);
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

static int print_dprc_info(uint32_t dprc_id)
{
	int error;
	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	bool found = false;

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
				restool.root_dprc_handle, 0, dprc_id,
				"dprc", &target_obj_desc,
				&target_parent_dprc_id, &found);
	if (error < 0)
		goto out;

	if (strcmp(target_obj_desc.type, "dprc")) {
		printf("dprc.%d does not exist\n", dprc_id);
		return -EINVAL;
	}

	error = print_dprc_attr(dprc_id, &target_obj_desc);
	if (error < 0)
		goto out;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_VERBOSE)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_VERBOSE);
		error = print_obj_verbose(&target_obj_desc, &dprc_ops);
	}

out:
	return error;
}

static int cmd_dprc_info(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dprc info <dprc-object> [--verbose]\n"
		"\n"
		"OPTIONS:\n"
		"--verbose\n"
		"   Shows extended/verbose information about the object\n"
		"\n"
		"EXAMPLE:\n"
		"Display information about dprc.5:\n"
		"   $ restool dprc info dprc.5\n"
		"\n";

	uint32_t dprc_id;
	const char *dprc_name;
	int error;

	if (restool.cmd_option_mask & ONE_BIT_MASK(INFO_OPT_HELP)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(INFO_OPT_HELP);
		error = 0;
		goto out;
	}

	if (restool.obj_name == NULL) {
		ERROR_PRINTF("<object> argument missing\n");
		puts(usage_msg);
		error = -EINVAL;
		goto out;
	}

	dprc_name = restool.obj_name;
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
static int create_child_dprc(uint16_t dprc_handle, uint64_t options,
			     bool has_label)
{
	int error;
	int error2;
	struct dprc_cfg cfg;
	int child_dprc_id;
	uint64_t mc_portal_offset;
	bool child_dprc_created = false;

	assert(dprc_handle != 0);

	cfg.icid = DPRC_GET_ICID_FROM_POOL;
	cfg.portal_id = DPRC_GET_PORTAL_ID_FROM_POOL;
	cfg.options = options;
	if (has_label) {
		strncpy(cfg.label,
			restool.cmd_option_args[CREATE_OPT_LABEL],
			MC_OBJ_LABEL_MAX_LENGTH);
		cfg.label[15] = '\0';
	} else
		cfg.label[0] = '\0';
	error = dprc_create_container(
			&restool.mc_io, 0,
			dprc_handle,
			&cfg,
			&child_dprc_id,
			&mc_portal_offset);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto error;
	}

	child_dprc_created = true;
	print_new_obj("dprc", child_dprc_id, restool.obj_name);
	DEBUG_PRINTF("dprc.%u's MC portal id %u, portal addr %#llx\n",
		child_dprc_id,
		(unsigned int)MC_PORTAL_OFFSET_TO_PORTAL_ID(mc_portal_offset),
		(unsigned long long)mc_portal_offset + MC_PORTALS_BASE_PADDR);

	return 0;
error:
	if (child_dprc_created) {
		error2 = dprc_destroy_container(&restool.mc_io, 0, dprc_handle,
						child_dprc_id);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
		}
	}

	return error;
}

static int cmd_dprc_create_child(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dprc create <parent-container> [--options=<options-mask>]\n"
		"		      [--label=<label-string>]\n"
		"\n"
		"OPTIONS:\n"
		"--options=<options-mask>\n"
		"   Where <options-mask> is a comma or space separated list of DPRC options:\n"
		"	DPRC_CFG_OPT_SPAWN_ALLOWED\n"
		"	DPRC_CFG_OPT_ALLOC_ALLOWED\n"
		"	DPRC_CFG_OPT_OBJ_CREATE_ALLOWED\n"
		"	DPRC_CFG_OPT_TOPOLOGY_CHANGES_ALLOWED\n"
		"	DPRC_CFG_OPT_AIOP\n"
		"	DPRC_CFG_OPT_IRQ_CFG_ALLOWED\n"
		"--label=<label-string>\n"
		"   Where <label-string> is a string of up to 15 chars to label\n"
		"   the container\n"
		"\n"
		"EXAMPLES:\n"
		"Create a child DPRC under parent dprc.1 with default options:\n"
		"   $ restool dprc create dprc.1\n"
		"Create a child DPRC under parent dprc.1 with default options and a label string:\n"
		"   $ restool dprc create dprc.1 --label=\"nadk's dprc\"\n"
		"\n";
	bool dprc_opened = false;
	bool has_label = false;
	uint64_t options = 0;
	uint16_t dprc_handle;
	uint32_t dprc_id;
	int error;

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_HELP)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_HELP);
		error = 0;
		goto out;
	}

	if (restool.obj_name == NULL) {
		ERROR_PRINTF("<object> argument missing\n");
		puts(usage_msg);
		error = -EINVAL;
		goto out;
	}

	error = parse_object_name(restool.obj_name,
				  "dprc", &dprc_id);
	if (error < 0)
		goto out;

	if (dprc_id != restool.root_dprc_id) {
		error = open_dprc(dprc_id, &dprc_handle);
		if (error < 0)
			goto out;

		dprc_opened = true;
	} else {
		dprc_handle = restool.root_dprc_handle;
	}
	DEBUG_PRINTF("dprc.%d's handle is: %#x\n", dprc_id, dprc_handle);

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_OPTIONS)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_OPTIONS);
		error = parse_generic_create_options(
				restool.cmd_option_args[CREATE_OPT_OPTIONS],
				&options,
				options_map,
				options_num);
		if (error < 0)
			goto out;
	} else {
		options = DPRC_CFG_OPT_SPAWN_ALLOWED |
			  DPRC_CFG_OPT_ALLOC_ALLOWED |
			  DPRC_CFG_OPT_OBJ_CREATE_ALLOWED |
			  DPRC_CFG_OPT_IRQ_CFG_ALLOWED;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(CREATE_OPT_LABEL)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CREATE_OPT_LABEL);
		has_label = true;
		DEBUG_PRINTF("Object label length: %d\n",
		(int)strlen(restool.cmd_option_args[CREATE_OPT_LABEL]));
		if (strlen(restool.cmd_option_args[CREATE_OPT_LABEL]) >
		    MC_OBJ_LABEL_MAX_LENGTH) {
			ERROR_PRINTF("object label length exceeding %d\n",
					MC_OBJ_LABEL_MAX_LENGTH);
			error = -EINVAL;
			puts(usage_msg);
			goto out;
		}
	} else {
		has_label = false;
	}

	error = create_child_dprc(dprc_handle, options, has_label);
out:
	if (dprc_opened) {
		int error2;

		error2 = dprc_close(&restool.mc_io, 0, dprc_handle);
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

static int cmd_dprc_destroy_child(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dprc destroy <container>\n"
		"\n"
		"NOTE:\n"
		" -<container> cannot be the root container\n"
		"\n";

	int error;
	uint32_t child_dprc_id;
	struct dprc_obj_desc child_obj_desc;
	uint32_t parent_dprc_id;
	uint16_t parent_dprc_handle;
	bool found = false;

	if (restool.cmd_option_mask & ONE_BIT_MASK(DESTROY_OPT_HELP)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(DESTROY_OPT_HELP);
		error = 0;
		goto out;
	}

	if (restool.obj_name == NULL) {
		ERROR_PRINTF("<object> argument missing\n");
		error = -EINVAL;
		goto out;
	}

	if (in_use(restool.obj_name, "destroyed")) {
		error = -EBUSY;
		goto out;
	}

	error = parse_object_name(restool.obj_name,
				  "dprc", &child_dprc_id);
	if (error < 0)
		goto out;

	if (child_dprc_id == restool.root_dprc_id) {
		ERROR_PRINTF("The root DPRC (%s) cannot be destroyed\n",
			     restool.obj_name);
		error = -EINVAL;
		goto out;
	}

	memset(&child_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
				restool.root_dprc_handle, 0,
				child_dprc_id, "dprc", &child_obj_desc,
				&parent_dprc_id, &found);

	if (!found) {
		if (error == 0)
			printf("%s does not exist\n", restool.obj_name);
		error = -EINVAL;
		goto out;
	}

	if (parent_dprc_id == restool.root_dprc_id)
		parent_dprc_handle = restool.root_dprc_handle;
	else {
		error = open_dprc(parent_dprc_id,
				&parent_dprc_handle);
		if (error < 0)
			goto out;
	}
	/*
	 * Destroy child container in the MC:
	 */
	error = dprc_destroy_container(&restool.mc_io, 0, parent_dprc_handle,
					child_dprc_id);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);

		goto out;
	}

	printf("dprc.%u is destroyed\n", child_dprc_id);

	if (parent_dprc_id != restool.root_dprc_id)
		error = dprc_close(&restool.mc_io, 0, parent_dprc_handle);

out:
	return error;
}

static int lookup_obj_desc(uint32_t parent_dprc_id,
			   const char *obj_type,
			   int obj_id,
			   struct dprc_obj_desc *obj_desc_out)
{
	uint16_t dprc_handle;
	int i;
	int error;
	int num_child_devices;
	bool dprc_opened = false;

	if (parent_dprc_id != restool.root_dprc_id) {
		error = open_dprc(parent_dprc_id, &dprc_handle);
		if (error < 0)
			goto out;

		dprc_opened = true;
	} else {
		dprc_handle = restool.root_dprc_handle;
	}

	error = dprc_get_obj_count(&restool.mc_io, 0,
				   dprc_handle,
				   &num_child_devices);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	for (i = 0; i < num_child_devices; i++) {
		struct dprc_obj_desc obj_desc;

		error = dprc_get_obj(&restool.mc_io, 0,
				     dprc_handle,
				     i,
				     &obj_desc);
		if (error < 0) {
			DEBUG_PRINTF(
				"dprc_get_object(%u) failed with error %d\n",
				i, error);
			goto out;
		}

		if (strcmp(obj_desc.type, obj_type) == 0 &&
		    obj_desc.id == obj_id) {
			*obj_desc_out = obj_desc;
			break;
		}
	}

	if (i == num_child_devices) {
		error = -ENOENT;
		ERROR_PRINTF("%s.%d does not exist in dprc.%u\n",
			     obj_type, obj_id, parent_dprc_id);
	}

out:
	if (dprc_opened) {
		int error2;

		error2 = dprc_close(&restool.mc_io, 0, dprc_handle);
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

static int do_dprc_assign_or_unassign(const char *usage_msg, bool do_assign)
{
	uint16_t dprc_handle;
	int error;
	bool dprc_opened = false;
	uint32_t parent_dprc_id;
	uint32_t child_dprc_id;
	struct dprc_res_req res_req;

	if (restool.cmd_option_mask & ONE_BIT_MASK(ASSIGN_OPT_HELP)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(ASSIGN_OPT_HELP);
		error = 0;
		goto out;
	}

	if (restool.obj_name == NULL) {
		ERROR_PRINTF("<parent-container> argument missing\n");
		puts(usage_msg);
		error = -EINVAL;
		goto out;
	}

	memset(&res_req, 0, sizeof(res_req));
	error = parse_object_name(restool.obj_name,
				  "dprc", &parent_dprc_id);
	if (error < 0)
		goto out;

	if (parent_dprc_id != restool.root_dprc_id) {
		error = open_dprc(parent_dprc_id, &dprc_handle);
		if (error < 0)
			goto out;

		dprc_opened = true;
	} else {
		dprc_handle = restool.root_dprc_handle;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(ASSIGN_OPT_CHILD)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(ASSIGN_OPT_CHILD);
		assert(restool.cmd_option_args[ASSIGN_OPT_CHILD] != NULL);
		error = parse_object_name(
				restool.cmd_option_args[ASSIGN_OPT_CHILD],
				"dprc", &child_dprc_id);
		if (error < 0)
			goto out;
	} else {
		child_dprc_id = parent_dprc_id;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(ASSIGN_OPT_RES_TYPE)) {
		/* moving resource case */
		restool.cmd_option_mask &= ~ONE_BIT_MASK(ASSIGN_OPT_RES_TYPE);
		assert(restool.cmd_option_args[ASSIGN_OPT_RES_TYPE] != NULL);
		error = check_resource_type(
			restool.cmd_option_args[ASSIGN_OPT_RES_TYPE]);
		if (error < 0) {
			puts(usage_msg);
			goto out;
		}
		strcpy(res_req.type,
		       restool.cmd_option_args[ASSIGN_OPT_RES_TYPE]);

		if (!(restool.cmd_option_mask &
		    ONE_BIT_MASK(ASSIGN_OPT_COUNT))) {
			ERROR_PRINTF("--count option missing\n");
			puts(usage_msg);
			error = -EINVAL;
			goto out;
		}

		assert(restool.cmd_option_args[ASSIGN_OPT_COUNT] != NULL);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(ASSIGN_OPT_COUNT);

		if (parent_dprc_id == child_dprc_id && !do_assign) {
			ERROR_PRINTF(
				"using unassign command to move resource?\n"
				"child-container must be different from grandparent\n");
			error = -EINVAL;
			goto out;
		}
		res_req.num = atoi(restool.cmd_option_args[ASSIGN_OPT_COUNT]);
		if (res_req.num <= 0) {
			ERROR_PRINTF("Invalid --count arg: %s\n",
				     restool.cmd_option_args[ASSIGN_OPT_COUNT]);
			error = -EINVAL;
			goto out;
		}

		res_req.options = 0;
		res_req.id_base_align = 0;
	} else if (restool.cmd_option_mask & ONE_BIT_MASK(ASSIGN_OPT_OBJECT)) {
		/* changing plugged state, moving object case */
		int n;
		int state;

		restool.cmd_option_mask &= ~ONE_BIT_MASK(ASSIGN_OPT_OBJECT);
		assert(restool.cmd_option_args[ASSIGN_OPT_OBJECT] != NULL);

		n = sscanf(restool.cmd_option_args[ASSIGN_OPT_OBJECT],
			   "%" STRINGIFY(OBJ_TYPE_MAX_LENGTH) "[a-z].%d",
			   res_req.type, &res_req.id_base_align);
		if (n != 2) {
			ERROR_PRINTF("Invalid --object arg: \'%s\'\n",
				restool.cmd_option_args[ASSIGN_OPT_OBJECT]);

			error = -EINVAL;
			goto out;
		}
		if (strcmp(res_req.type, "dprc") == 0) {
			ERROR_PRINTF(
				"Cannot change plugged state of dprc\n"
				"Cannot move dprc from one container to another\n");
			error = -EINVAL;
			goto out;
		}

		res_req.options = DPRC_RES_REQ_OPT_EXPLICIT;

		if (restool.cmd_option_mask &
		    ONE_BIT_MASK(ASSIGN_OPT_PLUGGED)) {
			/* changing plugged state case*/
			restool.cmd_option_mask &=
				~ONE_BIT_MASK(ASSIGN_OPT_PLUGGED);

			if (!do_assign) {
				ERROR_PRINTF(
					"Cannot change plugged state via \'dprc unassign\'\nPlease try \'restool dprc assign --help\'\n");
				error = -EINVAL;
				goto out;
			}
			if (in_use(restool.cmd_option_args[ASSIGN_OPT_OBJECT],
			    "changed plugged state")) {
				error = -EBUSY;
				goto out;
			}

			assert(restool.cmd_option_args[ASSIGN_OPT_PLUGGED] !=
			       NULL);
			state = atoi(restool.
					cmd_option_args[ASSIGN_OPT_PLUGGED]);

			if (state < 0 || state > 1) {
				ERROR_PRINTF(
				    "Invalid --plugged arg: \'%s\'\n",
				restool.cmd_option_args[ASSIGN_OPT_PLUGGED]);
				error = -EINVAL;
				goto out;
			}

			if (state == 1)
				res_req.options |= DPRC_RES_REQ_OPT_PLUGGED;
		} else { /* moving object case */
			if (child_dprc_id == parent_dprc_id) {
				ERROR_PRINTF(
					"change plugged state? --plugged option required\n"
					"move objects? child-container should be different from parent-container\n");
				puts(usage_msg);
				error = -EINVAL;
				goto out;
			}
			if (in_use(restool.cmd_option_args[ASSIGN_OPT_OBJECT],
			    "moved"))  {
				error = -EBUSY;
				goto out;
			}

			struct dprc_obj_desc obj_desc;

			error = lookup_obj_desc(do_assign ? parent_dprc_id :
							    child_dprc_id,
						res_req.type,
						res_req.id_base_align,
						&obj_desc);
			if (error < 0)
				goto out;
			if (obj_desc.state & DPRC_OBJ_STATE_PLUGGED) {
				ERROR_PRINTF(
				"%s cannot be moved because it is currently in plugged state\n"
				"unplug it first\n",
				restool.cmd_option_args[ASSIGN_OPT_OBJECT]);

				error = -EBUSY;
				goto out;
			}
		}
	} else { /* invalid command case */
		ERROR_PRINTF("Invalid command line\n");
		puts(usage_msg);
		error = -EINVAL;
		goto out;
	}

	if (do_assign) {
		error = dprc_assign(&restool.mc_io, 0,
				    dprc_handle,
				    child_dprc_id,
				    &res_req);
		if (error < 0) {
			mc_status = flib_error_to_mc_status(error);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
		}
	} else {
		error = dprc_unassign(&restool.mc_io, 0,
				      dprc_handle,
				      child_dprc_id,
				      &res_req);
		if (error < 0) {
			mc_status = flib_error_to_mc_status(error);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
		}
	}
out:
	if (dprc_opened) {
		int error2;

		error2 = dprc_close(&restool.mc_io, 0, dprc_handle);
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

static int cmd_dprc_assign(void)
{
	static const char usage_msg[] =
		"\n"
		"To move an object from parent to child:\n"
		"Usage: restool dprc assign <container> --child=<child-container>\n"
		"		  --object=<object>\n"
		"\n"
		"To move an object and set the plugged state of an object:\n"
		"Usage:  restool dprc assign <container> --child=<child-container>\n"
		"		  --object=<object> --plugged=<state>\n"
		"\n"
		"To set the plugged state of an object:\n"
		"Usage: restool dprc assign <container> --object=<object> --plugged=<state>\n"
		"\n"
		"  <container>\n"
		"    Specifies the source container for the operation.\n"
		"  --child=<child-container>\n"
		"    Specifies the destination container for the operation.\n"
		"  --object=<object>\n"
		"    Specifies the object to move from parent container to child container\n"
		"  --plugged=<state>\n"
		"    Specifies the plugged state of the object (valid values are 0 or 1)\n"
		"\n"
		"NOTES:\n"
		"  -It is possible\n"
		"  -It is not possible to assign DPRC objects.\n"
		"  -It is not possible (and unnecessary) to change the plugged state of a DPRC\n"
		"  -It is not possible to move plugged objects (i.e. plugged=1)\n"
		"  -The assign operation may be restricted by the permissions granted in\n"
		"   the container attributes.\n"
		"\n"
		"EXAMPLES:\n"
		"To move dpni.2 from dprc.1 to dprc.4 (leaving the plugged state unchanged):\n"
		"  $ restool dprc assign dprc.1 --child=dprc.4 --object=dpni.2\n"
		"To move dpni.2 from dprc.1 to dprc.4 and set dpni.2 to be plugged:\n"
		"  $ restool dprc assign dprc.1 --child=dprc.4 --object=dpni.2 --plugged=1\n"
		"To set dpni.2 in container dprc.1 to be plugged:\n"
		"  $ restool dprc assign dprc.1 --object=dprc.2 --plugged=1\n"
		"\n";

	return do_dprc_assign_or_unassign(usage_msg, true);
}

static int cmd_dprc_unassign(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dprc unassign <parent-container> --child=<child-container>\n"
		"		  --object=<object>\n"
		"\n"
		"  <parent-container>\n"
		"    Container that is the destination of the operation.\n"
		"  --child=<child-container>\n"
		"    Container that is the source of the operation.\n"
		"  --object=<object>\n"
		"    Specifies the object to move from parent to child.\n"
		"\n"
		"NOTES:\n"
		"  -It is not possible to unassign dprc objects\n"
		"\n"
		"EXAMPLE:\n"
		"To move dpni.2 from dprc.4 (child) to dprc.1 (parent):\n"
		"   $ restool dprc unassign dprc.1 --child=dprc.4 --object=dpni.2\n"
		"\n";

	return do_dprc_assign_or_unassign(usage_msg, false);
}

static int cmd_dprc_set_label(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dprc set-label <object> --label=<label>\n"
		"\n"
		"  --label=<label>\n"
		"    Specifies a string of up to 15 characters to label the object.\n"
		"\n"
		"NOTE:\n"
		"  -<object> cannot be the root container\n"
		"\n"
		"EXAMPLE:\n"
		"To set the label \"mountain\" on dpni.1:\n"
		"  $ restool dprc set-label dpni.1 --label=\"mountain\"\n"
		"\n";

	int error;
	int n;
	char obj_type[OBJ_TYPE_MAX_LENGTH + 1];
	uint32_t obj_id;
	bool target_parent_dprc_opened = false;
	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	uint16_t target_parent_dprc_handle;
	bool found;

	memset(&target_obj_desc, 0, sizeof(target_obj_desc));
	if (restool.cmd_option_mask & ONE_BIT_MASK(SET_LABEL_OPT_HELP)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(SET_LABEL_OPT_HELP);
		error = 0;
		goto out;
	}

	if (restool.obj_name == NULL) {
		ERROR_PRINTF("<object> argument missing\n");
		puts(usage_msg);
		error = -EINVAL;
		goto out;
	}

	n = sscanf(restool.obj_name, "%" STRINGIFY(OBJ_TYPE_MAX_LENGTH)
		   "[a-z].%u", obj_type, &obj_id);
	if (n != 2) {
		ERROR_PRINTF("Invalid MC object name: %s\n", restool.obj_name);
		return -EINVAL;
	}

	if (strcmp(obj_type, "dprc") == 0 && obj_id == restool.root_dprc_id) {
		ERROR_PRINTF("CANNOT set label for root dprc, i.e. dprc.1\n");
		puts(usage_msg);
		error = -EINVAL;
		goto out;
	}

	if (restool.cmd_option_mask & ONE_BIT_MASK(SET_LABEL_OPT_LABEL)) {
		restool.cmd_option_mask &= ~ONE_BIT_MASK(SET_LABEL_OPT_LABEL);
		if (strlen(restool.cmd_option_args[SET_LABEL_OPT_LABEL]) >
		    MC_OBJ_LABEL_MAX_LENGTH) {
			ERROR_PRINTF("label length > %d characters\n",
					MC_OBJ_LABEL_MAX_LENGTH);
			puts(usage_msg);
			error = -EINVAL;
			goto out;
		}
		if (strlen(restool.cmd_option_args[SET_LABEL_OPT_LABEL]) == 0) {
			ERROR_PRINTF("label length = 0 charcter\n");
			puts(usage_msg);
			error = -EINVAL;
			goto out;
		}
	} else {
		ERROR_PRINTF("missing --label option\n");
		puts(usage_msg);
		error = -EINVAL;
		goto out;
	}

	error = find_target_obj_desc(restool.root_dprc_id,
			restool.root_dprc_handle, 0, obj_id, obj_type,
			&target_obj_desc, &target_parent_dprc_id, &found);

	if (!found) {
		if (error == 0)
			printf("%s does not exist\n", restool.obj_name);
		error = -EINVAL;
		goto out;
	}

	if (error < 0)
		goto out;

	if (target_parent_dprc_id == restool.root_dprc_id)
		target_parent_dprc_handle = restool.root_dprc_handle;
	else {
		error = open_dprc(target_parent_dprc_id,
				&target_parent_dprc_handle);
		if (error < 0)
			goto out;
		target_parent_dprc_opened = true;
	}

	error = dprc_set_obj_label(&restool.mc_io, 0,
			target_parent_dprc_handle, obj_type, obj_id,
			restool.cmd_option_args[SET_LABEL_OPT_LABEL]);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
		     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

out:
	DEBUG_PRINTF("target_parent_dprc_opened=%d\n",
			(int)target_parent_dprc_opened);
	if (target_parent_dprc_opened) {
		int error2;

		error2 = dprc_close(&restool.mc_io, 0,
					target_parent_dprc_handle);
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

static int parse_endpoint(char *endpoint_str, struct dprc_endpoint *endpoint)
{
	int n;

	memset(endpoint, 0, sizeof(*endpoint));

	n = sscanf(endpoint_str,
		   "%" STRINGIFY(OBJ_TYPE_MAX_LENGTH) "[a-z].%d.%d",
		   endpoint->type, &endpoint->id, (int *)&endpoint->if_id);

	if (n < 2)
		return -EINVAL;

	if (n == 2)
		assert(endpoint->if_id == 0);

	return 0;
}


static int cmd_dprc_connect(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dprc connect <parent-container> --endpoint1=<object>\n"
		"		--endpoint2=<object>\n"
		"\n"
		"  <parent-container>\n"
		"    Specifies the parent-container.\n"
		"  --endpoint1=<object>\n"
		"    Specifies an endpoint object.\n"
		"  --endpoint2=<object>\n"
		"    Specifies an endpoint object.\n"
		"\n"
		"NOTES:\n"
		"  -<parent-container> must be a common ancestor of both <object> arguments\n"
		"   (for example the root container)\n"
		"  -endpoint1 and endpoint2 do not need to be in the same container.\n"
		"  -endpoints must be disconnected prior to being connected\n"
		"   (use 'dprc disconnect' to disconnect endpoints)\n"
		"  -multi-port objects such as a dpsw or dpdmux use the following naming\n"
		"   convention to specify endpoints: <object>.<id>.<port>\n"
		"\n"
		"EXAMPLE:\n"
		"To connect dpni.8 to dpsw.1.0:\n"
		"   $ restool dprc connect dprc.1 --endpoint1=dpsw.1.0 --endpoint2=dpni.8\n"
		"\n";

	uint16_t dprc_handle;
	int error;
	bool dprc_opened = false;
	uint32_t parent_dprc_id;
	struct dprc_endpoint endpoint1;
	struct dprc_endpoint endpoint2;
	struct dprc_connection_cfg dprc_connection_cfg = {
		/* If both rates are zero the connection */
		/* will be configured in "best effort" mode. */
		.committed_rate = 0,
		.max_rate = 0
	};

	if (restool.cmd_option_mask & ONE_BIT_MASK(CONNECT_OPT_HELP)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(CONNECT_OPT_HELP);
		error = 0;
		goto out;
	}

	if (restool.obj_name == NULL) {
		ERROR_PRINTF("<parent-container> argument missing\n");
		puts(usage_msg);
		error = -EINVAL;
		goto out;
	}

	error = parse_object_name(restool.obj_name,
				  "dprc", &parent_dprc_id);
	if (error < 0)
		goto out;

	if (parent_dprc_id != restool.root_dprc_id) {
		error = open_dprc(parent_dprc_id, &dprc_handle);
		if (error < 0)
			goto out;

		dprc_opened = true;
	} else {
		dprc_handle = restool.root_dprc_handle;
	}

	if (!(restool.cmd_option_mask & ONE_BIT_MASK(CONNECT_OPT_ENDPOINT1))) {
		ERROR_PRINTF("--endpoint1 option missing\n");
		puts(usage_msg);
		error = -EINVAL;
		goto out;
	}

	restool.cmd_option_mask &= ~ONE_BIT_MASK(CONNECT_OPT_ENDPOINT1);
	assert(restool.cmd_option_args[CONNECT_OPT_ENDPOINT1] != NULL);
	error = parse_endpoint(restool.cmd_option_args[CONNECT_OPT_ENDPOINT1],
			       &endpoint1);
	if (error < 0) {
		ERROR_PRINTF("Invalid --endpoint1 arg: '%s'\n",
			     restool.cmd_option_args[CONNECT_OPT_ENDPOINT1]);
		goto out;
	}

	if (!(restool.cmd_option_mask & ONE_BIT_MASK(CONNECT_OPT_ENDPOINT2))) {
		ERROR_PRINTF("--endpoint2 option missing\n");
		puts(usage_msg);
		error = -EINVAL;
		goto out;
	}

	restool.cmd_option_mask &= ~ONE_BIT_MASK(CONNECT_OPT_ENDPOINT2);
	assert(restool.cmd_option_args[CONNECT_OPT_ENDPOINT2] != NULL);
	error = parse_endpoint(restool.cmd_option_args[CONNECT_OPT_ENDPOINT2],
			       &endpoint2);
	if (error < 0) {
		ERROR_PRINTF("Invalid --endpoint2 arg: '%s'\n",
			     restool.cmd_option_args[CONNECT_OPT_ENDPOINT2]);
		goto out;
	}

	error = dprc_connect(&restool.mc_io, 0,
			     dprc_handle,
			     &endpoint1,
			     &endpoint2,
			     &dprc_connection_cfg);

	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
	}
out:
	if (dprc_opened) {
		int error2;

		error2 = dprc_close(&restool.mc_io, 0, dprc_handle);
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

static int cmd_dprc_disconnect(void)
{
	static const char usage_msg[] =
		"\n"
		"Usage: restool dprc disconnect <parent-container> --endpoint=<object>\n"
		"\n"
		"  <parent-container>\n"
		"    Specifies the parent-container.\n"
		"  --endpoint=<object>\n"
		"    Specifies either endpoint of a connection.\n"
		"\n"
		"NOTES:\n"
		"  -<parent-container> must be an ancestor of the <object>\n"
		"   (for example the root container)\n"
		"\n"
		"EXAMPLE:\n"
		"To disconnect dpni.8:\n"
		"   $ restool dprc disconnect dprc.1 --endpoint=dpni.8\n"
		"\n";

	uint16_t dprc_handle;
	int error;
	bool dprc_opened = false;
	uint32_t parent_dprc_id;
	struct dprc_endpoint endpoint;

	if (restool.cmd_option_mask & ONE_BIT_MASK(DISCONNECT_OPT_HELP)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(DISCONNECT_OPT_HELP);
		error = 0;
		goto out;
	}

	if (restool.obj_name == NULL) {
		ERROR_PRINTF("<parent-container> argument missing\n");
		puts(usage_msg);
		error = -EINVAL;
		goto out;
	}

	error = parse_object_name(restool.obj_name,
				  "dprc", &parent_dprc_id);
	if (error < 0)
		goto out;

	if (parent_dprc_id != restool.root_dprc_id) {
		error = open_dprc(parent_dprc_id, &dprc_handle);
		if (error < 0)
			goto out;

		dprc_opened = true;
	} else {
		dprc_handle = restool.root_dprc_handle;
	}

	if (!(restool.cmd_option_mask &
	    ONE_BIT_MASK(DISCONNECT_OPT_ENDPOINT))) {
		ERROR_PRINTF("--endpoint option missing\n");
		puts(usage_msg);
		error = -EINVAL;
		goto out;
	}

	restool.cmd_option_mask &= ~ONE_BIT_MASK(DISCONNECT_OPT_ENDPOINT);
	assert(restool.cmd_option_args[DISCONNECT_OPT_ENDPOINT] != NULL);
	error = parse_endpoint(restool.cmd_option_args[DISCONNECT_OPT_ENDPOINT],
			       &endpoint);
	if (error < 0) {
		ERROR_PRINTF("Invalid --endpoint arg: '%s'\n",
			     restool.cmd_option_args[DISCONNECT_OPT_ENDPOINT]);
		goto out;
	}

	error = dprc_disconnect(&restool.mc_io, 0,
				dprc_handle,
				&endpoint);

	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
	}
out:
	if (dprc_opened) {
		int error2;

		error2 = dprc_close(&restool.mc_io, 0, dprc_handle);
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

static int cmd_dpl_generate(void)
{
	int error;

	static const char usage_msg[] =
		"\n"
		"Usage: restool dprc generate-dpl <container>\n"
		"   <container> specifies the name of the container\n"
		"\n"
		"NOTES:\n"
		"Generates the DPL syntax for the specified container to stdout,\n"
		"including all child and decendant containers.\n"
		"\n"
		"EXAMPLE:\n"
		"Generate a DPL for dprc.1:\n"
		"   $ restool dprc generate-dpl dprc.1\n"
		"\n";

	if (restool.cmd_option_mask & ONE_BIT_MASK(GENERATE_OPT_HELP)) {
		puts(usage_msg);
		restool.cmd_option_mask &= ~ONE_BIT_MASK(GENERATE_OPT_HELP);
		return 0;
	}

	error = dpl_generate();

	return error;
}

/**
 * DPRC command table
 */
struct object_command dprc_commands[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dprc_help },

	{ .cmd_name = "sync",
	  .options = dprc_sync_options,
	  .cmd_func = cmd_dprc_sync },

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

	{ .cmd_name = "set-label",
	  .options = dprc_set_label_options,
	  .cmd_func = cmd_dprc_set_label },


	{ .cmd_name = "connect",
	  .options = dprc_connect_options,
	  .cmd_func = cmd_dprc_connect },

	{ .cmd_name = "disconnect",
	  .options = dprc_disconnect_options,
	  .cmd_func = cmd_dprc_disconnect },

	{ .cmd_name = "generate-dpl",
	  .options = dpl_generate_options,
	  .cmd_func = cmd_dpl_generate },

	{ .cmd_name = NULL },
};


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
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include "restool.h"
#include "utils.h"

static struct option global_options[] = {
	[GLOBAL_OPT_HELP] = {
		.name = "help",
		.val = 'h',
	},

	[GLOBAL_OPT_VERSION] = {
		.name = "version",
		.val = 'v',
	},

	[GLOBAL_OPT_MC_VERSION] = {
		.name = "mc-version",
		.val = 'm',
	},

	[GLOBAL_OPT_DEBUG] = {
		.name = "debug",
		.val = 'd',
	},

	[GLOBAL_OPT_SCRIPT] = {
		.name = "script",
		.val = 's',
	},

	[GLOBAL_OPT_ROOT] = {
		.name = "root",
		.val = 'r',
		.has_arg = optional_argument,
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(global_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static const struct obj_command_versions dprc_command_versions[] = {
	{ .version = 5, .obj_commands = dprc_commands },
	{ .version = 0, .obj_commands = NULL },
};
static const struct obj_command_versions dpni_command_versions[] = {
	{ .version = 6, .obj_commands = dpni_commands_v9 },
	{ .version = 7, .obj_commands = dpni_commands_v10 },
	{ .version = 0, .obj_commands = NULL },
};
static const struct obj_command_versions dpio_command_versions[] = {
	{ .version = 3, .obj_commands = dpio_commands_v9 },
	{ .version = 4, .obj_commands = dpio_commands_v10 },
	{ .version = 0, .obj_commands = NULL },
};
static const struct obj_command_versions dpbp_command_versions[] = {
	{ .version = 2, .obj_commands = dpbp_commands_v9 },
	{ .version = 3, .obj_commands = dpbp_commands_v10 },
	{ .version = 0, .obj_commands = NULL },
};
static const struct obj_command_versions dpsw_command_versions[] = {
	{ .version = 7, .obj_commands = dpsw_commands_v9 },
	{ .version = 8, .obj_commands = dpsw_commands_v10 },
	{ .version = 0, .obj_commands = NULL },
};
static const struct obj_command_versions dpci_command_versions[] = {
	{ .version = 2, .obj_commands = dpci_commands_v9 },
	{ .version = 3, .obj_commands = dpci_commands_v10 },
	{ .version = 0, .obj_commands = NULL },
};
static const struct obj_command_versions dpcon_command_versions[] = {
	{ .version = 2, .obj_commands = dpcon_commands_v9 },
	{ .version = 3, .obj_commands = dpcon_commands_v10 },
	{ .version = 0, .obj_commands = NULL },
};
static const struct obj_command_versions dpseci_command_versions[] = {
	{ .version = 3, .obj_commands = dpseci_commands_v9 },
	{ .version = 4, .obj_commands = dpseci_commands_v10 },
	{ .version = 0, .obj_commands = NULL },
};
static const struct obj_command_versions dpdmux_command_versions[] = {
	{ .version = 5, .obj_commands = dpdmux_commands_v9 },
	{ .version = 6, .obj_commands = dpdmux_commands_v10 },
	{ .version = 0, .obj_commands = NULL },
};
static const struct obj_command_versions dpmcp_command_versions[] = {
	{ .version = 3, .obj_commands = dpmcp_commands_v9 },
	{ .version = 4, .obj_commands = dpmcp_commands_v10 },
	{ .version = 0, .obj_commands = NULL },
};
static const struct obj_command_versions dpmac_command_versions[] = {
	{ .version = 3, .obj_commands = dpmac_commands_v9 },
	{ .version = 4, .obj_commands = dpmac_commands_v10 },
	{ .version = 0, .obj_commands = NULL },
};
static const struct obj_command_versions dpdcei_command_versions[] = {
	{ .version = 1, .obj_commands = dpdcei_commands_v9 },
	{ .version = 2, .obj_commands = dpdcei_commands_v10 },
	{ .version = 0, .obj_commands = NULL },
};
static const struct obj_command_versions dpaiop_command_versions[] = {
	{ .version = 1, .obj_commands = dpaiop_commands_v9 },
	{ .version = 2, .obj_commands = dpaiop_commands_v10 },
	{ .version = 0, .obj_commands = NULL },
};
static const struct obj_command_versions dpdbg_command_versions[] = {
	{ .version = 1, .obj_commands = dpdbg_commands },
	{ .version = 0, .obj_commands = NULL },
};
static const struct obj_command_versions dprtc_command_versions[] = {
	{ .version = 1, .obj_commands = dprtc_commands_v9 },
	{ .version = 2, .obj_commands = dprtc_commands_v10 },
	{ .version = 0, .obj_commands = NULL },
};

static const struct obj_command_versions dpdmai_command_versions[] = {
	{ .version = 2, .obj_commands = dpdmai_commands_v9 },
	{ .version = 3, .obj_commands = dpdmai_commands_v10 },
	{ .version = 0, .obj_commands = NULL },
};

static const struct object_cmd_parser object_cmd_parsers[] = {
	{ .obj_type = "dprc",   .obj_commands_versions = dprc_command_versions   },
	{ .obj_type = "dpni",   .obj_commands_versions = dpni_command_versions   },
	{ .obj_type = "dpio",   .obj_commands_versions = dpio_command_versions   },
	{ .obj_type = "dpbp",   .obj_commands_versions = dpbp_command_versions   },
	{ .obj_type = "dpsw",   .obj_commands_versions = dpsw_command_versions   },
	{ .obj_type = "dpci",   .obj_commands_versions = dpci_command_versions   },
	{ .obj_type = "dpcon",  .obj_commands_versions = dpcon_command_versions  },
	{ .obj_type = "dpseci", .obj_commands_versions = dpseci_command_versions },
	{ .obj_type = "dpdmux", .obj_commands_versions = dpdmux_command_versions },
	{ .obj_type = "dpmcp",  .obj_commands_versions = dpmcp_command_versions  },
	{ .obj_type = "dpmac",  .obj_commands_versions = dpmac_command_versions  },
	{ .obj_type = "dpdcei", .obj_commands_versions = dpdcei_command_versions },
	{ .obj_type = "dpaiop", .obj_commands_versions = dpaiop_command_versions },
	{ .obj_type = "dpdbg",  .obj_commands_versions = dpdbg_command_versions },
	{ .obj_type = "dprtc",  .obj_commands_versions = dprtc_command_versions },
	{ .obj_type = "dpdmai", .obj_commands_versions = dpdmai_command_versions },
};
/**
 * Individual object structs to hold the mapping of the MC Version
 * (major part only) to a corresponding object version(major part
 * only) supported by the MC.  Used in the lookup table below
 */
struct version_table dpaiop_version_table[] = {
	{ .mc_major_version = 9, .object_version = 1 },
	{ .mc_major_version = 10, .object_version = 2 },
	{ .mc_major_version = 0 }
};
struct version_table dpbp_version_table[] = {
	{ .mc_major_version = 9, .object_version = 2 },
	{ .mc_major_version = 10, .object_version = 3 },
	{ .mc_major_version = 0 }
};
struct version_table dpci_version_table[] = {
	{ .mc_major_version = 9, .object_version = 2 },
	{ .mc_major_version = 10, .object_version = 3 },
	{ .mc_major_version = 0 }
};
struct version_table dpcon_version_table[] = {
	{ .mc_major_version = 9, .object_version = 2 },
	{ .mc_major_version = 10, .object_version = 3 },
	{ .mc_major_version = 0 }
};
struct version_table dpdcei_version_table[] = {
	{ .mc_major_version = 9, .object_version = 1 },
	{ .mc_major_version = 10, .object_version = 2 },
	{ .mc_major_version = 0 }
};
struct version_table dpdmai_version_table[] = {
	{ .mc_major_version = 9, .object_version = 2 },
	{ .mc_major_version = 10, .object_version = 3 },
	{ .mc_major_version = 0 }
};
struct version_table dpdmux_version_table[] = {
	{ .mc_major_version = 8, .object_version = 4 },
	{ .mc_major_version = 9, .object_version = 5 },
	{ .mc_major_version = 10, .object_version = 6 },
	{ .mc_major_version = 0 }
};
struct version_table dpio_version_table[] = {
	{ .mc_major_version = 9, .object_version = 3 },
	{ .mc_major_version = 10, .object_version = 4 },
	{ .mc_major_version = 0 }
};
struct version_table dpmac_version_table[] = {
	{ .mc_major_version = 9, .object_version = 3 },
	{ .mc_major_version = 10, .object_version = 4 },
	{ .mc_major_version = 0 }
};
struct version_table dpmcp_version_table[] = {
	{ .mc_major_version = 9, .object_version = 3 },
	{ .mc_major_version = 10, .object_version = 4 },
	{ .mc_major_version = 0 }
};
struct version_table dpni_version_table[] = {
	{ .mc_major_version = 9, .object_version = 6 },
	{ .mc_major_version = 10, .object_version = 7 },
	{ .mc_major_version = 0 }
};
struct version_table dprc_version_table[] = {
	{ .mc_major_version = 8, .object_version = 5 },
	{ .mc_major_version = 9, .object_version = 5 },
	{ .mc_major_version = 10, .object_version = 5 },
	{ .mc_major_version = 0 }
};
struct version_table dpseci_version_table[] = {
	{ .mc_major_version = 8, .object_version = 3 },
	{ .mc_major_version = 9, .object_version = 3 },
	{ .mc_major_version = 10, .object_version = 4 },
	{ .mc_major_version = 0 }
};
struct version_table dpsw_version_table[] = {
	{ .mc_major_version = 9, .object_version = 7 },
	{ .mc_major_version = 10, .object_version = 8 },
	{ .mc_major_version = 0 }
};
struct version_table dpdbg_version_table[] = {
	{ .mc_major_version = 9, .object_version = 1 },
	{ .mc_major_version = 10, .object_version = 1 },
	{ .mc_major_version = 0 }
};
struct version_table dprtc_version_table[] = {
	{ .mc_major_version = 9, .object_version = 1 },
	{ .mc_major_version = 10, .object_version = 2 },
	{ .mc_major_version = 0 }
};

/**
 * Lookup table used to map a specific MC Version to its corresponding
 * supported object version
 */
struct lut_entry version_lut[] = {
	{ .object = "dpaiop", .versions_table = dpaiop_version_table },
	{ .object = "dpbp",   .versions_table = dpbp_version_table   },
	{ .object = "dpci",   .versions_table = dpci_version_table   },
	{ .object = "dpcon",  .versions_table = dpcon_version_table  },
	{ .object = "dpdcei", .versions_table = dpdcei_version_table  },
	{ .object = "dpdmai", .versions_table = dpdmai_version_table },
	{ .object = "dpdmux", .versions_table = dpdmux_version_table },
	{ .object = "dpio",   .versions_table = dpio_version_table   },
	{ .object = "dpmac",  .versions_table = dpmac_version_table  },
	{ .object = "dpmcp",  .versions_table = dpmcp_version_table  },
	{ .object = "dpni",   .versions_table = dpni_version_table   },
	{ .object = "dprc",   .versions_table = dprc_version_table   },
	{ .object = "dpseci", .versions_table = dpseci_version_table },
	{ .object = "dpsw",   .versions_table = dpsw_version_table   },
	{ .object = "dpdbg",  .versions_table = dpdbg_version_table  },
	{ .object = "dprtc",  .versions_table = dprtc_version_table  },
};

struct restool restool;

enum mc_cmd_status flib_error_to_mc_status(int error)
{
	switch (error) {
	case 0:
		return MC_CMD_STATUS_OK;
	case -EACCES:
		return MC_CMD_STATUS_AUTH_ERR;
	case -EPERM:
		return MC_CMD_STATUS_NO_PRIVILEGE;
	case -EIO:
		return MC_CMD_STATUS_DMA_ERR;
	case -ENXIO:
		return MC_CMD_STATUS_CONFIG_ERR;
	case -ETIMEDOUT:
		return MC_CMD_STATUS_TIMEOUT;
	case -ENAVAIL:
		return MC_CMD_STATUS_NO_RESOURCE;
	case -ENOMEM:
		return MC_CMD_STATUS_NO_MEMORY;
	case -EBUSY:
		return MC_CMD_STATUS_BUSY;
	case -524:
		/* #define ENOTSUPP 524 in Linux, no ENOTSUPP in user space */
		return MC_CMD_STATUS_UNSUPPORTED_OP;
	case -ENODEV:
		return MC_CMD_STATUS_INVALID_STATE;
	default:
		break;
	}

	/* Not expected to reach here */
	return error;	/* 1000 == 0x3e8 */
}

const char *mc_status_to_string(enum mc_cmd_status status)
{
	static const char *const status_strings[] = {
		[MC_CMD_STATUS_OK] = "Command completed successfully",
		[MC_CMD_STATUS_READY] = "Command ready to be processed",
		[MC_CMD_STATUS_AUTH_ERR] = "Authentication error",
		[MC_CMD_STATUS_NO_PRIVILEGE] = "No privilege",
		[MC_CMD_STATUS_DMA_ERR] = "DMA or I/O error",
		[MC_CMD_STATUS_CONFIG_ERR] = "Configuration error",
		[MC_CMD_STATUS_TIMEOUT] = "Operation timed out",
		[MC_CMD_STATUS_NO_RESOURCE] = "No resources",
		[MC_CMD_STATUS_NO_MEMORY] = "No memory available",
		[MC_CMD_STATUS_BUSY] = "Device is busy",
		[MC_CMD_STATUS_UNSUPPORTED_OP] = "Unsupported operation",
		[MC_CMD_STATUS_INVALID_STATE] = "Invalid state"
	};

	if ((unsigned int)status >= ARRAY_SIZE(status_strings))
		return "Unknown MC error";

	return status_strings[status];
}

int find_target_obj_desc(uint32_t dprc_id, uint16_t dprc_handle,
			int nesting_level,
			uint32_t target_id, char *target_type,
			struct dprc_obj_desc *target_obj_desc,
			uint32_t *target_parent_dprc_id, bool *found)
{
	int num_child_devices;
	int error = 0;
	enum mc_cmd_status mc_status;

	assert(nesting_level <= MAX_DPRC_NESTING);

	if (strcmp(target_type, "dprc") == 0 &&
	    target_id == restool.root_dprc_id) {
		DEBUG_PRINTF("This is root dprc.\n");
		strcpy(target_obj_desc->type, "dprc");
		target_obj_desc->id = 1;
		return 0;
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

		DEBUG_PRINTF("it is %s.%u\n", obj_desc.type, obj_desc.id);

		if (strcmp(obj_desc.type, target_type) == 0 &&
		    target_id == (uint32_t)obj_desc.id) {
			*target_obj_desc = obj_desc;
			*target_parent_dprc_id = dprc_id;
			DEBUG_PRINTF("target_parent_dprc_id: dprc.%d\n",
					dprc_id);
			DEBUG_PRINTF("object found\n");
			*found = true;
			goto out;
		} else if (strcmp(obj_desc.type, "dprc") == 0) {
			bool found2 = false;

			error = open_dprc(obj_desc.id, &child_dprc_handle);
			if (error < 0)
				goto out;

			DEBUG_PRINTF("entering %s.%u\n", obj_desc.type,
					obj_desc.id);
			error = find_target_obj_desc(obj_desc.id,
					child_dprc_handle,
					nesting_level + 1,
					target_id,
					target_type,
					target_obj_desc,
					target_parent_dprc_id,
					&found2);

			error2 = dprc_close(&restool.mc_io, 0,
						child_dprc_handle);
			if (error2 < 0) {
				mc_status = flib_error_to_mc_status(error2);
				ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status), mc_status);
				if (error == 0)
					error = error2;

				goto out;
			}

			DEBUG_PRINTF("exiting %s.%u\n", obj_desc.type,
					obj_desc.id);
			if (found2) {
				*found = found;
				goto out;
			}
		} else {
			continue;
		}
	}

out:
	return error;
}

bool find_obj(char *obj_type, uint32_t obj_id)
{
	struct dprc_obj_desc target_obj_desc;
	uint32_t target_parent_dprc_id;
	int error;
	bool found = false;

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
				restool.root_dprc_handle, 0, obj_id,
				obj_type, &target_obj_desc,
				&target_parent_dprc_id, &found);

	if (!found) {
		if (error == 0)
			printf("%s.%u does not exist\n", obj_type, obj_id);
		return false;
	}

	return true;
}

void print_obj_label(struct dprc_obj_desc *target_obj_desc)
{
	assert(strlen(target_obj_desc->label) <= MC_OBJ_LABEL_MAX_LENGTH);
	if (!(target_obj_desc->id == (int)restool.root_dprc_id &&
	    strcmp(target_obj_desc->type, "dprc") == 0) &&
	    strlen(target_obj_desc->label) > 0)
		printf("object label: %s\n", target_obj_desc->label);
}

int print_obj_verbose(struct dprc_obj_desc *target_obj_desc,
			const struct flib_ops *ops)
{
	uint16_t obj_handle;
	uint32_t irq_mask;
	uint32_t irq_status;
	enum mc_cmd_status mc_status;
	int error = 0;

	if (strcmp(target_obj_desc->type, "dprc") == 0 &&
	    target_obj_desc->id == (int)restool.root_dprc_id) {
		printf("number of mappable regions: 1\n");
		printf("number of interrupts: 1\n");
		error = dprc_get_irq_mask(&restool.mc_io, 0,
				restool.root_dprc_handle, 0, &irq_mask);
		if (error < 0) {
			mc_status = flib_error_to_mc_status(error);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status), mc_status);
		return error;
		}
		printf("interrupt[0] mask: %#x\n", irq_mask);
		error = dprc_get_irq_status(&restool.mc_io, 0,
				restool.root_dprc_handle, 0, &irq_status);
		if (error < 0) {
			mc_status = flib_error_to_mc_status(error);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status), mc_status);
		return error;
		}

		printf("interrupt[0] status: %#x\n", irq_status);
		return 0;
	}

	printf("number of mappable regions: %u\n",
		target_obj_desc->region_count);
	printf("number of interrupts: %u\n", target_obj_desc->irq_count);

	error = ops->obj_open(&restool.mc_io, 0, target_obj_desc->id,
				&obj_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			mc_status_to_string(mc_status), mc_status);
		return error;
	}

	for (int j = 0; j < target_obj_desc->irq_count; j++) {
		ops->obj_get_irq_mask(&restool.mc_io, 0, obj_handle, j,
					&irq_mask);
		printf("interrupt[%d] mask: %#x\n", j, irq_mask);
		ops->obj_get_irq_status(&restool.mc_io, 0, obj_handle, j,
					&irq_status);
		printf("interrupt[%d] status: %#x\n", j, irq_status);
	}

	error = ops->obj_close(&restool.mc_io, 0, obj_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			mc_status_to_string(mc_status), mc_status);
	}

	return error;
}

int check_resource_type(char *res_type)
{
	if (strcmp(res_type, "bp") == 0 ||
	    strcmp(res_type, "cg") == 0 ||
	    strcmp(res_type, "fq") == 0 ||
	    strcmp(res_type, "mcp") == 0 ||
	    strcmp(res_type, "qpr") == 0 ||
	    strcmp(res_type, "qd") == 0 ||
	    strcmp(res_type, "rplr") == 0)
		return 0;

	ERROR_PRINTF(
		"Entered resource type: %s.\n"
		"It should be within \"bp, cg, fq, mcp, qpr, qd, rplr\"\n",
		res_type);
	return -EINVAL;
}

void diff_time(struct timespec *start, struct timespec *end,
		struct timespec *latency)
{
	if (end->tv_nsec - start->tv_nsec < 0) {
		latency->tv_sec = end->tv_sec - start->tv_sec - 1;
		latency->tv_nsec = 1000000000UL + end->tv_nsec - start->tv_nsec;
	} else {
		latency->tv_sec = end->tv_sec - start->tv_sec;
		latency->tv_nsec = end->tv_nsec - start->tv_nsec;
	}
}

bool in_use(const char *obj, const char *situation)
{
	ssize_t r;
	int n;
	char symbolic[PATH_MAX] = {'\0'};
	char linkname[PATH_MAX] = {'\0'};

	n = snprintf(symbolic, PATH_MAX,
			"/sys/bus/fsl-mc/devices/%s/driver", obj);
	DEBUG_PRINTF("n = %d\n", n);
	if (n < 0 || n > PATH_MAX - 1)
		ERROR_PRINTF("symbolic link copy error.\n");
	DEBUG_PRINTF("symbolic=%s\n", symbolic);
	r = readlink(symbolic, linkname, PATH_MAX - 1);
	if (r != -1)
		linkname[r] = '\0';
	DEBUG_PRINTF("readlink's errno = %d\n", errno);
	DEBUG_PRINTF("linkname=%s\n", linkname);
	DEBUG_PRINTF("r = %d\n", (int)r);
	if (r > 0) {
		ERROR_PRINTF(
			"%s cannot be %s because it is bound to driver:\n"
			"%s -> %s\n"
			"unbind it first\n",
			obj, situation, symbolic, linkname);
		return true;
	}
	return false;
}

void print_new_obj(char *type, int id, const char *parent)
{
	if (restool.script) {
		printf("%s.%d\n", type, id);
		return;
	}

	if (parent == NULL) { /* by default, parent == dprc.1 */
		printf("%s.%d is created under dprc.1\n", type, id);
		return;
	}

	printf("%s.%d is created under %s\n", type, id, parent);
}

void print_unexpected_options_error(uint32_t option_mask,
				    const struct option *options)
{
	ERROR_PRINTF("Invalid options:\n");
	for (unsigned int i = 0; i < MAX_NUM_CMD_LINE_OPTIONS; i++) {
		if (option_mask & ONE_BIT_MASK(i))
			fprintf(stderr, "\t--%s\n", options[i].name);

	}
}

int get_option_value(int option, long *value, const char *error_msg, int min, int max)
{
	char *str, *endptr;
	long val;

	errno = 0;
	str = restool.cmd_option_args[option];
	val = strtol(str, &endptr, 0);

	if (STRTOL_ERROR(str, endptr, val, errno) ||
	    (val < min || val > max)) {
		ERROR_PRINTF("%s\n", error_msg);
		return -EINVAL;
	}

	*value = val;

	return 0;
}

int get_parent_dprc_id(uint32_t obj_id, char *obj_type, uint32_t *parent_dprc_id)
{
	struct dprc_obj_desc target_obj_desc;
	bool found = false;
	int error;

	memset(&target_obj_desc, 0, sizeof(struct dprc_obj_desc));
	error = find_target_obj_desc(restool.root_dprc_id,
				restool.root_dprc_handle, 0, obj_id,
				obj_type, &target_obj_desc,
				parent_dprc_id, &found);
	if (error || strcmp(target_obj_desc.type, obj_type))
		return -EINVAL;

	return 0;
}

static void print_usage(void)
{
	static const char usage_msg[] =
		"OVERVIEW\n"
		"\n"
		"restool provides the ability to dynamically create and manage DPAA2\n"
		"objects and containers.\n"
		"\n"
		"  For general help info:\n"
		"    restool --help\n"
		"\n"
		"  To see the commands each object suppports:\n"
		"    restool <object-type> help\n"
		"\n"
		"  For object type and command-specific help:\n"
		"    restool <object-type> <command> --help\n"
		"\n"
		"SYNOPSIS\n"
		"\n"
		"  restool [<global-opts>] <object-type> <command> <object-name> [ARGS...]\n"
		"\n"
		"OPTIONS\n"
		"\n"
		"  Valid <global-opts> are:\n"
		"   -v,--version     Displays tool version info\n"
		"   -m,--mc-version  Displays mc firmware version\n"
		"   -h,-?,--help     Displays general help info\n"
		"   -s, --script     Display script friendly output\n"
		"   --root=[dprc]    Specifies root container name\n"
		"\n"
		"  Valid <object-type> values: <dprc|dpni|dpio|dpsw|dpbp|dpci|dpcon|dpseci|dpdmux|\n"
		"                               dpmcp|dpmac|dpdcei|dpaiop|dpdmai>\n"
		"\n"
		"  Valid commands vary for each object type.\n"
		"  Most objects support the following commands:\n"
		"    help\n"
		"    info\n"
		"    create\n"
		"    destroy\n"
		"\n"
		"  <object-name> is a string containing object type and ID (e.g. dpni.7)\n"
		"\n";

	puts(usage_msg);
	restool.global_option_mask &= ~ONE_BIT_MASK(GLOBAL_OPT_HELP);
}

static void print_usage_v9(void)
{
	static const char usage_msg[] =
		"OVERVIEW\n"
		"\n"
		"  For general help info:\n"
		"    restool --help\n"
		"\n"
		"  To see the commands each object suppports:\n"
		"    restool <object-type> help\n"
		"\n"
		"  For object type and command-specific help:\n"
		"    restool <object-type> <command> --help\n"
		"\n"
		"SYNOPSIS\n"
		"\n"
		"  restool [<global-opts>] <object-type> <command> <object-name> [ARGS...]\n"
		"\n"
		"OPTIONS\n"
		"\n"
		"  Valid <global-opts> are:\n"
		"   -v,--version     Displays tool version info\n"
		"   -m,--mc-version  Displays mc firmware version\n"
		"   -h,-?,--help     Displays general help info\n"
		"   -s, --script     Display script friendly output\n"
		"   --root=[dprc]    Specifies root container name\n"
		"\n"
		"  Valid <object-type> values: <dprc|dpni|dpio|dpsw|dpbp|dpci|dpcon|dpseci|dpdmux|\n"
		"                               dpmcp|dpmac|dpdcei|dpaiop|dprtc|dpdmai>\n"
		"\n"
		"  Valid commands vary for each object type.\n"
		"  Most objects support the following commands:\n"
		"    help\n"
		"    info\n"
		"    create\n"
		"    destroy\n"
		"\n"
		"  <object-name> is a string containing object type and ID (e.g. dpni.7)\n"
		"\n";

	puts(usage_msg);
	restool.global_option_mask &= ~ONE_BIT_MASK(GLOBAL_OPT_HELP);
}

static void print_try_help(void)
{
	ERROR_PRINTF("try 'restool --help'\n");
}

static void print_version(void)
{
	printf("restool %s\n", VERSION);
	restool.global_option_mask &= ~ONE_BIT_MASK(GLOBAL_OPT_VERSION);
}

static void print_mc_version(void)
{
	printf("MC firmware version: %u.%u.%u\n",
	       restool.mc_fw_version.major,
	       restool.mc_fw_version.minor,
	       restool.mc_fw_version.revision);

	restool.global_option_mask &= ~ONE_BIT_MASK(GLOBAL_OPT_MC_VERSION);
}

int parse_object_name(const char *obj_name, char *expected_obj_type,
		      uint32_t *obj_id)
{
	int n;
	char obj_type[OBJ_TYPE_MAX_LENGTH + 1];

	assert(expected_obj_type != NULL);
	n = sscanf(obj_name, "%" STRINGIFY(OBJ_TYPE_MAX_LENGTH) "[a-z].%u",
		   obj_type, obj_id);
	if (n != 2) {
		ERROR_PRINTF("Invalid MC object name: %s\n", obj_name);
		return -EINVAL;
	}

	if (strcmp(obj_type, expected_obj_type) != 0) {
		ERROR_PRINTF("Expected \'%s\' object type\n",
			     expected_obj_type);
		return -EINVAL;
	}

	return 0;
}

int open_dprc(uint32_t dprc_id, uint16_t *dprc_handle)
{
	int error;
	enum mc_cmd_status mc_status;

	error = dprc_open(&restool.mc_io, 0,
			  dprc_id,
			  dprc_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	if (*dprc_handle == 0) {
		DEBUG_PRINTF(
			"dprc_open() returned invalid handle (auth 0) for dprc.%u\n",
			dprc_id);

		(void)dprc_close(&restool.mc_io, 0, *dprc_handle);
		error = -ENOENT;
		goto out;
	}

	error = 0;
out:
	return error;
}

static int check_arg(char *optarg)
{
	int str_len = 0;

	str_len = strlen(optarg);

	if (optarg == NULL || str_len > USR_DEV_FILE_SIZE)
		str_len = -1;
	else {
		char *check = "dprc.";

		if (strncmp(optarg, check, 5) != 0)
			str_len = -1;
	}
	return str_len;
}

static int parse_global_options(int argc, char *argv[],
				int *next_argv_index)
{
	int c;
	int opt_index;

	/*
	 * Initialize getopt global variables:
	 */
	optind = 1;
	optarg = NULL;

	restool.global_option_mask = 0;
	for ( ; ; ) {
		opt_index = 0;
		c = getopt_long(argc, argv, "+h?vmds", global_options, NULL);
		DEBUG_PRINTF("c=%d\n", c);
		DEBUG_PRINTF("optopt=%d\n", optopt);

		if (c == -1)
			break;

		switch (c) {
		case 'h':
		case '?':
			if (optopt == 0) {
				opt_index = GLOBAL_OPT_HELP;
				break;
			}
			print_try_help();
			return -EINVAL;

		case 'v':
			opt_index = GLOBAL_OPT_VERSION;
			break;

		case 'm':
			opt_index = GLOBAL_OPT_MC_VERSION;
			break;

		case 'd':
			opt_index = GLOBAL_OPT_DEBUG;
			break;

		case 's':
			opt_index = GLOBAL_OPT_SCRIPT;
			break;

		case 'r':
			opt_index = GLOBAL_OPT_ROOT;
			int str_len = check_arg(optarg);
			int size_buff;

			if (str_len < 0) {
				ERROR_PRINTF("Invalid Argument: incorrect value for root\n");
				return str_len;
			}

			size_buff = sprintf(restool.specified_dev_file, "%s", optarg);
			if (size_buff != str_len) {
				ERROR_PRINTF("sprintf failed\n");
				return -1;
			}

			break;

		default:
			DEBUG_PRINTF("\n");
			assert(false);
			DEBUG_PRINTF("\n");
		}

		assert((unsigned int)opt_index < MAX_NUM_CMD_LINE_OPTIONS);
		if (restool.global_option_mask & ONE_BIT_MASK(opt_index)) {
			ERROR_PRINTF("Duplicated option: %s\n",
				     global_options[opt_index].name);
			return -EINVAL;
		}

		restool.global_option_mask |= ONE_BIT_MASK(opt_index);
		if (global_options[opt_index].has_arg)
			restool.global_option_args[opt_index] = optarg;
		else
			restool.global_option_args[opt_index] = NULL;
	}

	DEBUG_PRINTF("optind: %d, argc: %d\n", optind, argc);
	*next_argv_index = optind;
	DEBUG_PRINTF("\n");
	return 0;
}

static int parse_cmd_options(int argc, char *argv[],
			     const struct option options[],
			     int *next_argv_index)
{
	int c;
	int opt_index;
	int error;

	/*
	 * Initialize getopt global variables:
	 */
	optind = 1;
	optarg = NULL;

	restool.cmd_option_mask = 0;
	assert(options != NULL);

	for ( ; ; ) {
		c = getopt_long_only(argc, argv, "+", options, &opt_index);
		if (c == -1)
			break;

		if (c != 0) {
			error = -EINVAL;
			goto error;
		}

		assert((unsigned int)opt_index < MAX_NUM_CMD_LINE_OPTIONS);
		if (restool.cmd_option_mask & ONE_BIT_MASK(opt_index)) {
			ERROR_PRINTF("Duplicated option: %s\n",
				     options[opt_index].name);
			error = -EINVAL;
			goto error;
		}

		restool.cmd_option_mask |= ONE_BIT_MASK(opt_index);
		if (options[opt_index].has_arg)
			restool.cmd_option_args[opt_index] = optarg;
		else
			restool.cmd_option_args[opt_index] = NULL;
	}

	DEBUG_PRINTF("optind: %d, argc: %d\n", optind, argc);
	*next_argv_index = optind;
	return 0;
error:
	return error;
}
/*
 * This function can be used to get the supported obj version(major) for a
 * specific object and your current MC Firmware Version
 */
static uint16_t get_obj_version(const char *obj_type)
{
	unsigned int i;
	uint16_t obj_version = 0;
	uint32_t mc_major_version = restool.mc_fw_version.major;
	struct lut_entry *lut_obj_entry = NULL;
	struct version_table *versions_table;

	/*
	 * get the specific object version lookup table
	 */
	for (i = 0; i < ARRAY_SIZE(version_lut); i++) {
		if (strcmp(obj_type, version_lut[i].object) == 0)
			lut_obj_entry = &version_lut[i];
	}

	if (lut_obj_entry == NULL) {
		ERROR_PRINTF("error: invalid object type \'%s\'\n", obj_type);
		goto out;
	}

	/*
	 * find the supported object version number from the MC Version
	 */
	versions_table = lut_obj_entry->versions_table;
	for (i = 0; versions_table[i].mc_major_version != 0; i++) {
		if (mc_major_version == versions_table[i].mc_major_version)
			obj_version = versions_table[i].object_version;
	}

	if (obj_version == 0) {
		ERROR_PRINTF("error: invalid MC firmware version %d for object type \'%s\'\n",
			     mc_major_version, obj_type);
		goto out;
	}

out:
	return obj_version;
}

static struct object_command *get_obj_cmd(const char *obj_type,
					  const char *cmd_name)
{
	unsigned int i;
	const struct object_cmd_parser *obj_cmd_parser = NULL;
	const struct obj_command_versions *obj_cmd_versions;
	struct object_command *obj_commands;
	struct object_command *obj_cmd = NULL;
	uint16_t obj_version;

	/*
	 * Lookup object command parser:
	 */
	for (i = 0; i < ARRAY_SIZE(object_cmd_parsers); i++) {
		if (strcmp(obj_type, object_cmd_parsers[i].obj_type) == 0) {
			obj_cmd_parser = &object_cmd_parsers[i];
			break;
		}
	}

	if (obj_cmd_parser == NULL) {
		ERROR_PRINTF("error: invalid object type \'%s\'\n", obj_type);
		print_try_help();
		goto out;
	}

	/*
	 * lookup object version number supported by MC firmware version
	 */
	obj_version = get_obj_version(obj_type);
	if (obj_version == 0)
		goto out;

	/*
	 * Find the right object_command struct assosiates with version
	 */
	obj_cmd_versions = obj_cmd_parser->obj_commands_versions;
	for (i = 0; obj_cmd_versions[i].obj_commands != NULL; i++) {
		if (obj_version ==  obj_cmd_versions[i].version)
			obj_commands = obj_cmd_versions[i].obj_commands;
	}

	if (obj_commands == NULL) {
		ERROR_PRINTF("error: invalid object version \'%u\'\n",
			     obj_version);
		goto out;
	}

	/*
	 * Lookup object-level command:
	 */
	for (i = 0; obj_commands[i].cmd_name != NULL; i++) {
		if (strcmp(cmd_name, obj_commands[i].cmd_name) == 0) {
			obj_cmd = &obj_commands[i];
			break;
		}
	}

	if (obj_cmd == NULL) {
		ERROR_PRINTF("Invalid command \'%s\' for object type \'%s\'\n",
			     cmd_name, obj_type);
		print_try_help();
		goto out;
	}

out:
	return obj_cmd;
}

static int parse_obj_command(const char *obj_type,
			     const char *cmd_name,
			     int argc,
			     char *argv[])
{
	int error;
	int next_argv_index;
	struct object_command *obj_cmd = NULL;
	struct timespec start_time = { 0 };
	struct timespec end_time = { 0 };
	struct timespec latency = { 0 };

	assert(argv[0] == cmd_name);
	obj_cmd = get_obj_cmd(obj_type, cmd_name);
	restool.obj_cmd = obj_cmd;
	if (restool.obj_cmd == NULL) {
		error = -EINVAL;
		goto out;
	}
	if (argc >= 2 && argv[1][0] != '-') {
		restool.obj_name = argv[1];
		argv++;
		argc--;
	} else {
		restool.obj_name = NULL;
	}
	/*
	 * Parse object-level command options:
	 */
	if (obj_cmd->options != NULL) {
		error = parse_cmd_options(argc, argv,
					  obj_cmd->options,
					  &next_argv_index);

		if (error != 0)
			goto out;

		if (next_argv_index != argc) {
			assert(next_argv_index < argc);
			ERROR_PRINTF("Invalid command line\n");
			print_try_help();
			error = -EINVAL;
			goto out;
		}
	} else {
		if (argc != 1) {
			ERROR_PRINTF("Invalid command line\n");
			print_try_help();
			error = -EINVAL;
			goto out;
		}
	}

	/*
	 * Execute object-level command:
	 */
	clock_gettime(CLOCK_REALTIME, &start_time);

	error = obj_cmd->cmd_func();

	clock_gettime(CLOCK_REALTIME, &end_time);
	diff_time(&start_time, &end_time, &latency);
	DEBUG_PRINTF("It takes %ld.%ld seconds to run command\n",
		latency.tv_sec, latency.tv_nsec);

	if (error < 0)
		goto out;
	if (restool.cmd_option_mask != 0) {
		print_unexpected_options_error(restool.cmd_option_mask,
					       obj_cmd->options);
		error = -EINVAL;
	}
out:
	return error;
}

static int get_device_file(void)
{
	int error = 0;
	int num_char;

	memset(restool.device_file, '\0', DEV_FILE_SIZE);

	if (restool.specified_dev_file[0] != '\0') {
		int temp_len = strlen(restool.specified_dev_file);

		temp_len += 5;
		num_char = sprintf(restool.device_file, "/dev/%s",
				   restool.specified_dev_file);
		if (num_char != temp_len) {
			ERROR_PRINTF("sprintf failed\n");
			error = -1;
			goto out;
		}
		if (access(restool.device_file, F_OK) != 0) {
			error = -1;
			ERROR_PRINTF("error: %s does not exist\n",
				     restool.device_file);
		}
	} else if (access("/dev/mc_restool", F_OK) == 0) {
		num_char = sprintf(restool.device_file, "/dev/mc_restool");
		if (num_char != 15) {
			ERROR_PRINTF("sprintf failed\n");
			error = -1;
			goto out;
		}
	} else {
		DIR           *d;
		struct dirent *dir;
		int num_dev_files = 0;
		char *dprc_index;

		d = opendir("/dev");
		if (!d) {
			ERROR_PRINTF("error opening directory /dev\n");
			error = -1;
			goto out;
		}
		while ((dir = readdir(d)) != NULL) {
			if (strncmp(dir->d_name, "dprc.", 5) == 0) {
				dprc_index = &dir->d_name[5];
				num_dev_files += 1;
			}
		}
		closedir(d);

		if (num_dev_files == 1) {
			int temp_len = strlen(dprc_index);

			temp_len += 10;
			num_char = sprintf(restool.device_file, "/dev/dprc.%s",
					   dprc_index);
			if (num_char != temp_len) {
				ERROR_PRINTF("sprintf error\n");
				error = -1;
				goto out;
			}
			restool.root_dprc_id = atoi(dprc_index);
			if (access(restool.device_file, F_OK) != 0)
				printf("no such dev file\n");
		} else {
			error = -1;
			if (num_dev_files == 0)
				ERROR_PRINTF("error: Did not find a device file\n");
			else
				ERROR_PRINTF("error: multiple root containers\n");
		}
	}
out:
	return error;
}

static int open_root_container(void)
{
	int error;
	uint32_t root_dprc_id;

	if (strcmp(restool.device_file, "/dev/mc_restool") == 0) {
		DEBUG_PRINTF("calling ioctl(RESTOOL_GET_ROOT_DPRC_INFO)\n");
		error = ioctl(restool.mc_io.fd,
			      RESTOOL_GET_ROOT_DPRC_INFO,
			      &root_dprc_id);
		if (error == -1) {
			error = -errno;
			return error;
		}

		DEBUG_PRINTF("ioctl returned MC-bus's root_dprc_id: %#x\n",
			     root_dprc_id);
	} else {
		char *dev_file = restool.device_file;

		root_dprc_id = atoi(&dev_file[10]);
	}

	restool.root_dprc_id = root_dprc_id;
	error = open_dprc(restool.root_dprc_id,
			  &restool.root_dprc_handle);
	return error;
}

int main(int argc, char *argv[])
{
	int error;
	int next_argv_index;
	const char *obj_type;
	const char *cmd_name;
	bool mc_io_initialized = false;
	bool root_dprc_opened = false;
	enum mc_cmd_status mc_status;
	bool talk_to_mc = true;

	#ifdef DEBUG
	restool.debug = true;
	#endif

	memset(restool.specified_dev_file, '\0', USR_DEV_FILE_SIZE);

	error = parse_global_options(argc, argv, &next_argv_index);
	if (error < 0)
		goto out;

	error = get_device_file();
	if (error < 0)
		goto out;

	DEBUG_PRINTF("restool built on " __DATE__ " " __TIME__ "\n");
	error = mc_io_init(&restool.mc_io);
	if (error != 0)
		goto out;

	mc_io_initialized = true;
	DEBUG_PRINTF("restool.mc_io.fd: %d\n", restool.mc_io.fd);

	error = mc_get_version(&restool.mc_io, 0,
				&restool.mc_fw_version);
	if (error != 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	if (restool.mc_fw_version.major < 9) {
		ERROR_PRINTF("This version of restool does no longer support MC\
			     firmware versions lower than v9. \
			     Please use restool v1.5\n");
	}

	DEBUG_PRINTF("MC firmware version: %u.%u.%u\n",
		     restool.mc_fw_version.major,
		     restool.mc_fw_version.minor,
		     restool.mc_fw_version.revision);

	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0 ||
			strcmp(argv[i], "--version") == 0 ||
			strcmp(argv[i], "-h") == 0 ||
			strcmp(argv[i], "-?") == 0 ||
			strcmp(argv[i], "--help") == 0 ||
			strcmp(argv[i], "help") == 0) {
			talk_to_mc = false;
			break;
		}
	}

	DEBUG_PRINTF("talk_to_mc = %d\n", talk_to_mc);
	if (talk_to_mc) {

		error = open_root_container();

		if (error < 0)
			goto out;

		DEBUG_PRINTF("newly opened restool's root_dprc_handle: %#x\n",
			     restool.root_dprc_handle);
		root_dprc_opened = true;
	}

	if (error < 0)
		goto out;

	if (next_argv_index == argc) {
		if (restool.global_option_mask == 0) {
			ERROR_PRINTF("Incomplete command line\n");
			print_try_help();
			error = -EINVAL;
			goto out;
		}

		if (restool.global_option_mask &
		    ONE_BIT_MASK(GLOBAL_OPT_HELP)) {
			if (restool.mc_fw_version.major == 8)
				print_usage();
			else if (restool.mc_fw_version.major == 9)
				print_usage_v9();
			else if (restool.mc_fw_version.major == 10)
				print_usage_v9();
		}

		if (restool.global_option_mask &
		    ONE_BIT_MASK(GLOBAL_OPT_VERSION))
			print_version();

		if (restool.global_option_mask &
		    ONE_BIT_MASK(GLOBAL_OPT_MC_VERSION))
			print_mc_version();

		if (restool.global_option_mask &
		    ONE_BIT_MASK(GLOBAL_OPT_DEBUG)) {
			restool.global_option_mask &=
				~ONE_BIT_MASK(GLOBAL_OPT_DEBUG);
			print_try_help();
			error = -EINVAL;
			goto out;
		}

		if (restool.global_option_mask &
		    ONE_BIT_MASK(GLOBAL_OPT_SCRIPT)) {
			restool.global_option_mask &=
				~ONE_BIT_MASK(GLOBAL_OPT_SCRIPT);
			print_try_help();
			error = -EINVAL;
			goto out;
		}

		if (restool.global_option_mask != 0) {
			print_unexpected_options_error(
				restool.global_option_mask,
				global_options);
			error = -EINVAL;
		}
	} else {
		if (restool.global_option_mask &
		    ONE_BIT_MASK(GLOBAL_OPT_DEBUG)) {
			restool.global_option_mask &=
				~ONE_BIT_MASK(GLOBAL_OPT_DEBUG);
			restool.debug = true;
		}

		if (restool.global_option_mask &
		    ONE_BIT_MASK(GLOBAL_OPT_SCRIPT)) {
			restool.global_option_mask &=
				~ONE_BIT_MASK(GLOBAL_OPT_SCRIPT);
			restool.script = true;
		}

		if (restool.global_option_mask &
		    ONE_BIT_MASK(GLOBAL_OPT_ROOT)) {
			restool.global_option_mask &=
				~ONE_BIT_MASK(GLOBAL_OPT_ROOT);
		}

		int num_remaining_args;

		assert(next_argv_index < argc);
		if (restool.global_option_mask != 0) {
			print_unexpected_options_error(
				restool.global_option_mask,
				global_options);
			print_try_help();
			error = -EINVAL;
			goto out;
		}

		num_remaining_args = argc - next_argv_index;
		if (num_remaining_args < 2) {
			ERROR_PRINTF("Incomplete command line\n");
			print_try_help();
			error = -EINVAL;
			goto out;
		}

		obj_type = argv[next_argv_index];
		cmd_name = argv[next_argv_index + 1];
		error = parse_obj_command(obj_type,
					  cmd_name,
					  num_remaining_args - 1,
					  &argv[next_argv_index + 1]);
		if (error < 0)
			goto out;
	}

	DEBUG_PRINTF("calling sytem()\n");
	error = system("echo 1 > /sys/bus/fsl-mc/rescan");
	if (error == -1) {
		error = -errno;
		DEBUG_PRINTF(
			"fsl-mc bus rescan failed (error %d)\n", error);
		goto out;
	}

out:
	if (root_dprc_opened) {
		int error2;

		error2 = dprc_close(&restool.mc_io, 0,
					restool.root_dprc_handle);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status), mc_status);
			if (error == 0)
				error = error2;
		}
	}
	if (mc_io_initialized)
		mc_io_cleanup(&restool.mc_io);

	return error;
}

static int uint64t_from_string(uint64_t *result, char *string)
{
	int errno_saved;
	char *p;

	errno_saved = errno;
	errno = 0;
	*result = strtoull(string, &p, 0);

	if (errno) {
		DEBUG_PRINTF("Integer overflow occured while reading \"%s\"\n", string);
		return -EINVAL;
	}

	if (string == p) {
		DEBUG_PRINTF("No integer stored at \"%s\"\n", string);
		return -EINVAL;
	}

	errno =  errno_saved;
	return 0;
}

int parse_generic_create_options(char *options_str,
				 uint64_t *options,
				 struct option_entry options_map[],
				 unsigned int num_options)
{
	char *cursor = NULL;
	char *opt_str = strtok_r(options_str, ",", &cursor);
	uint64_t options_mask = 0;
	uint64_t debug_option;
	bool option_found;
	unsigned int i;
	int error;

	while (opt_str != NULL) {

		option_found = false;
		for (i = 0; i < num_options; ++i) {
			if (strcmp(opt_str, options_map[i].str) == 0) {
				options_mask |= options_map[i].value;
				option_found = true;
				break;
			}
		}

		if (!option_found) {
			/* debug options are given as hexa value */
			error = uint64t_from_string(&debug_option, opt_str);
			if (!error) {
				DEBUG_PRINTF("Found debug option: 0x%x\n",
					     (unsigned int)debug_option);
				options_mask |= debug_option;
				option_found = true;
			}
		}

		if (!option_found) {
			ERROR_PRINTF("Invalid option: '%s'\n", opt_str);
			return -EINVAL;
		}

		opt_str = strtok_r(NULL, ",", &cursor);
	}

	*options = options_mask;

	return 0;
}


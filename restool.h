/*
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 * Author: German Rivera <German.Rivera@freescale.com>
 *	   Lijun Pan <Lijun.Pan@freescale.com>
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

#ifndef _RESTOOL_H_
#define _RESTOOL_H_

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <limits.h>
#include "fsl_mc_sys.h"
#include "fsl_dpmng.h"
#include "fsl_dprc.h"
#include "fsl_mc_ioctl.h"
#include "fsl_mc_cmd.h"

/**
 * MC object type string max length (without including the null terminator)
 */
#define OBJ_TYPE_MAX_LENGTH	8

/**
 * MC resource type string max length (without including the null terminator)
 */
#define RES_TYPE_MAX_LENGTH	15

/**
 * Maximum number of command line options
 */
#define MAX_NUM_CMD_LINE_OPTIONS	(sizeof(uint32_t) * 8)

/**
 * Maximum level of nesting of DPRCs
 */
#define MAX_DPRC_NESTING	16

/**
 * Maximum length of object label (without including the null terminator)
 */
#define MC_OBJ_LABEL_MAX_LENGTH 15

/**
 * MC endpoint object type max length (without including the null terminator)
 */
#define EP_OBJ_TYPE_MAX_LEN 15

/**
 * MC command high priority flag, original definition is too long
 * to be an appropriate pass-in parameter for each flib API
 */
#define PRI MC_CMD_FLAG_PRI

/**
 * MC command command completion flag, original definition is too long
 * to be an appropriate pass-in parameter for each flib API
 */
#define INTR MC_CMD_FLAG_INTR_DIS

/**
 * MC command high priority and command completion flag,
 * original definition is too long
 * to be an appropriate pass-in parameter for each flib API
 */
#define PRINTR (MC_CMD_FLAG_PRI | INTR MC_CMD_FLAG_INTR_DIS)

/*
 * TODO: Obtain the following constants from the fsl-mc bus driver via an ioctl
 */
#define MC_PORTALS_BASE_PADDR	0x00080C000000ULL
#define MC_PORTAL_STRIDE	0x10000
#define MC_PORTAL_SIZE		64
#define MAX_MC_PORTALS		512

#define MC_PORTAL_OFFSET_TO_PORTAL_ID(_portal_offset) \
	((_portal_offset) / MC_PORTAL_STRIDE)

struct restool;

typedef int restool_cmd_func_t(void);

struct object_command {
	/**
	 * object-specific command found in the command line
	 */
	const char *cmd_name;

	/**
	 * Pointer to array of getopt_long options
	 */
	const struct option *options;

	/**
	 * Pointer to command function
	 */
	restool_cmd_func_t *cmd_func;
};

/**
 * Object command parser structure
 */
struct object_cmd_parser {
	/**
	 * object-type found in the command line
	 */
	const char *obj_type;

	/**
	 * Pointer to array of commands for the object type
	 */
	struct object_command *obj_commands;
};

/**
 * Global state of the restool tool
 */
struct restool {
	/**
	 * Bit mask of command-line options not consumed yet
	 */
	uint32_t global_option_mask;

	/**
	 * Array of option arguments for global options found in the command line,
	 * that have arguments. One entry per option.
	 */
	const char *global_option_args[MAX_NUM_CMD_LINE_OPTIONS];

	/**
	 * Pointer to the command being processed
	 */
	struct object_command *obj_cmd;

	/**
	 * object name found in the command line
	 */
	const char *obj_name;

	/**
	 * Bit mask of command-line options not consumed yet
	 */
	uint32_t cmd_option_mask;

	/**
	 * Array of option arguments for options found in the command line,
	 * that have arguments. One entry per option.
	 */
	char *cmd_option_args[MAX_NUM_CMD_LINE_OPTIONS];

	/**
	 * MC I/O portal
	 */
	struct fsl_mc_io mc_io;

	/**
	 * MC firmware version
	 */
	struct mc_version mc_fw_version;

	/**
	 * Id for the root DPRC in the system
	 */
	uint32_t root_dprc_id;

	/**
	 * Handle for the root DPRC in the system
	 */
	uint16_t root_dprc_handle;

	/**
	 * global flag to enable debug printing
	 */
	bool debug;

	/**
	 * global flag to enable script printing
	 * it only print out created object name
	 * instead of the whole sentence
	 */
	bool script;
};

/**
 * Command-line option indices for global restool options
 */
enum global_options {
	GLOBAL_OPT_HELP = 0,
	GLOBAL_OPT_VERSION,
	GLOBAL_OPT_MC_VERSION,
	GLOBAL_OPT_DEBUG,
	GLOBAL_OPT_SCRIPT
};

/**
 * define generic flib operations
 */
typedef int flib_obj_open_t(struct fsl_mc_io	*mc_io,
				uint32_t	cmd_flags,
				int		obj_id,
				uint16_t	*token);
typedef int flib_obj_close_t(struct fsl_mc_io	*mc_io,
				uint32_t	cmd_flags,
				uint16_t	token);
typedef int flib_obj_get_irq_mask_t(struct fsl_mc_io	*mc_io,
				uint32_t	cmd_flags,
				uint16_t	token,
				uint8_t		irq_index,
				uint32_t	*mask);
typedef int flib_obj_get_irq_status_t(struct fsl_mc_io	*mc_io,
					uint32_t	cmd_flags,
					uint16_t	token,
					uint8_t		irq_index,
					uint32_t	*status);

struct flib_ops {
	flib_obj_open_t *obj_open;
	flib_obj_close_t *obj_close;
	flib_obj_get_irq_mask_t *obj_get_irq_mask;
	flib_obj_get_irq_status_t *obj_get_irq_status;
};

int parse_object_name(const char *obj_name, char *expected_obj_type,
		      uint32_t *obj_id);

int open_dprc(uint32_t dprc_id, uint16_t *dprc_handle);

void print_unexpected_options_error(uint32_t option_mask,
				    const struct option *options);

enum mc_cmd_status flib_error_to_mc_status(int error);
const char *mc_status_to_string(enum mc_cmd_status status);
int find_target_obj_desc(uint32_t dprc_id, uint16_t dprc_handle,
			int nesting_level,
			uint32_t target_id, char *target_type,
			struct dprc_obj_desc *target_obj_desc,
			uint32_t *target_parent_dprc_id, bool *found);
bool find_obj(char *obj_type, uint32_t obj_id);
void print_obj_label(struct dprc_obj_desc *target_obj_desc);
int print_obj_verbose(struct dprc_obj_desc *target_obj_desc,
			const struct flib_ops *ops);
int check_resource_type(char *res_type);
bool in_use(const char *obj, const char *situation);
void print_new_obj(char *type, int id, const char *parent);

extern struct restool restool;
extern struct object_command dprc_commands[];
extern struct object_command dpni_commands[];
extern struct object_command dpio_commands[];
extern struct object_command dpbp_commands[];
extern struct object_command dpsw_commands[];
extern struct object_command dpci_commands[];
extern struct object_command dpcon_commands[];
extern struct object_command dpseci_commands[];
extern struct object_command dpdmux_commands[];
extern struct object_command dpmcp_commands[];
extern struct object_command dpmac_commands[];
extern struct object_command dpdcei_commands[];
extern struct object_command dpaiop_commands[];

#endif /* _RESTOOL_H_ */

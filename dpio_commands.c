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
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include "resman.h"
#include "utils.h"
#include "fsl_dpio.h"

/**
 * dpio show command options
 */
enum dpio_info_options {
	OPT_VERBOSE = 0,
};

static struct option dpio_info_options[] = {
	[OPT_VERBOSE] = {
		.name = "verbose",
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpio_info_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static int cmd_dpio_help(void)
{
	ERROR_PRINTF("not implemented yet\n");
	return -ENOTSUP;
}

static int print_dpio_attr(uint32_t dpio_id)
{
	uint16_t dpio_handle;
	int error;
	struct dpio_attr dpio_attr;
	bool dpio_opened = false;

	error = dpio_open(&resman.mc_io, dpio_id, &dpio_handle);
	if (error < 0) {
		ERROR_PRINTF("dpio_open() failed for dpio.%u with error %d\n", dpio_id, error);
		goto out;
	}
	dpio_opened = true;
	if(0 == dpio_handle) {
		ERROR_PRINTF("dpio_open() returned invalid handle (auth 0) for dpio.%u\n", dpio_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpio_attr, 0, sizeof(dpio_attr));
	error = dpio_get_attributes(&resman.mc_io, dpio_handle, &dpio_attr);
	if (error < 0) {
		ERROR_PRINTF("dpio_get_attributes() failed with error: %d\n", error);
		goto out;
	}
	assert(dpio_id == (uint32_t)dpio_attr.id);

	printf("dpio version: %u.%u\n", dpio_attr.version.major, dpio_attr.version.minor);
	printf("dpio id: %d\n", dpio_attr.id);
	printf("physical address of qbman software portal cache-enabled area: %#llx\n", (long long unsigned int)dpio_attr.qbman_portal_ce_paddr);
	printf("physical address of qbman software portal cache-inhibited area: %#llx\n", (long long unsigned int)dpio_attr.qbman_portal_ci_paddr);
	printf("qbman software portal id: %#x\n", (unsigned int)dpio_attr.qbman_portal_id);
	printf("dpio channel mode is: ");
	dpio_attr.channel_mode == 0 ? printf("DPIO_NO_CHANNEL\n") :
	dpio_attr.channel_mode == 1 ? printf("DPIO_LOCAL_CHANNEL\n") :
	printf("wrong mode\n");
	printf("number of priorities is: %#x\n", (unsigned int)dpio_attr.num_priorities);

	error = 0;

out:
	if (dpio_opened) {
		int error2;

		error2 = dpio_close(&resman.mc_io, dpio_handle);
		if(error2 < 0) {
			ERROR_PRINTF("dpio_close() failed with error %d\n", error2);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int print_dpio_verbose(void)
{
	printf("not implemented yet\n");
	return -ENOTSUP;
}

static int print_dpio_info(uint32_t dpio_id)
{
	int error;

	error = print_dpio_attr(dpio_id);
	if (error < 0)
		goto out;

	if (resman.cmd_option_mask & ONE_BIT_MASK(OPT_VERBOSE)) {
		resman.cmd_option_mask &= ~ONE_BIT_MASK(OPT_VERBOSE);
		error = print_dpio_verbose();
		goto out;
	}

out:
	return error;
}

static int cmd_dpio_info(void)
{
	uint32_t obj_id;
	int error;

	if (resman.obj_name == NULL) {
		ERROR_PRINTF("<object> argument missing\n");
		error = -EINVAL;
		goto out;
	}

	error = parse_object_name(resman.obj_name, "dpio", &obj_id);
	if (error < 0)
		goto out;

	error = print_dpio_info(obj_id);
out:
	return error;
}

static int cmd_dpio_create(void)
{
	ERROR_PRINTF("not implemented yet\n");
	return -ENOTSUP;
}

static int cmd_dpio_destroy(void)
{
	ERROR_PRINTF("not implemented yet\n");
	return -ENOTSUP;
}

struct object_command dpio_commands[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpio_help },

	{ .cmd_name = "info",
	  .options = dpio_info_options,
	  .cmd_func = cmd_dpio_info },

	{ .cmd_name = "create",
	  .options = NULL,
	  .cmd_func = cmd_dpio_create },

	{ .cmd_name = "destroy",
	  .options = NULL,
	  .cmd_func = cmd_dpio_destroy },

	{ .cmd_name = NULL },
};


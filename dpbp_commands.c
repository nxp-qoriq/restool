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
#include "fsl_dpbp.h"

/**
 * dpbp show command options
 */
enum dpbp_info_options {
	OPT_VERBOSE = 0,
};

static struct option dpbp_info_options[] = {
	[OPT_VERBOSE] = {
		.name = "verbose",
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpbp_info_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);


static int cmd_dpbp_help(void)
{
	ERROR_PRINTF("not implemented yet\n");
	return -ENOTSUP;
}

static int print_dpbp_attr(uint32_t dpbp_id)
{
	uint16_t dpbp_handle;
	int error;
	struct dpbp_attr dpbp_attr;
	bool dpbp_opened = false;

	error = dpbp_open(&resman.mc_io, dpbp_id, &dpbp_handle);
	if (error < 0) {
		ERROR_PRINTF("dpbp_open() failed for dpbp.%u with error %d\n", dpbp_id, error);
		goto out;
	}
	dpbp_opened = true;
	if(0 == dpbp_handle) {
		ERROR_PRINTF("dpbp_open() returned invalid handle (auth 0) for dpbp.%u\n", dpbp_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpbp_attr, 0, sizeof(dpbp_attr));
	error = dpbp_get_attributes(&resman.mc_io, dpbp_handle, &dpbp_attr);
	if (error < 0) {
		ERROR_PRINTF("dpbp_get_attributes() failed with error: %d\n", error);
		goto out;
	}
	assert(dpbp_id == (uint32_t)dpbp_attr.id);

	printf("dpbp version: %u.%u\n", dpbp_attr.version.major, dpbp_attr.version.minor);
	printf("dpbp id: %d\n", dpbp_attr.id);
	printf("buffer pool id: %u\n", (unsigned int)dpbp_attr.bpid);

	error = 0;

out:
	if (dpbp_opened) {
		int error2;

		error2 = dpbp_close(&resman.mc_io, dpbp_handle);
		if(error2 < 0) {
			ERROR_PRINTF("dpbp_close() failed with error %d\n", error2);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int print_dpbp_verbose(void)
{
	printf("not implemented yet\n");
	return -ENOTSUP;
}

static int print_dpbp_info(uint32_t dpbp_id)
{
	int error;

	error = print_dpbp_attr(dpbp_id);
	if (error < 0)
		goto out;

	if (resman.cmd_option_mask & ONE_BIT_MASK(OPT_VERBOSE)) {
		resman.cmd_option_mask &= ~ONE_BIT_MASK(OPT_VERBOSE);
		error = print_dpbp_verbose();
		goto out;
	}

out:
	return error;
}

static int cmd_dpbp_info(void)
{
	uint32_t obj_id;
	int error;

	if (resman.obj_name == NULL) {
		ERROR_PRINTF("<object> argument missing\n");
		error = -EINVAL;
		goto out;
	}

	error = parse_object_name(resman.obj_name, "dpbp", &obj_id);
	if (error < 0)
		goto out;

	error = print_dpbp_info(obj_id);
out:
	return error;
}

static int cmd_dpbp_create(void)
{
	ERROR_PRINTF("not implemented yet\n");
	return -ENOTSUP;
}

static int cmd_dpbp_destroy(void)
{
	ERROR_PRINTF("not implemented yet\n");
	return -ENOTSUP;
}

struct object_command dpbp_commands[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpbp_help },

	{ .cmd_name = "info",
	  .options = dpbp_info_options,
	  .cmd_func = cmd_dpbp_info },

	{ .cmd_name = "create",
	  .cmd_func = cmd_dpbp_create },

	{ .cmd_name = "destroy",
	  .cmd_func = cmd_dpbp_destroy },

	{ .cmd_name = NULL },
};


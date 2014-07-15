/*
 * Freescale Layerscape MC I/O wrapper
 *
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 * Author: German Rivera <German.Rivera@freescale.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#include <unistd.h> /* usleep() */
#include <time.h>   /* clock() */
#include <errno.h>
#include "fsl_mc_sys.h"
#include "fsl_mc_io_wrapper.h"
#include "fsl_mc_cmd.h"
#include "utils.h"

/**
 * Timeout in clock ticks to wait for the completion of an MC command
 */
#define MC_CMD_COMPLETION_TIMEOUT   (CLOCKS_PER_SEC / 10)

/**
 * Delay in microseconds between polling iterations while
 * waiting for MC command completion
 */
#define MC_CMD_COMPLETION_POLLING_INTERVAL  500

static int mc_status_to_error(enum mc_cmd_status status)
{
	switch (status) {
	case MC_CMD_STATUS_OK:
		return 0;
	case MC_CMD_STATUS_AUTH_ERR:
		return -EACCES;
	case MC_CMD_STATUS_NO_PRIVILEGE:
		return -EPERM;
	case MC_CMD_STATUS_DMA_ERR:
		return -EIO;
	case MC_CMD_STATUS_CONFIG_ERR:
		return -EINVAL;
	case MC_CMD_STATUS_TIMEOUT:
		return -ETIMEDOUT;
	case MC_CMD_STATUS_NO_RESOURCE:
		return -ENAVAIL;
	case MC_CMD_STATUS_NO_MEMORY:
		return -ENOMEM;
	case MC_CMD_STATUS_BUSY:
		return -EBUSY;
	case MC_CMD_STATUS_UNSUPPORTED_OP:
		return -ENOTSUP;
	case MC_CMD_STATUS_INVALID_STATE:
		return -ENODEV;
	default:
		break;
	}

	/* Not expected to reach here */
	return -EINVAL;
}

int mc_send_command(void *mc_io, struct mc_command *cmd)
{
	enum mc_cmd_status status;
	int error;
	struct mc_portal_wrapper *mc_portal = mc_io;
	clock_t start_clock = clock();

	/*
	 * TODO: Call lock function here in case portal is shared
	 */

	mc_write_command(mc_portal->mmio_regs, cmd);

	for (;;) {
		status = mc_read_response(mc_portal->mmio_regs, cmd);
		if (status != MC_CMD_STATUS_READY)
			break;

		/*
		 * TODO: When MC command completion interrupts are supported
		 * call wait function here an ioctl() instead of usleep()
		 */
		usleep(MC_CMD_COMPLETION_POLLING_INTERVAL);

		clock_t end_clock = clock();
		if (CLOCK_DELTA(start_clock, end_clock) >=
			MC_CMD_COMPLETION_TIMEOUT) {
			ERROR_PRINTF(
				"MC timeout: start_clock %llu, end_clock %llu, timeout %llu\n",
				(unsigned long long)start_clock,
				(unsigned long long)end_clock,
				(unsigned long long)MC_CMD_COMPLETION_TIMEOUT);

			error = -ETIMEDOUT;
			goto out;
		}
	}

	error = mc_status_to_error(status);
out:
	/*
	 * TODO: Call unlock function here in case portal is shared
	 */

	return error;
}

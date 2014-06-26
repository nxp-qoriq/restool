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
#include "fsl_mc_io_wrapper.h"
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>		/* open() */
#include <unistd.h>		/* close(), usleep() */
#include <sys/mman.h>		/* mmap() */
#include <sys/param.h>		/* EXEC_PAGESIZE */
#include "fsl_mc_common_wrapper.h"
#include <fsl_mc_io.h>

#define RESMAN_DEVICE_FILE  "/dev/mc_resman"

int mc_portal_wrapper_init(
	struct mc_portal_wrapper *mc_portal)
{
	int fd = -1;
	volatile struct mc_command *mmio_regs = NULL;
	int error;

	fd = open(RESMAN_DEVICE_FILE, O_RDWR | O_SYNC);
	if (fd < 0) {
		error = -errno;
		perror("open() failed for " RESMAN_DEVICE_FILE);
		goto error;
	}

	mmio_regs = mmap(NULL, EXEC_PAGESIZE,
			 PROT_READ | PROT_WRITE, MAP_SHARED,
			 fd, 0);

	if (mmio_regs == MAP_FAILED) {
		error = -errno;
		perror("mmap() failed for MC portal");
		goto error;
	}

	mc_portal->fd = fd;
	mc_portal->mmio_regs = mmio_regs;
	return 0;
error:
	if (mmio_regs != NULL)
		munmap((void *)mmio_regs, EXEC_PAGESIZE);

	if (fd != -1)
		close(fd);

	return error;
}

void mc_portal_wrapper_cleanup(
	struct mc_portal_wrapper *mc_portal)
{
	int error;

	assert(mc_portal->fd != -1);
	assert(mc_portal->mmio_regs != NULL);

	error = munmap((void *)mc_portal->mmio_regs, EXEC_PAGESIZE);
	if (error == -1)
		perror("munmap failed");

	error = close(mc_portal->fd);
	if (error == -1)
		perror("close failed");
}

/**
 * mc_portal_wrapper_send_command - Send MC command and wait for response
 *
 * @mc_portal: Pointer to MC portal wrapper to be used
 * @cmd: MC command buffer. On input, it contains the command to send to the MC.
 * On output, it contains the response from the MC if any.
 *
 * Depending on the sharing option specified when creating the MC portal
 * wrapper, this function will use a spinlock or mutex to ensure exclusive
 * access to the MC portal from the point when the command is sent until a
 * response is received from the MC. Also, depending on the sharing  option
 * and if MC_CMD_COMPLETION_ISR_USED is defined, the wait for the response
 * from the MC will be done using a completion variable, instead of doing
 * polling, if possible.
 */
int mc_portal_wrapper_send_command(
	struct mc_portal_wrapper *mc_portal,
	struct mc_command *cmd)
{
	enum mc_cmd_status status;
	int error;
	int timeout = 2000;

	error = mc_write_command(mc_portal->mmio_regs, cmd);
	if (error < 0)
		goto out;

	for (;;) {
		status = mc_read_response(mc_portal->mmio_regs, cmd);
		switch (status) {
		case MC_CMD_STATUS_OK:
			error = 0;
			break;
		case MC_CMD_STATUS_READY:
			/* response not available yet */
			if (timeout-- <= 0) {
				error = -ETIMEDOUT;
				goto out;
			}

			usleep(1000);	/* throttle polling */
			continue;
		case MC_CMD_STATUS_AUTH_ERR:
			error = -EACCES; /* Authentication error */
			break;
		case MC_CMD_STATUS_NO_PRIVILEGE:
			error = -EPERM; /* Permission denied */
			break;
		case MC_CMD_STATUS_DMA_ERR:
			error = -EIO; /* Input/Output error */
			break;
		case MC_CMD_STATUS_CONFIG_ERR:
			error = -EINVAL; /* Device not configured */
			break;
		case MC_CMD_STATUS_TIMEOUT:
			error = -ETIMEDOUT; /* Operation timed out */
			break;
		case MC_CMD_STATUS_NO_RESOURCE:
			error = -ENAVAIL; /* Resource temporarily unavailable */
			break;
		case MC_CMD_STATUS_NO_MEMORY:
			error = -ENOMEM; /* Cannot allocate memory */
			break;
		case MC_CMD_STATUS_BUSY:
			error = -EBUSY; /* Device busy */
			break;
		case MC_CMD_STATUS_UNSUPPORTED_OP:
			error = -EINVAL; /* Operation not supported by device */
			break;
		case MC_CMD_STATUS_INVALID_STATE:
			error = -ENODEV; /* Invalid device state */
			break;
		default:
			error = -EINVAL;
		}

		break;
	}

	error = 0;
out:
	return error;
}


/*
 * Freescale resman MC I/O layer
 *
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 * Author: German Rivera <German.Rivera@freescale.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#include <errno.h>
#include <assert.h>
#include <fcntl.h>		/* open() */
#include <unistd.h>		/* close() */
#include <sys/ioctl.h>
#include "fsl_mc_io.h"
#include "fsl_mc_cmd.h"
#include "fsl_mc_ioctl.h"
#include "utils.h"

#define RESMAN_DEVICE_FILE  "/dev/mc_resman"

int mc_io_init(struct mc_io *mc_io)
{
	int fd = -1;
	int error;

	fd = open(RESMAN_DEVICE_FILE, O_RDWR | O_SYNC);
	if (fd < 0) {
		error = -errno;
		perror("open() failed for " RESMAN_DEVICE_FILE);
		goto error;
	}

	mc_io->fd = fd;
	return 0;
error:
	if (fd != -1)
		(void)close(fd);

	return error;
}

void mc_io_cleanup(
	struct mc_io *mc_io)
{
	int error;

	assert(mc_io->fd != -1);

	error = close(mc_io->fd);
	if (error == -1)
		perror("close failed");
}

int mc_send_command(void *mc_io, struct mc_command *cmd)
{
	int error;
	struct mc_io *resman_mc_io = mc_io;

	error = ioctl(resman_mc_io->fd, RESMAN_SEND_MC_COMMAND, cmd);
	if (error == -1) {
		error = -errno;
		ERROR_PRINTF(
			"ioctl(RESMAN_SEND_MC_COMMAND) failed with error %d\n",
			error);
	}

	return error;
}

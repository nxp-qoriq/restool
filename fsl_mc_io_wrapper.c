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
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>		/* open() */
#include <unistd.h>		/* close() */
#include <sys/mman.h>		/* mmap() */
#include <sys/param.h>		/* EXEC_PAGESIZE */
#include "fsl_mc_io_wrapper.h"
#include "fsl_mc_sys.h"

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


/*
 * Freescale resman MC I/O layer
 *
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef _FSL_MC_IO_H
#define _FSL_MC_IO_H

#include <stdint.h>

struct mc_command;

/**
 * struct mc_io - MC I/O object
 */
struct mc_io {
	int fd;
};

int mc_io_init(struct mc_io *mc_io);

void mc_io_cleanup(struct mc_io *mc_io);

int mc_send_command(void *mc_io, struct mc_command *cmd);

#endif /* _FSL_MC_IO_H */

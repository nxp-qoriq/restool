/*
 * Freescale Layerscape Management Complex (MC) I/O wrapper
 *
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#ifndef _FSL_MC_IO_WRAPPER_H
#define _FSL_MC_IO_WRAPPER_H

/**
 * struct mc_portal_wrapper - MC command portal wrapper object
 */
struct mc_portal_wrapper {
	int fd;
	volatile struct mc_command *mmio_regs;
};

int mc_portal_wrapper_init(
	struct mc_portal_wrapper *mc_portal);

void mc_portal_wrapper_cleanup(
	struct mc_portal_wrapper *mc_portal);

#endif /* _FSL_MC_IO_WRAPPER_H */


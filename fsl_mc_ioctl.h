/*
 * Freescale Management Complex (MC) ioclt interface
 *
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 * Author: German Rivera <German.Rivera@freescale.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#ifndef _FSL_MC_IOCTL_H_
#define _FSL_MC_IOCTL_H_

#include <linux/ioctl.h>

#define RESMAN_IOCTL_TYPE   'R'

#define RESMAN_GET_ROOT_DPRC_INFO \
	_IOR(RESMAN_IOCTL_TYPE, 0x1, struct ioctl_dprc_info)

#define RESMAN_SEND_MC_COMMAND \
	_IOWR(RESMAN_IOCTL_TYPE, 0x2, struct mc_portal)

#define RESMAN_RESCAN_ROOT_DPRC \
	_IO(RESMAN_IOCTL_TYPE, 0x3)

struct ioctl_dprc_info {
	uint32_t dprc_id;
	uint16_t dprc_handle;
};

#endif /* _FSL_MC_IOCTL_H_ */

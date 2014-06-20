/*
 * Freescale Layerscape Management Complex (MC) common wrapper
 *
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#ifndef _FSL_MC_COMMON_WRAPPER_H
#define _FSL_MC_COMMON_WRAPPER_H

#include <stdint.h>

#define lower_32_bits(_x)   ((uint32_t)(_x))
#define upper_32_bits(_x)   ((uint32_t)(((uint64_t)(_x)) >> 32))

static inline uint64_t readq(volatile uint64_t *io_reg)
{
	return *io_reg;
}

static inline void writeq(uint64_t value, volatile uint64_t *io_reg)
{
	*io_reg = value;
}

#endif /* _FSL_MC_COMMON_WRAPPER_H */


/*
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 * Author: German Rivera <German.Rivera@freescale.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define C_ASSERT(_cond) \
        extern const char c_assert_dummy_decl[(_cond) ? 1 : -1]

#define ARRAY_SIZE(_array) \
        (sizeof(_array) / sizeof((_array)[0]))

#define ONE_BIT_MASK(_bit_index)     (UINT32_C(0x1) << (_bit_index))

#ifdef ERROR_PRINT
#define ERROR_PRINTF(_fmt, ...)	\
	fprintf(stderr, "%s: " _fmt, __func__, ##__VA_ARGS__)
#else
#define ERROR_PRINTF(_fmt, ...) \
	printf(_fmt, ##__VA_ARGS__)
#endif

#ifdef DEBUG
#define DEBUG_PRINTF(_fmt, ...)	\
	fprintf(stderr, "DBG: %s: " _fmt, __func__, ##__VA_ARGS__)
#else
#define DEBUG_PRINTF(_fmt, ...)
#endif

#define STRINGIFY(_x)	__STRINGIFY_EXPANDED(_x)

#define __STRINGIFY_EXPANDED(_expanded_x)   #_expanded_x

#define CLOCK_DELTA(_start_clock, _end_clock) \
        ((clock_t)((int64_t)(_end_clock) - (int64_t)(_start_clock)))

#endif /* _UTILS_H */

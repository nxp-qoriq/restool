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

#define C_ASSERT(_cond) \
        extern const char c_assert_dummy_decl[(_cond) ? 1 : -1]

#define ARRAY_SIZE(_array) \
        (sizeof(_array) / sizeof((_array)[0]))

#define ONE_BIT_MASK(_bit_index)     (UINT32_C(0x1) << (_bit_index))

#define ERROR_PRINTF(_fmt, ...)	\
	fprintf(stderr, "%s: " _fmt, __func__, ##__VA_ARGS__)

#define DEBUG_PRINTF(_fmt, ...)	\
	fprintf(stderr, "DBG: %s: " _fmt, __func__, ##__VA_ARGS__)

#endif /* _UTILS_H */

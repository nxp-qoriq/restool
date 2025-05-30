/* Copyright 2014-2016 Freescale Semiconductor Inc.
 * Copyright 2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * * Neither the name of the above-listed copyright holders nor the
 * names of any contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "../restool.h"

#define C_ASSERT(_cond) \
	extern const char c_assert_dummy_decl[(_cond) ? 1 : -1]

#define ARRAY_SIZE(_array) \
	(sizeof(_array) / sizeof((_array)[0]))

#define ONE_BIT_MASK(_bit_index)     (UINT32_C(0x1) << (_bit_index))

#define ERROR_PRINTF(_fmt, ...) \
do { \
	if (restool.debug) \
		fprintf(stderr, "%s:%d " _fmt, \
			__func__, __LINE__, ##__VA_ARGS__); \
	else \
		fprintf(stderr, _fmt, ##__VA_ARGS__); \
} while (0)

#define DEBUG_PRINTF(_fmt, ...)	\
do { \
	if (restool.debug) \
		fprintf(stderr, "DBG: %s:%d: " _fmt, \
			__func__, __LINE__, ##__VA_ARGS__); \
} while (0)

#define STRINGIFY(_x)	__STRINGIFY_EXPANDED(_x)

#define __STRINGIFY_EXPANDED(_expanded_x)   #_expanded_x

#define CLOCK_DELTA(_start_clock, _end_clock) \
	((clock_t)((int64_t)(_end_clock) - (int64_t)(_start_clock)))

#define STRTOL_ERROR(_str, _endptr, _val, _errno) \
	(((_errno) == ERANGE && ((_val) == LONG_MAX || (_val) == LONG_MIN)) \
	|| ((_errno) != 0 && (_val) == 0) \
	|| ((_endptr) == (_str)) \
	|| (*(_endptr) != '\0'))

void diff_time(struct timespec *, struct timespec *, struct timespec *);

struct mc_api_compatibility_info {
	uint16_t major;
	uint16_t minor;
	uint16_t cmd_id;
};

static inline int cmp_ver(uint16_t ver_major_1, uint16_t ver_minor_1,
			  uint16_t ver_major_2, uint16_t ver_minor_2)
{
	if (ver_major_1 == ver_major_2)
		return ver_minor_1 - ver_minor_2;
	return ver_major_1 - ver_major_2;
}

static inline uint16_t get_cmd_id(struct mc_api_compatibility_info compat_info[], int num,
				  uint16_t obj_major, uint16_t obj_minor)
{
	int i;

	for (i = 0; i < num; i++) {
		/* If the current API version of the object is higher than a
		 * compatibility info entry, then we found the compatible
		 * command ID. This will only work if the array is sorted with
		 * the latest API versions first.
		 */
		if (cmp_ver(obj_major, obj_minor,
			    compat_info[i].major, compat_info[i].minor) >= 0) {
			return compat_info[i].cmd_id;
		}
	}

	/* The last entry in the array will be the default one */
	return compat_info[num - 1].cmd_id;
}

#endif /* _UTILS_H */

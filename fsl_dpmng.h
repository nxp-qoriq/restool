/* Copyright 2013-2014 Freescale Semiconductor Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 *  @file    fsl_dpmng.h
 *  @brief   Management Complex General API
 */

#ifndef __FSL_DPMNG_H
#define __FSL_DPMNG_H

/*!
 * @Group grp_dpmng	Management Complex General API
 *
 * @brief	Contains general API for the Management Complex firmware
 * @{
 */

struct fsl_mc_io;

/**
 * @brief	Management Complex firmware version information
 */
#define MC_VER_MAJOR 2
#define MC_VER_MINOR 0

struct mc_version {
	uint32_t major;
	/*!<
	 * Major version number: incremented on API compatibility changes
	 */
	uint32_t minor;
	/*!<
	 * Minor version number: incremented on API additions (backward
	 * compatible); reset when major version is incremented.
	 */
	uint32_t revision;
	/*!<
	 * Internal revision number: incremented on implementation changes
	 * and/or bug fixes that have no impact on API
	 */
};

/**
 * @brief	Retrieves the Management Complex firmware version information
 *
 * @param[in]   mc_io - Pointer to opaque I/O object
 * @param[out]	mc_ver_info - Pointer to version information structure
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int mc_get_version(struct fsl_mc_io *mc_io, struct mc_version *mc_ver_info);

/**
 * @brief	Resets an AIOP tile
 *
 * @param[in]   mc_io - Pointer to opaque I/O object
 * @param[in]	aiop_tile_id - AIOP tile ID to reset
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpmng_reset_aiop(struct fsl_mc_io *mc_io, int aiop_tile_id);

/**
 * @brief	Loads an image to AIOP tile
 *
 * @param[in]   mc_io - Pointer to opaque I/O object
 * @param[in]	aiop_tile_id - AIOP tile ID
 * @param[in]	img_addr - Pointer to AIOP ELF image in memory
 * @param[in]	img_size - Size in bytes of AIOP ELF image in memory
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpmng_load_aiop(struct fsl_mc_io *mc_io,
		    int aiop_tile_id,
		    uint8_t *img_addr,
		    int img_size);

/**
 * @brief	Starts AIOP tile execution
 *
 * @param[in]   mc_io - Pointer to opaque I/O object
 * @param[in]	aiop_tile_id - AIOP tile ID to run
 * @param[in]	cores_mask - Mask of AIOP cores to run (core 0 in msb)
 * @param[in]	options - Execution options (currently none defined)
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpmng_run_aiop(struct fsl_mc_io *mc_io,
		   int aiop_tile_id,
		   uint32_t cores_mask,
		   uint64_t options);

/** @} */

#endif /* __FSL_DPMNG_H */

/* Copyright 2013 Freescale Semiconductor, Inc. */
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

#ifdef MC
/*!< Forward declaration */
struct dpmng;
#else
struct dpmng {
	void *regs;
	/*!<
	 * Pointer to command interface registers (virtual address);
	 * Must be set by the user
	 */
	int auth; /*!< authentication ID */
};
#endif

/**
 * @brief	Management Complex firmware version information
 */
#define MC_VER_MAJOR 1
#define MC_VER_MINOR 0
#define MC_VER_REVISION 0

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
 * @param[in]	dpmng - Pointer to dpmng object
 * @param[out]	mc_ver_info - Pointer to version information structure
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int mc_get_version(void *mc_io, struct mc_version *mc_ver_info);

/**
 * @brief	Resets an AIOP tile
 *
 * @param[in]	dpmng - Pointer to dpmng object
 * @param[in]	aiop_tile_id - AIOP tile ID to reset
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpmng_reset_aiop(void *mc_io, int aiop_tile_id);

/**
 * @brief	Loads an image to AIOP tile
 *
 * @param[in]	dpmng - Pointer to dpmng object
 * @param[in]	aiop_tile_id - AIOP tile ID
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpmng_load_aiop(void *mc_io,
		    int aiop_tile_id,
	uint8_t *img_addr,
	int img_size);

/**
 * @brief	Starts AIOP tile execution
 *
 * @param[in]	dpmng - Pointer to dpmng object
 * @param[in]	aiop_tile_id - AIOP tile ID to run
 * @param[in]	cores_mask - Mask of AIOP cores to run (core 0 in msb)
 * @param[in]	options - Execution options (currently none defined)
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpmng_run_aiop(void *mc_io,
		   int aiop_tile_id,
		   uint32_t cores_mask,
		   uint64_t options);

/** @} */

#endif /* __FSL_DPMNG_H */

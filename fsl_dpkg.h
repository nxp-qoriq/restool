/* Copyright 2014 Freescale Semiconductor Inc.
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
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
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
#ifndef __FSL_DPKG_H_
#define __FSL_DPKG_H_

#include "fsl_net.h"

/*!
 * @Group grp_dpkg	Data Path Key Generator API
 *
 * @brief	Contains initialization APIs and runtime APIs for the
 *		Key Generator
 * @{
 */

/*!
 * @name Key Generator properties
 */
#define DPKG_NUM_OF_MASKS		4
/*!< Number of masks per key extraction */
#define DPKG_MAX_NUM_OF_EXTRACTS	8
/*!< Number of extractions per key profile */

/* @} */

/**
 * @brief	Enumeration for selecting extraction by header types
 */
enum dpkg_extract_from_hdr_type {
	DPKG_FROM_HDR,
	/*!< Extract selected bytes from header, by offset */
	DPKG_FROM_FIELD,
	/*!< Extract selected bytes from header, by offset from field */
	DPKG_FULL_FIELD
	/*!< Extract a full field */
};

/**
 * @brief	Enumeration for selecting extraction type
 */
enum dpkg_extract_type {
	DPKG_EXTRACT_FROM_HDR,
	/*!< Extract from the header */
	DPKG_EXTRACT_FROM_DATA,
	/*!< Extract from data not in the header */
	DPKG_EXTRACT_CONSTANT
	/*!< Extract user-selected constant values */
};

/**
 * @brief	A structure for defining a single extraction mask
 */
struct dpkg_mask {
	uint8_t mask;
	/*!< Byte mask for the extracted content */
	uint8_t offset;
	/*!< Offset within the extracted content */
};

/**
 * @brief	A structure for defining a single extraction
 */
struct dpkg_extract {
	enum dpkg_extract_type type;
	/*!< Determines how the union below is interpreted:
	 * DPKG_EXTRACT_FROM_HDR: selects 'from_hdr';
	 * DPKG_EXTRACT_FROM_DATA: selects 'from_data';
	 * DPKG_EXTRACT_CONSTANT: selects 'constant'
	 */
	union {
		struct {
			enum net_prot prot;
			/*!< Any of the supported headers */

			enum dpkg_extract_from_hdr_type type;
			/*!< Defines the type of header extraction:
			 * DPKG_FROM_HDR: use size & offset below;
			 * DPKG_FROM_FIELD: use field, size and offset below;
			 * DPKG_FULL_FIELD: use field below
			 */

			uint32_t field;
			/*!< One of the supported fields (NH_FLD_) */
			uint8_t size;
			/*!< Size in bytes */
			uint8_t offset;
			/*!< Byte offset */

			uint8_t hdr_index;
			/*!< Clear for cases not listed below;
			 * Used for protocols that may have more than a single
			 * header, 0 indicates an outer header;
			 * Supported protocols (possible values):
			 * NET_PROT_VLAN (0, HDR_INDEX_LAST);
			 * NET_PROT_MPLS (0, 1, HDR_INDEX_LAST);
			 * NET_PROT_IP(0, HDR_INDEX_LAST);
			 * NET_PROT_IPv4(0, HDR_INDEX_LAST);
			 * NET_PROT_IPv6(0, HDR_INDEX_LAST);
			 */
		} from_hdr;
		/*!< Used when 'type = DPKG_EXTRACT_FROM_HDR' */
		struct {
			uint8_t size;
			/*!< Size in bytes */
			uint8_t offset;
			/*!< Byte offset */
		} from_data;
		/*!< Used when 'type = DPKG_EXTRACT_FROM_DATA' */
		struct {
			uint8_t constant;
			/*!< A constant value */
			uint8_t num_of_repeats;
			/*!< Number of times the constant is to be
			 * entered to the key
			 */
		} constant;
		/*!< Used when 'type = DPKG_EXTRACT_CONSTANT' */
	} extract;
	/*!< Selects extraction method */

	uint8_t num_of_byte_masks;
	/*!< Defines the number of valid entries in the array below; This is
	 * also number of bytes to be used as masks
	 */
	struct dpkg_mask masks[DPKG_NUM_OF_MASKS];
	/*!< Mask parameters */
};

/**
 * @brief	A structure for defining a full Key Generation profile (rule)
 */
struct dpkg_profile_cfg {
	uint8_t num_extracts;
	/*!< Defines the number of valid entries in the array below */
	struct dpkg_extract extracts[DPKG_MAX_NUM_OF_EXTRACTS];
	/*!< Array of required extractions */
};

/** @} */

#endif /* __FSL_DPKG_H_ */

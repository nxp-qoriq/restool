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

#ifndef __FSL_DPKG_H_
#define __FSL_DPKG_H_

#include "fsl_net.h"

/*!
 * @Group grp_dpkg	DP Key Generator API
 *
 * @brief	Contains initialization APIs and runtime APIs for the
 *		Key Generator
 * @{
 */

/*!
 * @name Key Generator properties
 */
/* TODO - remove, take from soc_db */
#define DPKG_NUM_OF_MASKS					4
/*!< Number of profiles per CTLU Keygen */
/* TODO - remove, define and take from soc_db */
#define DPKG_MAX_NUM_OF_EXTRACTS				8
/*!< Number of extraction per Key */

/* @} */

/**
 * @brief	Enumeration type for selecting extraction by header types
 */
enum dpkg_extract_from_hdr_type {
	DPKG_FROM_HDR, /*!< Extract bytes from header */
	DPKG_FROM_FIELD, /*!< Extract bytes from header field */
	DPKG_FULL_FIELD
/*!< Extract a full field */
};

/**
 * @brief	Enumeration type for selecting extraction source
 *              (when it is not DPKG_EXTRACT_FROM_CONTEXT )
 */
enum dpkg_extract_from_context_type {
	DPKG_FROM_PARSE_RESULT, /*!< Extract from the Key generator result */
	DPKG_FROM_FCV, /*!< TODO: Extract from the Key generator result */
	DPKG_FROM_IFP
/*!< TODO: Extract from enqueue FQID */
/* add logic context's */
};

/**
 * @brief	Enumeration type for selecting extraction type
 */
enum dpkg_extract_type {
	DPKG_EXTRACT_FROM_HDR, /*!< Extract according to header */
	DPKG_EXTRACT_FROM_DATA, /*!< Extract from data that is not the header */
	DPKG_EXTRACT_FROM_CONTEXT, /*!< Extract from data that is not the header
	 */
	DPKG_EXTRACT_CONSTANT
/*!< Extract private info as specified by user */
};

/**
 * @brief	A structure for defining a single extraction mask
 */
struct dpkg_mask {
	uint8_t mask; /*!< a byte mask for the extracted content */
	uint8_t offset; /*!< offset within the extracted content */
};

/**
 * @brief	A structure for defining a single extraction
 */
struct dpkg_extract {
	enum dpkg_extract_type type;
	/*!< Type may be one of the members of
	 enum: dpkg_extract_type;
	 type determines how the union
	 below will be interpretted:
	 DPKG_EXTRACT_FROM_HDR: select "from_hdr";
	 DPKG_EXTRACT_FROM_DATA: select
	 "from_data";
	 DPKG_EXTRACT_FROM_CONTEXT: select
	 "from_context";
	 DPKG_EXTRACT_CONSTANT: select constant. */
	union {
		struct {
			enum net_prot prot;
			/*!< One of the headers supported by CTLU */

			enum dpkg_extract_from_hdr_type type;
			/*!< Defines the type of header extraction:
			 DPKG_FROM_HDR: use size & offset
			 below;
			 DPKG_FROM_FIELD: use field, size
			 & offset below;
			 DPKG_FULL_FIELD: use field below
			 */

			uint32_t field;
			/*!< One of the supported fields (NH_FLD_) */
			uint8_t size; /*!< Size in byte */
			uint8_t offset; /*!< Byte offset */

			uint8_t hdr_index;
			/*!< Clear for cases not listed below;
			 Used for protocols that may have more
			 than a single header, 0 indicates
			 outer header;
			 Supported protocols (possible values):
			 NET_PROT_VLAN (0, HDR_INDEX_LAST);
			 NET_PROT_MPLS (0, 1, HDR_INDEX_LAST);
			 NET_PROT_IP(0, HDR_INDEX_LAST);
			 NET_PROT_IPv4(0, HDR_INDEX_LAST);
			 NET_PROT_IPv6(0, HDR_INDEX_LAST);
			 */
		} from_hdr; /*!< used when type = DPKG_EXTRACT_FROM_HDR */
		struct {
			uint8_t size; /*!< Size in byte */
			uint8_t offset; /*!< Byte offset */
		} from_data; /*!< used when type = DPKG_EXTRACT_FROM_DATA */
		struct {
			enum dpkg_extract_from_context_type src;
			/*!< Non-header extraction source */
			uint8_t size; /*!< Size in byte */
			uint8_t offset; /*!< Byte offset */
		} from_context;
		/*!< used when type = DPKG_EXTRACT_FROM_CONTEXT */
		struct {
			uint8_t constant; /*!< a constant value */
			uint8_t num_of_repeats;
		/*!< The number of times
		 the constant is to be
		 entered to the key */
		} constant; /*!< used when type = DPKG_EXTRACT_CONSTANT */
	} extract; /*!< Selects extraction method */
	uint8_t num_of_byte_masks;
	/*!< Defines the valid number of entries in the
	 array below; This is also number of bytes
	 to be used as masks */
	struct dpkg_mask masks[DPKG_NUM_OF_MASKS]; /*!< Mask parameters */
};

/**
 * @brief	A structure for defining a full Key Generation profile (rule)
 */
struct dpkg_profile_cfg {
	uint8_t num_extracts; /*!< defines the valid number of
	 entries in the array below */
	struct dpkg_extract extracts[DPKG_MAX_NUM_OF_EXTRACTS];
/*!< An array of extractions definition. */
};

/** @} *//* end of grp_dpkg group */

#endif /* __FSL_DPKG_H_ */

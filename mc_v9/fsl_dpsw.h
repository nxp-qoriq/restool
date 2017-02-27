/* Copyright 2013-2015 Freescale Semiconductor Inc.
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
#ifndef __FSL_DPSW_V9_H
#define __FSL_DPSW_V9_H

#include "fsl_net.h"

/**
 * dpsw_open() - Open a control session for the specified object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @dpsw_id:	DPSW unique ID
 * @token:	Returned token; use in subsequent API calls
 *
 * This function can be used to open a control session for an
 * already created object; an object may have been declared in
 * the DPL or by calling the dpsw_create() function.
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent commands for
 * this specific object
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_open(struct fsl_mc_io	*mc_io,
	      uint32_t		cmd_flags,
	      int		dpsw_id,
	      uint16_t		*token);

/**
 * dpsw_close() - Close the control session of the object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 *
 * After this function is called, no further operations are
 * allowed on the object without opening a new control session.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_close(struct fsl_mc_io *mc_io,
	       uint32_t	cmd_flags,
	       uint16_t	token);

/* DPSW options */

/* Disable flooding */
#define DPSW_OPT_FLOODING_DIS		0x0000000000000001ULL
/* Disable Multicast */
#define DPSW_OPT_MULTICAST_DIS		0x0000000000000004ULL
/* Support control interface */
#define DPSW_OPT_CTRL_IF_DIS		0x0000000000000010ULL
/* Disable flooding metering */
#define DPSW_OPT_FLOODING_METERING_DIS  0x0000000000000020ULL
/* Enable metering */
#define DPSW_OPT_METERING_EN            0x0000000000000040ULL

/**
 * enum dpsw_component_type - component type of a bridge
 * @DPSW_COMPONENT_TYPE_C_VLAN: A C-VLAN component of an
 *   enterprise VLAN bridge or of a Provider Bridge used
 *   to process C-tagged frames
 * @DPSW_COMPONENT_TYPE_S_VLAN: An S-VLAN component of a
 *   Provider Bridge
 *
 */
enum dpsw_component_type {
	DPSW_COMPONENT_TYPE_C_VLAN = 0,
	DPSW_COMPONENT_TYPE_S_VLAN
};

/**
 * struct dpsw_cfg_v9 - DPSW configuration
 * @num_ifs: Number of external and internal interfaces
 * @adv: Advanced parameters; default is all zeros;
 *		 use this structure to change default settings
 */
struct dpsw_cfg_v9 {
	uint16_t		num_ifs;
	/**
	 * struct adv - Advanced parameters
	 * @options: Enable/Disable DPSW features (bitmap)
	 * @max_vlans: Maximum Number of VLAN's; 0 - indicates default 16
	 * @max_meters_per_if: Number of meters per interface
	 * @max_fdbs: Maximum Number of FDB's; 0 - indicates default 16
	 * @max_fdb_entries: Number of FDB entries for default FDB table;
	 *			0 - indicates default 1024 entries.
	 * @fdb_aging_time: Default FDB aging time for default FDB table;
	 *			0 - indicates default 300 seconds
	 * @max_fdb_mc_groups: Number of multicast groups in each FDB table;
	 *			0 - indicates default 32
	 * @component_type: Indicates the component type of this bridge
	 */
	struct {
		uint64_t	options;
		uint16_t	max_vlans;
		uint8_t	max_meters_per_if;
		uint8_t	max_fdbs;
		uint16_t	max_fdb_entries;
		uint16_t	fdb_aging_time;
		uint16_t	max_fdb_mc_groups;
		enum dpsw_component_type component_type;
	} adv;
};

/**
 * dpsw_create_v9() - Create the DPSW object.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @cfg:	Configuration structure
 * @token:	Returned token; use in subsequent API calls
 *
 * Create the DPSW object, allocate required resources and
 * perform required initialization.
 *
 * The object can be created either by declaring it in the
 * DPL file, or by calling this function.
 *
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent calls to
 * this specific object. For objects that are created using the
 * DPL file, call dpsw_open() function to get an authentication
 * token first
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_create_v9(struct fsl_mc_io	      *mc_io,
		   uint32_t		      cmd_flags,
		   const struct dpsw_cfg_v9   *cfg,
		   uint16_t		      *token);

/**
 * dpsw_destroy() - Destroy the DPSW object and release all its resources.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 *
 * Return:	'0' on Success; error code otherwise.
 */
int dpsw_destroy(struct fsl_mc_io	*mc_io,
		 uint32_t		cmd_flags,
		 uint16_t		token);

/**
 * dpsw_destroy() - Destroy the DPSW object and release all its resources.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 *
 * Return:	'0' on Success; error code otherwise.
 */
int dpsw_destroy(struct fsl_mc_io	*mc_io,
		 uint32_t		cmd_flags,
		 uint16_t		token);

/**
 * dpsw_get_irq_status() - Get the current status of any pending interrupts
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:		Token of DPSW object
 * @irq_index:	The interrupt index to configure
 * @status:		Returned interrupts status - one bit per cause:
 *					0 = no interrupt pending
 *					1 = interrupt pending
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_get_irq_status_v9(struct fsl_mc_io	*mc_io,
			   uint32_t		cmd_flags,
			   uint16_t		token,
			   uint8_t		irq_index,
			   uint32_t		*status);

/**
 * struct dpsw_attr_v9 - Structure representing DPSW attributes
 * @id: DPSW object ID
 * @version: DPSW version
 * @options: Enable/Disable DPSW features
 * @max_vlans: Maximum Number of VLANs
 * @max_meters_per_if:  Number of meters per interface
 * @max_fdbs: Maximum Number of FDBs
 * @max_fdb_entries: Number of FDB entries for default FDB table;
 *			0 - indicates default 1024 entries.
 * @fdb_aging_time: Default FDB aging time for default FDB table;
 *			0 - indicates default 300 seconds
 * @max_fdb_mc_groups: Number of multicast groups in each FDB table;
 *			0 - indicates default 32
 * @mem_size: DPSW frame storage memory size
 * @num_ifs: Number of interfaces
 * @num_vlans: Current number of VLANs
 * @num_fdbs: Current number of FDBs
 * @component_type: Component type of this bridge
 */
struct dpsw_attr_v9 {
	int		id;
	/**
	 * struct version - DPSW version
	 * @major: DPSW major version
	 * @minor: DPSW minor version
	 */
	struct {
		uint16_t major;
		uint16_t minor;
	} version;
	uint64_t	options;
	uint16_t	max_vlans;
	uint8_t	max_meters_per_if;
	uint8_t	max_fdbs;
	uint16_t	max_fdb_entries;
	uint16_t	fdb_aging_time;
	uint16_t	max_fdb_mc_groups;
	uint16_t	num_ifs;
	uint16_t	mem_size;
	uint16_t	num_vlans;
	uint8_t		num_fdbs;
	enum dpsw_component_type component_type;
};

/**
 * dpsw_get_attributes_v9() - Retrieve DPSW attributes
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:		Token of DPSW object
 * @attr:		Returned DPSW attributes
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_get_attributes_v9(struct fsl_mc_io	*mc_io,
			   uint32_t		cmd_flags,
			   uint16_t		token,
			   struct dpsw_attr_v9	*attr);

/**
 * dpsw_get_irq_mask() - Get interrupt mask.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:		Token of DPSW object
 * @irq_index:	The interrupt index to configure
 * @mask:		Returned event mask to trigger interrupt
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_get_irq_mask(struct fsl_mc_io	*mc_io,
		      uint32_t		cmd_flags,
		      uint16_t		token,
		      uint8_t		irq_index,
		      uint32_t		*mask);

/**
 * dpsw_get_irq_status() - Get the current status of any pending interrupts
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:		Token of DPSW object
 * @irq_index:	The interrupt index to configure
 * @status:		Returned interrupts status - one bit per cause:
 *					0 = no interrupt pending
 *					1 = interrupt pending
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_get_irq_status(struct fsl_mc_io	*mc_io,
			uint32_t		cmd_flags,
			uint16_t		token,
			uint8_t		irq_index,
			uint32_t		*status);

#endif /* __FSL_DPSW_V9_H */

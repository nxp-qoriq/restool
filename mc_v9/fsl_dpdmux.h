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
#ifndef __FSL_DPDMUX_V9_H
#define __FSL_DPDMUX_V9_H

/* Data Path Demux API
 * Contains API for handling DPDMUX topology and functionality
 */

/**
 * dpdmux_open() - Open a control session for the specified object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @dpdmux_id:		DPDMUX unique ID
 * @token:		Returned token; use in subsequent API calls
 *
 * This function can be used to open a control session for an
 * already created object; an object may have been declared in
 * the DPL or by calling the dpdmux_create() function.
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent commands for
 * this specific object.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdmux_open(struct fsl_mc_io	 *mc_io,
		uint32_t		 cmd_flags,
		int			 dpdmux_id,
		uint16_t		 *token);

/**
 * dpdmux_close() - Close the control session of the object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:		Token of DPDMUX object
 *
 * After this function is called, no further operations are
 * allowed on the object without opening a new control session.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdmux_close(struct fsl_mc_io	*mc_io,
		 uint32_t		cmd_flags,
		 uint16_t		token);

/*!
 * @name DPDMUX general options
 */
#define DPDMUX_OPT_BRIDGE_EN		0x0000000000000002ULL
/*!< Enable bridging between internal interfaces */
/* @} */

#define DPDMUX_IRQ_INDEX_IF			0x0000
#define DPDMUX_IRQ_INDEX		0x0001

/*!< IRQ event - Indicates that the link state changed */
#define DPDMUX_IRQ_EVENT_LINK_CHANGED	0x0001

/**
 * enum dpdmux_manip - DPDMUX manipulation operations
 * @DPDMUX_MANIP_NONE:	No manipulation on frames
 * @DPDMUX_MANIP_ADD_REMOVE_S_VLAN: Add S-VLAN on egress, remove it on ingress
 */
enum dpdmux_manip {
	DPDMUX_MANIP_NONE = 0x0,
	DPDMUX_MANIP_ADD_REMOVE_S_VLAN = 0x1
};

/**
 * enum dpdmux_method - DPDMUX method options
 * @DPDMUX_METHOD_NONE: no DPDMUX method
 * @DPDMUX_METHOD_C_VLAN_MAC: DPDMUX based on C-VLAN and MAC address
 * @DPDMUX_METHOD_MAC: DPDMUX based on MAC address
 * @DPDMUX_METHOD_C_VLAN: DPDMUX based on C-VLAN
 * @DPDMUX_METHOD_S_VLAN: DPDMUX based on S-VLAN
 */
enum dpdmux_method {
	DPDMUX_METHOD_NONE = 0x0,
	DPDMUX_METHOD_C_VLAN_MAC = 0x1,
	DPDMUX_METHOD_MAC = 0x2,
	DPDMUX_METHOD_C_VLAN = 0x3,
	DPDMUX_METHOD_S_VLAN = 0x4
};

/**
 * struct dpdmux_cfg - DPDMUX configuration parameters
 * @method: Defines the operation method for the DPDMUX address table
 * @manip: Required manipulation operation
 * @num_ifs: Number of interfaces (excluding the uplink interface)
 * @adv: Advanced parameters; default is all zeros;
 *	 use this structure to change default settings
 */
struct dpdmux_cfg_v9 {
	enum dpdmux_method	method;
	enum dpdmux_manip	manip;
	uint16_t		num_ifs;
	/**
	 * struct adv - Advanced parameters
	 * @options: DPDMUX options - combination of 'DPDMUX_OPT_<X>' flags
	 * @max_dmat_entries: Maximum entries in DPDMUX address table
	 *		0 - indicates default: 64 entries per interface.
	 * @max_mc_groups: Number of multicast groups in DPDMUX table
	 *		0 - indicates default: 32 multicast groups
	 * @max_vlan_ids: max vlan ids allowed in the system -
	 *		relevant only case of working in mac+vlan method.
	 *		0 - indicates default 16 vlan ids.
	 */
	struct {
		uint64_t options;
		uint16_t max_dmat_entries;
		uint16_t max_mc_groups;
		uint16_t max_vlan_ids;
	} adv;
};

/**
 * dpdmux_create() - Create the DPDMUX object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @cfg:	Configuration structure
 * @token:	Returned token; use in subsequent API calls
 *
 * Create the DPDMUX object, allocate required resources and
 * perform required initialization.
 *
 * The object can be created either by declaring it in the
 * DPL file, or by calling this function.
 *
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent calls to
 * this specific object. For objects that are created using the
 * DPL file, call dpdmux_open() function to get an authentication
 * token first.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdmux_create_v9(struct fsl_mc_io		*mc_io,
		     uint32_t			cmd_flags,
		     const struct dpdmux_cfg_v9	*cfg,
		     uint16_t			*token);

/**
 * dpdmux_destroy() - Destroy the DPDMUX object and release all its resources.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDMUX object
 *
 * Return:	'0' on Success; error code otherwise.
 */
int dpdmux_destroy(struct fsl_mc_io	*mc_io,
		   uint32_t		cmd_flags,
		   uint16_t		token);

/**
 * dpdmux_get_irq_status() - Get the current status of any pending interrupts.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDMUX object
 * @irq_index:	The interrupt index to configure
 * @status:	Returned interrupts status - one bit per cause:
 *			0 = no interrupt pending
 *			1 = interrupt pending
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdmux_get_irq_status_v9(struct fsl_mc_io	*mc_io,
			     uint32_t		cmd_flags,
			     uint16_t		token,
			     uint8_t		irq_index,
			     uint32_t		*status);

/**
 * struct dpdmux_attr - Structure representing DPDMUX attributes
 * @id: DPDMUX object ID
 * @version: DPDMUX version
 * @options: Configuration options (bitmap)
 * @method: DPDMUX address table method
 * @manip: DPDMUX manipulation type
 * @num_ifs: Number of interfaces (excluding the uplink interface)
 * @mem_size: DPDMUX frame storage memory size
 */
struct dpdmux_attr_v9 {
	int			id;
	/**
	 * struct version - DPDMUX version
	 * @major: DPDMUX major version
	 * @minor: DPDMUX minor version
	 */
	struct {
		uint16_t	major;
		uint16_t	minor;
	} version;
	uint64_t		options;
	enum dpdmux_method	method;
	enum dpdmux_manip	manip;
	uint16_t		num_ifs;
	uint16_t		mem_size;
};

/**
 * dpdmux_get_attributes() - Retrieve DPDMUX attributes
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDMUX object
 * @attr:	Returned object's attributes
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdmux_get_attributes_v9(struct fsl_mc_io	    *mc_io,
			     uint32_t		    cmd_flags,
			     uint16_t		    token,
			     struct dpdmux_attr_v9  *attr);

/**
 * dpdmux_get_irq_mask() - Get interrupt mask.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDMUX object
 * @irq_index:	The interrupt index to configure
 * @mask:	Returned event mask to trigger interrupt
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdmux_get_irq_mask(struct fsl_mc_io	*mc_io,
			uint32_t		cmd_flags,
			uint16_t		token,
			uint8_t			irq_index,
			uint32_t		*mask);

/**
 * dpdmux_get_irq_status() - Get the current status of any pending interrupts.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDMUX object
 * @irq_index:	The interrupt index to configure
 * @status:	Returned interrupts status - one bit per cause:
 *			0 = no interrupt pending
 *			1 = interrupt pending
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdmux_get_irq_status(struct fsl_mc_io	*mc_io,
			  uint32_t		cmd_flags,
			  uint16_t		token,
			  uint8_t		irq_index,
			  uint32_t		*status);

#endif /* __FSL_DPDMUX_V9_H */

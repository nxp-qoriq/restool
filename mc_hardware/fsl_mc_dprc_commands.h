/* Copyright 2013 Freescale Semiconductor, Inc. */
/*!
 *  @file    fsl_mc_dprc_commands.h
 *  @brief   Management Complex (MC) Data Path Resource Container (DPRC)
 *	     commands
 */
#ifndef _FSL_MC_DPRC_COMMANDS_H
#define _FSL_MC_DPRC_COMMANDS_H

#include <fsl_mc_cmd_common.h>

/*!
 * @Group grp_dprc	Data Path Resource Container API
 *
 * @brief	Contains DPRC API for managing and querying LDPAA resources
 * @{
 */

/*!
 * Set this value as the ICID value in dprc_cfg structure when creating a
 * container, in case the ICID is not selected by the user and should be
 * allocated by the DPRC from the pool of ICIDs.
 */
#define DPRC_GET_ICID_FROM_POOL		(uint16_t)(~(0))

/**
 * Resource types defines
 */
enum dp_resource_types {
	DP_RES_INVALID =	    0,
	DP_RES_MCPID =		    1,  /*!< Management portals */
	DP_RES_SWP =		    2,  /*!< Software portals */
	DP_RES_BPID =		    3,  /*!< Buffer pools */
	DP_RES_SWP_CHANNEL =	    4,  /*!< Channels */
	DP_RES_FQID =		    5,  /*!< Frame queues */
	DP_RES_QPR =		    6,  /*!< Queuing priority record */
	DP_RES_QDID =		    7,  /*!< Queuing destinations */
	DP_RES_CGID =		    8,  /*!< Congestion groups */
	DP_RES_CEETM_CQ_CHANNEL =   11, /*!< Class Queue Channel ID */
	DP_RES_TABLE_ID	=	    13, /*!< Classification tables */
	DP_RES_KEY_PROFILE_ID =	    14, /*!< Key ID*/
	DP_RES_PLPID =		    15, /*!< Policer profiles ID */
	DP_RES_PRPID =		    16, /* Parser profile ID */
	DP_RES_PPID =		    17, /*!< Physical ports */
	DP_RES_IFPID =		    18, /*!< Interface profiles */
	DP_RES_TASK =		    19, /*!< AIOP tasks*/
	DP_RES_RPLR =		    22, /*!< Replication list record */
	DP_RES_DPSW_PORT =	    23, /*!< DPSW port*/
	DP_RES_POLICY_ID =	    24, /*!< Policy ID */
};

/**
 * Device types definition
 */
enum dp_device_types {
	DP_DEV_INVALID =	0,
	DP_DEV_DPRC =		100,
	DP_DEV_DPNI =		101,
	DP_DEV_DPIO =		102,
	DP_DEV_DPBP =		103,
	DP_DEV_DPSW =		104,
	DP_DEV_DPDMUX =		105,
	DP_DEV_DPMAC =		106,
	DP_DEV_DPLAG =		107,
	DP_DEV_DPCI =		108,
	DP_DEV_DPCON =		109,
};

/*!
 * @name Resource request options
 */
#define DPRC_RES_REQ_OPT_EXPLICIT		0x00000001
/*!< Explicit resource id request - Relevant only for resources
 * request (and not devices). The requested resources are explicit and
 * sequential. The base ID is given at res_req at base_align field */
#define DPRC_RES_REQ_OPT_ALIGNED		0x00000002
/*!< Aligned resources request - Relevant only for resources
 * request (and not devices). Indicates that resources base id should be
 * sequential and aligned to the value given at dprc_res_req base_align field */
#define DPRC_RES_REQ_OPT_PLUGGED		0x00000004
/*!< Plugged Flag - Relevant only for device assignment request.
 * Indicates that after all devices assigned. An interrupt will be invoked at
 * the relevant GPP. The assigned device will be marked as plugged.
 * plugged devices cant be assigned from their container */
/* @} */

/*!
 * @name Container general options
 *
 * These options may be selected at container creation by the container creator
 * and can be retreived using dprc_get_attributes()
 */
#define DPRC_CFG_OPT_SPAWN_ALLOWED		0x00000001
/*!< Spawn Policy Option allowed - Indicates that the new container is allowd
 * to spawn and have its own child containers. */
#define DPRC_CFG_OPT_ALLOC_ALLOWED		0x00000002
/*!< General Container allocation policy - Indicates that the new container is
 * allowed to allocate requested resources from its parent container; if not
 * set, the container is only allowed to use resources in its own pools; Note
 * that this is a container's global policy, but the parent container may
 * override it and set specific quota per resource type. */
#define DPRC_CFG_OPT_DEVICE_CREATE_ALLOWED	0x00000004
/*!< Device initialization allowed - software context associated with this
 * container is allowed to invoke device intiialization operations. */
#define DPRC_CFG_OPT_TOPOLOGY_CHANGES_ALLOWED	0x00000008
/*!< Topology change allowed - software context associated with this
 * container is allowed to invoke topology operations, such as attach/detach
 * of network devices. */
#define DPRC_CFG_OPT_IOMMU_BYPASS		0x00000010
/*!<IOMMU bypass - indicates whether devices of this container are permitted
 * to bypass the IOMMU.  */
#define DPRC_CFG_OPT_AIOP			0x00000020
/*!<AIOP -Indicates that container belongs to aiop.  */
/* @} */

/*!
 * @name Device Attributes Flags
 */
#define DPRC_DEV_STATE_OPEN		0x00000001
/*!< Opened state - Indicates that a device was opened by at least one owner */
#define DPRC_DEV_STATE_PLUGGED		0x00000002
/*!< Plugged state - Indicates that a device is plugged */
/* @} */

/*!
 * @name Irq
 */
#define DPRC_NUM_OF_IRQS		1
/*!< Number of dprc's IRQs */
/* @} */

/*!
 * @name Device irq events
 */
#define DPRC_IRQ_EVENT_DEVICE_ASSIGNED		0x00000001
/*!< Irq event - Indicates that a new device assigned to the container */
#define DPRC_IRQ_EVENT_DEVICE_UNASSIGNED	0x00000002
/*!< Irq event - Indicates that a device was unassigned from the container */
#define DPRC_IRQ_EVENT_PRES_ASSIGNED		0x00000004
/*!< Irq event - Indicates that resources assigned to the container */
#define DPRC_IRQ_EVENT_PRES_UNASSIGNED		0x00000008
/*!< Irq event - Indicates that resources unassigned from the container */
#define DPRC_IRQ_EVENT_CONTAINER_DESTROYED	0x00000010
/*!< Irq event - Indicates that one of the descendant containers that opened by
 * this container is destroyed */
#define DPRC_IRQ_EVENT_DEVICE_DESTROYED		0x00000011
/*!< Irq event - Indicates that on one of the container's opened device is
destroyed */
/* @} */

/* Command IDs */
#define DPRC_CMDID_CREATE_CONT			0x151
#define DPRC_CMDID_DESTROY_CONT			0x152
#define DPRC_CMDID_GET_CONT_ID			0x830
#define DPRC_CMDID_RESET_CONT			0x154
#define DPRC_CMDID_SET_RES_QUOTA		0x155
#define DPRC_CMDID_GET_RES_QUOTA		0x156
#define DPRC_CMDID_ASSIGN			0x157
#define DPRC_CMDID_UNASSIGN			0x158
#define DPRC_CMDID_GET_DEV_COUNT		0x159
#define DPRC_CMDID_GET_DEVICE			0x15A
#define DPRC_CMDID_GET_RES_COUNT		0x15B
#define DPRC_CMDID_GET_RES_IDS			0x15C
#define DPRC_CMDID_GET_ATTR			0x15D
#define DPRC_CMDID_GET_DEV_REG			0x15E
#define DPRC_CMDID_SET_IRQ			0x15F
#define DPRC_CMDID_GET_IRQ			0x160
#define DPRC_CMDID_SET_IRQ_ENABLE		0x161
#define DPRC_CMDID_GET_IRQ_ENABLE		0x162
#define DPRC_CMDID_SET_IRQ_MASK			0x163
#define DPRC_CMDID_GET_IRQ_MASK			0x164
#define DPRC_CMDID_GET_IRQ_STATUS		0x165
#define DPRC_CMDID_CLEAR_IRQ_STATUS		0x166
#define DPRC_CMDID_CONNECT			0x167
#define DPRC_CMDID_DISCONNECT			0x168
#define DPRC_CMDID_OPEN				0x805
#define DPRC_CMDID_CLOSE			0x800

/* Command sizes */
#define DPRC_CMDSZ_CREATE_CONT			(8 * 2)
#define DPRC_CMDSZ_DESTROY_CONT			8
#define DPRC_CMDSZ_GET_CONT_ID			8
#define DPRC_CMDSZ_RESET_CONT			8
#define DPRC_CMDSZ_SET_RES_QUOTA		8
#define DPRC_CMDSZ_GET_RES_QUOTA		8
#define DPRC_CMDSZ_ASSIGN			(8 * 3)
#define DPRC_CMDSZ_UNASSIGN			(8 * 2)
#define DPRC_CMDSZ_GET_DEV_COUNT		0
#define DPRC_CMDSZ_GET_DEVICE			(8 * 3)
#define DPRC_CMDSZ_GET_RES_COUNT		8
#define DPRC_CMDSZ_GET_RES_IDS			8
#define DPRC_CMDSZ_GET_ATTR			(8 * 2)
#define DPRC_CMDSZ_GET_DEV_REG			(8 * 3)
#define DPRC_CMDSZ_SET_IRQ			(8 * 2)
#define DPRC_CMDSZ_GET_IRQ			(8 * 3)
#define DPRC_CMDSZ_SET_IRQ_ENABLE		8
#define DPRC_CMDSZ_GET_IRQ_ENABLE		8
#define DPRC_CMDSZ_SET_IRQ_MASK			8
#define DPRC_CMDSZ_GET_IRQ_MASK			8
#define DPRC_CMDSZ_GET_IRQ_STATUS		8
#define DPRC_CMDSZ_CLEAR_IRQ_STATUS		8
#define DPRC_CMDSZ_CONNECT			(8 * 3)
#define DPRC_CMDSZ_DISCONNECT			(8 * 2)
#define DPRC_CMDSZ_OPEN				8
#define DPRC_CMDSZ_CLOSE			0

/**
 * @brief	Resource request descriptor, to be used in assignment or
 *		un-assignment of resources and devices.
 */
struct dprc_res_req {
	uint16_t type;
	/*!< Resource/device type: one of DP_RES_ or DP_DEV_ values;
	 * Note: it is not possible to assign/unassign DP_DEV_DPRC devices */
	uint32_t num;
	/*!< Number of resources */
	uint32_t options;
	/*!< Request options: combination of DPRC_RES_REQ_OPT_ options */
	uint32_t id_base_align;
	/*!<
	 * In case of explicit assignment (DPRC_RES_REQ_OPT_EXPLICIT is set at
	 * option), this field represents the required base ID for resource
	 * allocation; In case of aligned assignment (DPRC_RES_REQ_OPT_ALIGNED
	 * is set at option), this field indicates the required alignment for
	 * the resource ID(s) - use 0 if there is no alignment or explicit id
	 * requirements.
	 */
};

/**
 * @brief	Device descriptor, returned from dprc_get_device()
 */
struct dprc_dev_desc {
	uint16_t vendor;
	/*!< Device vendor identifier */
	char type[16];
	/*!< Type of device: one of DP_DEV_ values */
	uint16_t id;
	/*!< ID of logical device resource */
	uint8_t rev_major;
	/*!< Major revision number */
	uint8_t rev_minor;
	/*!< Minor  revision number */
	uint8_t irq_count;
	/*!< Number of interrupts supported by the device */
	uint8_t region_count;
	/*!< Number of mappable regions supported by the device */
	uint32_t state;
	/*!< Device state: combination of DPRC_DEV_STATE_ states */
};

/**
 * @brief	Mappable region descriptor, returned by dprc_get_dev_region()
 */
struct dprc_region_desc {
	uint64_t base_paddr;
	/*!< Region base physical address */
	uint32_t size;
	/*!< Region size (in bytes) */
};

/**
 * @brief	Iteration status, for use with dprc_get_res_ids() function
 *
 */
enum dprc_iter_status {
	DPRC_ITER_STATUS_FIRST = 0,
	/*!< Perform first iteration */
	DPRC_ITER_STATUS_MORE = 1,
	/*!< Indicates more/next iteration is needed */
	DPRC_ITER_STATUS_LAST = 2
	/*!< Indicates last iteration */
};

/**
 * @brief	Resource Id range descriptor, Used at dprc_get_res_ids() and
 *		contains one range details.
 */
struct dprc_res_ids_range_desc {
	uint16_t base_id;
	/*!< Base resource ID of this range */
	uint16_t last_id;
	/*!< Last resource ID of this range */
	enum dprc_iter_status iter_status;
	/*!< Iterartion status - should be set to DPRC_ITER_STATUS_FIRST at
	 * first iteration; while the returned marker is DPRC_ITER_STATUS_MORE,
	 * additional iterations are needed, until the returned marker is
	 * DPRC_ITER_STATUS_LAST.
	 */
};

/**
 * @brief	Container attributes, returned by dprc_get_attributes()
 */
struct dprc_attributes {
	uint16_t container_id;
	/*!< Container's ID */
	uint16_t icid;
	/*!< Container's ICID */
	uint16_t portal_id;
	/*!< Container's portal ID */
	uint64_t options;
	/*!< Container's options as set at container's creation */
};

/**
 * @brief	Container configuration options, used in dprc_create_container()
 */
struct dprc_cfg {
	uint16_t icid;
	/*!< Container's ICID; if set to DPRC_GET_ICID_FROM_POOL, a free ICID
	 * will be allocated by the DPRC */
	uint64_t options;
	/*!< Combination of DPRC_CFG_OPT_ options */
};

/**
 * @brief	Obtains the container id associated with a given portal.
 */

static inline void build_cmd_dprc_get_container_id(struct mc_command *cmd)
{
	cmd->header = mc_encode_cmd_header(
				DPRC_CMDID_GET_CONT_ID,
				DPRC_CMDSZ_GET_CONT_ID,
				CMDIF_PRI_LOW,
				0);

	memset(&cmd->data, 0, sizeof(cmd->data));
}

static inline void parse_resp_dprc_get_container_id(
	struct mc_command *cmd,
	uint32_t *container_id)
{
	*container_id = lower_32_bits(cmd->data.params[0]);
}

/**
 * @brief	Opens a DPRC object for use
 *
 */

static inline void build_cmd_dprc_open(
	struct mc_command *cmd,
	uint32_t container_id)
{
	cmd->header = mc_encode_cmd_header(
				DPRC_CMDID_OPEN,
				DPRC_CMDSZ_OPEN,
				CMDIF_PRI_LOW,
				0);

	memset(&cmd->data, 0, sizeof(cmd->data));
	cmd->data.params[0] = container_id;
}

static inline void parse_resp_dprc_open(
	struct mc_command *cmd,
	uint16_t *dprc_handle)
{
	*dprc_handle = (cmd->header >> 38) & 0x3ff;
}

/**
 * @brief	Closes the DPRC object handle
 *
 * No further operations on the object are allowed after this call without
 * re-opening the object.
 *
 * @param[in]	dprc_handle		Parent DPRC object handle
 *
 * @returns	'0' on Success; Error code otherwise.
 */
static inline void build_cmd_dprc_close(
	struct mc_command *cmd,
	uint16_t dprc_handle)
{
	cmd->header = mc_encode_cmd_header(
				DPRC_CMDID_CLOSE,
				DPRC_CMDSZ_CLOSE,
				CMDIF_PRI_HIGH,
				dprc_handle);
}

/**
 * @brief	Creates a child container
 *
 * @param[in]	dprc_handle		Parent DPRC object handle
 * @param[in]	cfg			Child container configuration
 * @param[out]	child_container_id	Child container ID
 * @param[out]	child_portal_paddr	Base physical address of the
 *					child portal
 */

static inline void build_cmd_dprc_create_container(
	struct mc_command *cmd,
	uint16_t dprc_handle,
	struct dprc_cfg *cfg)
{
	cmd->header = mc_encode_cmd_header(
				DPRC_CMDID_CREATE_CONT,
				DPRC_CMDSZ_CREATE_CONT,
				CMDIF_PRI_LOW,
				dprc_handle);

	memset(&cmd->data, 0, sizeof(cmd->data));
	cmd->data.params[0] = (uint64_t)cfg->icid << 32;
	cmd->data.params[0] |= lower_32_bits(cfg->options);
}

static inline void parse_resp_dprc_create_container(
	struct mc_command *cmd,
	uint32_t *child_container_id,
	uint64_t *child_portal_paddr)
{
	*child_container_id = lower_32_bits(cmd->data.params[1]);
	*child_portal_paddr = cmd->data.params[2];
}

/**
 * @brief	Destroys a child container.
 *
 * This function terminates the child container, so following this call the
 * child container ID becomes invalid.
 *
 * Notes:
 * - All resources and devices of the destroyed container are returned to the
 * parent container or destroyed if were created be the destroyed container.
 * - This function destroy all the child containers of the specified
 *   container prior to destroying the container itself.
 *
 * @param[in]	portal_io		Pointer to MC portal I/O object
 * @param[in]	dprc_handle		Parent DPRC object handle
 * @param[in]	child_container_id	ID of the container to destroy
 *
 * @warning	Only the parent container is allowed to destroy a child policy
 *		Container 0 can't be destroyed
 */

static inline void build_cmd_dprc_destroy_container(
	struct mc_command *cmd,
	uint16_t dprc_handle,
	uint32_t child_container_id)
{
	cmd->header = mc_encode_cmd_header(
				DPRC_CMDID_DESTROY_CONT,
				DPRC_CMDSZ_DESTROY_CONT,
				CMDIF_PRI_LOW,
				dprc_handle);

	memset(&cmd->data, 0, sizeof(cmd->data));
	cmd->data.params[0] = child_container_id;
}

/**
 * @brief	Resets a child container.
 *
 * In case a software context crashes or becomes non-responsive, the parent
 * may wish to reset its resources container before the software context is
 * restarted.
 *
 * This routine informs all devices assigned to the child container that the
 * container is being reset, so they may perform any cleanup operations that are
 * needed. All device handles that were owned by the child container shall be
 * closed.
 *
 * Note that such request may be submitted even if the child software context
 * has not crashed, but the resulting device cleanup operations will not be
 * aware of that.
 *
 * @param[in]	dprc_handle		Parent DPRC object handle
 * @param[in]	child_container_id	ID of the container to reset
 */

static inline void build_cmd_dprc_reset_container(
	struct mc_command *cmd,
	uint16_t dprc_handle,
	uint32_t child_container_id)
{
	cmd->header = mc_encode_cmd_header(
				DPRC_CMDID_RESET_CONT,
				DPRC_CMDSZ_RESET_CONT,
				CMDIF_PRI_LOW,
				dprc_handle);

	memset(&cmd->data, 0, sizeof(cmd->data));
	cmd->data.params[0] = child_container_id;
}

/**
 * @brief	Assigns devices or resource to a child container.
 *
 * Assignment is usually done by a parent (this DPRC) to one of its child
 * containers.
 *
 * According to the DPRC allocation policy, the assigned resources may be taken
 * (allocated) from the container's ancestors, if not enough resources are
 * available in the container itself.
 *
 * The type of assignment depends on the dprc_res_req options, as follows:
 * - DPRC_RES_REQ_OPT_EXPLICIT: indicates that assigned resources should have
 *   the explicit base ID specified at the id_base_align field of res_req.
 * - DPRC_RES_REQ_OPT_ALIGNED: indicates that the assigned resources should be
 *   aligned to the value given at id_base_align field of res_req.
 * - DPRC_RES_REQ_OPT_PLUGGED: Relevant only for device assignment,
 *   and indicates that the device must be set to the plugged state.
 *
 * If IRQ information has been set in the child DPRC, it will signal an
 * interrupt following every change in its device assignment.
 *
 * @param[in]	dprc_handle	Parent DPRC object handle
 * @param[in]	container_id	ID of the child container
 * @param[in]	res_req		Describes the type and amount of resources to
 *				assign to the given container.
 */

static inline void build_cmd_dprc_assign(
	struct mc_command *cmd,
	uint16_t dprc_handle,
	uint32_t child_container_id,
	struct dprc_res_req *res_req)
{
	cmd->header = mc_encode_cmd_header(
				DPRC_CMDID_ASSIGN,
				DPRC_CMDSZ_ASSIGN,
				CMDIF_PRI_LOW,
				dprc_handle);

	memset(&cmd->data, 0, sizeof(cmd->data));
	cmd->data.params[0] = child_container_id;
	cmd->data.params[0] |= (res_req->type & 0x3ffULL) << 32;
	cmd->data.params[1] = res_req->num;
	cmd->data.params[1] |= (uint64_t)res_req->id_base_align << 32;
	cmd->data.params[2] = res_req->options;
}

/**
 * @brief	Un-assigns devices or resources from a child container
 *		and moves them into this (parent) DPRC.
 *
 * Un-assignment of resources moves arbitrary or explicit resources
 * from the specified child container to the parent container.
 *
 * Un-assignment of devices can succeed only if the device is not in the
 * plugged or opened state.
 *
 * A container may use this function with its own ID in order to change a
 * device state to plugged or unplugged.
 *
 * @param[in]	dprc_handle		Parent DPRC object handle
 * @param[in]	child_container_id	ID of the child container
 * @param[in]	res_req			Describes the type and amount of
 *					resources to un-assign from the child
 *					container
 */

static inline void build_cmd_dprc_unassign(
	struct mc_command *cmd,
	uint16_t dprc_handle,
	uint32_t child_container_id,
	struct dprc_res_req *res_req)
{
	cmd->header = mc_encode_cmd_header(
			DPRC_CMDID_UNASSIGN,
			DPRC_CMDSZ_UNASSIGN,
			CMDIF_PRI_LOW,
			dprc_handle);

	memset(&cmd->data, 0, sizeof(cmd->data));
	cmd->data.params[0] = child_container_id;
	cmd->data.params[0] |= (res_req->type & 0x3ffULL) << 32;
	cmd->data.params[1] = res_req->num;
	cmd->data.params[1] |= (uint64_t)res_req->id_base_align << 32;
	cmd->data.params[2] = res_req->options;
}

/**
 * @brief	Obtains the number of devices in the DPRC
 *
 * @param[in]	dprc_handle	DPRC object handle
 * @param[out]	dev_count	Number of devices assigned to the DPRC
 */

static inline void build_cmd_dprc_get_device_count(
	struct mc_command *cmd,
	uint16_t dprc_handle)
{
	cmd->header = mc_encode_cmd_header(
				DPRC_CMDID_GET_DEV_COUNT,
				DPRC_CMDSZ_GET_DEV_COUNT,
				CMDIF_PRI_LOW,
				dprc_handle);

	memset(&cmd->data, 0, sizeof(cmd->data));
}

static inline void parse_resp_dprc_get_device_count(
	struct mc_command *cmd,
	uint32_t *dev_count)
{
	*dev_count = upper_32_bits(cmd->data.params[0]);
}

/**
 * @brief	Obtains general information on a device
 *
 * @details	The device descriptors are retrieved one by one by incrementing
 *		dev_index up to (not including) the value of dev_count returned
 *		from dprc_get_device_count(). dprc_get_device_count() must
 *		be called prior to dprc_get_device_count().
 *
 * @param[in]	dprc_handle	DPRC object handle
 * @param[in]	dev_index	Index of the device to be queried (< dev_count)
 * @param[out]	dev_desc	Returns the requested device descriptor
 */

static inline void build_cmd_dprc_get_device(
	struct mc_command *cmd,
	uint16_t dprc_handle,
	uint16_t dev_index)
{
	cmd->header = mc_encode_cmd_header(
				DPRC_CMDID_GET_DEVICE,
				DPRC_CMDSZ_GET_DEVICE,
				CMDIF_PRI_LOW,
				dprc_handle);

	memset(&cmd->data, 0, sizeof(cmd->data));
	cmd->data.params[0] = dev_index & 0xffffULL;
}

static inline void parse_resp_dprc_get_device(
	struct mc_command *cmd,
	struct dprc_dev_desc *dev_desc)
{
	dev_desc->rev_minor = cmd->data.params[0] & 0xff;
	dev_desc->rev_major = (cmd->data.params[0] >> 8) & 0xff;
	dev_desc->id = upper_32_bits(cmd->data.params[0]);
	dev_desc->vendor = cmd->data.params[1] & 0xffff;
	dev_desc->irq_count = (cmd->data.params[1] >> 16) & 0xff;
	dev_desc->region_count = (cmd->data.params[1] >> 24) & 0xff;
	dev_desc->state = upper_32_bits(cmd->data.params[1]);
	memcpy(dev_desc->type, &cmd->data.params[3], 8);
	memcpy(&dev_desc->type[8], &cmd->data.params[4], 8);
}

/**
 * @brief	Obtains the number of free resources that are assigned
 *		to this container, by resource type
 *
 * @param[in]	dprc_handle	DPRC object handle
 * @param[in]	res_type	Resource type
 * @param[out]	res_count	Number of free resources of the given
 *				resource type that are assigned to this DPRC
 */

static inline void build_cmd_dprc_get_res_count(
	struct mc_command *cmd,
	uint16_t dprc_handle,
	char *res_type)
{
	cmd->header = mc_encode_cmd_header(
				DPRC_CMDID_GET_RES_COUNT,
				DPRC_CMDSZ_GET_RES_COUNT,
				CMDIF_PRI_LOW,
				dprc_handle);

	memset(&cmd->data, 0, sizeof(cmd->data));
	memcpy(&cmd->data.params[1], res_type, 8);
	memcpy(&cmd->data.params[2], &res_type[8], 8);
}

static inline void parse_resp_dprc_get_res_count(
	struct mc_command *cmd,
	uint32_t *res_count)
{
	*res_count = lower_32_bits(cmd->data.params[0]);
}

/**
 * @brief	Obtains IDs of free resources in the container
 *
 * @param[in]	portal_io	Pointer to MC portal I/O object
 * @param[in]	dprc_handle	DPRC object handle
 * @param[in]	res_type	Selects the resource type
 * @param[in,out] range_desc	range descriptor
 */

static inline void build_cmd_dprc_get_res_ids(
	struct mc_command *cmd,
	uint16_t dprc_handle,
	char *res_type,
	struct dprc_res_ids_range_desc *range_desc)
{
	cmd->header = mc_encode_cmd_header(
				DPRC_CMDID_GET_RES_IDS,
				DPRC_CMDSZ_GET_RES_IDS,
				CMDIF_PRI_LOW,
				dprc_handle);

	memset(&cmd->data, 0, sizeof(cmd->data));
	cmd->data.params[0] |= (range_desc->iter_status & 0x7fULL) << 42;
	cmd->data.params[1] = range_desc->base_id;
	cmd->data.params[1] |= (uint64_t)range_desc->last_id << 32;
	memcpy(&cmd->data.params[2], res_type, 8);
	memcpy(&cmd->data.params[3], &res_type[8], 8);
}

static inline void parse_resp_dprc_get_res_ids(
	struct mc_command *cmd,
	struct dprc_res_ids_range_desc *range_desc)
{
	range_desc->iter_status = (cmd->data.params[0] >> 42) & 0x7f;
	range_desc->base_id = lower_32_bits(cmd->data.params[1]);
	range_desc->last_id = upper_32_bits(cmd->data.params[1]);
}

/**
 * @brief	Obtains container attributes
 *
 * @param[in]	dprc_handle	DPRC object handle
 * @param[out]	attributes	Container attributes
 */

static inline void build_cmd_dprc_get_attributes(
	struct mc_command *cmd,
	uint16_t dprc_handle)
{
	cmd->header = mc_encode_cmd_header(
			DPRC_CMDID_GET_ATTR,
			DPRC_CMDSZ_GET_ATTR,
			CMDIF_PRI_LOW,
			dprc_handle);
}

static inline void parse_resp_dprc_get_attributes(
	struct mc_command *cmd,
	struct dprc_attributes *attributes)
{
	attributes->container_id = lower_32_bits(cmd->data.params[0]);
	attributes->icid = (cmd->data.params[0] >> 32) & 0xffff;
	attributes->portal_id = (cmd->data.params[0] >> 48) & 0xffff;
	attributes->options = lower_32_bits(cmd->data.params[1]);
}

/**
 * @brief	Returns region information for a specified device.
 *
 * @param[in]	dprc_handle	DPRC object handle
 * @param[in]	dev_type	Device type as returned in dprc_get_device()
 * @param[in]	dev_id		Unique device instance as returned in
 *				dprc_get_device()
 * @param[in]	region_index	The specific region to query
 * @param[out]	region_desc	Returns the requested region descriptor
 */

static inline void build_cmd_dprc_get_dev_region(
	struct mc_command *cmd,
	uint16_t dprc_handle,
	char *dev_type,
	uint16_t dev_id,
	uint8_t region_index)
{
	cmd->header = mc_encode_cmd_header(
				DPRC_CMDID_GET_DEV_REG,
				DPRC_CMDSZ_GET_DEV_REG,
				CMDIF_PRI_LOW,
				dprc_handle);

	memset(&cmd->data, 0, sizeof(cmd->data));
	cmd->data.params[0] = dev_id;
	cmd->data.params[0] |= (uint64_t)region_index << 48;
	memcpy(&cmd->data.params[3], dev_type, 8);
	memcpy(&cmd->data.params[4], &dev_type[8], 8);
}

static inline void parse_resp_dprc_get_dev_region(
	struct mc_command *cmd,
	struct dprc_region_desc *region_desc)
{
	region_desc->base_paddr = cmd->data.params[1];
	region_desc->size = lower_32_bits(cmd->data.params[2]);
}

/**
 * @brief	Sets IRQ information for the DPRC to trigger an interrupt.
 *
 * @param[in]	dprc_handle	DPRC object handle
 * @param[in]	irq_index	Identifies the interrupt index to configure
 *				DPRC supports only irq_index 0 - this interrupt
 *				will be signaled on every change to
 *				resource/device assignment in this DPRC.
 * @param[in]	irq_paddr	Physical IRQ address that must be written to
 *				signal a message-based interrupt
 * @param[in]	irq_val		Value to write into irq_paddr address
 */

static inline void build_cmd_dprc_set_irq(
	struct mc_command *cmd,
	uint16_t dprc_handle,
	uint8_t irq_index,
	uint64_t irq_paddr,
	uint32_t irq_val)
{
	cmd->header = mc_encode_cmd_header(
				DPRC_CMDID_SET_IRQ,
				DPRC_CMDSZ_SET_IRQ,
				CMDIF_PRI_LOW,
				dprc_handle);

	memset(&cmd->data, 0, sizeof(cmd->data));
	cmd->data.params[0] = (uint64_t)irq_index << 32;
	cmd->data.params[0] |= irq_val;
	cmd->data.params[1] = irq_paddr;
}

/**
 * @brief	Gets IRQ information from the DPRC.
 *
 * @param[in]	dprc_handle	DPRC object handle
 * @param[in]   irq_index	The interrupt index to configure;
 *				DPRC supports only irq_index 0 - this interrupt
 *				will be signaled on every change to
 *				resource/device assignment in this DPRC.
 * @param[out]  type		Interrupt type: 0 represents message interrupt
 *				type (both irq_paddr and irq_val are valid);
 * @param[out]	irq_paddr	Physical address that must be written in order
 *				to signal the message-based interrupt
 * @param[out]	irq_val		Value to write in order to signal the
 *				message-based interrupt
 */

static inline void build_cmd_dprc_get_irq(
	struct mc_command *cmd,
	uint16_t dprc_handle,
	uint8_t irq_index)
{
	cmd->header = mc_encode_cmd_header(
			DPRC_CMDID_GET_IRQ,
			DPRC_CMDSZ_GET_IRQ,
			CMDIF_PRI_LOW,
			dprc_handle);

	memset(&cmd->data, 0, sizeof(cmd->data));
	cmd->data.params[0] = (uint64_t)irq_index << 32;
}

static inline void parse_resp_dprc_get_irq(
	struct mc_command *cmd,
	uint32_t *type,
	uint64_t *irq_paddr,
	uint32_t *irq_val)
{
	*irq_val = lower_32_bits(cmd->data.params[0]);
	*irq_paddr = cmd->data.params[1];
	*type = lower_32_bits(cmd->data.params[2]);
}

/**
 * @brief	Sets overall interrupt state.
 *
 * Allows GPP software to control when interrupts are generated.
 * Each interrupt can have up to 32 causes.  The enable/disable control's the
 * overall interrupt state. if the interrupt is disabled no causes will cause an
 * interrupt.
 *
 * @param[in]	dprc_handle	DPRC object handle
 * @param[in]   irq_index	The interrupt index to configure
 * @param[in]	enable_state	Interrupt state - enable = 1, disable = 0.
 */

static inline void build_cmd_dprc_set_irq_enable(
	struct mc_command *cmd,
	uint16_t dprc_handle,
	uint8_t irq_index,
	uint8_t enable_state)
{
	cmd->header = mc_encode_cmd_header(
				DPRC_CMDID_SET_IRQ_ENABLE,
				DPRC_CMDSZ_SET_IRQ_ENABLE,
				CMDIF_PRI_LOW,
				dprc_handle);

	memset(&cmd->data, 0, sizeof(cmd->data));
	cmd->data.params[0] = (uint64_t)irq_index << 32;
	cmd->data.params[0] |= enable_state;
}

/**
 * @brief	Gets overall interrupt state
 *
 * @param[in]	dprc_handle	DPRC object handle
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	enable_state	Interrupt state - enable = 1, disable = 0.
 */

static inline void build_cmd_dprc_get_irq_enable(
	struct mc_command *cmd,
	uint16_t dprc_handle,
	uint8_t irq_index)
{
	cmd->header = mc_encode_cmd_header(
				DPRC_CMDID_GET_IRQ_ENABLE,
				DPRC_CMDSZ_GET_IRQ_ENABLE,
				CMDIF_PRI_LOW,
				dprc_handle);

	memset(&cmd->data, 0, sizeof(cmd->data));
	cmd->data.params[0] = (uint64_t)irq_index << 32;
}

static inline void parse_resp_dprc_get_irq_enable(
	struct mc_command *cmd,
	uint8_t *enable_state)
{
	*enable_state = cmd->data.params[0] & 0xff;
}

/**
 * @brief	Sets interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	dprc_handle	DPRC object handle
 * @param[in]   irq_index	The interrupt index to configure
 * @param[in]	mask		Event mask to trigger interrupt.
 *				each bit:
 *					0 = ignore event
 *					1 = consider event for asserting irq
 */

static inline void build_cmd_dprc_set_irq_mask(
	struct mc_command *cmd,
	uint16_t dprc_handle,
	uint8_t irq_index,
	uint32_t mask)
{
	cmd->header = mc_encode_cmd_header(
			DPRC_CMDID_SET_IRQ_MASK,
			DPRC_CMDSZ_SET_IRQ_MASK,
			CMDIF_PRI_LOW,
			dprc_handle);

	memset(&cmd->data, 0, sizeof(cmd->data));
	cmd->data.params[0] = (uint64_t)irq_index << 32;
	cmd->data.params[0] |= mask;
}

/**
 * @brief	Gets interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	dprc_handle	DPRC object handle
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	mask		Event mask to trigger interrupt
 */

static inline void build_cmd_dprc_get_irq_mask(
	struct mc_command *cmd,
	uint16_t dprc_handle,
	uint8_t irq_index)
{
	cmd->header = mc_encode_cmd_header(
				DPRC_CMDID_GET_IRQ_MASK,
				DPRC_CMDSZ_GET_IRQ_MASK,
				CMDIF_PRI_LOW,
				dprc_handle);

	memset(&cmd->data, 0, sizeof(cmd->data));
	cmd->data.params[0] = (uint64_t)irq_index << 32;
}

static inline void parse_resp_dprc_get_irq_mask(
	struct mc_command *cmd,
	uint32_t *mask)
{
	*mask = lower_32_bits(cmd->data.params[0]);
}

/**
 * @brief	Gets the current status of any pending interrupts.
 *
 * @param[in]	dprc_handle	DPRC object handle
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	 status		Interrupts status - one bit per cause
 *					0 = no interrupt pending
 *					1 = interrupt pending
 */

static inline void build_cmd_dprc_get_irq_status(
	struct mc_command *cmd,
	uint16_t dprc_handle,
	uint8_t irq_index)
{
	cmd->header = mc_encode_cmd_header(
				DPRC_CMDID_GET_IRQ_STATUS,
				DPRC_CMDSZ_GET_IRQ_STATUS,
				CMDIF_PRI_LOW,
				dprc_handle);

	memset(&cmd->data, 0, sizeof(cmd->data));
	cmd->data.params[0] = (uint64_t)irq_index << 32;
}

static inline void parse_resp_dprc_get_irq_status(
	struct mc_command *cmd,
	uint32_t *status)
{
	*status = lower_32_bits(cmd->data.params[0]);
}

/**
 * @brief	Clears a pending interrupt's status
 *
 * @param[in]	dprc_handle	DPRC object handle
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	status		Bits to clear (W1C) - one bit per cause
 *					0 = don't change
 *					1 = clear status bit
 */

static inline void build_cmd_dprc_clear_irq_status(
	struct mc_command *cmd,
	uint16_t dprc_handle,
	uint8_t irq_index,
	uint32_t status)
{
	cmd->header = mc_encode_cmd_header(
				DPRC_CMDID_CLEAR_IRQ_STATUS,
				DPRC_CMDSZ_CLEAR_IRQ_STATUS,
				CMDIF_PRI_LOW,
				dprc_handle);

	memset(&cmd->data, 0, sizeof(cmd->data));
	cmd->data.params[0] = (uint64_t)irq_index << 32;
	cmd->data.params[0] |= status;
}

#endif /* _FSL_MC_DPRC_COMMANDS_H */


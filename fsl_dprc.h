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
 *  @file    fsl_dprc.h
 *  @brief   Data Path Resource Container API
 */

#ifndef _FSL_DPRC_H
#define _FSL_DPRC_H

/*!
 * @Group grp_dprc	Data Path Resource Container API
 *
 * @brief	Contains DPRC API for managing and querying LDPAA resources
 * @{
 */

struct fsl_mc_io;

/*!
 * Set this value as the icid value in dprc_cfg structure when creating a
 * container, in case the ICID is not selected by the user and should be
 * allocated by the DPRC from the pool of ICIDs.
 */
#define DPRC_GET_ICID_FROM_POOL			(uint16_t)(~(0))
/*!
 * Set this value as the portal_id value in dprc_cfg structure when creating a
 * container, in case the portal id is not specifically selected by the
 * user and should be allocated by the DPRC from the pool of portal ids.
 */
#define DPRC_GET_PORTAL_ID_FROM_POOL	(int)(~(0))

/*!
 * @name Resource request options
 */
#define DPRC_RES_REQ_OPT_EXPLICIT		0x00000001
/*!< Explicit resource id request - The requested objects/resources
 * are explicit and sequential (in case of resources).
 * The base ID is given at res_req at base_align field */
#define DPRC_RES_REQ_OPT_ALIGNED		0x00000002
/*!< Aligned resources request - Relevant only for resources
 * request (and not objects). Indicates that resources base id should be
 * sequential and aligned to the value given at dprc_res_req base_align field*/
#define DPRC_RES_REQ_OPT_PLUGGED		0x00000004
/*!< Plugged Flag - Relevant only for object assignment request.
 * Indicates that after all objects assigned. An interrupt will be invoked at
 * the relevant GPP. The assigned object will be marked as plugged.
 * plugged objects can't be assigned from their container */
/* @} */

/*!
 * @name Container general options
 *
 * These options may be selected at container creation by the container creator
 * and can be retrieved using dprc_get_attributes()
 */
#define DPRC_CFG_OPT_SPAWN_ALLOWED		0x00000001
/*!< Spawn Policy Option allowed - Indicates that the new container is allowed
 * to spawn and have its own child containers. */
#define DPRC_CFG_OPT_ALLOC_ALLOWED		0x00000002
/*!< General Container allocation policy - Indicates that the new container is
 * allowed to allocate requested resources from its parent container; if not
 * set, the container is only allowed to use resources in its own pools; Note
 * that this is a container's global policy, but the parent container may
 * override it and set specific quota per resource type. */
#define DPRC_CFG_OPT_OBJ_CREATE_ALLOWED	0x00000004
/*!< Object initialization allowed - software context associated with this
 * container is allowed to invoke object initialization operations. */
#define DPRC_CFG_OPT_TOPOLOGY_CHANGES_ALLOWED	0x00000008
/*!< Topology change allowed - software context associated with this
 * container is allowed to invoke topology operations, such as attach/detach
 * of network objects. */
#define DPRC_CFG_OPT_IOMMU_BYPASS		0x00000010
/*!<IOMMU bypass - indicates whether objects of this container are permitted
 * to bypass the IOMMU.  */
#define DPRC_CFG_OPT_AIOP			0x00000020
/*!<AIOP -Indicates that container belongs to aiop.  */
/* @} */

/*!
 * @name Objects Attributes Flags
 */
#define DPRC_OBJ_STATE_OPEN		0x00000001
/*!< Opened state - Indicates that an object is open by at least one owner */
#define DPRC_OBJ_STATE_PLUGGED		0x00000002
/*!< Plugged state - Indicates that the object is plugged */
/* @} */

/*!
 * @name Irq
 */
#define DPRC_NUM_OF_IRQS		1
/*!< Number of dprc's IRQs */
/* @} */

/*!
 * @name Object irq events
 */
#define DPRC_IRQ_EVENT_OBJ_ASSIGNED			0x00000001
/*!< Irq event - Indicates that a new object assigned to the container */
#define DPRC_IRQ_EVENT_OBJ_UNASSIGNED		0x00000002
/*!< Irq event - Indicates that an object was unassigned from the container */
#define DPRC_IRQ_EVENT_RES_ASSIGNED			0x00000004
/*!< Irq event - Indicates that resources assigned to the container */
#define DPRC_IRQ_EVENT_RES_UNASSIGNED		0x00000008
/*!< Irq event - Indicates that resources unassigned from the container */
#define DPRC_IRQ_EVENT_CONTAINER_DESTROYED	0x00000010
/*!< Irq event - Indicates that one of the descendant containers that opened by
 * this container is destroyed */
#define DPRC_IRQ_EVENT_OBJ_DESTROYED		0x00000011
/*!< Irq event - Indicates that on one of the container's opened object is
 destroyed */
/* @} */

/**
 * @brief	Resource request descriptor, to be used in assignment or
 *		un-assignment of resources and objects.
 */
struct dprc_res_req {
	char type[16];
	/*!<
	 * Resource/object type: Represent as a NULL terminated string.
	 * This string may received by using dprc_get_pool() to get resource
	 * type and dprc_get_obj() to get object type.
	 * Note: it is not possible to assign/unassign DP_OBJ_DPRC objects
	 */
	uint32_t num;
	/*!< Number of resources */
	uint32_t options;
	/*!< Request options: combination of DPRC_RES_REQ_OPT_ options */
	int id_base_align;
	/*!<
	 * In case of explicit assignment (DPRC_RES_REQ_OPT_EXPLICIT is set at
	 * option), this field represents the required base ID for resource
	 * allocation;
	 * In case of aligned assignment (DPRC_RES_REQ_OPT_ALIGNED is set at
	 * option), this field indicates the required alignment for the
	 * resource ID(s) - use 0 if there is no alignment or explicit id
	 * requirements.
	 */
};

/**
 * @brief	Object descriptor, returned from dprc_get_obj()
 */
struct dprc_obj_desc {
	uint16_t vendor;
	/*!< Object vendor identifier */
	char type[16];
	/*!< Type of object: NULL terminated string */
	int id;
	/*!< ID of logical object resource */
	uint32_t ver_major;
	/*!< Major version number */
	uint32_t ver_minor;
	/*!< Minor version number */
	uint8_t irq_count;
	/*!< Number of interrupts supported by the object */
	uint8_t region_count;
	/*!< Number of mappable regions supported by the object */
	uint32_t state;
	/*!< Object state: combination of DPRC_OBJ_STATE_ states */
};

/**
 * @brief	Mappable region descriptor, returned by dprc_get_obj_region()
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
	int base_id;
	/*!< Base resource ID of this range */
	int last_id;
	/*!< Last resource ID of this range */
	enum dprc_iter_status iter_status;
	/*!<
	 * Iteration status - should be set to DPRC_ITER_STATUS_FIRST at
	 * first iteration; while the returned marker is DPRC_ITER_STATUS_MORE,
	 * additional iterations are needed, until the returned marker is
	 * DPRC_ITER_STATUS_LAST
	 */
};

/**
 * @brief	Container attributes, returned by dprc_get_attributes()
 */
struct dprc_attributes {
	int container_id;
	/*!< Container's ID */
	uint16_t icid;
	/*!< Container's ICID */
	int portal_id;
	/*!< Container's portal ID */
	uint64_t options;
	/*!< Container's options as set at container's creation */
	struct {
		uint32_t major; /*!< DPRC major version*/
		uint32_t minor; /*!< DPRC minor version*/
	} version;
	/*!< DPRC version */
};

/**
 * @brief	Container configuration options, used in dprc_create_container()
 */
struct dprc_cfg {
	uint16_t icid;
	/*!< Container's ICID; if set to DPRC_GET_ICID_FROM_POOL, a free ICID
	 * will be allocated by the DPRC */
	int portal_id;
	/*!< portal id; if set to DPRC_GET_PORTAL_ID_FROM_POOL, a free portal id
	 * will be allocated by the DPRC */
	uint64_t options;
	/*!< Combination of DPRC_CFG_OPT_ options */
};

/**
 * @brief	Endpoint description for link connect/disconnect operations
 */
struct dprc_endpoint {
	char type[16];
	/*!< Endpoint object type: NULL terminated string */
	int id;
	/*!< Endpoint object id */
	int interface_id;
	/*!<
	 * Interface id; should be set for endpoints with multiple interfaces
	 * (DP_OBJ_DPSW, DP_OBJ_DPDMUX); for others, always set to 0.
	 */
};

/**
 * @brief	Obtains the container id associated with a given portal.
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[out]	container_id	Requested container ID
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_container_id(struct fsl_mc_io *mc_io, int *container_id);

/**
 * @brief	Opens a DPRC object for use
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	container_id	Container ID to open
 * @param[out]	dprc_handle	Handle to the DPRC object
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Required before any operation on the object.
 */
int dprc_open(struct fsl_mc_io *mc_io, int container_id,
		 uint16_t *dprc_handle);

/**
 * @brief	Closes the DPRC object handle
 *
 * No further operations on the object are allowed after this call without
 * re-opening the object.
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_close(struct fsl_mc_io *mc_io, uint16_t dprc_handle);

/**
 * @brief	Creates a child container
 *
 * @param[in]   mc_io			Pointer to opaque I/O object
 * @param[in]	dprc_handle		Handle to the DPRC object
 * @param[in]	cfg			Child container configuration
 * @param[out]	child_container_id	Child container ID
 * @param[out]	child_portal_paddr	Base physical address of the
 *				child portal
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_create_container(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
			     struct dprc_cfg *cfg,
			     int *child_container_id,
			     uint64_t *child_portal_paddr);

/**
 * @brief	Destroys a child container.
 *
 * This function terminates the child container, so following this call the
 * child container ID becomes invalid.
 *
 * Notes:
 * - All resources and objects of the destroyed container are returned to the
 * parent container or destroyed if were created be the destroyed container.
 * - This function destroy all the child containers of the specified
 *   container prior to destroying the container itself.
 *
 * @param[in]   mc_io			Pointer to opaque I/O object
 * @param[in]	dprc_handle		Handle to the DPRC object
 * @param[in]	child_container_id	ID of the container to destroy
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Only the parent container is allowed to destroy a child policy
 *		Container 0 can't be destroyed
 */
int dprc_destroy_container(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
			   int child_container_id);

/**
 * @brief	Sets allocation policy for a specific resource/object type in a
 * child container
 *
 * Allocation policy determines whether or not a container may allocate
 * resources from its parent. Each container has a 'global' allocation policy
 * that is set when the container is created.
 *
 * This function sets allocation policy for a specific resource type.
 * The default policy for all resource types matches the container's 'global'
 * allocation policy.
 *
 * @param[in]   mc_io		    Pointer to opaque I/O object
 * @param[in]	dprc_handle	    Handle to the DPRC object
 * @param[in]	child_container_id  ID of the child container
 * @param[in]	type		    resource/object type
 * @param[in]	quota		    Sets the maximum number of resources of
 *				    the selected type that the child
 *				    container is allowed to allocate
 *				    from its parent;
 *				    when quota is set to -1, the policy is
 *				    the same as container's general policy.
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Only the parent container is allowed to change a child policy.
 */
int dprc_set_res_quota(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
		       int child_container_id,
		       char *type,
		       uint16_t quota);

/**
 * @brief	Gets the allocation policy of a specific resource/object type
 *		in a child container
 *
 * @param[in]   mc_io			Pointer to opaque I/O object
 * @param[in]	dprc_handle		Handle to the DPRC object
 * @param[in]	child_container_id	ID of the child container
 * @param[in]	type			resource/object type
 * @param[out]	quota			Holds the maximum number of resources of
 *					the selected type that the child
 *					container is allowed to allocate from
 *					the parent;
 *					when quota is set to -1, the policy is
 *					the same as container's general policy.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_res_quota(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
		       int child_container_id,
		       char *type,
		       uint16_t *quota);

/**
 * @brief	Resets a child container.
 *
 * In case a software context crashes or becomes non-responsive, the parent
 * may wish to reset its resources container before the software context is
 * restarted.
 *
 * This routine informs all objects assigned to the child container that the
 * container is being reset, so they may perform any cleanup operations that are
 * needed. All objects handles that were owned by the child container shall be
 * closed.
 *
 * Note that such request may be submitted even if the child software context
 * has not crashed, but the resulting object cleanup operations will not be
 * aware of that.
 *
 * @param[in]   mc_io			Pointer to opaque I/O object
 * @param[in]	dprc_handle		Handle to the DPRC object
 * @param[in]	child_container_id	ID of the container to reset
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_reset_container(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
			 int child_container_id);

/**
 * @brief	Assigns objects or resource to a child container.
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
 * - DPRC_RES_REQ_OPT_PLUGGED: Relevant only for object assignment,
 *   and indicates that the object must be set to the plugged state.
 *
 * A container may use this function with its own ID in order to change a
 * object state to plugged or unplugged.
 *
 * If IRQ information has been set in the child DPRC, it will signal an
 * interrupt following every change in its object assignment.
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 * @param[in]	container_id	ID of the child container
 * @param[in]	res_req		Describes the type and amount of resources to
 *				assign to the given container.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_assign(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
		int container_id,
		struct dprc_res_req *res_req);

/**
 * @brief	Un-assigns objects or resources from a child container
 *		and moves them into this (parent) DPRC.
 *
 * Un-assignment of objects can succeed only if the object is not in the
 * plugged or opened state.
 *
 * @param[in]   mc_io			Pointer to opaque I/O object
 * @param[in]	dprc_handle		Handle to the DPRC object
 * @param[in]	child_container_id	ID of the child container
 * @param[in]	res_req			Describes the type and amount of
 *					resources to un-assign from the child
 *					container
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_unassign(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
		  int child_container_id,
		  struct dprc_res_req *res_req);

/**
 * @brief	Get the number of dprc's pools
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 * @param[out]  pool_count	Number of resource pools in the dprc.
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dprc_get_pool_count(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
			int *pool_count);

/**
 * @brief	Get the type (string) of a certain dprc's pool
 *
 * The pool types retrieved one by one by incrementing
 * pool_index up to (not including) the value of pool_count returned
 * from dprc_get_pool_count(). dprc_get_pool_count() must
 * be called prior to dprc_get_pool().
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 * @param[in]   pool_index	Index of the pool to be queried (< pool_count)
 * @param[out]  type		The type of the pool.
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dprc_get_pool(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
		  int pool_index, char *type);

/**
 * @brief	Obtains the number of objects in the DPRC
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 * @param[out]	obj_count	Number of objects assigned to the DPRC
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_obj_count(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
		       int *obj_count);

/**
 * @brief	Obtains general information on an object
 *
 * The object descriptors are retrieved one by one by incrementing
 * obj_index up to (not including) the value of obj_count returned
 * from dprc_get_obj_count(). dprc_get_obj_count() must
 * be called prior to dprc_get_obj().
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 * @param[in]	obj_index	Index of the object to be queried (< obj_count)
 * @param[out]	obj_desc	Returns the requested object descriptor
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_obj(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
		 int obj_index,
		 struct dprc_obj_desc *obj_desc);

/**
 * @brief	Obtains the number of free resources that are assigned
 *		to this container, by pool type
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 * @param[in]	type		pool type
 * @param[out]	res_count	Number of free resources of the given
 *				resource type that are assigned to this DPRC
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_res_count(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
		       char *type, int *res_count);

/**
 * @brief	Obtains IDs of free resources in the container
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 * @param[in]	type		pool type
 * @param[in,out] range_desc	range descriptor
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_res_ids(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
		     char *type,
		     struct dprc_res_ids_range_desc *range_desc);

/**
 * @brief	Obtains the physical address of MC portals
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 * @param[in]	portal_id	MC portal id
 * @param[out]  portal_addr	The physical address of the MC portal id
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_portal_paddr(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
			  int portal_id, uint64_t *portal_addr);

/**
 * @brief	Obtains container attributes
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 * @param[out]	attributes	Container attributes
 *
 * @returns     '0' on Success; Error code otherwise.
 */
int dprc_get_attributes(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
			struct dprc_attributes *attributes);

/**
 * @brief	Returns region information for a specified object.
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 * @param[in]	obj_type	Object type as returned in dprc_get_obj()
 * @param[in]	obj_id		Unique object instance as returned in
 *				dprc_get_obj()
 * @param[in]	region_index	The specific region to query
 * @param[out]	region_desc	Returns the requested region descriptor
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_obj_region(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
			char *obj_type,
			   int obj_id,
			   uint8_t region_index,
			   struct dprc_region_desc *region_desc);

/**
 * @brief	Sets IRQ information for the DPRC to trigger an interrupt.
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 * @param[in]	irq_index	Identifies the interrupt index to configure
 *				DPRC supports only irq_index 0 - this interrupt
 *				will be signaled on every change to
 *				resource/object assignment in this DPRC.
 * @param[in]	irq_paddr	Physical IRQ address that must be written to
 *				signal a message-based interrupt
 * @param[in]	irq_val		Value to write into irq_paddr address
 * @param[in]	user_irq_id	A user defined number associated with this IRQ;
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_set_irq(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
		 uint8_t irq_index,
		 uint64_t irq_paddr,
		 uint32_t irq_val,
		 int user_irq_id);

/**
 * @brief	Gets IRQ information from the DPRC.
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 * @param[in]   irq_index	The interrupt index to configure;
 *				DPRC supports only irq_index 0 - this interrupt
 *				will be signaled on every change to
 *				resource/object assignment in this DPRC.
 * @param[out]  type		Interrupt type: 0 represents message interrupt
 *				type (both irq_paddr and irq_val are valid);
 * @param[out]	irq_paddr	Physical address that must be written in order
 *				to signal the message-based interrupt
 * @param[out]	irq_val		Value to write in order to signal the
 *				message-based interrupt
 * @param[out]	user_irq_id	A user defined number associated with this IRQ;
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_irq(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
		 uint8_t irq_index,
		 int *type,
		 uint64_t *irq_paddr,
		 uint32_t *irq_val,
		 int *user_irq_id);

/**
 * @brief	Sets overall interrupt state.
 *
 * Allows GPP software to control when interrupts are generated.
 * Each interrupt can have up to 32 causes.  The enable/disable control's the
 * overall interrupt state. if the interrupt is disabled no causes will cause an
 * interrupt.
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[in]	enable_state	Interrupt state - enable = 1, disable = 0.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_set_irq_enable(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
			uint8_t irq_index,
			uint8_t enable_state);

/**
 * @brief	Gets overall interrupt state
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	enable_state	Interrupt state - enable = 1, disable = 0.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_irq_enable(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
			uint8_t irq_index,
			uint8_t *enable_state);

/**
 * @brief	Sets interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[in]	mask		Event mask to trigger interrupt.
 *				each bit:
 *					0 = ignore event
 *					1 = consider event for asserting irq
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_set_irq_mask(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
		      uint8_t irq_index, uint32_t mask);

/**
 * @brief	Gets interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	mask		Event mask to trigger interrupt
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_irq_mask(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
		      uint8_t irq_index, uint32_t *mask);

/**
 * @brief	Gets the current status of any pending interrupts.
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	 status		Interrupts status - one bit per cause
 *					0 = no interrupt pending
 *					1 = interrupt pending
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dprc_get_irq_status(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
			uint8_t irq_index, uint32_t *status);

/**
 * @brief	Clears a pending interrupt's status
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 * @param[in]   irq_index	The interrupt index to configure
 * @param[out]	status		Bits to clear (W1C) - one bit per cause
 *					0 = don't change
 *					1 = clear status bit
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dprc_clear_irq_status(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
			  uint8_t irq_index, uint32_t status);

/**
 * @brief	Connects two endpoints to create a network link between them
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 * @param[in]   endpoint1	Endpoint 1 configuration parameters.
 * @param[in]	endpoint2	Endpoint 2 configuration parameters.
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dprc_connect(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
		 struct dprc_endpoint *endpoint1,
		 struct dprc_endpoint *endpoint2);

/**
 * @brief	Disconnects one endpoint to remove its network link
 *
 * @param[in]   mc_io		Pointer to opaque I/O object
 * @param[in]	dprc_handle	Handle to the DPRC object
 * @param[in]   endpoint	Endpoint configuration parameters.
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dprc_disconnect(struct fsl_mc_io *mc_io, uint16_t dprc_handle,
		    struct dprc_endpoint *endpoint);

/*! @} */

#endif /* _FSL_DPRC_H */


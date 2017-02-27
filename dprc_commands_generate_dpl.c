/* Copyright 2014-2017 Freescale Semiconductor Inc.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <getopt.h>
#include <ctype.h>
#include "restool.h"
#include "utils.h"
#include "dprc_commands_generate_dpl.h"
#include "mc_v9/fsl_dpaiop.h"
#include "mc_v9/fsl_dpbp.h"
#include "mc_v9/fsl_dpci.h"
#include "mc_v9/fsl_dpcon.h"
#include "mc_v9/fsl_dpdbg.h"
#include "mc_v9/fsl_dpdcei.h"
#include "mc_v9/fsl_dpdmai.h"
#include "mc_v9/fsl_dpdmux.h"
#include "mc_v9/fsl_dpio.h"
#include "mc_v9/fsl_dpmac.h"
#include "mc_v9/fsl_dpmcp.h"
#include "mc_v9/fsl_dprc.h"
#include "mc_v9/fsl_dprtc.h"
#include "mc_v9/fsl_dpseci.h"
#include "mc_v9/fsl_dpni.h"
#include "mc_v9/fsl_dpsw.h"
#include "mc_v10/fsl_dpni.h"

/* dprc stuff */
#define ALL_DPRC_OPTS_DPL (                     \
	DPRC_CFG_OPT_SPAWN_ALLOWED |            \
	DPRC_CFG_OPT_ALLOC_ALLOWED |            \
	DPRC_CFG_OPT_OBJ_CREATE_ALLOWED |       \
	DPRC_CFG_OPT_TOPOLOGY_CHANGES_ALLOWED | \
	DPRC_CFG_OPT_AIOP |                     \
	DPRC_CFG_OPT_IRQ_CFG_ALLOWED)

/* dpni stuff */
/**
 * max_dist: Maximum distribution size for Rx traffic class;
 * supported values: 1,2,3,4,6,7,8,12,14,16,24,28,32,48,56,64,96,
 * 112,128,192,224,256,384,448,512,768,896,1024;
 */
#define MAX_DIST_SIZE	1024
#define ALL_DPNI_OPTS (					\
	DPNI_OPT_ALLOW_DIST_KEY_PER_TC |		\
	DPNI_OPT_TX_CONF_DISABLED |			\
	DPNI_OPT_PRIVATE_TX_CONF_ERROR_DISABLED |	\
	DPNI_OPT_DIST_HASH |				\
	DPNI_OPT_DIST_FS |				\
	DPNI_OPT_UNICAST_FILTER	|			\
	DPNI_OPT_MULTICAST_FILTER |			\
	DPNI_OPT_VLAN_FILTER |				\
	DPNI_OPT_IPR |					\
	DPNI_OPT_IPF |					\
	DPNI_OPT_VLAN_MANIPULATION |			\
	DPNI_OPT_QOS_MASK_SUPPORT |			\
	DPNI_OPT_TX_FRM_RELEASE |			\
	DPNI_OPT_NO_MAC_FILTER |			\
	DPNI_OPT_HAS_POLICING |				\
	DPNI_OPT_SHARED_CONGESTION |			\
	DPNI_OPT_HAS_KEY_MASKING |			\
	DPNI_OPT_NO_FS |				\
	DPNI_OPT_FS_MASK_SUPPORT)

struct dpni_config {
	struct dpni_extended_cfg dpni_extended_cfg;
	struct dpni_cfg dpni_cfg;
	struct dpni_attr dpni_attr;
	uint16_t dpni_handle;
};

#define RESTOOL_DPNI_CREATE \
	_IOWR(RESTOOL_IOCTL_TYPE, 0x5, struct dpni_config)

#define RESTOOL_DPNI_GET_ATTRIBUTES \
	_IOWR(RESTOOL_IOCTL_TYPE, 0x6, struct dpni_config)

/* dpdmux stuff */
#define ALL_DPDMUX_OPTS		DPDMUX_OPT_BRIDGE_EN

/* dpsw stuff */
#define ALL_DPSW_OPTS (			\
	DPSW_OPT_FLOODING_DIS |		\
	DPSW_OPT_MULTICAST_DIS |	\
	DPSW_OPT_CTRL_IF_DIS |	\
	DPSW_OPT_FLOODING_METERING_DIS |	\
	DPSW_OPT_METERING_EN)


/* dpl stuff */
#define RESTOOL_DYNAMIC_DPL "./dynamic-dpl.dts"

/**
 * struct obj_list - linked list node of all objects
 * @next: tracks next objects, the objects are sorted
 * @type: object type
 * @id: object id
 * @label: object label
 */
struct obj_list {
	struct obj_list *next;
	char type[16];
	int id;
	char label[16];
};

/**
 * struct conn_list - linked list node of 2 connected endpoints
 * @next: tracks next connection, the connections are not sorted.
 * @type1: endpoint1's object type
 * @type2: endpoint2's object type
 * @id1: endpoint1's id
 * @id2: endpoint2's id
 * @if_id1: endpoint1's interface id, initialized as -1 if no interface
 * @if_id2: endpoint2's interface id, initialized as -1 if no interface
 */
struct conn_list {
	struct conn_list *next;
	char type1[16];
	char type2[16];
	int id1;
	int id2;
	int if_id1;
	int if_id2;
};

/**
 * struct container_list - linked list node of all containers
 * @next: tracks next container
 * @obj_list: tracks the objects in current container, sorted list
 * @id: current container's id
 * @parent_id: current container's parent id. 0 means no parent.
 * @options: configuration options of current container
 */
struct container_list {
	struct container_list *next;
	struct obj_list *obj;
	int id;
	int parent_id;
	uint64_t options;
};

static int container_count;
static struct container_list *container_head;
static struct obj_list *obj_head;
static struct conn_list *conn_head;

enum mc_cmd_status mc_status;

/**
 * compare_insert_obj - compare the newly added object node with existing ones,
 *			insert in sorted order
 * @head: head of the object list
 * @target: the object node to be inserted
 *
 * Returns 0 on success, negative otherwise
 */
static int compare_insert_obj(struct obj_list **head, struct obj_list *target)
{
	int error;
	struct obj_list *prev;
	struct obj_list *curr;

	if (*head == NULL) {
		*head = target;
		target->next = NULL;
		return 0;
	}

	curr = prev = *head;
	while (curr) {
		error = strcmp(target->type, curr->type);

		if (error < 0) {
			if (curr == prev) {
				*head = target;
				target->next = curr;
				return 0;
			}

			prev->next = target;
			target->next = curr;
			return 0;
		}

		if (error == 0) {
			if (target->id < curr->id) {
				if (curr == prev) {
					*head = target;
					target->next = curr;
					return 0;
				}


				prev->next = target;
				target->next = curr;
				return 0;
			}

			if (target->id == curr->id) {
				ERROR_PRINTF("Two objects the same: %s.%d\n",
						curr->type, curr->id);
				return -EINVAL;
			}

			if (target->id > curr->id) {
				prev = curr;
				curr = curr->next;
			}
		}

		if (error > 0) {
			prev = curr;
			curr = curr->next;
		}
	}

	prev->next = target;
	target->next = NULL;
	return 0;
}

static int find_all_obj_desc(uint32_t dprc_id,
			     uint16_t dprc_handle,
			     int nesting_level,
			     struct container_list **prev,
			     uint32_t parent_id)
{

	int num_child_devices;
	int error = 0;
	enum mc_cmd_status mc_status;
	struct container_list *prev_cont;
	struct container_list *curr_cont;
	struct dprc_attributes dprc_attr;

	if (prev)
		prev_cont = *prev;
	curr_cont = malloc(sizeof(struct container_list));
	if (curr_cont == NULL) {
		ERROR_PRINTF("malloc failed\n");
		error = -errno;
		goto out;
	}

	assert(nesting_level <= MAX_DPRC_NESTING);
	if (parent_id == 0) {
		DEBUG_PRINTF("This is the main dprc.\n");
		container_head = curr_cont;
	} else {
		DEBUG_PRINTF("This is child dprc.\n");
		prev_cont->next = curr_cont;
	}

	curr_cont->id = dprc_id;
	curr_cont->parent_id = parent_id;
	curr_cont->obj = NULL;
	curr_cont->next = NULL;
	if (prev)
		*prev = curr_cont;
	prev_cont = curr_cont;

	memset(&dprc_attr, 0, sizeof(dprc_attr));
	error = dprc_get_attributes(&restool.mc_io, 0,
				dprc_handle, &dprc_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	curr_cont->options = dprc_attr.options;
	container_count++;
	error = dprc_get_obj_count(&restool.mc_io, 0,
				   dprc_handle,
				   &num_child_devices);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	for (int i = 0; i < num_child_devices; i++) {
		struct dprc_obj_desc obj_desc;
		uint16_t child_dprc_handle;
		int error2;

		error = dprc_get_obj(
				&restool.mc_io, 0,
				dprc_handle,
				i,
				&obj_desc);
		if (error < 0) {
			DEBUG_PRINTF(
				"dprc_get_object(%u) failed with error %d\n",
				i, error);
			goto out;
		}

		DEBUG_PRINTF("it is %s.%u\n", obj_desc.type, obj_desc.id);

		if (strcmp(obj_desc.type, "dprc") == 0) {

			error = open_dprc(obj_desc.id, &child_dprc_handle);
			if (error < 0)
				goto out;

			DEBUG_PRINTF("entering %s.%u\n", obj_desc.type,
					obj_desc.id);
			error = find_all_obj_desc(obj_desc.id,
					child_dprc_handle,
					nesting_level + 1,
					&prev_cont,
					dprc_id);
			if (prev)
				*prev = prev_cont;

			error2 = dprc_close(&restool.mc_io, 0,
						child_dprc_handle);
			if (error2 < 0) {
				mc_status = flib_error_to_mc_status(error2);
				ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status), mc_status);
				if (error == 0)
					error = error2;

				goto out;
			}

			DEBUG_PRINTF("exiting %s.%u\n", obj_desc.type,
					obj_desc.id);
		} else {
			struct obj_list *curr_obj =
				malloc(sizeof(struct obj_list));
			if (curr_obj == NULL) {
				ERROR_PRINTF("malloc failed\n");
				error = -errno;
				goto out;
			}

			curr_obj->next = NULL;
			strncpy(curr_obj->type, obj_desc.type, 16);
			curr_obj->id = obj_desc.id;
			strncpy(curr_obj->label, obj_desc.label, 16);

			struct obj_list *curr_obj2 =
				malloc(sizeof(struct obj_list));
			if (curr_obj2 == NULL) {
				ERROR_PRINTF("malloc failed\n");
				error = -errno;
				goto out;
			}

			curr_obj2->next = NULL;
			strncpy(curr_obj2->type, obj_desc.type, 16);
			curr_obj2->id = obj_desc.id;
			strncpy(curr_obj2->label, obj_desc.label, 16);

			error = compare_insert_obj(&obj_head, curr_obj);
			if (error)
				goto out;
			error = compare_insert_obj(&curr_cont->obj, curr_obj2);
			if (error)
				goto out;
		}
	}

out:
	return error;
}

static int parse_layout(uint32_t dprc_id)
{
	int error;

	bool opened = false;

	uint16_t dprc_handle;

	/* if no dprc specified, use root dprc */
	if (restool.obj_name == NULL || dprc_id == restool.root_dprc_id) {
		dprc_id = restool.root_dprc_id;
		dprc_handle = restool.root_dprc_handle;
	} else {
		error = dprc_open(&restool.mc_io, 0, dprc_id, &dprc_handle);
		if (error < 0) {
			mc_status = flib_error_to_mc_status(error);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
			goto out;
		}
		opened = true;
	}

	error = find_all_obj_desc(dprc_id, dprc_handle, 0, NULL, 0);

	if (opened == true) {
		error = dprc_close(&restool.mc_io, 0, dprc_handle);
		if (error < 0) {
			mc_status = flib_error_to_mc_status(error);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
			goto out;
		}

	}

	if (error) {
		ERROR_PRINTF("Parsing Data Path Layout failed\n");
		return error;
	}

out:
	return 0;
}

static void parse_dprc_options(FILE *fp, uint64_t options)
{
	char buf[300];
	int len;

	if ((options & ~ALL_DPRC_OPTS_DPL) != 0)
		fprintf(fp, "\t\t\t/* Unrecognized options found... */\n");


	snprintf(buf, 14, "\t\t\toptions = ");

	if (options & DPRC_CFG_OPT_SPAWN_ALLOWED) {
		len = strlen(buf);
		snprintf(buf+len, 31, "\"DPRC_CFG_OPT_SPAWN_ALLOWED\", ");
	}

	if (options & DPRC_CFG_OPT_ALLOC_ALLOWED) {
		len = strlen(buf);
		snprintf(buf+len, 31, "\"DPRC_CFG_OPT_ALLOC_ALLOWED\", ");
	}

	if (options & DPRC_CFG_OPT_OBJ_CREATE_ALLOWED) {
		len = strlen(buf);
		snprintf(buf+len, 36, "\"DPRC_CFG_OPT_OBJ_CREATE_ALLOWED\", ");
	}

	if (options & DPRC_CFG_OPT_TOPOLOGY_CHANGES_ALLOWED) {
		len = strlen(buf);
		snprintf(buf+len, 42,
			"\"DPRC_CFG_OPT_TOPOLOGY_CHANGES_ALLOWED\", ");
	}

	if (options & DPRC_CFG_OPT_AIOP) {
		len = strlen(buf);
		snprintf(buf+len, 22, "\"DPRC_CFG_OPT_AIOP\", ");
	}

	if (options & DPRC_CFG_OPT_IRQ_CFG_ALLOWED) {
		len = strlen(buf);
		snprintf(buf+len, 33, "\"DPRC_CFG_OPT_IRQ_CFG_ALLOWED\", ");
	}

	len = strlen(buf);
	if (13 == len)
		return;
	buf[len-2] = ';';
	buf[len-1] = '\0';

	fprintf(fp, "%s\n", buf);
}

static void parse_obj_label(FILE *fp, char *label)
{
	assert(strlen(label) <= MC_OBJ_LABEL_MAX_LENGTH);
	if (strlen(label) > 0)
		fprintf(fp, "\t\t\t\t\tlabel = \"%s\";\n", label);
}

static char *to_upper(char *string)
{
	int length, i;
	char *upper_string;

	length = strlen(string);
	upper_string = malloc(length * sizeof(char));
	if (!upper_string) {
		ERROR_PRINTF("Could not alloc memory!");
		return NULL;
	}

	for (i = 0; i < length; i++)
		upper_string[i] = toupper(string[i]);
	upper_string[i] = '\0';

	return upper_string;
}

static int write_obj_set(char *obj_type, int start_index, int end_index)
{
	char *obj_type_upper;
	FILE *fp = stdout;
	int i;

	obj_type_upper = to_upper(obj_type);
	if (!obj_type_upper)
		return -ENOMEM;

	fprintf(fp, "\n");
	fprintf(fp, "\t\t\t\t/* -------------- %ss --------------*/\n", obj_type_upper);
	fprintf(fp, "\t\t\t\tobj_set@%s {\n", obj_type);
	fprintf(fp, "\t\t\t\t\ttype = \"%s\";\n", obj_type);
	fprintf(fp, "\t\t\t\t\tids = <");

	for (i = start_index; i <= end_index; i++)
		fprintf(fp, "%d ", i);

	fprintf(fp, ">;\n");
	fprintf(fp, "\t\t\t\t};\n");

	free(obj_type_upper);
	return 0;
}

static int write_containers(void)
{
	struct container_list *curr_cont;
	struct obj_list *curr_obj;
	struct obj_list *prev_obj;
	char curr_obj_type[OBJ_TYPE_MAX_LENGTH];
	int remain, obj_set_start, error;
	int curr_obj_id;
	int obj_num = 99;
	int base = 100;
	FILE *fp = stdout;

	fprintf(fp,
		"\t/*****************************************************************\n");
	fprintf(fp, "\t * Containers\n");
	fprintf(fp,
		"\t *****************************************************************/\n");

	fprintf(fp, "\tcontainers {\n");

	curr_cont = container_head;
	while (curr_cont) {
		obj_num = 99;
		prev_obj = NULL;
		curr_obj = curr_cont->obj;
		memset(curr_obj_type, 0, OBJ_TYPE_MAX_LENGTH);

		fprintf(fp, "\n");
		fprintf(fp, "\t\tdprc@%d {\n", curr_cont->id);
		fprintf(fp, "\t\t\tcompatible = \"fsl,dprc\";\n");
		if (curr_cont->parent_id == 0)
			fprintf(fp, "\t\t\tparent = \"none\";\n");
		else
			fprintf(fp, "\t\t\tparent = \"dprc@%d\";\n",
				curr_cont->parent_id);
		parse_dprc_options(fp, curr_cont->options);


		fprintf(fp, "\n");
		fprintf(fp, "\t\t\tobjects {\n");

		while (curr_obj) {
			if (strcmp(curr_obj->type, "dpmcp") == 0 &&
			    0 == curr_obj->id) {
				curr_obj = curr_obj->next;
				continue;
			}
			if (prev_obj == NULL ||
			    strcmp(curr_obj->type, prev_obj->type) > 0) {
				remain = obj_num % base;
				obj_num = obj_num + base - remain;
			}

			if (restool.mc_fw_version.major <= MC_FW_VERSION_9) {
				fprintf(fp, "\n");
				fprintf(fp, "\t\t\t\tobj@%d {\n", obj_num);
				fprintf(fp, "\t\t\t\t\tobj_name = \"%s@%d\";\n",
					curr_obj->type, curr_obj->id);
				parse_obj_label(fp, curr_obj->label);
				fprintf(fp, "\t\t\t\t};\n");
			} else if (restool.mc_fw_version.major == MC_FW_VERSION_10) {
				if (curr_obj_type[0] == '\0') {
					memcpy(curr_obj_type, curr_obj->type, OBJ_TYPE_MAX_LENGTH);
					curr_obj_id = curr_obj->id;
					obj_set_start = curr_obj->id;
				} else if (strcmp(curr_obj_type, curr_obj->type)) {
					error = write_obj_set(curr_obj_type, obj_set_start, curr_obj_id);
					if (error) {
						ERROR_PRINTF("write_obj_set() failed with error = %d\n", error);
						return error;
					}

					obj_set_start = curr_obj->id;
					memcpy(curr_obj_type, curr_obj->type, OBJ_TYPE_MAX_LENGTH);
					curr_obj_id = curr_obj->id;
				} else {
					curr_obj_id = curr_obj->id;
				}
			}

			obj_num++;
			prev_obj = curr_obj;
			curr_obj = curr_obj->next;
		}

		error = write_obj_set(curr_obj_type, obj_set_start, curr_obj_id);
		if (error) {
			ERROR_PRINTF("write_obj_set() failed with error = %d\n", error);
			return error;
		}

		fprintf(fp, "\t\t\t};\n");
		fprintf(fp, "\t\t};\n");
		curr_cont = curr_cont->next;
	}

	fprintf(fp, "\t};\n");

	return 0;
}

/**
 * compare_insert_connection - Just insert target into conn_list.
 *				The conn_list is unsorted.
 *				Algorithm is used to avoid adding duplication
 *				connection node from 2 objects
 * @head: conn_list head
 * @target: the one to be inserted
 *
 * Return 0 on success, negative otherwise
 */
static int compare_insert_connection(struct conn_list **head,
				     struct conn_list *target)
{
	struct conn_list *prev;
	struct conn_list *curr;

	if (*head == NULL) {
		*head = target;
		target->next = NULL;
		return 0;
	}

	curr = prev = *head;
	while (curr) {
		if (strcmp(target->type1, curr->type1) == 0 &&
		    target->id1 == curr->id1 &&
		    target->if_id1 == curr->if_id1) {

			if (strcmp(target->type2, curr->type2) == 0 &&
			    target->id2 == curr->id2 &&
			    target->if_id2 == curr->if_id2)
				return 0;

			return -EINVAL;
		}

		if (strcmp(target->type1, curr->type2) == 0 &&
		    target->id1 == curr->id2 &&
		    target->if_id1 == curr->if_id2) {

			if (strcmp(target->type2, curr->type1) == 0 &&
			    target->id2 == curr->id1 &&
			    target->if_id2 == curr->if_id1)
				return 0;

			return -EINVAL;
		}

		prev = curr;
		curr = curr->next;
	}

	prev->next = target;
	target->next = NULL;

	return 0;
}

/* objects don't Need to be parse and get attributes for now */
static int parse_dpbp(FILE *fp, struct obj_list *curr)
{
	(void)fp;
	(void)curr;
	return 0;
}

static int parse_dpdbg(FILE *fp, struct obj_list *curr)
{
	(void)fp;
	(void)curr;
	return 0;
}

static int parse_dpmcp(FILE *fp, struct obj_list *curr)
{
	(void)fp;
	(void)curr;
	return 0;
}

static int parse_dprc(FILE *fp, struct obj_list *curr)
{
	(void)fp;
	(void)curr;
	return 0;
}

static int parse_dprtc(FILE *fp, struct obj_list *curr)
{
	(void)fp;
	(void)curr;
	return 0;
}

/* objects Need to be parsed and get attributes*/
static int parse_dpaiop(FILE *fp, struct obj_list *curr)
{
	/* dpaiop_attr{} does not have field called aiop_container_id */
	(void)fp;
	(void)curr;
	return 0;
}

static int parse_dpcon(FILE *fp, struct obj_list *curr)
{
	uint16_t dpcon_handle;
	int error;
	struct dpcon_attr dpcon_attr;
	bool dpcon_opened = false;

	error = dpcon_open(&restool.mc_io, 0, curr->id, &dpcon_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpcon_opened = true;
	if (0 == dpcon_handle) {
		DEBUG_PRINTF(
			"dpcon_open() returned invalid handle (auth 0) for dpcon.%u\n",
			curr->id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpcon_attr, 0, sizeof(dpcon_attr));
	error = dpcon_get_attributes(&restool.mc_io, 0, dpcon_handle,
					&dpcon_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(curr->id == dpcon_attr.id);

	fprintf(fp, "\t\t\tnum_priorities = <%#x>;\n",
		dpcon_attr.num_priorities);

	error = 0;

out:
	if (dpcon_opened) {
		int error2;

		error2 = dpcon_close(&restool.mc_io, 0, dpcon_handle);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int parse_dpdcei(FILE *fp, struct obj_list *curr)
{
	/* dpdcei_attr{} does not have a field called priority */
	uint16_t dpdcei_handle;
	int error;
	struct dpdcei_attr dpdcei_attr;
	bool dpdcei_opened = false;

	error = dpdcei_open(&restool.mc_io, 0, curr->id, &dpdcei_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpdcei_opened = true;
	if (0 == dpdcei_handle) {
		DEBUG_PRINTF(
			"dpdcei_open() returned invalid handle (auth 0) for dpdcei.%u\n",
			curr->id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpdcei_attr, 0, sizeof(dpdcei_attr));
	error = dpdcei_get_attributes(&restool.mc_io, 0, dpdcei_handle,
					&dpdcei_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(curr->id == dpdcei_attr.id);

	switch (dpdcei_attr.engine) {
	case DPDCEI_ENGINE_COMPRESSION:
		fprintf(fp, "\t\t\tengine = \"DPDCEI_ENGINE_COMPRESSION\";\n");
		break;
	case DPDCEI_ENGINE_DECOMPRESSION:
		fprintf(fp, "\t\t\tengine = \"DPDCEI_ENGINE_DECOMPRESSION\";\n");
		break;
	default:
		assert(false);
		break;
	}

	error = 0;

out:
	if (dpdcei_opened) {
		int error2;

		error2 = dpdcei_close(&restool.mc_io, 0, dpdcei_handle);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int parse_dpdmai(FILE *fp, struct obj_list *curr)
{
	uint16_t dpdmai_handle;
	int error;
	struct dpdmai_attr dpdmai_attr;
	bool dpdmai_opened = false;

	error = dpdmai_open(&restool.mc_io, 0, curr->id, &dpdmai_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpdmai_opened = true;
	if (0 == dpdmai_handle) {
		DEBUG_PRINTF(
			"dpdmai_open() returned invalid handle (auth 0) for dpdmai.%u\n",
			curr->id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpdmai_attr, 0, sizeof(dpdmai_attr));
	error = dpdmai_get_attributes(&restool.mc_io, 0, dpdmai_handle,
					&dpdmai_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(curr->id == dpdmai_attr.id);

	fprintf(fp, "\t\t\tpriorities = <%#x>;\n",
		dpdmai_attr.num_of_priorities);

	error = 0;

out:
	if (dpdmai_opened) {
		int error2;

		error2 = dpdmai_close(&restool.mc_io, 0, dpdmai_handle);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int parse_dpio(FILE *fp, struct obj_list *curr)
{
	uint16_t dpio_handle;
	int error;
	struct dpio_attr dpio_attr;
	bool dpio_opened = false;

	error = dpio_open(&restool.mc_io, 0, curr->id, &dpio_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpio_opened = true;
	if (0 == dpio_handle) {
		DEBUG_PRINTF(
			"dpio_open() returned invalid handle (auth 0) for dpio.%u\n",
			curr->id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpio_attr, 0, sizeof(dpio_attr));
	error = dpio_get_attributes(&restool.mc_io, 0, dpio_handle, &dpio_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(curr->id == dpio_attr.id);

	fprintf(fp, "\t\t\tchannel_mode = ");
	dpio_attr.channel_mode == 0 ? fprintf(fp, "\"DPIO_NO_CHANNEL\";\n") :
	dpio_attr.channel_mode == 1 ? fprintf(fp, "\"DPIO_LOCAL_CHANNEL\";\n") :
	fprintf(fp, "\"wrong mode\"\n");
	fprintf(fp, "\t\t\tnum_priorities = <%#x>;\n",
	       (unsigned int)dpio_attr.num_priorities);

	error = 0;

out:
	if (dpio_opened) {
		int error2;

		error2 = dpio_close(&restool.mc_io, 0, dpio_handle);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int parse_dpseci(FILE *fp, struct obj_list *curr)
{
	int error;
	uint16_t dpseci_handle;
	bool dpseci_opened = false;
	struct dpseci_attr dpseci_attr;
	struct dpseci_tx_queue_attr tx_attr;
	char *priorities;

	error = dpseci_open(&restool.mc_io, 0, curr->id, &dpseci_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpseci_opened = true;
	if (0 == dpseci_handle) {
		DEBUG_PRINTF(
			"dpseci_open() returned invalid handle (auth 0) for dpseci.%u\n",
			curr->id);
		error = -ENOENT;
		goto out;
	}
	memset(&tx_attr, 0, sizeof(tx_attr));
	memset(&dpseci_attr, 0, sizeof(dpseci_attr));

	error = dpseci_get_attributes(&restool.mc_io, 0, dpseci_handle,
					&dpseci_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	priorities = malloc(dpseci_attr.num_tx_queues * sizeof(*priorities));
	if (priorities == NULL) {
		ERROR_PRINTF("malloc failed\n");
		error = -errno;
		goto out;
	}

	for (int i = 0; i < dpseci_attr.num_tx_queues; i++) {
		error = dpseci_get_tx_queue(&restool.mc_io, 0, dpseci_handle,
					    i, &tx_attr);

		if (error < 0) {
			mc_status = flib_error_to_mc_status(error);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
			free(priorities);
			goto out;
		}

		priorities[i] = tx_attr.priority;
	}
	fprintf(fp, "\t\t\tpriorities = <");
	for (int i = 0; i < dpseci_attr.num_tx_queues-1; i++)
		fprintf(fp, "%d ", priorities[i]);

	fprintf(fp, "%d>;\n", priorities[dpseci_attr.num_tx_queues-1]);

	free(priorities);


out:
	if (dpseci_opened) {
		int error2;

		error2 = dpseci_close(&restool.mc_io, 0, dpseci_handle);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status), mc_status);
			if (error == 0)
				error = error2;
		}
	}
	return 0;
}

/* following objects have possible connections*/
static int parse_dpci(FILE *fp, struct obj_list *curr)
{
	uint16_t dpci_handle;
	int error;
	struct dpci_attr dpci_attr;
	struct dpci_peer_attr dpci_peer_attr;
	bool dpci_opened = false;
	struct conn_list *curr_conn;


	error = dpci_open(&restool.mc_io, 0, curr->id, &dpci_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpci_opened = true;
	if (0 == dpci_handle) {
		DEBUG_PRINTF(
			"dpci_open() returned invalid handle (auth 0) for dpci.%u\n",
			curr->id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpci_attr, 0, sizeof(dpci_attr));
	error = dpci_get_attributes(&restool.mc_io, 0, dpci_handle, &dpci_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(curr->id == dpci_attr.id);

	error = dpci_get_peer_attributes(&restool.mc_io, 0, dpci_handle,
					 &dpci_peer_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	fprintf(fp, "\t\t\tnum_of_priorities = <%#x>;\n",
	       (unsigned int)dpci_attr.num_of_priorities);

	if (-1 == dpci_peer_attr.peer_id) {
		DEBUG_PRINTF("no peer\n");
	} else {
		/* dpci has connection */
		curr_conn = malloc(sizeof(struct conn_list));
		if (curr_conn == NULL) {
			ERROR_PRINTF("malloc failed\n");
			error = -errno;
			goto out;
		}
		curr_conn->next = NULL;
		strcpy(curr_conn->type1, "dpci");
		strcpy(curr_conn->type2, "dpci");
		curr_conn->id1 = dpci_attr.id;
		curr_conn->id2 = dpci_peer_attr.peer_id;
		curr_conn->if_id1 = -1;	/* -1 means no interface */
		curr_conn->if_id2 = -1;

		error = compare_insert_connection(&conn_head, curr_conn);
		if (error)
			goto out;
	}

	error = 0;

out:
	if (dpci_opened) {
		int error2;

		error2 = dpci_close(&restool.mc_io, 0, dpci_handle);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int parse_dpmac(FILE *fp, struct obj_list *curr)
{
	/* don't have anything in the dpl-example.dts */
	(void)fp;
	(void)curr;
	return 0;
}

static void parse_dpni_options(FILE *fp, uint32_t options)
{
	char buf[500];
	int len;

	if ((options & ~ALL_DPNI_OPTS) != 0)
		fprintf(fp, "\t\t\t/* Unrecognized options found... */\n");

	snprintf(buf, 14, "\t\t\toptions = ");

	if (options & DPNI_OPT_ALLOW_DIST_KEY_PER_TC) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPNI_OPT_ALLOW_DIST_KEY_PER_TC\", ");
	}

	if (options & DPNI_OPT_TX_CONF_DISABLED) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPNI_OPT_TX_CONF_DISABLED\", ");
	}

	if (options & DPNI_OPT_PRIVATE_TX_CONF_ERROR_DISABLED) {
		len = strlen(buf);
		snprintf(buf+len, 50,
			"\"DPNI_OPT_PRIVATE_TX_CONF_ERROR_DISABLED\", ");
	}

	if (options & DPNI_OPT_DIST_HASH) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPNI_OPT_DIST_HASH\", ");
	}

	if (options & DPNI_OPT_DIST_FS) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPNI_OPT_DIST_FS\", ");
	}

	if (options & DPNI_OPT_UNICAST_FILTER) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPNI_OPT_UNICAST_FILTER\", ");
	}

	if (options & DPNI_OPT_MULTICAST_FILTER) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPNI_OPT_MULTICAST_FILTER\", ");
	}

	if (options & DPNI_OPT_VLAN_FILTER) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPNI_OPT_VLAN_FILTER\", ");
	}

	if (options & DPNI_OPT_IPR) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPNI_OPT_IPR\", ");
	}

	if (options & DPNI_OPT_IPF) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPNI_OPT_IPF\", ");
	}

	if (options & DPNI_OPT_VLAN_MANIPULATION) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPNI_OPT_VLAN_MANIPULATION\", ");
	}

	if (options & DPNI_OPT_QOS_MASK_SUPPORT) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPNI_OPT_QOS_MASK_SUPPORT\", ");
	}

	if (options & DPNI_OPT_FS_MASK_SUPPORT) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPNI_OPT_FS_MASK_SUPPORT\", ");
	}

	len = strlen(buf);
	if (13 == len)
		return;

	buf[len-2] = ';';
	buf[len-1] = '\0';

	fprintf(fp, "%s\n", buf);
}

static void parse_dpni_options_v10(FILE *fp, uint32_t options)
{
	char buf[500];
	int len;

	if ((options & ~ALL_DPNI_OPTS) != 0)
		fprintf(fp, "\t\t\t/* Unrecognized options found... */\n");

	snprintf(buf, 14, "\t\t\toptions = ");

	if (options & DPNI_OPT_TX_FRM_RELEASE) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPNI_OPT_TX_FRM_RELEASE\", ");
	}

	if (options & DPNI_OPT_HAS_POLICING) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPNI_OPT_HAS_POLICING\", ");
	}
	if (options & DPNI_OPT_SHARED_CONGESTION) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPNI_OPT_SHARED_CONGESTION\", ");
	}

	if (options & DPNI_OPT_HAS_KEY_MASKING) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPNI_OPT_HAS_KEY_MASKING\", ");
	}

	if (options & DPNI_OPT_NO_FS) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPNI_OPT_NO_FS\", ");
	}

	len = strlen(buf);
	if (13 == len)
		return;

	buf[len-2] = ';';
	buf[len-1] = '\0';

	fprintf(fp, "%s\n", buf);
}

static int parse_endpoint_dpl(struct obj_list *curr_obj, uint16_t num_ifs)
{
	struct dprc_endpoint endpoint1;
	struct dprc_endpoint endpoint2;
	int state;
	int error = 0;
	int k;
	struct conn_list *curr_conn;

	/* dpni though not have interfaces,
	 * need to have num_ifs > 0,
	 * say 1000 to start the first loop,
	 * and exit at the end of first loop
	 */
	for (k = 0; k < num_ifs; ++k) {
		memset(&endpoint1, 0, sizeof(struct dprc_endpoint));
		memset(&endpoint2, 0, sizeof(struct dprc_endpoint));
		strncpy(endpoint1.type, curr_obj->type, EP_OBJ_TYPE_MAX_LEN);
		endpoint1.type[EP_OBJ_TYPE_MAX_LEN] = '\0';
		endpoint1.id = curr_obj->id;
		endpoint1.if_id = k;

		error = dprc_get_connection(&restool.mc_io, 0,
					restool.root_dprc_handle,
					&endpoint1,
					&endpoint2,
					&state);
		DEBUG_PRINTF("endpoint state: %d\n", state);

		if (error == 0 && state == -1) {
			DEBUG_PRINTF("\tinterface %d: No object associated\n",
					k);
		} else if (error == 0) {
			if (strcmp(endpoint2.type, "dpsw") == 0 ||
			    strcmp(endpoint2.type, "dpdmux") == 0) {
				DEBUG_PRINTF("\tinterface %d: %s.%d.%d",
					k, endpoint2.type, endpoint2.id,
					endpoint2.if_id);

				curr_conn = malloc(sizeof(struct conn_list));
				if (curr_conn == NULL) {
					ERROR_PRINTF("malloc failed\n");
					error = -errno;
					return error;
				}
				curr_conn->next = NULL;
				strncpy(curr_conn->type1, endpoint1.type,
					EP_OBJ_TYPE_MAX_LEN);
				strncpy(curr_conn->type2, endpoint2.type,
					EP_OBJ_TYPE_MAX_LEN);
				curr_conn->type1[EP_OBJ_TYPE_MAX_LEN] = '\0';
				curr_conn->type2[EP_OBJ_TYPE_MAX_LEN] = '\0';
				curr_conn->id1 = endpoint1.id;
				curr_conn->id2 = endpoint2.id;
				if (strcmp(curr_obj->type, "dpni") == 0)
					curr_conn->if_id1 = -1;
					/* -1 means no interface */
				else
					curr_conn->if_id1 = endpoint1.if_id;

				curr_conn->if_id2 = endpoint2.if_id;

				error = compare_insert_connection(&conn_head,
								  curr_conn);
				if (error)
					return error;
			} else if (endpoint2.if_id == 0) {
				DEBUG_PRINTF("\tinterface %d: %s.%d",
					k, endpoint2.type, endpoint2.id);

				curr_conn = malloc(sizeof(struct conn_list));
				if (curr_conn == NULL) {
					ERROR_PRINTF("malloc failed\n");
					error = -errno;
					return error;
				}
				curr_conn->next = NULL;
				strncpy(curr_conn->type1, endpoint1.type,
					EP_OBJ_TYPE_MAX_LEN);
				strncpy(curr_conn->type2, endpoint2.type,
					EP_OBJ_TYPE_MAX_LEN);
				curr_conn->type1[EP_OBJ_TYPE_MAX_LEN] = '\0';
				curr_conn->type2[EP_OBJ_TYPE_MAX_LEN] = '\0';
				curr_conn->id1 = endpoint1.id;
				curr_conn->id2 = endpoint2.id;
				if (strcmp(curr_obj->type, "dpni") == 0)
					curr_conn->if_id1 = -1;
					/* -1 means no interface */
				else
					curr_conn->if_id1 = endpoint1.if_id;

				curr_conn->if_id2 = -1;

				error = compare_insert_connection(&conn_head,
								  curr_conn);
				if (error)
					return error;
			}

			if (state == 1)
				DEBUG_PRINTF(", link is up\n");
			else if (state == 0)
				DEBUG_PRINTF(", link is down\n");
			else
				DEBUG_PRINTF(", link is in error state\n");

		} else {
			mc_status = flib_error_to_mc_status(error);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				mc_status_to_string(mc_status), mc_status);
			return error;
		}

		if (strcmp(curr_obj->type, "dpni") == 0)
			break;
	}

	return 0;
}

static int parse_dpni_v9(FILE *fp, struct obj_list *curr)
{
	uint16_t dpni_handle;
	int error;
	struct dpni_attr_v9 dpni_attr;
	uint8_t mac_addr[6];
	bool dpni_opened = false;
	struct dpni_extended_cfg dpni_extended_cfg;

	memset(&dpni_extended_cfg, 0, sizeof(dpni_extended_cfg));
	memset(&dpni_attr, 0, sizeof(dpni_attr));

	error = dpni_open(&restool.mc_io, 0, curr->id, &dpni_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpni_opened = true;
	if (0 == dpni_handle) {
		DEBUG_PRINTF(
			"dpni_open() returned invalid handle (auth 0) for dpni.%u\n",
			curr->id);
		error = -ENOENT;
		goto out;
	}

	error = dpni_get_attributes_v9(&restool.mc_io, 0, dpni_handle,
				       &dpni_attr, &dpni_extended_cfg);

	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	assert(curr->id == dpni_attr.id);
	assert(DPNI_MAX_TC >= dpni_attr.max_tcs);

	error = dpni_get_primary_mac_addr(&restool.mc_io, 0,
					dpni_handle, mac_addr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	parse_endpoint_dpl(curr, 1000);

	fprintf(fp, "\t\t\tmac_addr = <");
	for (int j = 0; j < 5; ++j)
		fprintf(fp, "%#02x ", mac_addr[j]);
	fprintf(fp, "%#02x>;\n", mac_addr[5]);

	fprintf(fp, "\t\t\tmax_senders = <%#x>;\n",
		(uint32_t)dpni_attr.max_senders);

	parse_dpni_options(fp, dpni_attr.options);

	fprintf(fp, "\t\t\tmax_tcs = <%#x>;\n", (uint32_t)dpni_attr.max_tcs);

	fprintf(fp, "\t\t\tmax_dist_per_tc = <");
	for (int k = 0; k < dpni_attr.max_tcs - 1; ++k)
		fprintf(fp, "%#x ",
			(uint32_t)dpni_extended_cfg.tc_cfg[k].max_dist);
	fprintf(fp, "%#x>;\n",
		(uint32_t)dpni_extended_cfg.
		tc_cfg[dpni_attr.max_tcs].max_dist);

	fprintf(fp, "\t\t\tmax_fs_entries = <");
	for (int m = 0; m < dpni_attr.max_tcs - 1; ++m)
		fprintf(fp, "%#x",
			(uint32_t)dpni_extended_cfg.tc_cfg[m].max_fs_entries);
	fprintf(fp, "%#x>;\n",
		(uint32_t)dpni_extended_cfg.
		tc_cfg[dpni_attr.max_tcs].max_fs_entries);

	fprintf(fp, "\t\t\tmax_unicast_filters = <%#x>;\n",
	       (uint32_t)dpni_attr.max_unicast_filters);

	fprintf(fp, "\t\t\tmax_multicast_filters = <%#x>;\n",
	       (uint32_t)dpni_attr.max_multicast_filters);

	fprintf(fp, "\t\t\tmax_vlan_filters = <%#x>;\n",
		(uint32_t)dpni_attr.max_vlan_filters);

	fprintf(fp, "\t\t\tmax_qos_entries = <%#x>;\n",
		(uint32_t)dpni_attr.max_qos_entries);

	fprintf(fp, "\t\t\tmax_qos_key_size = <%#x>;\n",
		(uint32_t)dpni_attr.max_qos_key_size);

	fprintf(fp, "\t\t\tmax_dist_key_size = <%#x>;\n",
	       (uint32_t)dpni_attr.max_dist_key_size);

	fprintf(fp, "\t\t\tmax_policers = <%#x>;\n",
		(uint32_t)dpni_attr.max_policers);

	fprintf(fp, "\t\t\tmax_congestion_ctrl = <%#x>;\n",
		(uint32_t)dpni_attr.max_congestion_ctrl);

	fprintf(fp, "\t\t\tmax_reass_frm_size = <%#x>;\n",
		(uint32_t)dpni_extended_cfg.ipr_cfg.max_reass_frm_size);

	fprintf(fp, "\t\t\tmin_frag_size_ipv4 = <%#x>;\n",
		(uint32_t)dpni_extended_cfg.ipr_cfg.min_frag_size_ipv4);

	fprintf(fp, "\t\t\tmin_frag_size_ipv6 = <%#x>;\n",
		(uint32_t)dpni_extended_cfg.ipr_cfg.min_frag_size_ipv6);

	fprintf(fp, "\t\t\tmax_open_frames_ipv4 = <%#x>;\n",
		(uint32_t)dpni_extended_cfg.ipr_cfg.max_open_frames_ipv4);

	fprintf(fp, "\t\t\tmax_open_frames_ipv6 = <%#x>;\n",
		(uint32_t)dpni_extended_cfg.ipr_cfg.max_open_frames_ipv6);

	error = 0;

out:
	if (dpni_opened) {
		int error2;

		error2 = dpni_close(&restool.mc_io, 0, dpni_handle);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int parse_dpni_v10(FILE *fp, struct obj_list *curr)
{
	struct dpni_attr_v10 dpni_attr;
	uint16_t dpni_handle;
	bool dpni_opened = false;
	int error = 0;
	int error2;

	error = dpni_open(&restool.mc_io, 0, curr->id, &dpni_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpni_opened = true;
	if (0 == dpni_handle) {
		DEBUG_PRINTF(
			"dpni_open() returned invalid handle (auth 0) for dpni.%u\n",
			curr->id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpni_attr, 0, sizeof(dpni_attr));
	error = dpni_get_attributes_v10(&restool.mc_io, 0,
					dpni_handle, &dpni_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}

	parse_endpoint_dpl(curr, 1000);

	fprintf(fp, "\t\t\ttype = \"DPNI_TYPE_NIC\";\n");

	parse_dpni_options_v10(fp, dpni_attr.options);

	fprintf(fp, "\t\t\tnum_queues = <%u>;\n", dpni_attr.num_queues);
	fprintf(fp, "\t\t\tnum_tcs = <%u>;\n", dpni_attr.num_tcs);
	fprintf(fp, "\t\t\tmac_filter_entries = <%u>;\n", dpni_attr.mac_filter_entries);
	fprintf(fp, "\t\t\tvlan_filter_entries = <%u>;\n", dpni_attr.vlan_filter_entries);
	fprintf(fp, "\t\t\tfs_entries = <%u>;\n", dpni_attr.fs_entries);
	fprintf(fp, "\t\t\tqos_entries = <%u>;\n", dpni_attr.qos_entries);

out:
	if (dpni_opened) {

		error2 = dpni_close(&restool.mc_io, 0, dpni_handle);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static void parse_dpdmux_options(FILE *fp, uint64_t options)
{
	char buf[100];
	int len;

	if ((options & ~ALL_DPDMUX_OPTS) != 0)
		fprintf(fp, "\t\t\t/* Unrecognized options found... */\n");

	snprintf(buf, 14, "\t\t\toptions = ");

	if (options & DPDMUX_OPT_BRIDGE_EN) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPDMUX_OPT_BRIDGE_EN\", ");
	}

	len = strlen(buf);
	if (13 == len)
		return;
	buf[len-2] = ';';
	buf[len-1] = '\0';

	fprintf(fp, "%s\n", buf);
}

static void parse_dpdmux_method(FILE *fp, enum dpdmux_method method)
{
	fprintf(fp, "\t\t\tmethod = ");
	switch (method) {
	case DPDMUX_METHOD_NONE:
		fprintf(fp, "\"DPDMUX_METHOD_NONE\";\n");
		break;
	case DPDMUX_METHOD_C_VLAN_MAC:
		fprintf(fp, "\"DPDMUX_METHOD_C_VLAN_MAC\";\n");
		break;
	case DPDMUX_METHOD_MAC:
		fprintf(fp, "\"DPDMUX_METHOD_MAC\";\n");
		break;
	case DPDMUX_METHOD_C_VLAN:
		fprintf(fp, "\"DPDMUX_METHOD_C_VLAN\";\n");
		break;
#if 0 /* TODO: Enable when MC support added */
	case DPDMUX_METHOD_S_VLAN:
		fprintf(fp, "DPDMUX_METHOD_S_VLAN\n");
		break;
#endif
	default:
		assert(false);
		break;
	}
}

static void parse_dpdmux_manip(FILE *fp, enum dpdmux_manip manip)
{
	fprintf(fp, "\t\t\tmanip = ");
	switch (manip) {
	case DPDMUX_MANIP_NONE:
		fprintf(fp, "\"DPDMUX_MANIP_NONE\";\n");
		break;
#if 0 /* TODO: Enable when MC support added */
	case DPDMUX_MANIP_ADD_REMOVE_S_VLAN:
		fprintf(fp, "DPDMUX_MANIP_ADD_REMOVE_S_VLAN\n");
		break;
#endif
	default:
		assert(false);
		break;
	}
}

static int parse_dpdmux_v9(FILE *fp, struct obj_list *curr)
{
	uint16_t dpdmux_handle;
	int error;
	struct dpdmux_attr_v9 dpdmux_attr;
	bool dpdmux_opened = false;

	error = dpdmux_open(&restool.mc_io, 0, curr->id, &dpdmux_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpdmux_opened = true;
	if (0 == dpdmux_handle) {
		DEBUG_PRINTF(
			"dpdmux_open() returned invalid handle (auth 0) for dpdmux.%u\n",
			curr->id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpdmux_attr, 0, sizeof(dpdmux_attr));
	error = dpdmux_get_attributes_v9(&restool.mc_io, 0, dpdmux_handle,
					&dpdmux_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(curr->id == dpdmux_attr.id);

	parse_endpoint_dpl(curr, dpdmux_attr.num_ifs + 1);
	parse_dpdmux_options(fp, dpdmux_attr.options);
	parse_dpdmux_method(fp, dpdmux_attr.method);
	parse_dpdmux_manip(fp, dpdmux_attr.manip);
	fprintf(fp, "\t\t\tnum_ifs = <%#x>;\n",
		(uint32_t)dpdmux_attr.num_ifs + 1);

	error = 0;

out:
	if (dpdmux_opened) {
		int error2;

		error2 = dpdmux_close(&restool.mc_io, 0, dpdmux_handle);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
			if (error == 0)
				error = error2;
		}
	}

	return error;

}

static void parse_dpsw_options(FILE *fp, uint64_t options)
{
	char buf[300];
	int len;

	if ((options & ~ALL_DPSW_OPTS) != 0)
		fprintf(fp, "\t\t\t/* Unrecognized options found... */\n");

	snprintf(buf, 14, "\t\t\toptions = ");

	if (options & DPSW_OPT_FLOODING_DIS) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPSW_OPT_FLOODING_DIS\", ");
	}

	if (options & DPSW_OPT_MULTICAST_DIS) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPSW_OPT_MULTICAST_DIS\", ");
	}

	if (options & DPSW_OPT_CTRL_IF_DIS) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPSW_OPT_CTRL_IF_DIS\", ");
	}

	if (options & DPSW_OPT_FLOODING_METERING_DIS) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPSW_OPT_FLOODING_METERING_DIS\", ");
	}

	if (options & DPSW_OPT_METERING_EN) {
		len = strlen(buf);
		snprintf(buf+len, 50, "\"DPSW_OPT_METERING_EN\", ");
	}

	len = strlen(buf);
	if (13 == len)
		return;
	buf[len-2] = ';';
	buf[len-1] = '\0';

	fprintf(fp, "%s\n", buf);

}

static int parse_dpsw_v9(FILE *fp, struct obj_list *curr)
{
	uint16_t dpsw_handle;
	int error;
	struct dpsw_attr_v9 dpsw_attr;
	bool dpsw_opened = false;

	error = dpsw_open(&restool.mc_io, 0, curr->id, &dpsw_handle);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	dpsw_opened = true;
	if (0 == dpsw_handle) {
		DEBUG_PRINTF(
			"dpsw_open() returned invalid handle (auth 0) for dpsw.%u\n",
			curr->id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpsw_attr, 0, sizeof(dpsw_attr));
	error = dpsw_get_attributes_v9(&restool.mc_io, 0, dpsw_handle,
				       &dpsw_attr);
	if (error < 0) {
		mc_status = flib_error_to_mc_status(error);
		ERROR_PRINTF("MC error: %s (status %#x)\n",
			     mc_status_to_string(mc_status), mc_status);
		goto out;
	}
	assert(curr->id == dpsw_attr.id);

	parse_endpoint_dpl(curr, dpsw_attr.num_ifs);
	parse_dpsw_options(fp, dpsw_attr.options);
	fprintf(fp, "\t\t\tmax_vlans = <%#x>;\n",
		(uint32_t)dpsw_attr.max_vlans);
	fprintf(fp, "\t\t\tmax_fdbs = <%#x>;\n", (uint32_t)dpsw_attr.max_fdbs);
	/* it should be num_fdb_entries,
	 * but dpsw_attr {} call it max_fdb_entries (typo)
	 */
	fprintf(fp, "\t\t\tnum_fdb_entries = <%#x>;\n",
		(uint32_t)dpsw_attr.max_fdb_entries);
	fprintf(fp, "\t\t\tfdb_aging_time = <%#x>;\n",
		(uint32_t)dpsw_attr.fdb_aging_time);
	fprintf(fp, "\t\t\tnum_ifs = <%#x>;\n", (uint32_t)dpsw_attr.num_ifs);
	fprintf(fp, "\t\t\tmax_fdb_mc_groups = <%#x>;\n",
		(uint32_t)dpsw_attr.max_fdb_mc_groups);
	fprintf(fp, "\t\t\tmax_meters_per_if = <%#x>;\n",
		(uint32_t)dpsw_attr.max_meters_per_if);

	error = 0;

out:
	if (dpsw_opened) {
		int error2;

		error2 = dpsw_close(&restool.mc_io, 0, dpsw_handle);
		if (error2 < 0) {
			mc_status = flib_error_to_mc_status(error2);
			ERROR_PRINTF("MC error: %s (status %#x)\n",
				     mc_status_to_string(mc_status), mc_status);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int write_objects(void)
{
	struct obj_list *curr_obj;
	FILE *fp = stdout;

	fprintf(fp, "\n");
	fprintf(fp,
		"\t/*****************************************************************\n");
	fprintf(fp, "\t * Objects\n");
	fprintf(fp,
		"\t *****************************************************************/\n");


	fprintf(fp, "\tobjects {\n");
	curr_obj = obj_head;
	while (curr_obj) {
		if (strcmp(curr_obj->type, "dpmcp") == 0 && 0 == curr_obj->id) {
			curr_obj = curr_obj->next;
			continue;
		}

		fprintf(fp, "\n");
		fprintf(fp, "\t\t%s@%d {\n", curr_obj->type, curr_obj->id);
		fprintf(fp, "\t\t\tcompatible = \"fsl,%s\";\n", curr_obj->type);

		/* objects don't need to be parsed and get attributes for now */
		if (strcmp(curr_obj->type, "dpbp") == 0)
			parse_dpbp(fp, curr_obj);
		if (strcmp(curr_obj->type, "dpdbg") == 0)
			parse_dpdbg(fp, curr_obj);
		if (strcmp(curr_obj->type, "dpmcp") == 0)
			parse_dpmcp(fp, curr_obj);
		if (strcmp(curr_obj->type, "dprc") == 0)
			parse_dprc(fp, curr_obj);
		if (strcmp(curr_obj->type, "dprtc") == 0)
			parse_dprtc(fp, curr_obj);

		/* objects need to be parsed and get attributes */
		if (strcmp(curr_obj->type, "dpaiop") == 0)
			parse_dpaiop(fp, curr_obj);

		if (strcmp(curr_obj->type, "dpcon") == 0)
			parse_dpcon(fp, curr_obj);

		if (strcmp(curr_obj->type, "dpdcei") == 0)
			parse_dpdcei(fp, curr_obj);

		if (strcmp(curr_obj->type, "dpdmai") == 0)
			parse_dpdmai(fp, curr_obj);

		if (strcmp(curr_obj->type, "dpio") == 0)
			parse_dpio(fp, curr_obj);

		if (strcmp(curr_obj->type, "dpseci") == 0)
			parse_dpseci(fp, curr_obj);

		/* following objects have possible connections */
		if (strcmp(curr_obj->type, "dpci") == 0)
			parse_dpci(fp, curr_obj);

		if (strcmp(curr_obj->type, "dpmac") == 0)
			parse_dpmac(fp, curr_obj);
			/* dpmac do not need to be parsed now */

		if (strcmp(curr_obj->type, "dpni") == 0) {
			if (restool.mc_fw_version.major == 9)
				parse_dpni_v9(fp, curr_obj);
			else if (restool.mc_fw_version.major == 10)
				parse_dpni_v10(fp, curr_obj);
		}


		/* following objects have possible connections and interface*/
		if (strcmp(curr_obj->type, "dpdmux") == 0) {
			if (restool.mc_fw_version.major == 9 ||
			    restool.mc_fw_version.major == 10)
				parse_dpdmux_v9(fp, curr_obj);
		}

		if (strcmp(curr_obj->type, "dpsw") == 0) {
			if (restool.mc_fw_version.major == 9 ||
			    restool.mc_fw_version.major == 10)
				parse_dpsw_v9(fp, curr_obj);
		}

		fprintf(fp, "\t\t};\n");
		curr_obj = curr_obj->next;
	}
	fprintf(fp, "\t};\n");

	return 0;
}

static int write_connections(void)
{
	struct conn_list *curr_conn;
	int conn_num = 1;
	FILE *fp = stdout;

	fprintf(fp, "\n");
	fprintf(fp,
		"\t/*****************************************************************\n");
	fprintf(fp, "\t * Connections\n");
	fprintf(fp,
		"\t *****************************************************************/\n");

	fprintf(fp, "\tconnections {\n");
	curr_conn = conn_head;
	while (curr_conn) {
		fprintf(fp, "\n");
		fprintf(fp, "\t\tconnection@%d{\n", conn_num);
		if (curr_conn->if_id1 < 0)
			fprintf(fp, "\t\t\tendpoint1 = \"%s@%d\";\n",
				curr_conn->type1, curr_conn->id1);
		else
			fprintf(fp, "\t\t\tendpoint1 = \"%s@%d/if@%d\";\n",
				curr_conn->type1, curr_conn->id1,
				curr_conn->if_id1);
		if (curr_conn->if_id2 < 0)
			fprintf(fp, "\t\t\tendpoint2 = \"%s@%d\";\n",
				curr_conn->type2, curr_conn->id2);
		else
			fprintf(fp, "\t\t\tendpoint2 = \"%s@%d/if@%d\";\n",
				curr_conn->type2, curr_conn->id2,
				curr_conn->if_id2);

		fprintf(fp, "\t\t};\n");
		curr_conn = curr_conn->next;
		conn_num++;
	}
	fprintf(fp, "\t};\n");

	return 0;
}

static void delete_all_list(void)
{
	struct container_list *curr_cont;
	struct container_list *tmp_cont;
	struct obj_list *curr_obj;
	struct obj_list *tmp_obj;
	struct conn_list *curr_conn;
	struct conn_list *tmp_conn;

	curr_cont = container_head;
	curr_obj = obj_head;
	curr_conn = conn_head;

	/* delete global object lists */
	while (curr_obj) {
		tmp_obj = curr_obj;
		curr_obj = curr_obj->next;
		free(tmp_obj);
	}
	obj_head = NULL;

	/* delete global connection lists */
	while (curr_conn) {
		tmp_conn = curr_conn;
		curr_conn = curr_conn->next;
		free(tmp_conn);
	}
	conn_head = NULL;

	/* delete global container lists */
	while (curr_cont) {
		tmp_cont = curr_cont;
		curr_cont = curr_cont->next;

		/* delete the associated object lists whithin each container */
		curr_obj = tmp_cont->obj;
		while (curr_obj) {
			tmp_obj = curr_obj;
			curr_obj = curr_obj->next;
			free(tmp_obj);
		}
		tmp_cont->obj = NULL;

		free(tmp_cont);
	}
	container_head = NULL;
}

int dpl_generate(void)
{
	int error;
	uint32_t dprc_id = 0;

	if (restool.obj_name != NULL) {
		error = parse_object_name(restool.obj_name, "dprc", &dprc_id);
		if (error < 0)
			return error;
	}

	FILE *fp = stdout;

	fprintf(fp, "/dts-v1/;\n");
	fprintf(fp, "/ {\n");
	fprintf(fp, "\tdpl-version = <%d>;\n", restool.mc_fw_version.major);

	error = parse_layout(dprc_id);
	if (error) {
		ERROR_PRINTF("parse_layout() failed, error=%d\n", error);
		return error;
	}

	error = write_containers();
	if (error) {
		ERROR_PRINTF("write_containers() failed, error=%d\n", error);
		return error;
	}

	error = write_objects();
	if (error) {
		ERROR_PRINTF("write_objects() failed, error=%d\n", error);
		return error;
	}

	error = write_connections();
	if (error) {
		ERROR_PRINTF("write_connections() failed, error=%d\n", error);
		return error;
	}

	fprintf(fp, "};\n");

	delete_all_list();

	return 0;
}

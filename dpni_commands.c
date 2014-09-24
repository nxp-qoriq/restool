/*
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 * Author: German Rivera <German.Rivera@freescale.com>
 * 	   Lijun Pan <Lijun.Pan@freescale.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation  and/or other materials provided with the distribution.
 * 3. Neither the names of the copyright holders nor the names of any
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include "resman.h"
#include "utils.h"
#include "fsl_dpni.h"

#define ALL_DPNI_OPTS (				\
	DPNI_OPT_ALLOW_DIST_KEY_PER_TC |	\
	DPNI_OPT_TX_CONF_DISABLED |		\
	DPNI_OPT_PRIVATE_TX_CONF_ERR_DISABLED |	\
	DPNI_OPT_QOS |				\
	DPNI_OPT_DIST_HASH |			\
	DPNI_OPT_DIST_FS |			\
	DPNI_OPT_POLICING |			\
	DPNI_OPT_UNICAST_FILTER	|		\
	DPNI_OPT_MULTICAST_FILTER |		\
	DPNI_OPT_VLAN_FILTER |			\
	DPNI_OPT_MACSEC	|			\
	DPNI_OPT_IPR |				\
	DPNI_OPT_IPF |				\
	DPNI_OPT_RSC |				\
	DPNI_OPT_GSO |				\
	DPNI_OPT_IPSEC |			\
	DPNI_OPT_VLAN_MANIPULATION)

/**
 * dpni show command options
 */
enum dpni_info_options {
	OPT_VERBOSE = 0,
};

static struct option dpni_info_options[] = {
	[OPT_VERBOSE] = {
		.name = "verbose",
	},

	{ 0 },
};

C_ASSERT(ARRAY_SIZE(dpni_info_options) <= MAX_NUM_CMD_LINE_OPTIONS + 1);

static int cmd_dpni_help(void)
{
	ERROR_PRINTF("not implemented yet\n");
	return -ENOTSUP;
}

static void print_dpni_options(uint64_t options)
{
	if (options == 0 || (options & ~ALL_DPNI_OPTS) != 0) {
		printf("\tUnrecognized options found...\n");
		return;
	}

	if (options & DPNI_OPT_ALLOW_DIST_KEY_PER_TC)
		printf("\tDPNI_OPT_ALLOW_DIST_KEY_PER_TC\n");

	if (options & DPNI_OPT_TX_CONF_DISABLED)
		printf("\tDPNI_OPT_TX_CONF_DISABLED\n");

	if (options & DPNI_OPT_PRIVATE_TX_CONF_ERR_DISABLED)
		printf("\tDPNI_OPT_PRIVATE_TX_CONF_ERR_DISABLED\n");

	if (options & DPNI_OPT_QOS)
		printf("\tDPNI_OPT_QOS\n");

	if (options & DPNI_OPT_DIST_HASH)
		printf("\tDPNI_OPT_DIST_HASH\n");

	if (options & DPNI_OPT_DIST_FS)
		printf("\tDPNI_OPT_DIST_FS\n");

	if (options & DPNI_OPT_POLICING)
		printf("\tDPNI_OPT_POLICING\n");

	if (options & DPNI_OPT_UNICAST_FILTER)
		printf("\tDPNI_OPT_UNICAST_FILTER\n");

	if (options & DPNI_OPT_MULTICAST_FILTER)
		printf("\tDPNI_OPT_MULTICAST_FILTER\n");

	if (options & DPNI_OPT_VLAN_FILTER)
		printf("\tDPNI_OPT_VLAN_FILTER\n");

	if (options & DPNI_OPT_MACSEC)
		printf("\tDPNI_OPT_MACSEC\n");

	if (options & DPNI_OPT_IPR)
		printf("\tDPNI_OPT_IPR\n");

	if (options & DPNI_OPT_IPF)
		printf("\tDPNI_OPT_IPF\n");

	if (options & DPNI_OPT_RSC)
		printf("\tDPNI_OPT_RSC\n");

	if (options & DPNI_OPT_GSO)
		printf("\tDPNI_OPT_GSOC\n");

	if (options & DPNI_OPT_IPSEC)
		printf("\tDPNI_OPT_IPSEC\n");

	if (options & DPNI_OPT_VLAN_MANIPULATION)
		printf("\tDPNI_OPT_VLAN_MANIPULATION\n");
}

static int print_dpni_attr(uint32_t dpni_id)
{
	uint16_t dpni_handle;
	int error;
	struct dpni_attr dpni_attr;
	uint8_t mac_addr[6];
	bool dpni_opened = false;
	int link_state;

	error = dpni_open(&resman.mc_io, dpni_id, &dpni_handle);
	if (error < 0) {
		ERROR_PRINTF("dpni_open() failed for dpni.%u with error %d\n", dpni_id, error);
		goto out;
	}
	dpni_opened = true;
	if(0 == dpni_handle) {
		ERROR_PRINTF("dpni_open() returned invalid handle (auth 0) for dpni.%u\n", dpni_id);
		error = -ENOENT;
		goto out;
	}

	memset(&dpni_attr, 0, sizeof(dpni_attr));
	error = dpni_get_attributes(&resman.mc_io, dpni_handle, &dpni_attr);
	if (error < 0) {
		ERROR_PRINTF("dpni_get_attributes() failed with error: %d\n", error);
		goto out;
	}
	assert(dpni_id == (uint32_t)dpni_attr.id);
	assert(DPNI_MAX_TC >= dpni_attr.max_tcs);

	error = dpni_get_primary_mac_addr(&resman.mc_io, dpni_handle, mac_addr);
	if (error < 0) {
		ERROR_PRINTF("dpni_get_primary_mac_addr() failed with error: %d\n", error);
		goto out;
	}

	error = dpni_get_link_state(&resman.mc_io, dpni_handle, &link_state);
	if (error < 0) {
		ERROR_PRINTF("dpni_get_link_state() failed with error: %d\n", error);
		goto out;
	}

	printf("dpni version: %u.%u\n", dpni_attr.version.major, dpni_attr.version.minor);
	printf("dpni id: %d\n", dpni_attr.id);
	printf("link status: %d - ", link_state);
	link_state == 0 ? printf("down\n") :
	link_state == 1 ? printf("up\n") : printf("error state\n");
	printf("mac address: ");
	for (int j = 0; j < 5; ++j){
		printf("%02x:", mac_addr[j]);
	}
	printf("%02x\n", mac_addr[5]);
	printf("dpni_attr.options value is: %#llx\n", (unsigned long long)dpni_attr.options);
	print_dpni_options(dpni_attr.options);
	printf("max senders: %u\n", (uint32_t)dpni_attr.max_senders);
	printf("max traffic classes: %u\n", (uint32_t)dpni_attr.max_tcs);
	printf("max distribution's size per RX traffic class:\n");
	for(int k = 0; k < dpni_attr.max_tcs; ++k)
		printf("\tclass %d's size: %u\n", k, (uint32_t)dpni_attr.max_dist_per_tc[k]);
	printf("max unicast filters: %u\n", (uint32_t)dpni_attr.max_unicast_filters);
	printf("max multicast filters: %u\n", (uint32_t)dpni_attr.max_multicast_filters);
	printf("max vlan filters: %u\n", (uint32_t)dpni_attr.max_vlan_filters);
	printf("max QoS entries: %u\n", (uint32_t)dpni_attr.max_qos_entries);
	printf("max QoS key size: %u\n", (uint32_t)dpni_attr.max_qos_key_size);
	printf("max distribution key size: %u\n", (uint32_t)dpni_attr.max_dist_key_size);

	error = 0;

out:
	if (dpni_opened) {
		int error2;

		error2 = dpni_close(&resman.mc_io, dpni_handle);
		if(error2 < 0) {
			ERROR_PRINTF("dpni_close() failed with error %d\n", error2);
			if (error == 0)
				error = error2;
		}
	}

	return error;
}

static int print_dpni_verbose(void)
{
	printf("dpni verbose info coming out soon\n");
	return 0;
}

static int print_dpni_info(uint32_t dpni_id)
{
	int error;

	error = print_dpni_attr(dpni_id);
	if (error < 0)
		goto out;

	if (resman.cmd_option_mask & ONE_BIT_MASK(OPT_VERBOSE)) {
		resman.cmd_option_mask &= ~ONE_BIT_MASK(OPT_VERBOSE);
		error = print_dpni_verbose();
		goto out;
	}

out:
	return error;
}

static int cmd_dpni_info(void)
{
	uint32_t obj_id;
	int error;

	if (resman.obj_name == NULL) {
		ERROR_PRINTF("<object> argument missing\n");
		error = -EINVAL;
		goto out;
	}

	error = parse_object_name(resman.obj_name, "dpni", &obj_id);
	if (error < 0)
		goto out;

	error = print_dpni_info(obj_id);
out:
	return error;
}

static int cmd_dpni_create(void)
{
	ERROR_PRINTF("not implemented yet\n");
	return -ENOTSUP;
}

static int cmd_dpni_destroy(void)
{
	ERROR_PRINTF("not implemented yet\n");
	return -ENOTSUP;
}

struct object_command dpni_commands[] = {
	{ .cmd_name = "help",
	  .options = NULL,
	  .cmd_func = cmd_dpni_help },

	{ .cmd_name = "info",
	  .options = dpni_info_options,
	  .cmd_func = cmd_dpni_info },

	{ .cmd_name = "create",
	  .options = NULL,
	  .cmd_func = cmd_dpni_create },

	{ .cmd_name = "destroy",
	  .options = NULL,
	  .cmd_func = cmd_dpni_destroy },

	{ .cmd_name = NULL },
};


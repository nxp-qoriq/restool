#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include "fsl_mc_io_wrapper.h"
#include "fsl_mc_cmd_wrappers.h"

static const char resman_version[] = "0.1";

#define ERROR_PRINTF(_fmt, ...)	\
	fprintf(stderr, "%s: " _fmt, __func__, ##__VA_ARGS__)

#define DEBUG_PRINTF(_fmt, ...)	\
	fprintf(stderr, "DBG: %s: " _fmt, __func__, ##__VA_ARGS__)

enum long_option_indices {
	OPT_HELP = 0,
	OPT_VERSION,
};

static struct option long_options[] = {
	[OPT_HELP] = {
		.name =  "help",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	[OPT_VERSION] = {
		.name = "version",
		.has_arg = 0,
		.flag = NULL,
		.val = 0,
	},

	{ 0 },
};

typedef int resman_cmd_func_t(int argc, char *argv[]);

struct resman_command {
	char *name;
	resman_cmd_func_t *func;
};

static int dprc_list(int argc, char *argv[]);

static const struct resman_command resman_commands[] = {
	{ "list", dprc_list }
};

static void print_usage(void)
{
	printf("Usage message place holder\n");
}

static void print_version(void)
{
	printf("Freescale MC resman tool version %s\n", resman_version);
}

static int parse_cmd_line(int argc, char *argv[])
{
	int c;
	int opt_index;
	int error = 0;

	for (;;) {
		c = getopt_long(argc, argv, "hv", long_options, &opt_index);
		if (c == -1)
			break;

		switch (c) {
		case 'h':
			print_usage();
			goto out;

		case 'v':
			print_version();
			goto out;

		case '?':
			error = -EINVAL;
			goto out;
		case 0:
			switch (opt_index) {
			case OPT_HELP:
				print_usage();
				break;
			case OPT_VERSION:
				print_version();
				break;
			default:
				assert(false);
			}

			break;
	       default:
			assert(false);
		}
	}

	if (optind >= argc) {
		fprintf(stderr, "Command arguments expected\n");
		error = -EINVAL;
		goto out;
	}

out:
	return error;
}

static int run_dprc_test(
	struct mc_portal_wrapper *mc_portal,
	uint16_t dprc_handle)
{
	int error = 0;
	struct dprc_attributes dprc_attr;
	struct dprc_region_desc region_desc;
	uint32_t num_child_devices;
	int test_failures = 0;
	uint32_t container_id;
	uint32_t dpni_id = (uint32_t)-1;
	struct mc_version mc_fw_version;

	printf("*** Running Unit tests (built " __DATE__ " " __TIME__ ")\n");

	/*
	 * Use Portal Id 0
	 */
	printf("*** calling mc_get_version() using portal %p\n",
	       mc_portal->mmio_regs);
	error = mc_get_version(mc_portal, &mc_fw_version);
	if (error != 0) {
		printf("mc_get_version() failed with error %d\n", error);
		goto out;
	}

	printf("*** MC Firmware version: %u.%u.%u\n",
	       mc_fw_version.major,
	       mc_fw_version.minor,
	       mc_fw_version.revision);

#if 0
	container_id = 0;
#else
	error = dprc_get_container_id(mc_portal, &container_id);
	if (error < 0) {
		printf("dprc_get_container_id() failed: %d\n", error);
		goto out;
	}
	DEBUG_PRINTF("*** DPRC container id %d\n", container_id);
#endif

//#if 0
	//DEBUG_PRINTF("*** calling dprc_open() for container id %d\n", container_id);
	DEBUG_PRINTF("*** calling dprc_open() for container id %d\n", 0);
	error = dprc_open(mc_portal, 0, &dprc_handle);
	if (error != 0) {
		DEBUG_PRINTF("*** dprc_open() failed with error %d\n", error);
		goto out;
	}

	DEBUG_PRINTF("*** dprc handle returned by dprc_open(): %u\n",
	       dprc_handle);
//#endif

	DEBUG_PRINTF("*** calling dprc_get_attributes() for container id %d\n", container_id);
	error = dprc_get_attributes(mc_portal, dprc_handle, &dprc_attr);
	if (error != 0) {
		DEBUG_PRINTF("*** dprc_get_attributes() failed with error %d\n", error);
		goto out;
	}

	DEBUG_PRINTF("*** attributes returned by dprc_get_attributes():\n"
	       "\tcontainer_id: %d\n"
	       "\ticid: %#0hx\n"
	       "\tportal_id: %d\n"
	       "\toptions: %#0llx\n",
	       dprc_attr.container_id,
	       dprc_attr.icid,
	       dprc_attr.portal_id,
	       (unsigned long long)dprc_attr.options);

	num_child_devices = 0;
	DEBUG_PRINTF("*** calling dprc_get_device_count()\n");
	error = dprc_get_device_count(mc_portal, dprc_handle, &num_child_devices);
	if (error != 0) {
		DEBUG_PRINTF("*** dprc_get_device_count() failed with error %d\n", error);
		goto out;
	}

	DEBUG_PRINTF("*** Number of devices returned by dprc_get_device_count(): %d\n",
		num_child_devices);

	//assert(num_child_devices != 0);

	{
		struct dprc_res_ids_range_desc range_desc;
		uint32_t res_count;

		DEBUG_PRINTF("*** calling dprc_get_res_count()\n");
		error = dprc_get_res_count(mc_portal, dprc_handle, "mcp", &res_count);
		if (error != 0) {
			DEBUG_PRINTF("*** dprc_get_res_count() failed with error %d\n", error);
			goto out;
		}

		DEBUG_PRINTF("*** Number of MC portals returned by dprc_get_res_count(): %d\n",
		       res_count);

		assert(res_count != 0);
		memset(&range_desc, 0,
		       sizeof(struct dprc_res_ids_range_desc));

		do {
			DEBUG_PRINTF("*** calling dprc_get_res_ids()\n");
			error = dprc_get_res_ids(mc_portal, dprc_handle, "mcp", &range_desc);
			if (error != 0) {
				DEBUG_PRINTF("*** dprc_get_res_ids() failed with error %d\n", error);
				goto out;
			}


			DEBUG_PRINTF("*** Portal id range returned by dprc_get_res_ids(): %u .. %u (status %d)\n",
			       range_desc.base_id, range_desc.last_id, range_desc.iter_status);

		} while (range_desc.iter_status != DPRC_ITER_STATUS_LAST);
	}

	{
		unsigned int i;
		struct dprc_dev_desc dev_desc;

		for (i = 0; i < num_child_devices; i++) {
			DEBUG_PRINTF("*** calling dprc_get_device() index %d\n", i);
			error = dprc_get_device(
					mc_portal, dprc_handle,
					i,
					&dev_desc);
			if (error != 0) {
				DEBUG_PRINTF("*** dprc_get_device() failed with error %d\n", error);
				test_failures ++;
				continue;
			}

			DEBUG_PRINTF("*** dprc_dev_desc returned by dprc_get_device(): \n"
				"\tvendor %#x\n"
				"\ttype %s\n"
				"\tid %#x\n"
				"\trev_major %#x\n"
				"\trev_minor %#x\n"
				"\tirq_count %d\n"
				"\tregion_count %d\n"
				"\tstate %#x\n",
				dev_desc.vendor,
				dev_desc.type,
				dev_desc.id,
				dev_desc.rev_major,
				dev_desc.rev_minor,
				dev_desc.irq_count,
				dev_desc.region_count,
				dev_desc.state);

			if (strcmp(dev_desc.type, "dpni") == 0) {
			    if (dpni_id == (uint32_t)-1) {
				    dpni_id = dev_desc.id;
			    }
			}

			if (dev_desc.region_count != 0) {
				DEBUG_PRINTF("*** calling dprc_get_dev_region() for device index %d\n", i);
				error = dprc_get_dev_region(
					mc_portal, dprc_handle,
					dev_desc.type,
					dev_desc.id,
					0,
					&region_desc);

				if (error != 0) {
					DEBUG_PRINTF("*** dprc_get_dev_region() failed with error %d\n", error);
					test_failures ++;
					continue;
				}

				DEBUG_PRINTF("*** dprc_region_desc returned by dprc_get_dev_region(): \n"
				       "\tbase_paddr %#llx\n"
				       "\tsize %hu\n",
				       (unsigned long long)region_desc.base_paddr,
				       region_desc.size);
			} else {
				assert(strcmp(dev_desc.type, "dprc") != 0);
			}
		}
	}

#if 0
	DEBUG_PRINTF("*** calling dprc_close()\n");
	error = dprc_close(mc_portal, dprc_handle);
	if (error != 0) {
		DEBUG_PRINTF("*** dprc_close() failed with error %d\n", error);
		goto out;
	}
#endif

	if (test_failures != 0) {
		DEBUG_PRINTF("*** %d Tests failed ***\n", test_failures);
		error = EINVAL;
		goto out;
	}

	DEBUG_PRINTF("*** All tests passed ***\n");
out:
	return error;
}

int main(int argc, char *argv[])
{
	struct mc_portal_wrapper mc_portal;
	int error;
	uint16_t dprc_handle;
	bool mc_portal_initialized = false;

	DEBUG_PRINTF("resman built on " __DATE__ " " __TIME__ "\n");
	error = mc_portal_wrapper_init(&mc_portal);
	if (error != 0)
		goto out;

	mc_portal_initialized = true;
	DEBUG_PRINTF("mc_portal regs: %p\n", mc_portal.mmio_regs);
	DEBUG_PRINTF("calling ioctl\n");
	error = ioctl(mc_portal.fd, 0x1, &dprc_handle);
	if (error == -1) {
		error = -errno;
		goto out;
	}

	DEBUG_PRINTF("ioctl returned dprc_handle: %#x\n", dprc_handle);
	//???
	run_dprc_test(&mc_portal, dprc_handle); //???
	//???
	error = parse_cmd_line(argc, argv);
out:
	if (mc_portal_initialized)
		mc_portal_wrapper_cleanup(&mc_portal);

	return error;
}

static int dprc_list(int argc, char *argv[])
{
	for (int i = 0; i < argc; i++) {
		DEBUG_PRINTF("argv[%d]: %s\n", i, argv[i]);
	}

	return 0;
}

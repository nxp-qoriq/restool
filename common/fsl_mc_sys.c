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

#include <errno.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>		/* open() */
#include <unistd.h>		/* close() */
#include <sys/ioctl.h>
#include "fsl_mc_sys.h"
#include "fsl_mc_ioctl.h"
#include "utils.h"

int mc_io_init(struct fsl_mc_io *mc_io)
{
	int fd = -1;
	int error;

	fd = open(restool.device_file, O_RDWR | O_SYNC);

	if (fd < 0) {
		error = -errno;
		perror("open() failed ");
		goto error;
	}

	mc_io->fd = fd;
	return 0;
error:
	if (fd != -1)
		(void)close(fd);

	return error;
}

void mc_io_cleanup(struct fsl_mc_io *mc_io)
{
	int error;

	assert(mc_io->fd != -1);

	error = close(mc_io->fd);
	if (error == -1)
		perror("close failed");
}

int mc_send_command(struct fsl_mc_io *mc_io, struct mc_command *cmd)
{
	int error;

	if (strcmp(restool.device_file, "/dev/mc_restool") == 0)
		error = ioctl(mc_io->fd, RESTOOL_SEND_MC_COMMAND_LEGACY, cmd);
	else
		error = ioctl(mc_io->fd, RESTOOL_SEND_MC_COMMAND, cmd);

	if (error == -1) {
		error = -errno;
		DEBUG_PRINTF(
			"ioctl(RESTOOL_SEND_MC_COMMAND) failed with error %d\n",
			error);
	}

	return error;
}

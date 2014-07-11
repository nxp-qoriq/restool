#ifndef _FSL_MC_SYS_H
#define _FSL_MC_SYS_H

#include <stdint.h>

#ifndef ENOTSUP
#define ENOTSUP		95
#endif

#ifndef __iomem
#define __iomem	volatile
#endif

static inline uint64_t ioread64(uint64_t __iomem *io_reg)
{
	return *io_reg;
}

static inline void iowrite64(uint64_t value, uint64_t __iomem *io_reg)
{
	*io_reg = value;
}

struct mc_command;

int mc_send_command(void *mc_io, struct mc_command *cmd);

#endif /* _FSL_MC_SYS_H */

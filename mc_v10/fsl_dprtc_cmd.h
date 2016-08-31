/* Copyright 2013-2016 Freescale Semiconductor Inc.
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
#ifndef _FSL_DPRTC_CMD_H
#define _FSL_DPRTC_CMD_H

/* DPRTC Version */
#define DPRTC_VER_MAJOR				2
#define DPRTC_VER_MINOR				0

/* Command IDs */
#define DPRTC_CMDID_CLOSE                               (0x800 << 4) | (0x1)
#define DPRTC_CMDID_OPEN                                (0x810 << 4) | (0x1)
#define DPRTC_CMDID_CREATE                              (0x910 << 4) | (0x1)
#define DPRTC_CMDID_DESTROY                             (0x990 << 4) | (0x1)
#define DPRTC_CMDID_GET_VERSION                         (0xa10 << 4) | (0x1)

#define DPRTC_CMDID_ENABLE                              (0x002 << 4) | (0x1)
#define DPRTC_CMDID_DISABLE                             (0x003 << 4) | (0x1)
#define DPRTC_CMDID_GET_ATTR                            (0x004 << 4) | (0x1)
#define DPRTC_CMDID_RESET                               (0x005 << 4) | (0x1)
#define DPRTC_CMDID_IS_ENABLED                          (0x006 << 4) | (0x1)

#define DPRTC_CMDID_SET_IRQ                             (0x010 << 4) | (0x1)
#define DPRTC_CMDID_GET_IRQ                             (0x011 << 4) | (0x1)
#define DPRTC_CMDID_SET_IRQ_ENABLE                      (0x012 << 4) | (0x1)
#define DPRTC_CMDID_GET_IRQ_ENABLE                      (0x013 << 4) | (0x1)
#define DPRTC_CMDID_SET_IRQ_MASK                        (0x014 << 4) | (0x1)
#define DPRTC_CMDID_GET_IRQ_MASK                        (0x015 << 4) | (0x1)
#define DPRTC_CMDID_GET_IRQ_STATUS                      (0x016 << 4) | (0x1)
#define DPRTC_CMDID_CLEAR_IRQ_STATUS                    (0x017 << 4) | (0x1)

#define DPRTC_CMDID_SET_CLOCK_OFFSET                    (0x1d0 << 4) | (0x1)
#define DPRTC_CMDID_SET_FREQ_COMPENSATION               (0x1d1 << 4) | (0x1)
#define DPRTC_CMDID_GET_FREQ_COMPENSATION               (0x1d2 << 4) | (0x1)
#define DPRTC_CMDID_GET_TIME                            (0x1d3 << 4) | (0x1)
#define DPRTC_CMDID_SET_TIME                            (0x1d4 << 4) | (0x1)
#define DPRTC_CMDID_SET_ALARM                           (0x1d5 << 4) | (0x1)
#define DPRTC_CMDID_SET_PERIODIC_PULSE                  (0x1d6 << 4) | (0x1)
#define DPRTC_CMDID_CLEAR_PERIODIC_PULSE                (0x1d7 << 4) | (0x1)
#define DPRTC_CMDID_SET_EXT_TRIGGER                     (0x1d8 << 4) | (0x1)
#define DPRTC_CMDID_CLEAR_EXT_TRIGGER                   (0x1d9 << 4) | (0x1)
#define DPRTC_CMDID_GET_EXT_TRIGGER_TIMESTAMP           (0x1dA << 4) | (0x1)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_CMD_OPEN(cmd, dpbp_id) \
	MC_CMD_OP(cmd, 0, 0,  32, int,	    dpbp_id)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_RSP_IS_ENABLED(cmd, en) \
	MC_RSP_OP(cmd, 0, 0,  1,  int,	    en)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_CMD_SET_IRQ(cmd, irq_index, irq_cfg) \
do { \
	MC_CMD_OP(cmd, 0, 0,  8,  uint8_t,  irq_index);\
	MC_CMD_OP(cmd, 0, 32, 32, uint32_t, irq_cfg->val);\
	MC_CMD_OP(cmd, 1, 0,  64, uint64_t, irq_cfg->addr); \
	MC_CMD_OP(cmd, 2, 0,  32, int,	    irq_cfg->irq_num); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_CMD_GET_IRQ(cmd, irq_index) \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_RSP_GET_IRQ(cmd, type, irq_cfg) \
do { \
	MC_RSP_OP(cmd, 0, 0,  32, uint32_t, irq_cfg->val); \
	MC_RSP_OP(cmd, 1, 0,  64, uint64_t, irq_cfg->addr); \
	MC_RSP_OP(cmd, 2, 0,  32, int,	    irq_cfg->irq_num); \
	MC_RSP_OP(cmd, 2, 32, 32, int,	    type); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_CMD_SET_IRQ_ENABLE(cmd, irq_index, en) \
do { \
	MC_CMD_OP(cmd, 0, 0,  8,  uint8_t,  en); \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_CMD_GET_IRQ_ENABLE(cmd, irq_index) \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_RSP_GET_IRQ_ENABLE(cmd, en) \
	MC_RSP_OP(cmd, 0, 0,  8,  uint8_t,  en)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_CMD_SET_IRQ_MASK(cmd, irq_index, mask) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, uint32_t, mask);\
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_CMD_GET_IRQ_MASK(cmd, irq_index) \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_RSP_GET_IRQ_MASK(cmd, mask) \
	MC_RSP_OP(cmd, 0, 0,  32, uint32_t, mask)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_CMD_GET_IRQ_STATUS(cmd, irq_index, status) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, uint32_t, status);\
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index);\
} while (0)
/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_RSP_GET_IRQ_STATUS(cmd, status) \
	MC_RSP_OP(cmd, 0, 0,  32, uint32_t, status)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_CMD_CLEAR_IRQ_STATUS(cmd, irq_index, status) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, uint32_t, status); \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index);\
} while (0)

/*                cmd, param, offset, width, type,	arg_name */
#define DPRTC_RSP_GET_ATTRIBUTES(cmd, attr) \
do { \
	MC_RSP_OP(cmd, 0, 32, 32, int,	    attr->id);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_CMD_SET_CLOCK_OFFSET(cmd, offset) \
	MC_CMD_OP(cmd, 0, 0,  64, int64_t, offset)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_CMD_SET_FREQ_COMPENSATION(cmd, freq_compensation) \
	MC_CMD_OP(cmd, 0, 0,  32, uint32_t, freq_compensation)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_RSP_GET_FREQ_COMPENSATION(cmd, freq_compensation) \
	MC_RSP_OP(cmd, 0, 0,  32, uint32_t, freq_compensation)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_RSP_GET_TIME(cmd, time) \
	MC_RSP_OP(cmd, 0, 0,  64, uint64_t, time)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_CMD_SET_TIME(cmd, time) \
	MC_CMD_OP(cmd, 0, 0,  64, uint64_t, time)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_CMD_SET_ALARM(cmd, time) \
	MC_CMD_OP(cmd, 0, 0,  64, uint64_t, time)
#if 0
/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_CMD_SET_PERIODIC_PULSE(cmd, cfg) \
do { \
	MC_CMD_OP(cmd, 0, 0,  8,  uint8_t,  cfg->periodic_pulse_id); \
	MC_CMD_OP(cmd, 1, 0,  64, uint64_t, cfg->period);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_CMD_CLEAR_PERIODIC_PULSE(cmd, id) \
	MC_CMD_OP(cmd, 0, 0,  8,  uint8_t,  id)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_CMD_SET_EXT_TRIGGER(cmd, cfg) \
do { \
	MC_CMD_OP(cmd, 0, 0,  8,  uint8_t,  cfg->ext_trigger_id);
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_CMD_CLEAR_EXT_TRIGGER(cmd, id) \
	MC_CMD_OP(cmd, 0, 0,  8,  uint8_t,  id)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_CMD_GET_EXT_TRIGGER_TIMESTAMP(cmd, id) \
	MC_CMD_OP(cmd, 0, 0,  8,  uint8_t,  id)

/*                cmd, param, offset, width, type, arg_name */
#define DPRTC_RSP_GET_EXT_TRIGGER_TIMESTAMP(cmd, timestamp) \
	MC_CMD_OP(cmd, 0, 0,  64, uint64_t,  timestamp)

#endif /* 0 */
/*                cmd, param, offset, width, type,      arg_name */
#define DPRTC_RSP_GET_VERSION(cmd, major, minor) \
    do { \
            MC_RSP_OP(cmd, 0, 0,  16, uint16_t, major);\
            MC_RSP_OP(cmd, 0, 16, 16, uint16_t, minor);\
    } while (0)
#endif /* _FSL_DPRTC_CMD_H */

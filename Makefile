CROSS_COMPILE ?=
DESTDIR ?=

ifeq ($(origin CC),default)
	CC := $(CROSS_COMPILE)gcc
endif

OBJS = restool.o \
       dprc_commands.o \
       dprc_commands_generate_dpl.o \
       dpni_commands.o \
       dpio_commands.o \
       dpbp_commands.o \
       dpsw_commands.o \
       dpci_commands.o \
       dpcon_commands.o \
       dpseci_commands.o \
       dpdmux_commands.o \
       dpmcp_commands.o \
       dpmac_commands.o \
       dpdcei_commands.o \
       dpaiop_commands.o \
       dpdbg_commands.o \
       dprtc_commands.o \
       dpdmai_commands.o \
       common/fsl_mc_sys.o \
       mc_v9/dpaiop.o \
       mc_v9/dpbp.o \
       mc_v9/dpci.o \
       mc_v9/dpcon.o \
       mc_v9/dpdcei.o \
       mc_v9/dpdmai.o \
       mc_v9/dpdmux.o \
       mc_v9/dpio.o \
       mc_v9/dpmac.o \
       mc_v9/dpmcp.o \
       mc_v9/dprc.o \
       mc_v9/dprtc.o \
       mc_v9/dpseci.o \
       mc_v9/dpmng.o \
       mc_v9/dpni.o \
       mc_v9/dpsw.o \
       mc_v9/dpdbg.o \
       mc_v10/dpaiop.o \
       mc_v10/dpbp.o \
       mc_v10/dpci.o \
       mc_v10/dpcon.o \
       mc_v10/dpdcei.o \
       mc_v10/dpdmai.o \
       mc_v10/dpio.o \
       mc_v10/dpmac.o \
       mc_v10/dpmcp.o \
       mc_v10/dprtc.o \
       mc_v10/dpseci.o \
       mc_v10/dpsw.o \
       mc_v10/dpdmux.o \
       mc_v10/dpni.o

CFLAGS = ${EXTRA_CFLAGS} \
	  -Wall \
          -Wstrict-prototypes \
          -Wextra -Wformat \
          -std=gnu99 \
          -Wmissing-prototypes \
          -Wpointer-arith \
          -Winline \
          -Werror \
          -Wundef \
          -Icommon/

VERSION=$(shell test -d .git && git describe)
ifeq ($(VERSION),)
VERSION=v1.6
endif

override CFLAGS += -DVERSION=\"${VERSION}\"

LDFLAGS = -static -Wl,--hash-style=gnu

PREFIX = $(DESTDIR)/usr/bin
EXEC_PREFIX = $(DESTDIR)/usr/bin

HEADER_DEPENDENCIES = $(subst .o,.d,$(OBJS))

all: restool

restool: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) -lm
	file $@

install:
	install -d $(PREFIX) $(EXEC_PREFIX)
	install -m 755 restool $(PREFIX)
	cp -d scripts/* $(EXEC_PREFIX)
	chmod 755 $(EXEC_PREFIX)/ls-main

clean:
	rm -f $(OBJS) \
	      $(HEADER_DEPENDENCIES) \
	      restool
	rm -rf build

%.d: %.c
	@($(CC) $(CFLAGS) -M $< | \
	  sed 's,\($(notdir $*)\.o\) *:,$(dir $@)\1 $@: ,' > $@.tmp); \
	 mv $@.tmp $@

-include $(HEADER_DEPENDENCIES)




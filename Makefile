CROSS_COMPILE ?=

CC = $(CROSS_COMPILE)gcc

OBJS = restool.o \
       dprc_commands.o \
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
       dprc.o \
       dpmng.o \
       dpni.o \
       dpbp.o \
       dpio.o \
       dpsw.o \
       dpci.o \
       dpcon.o \
       dpseci.o \
       dpdmux.o \
       dpmcp.o \
       dpmac.o \
       dpdcei.o \
       dpaiop.o \
       fsl_mc_sys.o

CFLAGS = ${EXTRA_CFLAGS} \
	  -Wall \
          -Wstrict-prototypes \
          -Wextra -Wformat \
          -std=gnu99 \
          -Wmissing-prototypes \
          -Wpointer-arith \
          -Winline \
          -Werror \
          -Wundef

LDFLAGS = -static -Wl,--hash-style=gnu

PREFIX = /sbin
EXEC_PREFIX = /usr/sbin

HEADER_DEPENDENCIES = $(subst .o,.d,$(OBJS))

all: restool

restool: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) -lm
	file $@

install:
	install -m 755 restool $(PREFIX)
	cp -d scripts/* $(EXEC_PREFIX)
	chmod 755 $(EXEC_PREFIX)/ls-main

clean:
	rm -f $(OBJS) \
	      $(HEADER_DEPENDENCIES) \
	      restool

%.d: %.c
	@($(CC) $(CFLAGS) -M $< | \
	  sed 's,\($(notdir $*)\.o\) *:,$(dir $@)\1 $@: ,' > $@.tmp); \
	 mv $@.tmp $@

-include $(HEADER_DEPENDENCIES)




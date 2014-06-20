CROSS_COMPILE ?=

CC = $(CROSS_COMPILE)gcc

OBJS = resman.o \
       fsl_mc_dpmng_cmd_wrappers.o \
       fsl_mc_dprc_cmd_wrappers.o \
       fsl_mc_io_wrapper.o

CFLAGS  = -Wall \
          -Wstrict-prototypes \
          -Wextra -Wformat \
          -std=gnu99 \
          -Wmissing-prototypes \
          -Wpointer-arith \
          -Winline \
          -Werror \
          -Wundef \
	  -I./mc_hardware

LDFLAGS = -static -Wl,--hash-style=gnu

HEADER_DEPENDENCIES = $(subst .o,.d,$(OBJS))

all: resman

resman: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
	file $@

clean:
	rm -f *.[od] resman

%.d: %.c
	@($(CC) $(CFLAGS) -M $< | \
	  sed 's,\($(notdir $*)\.o\) *:,$(dir $@)\1 $@: ,' > $@.tmp); \
	 mv $@.tmp $@

-include $(HEADER_DEPENDENCIES)




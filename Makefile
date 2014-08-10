CROSS_COMPILE ?=

CC = $(CROSS_COMPILE)gcc

OBJS = resman.o \
       dprc.o \
       dpmng.o \
       dpni.o \
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

HEADER_DEPENDENCIES = $(subst .o,.d,$(OBJS))

all: resman

resman: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
	file $@

clean:
	rm -f $(OBJS) \
	      $(HEADER_DEPENDENCIES) \
	      resman

%.d: %.c
	@($(CC) $(CFLAGS) -M $< | \
	  sed 's,\($(notdir $*)\.o\) *:,$(dir $@)\1 $@: ,' > $@.tmp); \
	 mv $@.tmp $@

-include $(HEADER_DEPENDENCIES)




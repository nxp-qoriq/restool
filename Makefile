ifeq ($(origin CC),default)
	CC := $(CROSS_COMPILE)gcc
endif

SRC = $(shell find . -name "*.c")
OBJ = $(patsubst %.c, %.o, $(SRC))

RESTOOL_SCRIPT_SYMLINKS = ls-addmux ls-addsw ls-addni ls-listni ls-listmac

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
VERSION=v2.1
endif

override CFLAGS += -DVERSION=\"${VERSION}\"

prefix ?= /usr/local
exec_prefix ?= ${prefix}
bindir ?= ${exec_prefix}/bin

all: restool

restool: $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@ -lm
	file $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

install: restool scripts/ls-main scripts/ls-append-dpl scripts/ls-debug
	install -D -m 755 restool $(DESTDIR)$(bindir)/restool
	install -D -m 755 scripts/ls-main $(DESTDIR)$(bindir)/ls-main
	install -D -m 755 scripts/ls-append-dpl $(DESTDIR)$(bindir)/ls-append-dpl
	install -D -m 755 scripts/ls-debug $(DESTDIR)$(bindir)/ls-debug
	$(foreach symlink, $(RESTOOL_SCRIPT_SYMLINKS), sh -c "cd $(DESTDIR)$(bindir) && ln -sf ls-main $(symlink)" ;)

clean:
	rm -f $(OBJ) \
	      restool


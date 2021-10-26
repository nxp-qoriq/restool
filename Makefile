ifeq ($(origin CC),default)
	CC := $(CROSS_COMPILE)gcc
endif

SRC = $(shell find . -name "*.c")
OBJ = $(patsubst %.c, %.o, $(SRC))
MANPAGE = restool.1
RESTOOL_SCRIPT_SYMLINKS = ls-addmux ls-addsw ls-addni ls-listni ls-listmac ls-delete

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
          -Wno-date-time \
          -Icommon/

MC_VERSION_COMPATIBLE='10.30.0'
VERSION_STATIC=v2.4
VERSION_COMMIT=$(shell test -d .git && git describe)
ifeq ($(VERSION_COMMIT),)
VERSION='$(VERSION_STATIC) ($(MC_VERSION_COMPATIBLE) compatible)'
else
VERSION='$(VERSION_STATIC) (commit $(VERSION_COMMIT)) (MC $(MC_VERSION_COMPATIBLE) compatible)'
endif

override CFLAGS += -DVERSION=\"${VERSION}\"

prefix ?= /usr/local
exec_prefix ?= ${prefix}
bindir ?= ${exec_prefix}/bin
bindir_completion ?= /usr/share/bash-completion/completions
datarootdir ?= ${prefix}/share
mandir ?= ${datarootdir}/man

get_man_section = $(lastword $(subst ., ,$1))
get_manpage_destination = $(join $(DESTDIR)${mandir}/man, \
			  $(join $(call get_man_section,$1)/, \
                          $(subst docs/man/,,$1)))

all: restool

restool: $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@ -lm
	file $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

%.1: %.md
	pandoc --standalone --to man $^ -o $@

install: restool scripts/ls-main scripts/ls-append-dpl scripts/ls-debug scripts/restool_completion.sh $(MANPAGE)
	install -D -m 755 restool $(DESTDIR)$(bindir)/restool
	install -D -m 755 scripts/ls-main $(DESTDIR)$(bindir)/ls-main
	install -D -m 755 scripts/ls-append-dpl $(DESTDIR)$(bindir)/ls-append-dpl
	install -D -m 755 scripts/ls-debug $(DESTDIR)$(bindir)/ls-debug
	$(foreach symlink, $(RESTOOL_SCRIPT_SYMLINKS), sh -c "cd $(DESTDIR)$(bindir) && ln -sf ls-main $(symlink)" ;)
	install -D -m 755 scripts/restool_completion.sh $(DESTDIR)$(bindir_completion)/restool
	install -m 0644 -D $(MANPAGE) $(call get_manpage_destination,$(MANPAGE))

clean:
	rm -f $(OBJ) $(MANPAGE) \
	      restool


ifeq ($(origin CC),default)
	CC := $(CROSS_COMPILE)gcc
endif

SRC = $(shell find . -name "*.c")
OBJ = $(patsubst %.c, %.o, $(SRC))

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

PREFIX = $(DESTDIR)/usr/bin
EXEC_PREFIX = $(DESTDIR)/usr/bin

all: restool

restool: $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@ -lm
	file $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

install:
	install -d $(PREFIX) $(EXEC_PREFIX)
	install -m 755 restool $(PREFIX)
	cp -d scripts/* $(EXEC_PREFIX)
	chmod 755 $(EXEC_PREFIX)/ls-main

clean:
	rm -f $(OBJ) \
	      restool


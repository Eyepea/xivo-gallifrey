# $Revision$
# $Date$
#
# set PATH and CC to include cross compiling tools for cross compilation, e.g.
# export PATH=/path/to/toolchain/powerpc-linux-gnu/bin:$PATH
# export CC=gcc
#

# you can override default options by calling make OPTIONS=...
CFLAGS ?= -O2
Z_CFLAGS = $(CFLAGS) -D _GNU_SOURCE -DAST_MODULE=\"res_watchdog\" -Wall -Wmissing-prototypes -Wstrict-prototypes $(OPTIONS)
LIBS ?=
Z_LIBS = $(LIBS) -lpthread

BINARY = res_watchdog.so
OBJECTS = res_watchdog.o

# use make DESTDIR=/path/to/sysroot PREFIX=/usr/local if you want to
# install this module in a non-standard location
DESTDIR ?=
PREFIX ?= /usr

.PHONY: install clean

$(BINARY): $(OBJECTS)
	$(CC) -shared -Xlinker -x -o $(BINARY) $(OBJECTS) $(Z_LIBS)

%.o: %.c
	$(CC) $(Z_CFLAGS) -c -o $@ $<

install: $(BINARY)
	install -d $(DESTDIR)$(PREFIX)/lib/asterisk/modules
	install -m 755 $(BINARY) $(DESTDIR)$(PREFIX)/lib/asterisk/modules
	install -d $(DESTDIR)/etc/asterisk
	install -m 644 configs/watchdog.conf.sample $(DESTDIR)/etc/asterisk/watchdog.conf

clean:
	rm -f $(BINARY) $(OBJECTS)


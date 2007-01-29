# set PATH and CC to include cross compiling tools for cross compilation, e.g.
# export PATH=/path/to/toolchain/powerpc-linux-gnu/bin:$PATH
# export CC=gcc

# you can override default options by calling make OPTIONS=...
OPTIONS ?= -O2
CFLAGS = -D _GNU_SOURCE -Wall -Wmissing-prototypes -Wstrict-prototypes $(OPTIONS)
LIBS = -lpthread

BINARY = res_watchdog.so
OBJECTS = res_watchdog.o

# use make INSTALL_PREFIX=/path/to/sysroot PREFIX=/usr/local if you want to
# install this module in a non-standard location
INSTALL_PREFIX ?=
PREFIX ?= /usr

.PHONY: install clean

$(BINARY): $(OBJECTS)
	$(CC) -shared -Xlinker -x -o $(BINARY) $(OBJECTS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

install: $(BINARY)
	install -m 755 $(BINARY) $(INSTALL_PREFIX)$(PREFIX)/lib/asterisk/modules

clean:
	rm -f $(BINARY) $(OBJECTS)

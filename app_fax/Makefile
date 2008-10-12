# Set PATH and CC to include cross compiling tools for cross compilation, e.g.
# export PATH=/path/to/toolchain/powerpc-linux-gnu/bin:$PATH
# export CC=gcc
# Relaxed Jedi Master Linux Compilation options:
MY_CFLAGS =     -Wundef                         \
                -Wstrict-prototypes             \
                -Wmissing-prototypes            \
                -Wall                           \
                -W                              \
                -Wcast-qual                     \
                -Wcast-align                    \
                -Wwrite-strings                 \
                -Wnested-externs                \
                -Wmissing-declarations          \
                -Wshadow

CFLAGS ?= -O2 -fPIC
Z_CFLAGS := $(CFLAGS) -D _GNU_SOURCE $(MY_CFLAGS) -Wall -Wmissing-prototypes -Wstrict-prototypes
LIBS ?=
Z_LIBS := $(LIBS) -lspandsp

BINARIES = app_rxfax.so app_txfax.so

# Use make DESTDIR=/path/to/sysroot PREFIX=/usr/local if you want to
# install this module in a non-standard location.
DESTDIR ?=
PREFIX ?= /usr

.PHONY: all install clean

all: $(BINARIES)

%.so: %.o
	./gwr.py $< --- $(CC) -shared -Xlinker -x -o $@ $< $(Z_LIBS)

%.o: %.c
	$(CC) $(Z_CFLAGS) -c -o $@ $<

install: $(BINARIES)
	install -d $(DESTDIR)$(PREFIX)/lib/asterisk/modules
	install -m 755 $(BINARIES) $(DESTDIR)$(PREFIX)/lib/asterisk/modules

clean:
	rm -f $(BINARIES) *.o

# Set PATH and CC to include cross compiling tools for cross compilation, e.g.
# export PATH=/path/to/toolchain/powerpc-linux-gnu/bin:$PATH
# export CC=gcc

# Relaxed Jedi Master Linux Compilation options:
MY_CFLAGS =     -std=gnu99                      \
                -Wundef                         \
                -Wstrict-prototypes             \
                -Wmissing-prototypes            \
                -Wall                           \
                -W                              \
                -Wcast-qual                     \
                -Wcast-align                    \
                -Wwrite-strings                 \
                -Wnested-externs                \
                -Wmissing-declarations          \
                -pedantic                       \
                -Wshadow

CFLAGS ?= -O2 -fomit-frame-pointer
Z_CFLAGS := $(CFLAGS) -fpic -DPIC -shared -pthread -D _GNU_SOURCE $(MY_CFLAGS) -DAST_MODULE=\"func_strsubst\" 
LIBS ?=
Z_LIBS := $(LIBS)

LINKFLAGS = -fpic -shared -Xlinker -x -pthread

BINARIES = func_strsubst.so

# Use make DESTDIR=/path/to/sysroot PREFIX=/usr/local if you want to
# install this module in a non-standard location.
DESTDIR ?=
PREFIX ?= /usr

.PHONY: all install clean

all: $(BINARIES)

%.so: %.o
	$(CC) $(LINKFLAGS) -o $@ $< $(Z_LIBS)

%.o: %.c
	$(CC) $(Z_CFLAGS) -c -o $@ $<

install: $(BINARIES)
	install -d $(DESTDIR)$(PREFIX)/lib/asterisk/modules
	install -m 755 $(BINARIES) $(DESTDIR)$(PREFIX)/lib/asterisk/modules

clean:
	rm -f $(BINARIES) *.o

# $Revision$
# $Date$
#
# set PATH and CC to include cross compiling tools for cross compilation, e.g.
# export PATH=/path/to/toolchain/powerpc-linux-gnu/bin:$PATH
# export CC=gcc

# you can override default options by calling make OPTIONS=...
CFLAGS ?= -O2
Z_CFLAGS := $(CFLAGS) -D _GNU_SOURCE -Wall -Wmissing-prototypes -Wstrict-prototypes
LIBS ?=
Z_LIBS := $(LIBS) -lpthread -lsqlite

BINARY = res_sqlite.so
OBJECTS = res_sqlite.o

DOXYGEN = doxygen
DOXYGEN_CONF = Doxyfile

# use make DESTDIR=/path/to/sysroot PREFIX=/usr/local if you want to
# install this module in a non-standard location
DESTDIR ?=
PREFIX ?= /usr

.PHONY: doc install clean-doc clean

$(BINARY): $(OBJECTS)
	$(CC) -shared -Xlinker -x -o $(BINARY) $(OBJECTS) $(Z_LIBS)

%.o: %.c
	$(CC) $(Z_CFLAGS) -c -o $@ $<

doc: $(DOXYGEN_CONF)
	$(DOXYGEN) $(DOXYGEN_CONF)

install: $(BINARY)
	install -d $(DESTDIR)$(PREFIX)/lib/asterisk/modules
	install -m 755 $(BINARY) $(DESTDIR)$(PREFIX)/lib/asterisk/modules

clean-obj:
	rm -f $(BINARY) $(OBJECTS)

clean: clean-obj
	rm -rf doc


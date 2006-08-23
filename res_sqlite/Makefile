# set PATH and CC to include cross compiling tools for cross compilation, e.g.
# export PATH=/path/to/toolchain/powerpc-linux-gnu/bin:$PATH
# export CC=gcc

# you can override default options by calling make OPTIONS=...
OPTIONS ?= -O2
CFLAGS = -D _GNU_SOURCE -Wall -Wmissing-prototypes -Wstrict-prototypes $(OPTIONS)
LIBS = -lpthread -lsqlite

BINARY = res_sqlite.so
OBJECTS = res_sqlite.o

DOXYGEN = doxygen
DOXYGEN_CONF = Doxyfile

# use make INSTALL_PREFIX=/path/to/sysroot PREFIX=/usr/local if you want to
# install this module in a non-standard location
INSTALL_PREFIX ?=
PREFIX ?= /usr

.PHONY: doc install clean-doc clean

$(BINARY): $(OBJECTS)
	$(CC) -shared -Xlinker -x -o $(BINARY) $(OBJECTS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

doc: $(DOXYGEN_CONF)
	$(DOXYGEN) $(DOXYGEN_CONF)

install: $(BINARY)
	install -m 755 $(BINARY) $(INSTALL_PREFIX)$(PREFIX)/lib/asterisk/modules

clean-obj:
	rm -f $(BINARY) $(OBJECTS)

clean: clean-obj
	rm -rf doc

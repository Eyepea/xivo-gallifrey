# $Revision$
# $Date$
#
# set PATH and CC to include cross compiling tools for cross compilation, e.g.
# export PATH=/path/to/toolchain/powerpc-linux-gnu/bin:$PATH
# export CC=gcc

MY_CFLAGS =	-fstrict-aliasing

# Relaxed Jedi Master Linux Compilation options:
MY_CFLAGS +=	-Wall				\
		-Wextra				\
		-Wundef 			\
		-Wstrict-prototypes		\
		-Wmissing-prototypes		\
		-Wmissing-declarations		\
		-Wcast-qual			\
		-Wcast-align			\
		-Wwrite-strings 		\
		-Wnested-externs		\
		-Wshadow			\
		-Winit-self			\
		-Wstrict-aliasing		\
		-Wsign-conversion

MY_CFLAGS +=	-D_GNU_SOURCE -DPIC -fPIC -pthread

CFLAGS ?= -O2

Z_CFLAGS = $(CFLAGS) $(MY_CFLAGS) -DAST_MODULE=\"res_config_sqlite\" $(OPTIONS)


MY_LDFLAGS := -shared -fPIC -pthread -lsqlite

ADD_LDFLAGS ?= 

LDFLAGS ?= -Xlinker -x

Z_LIBS := $(MY_LDFLAGS) $(LDFLAGS) $(ADD_LDFLAGS)


BINARY = res_config_sqlite.so
OBJECTS = res_config_sqlite.o

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
	./gwr.py $< --- $(CC) $(Z_CFLAGS) -c -o $@ $<

doc: $(DOXYGEN_CONF)
	$(DOXYGEN) $(DOXYGEN_CONF)

install: $(BINARY)
	install -d $(DESTDIR)$(PREFIX)/lib/asterisk/modules
	install -m 755 $(BINARY) $(DESTDIR)$(PREFIX)/lib/asterisk/modules

clean-obj:
	rm -f $(BINARY) $(OBJECTS)

clean: clean-obj
	rm -rf doc


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

MY_CFLAGS +=	-D_GNU_SOURCE -DPIC -fPIC -pthread -DAST_MODULE=\"ami_aoriginate\"

CFLAGS ?= -O2

Z_CFLAGS = $(CFLAGS) $(MY_CFLAGS) $(OPTIONS)


MY_LDFLAGS := -shared -fPIC -pthread -lpthread

ADD_LDFLAGS ?= 

LDFLAGS ?= -Xlinker -x

Z_LIBS := $(MY_LDFLAGS) $(LDFLAGS) $(ADD_LDFLAGS)


BINARIES = ami_aoriginate.so

# Use make DESTDIR=/path/to/sysroot PREFIX=/usr/local if you want to
# install this module in a non-standard location.
DESTDIR ?=
PREFIX ?= /usr

.PHONY: all install clean

all: $(BINARIES)

%.so: %.o
	$(CC) -o $@ $< $(Z_LIBS)

%.o: %.c
	./gwr.py $< --- $(CC) $(Z_CFLAGS) -c -o $@ $<

install: $(BINARIES)
	install -d $(DESTDIR)$(PREFIX)/lib/asterisk/modules
	install -m 755 $(BINARIES) $(DESTDIR)$(PREFIX)/lib/asterisk/modules

clean:
	rm -f $(BINARIES) *.o

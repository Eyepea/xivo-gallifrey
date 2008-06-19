# ifacenames - get logical interface names from /etc/network/interfaces
#
# $Date$
# $Revision$

override CC := gcc

objects = ifacenames.o

binary = ifacenames

MY_LDFLAGS =

ADD_LDFLAGS?=

# Relaxed Jedi Master Linux Compilation options:
MY_CFLAGS =	-std=gnu99			\
		-Wundef 			\
		-Wstrict-prototypes		\
		-Wmissing-prototypes		\
		-Wall				\
		-W				\
		-Wcast-qual			\
		-Wcast-align			\
		-Wwrite-strings 		\
		-Wnested-externs		\
		-Wmissing-declarations		\
		-pedantic			\
		-Wshadow

CFLAGS?=-O2

CFLAGS+=-fomit-frame-pointer

ADD_CFLAGS?=

DESTDIR?=
PREFIX?=/usr

$(binary) : $(objects)
	$(CC) $(MY_LDFLAGS) $(LDFLAGS) $(ADD_LDFLAGS) -o $(binary) $(objects)

$(objects): %.o: %.c
	$(CC) -c $(MY_CFLAGS) $(CFLAGS) $(ADD_CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f *~ $(objects) $(binary)

#.PHONY: indent
#indent:
#	indent -kr -i8 *.c *.h

.PHONY: install
install: $(binary)
	install -m 755 $(binary) $(DESTDIR)$(PREFIX)/bin

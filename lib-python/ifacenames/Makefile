# ifacenames - get logical interface names from /etc/network/interfaces
#
# $Date$
# $Revision$

override CC := gcc

objects = ifacenames.o

binary = ifacenames

MY_LDFLAGS =

ADD_LDFLAGS ?=

MY_CFLAGS =	-std=c99

MY_CFLAGS +=	-fstrict-aliasing

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
		-pedantic

# for gcc 4.3+
#MY_CFLAGS +=	-Wlogical-op			\
#		-Wstrict-overflow

CFLAGS ?=	-O2 -fomit-frame-pointer

ADD_CFLAGS ?=

DESTDIR ?=
PREFIX ?= /usr

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
	install -d -m 755 $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(binary) $(DESTDIR)$(PREFIX)/bin/$(binary)

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

MY_CFLAGS +=	-D_GNU_SOURCE -DPIC -fPIC -pthread -DAST_MODULE=\"app_nv_faxdetect\"

CFLAGS ?= -O2

Z_CFLAGS = $(CFLAGS) $(MY_CFLAGS) $(OPTIONS)


MY_LDFLAGS := -shared -fPIC -pthread

ADD_LDFLAGS ?= 

LDFLAGS ?= -Xlinker -x

Z_LIBS := $(MY_LDFLAGS) $(LDFLAGS) $(ADD_LDFLAGS)


BINARY = app_nv_faxdetect.so
OBJECTS = app_nv_faxdetect.o

DESTDIR ?=
PREFIX ?= /usr

.PHONY: install clean

$(BINARY): $(OBJECTS)
	$(CC) -o $(BINARY) $(OBJECTS) $(Z_LIBS)

%.o: %.c
	./gwr.py $< --- $(CC) $(Z_CFLAGS) -c -o $@ $<

install: $(BINARY)
	install -d $(DESTDIR)$(PREFIX)/lib/asterisk/modules
	install -m 755 $(BINARY) $(DESTDIR)$(PREFIX)/lib/asterisk/modules

clean:
	rm -f $(BINARY) $(OBJECTS)


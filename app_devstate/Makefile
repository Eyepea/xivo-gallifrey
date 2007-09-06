CFLAGS ?= -O2 -fPIC
Z_CFLAGS := $(CFLAGS) -D _GNU_SOURCE -Wall -Wmissing-prototypes -Wstrict-prototypes
LIBS ?=
Z_LIBS := $(LIBS) -lpthread -lsqlite

BINARY = app_devstate.so
OBJECTS = app_devstate.o

DESTDIR ?=
PREFIX ?= /usr

.PHONY: install clean

$(BINARY): $(OBJECTS)
	$(CC) -shared -Xlinker -x -o $(BINARY) $(OBJECTS) $(Z_LIBS)

%.o: %.c
	$(CC) $(Z_CFLAGS) -c -o $@ $<

install: $(BINARY)
	install -d $(DESTDIR)$(PREFIX)/lib/asterisk/modules
	install -m 755 $(BINARY) $(DESTDIR)$(PREFIX)/lib/asterisk/modules

clean:
	rm -f $(BINARY) $(OBJECTS)

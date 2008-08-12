CFLAGS ?= -O2 -fPIC
Z_CFLAGS := $(CFLAGS) -D _GNU_SOURCE -Wall -Wmissing-prototypes -Wstrict-prototypes -DAST_MODULE=\"app_nv_faxdetect\"
LIBS ?=
Z_LIBS := $(LIBS) -lpthread

BINARY = app_nv_faxdetect.so
OBJECTS = app_nv_faxdetect.o

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


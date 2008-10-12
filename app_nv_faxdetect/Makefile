# Relaxed Jedi Master Linux Compilation options:
MY_CFLAGS =     -Wundef                         \
                -Wstrict-prototypes             \
                -Wmissing-prototypes            \
                -Wall                           \
                -W                              \
                -Wcast-qual                     \
                -Wcast-align                    \
                -Wwrite-strings                 \
                -Wnested-externs                \
                -Wmissing-declarations          \
                -Wshadow

CFLAGS ?= -O2 -fPIC
Z_CFLAGS := $(CFLAGS) -D _GNU_SOURCE $(MY_CFLAGS) -Wall -Wmissing-prototypes -Wstrict-prototypes -DAST_MODULE=\"app_nv_faxdetect\"
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
	./gwr.py $< --- $(CC) $(Z_CFLAGS) -c -o $@ $<

install: $(BINARY)
	install -d $(DESTDIR)$(PREFIX)/lib/asterisk/modules
	install -m 755 $(BINARY) $(DESTDIR)$(PREFIX)/lib/asterisk/modules

clean:
	rm -f $(BINARY) $(OBJECTS)


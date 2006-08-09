PREFIX = /usr
INSTALL_PREFIX ?= /usr/cross/dists/xivo-i386/sysroot

CC = gcc
CFLAGS = -g -D _GNU_SOURCE -Wall -Wmissing-prototypes -Wstrict-prototypes
LIBS = -lpthread -lsqlite
OBJECTS = res_sqlite.o
HEADERS = res_sqlite.h

.PHONY: clean
.SILENT: rxim $(OBJECTS) clean

res_sqlite.so: $(OBJECTS)
	$(CC) -shared -Xlinker -x -o res_sqlite.so res_sqlite.o $(LIBS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

install: res_sqlite.so
	install -m 755 res_sqlite.so $(INSTALL_PREFIX)$(PREFIX)/lib/asterisk/modules

clean:
	rm -f res_sqlite.so *.o

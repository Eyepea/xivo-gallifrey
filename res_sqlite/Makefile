CC = gcc
CFLAGS = -g -Wall -Wmissing-prototypes -Wstrict-prototypes
OBJECTS = res_sqlite.o

.PHONY: clean
.SILENT: rxim $(OBJECTS) clean

res_sqlite.so: $(OBJECTS)
	$(CC) -shared -o res_sqlite.so res_sqlite.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f res_sqlite.so *.o

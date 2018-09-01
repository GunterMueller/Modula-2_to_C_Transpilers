CDEBUG = -g
CFLAGS = $(CDEBUG) -Wall

all: m2c

m2c: m2c.c version.h ../include/missing.c
	$(CC) $(CFLAGS) m2c.c -o m2c

clean:
	rm -f m2c || true

distclean: clean

dist: clean

web: clean

test: m2c

install: build

uninstall:

upgrade:

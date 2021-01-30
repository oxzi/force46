CC      ?= gcc
CFLAGS   = -fPIC -shared -ldl -Wall -Wextra -pedantic -Wno-discarded-qualifiers
DESTDIR ?= .

.PHONY: all clean

all: force4 force6

force4: force4.so
	echo "#!/bin/sh" > force4
	echo "LD_PRELOAD=$(realpath $(DESTDIR))/force4.so \"\$$@\"" >> force4
	chmod +x force4

force6: force6.so
	echo "#!/bin/sh" > force6
	echo "LD_PRELOAD=$(realpath $(DESTDIR))/force6.so \"\$$@\"" >> force6
	chmod +x force6

force4.so: force46.c
	$(CC) $(CFLAGS) -DAF_INET_AVOID=AF_INET6 -DAF_INET_WANT=AF_INET -o force4.so force46.c

force6.so: force46.c
	$(CC) $(CFLAGS) -DAF_INET_AVOID=AF_INET -DAF_INET_WANT=AF_INET6 -o force6.so force46.c

clean:
	rm force? force?.so

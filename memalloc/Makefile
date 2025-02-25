# CFLAGS=-Wall -Wextra -Wswitch-enum -Wmissing-prototypes -std=c11 -pedantic -ggdb
CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb
LIBS=

all: memalloc

memalloc: main.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	@rm -rf memalloc

.PHONY: all clean

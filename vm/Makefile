CFLAGS=-Wall -Wextra -Wswitch-enum -Wmissing-prototypes -std=c11 -pedantic -ggdb
# CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb
LIBS=

all: vm

vm: vm.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	@rm -rf vm

.PHONY: all clean

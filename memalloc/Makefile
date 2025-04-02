CFLAGS=-Wall -Wextra -Wswitch-enum -Wmissing-prototypes -std=c11 -pedantic -ggdb
# CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb
LIBS=

all: memalloc

memalloc: memalloc.o
	@mv $< $@

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	@rm -rf memalloc *.o *.out

.PHONY: all clean

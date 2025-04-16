CFLAGS=-Wall -Wextra -Wswitch-enum -Wmissing-prototypes -std=gnu11 -pedantic -ggdb
LIBS=

BUILD=build

all: $(BUILD)/bper $(BUILD)/bpe2dot

$(BUILD)/%: %.c bpe.c
	@if [ ! -d "$(dir $@)" ]; then mkdir -p $(dir $@); fi
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	@rm -rf $(BUILD)

.PHONY: all clean

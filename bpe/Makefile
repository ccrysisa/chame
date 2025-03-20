CFLAGS=-Wall -Wextra -Wswitch-enum -Wmissing-prototypes -pedantic -ggdb
LIBS=

TARGET=bpe

all: $(TARGET)

$(TARGET): $(TARGET).o
	@mv $< $@

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	@rm -rf *.o *.out $(TARGET)

.PHONY: all clean

CC = clang
CFLAGS = -Wall -Wextra -std=c23 -g -O0

SOURCES := $(wildcard day*.c)
DAYS := $(SOURCES:.c=)

.PHONY: all clean

all: $(DAYS)

$(DAYS): %: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(DAYS)

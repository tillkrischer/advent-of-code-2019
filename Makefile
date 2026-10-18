CC = clang
CFLAGS = -Wall -Wextra -std=c23

DAYS = day1 day2 day3 day4 day5 day6 day7 day8 day9 day10 day11 day12

.PHONY: all clean $(DAYS)

all: $(DAYS)

$(DAYS): %: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(DAYS)

CC      = gcc
CFLAGS  = -Wall -Wextra -g -I../src
LDFLAGS = -lm

SRC_ENTROPY = ../src/entropy.c

all: test_entropy

test_entropy: test_entropy.c $(SRC_ENTROPY)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

run: all
	@echo "--- running entropy tests ---"
	./test_entropy

clean:
	rm -f test_entropy

.PHONY: all run clean

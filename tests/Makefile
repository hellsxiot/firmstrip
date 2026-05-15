CC      = gcc
CFLAGS  = -Wall -Wextra -g -I../src
SRCDIR  = ../src

TESTS = test_entropy test_string_table test_file_classifier

all: $(TESTS)

test_entropy: test_entropy.c $(SRCDIR)/entropy.c
	$(CC) $(CFLAGS) -o $@ $^ -lm

test_string_table: test_string_table.c $(SRCDIR)/string_table.c
	$(CC) $(CFLAGS) -o $@ $^

test_file_classifier: test_file_classifier.c $(SRCDIR)/file_classifier.c
	$(CC) $(CFLAGS) -o $@ $^

run: all
	@for t in $(TESTS); do echo "--- $$t ---"; ./$$t; done

clean:
	rm -f $(TESTS)

.PHONY: all run clean

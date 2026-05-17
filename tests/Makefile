CC      = gcc
CFLAGS  = -Wall -Wextra -g -I../src
SRCDIR  = ../src

TESTS = test_entropy \
        test_string_table \
        test_file_classifier \
        test_path_filter \
        test_checksum \
        test_symlink_tracker \
        test_hardlink_map \
        test_permission_checker \
        test_xattr_reader \
        test_magic_bytes

all: $(TESTS)

test_entropy: test_entropy.c $(SRCDIR)/entropy.c
	$(CC) $(CFLAGS) -o $@ $^ -lm

test_string_table: test_string_table.c $(SRCDIR)/string_table.c
	$(CC) $(CFLAGS) -o $@ $^

test_file_classifier: test_file_classifier.c $(SRCDIR)/file_classifier.c
	$(CC) $(CFLAGS) -o $@ $^

test_path_filter: test_path_filter.c $(SRCDIR)/path_filter.c
	$(CC) $(CFLAGS) -o $@ $^

test_checksum: test_checksum.c $(SRCDIR)/checksum.c
	$(CC) $(CFLAGS) -o $@ $^

test_symlink_tracker: test_symlink_tracker.c $(SRCDIR)/symlink_tracker.c
	$(CC) $(CFLAGS) -o $@ $^

test_hardlink_map: test_hardlink_map.c $(SRCDIR)/hardlink_map.c
	$(CC) $(CFLAGS) -o $@ $^

test_permission_checker: test_permission_checker.c $(SRCDIR)/permission_checker.c
	$(CC) $(CFLAGS) -o $@ $^

test_xattr_reader: test_xattr_reader.c $(SRCDIR)/xattr_reader.c
	$(CC) $(CFLAGS) -o $@ $^

test_magic_bytes: test_magic_bytes.c $(SRCDIR)/magic_bytes.c
	$(CC) $(CFLAGS) -o $@ $^

run: all
	@for t in $(TESTS); do echo "--- $$t ---"; ./$$t; done

clean:
	rm -f $(TESTS)

.PHONY: all run clean

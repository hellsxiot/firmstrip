CC      = gcc
CFLAGS  = -Wall -Wextra -g -I../src
LDFLAGS =

SRC_DIR = ../src

TESTS = test_entropy test_string_table test_file_classifier \
        test_path_filter test_checksum test_symlink_tracker \
        test_hardlink_map test_xattr_reader

all: $(TESTS)

test_entropy: test_entropy.c $(SRC_DIR)/entropy.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lm

test_string_table: test_string_table.c $(SRC_DIR)/string_table.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_file_classifier: test_file_classifier.c $(SRC_DIR)/file_classifier.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_path_filter: test_path_filter.c $(SRC_DIR)/path_filter.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_checksum: test_checksum.c $(SRC_DIR)/checksum.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_symlink_tracker: test_symlink_tracker.c $(SRC_DIR)/symlink_tracker.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_hardlink_map: test_hardlink_map.c $(SRC_DIR)/hardlink_map.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_xattr_reader: test_xattr_reader.c $(SRC_DIR)/xattr_reader.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

run: $(TESTS)
	@for t in $(TESTS); do echo "--- $$t ---"; ./$$t; done

clean:
	rm -f $(TESTS)

.PHONY: all run clean

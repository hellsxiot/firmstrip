CC      = gcc
CFLAGS  = -Wall -Wextra -g -I../src
LDFLAGS =

SRC_DIR = ../src

TESTS = \
    test_entropy \
    test_string_table \
    test_file_classifier \
    test_path_filter \
    test_checksum \
    test_symlink_tracker \
    test_hardlink_map \
    test_permission_checker \
    test_xattr_reader \
    test_magic_bytes \
    test_inode_stat \
    test_owner_info

all: $(TESTS)

test_entropy: test_entropy.c $(SRC_DIR)/entropy.c
	$(CC) $(CFLAGS) -o $@ $^ -lm

test_string_table: test_string_table.c $(SRC_DIR)/string_table.c
	$(CC) $(CFLAGS) -o $@ $^

test_file_classifier: test_file_classifier.c $(SRC_DIR)/file_classifier.c
	$(CC) $(CFLAGS) -o $@ $^

test_path_filter: test_path_filter.c $(SRC_DIR)/path_filter.c
	$(CC) $(CFLAGS) -o $@ $^

test_checksum: test_checksum.c $(SRC_DIR)/checksum.c
	$(CC) $(CFLAGS) -o $@ $^

test_symlink_tracker: test_symlink_tracker.c $(SRC_DIR)/symlink_tracker.c
	$(CC) $(CFLAGS) -o $@ $^

test_hardlink_map: test_hardlink_map.c $(SRC_DIR)/hardlink_map.c
	$(CC) $(CFLAGS) -o $@ $^

test_permission_checker: test_permission_checker.c $(SRC_DIR)/permission_checker.c
	$(CC) $(CFLAGS) -o $@ $^

test_xattr_reader: test_xattr_reader.c $(SRC_DIR)/xattr_reader.c
	$(CC) $(CFLAGS) -o $@ $^

test_magic_bytes: test_magic_bytes.c $(SRC_DIR)/magic_bytes.c
	$(CC) $(CFLAGS) -o $@ $^

test_inode_stat: test_inode_stat.c $(SRC_DIR)/inode_stat.c
	$(CC) $(CFLAGS) -o $@ $^

test_owner_info: test_owner_info.c $(SRC_DIR)/owner_info.c
	$(CC) $(CFLAGS) -o $@ $^

run: all
	@for t in $(TESTS); do echo "--- $$t ---"; ./$$t; echo; done

clean:
	rm -f $(TESTS)

.PHONY: all run clean

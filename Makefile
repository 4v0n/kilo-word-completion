CFLAGS = -Wall -Wextra -pedantic -std=c99 -g -Iinclude
SRCS = src/terminal.c \
      src/input.c \
      src/output.c \
      src/io.c \
      src/row_operations.c \
      src/editor_operations.c \
      src/search.c \
      src/syntax_highlighting.c \
      src/trie.c

TESTS = tests/test_trie.c

main: $(SRCS) src/main.c
	$(CC) $(CFLAGS) $^ -o build/kilo

# install CUnit with - sudo apt-get install libcunit1 libcunit1-doc libcunit1-dev
test: $(SRCS) $(TESTS) tests/test_runner.c
	$(CC) $(CFLAGS) $^ -o build/tests -I. -lcunit

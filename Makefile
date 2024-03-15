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

.PHONY: main test

main: $(SRCS) src/main.c | build
	$(CC) $(CFLAGS) $^ -o build/kilo

test: $(SRCS) $(TESTS) tests/test_runner.c | build
	$(CC) $(CFLAGS) $^ -o build/tests -I. -lcunit

build:
	mkdir -p build

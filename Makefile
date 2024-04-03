CFLAGS = -Wall -Wextra -pedantic -std=c99 -g -Iinclude

SRCS = src/terminal.c \
      src/input.c \
      src/output.c \
      src/io.c \
      src/row_operations.c \
      src/editor_operations.c \
      src/search.c \
      src/syntax_highlighting.c \
      src/list.c \
      src/word_completion.c \
      src/prefix_matcher.c \
      src/trie.c \
      src/levenshtein.c \
      src/levenshtein_matcher.c \
      src/util.c

TESTS = tests/test_trie.c \
        tests/test_list.c \
        tests/test_levenshtein.c

.PHONY: main test clean

main: $(SRCS) src/main.c | build
	$(CC) $(CFLAGS) $^ -o build/kilo

tests: $(SRCS) $(TESTS) tests/test_runner.c | build
	$(CC) $(CFLAGS) $^ -o build/tests -I. -lcunit

build:
	mkdir -p build

clean:
	rm -f build/tests build/kilo

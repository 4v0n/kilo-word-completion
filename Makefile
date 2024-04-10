CFLAGS_COMMON = -Wall -Wextra -pedantic -std=c99 -Iinclude
CFLAGS_DEBUG = $(CFLAGS_COMMON) -g
CFLAGS_RELEASE = $(CFLAGS_COMMON)

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
      src/language_matcher.c \
      src/word_completion_visualisation.c \
      src/util.c

TESTS = tests/test_trie.c \
        tests/test_list.c \
        tests/test_levenshtein.c

.PHONY: main debug test clean

main: $(SRCS) src/main.c | build
	$(CC) $(CFLAGS_RELEASE) $^ -o build/kilo

debug: $(SRCS) src/main.c | build
	$(CC) $(CFLAGS_DEBUG) $^ -o build/kilo_debug

# Requires CUnit - install using: sudo apt-get install libcunit1 libcunit1-doc libcunit1-dev
tests: $(SRCS) $(TESTS) tests/test_runner.c | build
	$(CC) $(CFLAGS_DEBUG) $^ -o build/tests -I. -lcunit

build:
	mkdir -p build

clean:
	rm -f build/tests build/kilo build/kilo_debug

#include <ctype.h>
#include <levenshtein.h>
#include <levenshtein_matcher.h>
#include <list.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <trie.h>
#include <word_completion.h>

TrieNode *root;

struct LevenshteinSuggestion {
  char word[100];
  int distance;
} LevenshteinSuggestion;

int compareLevenshteinSuggestions(const Node *a, const Node *b) {
  const struct LevenshteinSuggestion *sA =
      (const struct LevenshteinSuggestion *)a->data;
  const struct LevenshteinSuggestion *sB =
      (const struct LevenshteinSuggestion *)b->data;
  return sA->distance - sB->distance;
}

void findCompletions(TrieNode *node, const char *prefix, char *currentWord,
                     int level, List *suggestions, int* suggestionCount) {

  if (!node || *suggestionCount >= MAX_LEVENSHTEIN_SEARCH)
    return;

  if (level >= 99) {
    return;
  }

  if (node->isEndOfWord) {
    currentWord[level] = '\0';
    int distance = getLevenshteinDistance(prefix, currentWord);

    if (*suggestionCount < MAX_SUGGESTIONS) {
      struct LevenshteinSuggestion *suggestion =
          malloc(sizeof(LevenshteinSuggestion));

      strcpy(suggestion->word, currentWord);
      suggestion->distance = distance;
      addElement(suggestions, suggestion, sizeof(*suggestion));
      (*suggestionCount)++;
    }
  }

  for (int i = 0; i < ALPHABET_SIZE && *suggestionCount < MAX_SUGGESTIONS; i++) {
    if (node->children[i]) {
      currentWord[level] = 'a' + i;
      findCompletions(node->children[i], prefix, currentWord, level + 1,
                      suggestions, suggestionCount);
    }
  }
}

void toLowerCase(char *str) {
  if (str) {
    while (*str) {
      *str = tolower((unsigned char)*str);
      str++;
    }
  }
}

List *lmGetSuggestions(const char *word) {
  char prefix[MAX_PREFIX_LENGTH + 1];
  strncpy(prefix, word, sizeof(prefix));
  prefix[sizeof(prefix) - 1] = '\0';
  toLowerCase(prefix);

  List *suggestions = createList();
  char currentWord[100];
  int numSuggestions = 0;
  findCompletions(root, prefix, currentWord, 0, suggestions, &numSuggestions);

  sortList(suggestions, compareLevenshteinSuggestions);

  List *results = createList();
  Node *current = suggestions->head;
  for (int i = 0; i < MAX_SUGGESTIONS && current != NULL; i++) {
    const struct LevenshteinSuggestion *suggestion =
        (const struct LevenshteinSuggestion *)current->data;
    addElement(results, suggestion->word, strlen(suggestion->word) + 1);
    current = current->next;
  }

  freeList(suggestions);
  return results;
}

bool initLM() {
  root = getNode();
  FILE *file = fopen(WORDS_PATH, "r");

  if (file == NULL) {
    return NULL;
  }

  char buffer[MAX_LINE_LENGTH];
  while (fgets(buffer, MAX_LINE_LENGTH, file)) {
    char *word = malloc(MAX_LINE_LENGTH * sizeof(char));
    int count;

    // Check if the word and count are successfully parsed
    if (sscanf(buffer, "%[^,],%d", word, &count) == 2) {
      insert(root, word, count);
      free(word);
    } else {
      free(word);
    }
  }

  return true;
}

void destroyLM() { freeTrie(root); }
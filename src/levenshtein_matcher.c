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
  char *word;
  int distance;
} LevenshteinSuggestion;

int compareLevenshteinSuggestions(const Node *a, const Node *b) {
  const struct LevenshteinSuggestion *sA =
      (const struct LevenshteinSuggestion *)a->data;
  const struct LevenshteinSuggestion *sB =
      (const struct LevenshteinSuggestion *)b->data;
  return sA->distance - sB->distance;
}

void dfsLevenshtein(TrieNode *root, List *suggestions, int *count, char *currentWord,
         int depth, char *prefix) {
  if (!root) {
    return;
  }

  // word hit
  if (root->isEndOfWord) {
    currentWord[depth] = '\0';

    struct LevenshteinSuggestion suggestion;
    suggestion.word = (char *)malloc(strlen(currentWord) + 1);
    strcpy(suggestion.word, currentWord);
    suggestion.distance = getLevenshteinDistance(prefix, currentWord);

    addElement(suggestions, &suggestion, sizeof(suggestion));

    (*count)++;
  }

  // visit all children nodes
  for (int i = 0; i < ALPHABET_SIZE; i++) {
    if (root->children[i]) {
      currentWord[depth] = 'a' + i;  // append current char to word
      currentWord[depth + 1] = '\0'; // null terminate string
      dfs(root->children[i], suggestions, count, currentWord, depth + 1, prefix);
      currentWord[depth] = '\0'; // remove last char
    }
  }
}

List *lmGetSuggestions(const char *word) {
  char lowerPrefix[MAX_PREFIX_LENGTH + 1] = {0};

  // Convert prefix to lowercase
  for (int i = 0; word[i] != '\0'; i++) {
    lowerPrefix[i] = tolower(word[i]);
  }

  TrieNode *leaf = getTrieLeaf(root, lowerPrefix);
  if (!leaf)
    return NULL;

  List suggestions = *createList();
  int count = 0;
  char currentWord[100] = {0};

  // Use the lowercase prefix
  strncpy(currentWord, lowerPrefix, strlen(lowerPrefix));

  dfsLevenshtein(leaf, &suggestions, &count, currentWord, strlen(lowerPrefix), lowerPrefix);
  sortList(&suggestions, compareLevenshteinSuggestions);

  List *result = createList();
  int numSuggestions = (count < MAX_SUGGESTIONS) ? count : MAX_SUGGESTIONS;

  for (int i = 0; i < numSuggestions; i++) {
    Suggestion currentSuggestion =
        *(Suggestion *)getListElement(&suggestions, i);
    addElement(result, currentSuggestion.word,
               strlen(currentSuggestion.word) + 1);
  }

  return result;
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
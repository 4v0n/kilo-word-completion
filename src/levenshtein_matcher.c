#include <ctype.h>
#include <levenshtein.h>
#include <levenshtein_matcher.h>
#include <list.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <trie.h>
#include <word_completion.h>
#include <limits.h>

TrieNode *root;

typedef struct LevenshteinSuggestion {
  char *word;
  int distance;
} LevenshteinSuggestion;

int compareLevenshteinSuggestions(const Node *a, const Node *b) {
  LevenshteinSuggestion *sA =
      (LevenshteinSuggestion *)a->data;
  LevenshteinSuggestion *sB =
      (LevenshteinSuggestion *)b->data;
  return sA->distance - sB->distance;
}

void dfsLevenshtein(TrieNode *root, List *suggestions, int *count, char *currentWord,
                    int depth, char *prefix) {
  if (!root || depth > MAX_PREFIX_LENGTH) {
    return;
  }

  currentWord[depth] = '\0';
  int currentDistance = getLevenshteinDistance(prefix, currentWord);

  if (currentDistance > MAX_PREFIX_LENGTH - depth) {
    return;
  }

  // Word hit
  if (root->isEndOfWord) {
    LevenshteinSuggestion suggestion;
    suggestion.word = (char *)malloc(strlen(currentWord) + 1);
    strcpy(suggestion.word, currentWord);
    suggestion.distance = currentDistance;

    addElement(suggestions, &suggestion, sizeof(suggestion));

    (*count)++;
  }

  // Visit all children nodes
  for (int i = 0; i < ALPHABET_SIZE; i++) {
    if (root->children[i]) {
      currentWord[depth] = 'a' + i; // append current char to word
      dfsLevenshtein(root->children[i], suggestions, count, currentWord, depth + 1, prefix);
      currentWord[depth] = '\0'; // remove last char for backtracking
    }
  }
}

char* getFirstHalf(const char* input) {
    int length = strlen(input);
    int halfLength = (length + 1) / 2; // +1 ensures a single char is copied entirely

    // Allocate memory for the new string (+1 for the null terminator)
    char* firstHalf = (char*)malloc(halfLength + 1);
    
    // Copy the first half of the input to the new string
    strncpy(firstHalf, input, halfLength);

    // Ensure the new string is null-terminated
    firstHalf[halfLength] = '\0';

    return firstHalf;
}

List *lmGetSuggestions(const char *word) {
  /*
    Due to the aditional complexity of the Levenshtein Distance calculation,
    this function is more of a hybrid between prefix and fuzzy matching.

    The first half of the prefix is used to navigate down a trie and the rest is fuzzy matched
  */

  char lowerPrefix[MAX_PREFIX_LENGTH + 1] = {0};

  // Convert prefix to lowercase
  for (int i = 0; word[i] != '\0'; i++) {
    lowerPrefix[i] = tolower(word[i]);
  }

  char *halfPrefix = getFirstHalf(lowerPrefix);

  TrieNode *leaf = getTrieLeaf(root, halfPrefix);
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

  free(halfPrefix);
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
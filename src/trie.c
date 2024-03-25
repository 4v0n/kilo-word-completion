#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <trie.h>
#include <word_completion.h>

// Initialises and returns the root node of a trie
TrieNode *getNode() {
  // allocate memory for trie
  TrieNode *parentNode = malloc(sizeof(TrieNode));

  // initialise trie
  if (parentNode) {
    parentNode->isEndOfWord = false;
    parentNode->weight = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++)
      parentNode->children[i] = NULL;
  }

  return parentNode;
}

// Inserts a word into the trie
void insert(TrieNode *root, const char *key, int weight) {
  TrieNode *pCrawl = root;

  if (key[0] == '\0') {
    return; // do not insert empty character
  }

  // crawl through trie
  while (*key) {
    int index;
    if (isupper(*key)) { // convert to lower case
      index = tolower(*key) - 'a';
    } else {
      index = *key - 'a';
    }

    // if not a lower case letter
    if (index < 0 || index >= ALPHABET_SIZE) {
      return;
    }

    if (!pCrawl->children[index])
      pCrawl->children[index] = getNode();
    pCrawl = pCrawl->children[index];
    key++;
  }

  pCrawl->isEndOfWord = true;
  pCrawl->weight = weight;
}

TrieNode *getTrieLeaf(TrieNode *root, const char *prefix) {
  if (*prefix == '\0') {
    return root; // reached end of prefix
  }

  int index = *prefix - 'a';
  if (index < 0 || index >= ALPHABET_SIZE || root->children[index] == NULL) {
    return NULL; // abort if out of bounds / not part of trie
  }

  return getTrieLeaf(root->children[index], prefix + 1);
}

void dfs(TrieNode *root, List *suggestions, int *count, char *currentWord,
         int depth) {
  if (!root)
    return;

  // word hit
  if (root->isEndOfWord) {
    currentWord[depth] = '\0';

    Suggestion suggestion;
    suggestion.word = (char *)malloc(strlen(currentWord) + 1);
    strcpy(suggestion.word, currentWord);
    suggestion.weight = root->weight;

    addElement(suggestions, &suggestion, sizeof(suggestion));

    (*count)++;
  }

  // visit all children nodes
  for (int i = 0; i < ALPHABET_SIZE; i++) {
    if (root->children[i]) {
      currentWord[depth] = 'a' + i;  // append current char to word
      currentWord[depth + 1] = '\0'; // null terminate string
      dfs(root->children[i], suggestions, count, currentWord, depth + 1);
      currentWord[depth] = '\0'; // remove last char
    }
  }
}

int compare(const void *a, const void *b) {
  Suggestion *suggestionA = (Suggestion *)a;
  Suggestion *suggestionB = (Suggestion *)b;
  return suggestionB->weight - suggestionA->weight;
}

List *getSuggestions(TrieNode *root, const char *prefix) {
  TrieNode *leaf = getTrieLeaf(root, prefix);
  if (!leaf)
    return NULL;

  List suggestions;
  initList(&suggestions);

  int count = 0;
  char currentWord[100] = {0};
  strncpy(currentWord, prefix, strlen(prefix));

  dfs(leaf, &suggestions, &count, currentWord, strlen(prefix));
  sortList(&suggestions, compare);

  List *result = malloc(sizeof(List));
  initList(result);

  int numSuggestions = (count < MAX_SUGGESTIONS) ? count : MAX_SUGGESTIONS;
  for (int i = 0; i < numSuggestions; i++) {
    Suggestion currentSuggestion = *(Suggestion *)getListElement(&suggestions, i);
    addElement(result, currentSuggestion.word, sizeof(currentSuggestion.word));
  }

  return result;
}

// Recursively free memory allocated for a trie
void freeTrie(TrieNode *root) {
  if (!root)
    return;

  for (int i = 0; i < ALPHABET_SIZE; i++) {
    if (root->children[i]) {
      freeTrie(root->children[i]);
    }
  }

  // Free the root node after all its children have been freed
  free(root);
}

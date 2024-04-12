/*
  This file defines the logic for a weighted trie structure
*/

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <trie.h>
#include <util.h>
#include <word_completion.h>

// Initialises and returns a node of a trie
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
// adapted from https://www.geeksforgeeks.org/trie-insert-and-search/
void insert(struct TrieNode *root, const char *key, int weight) {
  if (!isAlphabetWord(key) || (int)strlen(key) == 0) {
    return;
  }

  int length = strlen(key);
  int index;

  struct TrieNode *pCrawl = root;

  // navigate to relevant trie node
  for (int level = 0; level < length; level++) {
    char c = key[level];
    if (isupper(c)) {
      index = tolower(c) - 'a';
    } else {
      index = c - 'a';
    }

    if (!pCrawl->children[index]) {
      pCrawl->children[index] = getNode();
    }
    pCrawl = pCrawl->children[index];
  }

  // add data to node
  pCrawl->isEndOfWord = true;
  pCrawl->weight = weight;
}

// Returns the leaf of a trie
TrieNode *getTrieLeaf(TrieNode *root, const char *prefix) {
  if (prefix == NULL || *prefix == '\0') {
    return root; // reached end of prefix
  }

  int index = *prefix - 'a';
  if (index < 0 || index >= ALPHABET_SIZE || root->children[index] == NULL) {
    return NULL; // abort if out of bounds / not part of trie
  }

  return getTrieLeaf(root->children[index], prefix + 1);
}

// Uses DFS to return a list of words
void dfs(TrieNode *root, List *suggestions, int *count, char *currentWord,
         int depth) {

  if (!root) {
    return;
  }

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

int compare(const Node *a, const Node *b) {
  Suggestion *suggestionA = (Suggestion *)a->data;
  Suggestion *suggestionB = (Suggestion *)b->data;
  return suggestionB->weight - suggestionA->weight;
}

// Returns the first suggestions relevant to the prefix
List *getSuggestions(TrieNode *root, const char *prefix) {
  char lowerPrefix[MAX_PREFIX_LENGTH + 1] = {0};

  // Convert prefix to lowercase
  for (int i = 0; prefix[i] != '\0'; i++) {
    lowerPrefix[i] = tolower(prefix[i]);
  }

  // navigate to leaf from prefix
  TrieNode *leaf = getTrieLeaf(root, lowerPrefix);
  if (!leaf)
    return NULL;

  List *suggestions = createList();
  int count = 0;
  char currentWord[100] = {0};

  // Use the lowercase prefix
  memcpy(currentWord, lowerPrefix, strlen(lowerPrefix));
  currentWord[strlen(lowerPrefix)] = '\0';

  dfs(leaf, suggestions, &count, currentWord, strlen(lowerPrefix));
  sortList(suggestions, compare);

  List *result = createList();
  int numSuggestions = (count < MAX_SUGGESTIONS) ? count : MAX_SUGGESTIONS;

  for (int i = 0; i < numSuggestions; i++) {
    Suggestion *currentSuggestion =
        (Suggestion *)getListElement(suggestions, i);
    addElement(result, currentSuggestion->word,
               strlen(currentSuggestion->word) + 1);
    free(currentSuggestion->word);
  }

  for (int i = numSuggestions; i < suggestions->size; i++) {
    Suggestion *currentSuggestion = (Suggestion *)getListElement(suggestions, i);
    free(currentSuggestion->word);
  }

  freeList(suggestions);
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

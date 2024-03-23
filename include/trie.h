/*
This file prototypes a weighted trie structure and its relevant functions
*/

#ifndef WEIGHTED_TRIE_H
#define WEIGHTED_TRIE_H

#include <stdbool.h>

#define ALPHABET_SIZE 26
#define MAX_SUGGESTIONS 5
#define MAX_WORD_LENGTH 15

typedef struct {
  char suggestion[MAX_WORD_LENGTH];
  int weight;
} Suggestion;

typedef struct TrieNode {
  struct TrieNode *children[ALPHABET_SIZE];
  bool isEndOfWord;
  int weight;
} TrieNode;

TrieNode *getNode();
void insert(TrieNode *root, const char *key, int weight);
TrieNode *getTrieLeaf(TrieNode *root, const char *prefix);
Suggestion *getSuggestions(TrieNode *root, const char *prefix);
void freeTrie(TrieNode *root);

#endif
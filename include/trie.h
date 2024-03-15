/*
This file prototypes a weighted trie structure and its relevant functions
*/

#ifndef WEIGHTED_TRIE_H
#define WEIGHTED_TRIE_H

#include <stdbool.h>

#define ALPHABET_SIZE 26

typedef struct TrieNode {
  struct TrieNode *children[ALPHABET_SIZE];
  bool isEndOfWord;
  int weight;
} TrieNode;

TrieNode *getNode();
void insert(TrieNode *root, const char *key, int weight);
void freeTrie(TrieNode *root);

#endif
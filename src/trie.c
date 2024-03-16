#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <trie.h>

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

    if (!pCrawl->children[index])
      pCrawl->children[index] = getNode();
    pCrawl = pCrawl->children[index];
    key++;
  }

  pCrawl->isEndOfWord = true;
  pCrawl->weight = weight;
}

char** getSuggestions(TrieNode *root, const char *prefix) {
  return 0;
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

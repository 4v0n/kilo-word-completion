#include <ctype.h>
#include <stdbool.h>
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

void dfs(TrieNode* root, Suggestion suggestions[], int* count, char* currentWord, int depth) {
  if (!root) return;

  // word hit
  if (root->isEndOfWord) {
    currentWord[depth] = '\0';
    strcpy(suggestions[*count].suggestion, currentWord);
    suggestions[*count].weight = root->weight;
    (*count)++;
  }

  // visit all children nodes
  for (int i = 0; i < ALPHABET_SIZE; i++) {
    if (root->children[i]) {
      currentWord[depth] = 'a' + i; // append current char to word
      currentWord[depth + 1] = '\0'; // null terminate string
      dfs(root->children[i], suggestions, count, currentWord, depth + 1);
      currentWord[depth] = '\0'; // remove last char
    }
  }
}

int compare (const void* a, const void* b) {
  Suggestion *suggestionA = (Suggestion*)a;
  Suggestion *suggestionB = (Suggestion*)b;
  return suggestionB->weight - suggestionA->weight;
}

Suggestion *getSuggestions(TrieNode *root, const char *prefix) {
  TrieNode *leaf = getTrieLeaf(root, prefix);
  if (!leaf) return NULL;

  Suggestion suggestions[100];
  int count = 0;
  char currentWord[100] = {0};
  strncpy(currentWord, prefix, strlen(prefix));

  dfs(leaf, suggestions, &count, currentWord, strlen(prefix));

  qsort(suggestions, count, sizeof(Suggestion), compare);

  int numSuggestions = (count < MAX_SUGGESTIONS) ? count : MAX_SUGGESTIONS;

  char **result = malloc(numSuggestions * sizeof(char*));
  if (!result) return NULL;

  // TODO: restrict suggestions to first 5

  return suggestions;
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

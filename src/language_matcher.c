#include <list.h>
#include <stdbool.h>
#include <string.h>
#include <terminal.h>


/*** Custom lang Trie ***/

// Alternative implementation of existing trie to deal with all ASCII characters

typedef struct LangTrieNode {
  struct LangTrieNode *children[128]; // 128 ascii characters
  bool isEndOfWord;
} LangTrieNode;

// Initialises and returns the root node of a trie
LangTrieNode *LTgetNode() {
  // allocate memory for trie
  LangTrieNode *parentNode = malloc(sizeof(LangTrieNode));

  // initialise trie
  if (parentNode) {
    parentNode->isEndOfWord = false;
    for (int i = 0; i < 128; i++)
      parentNode->children[i] = NULL;
  }

  return parentNode;
}

// Inserts a word into the trie
// adapted from https://www.geeksforgeeks.org/trie-insert-and-search/
void LTinsert(struct LangTrieNode *root, const char *key) {
  int length = strlen(key);
  
  // Exclude the '|' character at the end of the keyword, if present
  if (length > 0 && key[length - 1] == '|') {
    length--;
  }

  if (length == 0) {
    return; // No valid keyword to insert
  }

  struct LangTrieNode *pCrawl = root;

  for (int level = 0; level < length; level++) {
    char c = key[level];
    int index = (unsigned char)c; // Directly use ASCII value for indexing

    // Ensure the index is within the bounds of the children array
    if (index < 0 || index >= 128) {
      continue; // Skip characters that are out of bounds
    }

    if (!pCrawl->children[index]) {
      pCrawl->children[index] = LTgetNode();
    }
    pCrawl = pCrawl->children[index];
  }

  pCrawl->isEndOfWord = true;
}

LangTrieNode *LTgetTrieLeaf(LangTrieNode *root, const char *prefix) {
  if (prefix == NULL || *prefix == '\0') {
    return root; // reached end of prefix
  }

  int index = *prefix - 'a';
  if (index < 0 || index >= 128 || root->children[index] == NULL) { 
    return NULL; // abort if out of bounds / not part of trie
  }

  return LTgetTrieLeaf(root->children[index], prefix + 1); 
}

// Recursively free memory allocated for a trie
void LTfreeTrie(LangTrieNode *root) {
  if (!root)
    return;

  for (int i = 0; i < 128; i++) {
    if (root->children[i]) {
      LTfreeTrie(root->children[i]);
    }
  }

  // Free the root node after all its children have been freed
  free(root);
}

/*** Language Matcher functions ***/

LangTrieNode *root;

List *langGetSuggestions(const char *word){
  List *result = createList();
  return result;
}

bool initLangM() {
  root = LTgetNode();

  struct editorConfig *E = getEditorConfig();

  if (!E->syntax) {
    return false;
  }

  char **keywords = E->syntax->keywords;

  for (int i = 0; keywords[i] != NULL; i++) {
    LTinsert(root, keywords[i]);
  }

  return true;
}

void destroyLangM() {
  LTfreeTrie(root);
}
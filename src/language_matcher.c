#include <data.h>
#include <language_matcher.h>
#include <list.h>
#include <output.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <terminal.h>
#include <word_completion.h>

/*** Custom lang Trie ***/

// Alternative implementation of existing trie to deal with all ASCII characters

typedef struct LangTrieNode {
  struct LangTrieNode *children[128]; // 128 ascii characters
  bool isEndOfWord;
  char *shortcutExpansion;
} LangTrieNode;

// Initialises and returns the root node of a trie
LangTrieNode *LTgetNode() {
  // allocate memory for trie
  LangTrieNode *parentNode = malloc(sizeof(LangTrieNode));

  // initialise trie
  if (parentNode) {
    parentNode->isEndOfWord = false;
    parentNode->shortcutExpansion = NULL;
    for (int i = 0; i < 128; i++)
      parentNode->children[i] = NULL;
  }

  return parentNode;
}

// Inserts a word into the trie
// adapted from https://www.geeksforgeeks.org/trie-insert-and-search/
void LTinsert(struct LangTrieNode *root, const char *key) {
  if (!key) {
    return;
  }

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

void LTinsertShortcut(LangTrieNode *root, const char *key,
                      const char *expansion) {
  if (key == NULL || expansion == NULL)
    return;
  int length = strlen(key);
  if (length == 0)
    return; // No valid keyword to insert

  struct LangTrieNode *pCrawl = root;
  for (int level = 0; level < length; level++) {
    int index = (unsigned char)key[level];
    if (index < 0 || index >= 128)
      continue;
    if (!pCrawl->children[index]) {
      pCrawl->children[index] = LTgetNode();
    }
    pCrawl = pCrawl->children[index];
  }

  if (pCrawl->shortcutExpansion) {
    return;
  }

  // Allocate memory for the shortcut expansion and copy the string
  int expansionLength =
      strlen(expansion); // Include space for the null terminator
  pCrawl->shortcutExpansion = (char *)malloc(expansionLength * sizeof(char));
  if (pCrawl->shortcutExpansion) {
    if (expansionLength > 0 && expansion[expansionLength - 1] == '|') {
      expansionLength--;
    }

    memcpy(pCrawl->shortcutExpansion, expansion, expansionLength);
  }
}

LangTrieNode *LTgetTrieLeaf(LangTrieNode *root, const char *prefix) {
  if (prefix == NULL || *prefix == '\0') {
    return root; // reached end of prefix
  }

  int index = *prefix;
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
  free(root->shortcutExpansion);
  free(root);
}

void LTdfs(LangTrieNode *root, List *suggestions, int *count, char *currentWord,
           int depth) {
  if (!root) {
    return;
  }

  currentWord[depth] = '\0';

  // Shortcut expansion
  if (root->shortcutExpansion != NULL) {
    Suggestion suggestion;
    int expansionLength = strlen(root->shortcutExpansion);
    suggestion.word = (char *)calloc(expansionLength + 1, sizeof(char));
    strcpy(suggestion.word, root->shortcutExpansion);
    suggestion.weight = 2;
    addElement(suggestions, &suggestion, sizeof(suggestion));
    (*count)++;
  }

  // Word hit
  if (root->isEndOfWord) {
    Suggestion suggestion;
    int currentWordLength = strlen(currentWord);
    suggestion.word = (char *)calloc(currentWordLength + 1, sizeof(char));
    strcpy(suggestion.word, currentWord);
    suggestion.weight = 1;
    addElement(suggestions, &suggestion, sizeof(suggestion));
    (*count)++;
  }

  // Visit all children nodes
  for (int i = 0; i < 128; i++) {
    if (root->children[i]) {
      currentWord[depth] = i; // Append current char to word
      LTdfs(root->children[i], suggestions, count, currentWord, depth + 1);
      currentWord[depth] = '\0'; // Reset the currentWord after recursion
    }
  }
}


/*** Language Matcher functions ***/

LangTrieNode *root;

int compareLT(const Node *a, const Node *b) {
  Suggestion *suggestionA = (Suggestion *)a->data;
  Suggestion *suggestionB = (Suggestion *)b->data;
  return suggestionB->weight - suggestionA->weight;
}

List *langGetSuggestions(const char *word) {
  LangTrieNode *leaf = LTgetTrieLeaf(root, word);
  if (!leaf)
    return NULL;

  List suggestions = *createList();
  int count = 0;
  char currentWord[100] = {0};

  // Use the lowercase prefix
  memcpy(currentWord, word, strlen(word));
  currentWord[strlen(word)] = '\0';

  LTdfs(leaf, &suggestions, &count, currentWord, strlen(word));
  sortList(&suggestions, compareLT);

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

pairing getLanguagePairing(const char *word) {
  struct editorConfig *E = getEditorConfig();
  pairing temp = {NULL, NULL, 0};

  if (!E->syntax->pairings) {
    return temp;
  }

  for (int i = 0; E->syntax->pairings[i].word != NULL; i++) {
    if (strcmp(E->syntax->pairings[i].word, word) == 0) {
      return E->syntax->pairings[i];
    }
  }

  return temp;
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

  // Insert shortcuts and their expansions into the trie
  if (!E->syntax->shortcuts) {
    return true;
  }

  for (int i = 0; E->syntax->shortcuts[i].key != NULL; i++) {
    LTinsert(root, E->syntax->shortcuts[i].value);
    LTinsertShortcut(root, E->syntax->shortcuts[i].key,
                     E->syntax->shortcuts[i].value);
  }

  return true;
}

void destroyLangM() { LTfreeTrie(root); }

void filterString(char *src, char *dest) {
    int j = 0;
    for (int i = 0; src[i] != '\0'; i++) {
        if (src[i] != '\t' && src[i] != '\n' && src[i] != '\r') {
            dest[j++] = src[i];
        }
    }
    dest[j] = '\0';
}

void visualiseLangM(struct abuf *ab) {
  struct engineConfig *EC = getEngineConfig();
  struct editorConfig *E = getEditorConfig();

  int lines = 0;
  while (lines < MAX_SUGGESTIONS) {
    int len = 0;
    // Allocate and initialize the memory to avoid using uninitialized memory.
    char *string = malloc((E->screencols * 2) * sizeof(char));
    if (string == NULL) {
      // Handle memory allocation failure, if needed.
      break;
    }
    string[0] = '\0'; // Initialize to an empty string for safe use with strcat.

    if (lines < EC->suggestions.size) {
      char *word = (char *)getListElement(&EC->suggestions, lines);

      strcat(string, word);
      strcat(string, " = ");

      for (int i = 0; i < (int)strlen(word); i++) {
        char tempStr[3] = {word[i], '\0'};
        strcat(string, tempStr);
        if (i != ((int)strlen(word) - 1)) {
          strcat(string, " -> ");
        }
      }

      pairing pair = getLanguagePairing(word);
      if (pair.pairing) {
        strcat(string, " | Pairing = ");
        char filtered[strlen(pair.pairing) + 1];
        filterString(pair.pairing, filtered);
        strcat(string, filtered);
      }

      len = strlen(string);
    }

    if (len > E->screencols) {
      len = E->screencols;
    }

    while (len < E->screencols) {
      strcat(string, " "); // Fill with spaces safely
      len++;
    }

    abAppend(ab, string, len);
    abAppend(ab, "\r\n", 2); // Newline
    lines++;
    free(string); // Ensure to free the allocated string to avoid memory leaks.
  }
}
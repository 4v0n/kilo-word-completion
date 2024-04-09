#include <output.h>
#include <prefix_matcher.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <terminal.h>
#include <trie.h>
#include <word_completion.h>
#include <word_completion_visualisation.h>

#define MAX_LINE_LENGTH 1024

TrieNode *root;

List *pmGetSuggestions(const char *word) {
  List *suggestions = getSuggestions(root, word);
  if (suggestions != NULL) {
    return suggestions;
  }

  return NULL;
}

bool initPM() {
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

void destroyPM() { freeTrie(root); }

void visualisePM(struct abuf *ab) {
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

      TrieNode *leafNode = getTrieLeaf(root, word);

      for (int i = 0; i < (int)strlen(word); i++) {
        char tempStr[3] = {word[i], '\0'};
        strcat(string, tempStr);
        if (i != ((int)strlen(word) - 1)) {
          strcat(string, " -> ");
        }
      }

      strcat(string, " | weight: ");
      char weightStr[20]; // Adjust size as needed
      sprintf(weightStr, "%d", leafNode->weight);
      strcat(string, weightStr);

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

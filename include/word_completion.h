/*
  This function prototypes functions and defines structures
  used by the word completion engine
*/

#ifndef WORD_COMPLETION_H
#define WORD_COMPLETION_H

#include <data.h>
#include <stdbool.h>
#include <list.h>

#define WORDS_PATH "../resources/words.csv"
#define WORD_COMPLETE_HELP "HELP: Ctrl-enter = complete | Ctrl-A/D = Select | Ctrl-W = Switch Mode | Ctrl-P = Toggle Visualisation"

#define MAX_PREFIX_LENGTH 10
#define MAX_SUGGESTIONS 5

enum completionModes { PREFIX = 0, FUZZY, LANGUAGE};
#define NUM_MODES 3

typedef struct Suggestion {
  char *word;
  int weight;
} Suggestion;

typedef struct engineConfig {
  bool isActive; // state of word completion engine
  char *prefix; // current prefix typed by the user
  List suggestions; // list of suggestions for the prefix
  int selection; // which suggestion is selected
  int mode; // which completion mode the engine is currently using
} engineConfig;

struct engineConfig *getEngineConfig();

void drawWordCompletionPromptRow(struct abuf *ab);

void toggleWordCompletionEngine();
void wordCompletionChooseCompletion(char c);
void completeWord();
void updateEC();

void initWordCompletionEngine();
void toggleCompletionMode();

void freeSuggestion(Suggestion *suggestion);

#endif
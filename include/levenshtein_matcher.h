/*
  This file prototypes functions relating to
  fuzzy match word completion
*/

#ifndef LEVENSHTEIN_MATCHER_H
#define LEVENSHTEIN_MATCHER_H

#include <data.h>
#include <list.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

List *lmGetSuggestions(const char *word);
bool initLM();
void destroyLM();
void visualiseLM(struct abuf *ab);

#endif
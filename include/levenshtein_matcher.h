#ifndef LEVENSHTEIN_MATCHER_H
#define LEVENSHTEIN_MATCHER_H

#include <list.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define MAX_LEVENSHTEIN_SEARCH 5000

List *lmGetSuggestions(const char *word);
bool initLM();
void destroyLM();

#endif
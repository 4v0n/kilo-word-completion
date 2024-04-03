#ifndef LEVENSHTEIN_MATCHER_H
#define LEVENSHTEIN_MATCHER_H

#include <list.h>
#include <stdbool.h>
#include <string.h>

List *lmGetSuggestions(const char *word);
bool initLM();
void destroyLM();

#endif
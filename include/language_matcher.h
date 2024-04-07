#ifndef LANGUAGE_MATCHER_H
#define LANGUAGE_MATCHER_H

#include <list.h>
#include <stdbool.h>
#include <string.h>

List *langGetSuggestions(const char *word);
bool initLangM();
void destroyLangM();

#endif
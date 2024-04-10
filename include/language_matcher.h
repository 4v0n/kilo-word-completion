/*
  This file prototypes for functions relating to
  Language word completion
*/

#ifndef LANGUAGE_MATCHER_H
#define LANGUAGE_MATCHER_H

#include <list.h>
#include <stdbool.h>
#include <string.h>
#include <data.h>

List *langGetSuggestions(const char *word);
pairing getLanguagePairing(const char *word);
bool initLangM();
void destroyLangM();
void visualiseLangM(struct abuf *ab);

#endif
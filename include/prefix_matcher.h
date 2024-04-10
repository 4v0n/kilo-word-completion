/*
  This file prototypes functions relating to 
  prefix match word completion
*/

#ifndef PREFIX_MATCHER_H
#define PREFIX_MATCHER_H

#include <data.h>
#include <list.h>
#include <stdbool.h>

List *pmGetSuggestions(const char *word);
bool initPM();
void destroyPM();
void visualisePM(struct abuf *ab);

#endif
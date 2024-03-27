#ifndef PREFIX_MATCHER_H
#define PREFIX_MATCHER_H

#include <list.h>
#include <stdbool.h>

List pmGetSuggestions(const char *word);
bool initPM();
void destroyPM();

#endif
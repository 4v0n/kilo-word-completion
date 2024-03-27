#ifndef PREFIX_MATCHER_H
#define PREFIX_MATCHER_H

#include <list.h>

List pmGetSuggestions(const char *word);
void initPM();
void destroyPM();

#endif
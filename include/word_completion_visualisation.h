/*
  This function prototypes functions relating to
  visualisation of the word completion engine
*/

#ifndef WORD_COMPLETION_VISUALISATION_H
#define WORD_COMPLETION_VISUALISATION_H

#include <stdbool.h>

bool isVisualisationActive();
void toggleVisualisation();
void drawVisualisation(struct abuf *ab);

#endif
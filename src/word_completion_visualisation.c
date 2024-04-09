#include <data.h>
#include <output.h>
#include <terminal.h>
#include <word_completion.h>
#include <word_completion_visualisation.h>

#define VISUALISATION_BOX_SIZE 6

bool isActive = false;

bool isVisualisationActive() {
  return isActive;
}

void toggleVisualisation() {
  struct engineConfig *EC = getEngineConfig();
  struct editorConfig *E = getEditorConfig();

  if (!EC->isActive && !isActive) {
    editorSetStatusMessage("Word Completion is currently inactive!");
    return;
  }

  if (E->screenrows < VISUALISATION_BOX_SIZE*2) {
    editorSetStatusMessage("Your current terminal is too small for visualisation!");
    return;
  }

  isActive = !isActive;

  if (isActive) {
    E->screenrows = E->screenrows - VISUALISATION_BOX_SIZE;
  } else {
    E->screenrows = E->screenrows + VISUALISATION_BOX_SIZE;
  }
}

void fillRows(struct abuf *ab) {
  struct editorConfig *E = getEditorConfig();
  for (int i = 0; i < VISUALISATION_BOX_SIZE; i++) {
    int len = 0;
    while (len < E->screencols) {
    abAppend(ab, " ", 1); // fill with spaces
    len++;
    }
  }
}

void drawVisualisation(struct abuf *ab) {
  abAppend(ab, "\x1b[44m", 5);
  
  struct engineConfig *EC = getEngineConfig();
  switch (EC->mode) {
    case PREFIX:
    case FUZZY:
    case LANGUAGE:

    default:
      fillRows(ab);
      editorSetStatusMessage("Failed to draw visualisation!");
      break;
  }

  abAppend(ab, "\x1b[m", 3); // switch back to normal
}

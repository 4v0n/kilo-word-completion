#include <data.h>
#include <language_matcher.h>
#include <levenshtein_matcher.h>
#include <output.h>
#include <prefix_matcher.h>
#include <terminal.h>
#include <word_completion.h>
#include <word_completion_visualisation.h>

bool isActive = false;

bool isVisualisationActive() { return isActive; }

void toggleVisualisation() {
  struct engineConfig *EC = getEngineConfig();
  struct editorConfig *E = getEditorConfig();

  if (!EC->isActive && !isActive) {
    editorSetStatusMessage("Word Completion is currently inactive!");
    return;
  }

  if (E->screenrows < MAX_SUGGESTIONS * 2) {
    editorSetStatusMessage(
        "Your current terminal is too small for visualisation!");
    return;
  }

  isActive = !isActive;

  if (isActive) {
    E->screenrows = E->screenrows - MAX_SUGGESTIONS;
  } else {
    E->screenrows = E->screenrows + MAX_SUGGESTIONS;
  }
}

void fillRows(struct abuf *ab) {
  struct editorConfig *E = getEditorConfig();
  for (int i = 0; i < MAX_SUGGESTIONS; i++) {
    int len = 0;
    while (len < E->screencols) {
      abAppend(ab, " ", 1); // fill with spaces
      len++;
    }
    abAppend(ab, "\r\n", 2); // newline
  }
}

void drawVisualisation(struct abuf *ab) {
  abAppend(ab, "\x1b[44m", 5);

  struct engineConfig *EC = getEngineConfig();
  switch (EC->mode) {
  case PREFIX:
    visualisePM(ab);
    break;
  case FUZZY:
    visualiseLM(ab);
    break;
  case LANGUAGE:
    visualiseLangM(ab);
    break;

  default:
    fillRows(ab);
    editorSetStatusMessage("Failed to draw visualisation!");
    break;
  }

  abAppend(ab, "\x1b[m", 3); // switch back to normal
}

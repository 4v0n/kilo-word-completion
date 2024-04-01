#include <stdbool.h>
#include <ctype.h>
#include <string.h>


// Returns whether the char is an alphabet 
bool isAlphabetChar(const char c) {
  int val;
  if (isupper(c)) {
    val = tolower(c) - 'a';
  } else {
    val = c - 'a';
  }

  return (val >= 0 && val < 26);
}

bool isAlphabetWord(const char *w) {
  for (int i = 0; i < (int)strlen(w) - 1; i++) {
    if (!isAlphabetChar(w[i])) {
      return false;
    }
  }
  return true;
}
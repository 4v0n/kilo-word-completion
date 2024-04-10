/*
  This file handles the calculation of the
  levenshtein distance between two strings
*/

#include <levenshtein.h>
#include <stdio.h>
#include <string.h>

// returns the minimum of 3 integers
int min(int a, int b, int c) {
  if (a < b && a < c)
    return a;
  if (b < a && b < c)
    return b;
  return c;
}

// calculates the Levenshtein distance between two strings
int getLevenshteinDistance(const char *a, const char *b) {
  int len1 = strlen(a), len2 = strlen(b);
  int matrix[len1 + 1][len2 + 1];

  for (int i = 0; i <= len1; i++)
    matrix[i][0] = i;
  for (int j = 0; j <= len2; j++)
    matrix[0][j] = j;

  for (int i = 1; i <= len1; i++) { // iterate over each char of a
    for (int j = 1; j <= len2; j++) { // iterate over each char of b
      int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
      matrix[i][j] = min(matrix[i - 1][j] + 1, matrix[i][j - 1] + 1,
                         matrix[i - 1][j - 1] + cost);
    }
  }

  // bottom right cell is levenshtein distance
  return matrix[len1][len2];
}

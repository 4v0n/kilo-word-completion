#include <CUnit/Basic.h>
#include <levenshtein.h>
#include <string.h>

void testLevenshteinDistance() {
  char *s1 = "apple";
  char *s2 = "apply";
  char *s3 = "appl";
  char *s4 = "pple";
  char *s5 = "ppl";
  char *s6 = "apples";
  char *s7  = "zzzzz";

  CU_ASSERT_EQUAL(getLevenshteinDistance(s1, s2), 1);
  CU_ASSERT_EQUAL(getLevenshteinDistance(s1, s3), 1);
  CU_ASSERT_EQUAL(getLevenshteinDistance(s1, s4), 1);
  CU_ASSERT_EQUAL(getLevenshteinDistance(s1, s5), 2);
  CU_ASSERT_EQUAL(getLevenshteinDistance(s1, s6), 1);
  CU_ASSERT_EQUAL(getLevenshteinDistance(s1, s7), 5);
}

void addLevenshteinTests() {
  CU_pSuite suite = CU_add_suite("Levenshtein Distance Tests", NULL, NULL);

  CU_add_test(suite, "Test Levenshtein distance", testLevenshteinDistance);
}
#include <CUnit/Basic.h>
#include <trie.h>

void test_test() {
  CU_ASSERT_EQUAL(10, 10);
}

void add_tests_trie() {
  CU_pSuite suite = CU_add_suite("Trie Tests", NULL, NULL);
  CU_add_test(suite, "example test", test_test);
}
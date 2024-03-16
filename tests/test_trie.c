#include <CUnit/Basic.h>
#include <trie.h>

// Tests whether getNode() allocates memory for a new trie node and returns a non-null pointer
void test_getNode() {
  TrieNode *node = getNode();
  CU_ASSERT_NOT_EQUAL(node, NULL);
  CU_ASSERT_FALSE(node->isEndOfWord);
  CU_ASSERT_EQUAL(node->weight, 0);

  for (int i = 0; i < ALPHABET_SIZE; i++)
    CU_ASSERT_EQUAL(node->children[i], NULL);
}

void add_tests_trie() {
  CU_pSuite suite = CU_add_suite("Trie Tests", NULL, NULL);

  CU_add_test(suite, "test getNode()", test_getNode);
}
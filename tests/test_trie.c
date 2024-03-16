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

// Tests single word insertion
void test_insert_single_word() {
  TrieNode *node = getNode();
  insert(node, "app", 3);

  // subtract index a due to offset
  CU_ASSERT_TRUE(node->children[0]->children['p' - 'a']->children['p' - 'a']->isEndOfWord);
  CU_ASSERT_EQUAL(node->children[0]->children['p' - 'a']->children['p' - 'a']->weight, 3);

  for (int i = 0; i < ALPHABET_SIZE; i++)
    CU_ASSERT_EQUAL(node->children[0]->children['p' - 'a']->children['p' - 'a']->children[i], NULL);
}

void add_tests_trie() {
  CU_pSuite suite = CU_add_suite("Trie Tests", NULL, NULL);

  CU_add_test(suite, "test getNode()", test_getNode);
  CU_add_test(suite, "test insert single word", test_insert_single_word);
}
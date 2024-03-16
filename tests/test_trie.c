#include <CUnit/Basic.h>
#include <trie.h>

TrieNode *root;

int setup() {
  root = getNode();
  return 0;
}

int teardown() {
  freeTrie(root);
  return 0;
}

void assertLeafEmpty(struct TrieNode *node) {
  CU_ASSERT_NOT_EQUAL(node, NULL);
  CU_ASSERT_FALSE(node->isEndOfWord);
  CU_ASSERT_EQUAL(node->weight, 0);

  for (int i = 0; i < ALPHABET_SIZE; i++)
    CU_ASSERT_EQUAL(node->children[i], NULL);
}

// Tests whether getNode() allocates memory for a new trie node and returns a non-null pointer
void test_getNode() {
  assertLeafEmpty(root);
}

// Tests single word insertion
void test_insert_single_word() {
  insert(root, "app", 3);

  // skip a (0)
  for (int i = 1; i < ALPHABET_SIZE; i++) {
    CU_ASSERT_EQUAL(root->children[i], NULL);
  }

  // subtract index a due to offset
  CU_ASSERT_TRUE(root->children[0]->children['p' - 'a']->children['p' - 'a']->isEndOfWord);
  CU_ASSERT_EQUAL(root->children[0]->children['p' - 'a']->children['p' - 'a']->weight, 3);

  for (int i = 0; i < ALPHABET_SIZE; i++)
    CU_ASSERT_EQUAL(root->children[0]->children['p' - 'a']->children['p' - 'a']->children[i], NULL);
}

// Tests multiple word insertion with a common prefix
void test_insert_multi_word() {
  insert(root, "app", 3);
  insert(root, "ape", 4);

  for (int i = 1; i < ALPHABET_SIZE; i++) {
    CU_ASSERT_EQUAL(root->children[i], NULL);
  }

  // test "app" is correct
  CU_ASSERT_TRUE(root->children[0]->children['p' - 'a']->children['p' - 'a']->isEndOfWord);
  CU_ASSERT_EQUAL(root->children[0]->children['p' - 'a']->children['p' - 'a']->weight, 3);

  for (int i = 0; i < ALPHABET_SIZE; i++)
    CU_ASSERT_EQUAL(root->children[0]->children['p' - 'a']->children['p' - 'a']->children[i], NULL);

  // test "ape" is correct
  CU_ASSERT_TRUE(root->children[0]->children['p' - 'a']->children['e' - 'a']->isEndOfWord);
  CU_ASSERT_EQUAL(root->children[0]->children['p' - 'a']->children['e' - 'a']->weight, 4);

  for (int i = 0; i < ALPHABET_SIZE; i++)
    CU_ASSERT_EQUAL(root->children[0]->children['p' - 'a']->children['e' - 'a']->children[i], NULL);
}

// Tests insertion of duplicate words
void test_insert_duplicate_words() {
  insert(root, "app", 3);
  insert(root, "app", 4);

  // skip a (0)
  for (int i = 1; i < ALPHABET_SIZE; i++) {
    CU_ASSERT_EQUAL(root->children[i], NULL);
  }

  // should update to new values
  CU_ASSERT_TRUE(root->children[0]->children['p' - 'a']->children['p' - 'a']->isEndOfWord);
  CU_ASSERT_EQUAL(root->children[0]->children['p' - 'a']->children['p' - 'a']->weight, 4);

  for (int i = 0; i < ALPHABET_SIZE; i++)
    CU_ASSERT_EQUAL(root->children[0]->children['p' - 'a']->children['p' - 'a']->children[i], NULL);
}

void test_insert_empty_string() {
  // reset trie
  teardown();
  setup();

  assertLeafEmpty(root);
  insert(root, "", 1);
  assertLeafEmpty(root);
}

void add_tests_trie() {
  CU_pSuite suite = CU_add_suite("Trie Tests", setup, teardown);

  CU_add_test(suite, "test getNode()", test_getNode);
  CU_add_test(suite, "test insert single word", test_insert_single_word);
  CU_add_test(suite, "test insert multiple words", test_insert_multi_word);
  CU_add_test(suite, "test insert duplicate words", test_insert_duplicate_words);
  CU_add_test(suite, "test insert empty string", test_insert_empty_string);
}
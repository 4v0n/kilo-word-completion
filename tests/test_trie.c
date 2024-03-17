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

// Tests whether getNode() allocates memory for a new trie node and returns a
// non-null pointer
void testGetNode() { assertLeafEmpty(root); }

// Tests single word insertion
void testInsertSingleWord() {
  insert(root, "app", 3);

  // skip a (0)
  for (int i = 1; i < ALPHABET_SIZE; i++) {
    CU_ASSERT_EQUAL(root->children[i], NULL);
  }

  // subtract index a due to offset
  CU_ASSERT_TRUE(
      root->children[0]->children['p' - 'a']->children['p' - 'a']->isEndOfWord);
  CU_ASSERT_EQUAL(
      root->children[0]->children['p' - 'a']->children['p' - 'a']->weight, 3);

  for (int i = 0; i < ALPHABET_SIZE; i++)
    CU_ASSERT_EQUAL(root->children[0]
                        ->children['p' - 'a']
                        ->children['p' - 'a']
                        ->children[i],
                    NULL);
}

// Tests multiple word insertion with a common prefix
void testInsertMultiWord() {
  insert(root, "app", 3);
  insert(root, "ape", 4);

  for (int i = 1; i < ALPHABET_SIZE; i++) {
    CU_ASSERT_EQUAL(root->children[i], NULL);
  }

  // test "app" is correct
  CU_ASSERT_TRUE(
      root->children[0]->children['p' - 'a']->children['p' - 'a']->isEndOfWord);
  CU_ASSERT_EQUAL(
      root->children[0]->children['p' - 'a']->children['p' - 'a']->weight, 3);

  for (int i = 0; i < ALPHABET_SIZE; i++)
    CU_ASSERT_EQUAL(root->children[0]
                        ->children['p' - 'a']
                        ->children['p' - 'a']
                        ->children[i],
                    NULL);

  // test "ape" is correct
  CU_ASSERT_TRUE(
      root->children[0]->children['p' - 'a']->children['e' - 'a']->isEndOfWord);
  CU_ASSERT_EQUAL(
      root->children[0]->children['p' - 'a']->children['e' - 'a']->weight, 4);

  for (int i = 0; i < ALPHABET_SIZE; i++)
    CU_ASSERT_EQUAL(root->children[0]
                        ->children['p' - 'a']
                        ->children['e' - 'a']
                        ->children[i],
                    NULL);
}

// Tests insertion of duplicate words
void testInsertDuplicateWords() {
  insert(root, "app", 3);
  insert(root, "app", 4);

  // skip a (0)
  for (int i = 1; i < ALPHABET_SIZE; i++) {
    CU_ASSERT_EQUAL(root->children[i], NULL);
  }

  // should update to new values
  CU_ASSERT_TRUE(
      root->children[0]->children['p' - 'a']->children['p' - 'a']->isEndOfWord);
  CU_ASSERT_EQUAL(
      root->children[0]->children['p' - 'a']->children['p' - 'a']->weight, 4);

  for (int i = 0; i < ALPHABET_SIZE; i++)
    CU_ASSERT_EQUAL(root->children[0]
                        ->children['p' - 'a']
                        ->children['p' - 'a']
                        ->children[i],
                    NULL);
}

void testInsertEmptyString() {
  // reset trie
  teardown();
  setup();

  assertLeafEmpty(root);
  insert(root, "", 1);
  assertLeafEmpty(root);
}

void testInsertInvalidString() {
  // reset trie
  teardown();
  setup();

  assertLeafEmpty(root);
  insert(root, "!@#^#$", 1);
  assertLeafEmpty(root);
}

void testInsertCapitalWord() {
  insert(root, "APP", 3);

  // skip a (0)
  for (int i = 1; i < ALPHABET_SIZE; i++) {
    CU_ASSERT_EQUAL(root->children[i], NULL);
  }

  // subtract index a due to offset
  CU_ASSERT_TRUE(
      root->children[0]->children['p' - 'a']->children['p' - 'a']->isEndOfWord);
  CU_ASSERT_EQUAL(
      root->children[0]->children['p' - 'a']->children['p' - 'a']->weight, 3);

  for (int i = 0; i < ALPHABET_SIZE; i++)
    CU_ASSERT_EQUAL(root->children[0]
                        ->children['p' - 'a']
                        ->children['p' - 'a']
                        ->children[i],
                    NULL);
}

void testGetNonexistentLeaf() {
  TrieNode *leaf = getTrieLeaf(root, "ten");
  CU_ASSERT_EQUAL(leaf, NULL);
}

void testGetLeafOutOfBounds() {
  TrieNode *leaf = getTrieLeaf(root, "!@#");
  CU_ASSERT_EQUAL(leaf, NULL);
}

void testGetLeaf() {
  insert(root, "app", 3);
  TrieNode *leaf = getTrieLeaf(root, "ap");

  CU_ASSERT_FALSE(leaf->isEndOfWord);
  CU_ASSERT_EQUAL(leaf->weight, 0);
  CU_ASSERT_NOT_EQUAL(leaf->children['p' - 'a'], NULL);
}

void testSearchNonexistentWord() {
  char **suggestions = getSuggestions(root, "pro");
  CU_ASSERT_EQUAL(suggestions[0], NULL);
}

void testSearchInsertedWord() {
  insert(root, "app", 3);

  char **suggestions = getSuggestions(root, "ap");
  CU_ASSERT_EQUAL(suggestions[0], "app");
}

void addTrieTests() {
  CU_pSuite suite = CU_add_suite("Trie Tests", setup, teardown);

  // getNode
  CU_add_test(suite, "test getNode()", testGetNode);

  // insert
  CU_add_test(suite, "test insert single word", testInsertSingleWord);
  CU_add_test(suite, "test insert multiple words", testInsertMultiWord);
  CU_add_test(suite, "test insert duplicate words", testInsertDuplicateWords);
  CU_add_test(suite, "test insert empty string", testInsertEmptyString);
  CU_add_test(suite, "test insert invalid word", testInsertInvalidString);
  CU_add_test(suite, "test insert capital word", testInsertCapitalWord);

  // getTrieLeaf
  CU_add_test(suite, "test get non existent trie leaf", testGetNonexistentLeaf);
  CU_add_test(suite, "test get out of bounds / invalid trie leaf", testGetLeafOutOfBounds);
  CU_add_test(suite, "test get trie leaf", testGetLeaf);

  // search
  CU_add_test(suite, "test search non-existent word",
              testSearchNonexistentWord);
  CU_add_test(suite, "test search inserted word", testSearchInsertedWord);
}
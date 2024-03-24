#include <CUnit/Basic.h>

extern void addListTests();
extern void addTrieTests();

int main() {
  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();
  
  // Add tests
  addListTests();
  addTrieTests();

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();

  return CU_get_error();
}
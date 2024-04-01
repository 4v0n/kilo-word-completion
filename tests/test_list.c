#include <CUnit/Basic.h>
#include <list.h>

int compareIntegers(const Node *a, const Node *b) {
  int intA = *(const int*)a->data;
  int intB = *(const int*)b->data;

  if (intA < intB) return -1;
  if (intA == intB) return 0;
  return 1;
}

void testInitList() {
  List list = *createList();

  CU_ASSERT_EQUAL(list.head, NULL);
  CU_ASSERT_EQUAL(list.size, 0);
}

void testAddInvalidElement() {
  List list = *createList();


  addElement(&list, NULL, sizeof(NULL));

  CU_ASSERT_EQUAL(list.head, NULL);
  CU_ASSERT_EQUAL(list.size, 0);
}

void testAddSingleElement() {
  List list = *createList();

  int item = 2;

  addElement(&list, &item, sizeof(item));

  CU_ASSERT_NOT_EQUAL(list.head, NULL);
  CU_ASSERT_EQUAL(*(int*)list.head->data, item);
  CU_ASSERT_EQUAL(list.size, 1);
}

void testAddMultipleElements() {
  List list = *createList();

  int item1 = 1;
  int item2 = 2;

  addElement(&list, &item1, sizeof(item1));
  addElement(&list, &item2, sizeof(item2));

  CU_ASSERT_NOT_EQUAL(list.head, NULL);
  CU_ASSERT_NOT_EQUAL(list.head->next, NULL);

  CU_ASSERT_EQUAL(*(int*)list.head->data, item1);
  CU_ASSERT_EQUAL(*(int*)list.head->next->data, item2);

  CU_ASSERT_EQUAL(list.size, 2);
}

void testGetElement() {
  List list = *createList();


  int item1 = 1;
  int item2 = 8;

  addElement(&list, &item1, sizeof(item1));
  addElement(&list, &item2, sizeof(item2));

  CU_ASSERT_EQUAL(item1, *(int*)getListElement(&list, 0));
  CU_ASSERT_EQUAL(item2, *(int*)getListElement(&list, 1));
}

void testGetOutOfBoundsElement() {
  List list = *createList();

  int item1 = 1;
  int item2 = 8;

  addElement(&list, &item1, sizeof(item1));
  addElement(&list, &item2, sizeof(item2));

  CU_ASSERT_EQUAL(NULL, getListElement(&list, -1));
  CU_ASSERT_EQUAL(NULL, getListElement(&list, 2));
}

void testSortEmptyList() {
  List list = *createList();


  sortList(&list, compareIntegers);

  CU_ASSERT_EQUAL(list.head, NULL);
  CU_ASSERT_EQUAL(list.size, 0);
}

void testSortSingleElementList() {
  List list = *createList();


  int item = 2;
  addElement(&list, &item, sizeof(item));

  sortList(&list, compareIntegers);

  CU_ASSERT_NOT_EQUAL(list.head, NULL);
  CU_ASSERT_EQUAL(*(int*)list.head->data, item);
  CU_ASSERT_EQUAL(list.size, 1);
}

void testSortUnsortedList() {
  List list = *createList();


  int items[] = {9, 3, 5, 7, 3, 2};

  for (int i = 0; i < 6; i++) {
    addElement(&list, &items[i], sizeof(items[i]));
  }

  sortList(&list, compareIntegers);

  CU_ASSERT_NOT_EQUAL(list.head, NULL);
  CU_ASSERT_NOT_EQUAL(list.head->next, NULL);

  CU_ASSERT_EQUAL(*(int*)getListElement(&list, 0), 2);
  CU_ASSERT_EQUAL(*(int*)getListElement(&list, 1), 3);
  CU_ASSERT_EQUAL(*(int*)getListElement(&list, 2), 3);
  CU_ASSERT_EQUAL(*(int*)getListElement(&list, 3), 5);
  CU_ASSERT_EQUAL(*(int*)getListElement(&list, 4), 7);
  CU_ASSERT_EQUAL(*(int*)getListElement(&list, 5), 9);

  CU_ASSERT_EQUAL(list.size, 6);
}

void testSortSortedList() {
  List list = *createList();


  int items[] = {1, 2, 3, 4, 5, 6};

  for (int i = 0; i < 6; i++) {
    addElement(&list, &items[i], sizeof(items[i]));
  }

  sortList(&list, compareIntegers);

  CU_ASSERT_NOT_EQUAL(list.head, NULL);
  CU_ASSERT_NOT_EQUAL(list.head->next, NULL);

  CU_ASSERT_EQUAL(*(int*)getListElement(&list, 0), 1);
  CU_ASSERT_EQUAL(*(int*)getListElement(&list, 1), 2);
  CU_ASSERT_EQUAL(*(int*)getListElement(&list, 2), 3);
  CU_ASSERT_EQUAL(*(int*)getListElement(&list, 3), 4);
  CU_ASSERT_EQUAL(*(int*)getListElement(&list, 4), 5);
  CU_ASSERT_EQUAL(*(int*)getListElement(&list, 5), 6);

  CU_ASSERT_EQUAL(list.size, 6);
}

void testSortWithNullComparator() {
  List list = *createList();


  int items[] = {9, 3, 5, 7, 3, 2};

  for (int i = 0; i < 6; i++) {
    addElement(&list, &items[i], sizeof(items[i]));
  }

  sortList(&list, NULL);

  CU_ASSERT_NOT_EQUAL(list.head, NULL);
  CU_ASSERT_NOT_EQUAL(list.head->next, NULL);

  CU_ASSERT_EQUAL(*(int*)getListElement(&list, 0), 9);
  CU_ASSERT_EQUAL(*(int*)getListElement(&list, 1), 3);
  CU_ASSERT_EQUAL(*(int*)getListElement(&list, 2), 5);
  CU_ASSERT_EQUAL(*(int*)getListElement(&list, 3), 7);
  CU_ASSERT_EQUAL(*(int*)getListElement(&list, 4), 3);
  CU_ASSERT_EQUAL(*(int*)getListElement(&list, 5), 2);

  CU_ASSERT_EQUAL(list.size, 6);
}

void testFreeEmptyList() {
  List *list = createList();


  freeList(list);
}

void testFreeNonEmptyList() {
  List *list = createList();

  int items[] = {9, 3, 5, 7, 3, 2};

  for (int i = 0; i < 6; i++) {
    addElement(list, &items[i], sizeof(items[i]));
  }

  freeList(list);
}

void addListTests() {
  CU_pSuite suite = CU_add_suite("Linked List Tests", NULL, NULL);

  CU_add_test(suite, "Test initialise list", testInitList);

  CU_add_test(suite, "Test add invalid element", testAddInvalidElement);
  CU_add_test(suite, "Test add single element", testAddSingleElement);
  CU_add_test(suite, "Test add multiple elements", testAddMultipleElements);
  
  CU_add_test(suite, "Test get element", testGetElement);
  CU_add_test(suite, "Test get out of bounds element", testGetOutOfBoundsElement);

  CU_add_test(suite, "Test sort empty list", testSortEmptyList);
  CU_add_test(suite, "Test sort single element list", testSortSingleElementList);
  CU_add_test(suite, "Test sort unsorted list", testSortUnsortedList);
  CU_add_test(suite, "Test sort sorted list", testSortSortedList);
  CU_add_test(suite, "Test sort sort with null comparator", testSortWithNullComparator);

  CU_add_test(suite, "Test free empty list", testFreeEmptyList);
  CU_add_test(suite, "Test free non-empty list", testFreeNonEmptyList);
}
#include <stdlib.h>

typedef struct Node {
  void *data;
  struct Node *next;
} Node;

typedef struct List {
  Node *head;
  int size;
} List;

typedef int (*Comparator)(const void*, const void*);

void initList(List *list);
void addElement(List *list, void *data, size_t dataSize);
void *getListElement(List *list, int index);
void sortList(List* list, Comparator comp);

void freeList(List *list);
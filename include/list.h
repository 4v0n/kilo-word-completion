#include <stdlib.h>

typedef struct Node {
  void *data;
  struct Node *next;
} Node;

typedef struct List {
  Node *head;
  int size;
} List;

void initList(List *list);
void addElement(List *list, void *data, size_t dataSize);
void *getListElement(List *list, int index);
void free();
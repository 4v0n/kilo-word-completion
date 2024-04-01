#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

typedef struct Node {
  void *data;
  struct Node *next;
} Node;

typedef struct List {
  Node *head;
  int size;
} List;

typedef int (*Comparator)(const struct Node *, const struct Node *);

List *createList();
void addElement(List *list, const void *data, const size_t dataSize);
void *getListElement(const List *list, int index);
void sortList(List *list, Comparator comp);
void emptyList(List *list);
void freeList(List *list);

#endif
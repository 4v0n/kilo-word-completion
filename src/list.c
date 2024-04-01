#include <list.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

List *createList() {
  List *list = (List *)malloc(sizeof(List));
  list->head = NULL;
  list->size = 0;

  return list;
}

Node *getListNode(const List *list, const int index) {
  Node *currentNode = list->head;

  if (index == 0) {
    return currentNode;
  }

  for (int i = 0; i < index; i++) {
    Node *nextNode = currentNode->next;

    if (nextNode == NULL) {
      return NULL;
    } else if ((i + 1) == index) {
      return nextNode;
    }

    currentNode = nextNode;
  }

  return NULL;
}

void addElement(List *list, const void *data, const size_t dataSize) {
  if (data == NULL || list == NULL || dataSize <= 0)
    return;

  Node *newNode = (Node *)malloc(sizeof(Node));
  newNode->data = malloc(dataSize);
  newNode->next = NULL;
  memcpy(newNode->data, data, dataSize);

  if (list->head == NULL) {
    list->head = newNode;
  } else {
    Node *prevNode = getListNode(list, list->size - 1);
    prevNode->next = newNode;
  }

  list->size++;
}

void *getListElement(const List *list, const int index) {
  Node *node = getListNode(list, index);

  if (node == NULL || index > (list->size - 1) || index < 0) {
    return NULL;
  }

  return node->data;
}

// adapted from https://www.geeksforgeeks.org/merge-sort-for-linked-list/
void frontBackSplit(struct Node* source, struct Node** frontRef, struct Node** backRef) {
  struct Node *fast;
  struct Node *slow;
  slow = source;
  fast = source->next;

  while (fast != NULL) {
    fast = fast->next;
    if (fast != NULL) {
      slow = slow->next;
      fast = fast->next;
    }
  }

  *frontRef = source;
  *backRef = slow->next;
  slow->next = NULL;
}

// adapted from https://www.geeksforgeeks.org/merge-sort-for-linked-list/
struct Node *sortedMerge(struct Node *a, struct Node *b, Comparator comp) {
  struct Node *result = NULL;

  if (a == NULL) {
    return b;
  } else if (b == NULL) {
    return a;
  }

  if (comp(a,b) <= 0) {
    result = a;
    result->next = sortedMerge(a->next, b, comp);
  } else {
    result = b;
    result->next = sortedMerge(a, b->next, comp);
  }

  return result;
}

// MergeSort algorithm adapted from
// https://www.geeksforgeeks.org/merge-sort-for-linked-list/
void mergeSort(struct Node **headRef, Comparator comp) {
  struct Node *head = *headRef;
  struct Node *a;
  struct Node *b;

  if ((head == NULL) || (head->next == NULL)) {
    return;
  }

  frontBackSplit(head, &a, &b);

  mergeSort(&a, comp);
  mergeSort(&b, comp);

  *headRef = sortedMerge(a,b, comp);
}

void sortList(List *list, Comparator comp) {
  if (list->head == NULL || comp == NULL) {
    return;
  }
  mergeSort(&list->head, comp);
}

void emptyList(List *list) {
  Node *current = list->head;
  while (current != NULL) {
    Node *next = current->next;
    free(current->data); // Free the data pointed by the node
    free(current);       // Free the node itself
    current = next;
  }
  list->head = NULL; // After all nodes are freed, set head to NULL
  list->size = 0;    // Reset size to 0
}

void freeList(List *list) {
  emptyList(list);
  free(list);
}

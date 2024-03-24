#include <list.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initList(List *list) {
  list->head = NULL;
  list->size = 0;
}

Node *getListNode(List *list, int index) {
  Node *currentNode = list->head;

  if (index == 0) {
    return currentNode;
  }

  for (int i = 0; i < index; i++) {
    Node *nextNode = currentNode->next;

    if (nextNode == NULL) {
      return NULL;
    } else if ((i+1) == index) {
      return nextNode;
    }

    currentNode = nextNode;
  }

  return NULL;
}

void addElement(List *list, void *data, size_t dataSize) {
  if (data == NULL)
    return;

  Node *newNode = (Node *)malloc(sizeof(Node));
  newNode->data = malloc(dataSize);
  memcpy(newNode->data, data, dataSize);

  if (list->head == NULL) {
    list->head = newNode;
  } else {
    Node *prevNode = getListNode(list, list->size - 1);
    prevNode->next = newNode;
  }

  list->size++;
}

void *getListElement(List *list, int index) {
  Node *node = getListNode(list, index);

  if (node == NULL || index > (list->size - 1) || index < 0) {
    return NULL;
  }

  return node->data;
}

void sortedInsert(Node **headRef, Node *newNode, Comparator comp) {
  Node dummy;
  Node *current = &dummy;
  dummy.next = *headRef;

  while (current->next != NULL && comp(current->next->data, newNode->data) < 0) {
    current = current->next;
  }

  newNode->next = current->next;
  current->next = newNode;

  *headRef = dummy.next;
}

void sortList(List *list, Comparator comp) {

  if (comp == NULL) {
    return;
  }

  Node *sorted = NULL;
  Node *current = list->head;
  while (current != NULL) {
    Node *next = current->next;
    sortedInsert(&sorted, current, comp);
    current = next;
  }
  list->head = sorted;
}

void freeAllNodes(List* list) {
    Node* current = list->head;
    while (current != NULL) {
        Node* next = current->next;
        free(current->data); // Free the data pointed by the node
        free(current); // Free the node itself
        current = next;
    }
    list->head = NULL; // After all nodes are freed, set head to NULL
    list->size = 0; // Reset size to 0
}

void freeList(List *list) {
  freeAllNodes(list);
  free(list);
}

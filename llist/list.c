/**
 * @file list.c
 * @brief Implementing a Generic Linked List 
 * @details Implementing a Generic Linked List. This is not thread safe
 * 
 * @authors [upstream] @b pseudomuto
 * @authors Pham Ngoc Thang (thangdc94)
 * @see https://gist.github.com/pseudomuto/6334796#file-sample_app-c
 * @see http://pseudomuto.com/development/2013/05/02/implementing-a-generic-linked-list-in-c/
 */

#include <stdlib.h> /* to use malloc/free */
#include <string.h> /* to use memcpy */
#include <assert.h> /* to use the assert macro */
 
#include "list.h" /* to have linked list typedefs, structs and function prototypes available to us */
 
void list_new(list *list, int elementSize, freeFunction freeFn)
{
  assert(elementSize > 0);
  list->logicalLength = 0;
  list->elementSize = elementSize;
  list->head = list->tail = NULL;
  list->freeFn = freeFn;
}
 
void list_destroy(list *list)
{
  listNode *current;
  while(list->head != NULL) {
    current = list->head;
    list->head = current->next;

    if(list->freeFn) {
      list->freeFn(current->data);
    }

    free(current->data);
    free(current);
  }
   list->logicalLength = 0;
}
 
void list_prepend(list *list, void *element)
{
  listNode *node = malloc(sizeof(listNode));
  node->data = malloc(list->elementSize);
  memcpy(node->data, element, list->elementSize);

  node->next = list->head;
  list->head = node;

  // first node?
  if(!list->tail) {
    list->tail = list->head;
  }

  list->logicalLength++;
}
 
void list_append(list *list, void *element)
{
  listNode *node = malloc(sizeof(listNode));
  node->data = malloc(list->elementSize);
  node->next = NULL;

  memcpy(node->data, element, list->elementSize);

  if(list->logicalLength == 0) {
    list->head = list->tail = node;
  } else {
    list->tail->next = node;
    list->tail = node;
  }

  list->logicalLength++;
}
 
void list_for_each(list *list, listIterator iterator)
{
  assert(iterator != NULL);
 
  listNode *node = list->head;
  bool result = TRUE;
  while(node != NULL && result) {
    result = iterator(node->data);
    node = node->next;
  }
}
 
void list_head(list *list, void *element, bool removeFromList)
{
  assert(list->head != NULL);
 
  listNode *node = list->head;
  memcpy(element, node->data, list->elementSize);
 
  if(removeFromList) {
    list->head = node->next;
    list->logicalLength--;
    if(list->freeFn){ list->freeFn(node->data); }
    free(node->data);
    free(node);
  }
}
 
void list_tail(list *list, void *element)
{
  assert(list->tail != NULL);
  listNode *node = list->tail;
  memcpy(element, node->data, list->elementSize);
}
 
int list_size(list *list)
{
  return list->logicalLength;
}
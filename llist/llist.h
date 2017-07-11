/**
 * @file list.h
 * @brief A  Generic Linked List
 * @details A Generic Linked List. This is not thread safe
 * 
 * @authors David Muto (pseudomuto) | Contact: <david.muto@gmail.com>
 * @authors Pham Ngoc Thang (thangdc94)
 * @see https://gist.github.com/pseudomuto/6334796#file-sample_app-c
 * @see http://pseudomuto.com/development/2013/05/02/implementing-a-generic-linked-list-in-c/
 */

#ifndef __LIST_H
#define __LIST_H

/**
 * @brief a pointer function used to free malloc'd objects
 * @details a common function pointer used to free malloc'd objects.
 * This will help you prevent memory leak
 * 
 * @param[in] first item to be freed.
 */
typedef void (*freeFunction)(void *);

/** define a bool type */
typedef enum {
  FALSE, /**< FALSE = 0 */
  TRUE,  /**< TRUE = 1  */
} bool;

/**
 * @brief Iterating function pointer
 * @details This function will be passed a pointer to the node's data element. 
 * This function will be called for each element in the list during ::list_for_each()
 * 
 * @param[in] first a pointer to the node's data element
 * @return @b TRUE|FALSE if FALSE: stop iterating, TRUE: continue until we are out of nodes
 */
typedef bool (*listIterator)(void *);

/** a typical linked list node */
typedef struct _listNode
{
  void *data;             /**< a void * field data for storing whatever the implementer likes */
  struct _listNode *next; /**< pointer pointing to the next node in the list */
} listNode;

/** Linked list */
typedef struct
{
  int logicalLength;   /**< the number of elements in the list */
  int elementSize;     /**< the size of each element */
  listNode *head;      /**< a pointer pointing to head of the list */
  listNode *tail;      /**< a pointer pointing to tail of the list */
  freeFunction freeFn; /**< pointer pointing to the function used to free malloc'd objects */
} list;

/**
 * @brief Create new linked list
 * @details initializes a linked list to store elements of @p elementSize and to 
 * call ::freeFunction for each element when destroying a list
 * @warning You need pass a real ::freeFunction() if you use malloc() or some functions 
 * that use malloc() in your data such as strdup() when you add element data into 
 * the linked list using ::list_append()
 * 
 * @param[out] list linked list
 * @param[in] elementSize [description]
 * @param[in] freeFn freeFunction (which can be NULL for simple/stack types).
 */
void list_new(list *list, int elementSize, freeFunction freeFn);

/**
 * @brief Destroy the linked list
 * @details frees dynamically allocated nodes and optionally 
 * calls ::freeFunction with each node's data pointer. If a ::freeFunction was supplied 
 * (not NULL) with the call to list_new for this list, it will be called on each node's data
 * 
 * @param[in] list linked list 
 */
void list_destroy(list *list);

/**
 * @brief Prepend an element to the list
 * @details adds a node to the head of the list
 * 
 * @param[in] list linked list
 * @param[in] element node element you want to add to head of the list
 */
void list_prepend(list *list, void *element);

/**
 * @brief Append an element to the list
 * @details adds a node to the tail of the list
 * 
 * @param[in] list linked list
 * @param[in] element node element you want to add to last of the list
 */
void list_append(list *list, void *element);

/**
 * @brief get linked list size
 * @details get the number of items in the list
 * 
 * @param[in] list linked list
 * @return the number of items in the list
 */
int list_size(list *list);

/**
 * @brief Do function on each node
 * @details calles the supplied iterator function with the data element 
 * of each node (iterates over the list)
 * 
 * @param[in] list linked list
 * @param[in] iterator the iterator function
 */
void list_for_each(list *list, listIterator iterator);

/**
 * @brief get the head of the list
 * @details get the head of the list (optionally removing it at the same time)
 * 
 * @param[in] list linked list
 * @param[out] element [description]
 * @param[in] removeFromList @b TRUE|FALSE, if TRUE: remove the head node from the list, 
 * updating head/tail values along the way, and free the malloc'd memory for node->data and node,
 * if FALSE: don't remove it
 */
void list_head(list *list, void *element, bool removeFromList);

/**
 * @brief get the tail of the list
 * @details get the tail of the list
 * 
 * @param[in] list linked list
 * @param[in] element [description]
 */
void list_tail(list *list, void *element);

#endif //__LIST_H
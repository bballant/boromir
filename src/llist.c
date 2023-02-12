
#include <glib.h>
#include <stdlib.h>
#include <string.h>

#include "llist.h"

void llist_init(llist *list, void (*destroy)(void *data)) {
  list->size = 0;
  list->destroy = destroy;
  list->head = NULL;
  list->tail = NULL;
  return;
}

void llist_destroy(llist *list) {
  void *data;
  while (list->size > 0) {

    if (llist_rem_next(list, NULL, (void **)&data) == 0 &&
        list->destroy != NULL) {

      list->destroy(data);
    }
  }
  memset(list, 0, sizeof(llist));
  return;
}

int llist_ins_next(llist *list, lval *element, const void *data) {
  lval *new_element;
  if ((new_element = (lval *)malloc(sizeof(lval))) == NULL)
    return -1;
  new_element->data = (void *)data;
  if (element == NULL) {
    if (list->size == 0)
      list->tail = new_element;
    new_element->next = list->head;
    list->head = new_element;
  }
  else {
    if (element->next == NULL)
      list->tail = new_element;
    new_element->next = element->next;
    element->next = new_element;
  }
  list->size++;
  return 0;
}

int llist_rem_next(llist *list, lval *element, void **data) {
  lval *old_element;
  if (list->size == 0)
    return -1;
  if (element == NULL) {
    *data = list->head->data;
    old_element = list->head;
    list->head = list->head->next;
    if (list->size == 1)
      list->tail = NULL;
  } else {
    if (element->next == NULL)
      return -1;
    *data = element->next->data;
    old_element = element->next;
    element->next = element->next->next;
    if (element->next == NULL)
      list->tail = element;
  }
  free(old_element);
  list->size--;
  return 0;
}

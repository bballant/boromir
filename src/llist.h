#ifndef __LLIST_H
#define __LLIST_H

#include <stdlib.h>

typedef struct lval {
  void *data;
  struct lval *next;
} lval;

typedef struct llist {
  int size;
  int (*match)(const void *key1, const void *key2);
  void (*destroy)(void *data);
  lval *head;
  lval *tail;
} llist;

void llist_init(llist *list, void (*destroy)(void *data));

void llist_destroy(llist *list);

int llist_ins_next(llist *list, lval *val, const void *data);

int llist_rem_next(llist *list, lval *val, void **data);

#define llist_add(list, data) llist_ins_next(list, NULL, data)

#define llist_is_head(list, element) ((element) == (list)->head ? 1 : 0)

#define llist_is_tail(element) ((element)->next == NULL ? 1 : 0)

#endif

#ifndef HLIST_H_
#define HLIST_H_

typedef struct h_item_ {
  char *key;
  void *value;
  struct h_item_ *next;
} h_item;

typedef struct h_list_ {
  h_item *head;
} h_list;

void h_test(void);

#endif // HLIST_H_

#include "hlist.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
**  // h_list
*/

h_list *h_new_list(void) {
  h_list *list = malloc(sizeof(h_list));
  list->head = NULL;
  return list;
}

h_item *h_new_item(char *key, void *value) {
  h_item *item = malloc(sizeof(h_item));
  item->key = key;
  item->value = value;
  item->next = NULL;
  return item;
}

void h_print_item(h_item *item) {
  if (item == NULL) {
    printf("NULL\n");
  } else {
    printf("- %s: %s\n", item->key, (char *)item->value);
  }
};

void h_print_list(h_list *list) {
  h_item *item = list->head;
  while (item != NULL) {
    h_print_item(item);
    item = item->next;
  }
}

h_item *h_scan_to(h_list *list, char *key) {
  h_item *curr_item = NULL;
  h_item *next_item = list->head;
  while (next_item != NULL && strcmp(next_item->key, key) < 0) {
    curr_item = next_item;
    next_item = curr_item->next;
  }
  return curr_item;
}

h_item *h_find(h_list *list, char *key) {
  h_item *item = h_scan_to(list, key);
  if (item != NULL && item->next != NULL && strcmp(key, item->next->key) == 0) {
    return item->next;
  } else {
    return NULL;
  }
}

void *h_get(h_list *list, char *key) {
  h_item *item = h_find(list, key);
  if (item != NULL) {
    return item->value;
  } else {
    return NULL;
  }
}

h_item *h_upsert(h_list *list, char *key, void *value) {
  h_item *item = h_scan_to(list, key);
  if (item != NULL && item->next != NULL && strcmp(key, item->next->key) == 0) {
    item->next->value = value;
    return item;
  } else {
    h_item *new_item = h_new_item(key, value);
    if (item == NULL) {
      if (list->head != NULL) {
        new_item->next = list->head;
      }
      list->head = new_item;
    } else {
      new_item->next = item->next;
      item->next = new_item;
    }
    return new_item;
  }
}

h_item *h_remove(h_list *list, char *key) {
  h_item *prev = h_scan_to(list, key);
  if (prev != NULL && prev->next != NULL && strcmp(key, prev->next->key) == 0) {
    h_item *curr = prev->next;
    h_item *next = curr->next;
    prev->next = next;
    return curr;
  }
  return NULL;
}

void h_free_item_fns(h_item *item, void(free_key)(void *k),
                     void(free_value)(void *k)) {
  free_key(item->key);
  free_value(item->value);
  free(item);
}

void h_free_item(h_item *item) { h_free_item_fns(item, free, free); }

void h_free_list_fns(h_list *list, void(free_key)(void *k),
                     void(free_value)(void *k)) {
  h_item *item = list->head;
  while (item != NULL) {
    h_item *next_item = item->next;
    h_free_item_fns(item, free_key, free_value);
    item = next_item;
  }
  free(list);
}

void h_free_list(h_list *item) {
    h_free_list_fns(item, free, free);
}

//return a malloc'd string from a string literal
char *from_literal(char *str_lit) {
  char *ret = malloc(strlen(str_lit) + 1);
  sprintf(ret, "%s", str_lit);
  return ret;
}

void h_test(void) {
  h_list *list = h_new_list();
  for (int i=0; i < 100; i = i + 10) {
    char *key = malloc(sizeof(char) * 4);
    sprintf(key, "%03d", i);
    char *val = malloc(sizeof(char) * 6);
    sprintf(val, "v%04d", i * 77);
    h_item *item = h_upsert(list, key, val);
    if (item == NULL) {
      printf("error inserting %s: %s", key, val);
    }
  }
  h_upsert(list, from_literal("070"), from_literal("foooooo"));
  h_upsert(list, from_literal("089"), from_literal("here"));
  h_print_list(list);

  h_item *rem = h_remove(list, from_literal("070"));
  h_free_item(rem);
  printf("xool --------------------\n");
  h_print_list(list);

  h_free_list(list);
}

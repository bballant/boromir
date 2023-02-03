#include "ds.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *key;
  int value;
} item;

item *linear_search(item *items, size_t size, const char *key) {
  for (size_t i = 0; i < size; i++) {
    if (strcmp(items[i].key, key) == 0) {
      return &items[i];
    }
  }
  return NULL;
}

void test_linear_search(void) {
  item items[] = {{"foo", 10}, {"bar", 42},   {"baz", 36}, {"buzz", 7},
                  {"bob", 11}, {"jame", 100}, {"x", 200}};
  size_t num_items = sizeof(items) / sizeof(item);

  item *found = linear_search(items, num_items, "bob");
  if (!found) {
    printf("not found!\n");
  }
  printf("linear_serch: value of 'bob' is %d\n", found->value);
}

int cmp(const void *a, const void *b) {
  item *item_a = (item *)a;
  item *item_b = (item *)b;
  return strcmp(item_a->key, item_b->key);
}

item *binary_search(item *items, size_t size, const char *key) {
  if (size + size < size) {
    return NULL; // size too big; avoid overflow
  }
  size_t low = 0;
  size_t high = size;
  while (low < high) {
    size_t mid = (low + high) / 2;
    int c = strcmp(items[mid].key, key);
    if (c == 0) {
      return &items[mid];
    }
    if (c < 0) {
      low = mid + 1; // eliminate low half of array
    } else {
      high = mid; // eliminate high half of array
    }
  }
  // Entire array has been eliminated, key not found.
  return NULL;
}

void test_binary_search(void) {
  item items[] = {{"bar", 42}, {"bazz", 36},  {"bob", 11}, {"buzz", 7},
                  {"foo", 10}, {"jane", 100}, {"x", 200}};
  size_t num_items = sizeof(items) / sizeof(item);

  item key = {"bob", 0};
  item *found = bsearch(&key, items, num_items, sizeof(item), cmp);
  if (found == NULL) {
    printf("Not found!");
  }
  printf("bsearch: value of 'bob' is %d\n", found->value);

  found = binary_search(items, num_items, "bob");
  if (found == NULL) {
    printf("Not found!");
  }
  printf("binary_search: value of 'bob' is %d\n", found->value);
}

void ds_test(void) { test_binary_search(); }

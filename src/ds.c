#include "ds.h"
#include "llist.h"

#include <assert.h>
#include <stdbool.h>
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

typedef struct {
  const char *key;
  void *value;
} ht_entry;

typedef struct {
  ht_entry *entries;
  size_t capacity;
  size_t length;
} ht;

#define INITIAL_CAPACITY 16 // must not be zero

ht *ht_create(void) {
  ht *table = malloc(sizeof(ht));
  if (table == NULL) {
    return NULL;
  }
  table->length = 0;
  table->capacity = INITIAL_CAPACITY;

  table->entries = calloc(table->capacity, sizeof(ht_entry));
  if (table->entries == NULL) {
    free(table);
    return NULL;
  }

  return table;
}

void ht_destroy(ht *table) {
  for (size_t i = 0; i < table->capacity; i++) {
    free((void *)table->entries[i].key);
  }
  free(table->entries);
  free(table);
}

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

static u_int64_t hash_key(const char *key) {
  u_int64_t hash = FNV_OFFSET;
  for (const char *p = key; *p; p++) {
    hash ^= (u_int64_t)(unsigned char)(*p);
    hash *= FNV_PRIME;
  }
  return hash;
}

void *ht_get(ht *table, const char *key) {
  u_int64_t hash = hash_key(key);
  size_t index = (size_t)(hash & (u_int64_t)(table->capacity - 1));
  // looks at hash location, then probes to find key
  // if it finds an empty slot before it finds key, then key is not there
  while (table->entries[index].key != NULL) {
    if (strcmp(key, table->entries[index].key) == 0) {
      // found key, return value
      return table->entries[index].value;
    }
    index++;
    if (index >= table->capacity) {
      index = 0;
    }
  }
  return NULL;
}

static const char *ht_set_entry(ht_entry *entries, size_t capacity,
                                const char *key, void *value, size_t *plength) {
  u_int64_t hash = hash_key(key);
  size_t index = (size_t)(hash & (u_int64_t)(capacity - 1));

  // looks at hash location, then probes to find key
  // if it finds an empty slot before it finds key, then key is not there
  while (entries[index].key != NULL) {
    if (strcmp(key, entries[index].key) == 0) {
      // found key, set value
      entries[index].value = value;
      return entries[index].key;
    }
    index++;
    if (index >= capacity) {
      index = 0;
    }
  }

  // didn't find key, insert it
  if (plength != NULL) {
    key = strdup(key);
    if (key == NULL) {
      return NULL;
    }
    (*plength)++;
  }
  entries[index].key = key;
  entries[index].value = value;
  return key;
}

// Expand hash table to twice its current size. Return true on success,
// false if out of memory.
static bool ht_expand(ht *table) {
  // Allocate new entries array.
  size_t new_capacity = table->capacity * 2;
  if (new_capacity < table->capacity) {
    return false; // overflow (capacity would be too big)
  }
  ht_entry *new_entries = calloc(new_capacity, sizeof(ht_entry));
  if (new_entries == NULL) {
    return false;
  }

  // Iterate entries, move all non-empty ones to new table's entries.
  for (size_t i = 0; i < table->capacity; i++) {
    ht_entry entry = table->entries[i];
    if (entry.key != NULL) {
      ht_set_entry(new_entries, new_capacity, entry.key, entry.value, NULL);
    }
  }

  // Free old entries array and update this table's details.
  free(table->entries);
  table->entries = new_entries;
  table->capacity = new_capacity;
  return true;
}

const char *ht_set(ht *table, const char *key, void *value) {
  assert(value != NULL);
  if (value == NULL) {
    return NULL;
  }
  if (table->length >= table->capacity / 2) {
    if (!ht_expand(table)) {
      return NULL;
    }
  }

  return ht_set_entry(table->entries, table->capacity, key, value,
                      &table->length);
}

size_t ht_length(ht *table) { return table->length; }

typedef struct {
  const char *key; // current key
  void *value;     // current value

  // Don't use these fields directly.
  ht *_table;    // reference to hash table being iterated
  size_t _index; // current index into ht._entries
} hti;

hti ht_iterator(ht *table) {
  hti it;
  it._table = table;
  it._index = 0;
  return it;
}

bool ht_next(hti *it) {
  // Loop till we've hit end of entries array.
  ht *table = it->_table;
  while (it->_index < table->capacity) {
    size_t i = it->_index;
    it->_index++;
    if (table->entries[i].key != NULL) {
      // Found next non-empty item, update iterator key and value.
      ht_entry entry = table->entries[i];
      it->key = entry.key;
      it->value = entry.value;
      return true;
    }
  }
  return false;
}

void test_hash_table(void) {
  ht *counts = ht_create();
  if (counts == NULL) {
    printf("error\n");
    return;
  }

  // Read next word from stdin (at most 100 chars long).
  char word[101];
  while (scanf("%100s", word) != EOF) {
    // Look up word.
    void *value = ht_get(counts, word);
    if (value != NULL) {
      // Already exists, increment int that value points to.
      int *pcount = (int *)value;
      (*pcount)++;
      continue;
    }

    // Word not found, allocate space for new int and set to 1.
    int *pcount = malloc(sizeof(int));
    if (pcount == NULL) {
      printf("error\n");
      return;
    }
    *pcount = 1;
    if (ht_set(counts, word, pcount) == NULL) {
      printf("error\n");
      return;
    }
  }

  // Print out words and frequencies, freeing values as we go.
  hti it = ht_iterator(counts);
  while (ht_next(&it)) {
    printf("%s %d\n", it.key, *(int *)it.value);
    free(it.value);
  }

  // Show the number of unique words.
  printf("%d\n", (int)ht_length(counts));

  ht_destroy(counts);
}

typedef llist Stack;

#define stack_init llist_init

#define stack_destroy llist_destroy

void stack_push(Stack *s, void *data) {
  lval *e = malloc(sizeof(lval));
  *e = (lval){.data = data, .next = s->head};
  s->head = e;
}

void stack_pop(Stack *s, void **data) {
  lval *h = s->head;
  *data = h->data;
  s->head = h->next;
  free(h);
}

void *stack_peek(Stack *s) {
  lval *h = s->head;
  return h->data;
}

void test_stack(void) {
  Stack *stack = malloc(sizeof(Stack));
  stack_init(stack, free);
  stack_push(stack, "Foo");
  stack_push(stack, "Bar");
  stack_push(stack, "Baz");
  printf("%s\n", (char *)stack_peek(stack));
  void **data = malloc(sizeof(void*));
  stack_pop(stack, data);
  printf("%s\n", (char *)(*data));
  printf("%s\n", (char *)stack_peek(stack));
}

void ds_test(void) { test_stack(); }

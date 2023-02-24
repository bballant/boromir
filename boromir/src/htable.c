/*
** This code was borrowed and adapted from the following excellent tutorial by Ben Hoyt
** https://benhoyt.com/writings/hash-table-in-c/
*/
#include "htable.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

htable *htable_create(void) {
  htable *table = malloc(sizeof(htable));
  if (table == NULL) {
    return NULL;
  }
  table->length = 0;
  table->capacity = HT_INITIAL_CAPACITY;

  table->entries = calloc(table->capacity, sizeof(htval));
  if (table->entries == NULL) {
    free(table);
    return NULL;
  }

  return table;
}

void htable_destroy(htable *table) {
  for (size_t i = 0; i < table->capacity; i++) {
    free((void *)table->entries[i].key);
  }
  free(table->entries);
  free(table);
}

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

static u_int64_t htable_hash_key(const char *key) {
  u_int64_t hash = FNV_OFFSET;
  for (const char *p = key; *p; p++) {
    hash ^= (u_int64_t)(unsigned char)(*p);
    hash *= FNV_PRIME;
  }
  return hash;
}

void *htable_get(htable *table, const char *key) {
  u_int64_t hash = htable_hash_key(key);
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

static const char *htable_set_entry(htval *entries, size_t capacity,
                                    const char *key, void *value, size_t *plength) {
  u_int64_t hash = htable_hash_key(key);
  size_t index = (size_t)(hash & (u_int64_t)(capacity - 1));

  while (entries[index].key != NULL) {
    if (strcmp(key, entries[index].key) == 0) {
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
static bool htable_expand(htable *table) {
  size_t new_capacity = table->capacity * 2;
  if (new_capacity < table->capacity) {
    // new_capacity overflow, can't expand
    return false;
  }
  htval *new_entries = calloc(new_capacity, sizeof(htval));
  if (new_entries == NULL) {
    return false;
  }

  for (size_t i = 0; i < table->capacity; i++) {
    htval entry = table->entries[i];
    if (entry.key != NULL) {
      htable_set_entry(new_entries, new_capacity, entry.key, entry.value, NULL);
    }
  }

  // Free old entries array and update table.
  free(table->entries);
  table->entries = new_entries;
  table->capacity = new_capacity;
  return true;
}

const char *htable_set(htable *table, const char *key, void *value) {
  assert(value != NULL);
  if (value == NULL) {
    return NULL;
  }
  if (table->length >= table->capacity / 2) {
    if (!htable_expand(table)) {
      return NULL;
    }
  }
  return htable_set_entry(table->entries, table->capacity, key, value,
                      &table->length);
}

size_t ht_length(htable *table) { return table->length; }

typedef struct {
  const char *key;
  void *value;

  // Don't use these fields directly.
  htable *_table;
  // current index of htable entries
  size_t _index;
} htable_iterator;

htable_iterator htable_iterator_create(htable *table) {
  htable_iterator it;
  it._table = table;
  it._index = 0;
  return it;
}

bool htable_next(htable_iterator *it) {
  htable *table = it->_table;
  while (it->_index < table->capacity) {
    size_t i = it->_index;
    it->_index++;
    if (table->entries[i].key != NULL) {
      // Found next non-empty item, update iterator key and value.
      htval entry = table->entries[i];
      it->key = entry.key;
      it->value = entry.value;
      return true;
    }
  }
  return false;
}

void test_hash_table(void) {
  htable *counts = htable_create();
  if (counts == NULL) {
    printf("error\n");
    return;
  }

  // Read next word from stdin (at most 100 chars long).
  char word[101];
  while (scanf("%100s", word) != EOF) {
    void *value = htable_get(counts, word);
    if (value != NULL) {
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
    if (htable_set(counts, word, pcount) == NULL) {
      printf("error\n");
      return;
    }
  }

  // Print out words and frequencies, freeing values as we go.
  htable_iterator it = htable_iterator_create(counts);
  while (htable_next(&it)) {
    printf("%s %d\n", it.key, *(int *)it.value);
    free(it.value);
  }

  // Show the number of unique words.
  printf("%d\n", (int)ht_length(counts));
  htable_destroy(counts);
}

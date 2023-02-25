#ifndef HTABLE_H_
#define HTABLE_H_

#include <stdbool.h>
#include <stdlib.h>

typedef struct {
  const char *key;
  void *value;
} htval;

typedef struct {
  htval *entries;
  size_t capacity;
  size_t length;
} htable;

#define HT_INITIAL_CAPACITY 16  // must not be zero

htable *htable_create(void);

void htable_destroy(htable *table);

void *htable_get(htable *table, const char *key);

const char *htable_set(htable *table, const char *key, void *value);

size_t htable_length(htable *table);

typedef struct {
  const char *key;
  void *value;

  // Don't use these fields directly.
  htable *_table;
  // current index of htable entries
  size_t _index;
} htable_iterator;

htable_iterator htable_iterator_create(htable *table);

bool htable_next(htable_iterator *it);

#endif // HTABLE_H_

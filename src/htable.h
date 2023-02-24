#ifndef HTABLE_H_
#define HTABLE_H_

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

size_t ht_length(htable *table);


#endif // HTABLE_H_

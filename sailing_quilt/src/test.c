#include <glib.h>
#include <stdio.h>

#include "test.h"

#include "llist.h"


// Testing the Linked List ////////////////////////

typedef struct {
  llist *ll;
} llfixture;

void llist_test_setup(llfixture *llf, gconstpointer test_data) {
  llist *l = malloc(sizeof(llist));
  llist_init(l, NULL);
  llist_add(l, "foo");
  llist_add(l, "bar");
  llf->ll = l;
}

void llist_test_teardown(llfixture *llf, gconstpointer test_data) {
  llist_destroy(llf->ll);
}

void llist_test_foo(llfixture *llf, gconstpointer ignored) {
  g_assert(1 == 1);
  char *head = (char *)llf->ll->head->data;
  char *tail = (char *)llf->ll->tail->data;
  g_assert_cmpstr(head, ==, "bar");
  g_assert_cmpstr(tail, ==, "foo");
  llist_add(llf->ll, "cool");
  g_assert_cmpstr((char *)llf->ll->head->data, ==, "cool");
  g_assert_cmpstr(head, ==, "bar");
}



// Run all tests /////////////////
int g_test(int argc, char *argv[]) {
  g_test_init(&argc, &argv, NULL);
  g_test_add("/llist/foo", llfixture, NULL, llist_test_setup, llist_test_foo,
             llist_test_teardown);
  return g_test_run();
}

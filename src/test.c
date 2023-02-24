#include <glib.h>
#include <stdio.h>

#include "test.h"

#include "bstree.h"
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

// Testing the BST ////////////////////////

bool add_data_to_ll(void *acc, void *str) {
  llist *list = (llist *)acc;
  return (llist_add(list, str) == 0);
}

typedef struct {
  bstree *bst;
} bstfixture;

void bst_test_setup(bstfixture *bstf, gconstpointer test_data) {
  bstree *str_tree = bst_new_bstree((bst_cmp_fn_t *)strcmp, NULL);
  char input_str[] = "the approach will not be easy you are required "
                     "to maneuver straight down this trench and skim";
  const char *delim = " ";
  char *token = strtok(input_str, delim);
  while (token) {
    size_t len = strlen(token);
    if (!bst_insert(str_tree, token, len + 1))
      break;
    token = strtok(NULL, delim);
  }
  bstf->bst = str_tree;
}

void bst_test_teardown(bstfixture *bstf, gconstpointer test_data) {
  bst_clear(bstf->bst);
}

void bst_test_sort(bstfixture *bstf, gconstpointer ignored) {
  llist *l = malloc(sizeof(llist));
  llist_init(l, NULL);
  int n = bst_inorder_acc(bstf->bst, l, add_data_to_ll);
  g_assert_cmpint(l->size, ==, 17);
  g_assert_cmpint(n, ==, 17);
  // linked list makes order reversed
  g_assert_cmpstr(l->head->data, ==, "you");
  g_assert_cmpstr(l->tail->data, ==, "and");
  llist_destroy(l);
}

// Run all tests /////////////////
int g_test(int argc, char *argv[]) {
  g_test_init(&argc, &argv, NULL);
  g_test_add("/llist/foo", llfixture, NULL, llist_test_setup, llist_test_foo,
             llist_test_teardown);
  g_test_add("/bst/sort", bstfixture, NULL, bst_test_setup, bst_test_sort,
             bst_test_teardown);
  return g_test_run();
}

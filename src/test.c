#include <glib.h>
#include <stdio.h>

#include "test.h"

#include "bstree.h"
#include "htable.h"
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

// Testing the htable ////////////////////////

typedef struct {
  htable *ht;
} htfixture;

void ht_test_setup(htfixture *htf, gconstpointer test_data) {
  htable *tok_counts = htable_create();
  if (tok_counts == NULL) {
    printf("error\n");
    return;
  }
  char input_str[] =
      "The approach will not be easy. You are required to maneuver "
      "straight down this trench and skim the surface to this point. The "
      "target area is only two meters wide. It’s a small thermal exhaust "
      "port, right below the main port. The shaft leads directly to the "
      "reactor system. A precise hit will start a chain reaction which should "
      "destroy the station. Only a precise hit will set up a chain reaction. "
      "The shaft is ray-shielded, so you’ll have to use proton torpedoes. "
      "That’s impossible, even for a computer. It’s not impossible. I used "
      "to bull’s-eye womp rats in my T-sixteen back home. They’re not much "
      "bigger than two meters. Man your ships! And may the Force be with you! ";

  const char *delim = " ";
  char *token = strtok(input_str, delim);
  while (token) {
    void *value = htable_get(tok_counts, token);
    if (value != NULL) {
      int *pcount = (int *)value;
      (*pcount)++;
      token = strtok(NULL, delim);
      continue;
    }

    // token not found, create count
    int *pcount = malloc(sizeof(int));
    if (pcount == NULL) {
      printf("error\n");
      return;
    }
    *pcount = 1;
    if (htable_set(tok_counts, token, pcount) == NULL) {
      printf("error\n");
      return;
    }
    token = strtok(NULL, delim);
  }

  htf->ht = tok_counts;
}

void ht_test_teardown(htfixture *htf, gconstpointer test_data) {
  htable_iterator it = htable_iterator_create(htf->ht);
  while (htable_next(&it)) {
    free(it.value);
  }
  htable_destroy(htf->ht);
}

void ht_test_counts(htfixture *htf, gconstpointer ignored) {
  int unique_words = (int)htable_length(htf->ht);
  printf("%d\n", unique_words);
  g_assert_cmpint(unique_words, ==, 92);
  g_assert_cmpint(*(int *)htable_get(htf->ht, "chain"), ==, 2);
}

// Run all tests /////////////////
int g_test(int argc, char *argv[]) {
  g_test_init(&argc, &argv, NULL);
  g_test_add("/llist/foo", llfixture, NULL, llist_test_setup, llist_test_foo,
             llist_test_teardown);
  g_test_add("/bst/sort", bstfixture, NULL, bst_test_setup, bst_test_sort,
             bst_test_teardown);
  g_test_add("/htable/sort", htfixture, NULL, ht_test_setup, ht_test_counts,
             ht_test_teardown);
  return g_test_run();
}

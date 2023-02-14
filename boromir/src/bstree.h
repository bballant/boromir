#ifndef BST_H_
#define BST_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct bst_node {
  struct bst_node *left, *right;
  size_t size;
  char data[];
} bst_node;

typedef const void *bst_key_fn_t(const void *data);

typedef int bst_cmp_fn_t(const void *key1, const void *key2);

typedef struct {
  struct bst_node *root;
  bst_cmp_fn_t *cmp;
  bst_key_fn_t *get_key;
} bstree;

bstree *bst_new_bstree(bst_cmp_fn_t *cmp, bst_key_fn_t *getKey);

bool bst_insert(bstree *bst, const void *data, size_t size);

const void *bst_search(bstree *bst, const void *key);

bool bst_erase(bstree *bst, const void *key);

void bst_clear(bstree *bst);

int bst_inorder(bstree *bst, bool (*action)(void *data));

int bst_inorder_acc(bstree *bst, void *acc,
                    bool (*accum)(void *acc, void *data));

//int BST_rev_inorder( BST_t *pBST, _Bool (*action)( void *pData ));
//int BST_preorder( BST_t *pBST, _Bool (*action)( void *pData ));
//int BST_postorder( BST_t *pBST, _Bool (*action)( void *pData ));


#endif // BST_H_

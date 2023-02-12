#include "bstree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const void *default_get_key(const void *data) { return data; }

/*
** Create Empty bstree
*/
bstree *bst_new_bstree(bst_cmp_fn_t *cmp, bst_key_fn_t *get_key) {
  bstree *bst = NULL;
  if (cmp != NULL) {
    bst = malloc(sizeof(bstree));
  }
  if (bst != NULL) {
    bst->root = NULL;
    bst->cmp = cmp;
    bst->get_key = (get_key != NULL) ? get_key : default_get_key;
  }
  return bst;
}

static bool _insert(bstree *bst, bst_node **nnode, const void *data,
                    size_t size);

bool bst_insert(bstree *bst, const void *data, size_t size) {
  if (bst == NULL || data == NULL || size == 0) {
    return false;
  }
  return _insert(bst, &(bst->root), data, size);
}

static bool _insert(bstree *bst, bst_node **nnode, const void *data,
                    size_t size) {
  bst_node *node = *nnode;
  if (node == NULL) {
    node = malloc(sizeof(bst_node) + size);
    if (node != NULL) {
      node->left = node->right = NULL;
      memcpy(node->data, data, size);
      *nnode = node;
      return true;
    } else {
      return false;
    }
  } else {
    const void *key1 = bst->get_key(data);
    const void *key2 = bst->get_key(node->data);
    if (bst->cmp(key1, key2) < 0) {
      return _insert(bst, &(node->left), data, size);
    } else {
      return _insert(bst, &(node->right), data, size);
    }
  }
}

static const void *_search(bstree *bst, const bst_node *node, const void *key);

const void *bst_search(bstree *bst, const void *key) {
  if (bst == NULL || key == NULL)
    return NULL;
  return _search(bst, bst->root, key); // Start at the root of the
                                      // tree.
}

static const void *_search(bstree *bst, const bst_node *node, const void *key) {
  if (node == NULL) return NULL;  // No subtree to search;
  // no match found.
  else {  // Compare data:
    int cmp_res = bst->cmp(key, bst->get_key(node->data));
    if (cmp_res == 0)  // Found a match.
      return node->data;
    else if (cmp_res < 0)                   // Continue the search
      return _search(bst, node->left, key);  // in the left subtree,
    else
      return _search(bst, node->right, key);  // or in the right
    // subtree.
  }
}

static bst_node *detachMin(bst_node **nnode) {
  bst_node *node = *nnode; // A pointer to the current node.
  if (node == NULL)
    return NULL; // pNode is an empty subtree.
  else if (node->left != NULL)
    return detachMin(&(node->left)); // The minimum is in the left
  // subtree.
  else {                  // pNode points to the minimum node.
    *nnode = node->right; // Attach the right child to the parent.
    return node;
  }
}

static bool erase(bstree *bst, bst_node **nnode, const void *key);

bool bst_erase(bstree *bst, const void *key) {
  if (bst == NULL || key == NULL)
    return false;
  return erase(bst, &(bst->root), key); // Start at the root of
}

static bool erase(bstree *bst, bst_node **nnode, const void *key) {
  bst_node *node = *nnode; // Pointer to the current node.
  if (node == NULL)
    return false; // No match found.
  // Compare data:
  int cmp_res = bst->cmp(key, bst->get_key(node->data));
  if (cmp_res < 0)                         // Continue the search
    return erase(bst, &(node->left), key); // in the left subtree,
  else if (cmp_res > 0)
    return erase(bst, &(node->right), key); // or in the right
  // subtree.
  else {                    // Found the node to be deleted.
    if (node->left == NULL) // If no more than one child,
      *nnode = node->right; // attach the child to the parent.
    else if (node->right == NULL)
      *nnode = node->left;
    else // Two children: replace the node with
    {    // the minimum from the right subtree.
      bst_node *min = detachMin(&(node->right));
      *nnode = min;           // Graft it onto the deleted node's parent.
      min->left = node->left; // Graft the deleted node's children.
      min->right = node->right;
    }
    free(node); // Release the deleted node's storage.
    return true;
  }
}

static void clear(bst_node *node);

void bst_clear(bstree *bst) {
  if (bst != NULL) {
    clear(bst->root);
    bst->root = NULL;
  }
}

static void clear(bst_node *node) {
  if (node != NULL) {
    clear(node->left);
    clear(node->right);
    free(node);
  }
}

static int inorder(bst_node *node, bool (*action)(void *data));

int bst_inorder(bstree *bst, bool (*action)(void *data)) {
  if (bst == NULL || action == NULL)
    return 0;
  else
    return inorder(bst->root, action);
}

static int inorder(bst_node *node, bool (*action)(void *data)) {
  int count = 0;
  if (node == NULL)
    return 0;
  count = inorder(node->left, action); // L: Traverse the left
  // subtree.
  if (action(node->data))                // N: Visit the current node
    ++count;                             // itself.
  count += inorder(node->right, action); // R: Traverse the right
  // subtree.
  return count;
}

static int inorder_acc(bst_node *node, void *acc,
                       bool (*accum)(void *acc, void *data));

int bst_inorder_acc(bstree *bst, void *acc,
                    bool (*accum)(void *acc, void *data)) {
  if (bst == NULL || accum == NULL)
    return 0;
  else
    return inorder_acc(bst->root, acc, accum);
}

static int inorder_acc(bst_node *node, void *acc,
                       bool (*accum)(void *acc, void *data)) {
  int count = 0;
  if (node == NULL) return 0;
  count = inorder_acc(node->left, acc, accum);
  if (accum(acc, node->data)) ++count;
  count += inorder_acc(node->right, acc, accum);
  return count;
}

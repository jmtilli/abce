#ifndef _ABCE_RBTREE_H_
#define _ABCE_RBTREE_H_

#include "abce_errno.h"
#include "abcelikely.h"

#ifdef __cplusplus
extern "C" {
#endif

struct abce_rb_tree_node {
  int is_black;
  struct abce_rb_tree_node *children[2];
  struct abce_rb_tree_node *parent;
};

static inline struct abce_rb_tree_node *abce_rb_tree_left(const struct abce_rb_tree_node *node)
{
  return node->children[0];
}
static inline struct abce_rb_tree_node *abce_rb_tree_right(const struct abce_rb_tree_node *node)
{
  return node->children[1];
}

typedef int (*abce_rb_tree_cmp)(struct abce_rb_tree_node *a, struct abce_rb_tree_node *b, void *ud);
typedef int (*abce_rb_tree_cmp_asym)(const void *a, struct abce_rb_tree_node *b, void *ud);

struct abce_rb_tree_nocmp {
  struct abce_rb_tree_node *root;
};

struct abce_rb_tree {
  struct abce_rb_tree_nocmp nocmp;
  abce_rb_tree_cmp cmp;
  void *cmp_userdata;
};

static inline void abce_rb_tree_nocmp_init(struct abce_rb_tree_nocmp *tree)
{
  tree->root = NULL;
}

static inline void abce_rb_tree_init(struct abce_rb_tree *tree, abce_rb_tree_cmp cmp, void *cmp_userdata)
{
  abce_rb_tree_nocmp_init(&tree->nocmp);
  tree->cmp = cmp;
  tree->cmp_userdata = cmp_userdata;
}

static inline struct abce_rb_tree_node *abce_rb_tree_nocmp_root(struct abce_rb_tree_nocmp *tree)
{
  return tree->root;
}

static inline struct abce_rb_tree_node *abce_rb_tree_root(struct abce_rb_tree *tree)
{
  return abce_rb_tree_nocmp_root(&tree->nocmp);
}

int abce_rb_tree_nocmp_valid(struct abce_rb_tree_nocmp *tree);

static inline int abce_rb_tree_valid(struct abce_rb_tree *tree)
{
  return abce_rb_tree_nocmp_valid(&tree->nocmp);
}

struct abce_rb_tree_node *abce_rb_tree_nocmp_leftmost(struct abce_rb_tree_nocmp *tree);

static inline struct abce_rb_tree_node *abce_rb_tree_leftmost(struct abce_rb_tree *tree)
{
  return abce_rb_tree_nocmp_leftmost(&tree->nocmp);
}

struct abce_rb_tree_node *abce_rb_tree_nocmp_rightmost(struct abce_rb_tree_nocmp *tree);

static inline struct abce_rb_tree_node *abce_rb_tree_rightmost(struct abce_rb_tree *tree)
{
  return abce_rb_tree_nocmp_rightmost(&tree->nocmp);
}

void abce_rb_tree_nocmp_insert_repair(struct abce_rb_tree_nocmp *tree, struct abce_rb_tree_node *node);

static inline void abce_rb_tree_insert_repair(struct abce_rb_tree *tree, struct abce_rb_tree_node *node)
{
  abce_rb_tree_nocmp_insert_repair(&tree->nocmp, node);
}

void abce_rb_tree_insert(struct abce_rb_tree *tree, struct abce_rb_tree_node *node);

void abce_rb_tree_nocmp_delete(struct abce_rb_tree_nocmp *tree, struct abce_rb_tree_node *node);

static inline void abce_rb_tree_delete(struct abce_rb_tree *tree, struct abce_rb_tree_node *node)
{
  abce_rb_tree_nocmp_delete(&tree->nocmp, node);
}

#define ABCE_RB_TREE_NOCMP_FIND(tree, cmp, cmp_userdata, tofind) \
  ({ \
    const struct abce_rb_tree_nocmp *__abce_rb_tree_find_tree = (tree); \
    struct abce_rb_tree_node *__abce_rb_tree_find_node = __abce_rb_tree_find_tree->root; \
    while (__abce_rb_tree_find_node != NULL) \
    { \
      int __abce_rb_tree_find_res = \
        (cmp)((tofind), __abce_rb_tree_find_node, (cmp_userdata)); \
      if (__abce_rb_tree_find_res < 0) \
      { \
        __abce_rb_tree_find_node = __abce_rb_tree_find_node->children[0]; \
      } \
      else if (__abce_rb_tree_find_res > 0) \
      { \
        __abce_rb_tree_find_node = __abce_rb_tree_find_node->children[1]; \
      } \
      else \
      { \
        break; \
      } \
    } \
    __abce_rb_tree_find_node; \
  })

#if 1
#undef ABCE_RB_TREE_NOCMP_FIND
#define ABCE_RB_TREE_NOCMP_FIND(tree, cmp, cmp_userdata, tofind) \
  abce_rb_tree_nocmp_find_asym((tree),(cmp),(cmp_userdata),(tofind))
#endif

/*
 * NB: this is slower than the macro version
 */
static inline struct abce_rb_tree_node *abce_rb_tree_nocmp_find(
  struct abce_rb_tree_nocmp *tree, abce_rb_tree_cmp cmp, void *cmp_userdata,
  struct abce_rb_tree_node *tofind)
{
  struct abce_rb_tree_node *node = tree->root;
  while (node != NULL)
  {
    int res = cmp(tofind, node, cmp_userdata);
    #if 1
    if (abce_likely(res == 0))
    {
      return node;
    }
    node = node->children[res > 0];
    #else
    if (res < 0)
    {
      node = node->left;
    }
    else if (res > 0)
    {
      node = node->right;
    }
    else
    {
      break;
    }
    #endif
  }
  return node;
}

static inline struct abce_rb_tree_node *abce_rb_tree_nocmp_find_asym(
  const struct abce_rb_tree_nocmp *tree, abce_rb_tree_cmp_asym cmp, void *cmp_userdata,
  const void *tofind)
{
  struct abce_rb_tree_node *node = tree->root;
  while (node != NULL)
  {
    int res = cmp(tofind, node, cmp_userdata);
    #if 1
    if (abce_likely(res == 0))
    {
      return node;
    }
    node = node->children[res > 0];
    #else
    if (res < 0)
    {
      node = node->left;
    }
    else if (res > 0)
    {
      node = node->right;
    }
    else
    {
      break;
    }
    #endif
  }
  return node;
}

static inline int abce_rb_tree_nocmp_insert_nonexist(
  struct abce_rb_tree_nocmp *tree, abce_rb_tree_cmp cmp, void *cmp_userdata,
  struct abce_rb_tree_node *toinsert)
{
  void *cmp_ud = cmp_userdata;
  struct abce_rb_tree_node *node = tree->root;
  int finalres = 0;

  toinsert->is_black = 0;
  toinsert->children[0] = NULL;
  toinsert->children[1] = NULL;
  if (node == NULL)
  {
    tree->root = toinsert;
    toinsert->parent = NULL;
    abce_rb_tree_nocmp_insert_repair(tree, toinsert);
  }
  while (node != NULL)
  {
    int res = cmp(toinsert, node, cmp_ud);
    int child_off;
    if (res == 0)
    {
      finalres = -EEXIST;
      break;
    }
    child_off = (res > 0);
    if (node->children[child_off] == NULL)
    {
      node->children[child_off] = toinsert;
      toinsert->parent = node;
      abce_rb_tree_nocmp_insert_repair(tree, toinsert);
      break;
    }
    node = node->children[child_off];
  }
  return finalres;
}

/*
 * NB: this is slower than the non-macro version
 */
#define ABCE_RB_TREE_NOCMP_INSERT_NONEXIST(tree, cmp, cmp_userdata, toinsert) \
  ({ \
    struct abce_rb_tree_nocmp *__abce_rb_tree_insert_tree = (tree); \
    abce_rb_tree_cmp __abce_rb_tree_insert_cmp = (cmp); \
    void *__abce_rb_tree_insert_cmp_ud = (cmp_userdata); \
    struct abce_rb_tree_node *__abce_rb_tree_insert_toinsert = (toinsert); \
    struct abce_rb_tree_node *__abce_rb_tree_insert_node = __abce_rb_tree_insert_tree->root; \
    int __abce_rb_tree_insert_finalres = 0; \
    __abce_rb_tree_insert_toinsert->is_black = 0; \
    __abce_rb_tree_insert_toinsert->children[0] = NULL; \
    __abce_rb_tree_insert_toinsert->children[1] = NULL; \
    if (__abce_rb_tree_insert_node == NULL) \
    { \
      __abce_rb_tree_insert_tree->root = __abce_rb_tree_insert_toinsert; \
      __abce_rb_tree_insert_toinsert->parent = NULL; \
      abce_rb_tree_nocmp_insert_repair(__abce_rb_tree_insert_tree, \
                                  __abce_rb_tree_insert_toinsert); \
    } \
    while (__abce_rb_tree_insert_node != NULL) \
    { \
      int __abce_rb_tree_insert_res = \
        __abce_rb_tree_insert_cmp(__abce_rb_tree_insert_toinsert, __abce_rb_tree_insert_node, \
                             __abce_rb_tree_insert_cmp_ud); \
      if (__abce_rb_tree_insert_res < 0) \
      { \
        if (__abce_rb_tree_insert_node->children[0] == NULL) \
        { \
          __abce_rb_tree_insert_node->children[0] = __abce_rb_tree_insert_toinsert; \
          __abce_rb_tree_insert_toinsert->parent = __abce_rb_tree_insert_node; \
          abce_rb_tree_nocmp_insert_repair(__abce_rb_tree_insert_tree, \
                                      __abce_rb_tree_insert_toinsert); \
          break; \
        } \
        __abce_rb_tree_insert_node = __abce_rb_tree_insert_node->children[0]; \
      } \
      else if (__abce_rb_tree_insert_res > 0) \
      { \
        if (__abce_rb_tree_insert_node->children[1] == NULL) \
        { \
          __abce_rb_tree_insert_node->children[1] = __abce_rb_tree_insert_toinsert; \
          __abce_rb_tree_insert_toinsert->parent = __abce_rb_tree_insert_node; \
          abce_rb_tree_nocmp_insert_repair(__abce_rb_tree_insert_tree, \
                                      __abce_rb_tree_insert_toinsert); \
          break; \
        } \
        __abce_rb_tree_insert_node = __abce_rb_tree_insert_node->children[1]; \
      } \
      else \
      { \
        __abce_rb_tree_insert_finalres = -EEXIST; \
        break; \
      } \
    } \
    __abce_rb_tree_insert_finalres; \
  })

#ifdef __cplusplus
};
#endif

#endif

#ifndef _TREES_H_
#define _TREES_H_

#include <stddef.h>
#include "abce.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline int
abce_tree_get_str(struct abce *abce, struct abce_mb **mbres,
                  struct abce_mb *mbt, const struct abce_mb *mbkey)
{
  struct abce_rb_tree_node *n;
  if (mbt->typ != ABCE_T_T || mbkey->typ != ABCE_T_S)
  {
    abort();
  }
  n = ABCE_RB_TREE_NOCMP_FIND(&mbt->u.area->u.tree.tree, abce_str_cmp_halfsym, NULL, mbkey);
  if (n == NULL)
  {
    return -ENOENT;
  }
  *mbres = &ABCE_CONTAINER_OF(n, struct abce_mb_rb_entry, n)->val;
  return 0;
}
static inline int
abce_tree_del_str(struct abce *abce,
                  struct abce_mb *mbt, const struct abce_mb *mbkey)
{
  struct abce_rb_tree_node *n;
  struct abce_mb_rb_entry *mbe;
  if (mbt->typ != ABCE_T_T || mbkey->typ != ABCE_T_S)
  {
    abort();
  }
  n = ABCE_RB_TREE_NOCMP_FIND(&mbt->u.area->u.tree.tree, abce_str_cmp_halfsym, NULL, mbkey);
  if (n == NULL)
  {
    return -ENOENT;
  }
  mbe = ABCE_CONTAINER_OF(n, struct abce_mb_rb_entry, n);
  abce_mb_refdn(abce, &mbe->key);
  abce_mb_refdn(abce, &mbe->val);
  abce_rb_tree_nocmp_delete(&mbt->u.area->u.tree.tree, n);
  abce->alloc(mbe, sizeof(*mbe), 0, &abce->alloc_baton);
  return 0;
}

static inline int
abce_tree_set_str(struct abce *abce,
                  struct abce_mb *mbt,
                  const struct abce_mb *mbkey,
                  const struct abce_mb *mbval)
{
  struct abce_mb *mbres;
  struct abce_mb_rb_entry *e;
  if (mbt->typ != ABCE_T_T)
  {
    abort();
  }
  if (mbkey->typ != ABCE_T_S)
  {
    abort();
  }
  if (abce_tree_get_str(abce, &mbres, mbt, mbkey) == 0)
  {
    abce_mb_refreplace(abce, mbres, mbval);
    return 0;
  }
  e = abce->alloc(NULL, 0, sizeof(*e), &abce->alloc_baton);
  if (e == NULL)
  {
    abce->err.code = ABCE_E_NO_MEM;
    abce_mb_errreplace_noinline(abce, mbkey);
    abce->err.val2 = sizeof(*e);
    return -ENOMEM;
  }
  e->key = abce_mb_refup(abce, mbkey);
  e->val = abce_mb_refup(abce, mbval);
  if (abce_rb_tree_nocmp_insert_nonexist(&mbt->u.area->u.tree.tree, abce_str_cmp_sym, NULL, &e->n) != 0)
  {
    abort();
  }
  mbt->u.area->u.tree.sz += 1;
  return 0;
}

// FIXME verify if algorithm correct
static inline int
abce_rbtree_get_prev(const struct abce_mb **mbreskey,
                     const struct abce_mb **mbresval,
                     const struct abce_rb_tree_nocmp *head,
                     const struct abce_mb *mbkey)
{
  struct abce_rb_tree_node *node;
  struct abce_mb_rb_entry *mbe;
  if (mbkey->typ != ABCE_T_S && mbkey->typ != ABCE_T_N)
  {
    abort();
  }
  node = head->root;
  if (mbkey->typ == ABCE_T_N)
  {
    while (node != NULL)
    {
      if (node->children[1] == NULL)
      {
        break;
      }
      node = node->children[1];
    }
    goto out;
  }
  while (node != NULL)
  {
    int res = abce_str_cmp_halfsym(mbkey, node, NULL);
    if (res < 0)
    {
      if (node->children[0] == NULL)
      {
        //goto out;
        break;
      }
      node = node->children[0];
    }
    else if (res > 0)
    {
      if (node->children[1] == NULL)
      {
        //break;
        goto out;
      }
      node = node->children[1];
    }
    else if (res == 0)
    {
      break;
    }
  }
  if (node->children[0] != NULL)
  {
    node = node->children[0];
    for (;;)
    {
      if (node->children[1] == NULL)
      {
        break;
      }
      node = node->children[1];
    }
  }
  else
  {
    while (node->parent && node->parent->children[0] == node)
    {
      node = node->parent;
    }
    node = node->parent;
  }
out:
  if (node == NULL)
  {
    *mbreskey = NULL;
    *mbresval = NULL;
    return -ENOENT;
  }
  mbe = ABCE_CONTAINER_OF(node, struct abce_mb_rb_entry, n);
  *mbreskey = &mbe->key;
  *mbresval = &mbe->val;
  return 0;
}

// FIXME verify if algorithm correct
static inline int
abce_rbtree_get_next(const struct abce_mb **mbreskey,
                     const struct abce_mb **mbresval,
                     const struct abce_rb_tree_nocmp *head,
                     const struct abce_mb *mbkey)
{
  struct abce_rb_tree_node *node;
  struct abce_mb_rb_entry *mbe;
  if (mbkey->typ != ABCE_T_S && mbkey->typ != ABCE_T_N)
  {
    abort();
  }
  node = head->root;
  if (mbkey->typ == ABCE_T_N)
  {
    while (node != NULL)
    {
      if (node->children[0] == NULL)
      {
        break;
      }
      node = node->children[0];
    }
    goto out;
  }
  while (node != NULL)
  {
    int res = abce_str_cmp_halfsym(mbkey, node, NULL);
    if (res < 0)
    {
      if (node->children[0] == NULL)
      {
        goto out;
        //break;
      }
      node = node->children[0];
    }
    else if (res > 0)
    {
      if (node->children[1] == NULL)
      {
        break;
      }
      node = node->children[1];
    }
    else if (res == 0)
    {
      break;
    }
  }
  if (node->children[1] != NULL)
  {
    node = node->children[1];
    for (;;)
    {
      if (node->children[0] == NULL)
      {
        break;
      }
      node = node->children[0];
    }
  }
  else
  {
    while (node->parent && node->parent->children[1] == node)
    {
      node = node->parent;
    }
    node = node->parent;
  }
out:
  if (node == NULL)
  {
    *mbreskey = NULL;
    *mbresval = NULL;
    return -ENOENT;
  }
  mbe = ABCE_CONTAINER_OF(node, struct abce_mb_rb_entry, n);
  *mbreskey = &mbe->key;
  *mbresval = &mbe->val;
  return 0;
}

static inline int
abce_tree_get_prev(struct abce *abce,
                   const struct abce_mb **mbreskey,
                   const struct abce_mb **mbresval,
                   const struct abce_mb *mbt,
                   const struct abce_mb *mbkey)
{
  if (mbt->typ != ABCE_T_T)
  {
    abort();
  }
  return abce_rbtree_get_prev(mbreskey, mbresval, &mbt->u.area->u.tree.tree, mbkey);
}

static inline int
abce_tree_get_next(struct abce *abce,
                   const struct abce_mb **mbreskey,
                   const struct abce_mb **mbresval,
                   const struct abce_mb *mbt,
                   const struct abce_mb *mbkey)
{
  if (mbt->typ != ABCE_T_T)
  {
    abort();
  }
  return abce_rbtree_get_next(mbreskey, mbresval, &mbt->u.area->u.tree.tree, mbkey);
}

#ifdef __cplusplus
};
#endif

#endif

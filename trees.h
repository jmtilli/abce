#ifndef _TREES_H_
#define _TREES_H_

#include <stddef.h>
#include "datatypes.h"

static inline int
abce_tree_get_str(struct abce *abce, struct abce_mb **mbres,
                  struct abce_mb *mbt, const struct abce_mb *mbkey)
{
  struct rb_tree_node *n;
  if (mbt->typ != ABCE_T_T || mbkey->typ != ABCE_T_SC)
  {
    abort();
  }
  n = RB_TREE_NOCMP_FIND(&mbt->u.area->u.tree.tree, abce_str_cmp_halfsym, NULL, mbkey);
  if (n == NULL)
  {
    return -ENOENT;
  }
  *mbres = &CONTAINER_OF(n, struct abce_mb_rb_entry, n)->val;
  return 0;
}
static inline int
abce_tree_del_str(struct abce *abce,
                  struct abce_mb *mbt, const struct abce_mb *mbkey)
{
  struct rb_tree_node *n;
  struct abce_mb_rb_entry *mbe;
  if (mbt->typ != ABCE_T_T || mbkey->typ != ABCE_T_SC)
  {
    abort();
  }
  n = RB_TREE_NOCMP_FIND(&mbt->u.area->u.tree.tree, abce_str_cmp_halfsym, NULL, mbkey);
  if (n == NULL)
  {
    return -ENOENT;
  }
  mbe = CONTAINER_OF(n, struct abce_mb_rb_entry, n);
  abce_mb_refdn(abce, &mbe->key);
  abce_mb_refdn(abce, &mbe->val);
  rb_tree_nocmp_delete(&mbt->u.area->u.tree.tree, n);
  abce->alloc(mbe, 0, abce->alloc_baton);
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
    abce_mb_refdn(abce, mbres);
    *mbres = abce_mb_refup(abce, mbval);
    return 0;
  }
  e = abce->alloc(NULL, sizeof(*e), abce->alloc_baton);
  if (e == NULL)
  {
    return -ENOMEM;
  }
  e->key = abce_mb_refup(abce, mbkey);
  e->val = abce_mb_refup(abce, mbval);
  if (rb_tree_nocmp_insert_nonexist(&mbt->u.area->u.tree.tree, abce_str_cmp_sym, NULL, &e->n) != 0)
  {
    abort();
  }
  mbt->u.area->u.tree.sz += 1;
  return 0;
}

#endif
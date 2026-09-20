#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "abcerbtree.h"

static int abce_rb_subtree_ptrs_valid(struct abce_rb_tree_node *node)
{
  int resultl = 0, resultr = 0;
  if (node->children[0] != NULL)
  {
    if (node->children[0]->parent != node)
    {
      return 0;
    }
    resultl = abce_rb_subtree_ptrs_valid(node->children[0]);
    if (resultl == 0)
    {
      return 0;
    }
  }
  if (node->children[1] != NULL)
  {
    if (node->children[1]->parent != node)
    {
      return 0;
    }
    resultr = abce_rb_subtree_ptrs_valid(node->children[1]);
    if (resultr == 0)
    {
      return 0;
    }
  }
  return 1;
}

static int abce_rb_subtree_height(struct abce_rb_tree_node *node)
{
  int resultl = 0, resultr = 0;
  if (node->children[0] != NULL)
  {
    if (node->children[0]->parent != node)
    {
      return -1;
    }
    resultl = abce_rb_subtree_height(node->children[0]);
    if (resultl < 0)
    {
      return -1;
    }
  }
  if (node->children[1] != NULL)
  {
    if (node->children[1]->parent != node)
    {
      return -1;
    }
    resultr = abce_rb_subtree_height(node->children[1]);
    if (resultr < 0)
    {
      return -1;
    }
  }
  if (resultl != resultr)
  {
    return -1;
  }
  return node->is_black ? 1 + resultr : resultr;
}

static int __attribute__((unused)) abce_rb_tree_nocmp_ptrs_valid(struct abce_rb_tree_nocmp *tree)
{
  if (tree->root == NULL)
  {
    return 1;
  }
  if (tree->root->parent != NULL)
  {
    return 0;
  }
  if (abce_rb_subtree_ptrs_valid(tree->root) == 0)
  {
    return 0;
  }
  return 1;
}

int abce_rb_tree_nocmp_valid(struct abce_rb_tree_nocmp *tree)
{
  if (tree->root == NULL)
  {
    return 1;
  }
  if (tree->root->parent != NULL)
  {
    return 0;
  }
  if (abce_rb_subtree_height(tree->root) < 0)
  {
    return 0;
  }
  return 1;
}

struct abce_rb_tree_node *abce_rb_tree_nocmp_leftmost(struct abce_rb_tree_nocmp *tree)
{
  struct abce_rb_tree_node *node = tree->root;
  if (node == NULL)
  {
    return NULL;
  }
  for (;;)
  {
    if (node->children[0] == NULL)
    {
      return node;
    }
    node = node->children[0];
  }
}

struct abce_rb_tree_node *abce_rb_tree_nocmp_rightmost(struct abce_rb_tree_nocmp *tree)
{
  struct abce_rb_tree_node *node = tree->root;
  if (node == NULL)
  {
    return NULL;
  }
  for (;;)
  {
    if (node->children[1] == NULL)
    {
      return node;
    }
    node = node->children[1];
  }
}

static inline struct abce_rb_tree_node *sibling(struct abce_rb_tree_node *node)
{
  struct abce_rb_tree_node *p = node->parent;
  if (p == NULL)
  {
    return NULL;
  }
  if (node == p->children[0])
  {
    return p->children[1];
  }
  else
  {
    return p->children[0];
  }
}

static inline struct abce_rb_tree_node *sibling_parent(struct abce_rb_tree_node *node, struct abce_rb_tree_node *p)
{
  if (p == NULL)
  {
    return NULL;
  }
  if (node == p->children[0])
  {
    return p->children[1];
  }
  else
  {
    return p->children[0];
  }
}

static inline struct abce_rb_tree_node *uncle(struct abce_rb_tree_node *node)
{
  struct abce_rb_tree_node *p = node->parent;
  struct abce_rb_tree_node *g = p->parent;
  if (g == NULL)
  {
    return NULL;
  }
  return sibling(p);
}

static inline void rotate_left(struct abce_rb_tree_nocmp *tree, struct abce_rb_tree_node *p)
{
  struct abce_rb_tree_node *parent = p->parent;
  struct abce_rb_tree_node *q = p->children[1];
  struct abce_rb_tree_node *a = p->children[0];
  struct abce_rb_tree_node *b = q->children[0];
  struct abce_rb_tree_node *c = q->children[1];
  //printf("rotating left\n");
  p->children[0] = a;
  if (a)
  {
    a->parent = p;
  }
  p->children[1] = b;
  if (b)
  {
    b->parent = p;
  }
  q->children[0] = p;
  p->parent = q;
  q->children[1] = c;
  if (c)
  {
    c->parent = q;
  }
  if (parent == NULL)
  {
    tree->root = q;
    q->parent = NULL;
    return;
  }
  if (parent->children[0] == p)
  {
    parent->children[0] = q;
    q->parent = parent;
  }
  else if (parent->children[1] == p)
  {
    parent->children[1] = q;
    q->parent = parent;
  }
  else
  {
    abort();
  }
}

static inline void rotate_right(struct abce_rb_tree_nocmp *tree, struct abce_rb_tree_node *q)
{
  struct abce_rb_tree_node *parent = q->parent;
  struct abce_rb_tree_node *p = q->children[0];
  struct abce_rb_tree_node *a = p->children[0];
  struct abce_rb_tree_node *b = p->children[1];
  struct abce_rb_tree_node *c = q->children[1];
  //printf("rotating right\n");
  p->children[0] = a;
  if (a)
  {
    a->parent = p;
  }
  p->children[1] = q;
  q->parent = p;
  q->children[0] = b;
  if (b)
  {
    b->parent = q;
  }
  q->children[1] = c;
  if (c)
  {
    c->parent = q;
  }
  if (parent == NULL)
  {
    tree->root = p;
    p->parent = NULL;
    return;
  }
  if (parent->children[0] == q)
  {
    parent->children[0] = p;
    p->parent = parent;
  }
  else if (parent->children[1] == q)
  {
    parent->children[1] = p;
    p->parent = parent;
  }
  else
  {
    abort();
  }
}

void abce_rb_tree_nocmp_insert_repair(struct abce_rb_tree_nocmp *tree, struct abce_rb_tree_node *node)
{
  if (node->parent == NULL)
  {
    node->is_black = 1;
  }
  else if (node->parent->is_black)
  {
    // Do nothing.
  }
  else if (uncle(node) && uncle(node)->is_black == 0)
  {
    node->parent->is_black = 1;
    uncle(node)->is_black = 1;
    node->parent->parent->is_black = 0;
    abce_rb_tree_nocmp_insert_repair(tree, node->parent->parent); // Tail recursion
  }
  else
  {
    struct abce_rb_tree_node *p = node->parent;
    struct abce_rb_tree_node *g = p->parent;
    //printf("case 4 %p %p\n", p, g);
    if (g->children[0] && node == g->children[0]->children[1])
    {
      //printf("case 4.1\n");
      rotate_left(tree, p);
      node = node->children[0];
    }
    else if (g->children[1] && node == g->children[1]->children[0])
    {
      //printf("case 4.2\n");
      rotate_right(tree, p);
      node = node->children[1]; 
    }
    p = node->parent;
    g = p->parent;
    //printf("case 4 cont %p %p\n", p, g);
    if (node == p->children[0])
    {
      //printf("case 4 step 2.1\n");
      rotate_right(tree, g);
    }
    else
    {
      //printf("case 4 step 2.2\n");
      rotate_left(tree, g);
    }
    p->is_black = 1;
    g->is_black = 0;
  }
}

void abce_rb_tree_insert(struct abce_rb_tree *tree, struct abce_rb_tree_node *node)
{
  struct abce_rb_tree_node *node2;
  node->is_black = 0;
  node->children[0] = NULL;
  node->children[1] = NULL;
  if (tree->nocmp.root == NULL)
  {
    tree->nocmp.root = node;
    node->parent = NULL;
    abce_rb_tree_nocmp_insert_repair(&tree->nocmp, node);
    return;
  }
  node2 = tree->nocmp.root;
  for (;;)
  {
    if (tree->cmp(node, node2, tree->cmp_userdata) < 0)
    {
      if (node2->children[0] == NULL)
      {
        node2->children[0] = node;
        node->parent = node2;
        abce_rb_tree_nocmp_insert_repair(&tree->nocmp, node);
        return;
      }
      node2 = node2->children[0];
    }
    else
    {
      if (node2->children[1] == NULL)
      {
        node2->children[1] = node;
        node->parent = node2;
        abce_rb_tree_nocmp_insert_repair(&tree->nocmp, node);
        return;
      }
      node2 = node2->children[1];
    }
  }
}

static inline int is_leaf(struct abce_rb_tree_node *node)
{
  return node == NULL;
  //return node->children[0] == NULL && node->children[1] == NULL;
}

static void abce_rb_tree_delete_case6(struct abce_rb_tree_nocmp *tree, struct abce_rb_tree_node *n, struct abce_rb_tree_node *parent)
{
  struct abce_rb_tree_node *s = sibling_parent(n, parent);
 
  s->is_black = parent->is_black;
  parent->is_black = 1;
 
  if (n == parent->children[0])
  {
    s->children[1]->is_black = 1;
    rotate_left(tree, parent);
  }
  else
  {
    s->children[0]->is_black = 1;
    rotate_right(tree, parent);
  }
}

static void abce_rb_tree_delete_case5(struct abce_rb_tree_nocmp *tree, struct abce_rb_tree_node *n, struct abce_rb_tree_node *parent)
{
  struct abce_rb_tree_node *s = sibling_parent(n, parent);
 
  if (s->is_black)
  {
    if ((n == parent->children[0]) &&
        (s->children[1] == NULL || s->children[1]->is_black) &&
        (s->children[0] != NULL && s->children[0]->is_black == 0))
    {
      s->is_black = 0;
      s->children[0]->is_black = 1;
      rotate_right(tree, s);
    }
    else if ((n == parent->children[1]) &&
             (s->children[0] == NULL || s->children[0]->is_black) &&
             (s->children[1] != NULL && s->children[1]->is_black == 0))
    {
      s->is_black = 0;
      s->children[1]->is_black = 1;
      rotate_left(tree, s);
    }
  }
  abce_rb_tree_delete_case6(tree, n, parent);
}

static void abce_rb_tree_delete_case4(struct abce_rb_tree_nocmp *tree, struct abce_rb_tree_node *n, struct abce_rb_tree_node *parent)
{
  struct abce_rb_tree_node *s = sibling_parent(n, parent);
  if ((parent->is_black == 0) &&
      (s == NULL || s->is_black) &&
      (s->children[0] == NULL || s->children[0]->is_black) &&
      (s->children[1] == NULL || s->children[1]->is_black))
  {
    s->is_black = 0;
    parent->is_black = 1;
  }
  else
  {
    abce_rb_tree_delete_case5(tree, n, parent);
  }
}

static void abce_rb_tree_delete_case1(struct abce_rb_tree_nocmp *tree, struct abce_rb_tree_node *node, struct abce_rb_tree_node *parent);

static void abce_rb_tree_delete_case3(struct abce_rb_tree_nocmp *tree, struct abce_rb_tree_node *n, struct abce_rb_tree_node *parent)
{
  struct abce_rb_tree_node *s = sibling_parent(n, parent);
  //printf("case3\n");
  if ((parent == NULL || parent->is_black) &&
      (s == NULL || s->is_black) &&
      (s->children[0] == NULL || s->children[0]->is_black) &&
      (s->children[1] == NULL || s->children[1]->is_black))
  {
    s->is_black = 0;
    abce_rb_tree_delete_case1(tree, parent, parent->parent);
  }
  else
  {
    abce_rb_tree_delete_case4(tree, n, parent);
  }
}

static void abce_rb_tree_delete_case2(struct abce_rb_tree_nocmp *tree, struct abce_rb_tree_node *n, struct abce_rb_tree_node *parent)
{
  struct abce_rb_tree_node *s = sibling_parent(n, parent);
  //printf("case2\n");
  if (s && s->is_black == 0)
  {
    parent->is_black = 0;
    s->is_black = 1;
    if (n == parent->children[0])
    {
      rotate_left(tree, parent);
    }
    else
    {
      rotate_right(tree, parent);
    }
  }
  abce_rb_tree_delete_case3(tree, n, parent);
}

static void abce_rb_tree_delete_case1(struct abce_rb_tree_nocmp *tree, struct abce_rb_tree_node *node, struct abce_rb_tree_node *parent)
{
  //printf("case1 %p %p\n", node, parent);
  if (parent != NULL) // XXX
  {
    abce_rb_tree_delete_case2(tree, node, parent);
  }
}

static void __attribute__((unused)) abce_rb_tree_exchange(struct abce_rb_tree_nocmp *tree, struct abce_rb_tree_node *n1, struct abce_rb_tree_node *n2)
{
  struct abce_rb_tree_node *n1_parent = n1->parent;
  struct abce_rb_tree_node *n1_left = n1->children[0];
  struct abce_rb_tree_node *n1_right = n1->children[1];
  int n1_is_black = n1->is_black;
  struct abce_rb_tree_node *n2_parent = n2->parent;
  struct abce_rb_tree_node *n2_left = n2->children[0];
  struct abce_rb_tree_node *n2_right = n2->children[1];
  int n2_is_black = n2->is_black;
  if (n2_parent == n1 && n1->children[0] == n2)
  {
    n1->children[0] = n2_left;
    n1->children[1] = n2_right;
    n1->parent = n2;
    if (n1->children[0])
    {
      n1->children[0]->parent = n1;
    }
    if (n1->children[1])
    {
      n1->children[1]->parent = n1;
    }
    n2->children[0] = n1;
    n2->children[1] = n1_right;
    n2->parent = n1_parent;
    if (n1_right)
    {
      n1_right->parent = n2;
    }
    if (n2->parent == NULL)
    {
      tree->root = n2;
    }
    else if (n2->parent->children[0] == n1)
    {
      n2->parent->children[0] = n2;
    }
    else if (n2->parent->children[1] == n1)
    {
      n2->parent->children[1] = n2;
    }
    else
    {
      printf("shouldn't reach\n");
      abort();
    }
    n1->is_black = n2_is_black;
    n2->is_black = n1_is_black;
    return;
  }
  if (n2_parent == n1 && n1->children[1] == n2)
  {
    n1->children[0] = n2_left;
    n1->children[1] = n2_right;
    n1->parent = n2;
    if (n1->children[0])
    {
      n1->children[0]->parent = n1;
    }
    if (n1->children[1])
    {
      n1->children[1]->parent = n1;
    }
    n2->children[0] = n1_left;
    n2->children[1] = n1;
    n2->parent = n1_parent;
    if (n1_left)
    {
      n1_left->parent = n2;
    }
    if (n2->parent == NULL)
    {
      tree->root = n2;
    }
    else if (n2->parent->children[0] == n1)
    {
      n2->parent->children[0] = n2;
    }
    else if (n2->parent->children[1] == n1)
    {
      n2->parent->children[1] = n2;
    }
    else
    {
      printf("shouldn't reach\n");
      abort();
    }
    n1->is_black = n2_is_black;
    n2->is_black = n1_is_black;
    return;
  }
  if (n1_parent == n2)
  {
    abce_rb_tree_exchange(tree, n2, n1);
    return;
  }
  //printf("exchanging\n");
#if 1
  if (n1_parent)
  {
    if (n1_parent->children[0] == n1)
    {
      n1_parent->children[0] = n2;
    }
    else if (n1_parent->children[1] == n1)
    {
      n1_parent->children[1] = n2;
    }
    else
    {
      printf("1\n");
      abort();
    }
  }
  if (n2_parent)
  {
    if (n2_parent->children[0] == n2)
    {
      n2_parent->children[0] = n1;
    }
    else if (n2_parent->children[1] == n2)
    {
      n2_parent->children[1] = n1;
    }
    else
    {
      printf("2\n");
      abort();
    }
  }
#endif
  n2->parent = n1_parent;
  n2->children[0] = n1_left;
  n2->children[1] = n1_right;
  n1->parent = n2_parent;
  n1->children[0] = n2_left;
  n1->children[1] = n2_right;
  if (n1->children[0])
  {
    n1->children[0]->parent = n1;
  }
  if (n1->children[1])
  {
    n1->children[1]->parent = n1;
  }
  if (n2->children[0])
  {
    n2->children[0]->parent = n2;
  }
  if (n2->children[1])
  {
    n2->children[1]->parent = n2;
  }
  if (n1->parent == NULL)
  {
    if (n2->parent == NULL)
    {
      abort();
    }
    tree->root = n1;
  }
  if (n2->parent == NULL)
  {
    tree->root = n2;
  }
  n1->is_black = n2_is_black;
  n2->is_black = n1_is_black;
}

static void __attribute__((unused)) abce_rb_tree_replace(struct abce_rb_tree_nocmp *tree, struct abce_rb_tree_node *n1, struct abce_rb_tree_node *n2)
{
  struct abce_rb_tree_node *n1_parent = n1->parent;
  struct abce_rb_tree_node *n1_left = n1->children[0];
  struct abce_rb_tree_node *n1_right = n1->children[1];
  //printf("replacing %p\n", n1_parent);
  // substitute n2 into n1's place in the tree
  if (n1_parent)
  {
    if (n1_parent->children[0] == n1)
    {
      n1_parent->children[0] = n2;
    }
    else if (n1_parent->children[1] == n1)
    {
      n1_parent->children[1] = n2;
    }
    else
    {
      printf("1\n");
      abort();
    }
  }
  n2->parent = n1_parent;
  n2->children[0] = n1_left;
  n2->children[1] = n1_right;
  n1->parent = NULL;
  n1->children[0] = NULL;
  n1->children[1] = NULL;
  //printf("%p\n", n2->parent);
  if (n2->children[0])
  {
    n2->children[0]->parent = n2;
  }
  //printf("%p\n", n2->parent);
  if (n2->children[1])
  {
    n2->children[1]->parent = n2;
  }
  //printf("%p\n", n2->parent);
  if (n2->parent == NULL)
  {
    //printf("replacing root\n");
    tree->root = n2;
  }
}

static void abce_rb_tree_delete_one_child(struct abce_rb_tree_nocmp *tree, struct abce_rb_tree_node *node)
{
  /*
   * Precondition: n has at most one non-leaf child.
   */
  struct abce_rb_tree_node *child = is_leaf(node->children[1]) ? node->children[0] : node->children[1];
  //abce_rb_tree_replace(tree, node, child);
  if (node->parent == NULL)
  {
    tree->root = child;
    if (child != NULL)
    {
      child->parent = NULL;
    }
  }
  else if (node->parent->children[0] == node)
  {
    node->parent->children[0] = child;
    if (child != NULL)
    {
      child->parent = node->parent;
    }
  }
  else if (node->parent->children[1] == node)
  {
    node->parent->children[1] = child;
    if (child != NULL)
    {
      child->parent = node->parent;
    }
  }
  if (child != NULL)
  {
    //child->is_black = node->is_black; // XXX
  }
  if (node->is_black)
  {
    if (child && child->is_black == 0)
    {
      child->is_black = 1;
    }
    else
    {
      abce_rb_tree_delete_case1(tree, child, node->parent);
    }
  }
}

void abce_rb_tree_nocmp_delete(struct abce_rb_tree_nocmp *tree, struct abce_rb_tree_node *node)
{
#if 0
  if (node->children[0] == NULL && node->children[1] == NULL)
  {
    if (node->parent == NULL)
    {
      tree->root = NULL;
    }
    else if (node->parent->children[0] == node)
    {
      node->parent->children[0] = NULL;
    }
    else if (node->parent->children[1] == node)
    {
      node->parent->children[1] = NULL;
    }
    else
    {
      abort();
    }
    return;
  }
#endif
  if (!is_leaf(node->children[0]) && !is_leaf(node->children[1]))
  {
    struct abce_rb_tree_node *node2 = node->children[0];
    //struct abce_rb_tree_node *oldright = node->children[1];
    for (;;)
    {
      if (node2->children[1] == NULL)
      {
        break;
      }
      node2 = node2->children[1];
    }
    //printf("node->parent %p\n", node->parent);
#if 0
    if (node->parent == NULL)
    {
      tree->root = node2;
      node2->parent = NULL;
    }
    else if (node->parent->children[0] == node)
    {
      node->parent->children[0] = node2;
      node2->parent = node->parent;
    }
    else if (node->parent->children[1] == node)
    {
      node->parent->children[1] = node2;
      node2->parent = node->parent;
    }
    node2->children[1] = node->children[1];
    node2->children[1]->parent = node2;
#endif
    //abort();
    abce_rb_tree_exchange(tree, node, node2);
    //abort();
#if 0
    if (!abce_rb_tree_ptrs_valid(tree))
    {
      printf("invalid ptrs inside\n");
      return;
      abort();
    }
    if (!abce_rb_tree_valid(tree))
    {
      printf("invalid 1 inside\n");
      return;
      abort();
    }
#endif
    abce_rb_tree_delete_one_child(tree, node);
    //node2->is_black = node->is_black; // XXX
    //abce_rb_tree_replace(tree, node, node2);
    //node2->children[1] = oldright;
    //oldright->parent = node2;
#if 0
    if (!abce_rb_tree_ptrs_valid(tree))
    {
      printf("invalid 2 ptrs inside\n");
      return;
      abort();
    }
    if (!abce_rb_tree_valid(tree))
    {
      printf("invalid 2 inside\n");
      return;
      abort();
    }
#endif
    //printf("valid inside\n");
    return;
  }
  //printf("deleting one child\n");
  abce_rb_tree_delete_one_child(tree, node);
}


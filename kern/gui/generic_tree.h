#ifndef GENERIC_TREE_H
#define GENERIC_TREE_H
#include <list.h>

typedef struct gtreenode {
    list_entry_t siblings;
    list_entry_t childrens;
    struct gtreenode *parent;
    void *value;
} gtreenode_t;

typedef struct gtree {
    gtreenode_t *root;
} gtree_t;

gtree_t *tree_create();

gtreenode_t *treenode_create(void *value);

gtreenode_t *tree_insert(gtree_t *tree, gtreenode_t *subroot, void *value);

gtreenode_t *tree_find_parent(gtreenode_t *remove_node);

void tree_remove(gtreenode_t *remove_node);

void tree2array(gtree_t *tree, void **array, int *size);

#endif

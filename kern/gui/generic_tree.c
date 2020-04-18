#include <generic_tree.h>
#include <list.h>
#include <kmalloc.h>

#define le2gtreenode(le, member) \
    to_struct((le), gtreenode_t, member)

/*
 * Create a tree with root = NULL
 * */
gtree_t * tree_create() {
    return (gtree_t*)kcalloc(sizeof(gtree_t), 1);
}

/*
 * Create a tree node with specified value, and a list of 0 childrens
 * */
gtreenode_t * treenode_create(void * value) {
    gtreenode_t * n = kcalloc(sizeof(gtreenode_t), 1);
    n->value = value;
    list_init(&(n->childrens));
    list_init(&(n->siblings));
    n->parent = NULL;
    return n;
}

/*
 * Insert a node under subroot
 * */
gtreenode_t *tree_insert(gtree_t *tree, gtreenode_t *subroot, void *value) {
    // Create a treenode
    gtreenode_t *treenode = treenode_create(value);

    // Insert it
    if(tree->root == NULL) {
        assert(subroot == NULL);
        tree->root = treenode;
        return treenode;
    } else if (subroot == NULL) {
        cprintf("gtree %p had a root but insert another\n", tree);
        assert(0);
    }
    list_add_before(&(subroot->childrens), &(treenode->siblings));
    treenode->parent = subroot;
    return treenode;
}

gtreenode_t *tree_find_parent(gtreenode_t *node) {
    return node->parent;
}

void tree_remove(gtreenode_t *remove_node) {
    if (remove_node == NULL) {
        return ;
    }
    list_del(&(remove_node->siblings));
    list_entry_t *list = &(remove_node->childrens);
    list_entry_t *p;
    while (!list_empty(list)) {
        p = list_next(list);
        list_del(p);
        gtreenode_t *child_node = le2gtreenode(p, siblings);
        tree_remove(child_node);
    }
    kfree(remove_node);
}

void tree2array(gtreenode_t *subroot, void ** array, int * size) {
    if(subroot == NULL) {
        return;
    }
    void *curr_val = (void*)subroot->value;
    array[*size] = curr_val;
    *size = *size + 1;
    list_entry_t *list = &(subroot->childrens);
    list_entry_t *p = list;
    while ((p = list_next(p)) != list) {
        gtreenode_t *child_node = le2gtreenode(p, siblings);
        tree2array(child_node, array, size);
    }
}


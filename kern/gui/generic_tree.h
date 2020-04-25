#ifndef GENERIC_TREE_H
#define GENERIC_TREE_H
#include <x86.h>
#include <kmalloc.h>

typedef struct listnode {
	struct listnode * prev;
	struct listnode * next;
	void * val;
}listnode_t;

typedef struct list{
	listnode_t * head;
	listnode_t * tail;
	uint32_t size;
}list_t;

#define foreach(t, list) for(listnode_t * t = list->head; t != NULL; t = t->next)

list_t * list_create();

uint32_t list_size(list_t * list);

listnode_t * list_insert_front(list_t * list, void * val);

void list_insert_back(list_t * list, void * val);

void * list_remove_node(list_t * list, listnode_t * node);

void * list_remove_front(list_t * list);

void * list_remove_back(list_t * list);

void list_push(list_t * list, void * val);

listnode_t * list_pop(list_t * list);

void list_enqueue(list_t * list, void * val);

listnode_t * list_dequeue(list_t * list);

void * list_peek_front(list_t * list);

void * list_peek_back(list_t * list);

void list_destroy(list_t * list);

void listnode_destroy(listnode_t * node);

int list_contain(list_t * list, void * val);

listnode_t * list_get_node_by_index(list_t * list, int index);

void * list_remove_by_index(list_t * list, int index);

typedef struct gtreenode {
    list_t * children;
    void * value;
}gtreenode_t;

typedef struct gtree {
    gtreenode_t * root;
}gtree_t;

gtree_t * tree_create();

gtreenode_t * treenode_create(void * value);

gtreenode_t * tree_insert(gtree_t * tree, gtreenode_t * subroot, void * value);

gtreenode_t * tree_find_parent(gtree_t * tree, gtreenode_t * remove_node, int * child_index);

gtreenode_t * tree_find_parent_recur(gtree_t * tree, gtreenode_t * remove_node, gtreenode_t * subroot, int * child_index);

void tree_remove(gtree_t * tree, gtreenode_t * remove_node);

void tree2list_recur(gtreenode_t * subroot, list_t * list);

void tree2list(gtree_t * tree, list_t * list);

void tree2array(gtree_t * tree, void ** array, int * size);

void tree2array_recur(gtreenode_t * subroot, void ** array, int * size);

#endif

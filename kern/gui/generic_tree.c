#include <generic_tree.h>

/*
 * An implementation for generic, doubly linked list, may be handy in the future when we do vfs, process manmagement, etc..
 * */

/*
 * Create a list and set head, tail to NULL, and size to 0
 * */
list_t * list_create() {
	list_t * list = kcalloc(sizeof(list_t), 1);
	return list;
}

/*
 * Get list size
 * */
uint32_t list_size(list_t * list) {
    if(!list) return 0;
	return list->size;
}

/*
 * Given a listnode, remove it from lis
 * */
void * list_remove_node(list_t * list, listnode_t * node) {
    void * val = node->val;
    if(list->head == node)
        return list_remove_front(list);
    else if(list->tail == node)
        return list_remove_back(list);
    else {
        node->next->prev = node->prev;
        node->prev->next = node->next;
        list->size--;
        kfree(node);
    }
    return val;
}
/*
 * Insert a value at the front of list
 * */
listnode_t * list_insert_front(list_t * list, void * val) {
	listnode_t * t = kcalloc(sizeof(listnode_t), 1);
	list->head->prev = t;
    t->next = list->head;
	t->val = val;

	// If it's the first element, then it's both head and tail
	if(!list->head)
		list->tail = t;

	list->head = t;
	list->size++;
	return t;
}

/*
 * Insert a value at the back of list
 * */
void list_insert_back(list_t * list, void * val) {
	listnode_t * t = kcalloc(sizeof(listnode_t), 1);
	t->prev = list->tail;
    if(list->tail)
        list->tail->next = t;
	t->val = val;

	if(!list->head)
		list->head = t;

	list->tail = t;
	list->size++;
}

/*
 * Remove a value at the front of list
 * */
void * list_remove_front(list_t * list) {
	if(!list->head) return NULL;
	listnode_t * t = list->head;
    void * val = t->val;
	list->head = t->next;
	if(list->head)
		list->head->prev = NULL;
	kfree(t);
	list->size--;
    return val;
}

/*
 * Remove a value at the back of list
 * */
void * list_remove_back(list_t * list) {
	if(!list->head) return NULL;
	listnode_t * t = list->tail;
    void * val = t ->val;
	list->tail = t->prev;
	if(list->tail)
		list->tail->next = NULL;
	kfree(t);
	list->size--;
    return val;
}

/*
 * Insert after tail of list(same as insert back)
 * */
void list_push(list_t * list, void * val) {
	list_insert_back(list, val);
}

/*
 * Remove and return tail of list(user is responsible for freeing the returned node and the value)
 * */
listnode_t * list_pop(list_t * list) {
	if(!list->head) return NULL;
	listnode_t * t = list->tail;
	list->tail = t->prev;
	if(list->tail)
		list->tail->next = NULL;
	list->size--;
	return t;
}

/*
 * Insert before head of list(same as insert front)
 * */
void list_enqueue(list_t * list, void * val) {
	list_insert_front(list, val);
}

/*
 * Remove and return tail of list(same as list_pop
 * */
listnode_t * list_dequeue(list_t * list) {
	return list_pop(list);
}

/*
 * Get the value of the first element but not remove it
 * */
void * list_peek_front(list_t * list) {
	if(!list->head) return NULL;
	return list->head->val;
}

/*
 * Get the value of the last element but not remove it
 * */
void * list_peek_back(list_t * list) {
	if(!list->tail) return NULL;
	return list->tail->val;
}


/*
 * Does the list contain a value
 * Return -1 if list element is not found
 * */

int list_contain(list_t * list, void * val) {
    int idx = 0;
    foreach(listnode, list) {
        if(listnode->val == val)
            return idx;
        idx++;
    }
    return -1;
}

listnode_t * list_get_node_by_index(list_t * list, int index) {
    if(index < 0 || index >= list_size(list))
        return NULL;
    int curr = 0;
    foreach(listnode, list) {
        if(index == curr) return listnode;
        curr++;
    }
    return NULL;
}

void * list_remove_by_index(list_t * list, int index) {
    listnode_t * node = list_get_node_by_index(list, index);
    return list_remove_node(list, node);
}

void list_destroy(list_t * list) {
	// Free each node's value and the node itself
	listnode_t * node = list->head;
	while(node != NULL) {
		listnode_t * save = node;
		node = node->next;
		kfree(save);
	}
	// Free the list
	kfree(list);
}

void listnode_destroy(listnode_t * node) {
	kfree(node);
}

/*
 * Create a tree with root = NULL
 * */
gtree_t * tree_create() {
    return (gtree_t*)kcalloc(sizeof(gtree_t), 1);
}

/*
 * Create a tree node with specified value, and a list of 0 children
 * */
gtreenode_t * treenode_create(void * value) {
    gtreenode_t * n = kcalloc(sizeof(gtreenode_t), 1);
    n->value = value;
    n->children = list_create();
    return n;
}

/*
 * Insert a node under subroot
 * */
gtreenode_t * tree_insert(gtree_t * tree, gtreenode_t * subroot, void * value) {
    // Create a treenode
    gtreenode_t * treenode = kcalloc(sizeof(gtreenode_t), 1);
    treenode->children = list_create();
    treenode->value = value;

    // Insert it
    if(!tree->root) {
        tree->root = treenode;
        return treenode;
    }
    list_insert_front(subroot->children, treenode);
    return treenode;
}

gtreenode_t * tree_find_parent(gtree_t * tree, gtreenode_t * remove_node, int * child_index) {
    // If subroot is the parent
    if(remove_node == tree->root) return NULL;
    return tree_find_parent_recur(tree, remove_node, tree->root, child_index);
}

gtreenode_t * tree_find_parent_recur(gtree_t * tree, gtreenode_t * remove_node, gtreenode_t * subroot, int * child_index) {
    int idx;
    if((idx = list_contain(subroot->children, remove_node)) != -1) {
        *child_index = idx;
        return subroot;
    }
    foreach(child, subroot->children) {
        gtreenode_t * ret = tree_find_parent_recur(tree, remove_node, child->val, child_index);
        if(ret != NULL) {
            return ret;
        }
        // ret is NULL, keep searching.
    }
    return NULL;
}

void tree_remove(gtree_t * tree, gtreenode_t * remove_node) {
    // Search for tree's parent and remove the node from parent's children list
    // If parent is NULL, then just set tree->root to NULL(yeah, I dont care about mem leaks)
    int child_index = -1;
    gtreenode_t * parent = tree_find_parent(tree, remove_node, &child_index);
    // Do treenode remove in here:
    if(parent != NULL) {
        gtreenode_t * freethis = list_remove_by_index(parent->children, child_index);
        // Free tree node here
        kfree(freethis);
    }
}

void tree2list_recur(gtreenode_t * subroot, list_t * list) {
    if(subroot== NULL)
        return;
    foreach(child, subroot->children) {
        gtreenode_t * curr_treenode = (gtreenode_t*)child->val;
        void * curr_val = curr_treenode->value;
        list_insert_back(list, curr_val);
        tree2list_recur(child->val, list);
    }
}

void tree2list(gtree_t * tree, list_t * list) {
    tree2list_recur(tree->root, list);
}



void tree2array(gtree_t * tree, void ** array, int * size) {
    tree2array_recur(tree->root, array, size);
}

void tree2array_recur(gtreenode_t * subroot, void ** array, int * size) {
    if(subroot== NULL)
        return;
    void * curr_val = (void*)subroot->value;
    array[*size] = curr_val;
    *size = *size + 1;
    foreach(child, subroot->children) {
        tree2array_recur(child->val, array, size);
    }
}


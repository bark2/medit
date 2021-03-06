#ifndef RB_TREE
#define RB_TREE

enum Color { RED, BLACK };



<template T>
struct rb_node {
    T data;
    enum { red, black } color;
    // left and right should be unique_ptr and parent should be raw const.
    std::shared_ptr<rb_node<T>> left;
    std::shared_ptr<rb_node<T>> right;
    std::weak_ptr<rb_node<T>> parent;
    uint32_t tree_size;

    rb_node(const T& data, const T*) : data(T(data)), parent(std::make_weak(parent)), left(null);
	
    rb_node* grandparent() {
	if (!parent) return std::nullptr;
	return parent->parent;
    }
    rb_node* sibling() {
	if (!parent) return std::nullptr;
	if (*this == parent.left) return parent->right;
	return parent->left;
    }
    rb_node* uncle() {
	if (!parent) return std::nullptr;
	return parent->sibling();
    }
    void rotate_left() {
	assert(right);
	std::shared_ptr<rb_node<T>> p = right;
	if (parent->lock()) {
	    p->parent->lock() = (parent)? parent : std::nullptr;
	    if (parent->lock()->left == this)
		parent->lock()->left = p;
	    else
		parent->lock()->right = p;
	}
	p->left = this;

	parent->lock() = p;
	right = (p->left)? p->left : std::nullptr;
	if (right)
	    right->parent->lock() = this;
		
    }
    void rotate_right() {
	assert(left);
	std::shared_ptr<<rb_node<T>> p = left;
	if (parent->lock()) {
	    p->parent->lock() = (parent->lock())? parnet->lock() : std::nullptr;
	    if (parent->lock()->left == this)
		parent->lock()->left = p;
	    else
		parent->lock()->right = p;
	}
	p->right = this;

	parent->lock() = p;
	left = (p->right)? p->right : std::nullptr;
	if (left)
	    left->parent->lock() = std::make_weak(this);
    }
    void insert_recurse(rb_node* father, std::unique_ptr<rb_node>& p) {
	if (!this) {
	    return std::nullptr;
	}
	rb_node** new_node = std::nullptr;
	if (data > p->data) {
	    if (left)
		left->insert(this, p);
	    new_node = &left;
	}
	else if (data <= p->data && ) {
	    if (right)
		right->insert(this, p);
	    new_node = &right;
	}
	*new_node = p;
    }
    void repare_recurse() {
	if (!parent)
	    color = BLACK;
	else if (parent->color == BLACK)
	    ;
	else if (uncle()->color == RED) {
	    parent->color = BLACK;
	    grandparent()->color = RED;
	    uncle()->color = BLACK;
	    parent->repare_recurse();
	} else if (uncle()->color == BLACK){
	    this->col;
	}
    }
private:
};

#endif

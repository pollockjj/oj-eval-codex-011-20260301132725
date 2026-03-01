#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {
/**
 * @brief a container like std::priority_queue which is a heap internal.
 * **Exception Safety**: The `Compare` operation might throw exceptions for certain data.
 * In such cases, any ongoing operation should be terminated, and the priority queue should be restored to its original state before the operation began.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
	struct node {
		T value;
		node *left;
		node *right;
		size_t dist;
		explicit node(const T &v) : value(v), left(NULL), right(NULL), dist(1) {}
	};

	node *root;
	size_t node_count;

	static size_t get_dist(node *p) {
		return p == NULL ? 0 : p->dist;
	}

	static void swap_node_ptr(node *&a, node *&b) {
		node *tmp = a;
		a = b;
		b = tmp;
	}

	static void clear(node *p) {
		if (p == NULL) return;
		clear(p->left);
		clear(p->right);
		delete p;
	}

	static node *clone(node *p) {
		if (p == NULL) return NULL;
		node *res = new node(p->value);
		try {
			res->left = clone(p->left);
			res->right = clone(p->right);
		} catch (...) {
			clear(res);
			throw;
		}
		res->dist = p->dist;
		return res;
	}

	node *merge_nodes(node *a, node *b) {
		if (a == NULL) return b;
		if (b == NULL) return a;
		if (Compare()(a->value, b->value)) swap_node_ptr(a, b);
		node *merged_right = merge_nodes(a->right, b);
		a->right = merged_right;
		if (get_dist(a->left) < get_dist(a->right)) swap_node_ptr(a->left, a->right);
		a->dist = get_dist(a->right) + 1;
		return a;
	}

public:
	/**
	 * @brief default constructor
	 */
	priority_queue() : root(NULL), node_count(0) {}

	/**
	 * @brief copy constructor
	 * @param other the priority_queue to be copied
	 */
	priority_queue(const priority_queue &other) : root(NULL), node_count(other.node_count) {
		root = clone(other.root);
	}

	/**
	 * @brief deconstructor
	 */
	~priority_queue() {
		clear(root);
	}

	/**
	 * @brief Assignment operator
	 * @param other the priority_queue to be assigned from
	 * @return a reference to this priority_queue after assignment
	 */
	priority_queue &operator=(const priority_queue &other) {
		if (this == &other) return *this;
		priority_queue tmp(other);
		node *old_root = root;
		root = tmp.root;
		tmp.root = old_root;
		size_t old_count = node_count;
		node_count = tmp.node_count;
		tmp.node_count = old_count;
		return *this;
	}

	/**
	 * @brief get the top element of the priority queue.
	 * @return a reference of the top element.
	 * @throws container_is_empty if empty() returns true
	 */
	const T & top() const {
		if (root == NULL) throw container_is_empty();
		return root->value;
	}

	/**
	 * @brief push new element to the priority queue.
	 * @param e the element to be pushed
	 */
	void push(const T &e) {
		node *new_node = new node(e);
		try {
			root = merge_nodes(root, new_node);
			++node_count;
		} catch (...) {
			delete new_node;
			throw runtime_error();
		}
	}

	/**
	 * @brief delete the top element from the priority queue.
	 * @throws container_is_empty if empty() returns true
	 */
	void pop() {
		if (root == NULL) throw container_is_empty();
		try {
			node *new_root = merge_nodes(root->left, root->right);
			node *old_root = root;
			root = new_root;
			delete old_root;
			--node_count;
		} catch (...) {
			throw runtime_error();
		}
	}

	/**
	 * @brief return the number of elements in the priority queue.
	 * @return the number of elements.
	 */
	size_t size() const {
		return node_count;
	}

	/**
	 * @brief check if the container is empty.
	 * @return true if it is empty, false otherwise.
	 */
	bool empty() const {
		return node_count == 0;
	}

	/**
	 * @brief merge another priority_queue into this one.
	 * The other priority_queue will be cleared after merging.
	 * The complexity is at most O(logn).
	 * @param other the priority_queue to be merged.
	 */
	void merge(priority_queue &other) {
		if (this == &other || other.root == NULL) return;
		try {
			root = merge_nodes(root, other.root);
			node_count += other.node_count;
			other.root = NULL;
			other.node_count = 0;
		} catch (...) {
			throw runtime_error();
		}
	}
};

}

#endif

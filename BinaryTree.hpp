#pragma once
#ifndef __BITREE_HPP__
#define __BITREE_HPP__

#include <cstddef>
#include <iterator>
#include <algorithm>
#include <typeinfo>

template<typename _Ty>
struct _Node {
	typedef std::size_t size_type;
	typedef _Ty value_type;
	typedef _Node<_Ty> this_type;
	
	_Node() {}
	_Node(const value_type &_Value, this_type *_Left = nullptr, this_type *_Right = nullptr) : value(_Value), left(_Left), right(_Right), parent(nullptr) {
		if (left)
			left->parent = this;
		if (right)
			right->parent = this;
	}

	static size_type count(this_type *node) {
		return node ? count(node->left) + count(node->right) + 1 : 0;
	}

	static size_type depth(this_type *node) {
		return node ? std::max(depth(node->left), depth(node->right)) + 1 : 0;
	}

	value_type value;
	this_type
		*parent,
		*left,
		*right;
};

template<typename _Node_Ty>
class _Iterator : public std::iterator<std::forward_iterator_tag, typename _Node_Ty::value_type> {
public:
	typedef _Iterator this_type;
	typedef _Node_Ty node_type;
	typedef typename node_type::value_type value_type;

	static const this_type null_iterator;

	_Iterator() : _ptr(nullptr) {}
	_Iterator(const this_type &it) : _ptr(it._ptr) {}
	_Iterator(node_type &n) : _ptr(&n) {}
	_Iterator(node_type *n) : _ptr(n) {}

	this_type & operator ++ () {
		increment();
		return *this;
	};

	this_type operator ++ (int) {
		this_type ret = *this;
		++(*this);
		return ret;
	}

	value_type &operator -> () const {
		return _ptr->value;
	}

	value_type &operator * () const {
		return _ptr->value;
	}

	this_type &operator = (const this_type &other) {
		_ptr = other._ptr;
		return *this;
	}

	bool operator == (const this_type &other) const {
		return _ptr == other._ptr;
	}

	bool operator != (const this_type &other) const {
		return !(*this == other);
	}

	this_type parent() const {
		return this_type(_ptr->parent);
	}

	this_type left() const {
		return this_type(_ptr->left);
	}

	this_type right() const {
		return this_type(_ptr->right);
	}

protected:
	node_type *_ptr;

	virtual void increment() {}
};

template<typename _Node_Ty>
const _Iterator<_Node_Ty> _Iterator<_Node_Ty>::null_iterator = _Iterator<_Node_Ty>();

template<typename _Node_Ty>
class _PreOrder_Iterator : public _Iterator < _Node_Ty > {
public:
	typedef _Iterator<_Node_Ty> base_type;
	typedef _PreOrder_Iterator this_type;
	typedef typename base_type::node_type node_type;

	_PreOrder_Iterator() : base_type() {}
	_PreOrder_Iterator(const base_type &it) : base_type(it) {}
	_PreOrder_Iterator(const this_type &it) : base_type(it) {}
	_PreOrder_Iterator(node_type &n) : base_type(n) {}
	_PreOrder_Iterator(node_type *n) : base_type(n) {}

private:
	void increment(){
		if (base_type::_ptr->left)
			base_type::_ptr = base_type::_ptr->left;
		else if (base_type::_ptr->right)
			base_type::_ptr = base_type::_ptr->right;
		else
			while (base_type::_ptr) {
				if (base_type::_ptr->parent && base_type::_ptr == base_type::_ptr->parent->left && base_type::_ptr->parent->right) {
					base_type::_ptr = base_type::_ptr->parent->right;
					break;
				}
				base_type::_ptr = base_type::_ptr->parent;
			}
	}
};

template<typename _Node_Ty>
class _InOrder_Iterator : public _Iterator < _Node_Ty > {
public:
	typedef _Iterator<_Node_Ty> base_type;
	typedef _InOrder_Iterator this_type;
	typedef typename base_type::node_type node_type;

	_InOrder_Iterator() : base_type() {}
	_InOrder_Iterator(const base_type &it) : base_type(it) {}
	_InOrder_Iterator(const this_type &it) : base_type(it) {}
	_InOrder_Iterator(node_type &n) : base_type(n) {}
	_InOrder_Iterator(node_type *n) : base_type(n) {}

private:
	void increment() {
		if (base_type::_ptr->right) {
			base_type::_ptr = base_type::_ptr->right;
			while (base_type::_ptr->left)
				base_type::_ptr = base_type::_ptr->left;
		}
		else {
			node_type *tnode = base_type::_ptr->parent;
			while (tnode && base_type::_ptr == tnode->right) {
				base_type::_ptr = tnode;
				tnode = tnode->parent;
			}
			if (base_type::_ptr->right != tnode)
				base_type::_ptr = tnode;
		}
	}
};

template<typename _Node_Ty>
class _PostOrder_Iterator : public _Iterator < _Node_Ty > {
public:
	typedef _Iterator<_Node_Ty> base_type;
	typedef _PostOrder_Iterator this_type;
	typedef typename base_type::node_type node_type;

	_PostOrder_Iterator() : base_type() {}
	_PostOrder_Iterator(const base_type &it) : base_type(it) {}
	_PostOrder_Iterator(const this_type &it) : base_type(it) {}
	_PostOrder_Iterator(node_type &n) : base_type(n) {}
	_PostOrder_Iterator(node_type *n) : base_type(n) {}

private:
	void increment() {
		if (base_type::_ptr->parent) {
			if (base_type::_ptr == base_type::_ptr->parent->left && base_type::_ptr->parent->right) {
				base_type::_ptr = base_type::_ptr->parent->right;
				while (base_type::_ptr->left)
					base_type::_ptr = base_type::_ptr->left;
			}
			else
				base_type::_ptr = base_type::_ptr->parent;
		}
		else
			base_type::_ptr = nullptr;
	}
};

template<typename _Ty, typename _Node_Ty>
class _Binary_Tree {
public:
	typedef _Binary_Tree this_type;
	typedef _Node_Ty node_type;
	typedef typename node_type::size_type size_type;
	typedef typename node_type::value_type value_type;

	struct iterator_category {
		typedef _PreOrder_Iterator<node_type> preorder_iterator;
		typedef _InOrder_Iterator<node_type> inorder_iterator;
		typedef _PostOrder_Iterator<node_type> postorder_iterator;
	};

	_Binary_Tree() : _root(nullptr) {}

	template<typename _Input_Iterator>
	_Binary_Tree(_Input_Iterator First, _Input_Iterator Last, const value_type &Null_marker) : this_type() {
		construct(First, Last, Null_marker);
	}

	_Binary_Tree(const this_type &Other) {
		deep_copy(Other.begin<typename iterator_category::preorder_iterator>(), _root, nullptr);
	}

	template<typename _TIterator>
	_Binary_Tree(_TIterator iter) {
		deep_copy(iter, _root, nullptr);
	}

	~_Binary_Tree() {
		destruct(_root);
	}

	template<typename _Input_Iterator>
	void construct(_Input_Iterator First, _Input_Iterator Last, const value_type &Null_marker) {
		node_type *troot = new node_type(),
			*pnode = troot,
			**tnode = &(pnode->left);
		for (; First != Last; ++First) {
			if (*tnode == pnode->left && *First == Null_marker) {
				tnode = &(pnode->right);
				++First;
			}
			if (*tnode == pnode->right && *First == Null_marker) {
				for (; pnode->parent != troot; pnode = pnode->parent)
					if (!(pnode->parent->right)) {
						pnode = pnode->parent;
						break;
					}
				tnode = &(pnode->right);
				continue;
			}
			(*tnode = new node_type(*First))->parent = pnode;
			(*tnode == pnode->left ? pnode->left : pnode->right) = *tnode;
			tnode = &((pnode = *tnode)->left);
		}
		_root = troot->left;
		delete _root->parent;
		_root->parent = nullptr;
	}

	template<typename _TIterator>
	this_type subtree(_TIterator iter) const {
		return this_type(iter);
	}

	template<typename _TIterator>
	_TIterator begin() const {
		node_type *tnode = _root;
		if (typeid(_TIterator) == typeid(typename iterator_category::preorder_iterator))
			;
		else if (typeid(_TIterator) == typeid(typename iterator_category::inorder_iterator))
			while (tnode != nullptr && tnode->left)
				tnode = tnode->left;
		else if (typeid(_TIterator) == typeid(typename iterator_category::postorder_iterator))
			while (tnode != nullptr && (tnode->left || tnode->right)) {
				if (tnode->left) {
					tnode = tnode->left;
					continue;
				}
				if (tnode->right)
					tnode = tnode->right;
			}
		else
			;
		return _TIterator(*tnode);
	}

	template<typename _TIterator>
	_TIterator end() const {
		return _TIterator();
	}

private:
	node_type *_root;

	template<typename _TIterator>
	void deep_copy(_TIterator iter, node_type *&tnode, node_type *pnode) {
		if (iter == _TIterator::null_iterator)
			return;
		tnode = new node_type(*iter);
		tnode->parent = pnode;
		deep_copy(iter.left(), tnode->left, tnode);
		deep_copy(iter.right(), tnode->right, tnode);
	}

	void destruct(node_type *dnode) {
		if (dnode == nullptr)
			return;
		destruct(dnode->left);
		destruct(dnode->right);
		delete dnode;
	}
};

struct _Iterator_TraversalOrder {
	struct preorder;
	struct inorder;
	struct postorder;
};

struct _Iterator_TraversalOrder::preorder : public _Iterator_TraversalOrder{};
struct _Iterator_TraversalOrder::inorder : public _Iterator_TraversalOrder{};
struct _Iterator_TraversalOrder::postorder : public _Iterator_TraversalOrder{};

template<typename _Ty, typename _Traversal_Order>
struct _Iterator_Traits {};

template<typename _Ty>
struct _Iterator_Traits<_Ty, _Iterator_TraversalOrder::preorder> {
	typedef _PreOrder_Iterator<_Ty> iterator;
};

template<typename _Ty>
struct _Iterator_Traits<_Ty, _Iterator_TraversalOrder::inorder> {
	typedef _InOrder_Iterator<_Ty> iterator;
};

template<typename _Ty>
struct _Iterator_Traits<_Ty, _Iterator_TraversalOrder::postorder> {
	typedef _PostOrder_Iterator<_Ty> iterator;
};

template<typename _Ty>
class BiTree : public _Binary_Tree < _Ty, _Node<_Ty> > {
public:
	typedef BiTree this_type;
	typedef _Binary_Tree < _Ty, _Node<_Ty> > base_type;
	typedef typename base_type::size_type size_type;
	typedef typename base_type::value_type value_type;

	typedef _Iterator_TraversalOrder traversal_order;
	typedef traversal_order::preorder default_traversal_order;
	template<typename _Traversal_Order = default_traversal_order>
	using iterator = typename _Iterator_Traits < typename base_type::node_type, _Traversal_Order >::iterator;

	BiTree() : base_type() {}

	template<typename _Input_Iterator>
	BiTree(_Input_Iterator First, _Input_Iterator Last, const value_type &Null_marker) : base_type(First, Last, Null_marker) {}

	BiTree(const base_type &Other) : base_type(Other) {}

	template<typename _Traversal_Iterator>
	size_type count(_Traversal_Iterator iter) const {
		return iter == _Traversal_Iterator::null_iterator ? 0 : count(iter.left()) + count(iter.right()) + 1;
	}

	template<typename _Traversal_Order = default_traversal_order>
	iterator<_Traversal_Order> begin() const {
		return base_type::template begin<iterator<_Traversal_Order> >();
	}

	template<typename _Traversal_Order = default_traversal_order>
	iterator<_Traversal_Order> end() const {
		return base_type::template end<iterator<_Traversal_Order> >();
	}

	size_type count() const {
		return count(begin());
	}

	template<typename _Traversal_Iterator>
	size_type depth(_Traversal_Iterator iter) const {
		return iter == _Traversal_Iterator::null_iterator ? 0 : std::max(depth(iter.left()), depth(iter.right())) + 1;
	}

	size_type depth() const {
		return depth(begin());
	}

	bool empty() const {
		return begin() == end();
	}

	template<typename _TIterator>
	_TIterator parent(_TIterator iter) const {
		return iter.parent();
	}

	template<typename _Traversal_Order = default_traversal_order>
	iterator<_Traversal_Order> find(const value_type &val) const {
		iterator<_Traversal_Order> ret;
		for (iterator<_Traversal_Order> it = begin<_Traversal_Order>(); it != end<_Traversal_Order>(); ++it) {
			if (*it == val) {
				ret = it;
				break;
			}
		}
		return ret;
	}
};

#endif

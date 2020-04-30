/**
 * implement a container like std::map
 * Based on Scape_goat_Tree
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <map>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

// 两个compare 不同的map 不能相互赋值
template<class Key, class T, class Compare = std::less<Key> > 
class map {
	friend class iterator;
	friend class const_iterator;
public:
	typedef pair<const Key, T> value_type;
	class Tree
	{
	public:
		Tree *_l, *_r;
		value_type *_data;   // TODO: 有点疑惑 ！    应该是为了迭代器的 -> 操作, 不然 应该会有memory_leak
		int _size;           //  the total num of the nodes in the subtree, both existing and non-existent.
		bool _exist;         //  whether this node exists.
		int _fact;           //  Actual number of nodes in the subtree

		Tree (value_type* _val = nullptr) : _size(1), _exist(true), _fact(1), _l(nullptr), _r(nullptr), _data(_val) {
			if (_val)	_data = new value_type(*_val);
		}
		Tree (const Tree &other) : _size(other._size), _exist(other._exist), _fact(other._fact), _l(nullptr), _r(nullptr) {
			if (other._data == nullptr) _data = nullptr;
 			else _data = new value_type(*(other._data));
		}
		~Tree() {
			delete _data;
			_data = nullptr;
		}
		void ReConstruct() {
			_l = nullptr;
			_r = nullptr;
			_size = _fact = 1;
		}
	};
private:
	constexpr static double alpha = 0.77;
	constexpr static double beta = 0.35;
	const int total = 100000;
	int rec_cnt, del_cnt;
	Compare cmp;
	Tree *_root;
	Tree ** _rec_array;
	Tree ** _del_array;

public:   // TODO 为什么一定要public
	class const_iterator;
	class iterator {
		friend class map;
		Tree *_root;
		Tree *_self;
	public:
		iterator(Tree *A = nullptr, Tree *B = nullptr): _root(A), _self(B) {}
		iterator(const iterator &other) {
			_root = other._root;
			_self = other._self;
		}
		// iter ++
		iterator operator++(int) {
			iterator tmp(*this);
			++ (*this);
			return tmp;
		}
		// ++ iter
		iterator & operator++() {
			if (_self == nullptr) throw invalid_iterator();
			do find_upper();	while (_self && !(_self -> _exist));
			return *this;
		}
		iterator operator--(int) {
			iterator tmp(*this);
			-- (*this);
			return tmp;
		}
		iterator & operator--() {
			do find_lower();	while (_self && !(_self -> _exist));
			if (_self == nullptr) throw	invalid_iterator();
			return *this;
		}
		value_type & operator*() const {
			return *(_self -> _data);
		}
		bool operator==(const iterator &rhs) const {
			return (_root == rhs._root && _self == rhs._self);
		}
		bool operator==(const const_iterator &rhs) const {
			return (_root == rhs._root && _self == rhs._self);
		}
		bool operator!=(const iterator &rhs) const {
			return (_root != rhs._root || _self != rhs._self);
		}
		bool operator!=(const const_iterator &rhs) const {
			return (_root != rhs._root || _self != rhs._self);
		}
		// 可以通过这两种方式来修改 元素的值
		value_type* operator->() const noexcept {
			return _self -> _data;
		}
	private:
		// 找第一个key值大于它的结点
		void find_upper() {
			Compare cmp;
			Tree *tp = _root;
			Tree *rec = nullptr;
			if (_self && _self -> _r) {  // 如果有右子树， 那么走到右边，然后一直找左子树即可
				_self = _self -> _r;
				while (_self -> _l) _self = _self -> _l;
			}
			else {   	// 如果没有， 从根开始往_self 走， 遇到key 大于它的就更新， 否则直接走， 
				while (tp) {
					if (cmp(tp -> _data -> first, _self -> _data -> first)) {
						tp = tp -> _r;
					}
					else if (cmp(_self -> _data -> first, tp -> _data -> first)) {
						rec = tp;
						tp = tp -> _l;
					}
					else break;
				}
				_self = rec;
			}
		}
		
		// 第一个 Key 小于 _self 的结点
		void find_lower() {
			// 可能当前 iterator == end() , 所以需特判
			Compare cmp;
			if (_self == nullptr) {
				_self = _root;
				while (_self && _self -> _r) _self = _self -> _r;
			}
			else {
				if (_self -> _l) {
					_self = _self -> _l;
					while (_self -> _r) _self = _self -> _r;
				}
				else {
					// 找路径上第一个小于它的 key
					Tree *tp = _root;
					Tree *rec = nullptr;
					while (tp) {
						if (cmp(tp -> _data -> first, _self -> _data -> first)) {
							rec = tp;
							tp = tp -> _r;
						}
						else if (cmp(_self -> _data -> first, tp -> _data -> first)) {
							tp = tp -> _l;
						}
						else break;
					}
					_self = rec;
				}
			}
		}
	};
	class const_iterator {
		friend class map;
	public:
		const_iterator(Tree *A = nullptr, Tree *B = nullptr) : _root(A), _self(B) {}
		const_iterator(const const_iterator &other) {
			_root = other._root;
			_self = other._self;
		}
		const_iterator(const iterator &other) {
			_root = other._root;
			_self = other._self;
		}

		const_iterator operator++(int) {
			const_iterator tmp(*this);
			++ (*this);
			return tmp;
		}
		// ++ iter
		const_iterator & operator++() {
			if (_self == nullptr) throw invalid_iterator();
			do find_upper();	while (_self && !(_self -> _exist));
			return *this;
		}
		const_iterator operator--(int) {
			const_iterator tmp(*this);
			-- (*this);
			return tmp;
		}
		const_iterator & operator--() {
			do find_lower();	while (_self && !(_self -> _exist));
			if (_self == nullptr) throw	invalid_iterator();
			return *this;
		}

		// 返回一个 const引用 : 指向const 对象的引用, 只可读不可改
		const value_type & operator*() const {
			return *(_self -> _data);
		}
		bool operator==(const iterator &rhs) const {
			return (_root == rhs._root && _self == rhs._self);
		}
		bool operator==(const const_iterator &rhs) const {
			return (_root == rhs._root && _self == rhs._self);
		}
		bool operator!=(const iterator &rhs) const {
			return (_root != rhs._root || _self != rhs._self);
		}
		bool operator!=(const const_iterator &rhs) const {
			return (_root != rhs._root || _self != rhs._self);
		}
		// 可以通过这两种方式来修改 元素的值
		const value_type* operator->() const noexcept {
			return _self -> _data;
		}

	private:
		Tree *_root;
		Tree *_self;
		// 妙 O(∩_∩)O
		void find_upper() {
			Compare cmp;
			Tree *tp = _root;
			Tree *rec = nullptr;
			if (_self && _self -> _r) {
				_self = _self -> _r;
				while (_self -> _l) _self = _self -> _l;
			}
			else {
				while (tp) {
					if (cmp(tp -> _data -> first, _self -> _data -> first)) {
						tp = tp -> _r;
					}
					else if (cmp(_self -> _data -> first, tp -> _data -> first)) {
						rec = tp;
						tp = tp -> _l;
					}
					else break;
				}
				_self = rec;
			}
		}

		void find_lower() {
			// 可能当前 iterator == end() , 所以需特判
			Compare cmp;
			if (_self == nullptr) {
				_self = _root;
				while (_self && _self -> _r) _self = _self -> _r;
			}
			else {
				if (_self -> _l) {
					_self = _self -> _l;
					while (_self -> _r) _self = _self -> _r;
				}
				else {
					// 找路径上第一个小于它的 key
					Tree *tp = _root;
					Tree *rec = nullptr;
					while (tp) {
						if (cmp(tp -> _data -> first, _self -> _data -> first)) {
							rec = tp;
							tp = tp -> _r;
						}
						else if (cmp(_self -> _data -> first, tp -> _data -> first)) {
							tp = tp -> _l;
						}
						else break;
					}
					_self = rec;
				}
			}
		}
	};

public:
	map(): _root(nullptr), _rec_array(nullptr), del_cnt(0) {
		_del_array = new Tree* [total];
	}
	
	map(const map &other): _rec_array(nullptr), del_cnt(0) {
		_del_array = new Tree* [total];
		if (!other._root) _root = nullptr;
		else CopySubTree(_root, other._root);
	}
	// 防止自己复制自己
	map & operator=(const map &other) {
		if (&other == this) return *this;
		DelSubTree(_root);
		CopySubTree(_root, other._root);
		return *this;
	}
	~map() {
		DelSubTree(_root);
		Flush();
		delete [] _del_array;
	}

	T & at(const Key &key) {
		iterator tmp(find(key));
		if (tmp._self == nullptr) throw index_out_of_bound();
		return tmp._self -> _data -> second;
	}
	const T & at(const Key &key) const {
		const_iterator tmp(find(key));
		if (tmp._self == nullptr) throw index_out_of_bound();
		return tmp._self -> _data -> second;
	}

	T & operator[](const Key &key) {
		iterator tmp = find(key);
		if (tmp._self != nullptr) {
			return tmp._self -> _data -> second;
		}
		else {
			tmp = Insert_1(_root, key);
			return tmp._self -> _data -> second;
		}
	}

	const T & operator[](const Key &key) const {
		// 相当于 this -> at() , 当this 指针是const 的时候， 那么自然会调用 const 版本的st();
		return at(key);
	}

	iterator begin() const{
		Tree *tmp = _root;
		if (tmp == nullptr) return iterator(_root, nullptr);
		while (tmp -> _l) tmp = tmp -> _l;
		iterator iter(_root, tmp);
		while (iter._self && !iter._self -> _exist) ++ iter;
		return iter;
	}
	const_iterator cbegin() const {
		Tree *tmp = _root;
		if (tmp == nullptr) return const_iterator(_root, tmp);
		while (tmp -> _l) tmp = tmp -> _l;
		const_iterator iter(_root, tmp);
		while (iter._self && !iter._self -> _exist) ++ iter;
		return iter;
	}
	iterator end() const{
		return iterator(_root, nullptr);
	}
	
	const_iterator cend() const {
		return iterator(_root, nullptr);
	}
	bool empty() const {
		if (_root == nullptr ||  _root -> _fact == 0) return true;
		return false;
	}
	size_t size() const {
		if (!_root) return 0;
		return _root -> _fact;
	}
	void clear() {
		DelSubTree(_root);
	}

	pair<iterator, bool> insert(const value_type &value) {
		iterator tmp = find(value.first);
		if (tmp._self != nullptr) {
			return pair<iterator, bool>(tmp, false);
		}
		tmp = Insert_1(_root, value.first);
		tmp._self -> _data -> second = value.second;
		return pair<iterator, bool>(tmp, true);
	}

	void erase(iterator pos) {
		if (pos._root != _root || pos._self == nullptr) throw invalid_iterator();
		if (Find(pos._self -> _data -> first).second == nullptr) throw invalid_iterator();
		erase(_root, pos._self -> _data -> first);
	}

	size_t count(const Key &key) const {
		if (Find(key).second != nullptr) return 1;
		return 0;
	}
	
	iterator find(const Key &key) {
		pair<Tree*, Tree*> tmp = Find(key);
		return iterator(tmp.first, tmp.second);
	}

	const_iterator find(const Key &key) const {
		pair<Tree*, Tree*> tmp = Find(key);
		return const_iterator(tmp.first, tmp.second);
	}

private:
	bool IsBalance(Tree *tr) const{
		// 当前节点的左子树或右子树的size  > 当前结点的大小 乘 alpha 
		// 或以当前结点为根的子树中被删掉的点 > 该子树大小的 beta
		if (tr -> _l != nullptr && tr -> _l -> _size > tr -> _size * alpha) return false;
		if (tr -> _r != nullptr && tr -> _r -> _size > tr -> _size * alpha) return false;
		if (tr -> _size - tr -> _fact > tr -> _size * beta) return false;
		return true;
	}

	void PushUp(Tree *tmp, Tree *ed) {
		if (!tmp || tmp == ed) return ;
		if (cmp(tmp -> _data -> first, ed -> _data -> first)) PushUp(tmp -> _r, ed);
		else if (cmp(ed -> _data -> first, tmp -> _data -> first)) PushUp(tmp -> _l, ed);
		
		tmp -> _size = 1;
		if (tmp -> _l)	tmp -> _size += tmp -> _l -> _size;
		if (tmp -> _r) tmp -> _size += tmp -> _r -> _size;
	}

	void pushup(Tree *tmp, Tree *ed, int size) {
		if (!tmp || tmp == ed) return ;
		if (cmp(tmp -> _data -> first, ed -> _data -> first)) pushup(tmp -> _r, ed, size);
		else if (cmp(ed -> _data -> first, tmp -> _data -> first)) pushup(tmp -> _l, ed, size);
		tmp -> _size -= size;
	}

	// 让以tmp为根的树等于以other为根的数
	void CopySubTree(Tree *&tmp, Tree *other) {
		if (other) {
			tmp = new Tree(*other);
			CopySubTree(tmp -> _l, other -> _l);
			CopySubTree(tmp -> _r, other -> _r);
		}
		else tmp = nullptr;
	}

	void DelSubTree(Tree *&tmp) {
		if (tmp == nullptr) return ;
		if (tmp -> _l) DelSubTree(tmp -> _l);
		if (tmp -> _r) DelSubTree(tmp -> _r);
		
		if (del_cnt >= total - 10)	Flush();
		_del_array[del_cnt ++] = tmp;
		tmp = nullptr;
	}

	void Flush() {
		for (int i = 0; i < del_cnt; i ++) {
			delete _del_array[i];
		}
		del_cnt = 0;
	}

	// 从根结点开始检查是否平衡
	void Check(Tree *&tmp, Tree *ed) {
		if (tmp == ed)	return ;
		if (!IsBalance(tmp)) {
			ReBuild(tmp);
			if (tmp) PushUp(_root, tmp);
			return ;
		}
		if (cmp(tmp -> _data -> first, ed -> _data -> first)) {
			return Check(tmp -> _r, ed);
		}
		else if (cmp(ed -> _data -> first, tmp -> _data -> first)) {
			return Check(tmp -> _l, ed);
		}
	}

	void ReBuild(Tree *&tmp) {
		if (tmp == nullptr) return ;
		if (tmp -> _fact == 0) {
			pushup(_root, tmp, tmp -> _size);
			DelSubTree(tmp);
			return ;
		}
		rec_cnt = 0;
		_rec_array = new Tree* [tmp -> _fact + 1];
		InOrder(tmp);
		tmp = Lift(0, rec_cnt - 1);
		PushUp(_root, tmp);
		delete [] _rec_array;
		_rec_array = nullptr;
	}

	Tree* Lift(int l, int r) const{
		if (l > r) return nullptr;
		if (l == r) return _rec_array[l];
		int mid = l + r >> 1;
		Tree *tmp = _rec_array[mid];
		tmp -> _l = Lift(l, mid - 1);
		tmp -> _r = Lift(mid + 1, r);
		tmp -> _size = tmp -> _fact = r - l + 1;
		return tmp;
	}

	// 中序遍历完之后的结点就是一个一个分立的结点
	// 这一步注意要释放内存！！
	void InOrder(Tree *&tmp) {
		if (tmp -> _l) InOrder(tmp -> _l);
		if (tmp -> _exist) _rec_array[rec_cnt ++] = tmp;
		if (tmp -> _r) InOrder(tmp -> _r);
		if (tmp -> _exist) tmp -> ReConstruct();
		else {
			if (del_cnt >= total - 10)	Flush();
			_del_array[del_cnt ++] = tmp;
		}
		tmp = nullptr;
	}

	pair<Tree*, Tree*> Find(const Key &key) const{
		if (_root == nullptr) return pair<Tree*, Tree*>(_root, nullptr);
		Tree *tmp = _root;
		while(tmp) {
			if (cmp(tmp -> _data -> first, key))	tmp = tmp -> _r;
			else if (cmp(key, tmp -> _data -> first))	 tmp = tmp -> _l;
			else {
				if (tmp -> _exist) return pair<Tree*, Tree*>(_root, tmp);
				else return pair<Tree*, Tree*>(_root, nullptr);
			}
		}
		return pair<Tree*, Tree*>(_root, nullptr);
	}

	void erase(Tree *tmp, const Key &key) {
		tmp -> _fact --;
		if (!cmp(tmp -> _data -> first, key) && !cmp(key, tmp -> _data -> first)) {
			if (tmp -> _exist) {
				tmp -> _exist = false;
				Check(_root, tmp);
			}
			return ;
		}
		else if (cmp(tmp -> _data -> first, key)) erase(tmp -> _r, key);
		else erase(tmp -> _l, key);
	}

	// 递归的同时记录最上面一个不平衡的点, 妙， 
	Tree** Insert_2(Tree *&tmp, const Key &key) {
		if (tmp == nullptr) {
			T value;
			tmp = new Tree();
			tmp -> _data = new value_type(key, value);
			return nullptr;
		}
		Tree **p = nullptr;
		if (cmp(tmp -> _data -> first, key)) p = Insert_2(tmp -> _r, key);
		else p = Insert_2(tmp -> _l, key);
		tmp -> _size ++;
		tmp -> _fact ++;
		if (!IsBalance(tmp)) p = &tmp;
		return p;
	}

	iterator Insert_1(Tree *&tmp, const Key &key) {
		Tree **sec_ptr = Insert_2(tmp, key);
		if (sec_ptr) ReBuild(*sec_ptr);
		return find(key);
	}
};

}

#endif
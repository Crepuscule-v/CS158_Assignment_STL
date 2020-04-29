#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include <cstddef>
#include <iostream>

namespace sjtu {

template<class T>
class deque {
    friend class iterator;
    friend class const_iterator;
    friend class list;
public:
    class list {
        friend class deque;
    public:
        list (int _size): _init_size(_size), _elem_size(0), _prev(nullptr), _next(nullptr) {
            _data = new T*[_init_size];
        }
        list (const list &other): _init_size(other._init_size), _elem_size(other._elem_size), _prev(nullptr), _next(nullptr) {
            _data = new T*[_init_size];
            for (int i = 0; i < other._elem_size; i ++) {
                _data[i] = new T(*(other._data[i]));
            }
        }
        ~list () {
            for (int i = 0; i < _elem_size; i ++)  delete _data[i];
            delete [] _data;
        }
    private:
        T **_data;
        int _init_size;
        int _elem_size;
        list *_prev, *_next;
    };

    class const_iterator;
    class iterator {
        friend class deque;
        friend class const_iterator;
        friend class list;
    private:
        deque<T> *dq;       // can modify the value of the element through the iterator 
        list *l;
        int _idx;        
    public:
        // constructors 
        iterator(): dq(nullptr), _idx(0), l(nullptr) {}
        iterator(deque<T> *deq, list *tp, int idx): dq(deq), _idx(idx), l(tp) {}
        iterator(const iterator &other): dq(other.dq), l(other.l), _idx(other._idx) {}

        iterator operator+(const int &n) const {
            if (n < 0) return operator- (-n);
            if (_idx + n < l -> _elem_size) return iterator(dq, l, _idx + n);
            else {
                list *tp = l;
                if (tp == dq -> _tail) {
                    if (_idx + n == l -> _elem_size) return dq -> end();
                    else throw index_out_of_bound(); 
                }
                int pre = tp -> _elem_size - _idx;
                int num = n - pre;      
                tp = tp -> _next;
                while (num - tp -> _elem_size >= 0 && tp != dq -> _tail) {
                    num -= tp -> _elem_size;
                    tp = tp -> _next;
                }
                if (tp == dq -> _tail) {
                    if (num == 0) return dq -> end();
                    else throw index_out_of_bound();
                }
                else {
                    return iterator(dq, tp, num);
                }
            }
        }

        iterator operator-(const int &n) const {
            if (n < 0) return operator+ (-n);
            if (n <= _idx) return iterator(dq, l, _idx - n);
            else {
                if (l == dq -> _head) throw index_out_of_bound();
                int num = n - _idx - 1;
                list *tp = l -> _prev;
                while (num >= tp -> _elem_size && tp != dq -> _head) {
                    num-= tp -> _elem_size;
                    tp = tp -> _prev;
                } 
                if (tp == dq -> _head) 
                {
                    if (num < tp -> _elem_size) return iterator(dq, tp, tp -> _elem_size - 1 - num);
                    else throw index_out_of_bound();
                }
                return iterator(dq, tp, tp -> _elem_size - 1 - num);
            }
        }
        int operator-(const iterator &rhs) const {
            // calculate the relative distance
            if (dq != rhs.dq) throw invalid_iterator();
			if (l == rhs.l) return _idx - rhs._idx;
            int ans = l -> _elem_size - _idx;
            list *tp = l -> _next;
            while (tp != nullptr && tp != rhs.l) {
                ans += tp -> _elem_size;
                tp = tp -> _next;
            }
            if (tp != nullptr) return -(ans + rhs._idx);
            
            ans = rhs.l -> _elem_size - rhs._idx;
            tp = rhs.l -> _next;
            while (tp != nullptr && tp != l) {
                ans += tp -> _elem_size;
                tp = tp -> _next;
            }
            if (tp != nullptr) return (ans + _idx);
            throw invalid_iterator();

            // if (dq != rhs.dq) throw invalid_iterator();
            // if (l == rhs.l) return _idx - rhs._idx;
            // // calculate their distance to the beginning seperately
            // int dist1 = _idx, dist2 = _idx;
            // list *h = dq -> _head;
            // // list *hh = dq -> _head;
            // while (h != l) {
            //     dist1 += h -> _elem_size;
            //     h = h -> _next;
            // }
            // h = dq -> _head;
            // while (h != rhs.l) {
            //     dist2 += h -> _elem_size;
            //     h = h -> _next;
            // }
            // return dist1 - dist2;
        }
        iterator& operator+=(const int &n) {
            *this = *this + n;
            return *this;
        }
        iterator& operator-=(const int &n) {
            *this = *this - n;
            return *this;
        }
        iterator operator++(int) {
            iterator tp(*this);
            ++ *this;
            return tp;
        }
        iterator& operator++() {
            if (_idx == l -> _elem_size - 1) {
                if (l -> _next == nullptr)  throw index_out_of_bound();
                l = l -> _next;
                _idx = 0;
            }
            else _idx ++;
            return *this;
        }
        iterator operator--(int) {
            iterator tp(*this);
            -- *this;
            return tp;
        }
        iterator& operator--() {
            if (_idx == 0) {
                if (l -> _prev == nullptr) throw index_out_of_bound();
                l = l -> _prev;
                _idx = l -> _elem_size - 1;
            }
            else _idx --;
            return *this;
        }
        T& operator*() const {
            if (_idx < 0 || _idx >= l -> _elem_size) throw invalid_iterator();
            return *(l -> _data[_idx]); 
        }
        T* operator->() const noexcept { 
            return l -> _data[_idx]; 
        }

        bool operator==(const iterator &rhs) const {
            if (dq == rhs.dq && *this - rhs == 0) return true;
            return false;
        }
        bool operator==(const const_iterator &rhs) const {
            if (dq == rhs.dq && *this - rhs == 0) return true;
            else return false;
        }
        bool operator!=(const iterator &rhs) const {
            if (*this - rhs != 0 || dq != rhs.dq) return true;
            return false;
        }
        bool operator!=(const const_iterator &rhs) const {
            if (*this - rhs != 0 || dq != rhs.dq) return true;
            return false;
        }
    };
    class const_iterator {
    private:
        const deque<T> *dq;       // can't modify the value of the element through the iterator 
        list *l;
        int _idx;                
        
    public:
        const_iterator(): dq(nullptr), _idx(0), l(nullptr) {}
        const_iterator(const deque<T> *deq, list *tp, int idx): dq(deq), _idx(idx), l(tp) {}
        const_iterator(const const_iterator &other) : dq(other.dq), l(other.l), _idx(other._idx) {}
        const_iterator(const iterator &other) : dq(other.dq), l(other.l), _idx(other._idx) {}
        
        const_iterator operator+(const int &n) const {
            if (n < 0) return operator- (-n);
            if (_idx + n < l -> _elem_size) return const_iterator(dq, l, _idx + n);
            else {
                list *tp = l;
                if (tp == dq -> _tail) {
                    if (_idx + n == l -> _elem_size) return dq -> cend();
                    else throw index_out_of_bound(); 
                }
                int pre = tp -> _elem_size - _idx;
                int num = n - pre;       // remaining steps
                tp = tp -> _next;
                while (num - tp -> _elem_size >= 0 && tp != dq -> _tail) {
                    num -= tp -> _elem_size;
                    tp = tp -> _next;
                }
                if (tp == dq -> _tail) {
                    if (num == 0) return dq -> cend();
                    else throw index_out_of_bound();
                }
                else {
                    return const_iterator(dq, tp, num);
                }
            }
        }

        const_iterator operator-(const int &n) const {
            if (n < 0) return operator+ (-n);
            if (n <= _idx) return const_iterator(dq, l, _idx - n);
            else {
                if (l == dq -> _head) throw index_out_of_bound();
                int num = n - _idx - 1;
                list *tp = l -> _prev;
                while (num >= tp -> _elem_size && tp != dq -> _head) {
                    num-= tp -> _elem_size;
                    tp = tp -> _prev;
                } 
                if (tp == dq -> _head) 
                {
                    if (num < tp -> _elem_size) return const_iterator(dq, tp, tp -> _elem_size - 1 - num);
                    else throw index_out_of_bound();
                }
                return const_iterator(dq, tp, tp -> _elem_size - 1 - num);
            }
        }
        int operator-(const const_iterator &rhs) const {
            // calculate the relative distance
            if (dq != rhs.dq) throw invalid_iterator();
			if (l == rhs.l) return _idx - rhs._idx;
            int ans = l -> _elem_size - _idx;
            list *tp = l -> _next;
            while (tp != nullptr && tp != rhs.l) {
                ans += tp -> _elem_size;
                tp = tp -> _next;
            }
            if (tp != nullptr) return -(ans + rhs._idx);
            
            ans = rhs.l -> _elem_size - rhs._idx;
            tp = rhs.l -> _next;
            while (tp != nullptr && tp != l) {
                ans += tp -> _elem_size;
                tp = tp -> _next;
            }
            if (tp != nullptr) return (ans + _idx);
            throw invalid_iterator();
        }
        const_iterator& operator+=(const int &n) {
            *this = *this + n;
            return *this;
        }
        const_iterator& operator-=(const int &n) {
            *this = *this - n;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tp(*this);
            ++ *this;
            return tp;
        }
        const_iterator& operator++() {
            if (_idx == l -> _elem_size - 1) {
                if (l -> _next == nullptr)  throw index_out_of_bound();
                l = l -> _next;
                _idx = 0;
            }
            else _idx ++;
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator tp(*this);
            -- *this;
            return tp;
        }
        const_iterator& operator--() {
            if (_idx == 0) {
                if (l -> _prev == nullptr) throw index_out_of_bound();
                l = l -> _prev;
                _idx = l -> _elem_size - 1;
            }
            else _idx --;
            return *this;
        }
        T& operator*() const { 
            if (_idx < 0 || _idx >= l -> _elem_size) throw invalid_iterator();
            return *(l -> _data[_idx]); 
        }
        T* operator->() const noexcept { 
            return l -> _data[_idx];
        }

        bool operator==(const iterator &rhs) const {
            if (dq == rhs.dq && *this - rhs == 0) return true;
            return false;
        }
        bool operator==(const const_iterator &rhs) const {
            if (dq == rhs.dq && *this - rhs == 0) return true;
            else return false;
        }
        bool operator!=(const iterator &rhs) const {
            if (*this - rhs != 0 || dq != rhs.dq) return true;
            return false;
        }
        bool operator!=(const const_iterator &rhs) const {
            if (*this - rhs != 0 || dq != rhs.dq) return true;
            return false;
        }
    };

    deque(): _head(nullptr), _tail(nullptr), _current_size(0), _each_list_size(1000) {
        _head = new list(1000);
        _tail = new list(1);
        _head -> _next = _tail;
        _head -> _prev = nullptr;
        _tail -> _prev = _head;
        _tail -> _next = nullptr;
    }
    deque(const deque &other): _head(nullptr), _tail(nullptr), _current_size(other._current_size), _each_list_size(other._each_list_size) {
        _head = new list(*other._head);
        _tail = new list(1);
        list *l = _head;
        list *tp = nullptr;
        for (list *it = other._head -> _next; it != other._tail; it = it -> _next) {
            tp = new list(*it);
            l -> _next = tp;
            tp -> _prev = l;
            l = tp;
        }
        if (tp == nullptr) {
            _head -> _next = _tail;
            _tail -> _prev = _head;
        }
        else {
            tp -> _next = _tail;
            _tail -> _prev = tp;
        }
        _tail -> _next = nullptr;
        _head -> _prev = nullptr;
    }

    ~deque() {
        clear();
        delete _head;
        delete _tail;
    }
    deque &operator=(const deque &other) {
        if (this == &other) return *this;
        clear();
        delete _head;
        _current_size = other._current_size;
        _each_list_size = other._each_list_size;
        _head = new list(*other._head);
        _head -> _prev = nullptr;
        list *l = _head;
        list *tp = nullptr;
        for (list *it = other._head -> _next; it != other._tail; it = it -> _next) {
            tp = new list(*it);
            l -> _next = tp;
            tp -> _prev = l;
            l = tp;
        }
        if (tp == nullptr) {
            _head -> _next = _tail;
            _tail -> _prev = _head;
        }
        else {
            tp -> _next = _tail;
            _tail -> _prev = tp;
        }
        return *this;
    }

    T & at(const size_t &pos) {
        if (pos < 0 || pos >= _current_size)   throw index_out_of_bound();
        else {
            int _pos = pos;
            list *tp = _head;
            while (_pos - tp -> _elem_size >= 0) {
                _pos -= tp -> _elem_size;
                tp = tp -> _next;
            }
            return *(tp -> _data[_pos]);
        }
    }
    const T & at(const size_t &pos) const {
        if (pos < 0 || pos >= _current_size)   throw index_out_of_bound();
        else {
            int _pos = pos;
            list *tp = _head;
            while (_pos - tp -> _elem_size >= 0) {
                _pos -= tp -> _elem_size;
                tp = tp -> _next;
            }
            return *(tp -> _data[_pos]);
        }
    }
    T & operator[](const size_t &pos) {
        return at(pos);
    }

    const T & operator[](const size_t &pos) const {
        return at(pos);
    }

    const T & front() const {
        if (!_current_size) throw container_is_empty();
        else return (*this)[0];
    }
    const T & back() const {
        if (!_current_size) throw container_is_empty();
        else return (*this)[_current_size - 1];
    }
    iterator begin() {
        if (_current_size == 0) return end();
        return iterator(this, _head, 0);
    }
    const_iterator cbegin() const {
        if (_current_size == 0) return cend();
        return const_iterator(this, _head, 0);
    }

    iterator end() { return iterator(this, _tail, 0); }
    const_iterator cend() const { return const_iterator(this, _tail, 0); }
    bool empty() const { return !_current_size; }
    size_t size() const { return _current_size; }

    void clear() {
        list *p = _head;
        while (p != _tail) {
            list *tp = p;
            p = p -> _next;
            delete tp;
        }
        _head = new list(1000);
        _head -> _prev = nullptr;
        _head -> _next = _tail;
        _tail -> _prev = _head;
        _current_size = 0;
    }

    iterator insert(iterator pos, const T &value) { 
        if (pos.dq != this) throw invalid_iterator();
        _current_size ++;
        int idx = pos._idx;
        list *l = pos.l;
        if (idx == 0 && l -> _prev != nullptr) {
            l = l -> _prev;
            idx = l -> _elem_size;
        }
        merge(l);
        split(l);

        if (idx > l -> _elem_size && l != _tail) {
            idx -= l -> _elem_size;
            l = l -> _next;
        }
        if (l == _tail) throw index_out_of_bound();
        for (int i = l -> _elem_size; i > idx; i --) l -> _data[i] = l -> _data[i - 1];
        l -> _data[idx] = new T(value);
        l -> _elem_size ++ ;
        return iterator(this, l, idx);
    }

    iterator erase(iterator pos) {
        if (_current_size == 0 || pos.dq != this || pos.l == _tail) throw invalid_iterator();
        _current_size --;
        int idx = pos._idx;
        list *l = pos.l;
        
        merge(l);
        split(l);

        if (idx >= l -> _elem_size && l != _tail) {
            idx -= l -> _elem_size;
            l = l -> _next;
        }
        if (l == _tail) throw index_out_of_bound();
        delete l -> _data[idx];
        for (int i = idx; i < l -> _elem_size - 1; i ++) l -> _data[i] = l -> _data[i + 1];
        l -> _elem_size --;
        if (l -> _elem_size == 0 && l != _head) {
            list *tp = l;
            l = l -> _next;
            l -> _prev = tp -> _prev;
            tp -> _prev -> _next = l;
            idx = 0;
            delete tp;
        }
        if (idx == l -> _elem_size) {
            idx = 0;
            l = l -> _next;
        }
        return iterator(this, l, idx);
    }

    void push_back(const T &value) { insert(iterator(this, _tail -> _prev, _tail -> _prev -> _elem_size), value); }
    void pop_back() {
        if (_current_size == 0) throw index_out_of_bound();
        erase(iterator(this, _tail -> _prev, _tail -> _prev -> _elem_size - 1));
    }
    void push_front(const T &value) { insert(iterator(this, _head, 0), value); }
    void pop_front() {
        if (_current_size == 0) throw index_out_of_bound();
        erase(begin());
    }

private:
    list *_head, *_tail;
    int _current_size;          //  total number of elements
    int _each_list_size;        //  maxinum number of the elements in each list 

    // only try to merge the next list, so the head and tail pointer will not change
    void merge(list *&l) {   
        if (l == _tail) return ;
        if (l -> _elem_size < _each_list_size / 2) {
            while (l -> _elem_size + l -> _next -> _elem_size <= _each_list_size) {
                if (l -> _next == _tail) return ;
                list *tp = l -> _next;
                for (int i = l -> _elem_size, j = 0; j < tp -> _elem_size; j ++, i ++) {
                    l -> _data[i] = tp -> _data[j];
                }
                l -> _elem_size += tp -> _elem_size;
                l -> _next = tp -> _next;
                tp -> _next -> _prev = l;
                tp -> _elem_size = 0;
                delete tp;
            }
        }
    }

    void split(list *&l) {
        if (l == _tail) return ;
        if (l -> _elem_size >= _each_list_size) {
            int k = l -> _elem_size / 2;
            list *tp = new list(_each_list_size);
            for (int i = k, j = 0; i < l -> _elem_size; i ++, j ++) {
                tp -> _data[j] = l -> _data[i];
            }
            tp -> _next = l -> _next;
            tp -> _prev = l;
            l -> _next -> _prev = tp;
            l -> _next = tp;
            tp -> _elem_size = l -> _elem_size - k;
            l -> _elem_size = k;
        }
    }
};

}

#endif
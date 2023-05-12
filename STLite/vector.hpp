#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
    template<typename T>
    class vector {
    public:
        /**
         * a type for actions of the elements of a vector, and you should write
         *   a class named const_iterator with same interfaces.
         */
        /**
         * you can see RandomAccessIterator at CppReference for help.
         */
        class const_iterator;

        class iterator {
            // The following code is written for the C++ type_traits library.
            // Type traits is a C++ feature for describing certain properties of a type.
            // For instance, for an iterator, iterator::value_type is the type that the
            // iterator points to.
            // STL algorithms and containers may use these type_traits (e.g. the following
            // typedef) to work properly. In particular, without the following code,
            // @code{std::sort(iter, iter1);} would not compile.
            // See these websites for more information:
            // https://en.cppreference.com/w/cpp/header/type_traits
            // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
            // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_category = std::output_iterator_tag;

        private:

            pointer p = nullptr; //标记当前位置
            vector *v = nullptr; //标记所属vector

        public:
            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            iterator() = default;

            iterator(const iterator &a) : p(a.p), v(a.v) {}

            iterator(pointer p, vector *v) : p(p), v(v) {}

            iterator operator+(const int &n) const {
                return iterator(p + n, v);
            }

            iterator operator-(const int &n) const {
                return iterator(p - n, v);
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invalid_iterator.
            int operator-(const iterator &rhs) const {
                if (v != rhs.v) throw invalid_iterator();
                return p - rhs.p;
            }

            iterator &operator+=(const int &n) {
                p += n;
                return *this;
            }

            iterator &operator-=(const int &n) {
                p -= n;
                return *this;
            }

            iterator operator++(int) {
                return iterator(p++, v);
            }

            iterator &operator++() {
                ++p;
                return *this;
            }

            iterator operator--(int) {
                return iterator(p--, v);
            }

            iterator &operator--() {
                --p;
                return *this;
            }

            T &operator*() const {
                return *p;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                return p == rhs.p;
            }

            bool operator==(const const_iterator &rhs) const {
                return p == rhs.p;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return p != rhs.p;
            }

            bool operator!=(const const_iterator &rhs) const {
                return p != rhs.p;
            }

            friend class const_iterator;
        };

        /**
         * has same function as iterator, just for a const object.
         */
        class const_iterator {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_category = std::output_iterator_tag;

        private:
            const T *p = nullptr; //Mysterious bug: why can't I use pointer here?
            const vector *v = nullptr;

        public:
            const_iterator() = default;

            const_iterator(const const_iterator &a) : p(a.p), v(a.v) {}

            const_iterator(pointer p, const vector *v) : p(p), v(v) {}

            const_iterator operator+(const int &n) const {
                return const_iterator(p + n, v);
            }

            const_iterator operator-(const int &n) const {
                return const_iterator(p - n, v);
            }

            int operator-(const const_iterator &rhs) const {
                if (v != rhs.v) throw invalid_iterator();
                return p - rhs.p;
            }

            const_iterator &operator+=(const int &n) {
                p += n;
                return *this;
            }

            const_iterator &operator-=(const int &n) {
                p -= n;
                return *this;
            }

            const_iterator operator++(int) {
                return const_iterator(p++, v);
            }

            const_iterator &operator++() {
                ++p;
                return *this;
            }

            const_iterator operator--(int) {
                return const_iterator(p--, v);
            }

            const_iterator &operator--() {
                --p;
                return *this;
            }

            const T &operator*() const {
                return *p;
            }

            bool operator==(const iterator &rhs) const {
                return p == rhs.p;
            }

            bool operator==(const const_iterator &rhs) const {
                return p == rhs.p;
            }

            bool operator!=(const iterator &rhs) const {
                return p != rhs.p;
            }

            bool operator!=(const const_iterator &rhs) const {
                return p != rhs.p;
            }

            friend class iteraror;
        };

        /**
         * Constructs
         * At least two: default constructor, copy constructor
         */
        vector() : size_(0), capacity(10) {
            data = (T *) malloc(capacity * sizeof(T));
        }

        vector(const vector &other) : size_(other.size_), capacity(other.capacity) {
            data = (T *) malloc(capacity * sizeof(T));
            for (int i = 0; i < size_; ++i) new(data + i) T(*(other.data + i));
        }

        /**
         * Destructor
         */
        ~vector() {
            for (int i = 0; i < size_; ++i) (data + i)->~T();
            free(data);
        }

        /**
         * Assignment operator
         */
        vector &operator=(const vector &other) {
            //Bug: Never forget to judge the assignment to itself!
            if (*this == other) return *this;
            for (int i = 0; i < size_; ++i) (data + i)->~T();
            free(data);
            size_ = other.size_;
            capacity = other.capacity;
            data = (T *) malloc(capacity * sizeof(T));
            for (int i = 0; i < size_; ++i) new(data + i) T(*(other.data + i));
            return *this;
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         */
        T &at(const size_t &pos) {
            if (pos < 0 || pos >= size_) throw index_out_of_bound();
            return data[pos];
        }

        const T &at(const size_t &pos) const {
            if (pos < 0 || pos >= size_) throw index_out_of_bound();
            return data[pos];
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         * !!! Pay attentions
         *   In STL this operator does not check the boundary but I want you to do.
         */
        T &operator[](const size_t &pos) {
            if (pos < 0 || pos >= size_) throw index_out_of_bound();
            return data[pos];
        }

        const T &operator[](const size_t &pos) const {
            if (pos < 0 || pos >= size_) throw index_out_of_bound();
            return data[pos];
        }

        /**
         * access the first element.
         * throw container_is_empty if size == 0
         */
        const T &front() const {
            if (size_) return *data;
            else throw container_is_empty();
        }

        /**
         * access the last element.
         * throw container_is_empty if size == 0
         */
        const T &back() const {
            if (size_) return data[size_ - 1];
            else throw container_is_empty();
        }

        /**
         * returns an iterator to the beginning.
         */
        iterator begin() {
            return iterator(data, this);
        }

        const_iterator cbegin() const {
            return const_iterator(data, this);
        }

        /**
         * returns an iterator to the end.
         */
        iterator end() {
            return iterator(data + size_, this);
        }

        const_iterator cend() const {
            return const_iterator(data + size_, this);
        }

        /**
         * checks whether the container is empty
         */
        bool empty() const { return size_ == 0; }

        /**
         * returns the number of elements
         */
        size_t size() const { return size_; }

        /**
         * clears the contents
         */
        void clear() {
            for (int i = 0; i < size_; ++i)
                (data + i)->~T();
            size_ = 0;
            if (capacity >= 20) {
                free(data);
                capacity = 10;
                data = (T *) malloc(capacity * sizeof(T));
            }
        }

        /**
         * inserts value before pos
         * returns an iterator pointing to the inserted value.
         */
        iterator insert(iterator pos, const T &value) {
            int i = pos - begin(); //*pos = data[i] --> value
            if (size_ < capacity - 1) { //normal
                for (int j = size_; j > i; --j)
                    new(data + j) T(data[j - 1]);
                new(data + i) T(value);
                ++size_;
                return pos;
            } else {
                doubleSize();
                for (int j = size_; j > i; --j)
                    new(data + j) T(data[j - 1]);
                new(data + i) T(value);
                ++size_;
                return iterator(data + i, this);
            }
        }

        /**
         * inserts value at index ind.
         * after inserting, this->at(ind) == value
         * returns an iterator pointing to the inserted value.
         * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
         */
        iterator insert(const size_t &ind, const T &value) {
            int i = ind;
            if (i < 0 || i > size_) throw index_out_of_bound();
            if (size_ < capacity - 1) { //normal
                for (int j = size_; j > i; --j)
                    new(data + j) T(data[j - 1]);
                new(data + i) T(value);
                ++size_;
                return iterator(data + i, this);
            } else {
                doubleSize();
                for (int j = size_; j > i; --j)
                    new(data + j) T(data[j - 1]);
                new(data + i) T(value);
                ++size_;
                return iterator(data + i, this);
            }
        }

        /**
         * removes the element at pos.
         * return an iterator pointing to the following element.
         * If the iterator pos refers the last element, the end() iterator is returned.
         */
        iterator erase(iterator pos) {
            int i = pos - begin(); //*pos = data[i]
            for (int j = i; j < size_ - 1; ++j) data[j] = data[j + 1];
            (data + size_ - 1)->~T();
            --size_;
            if (size_ < capacity / 3 && capacity >= 20) {
                T *pre = data;
                capacity >>= 1;
                data = (T *) malloc(capacity * sizeof(T));
                for (int j = 0; j < size_; ++j) {
                    new(data + j) T(*(pre + j));
                    (pre + j)->~T();
                }
                free(pre);
            }
            return iterator(data + i, this);
        }

        /**
         * removes the element with index ind.
         * return an iterator pointing to the following element.
         * throw index_out_of_bound if ind >= size
         */
        iterator erase(const size_t &ind) {
            int i = ind;
            for (int j = i; j < size_ - 1; ++j) data[j] = data[j + 1];
            (data + size_ - 1)->~T();
            --size_;
            if (size_ < capacity / 3 && capacity >= 20) {
                T *pre = data;
                capacity >>= 1;
                data = (T *) malloc(capacity * sizeof(T));
                for (int j = 0; j < size_; ++j) {
                    new(data + j) T(*(pre + j));
                    (pre + j)->~T();
                }
                free(pre);
            }
            return iterator(data + i, this);
        }

        /**
         * adds an element to the end.
         */
        void push_back(const T &value) {
            if (size_ == capacity - 1)
                doubleSize();
            new(data + size_) T(value);
            ++size_;
        }

        /**
         * remove the last element from the end.
         * throw container_is_empty if size() == 0
         */
        void pop_back() {
            if (!size_) throw container_is_empty();
            if (size_ - 1 < capacity / 3 && capacity >= 20) {
                T *pre = data;
                capacity >>= 1;
                data = (T *) malloc(capacity * sizeof(T));
                for (int i = 0; i < size_ - 1; ++i) {
                    new(data + i) T(*(pre + i));
                    (pre + i)->~T();
                }
                pre[size_ - 1].~T();
                free(pre);
                --size_;
            } else {
                data[--size_].~T();
            }
        }

        bool operator==(const vector &v) const {
            return data == v.data && size_ == v.size_ && capacity == v.capacity;
        }

    private:
        T *data = nullptr;
        int size_; //always keep the memory out of size raw
        int capacity; //reserve one for the end. (size_max = capacity - 1)

        void doubleSize() { //size not changed
            T *pre = data;
            capacity <<= 1;
            data = (T *) malloc(capacity * sizeof(T));
            for (int i = 0; i < size_; ++i) {
                new(data + i) T(*(pre + i));
                (pre + i)->~T();
            }
            free(pre);
        }

    };

}

#endif

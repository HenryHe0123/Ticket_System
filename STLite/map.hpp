#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

    template<
            class Key,
            class T,
            class Compare = std::less<Key>
    >
    class map {
    public:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::map as value_type by typedef.
         */
        typedef pair<const Key, T> value_type;

        struct Node {
            value_type *data = nullptr;
            Node *fa = nullptr; //叶子节点的子节点不保存父节点信息
            Node *ls = nullptr;
            Node *rs = nullptr;
            int height = 1;

            Node() = default;

            ~Node() { delete data; }

            Node(const value_type &v, Node *fa, Node *ls = nullptr, Node *rs = nullptr, int height = 1) :
                    fa(fa), ls(ls), rs(rs), height(height) { data = new value_type(v); }

            inline void updateH() { if (this) height = std::max(h(ls), h(rs)) + 1; }
        };

    private: // AVL tree
        Node *root = nullptr;
        size_t size_ = 0;
        Compare cmp{};

        inline static int h(Node *t) { return (t) ? t->height : 0; }

        inline bool equal(const Key &a, const Key &b) const {
            return !(cmp(a, b) || cmp(b, a));
        }

        Node *find(const Key &key, Node *t) const { //return nullptr if no found
            if (t == nullptr || equal(t->data->first, key)) return t;
            if (cmp(key, t->data->first)) return find(key, t->ls);
            else return find(key, t->rs);
        }

        void free(Node *t) {
            if (t) {
                free(t->ls);
                free(t->rs);
                delete t;
            }
        }

        Node *copy(Node *t, Node *fa) {
            if (t == nullptr) return nullptr;
            Node *newt = new Node(*(t->data), fa, nullptr, nullptr, t->height);
            newt->ls = copy(t->ls, newt);
            newt->rs = copy(t->rs, newt);
            return newt;
        }

        inline void LL(Node *&tt) {
            Node *t = tt; //不基于tt直接修改
            bool isRoot = (tt == root);
            Node *t1 = t->ls; //未来的树根
            //先处理t->fa相关
            t1->fa = t->fa;
            if (t->fa) {
                if (t == t->fa->ls) t->fa->ls = t1;
                else t->fa->rs = t1;
            } //Bug2：别忘了把父节点反连回去！
            t->ls = t1->rs;
            if (t->ls) t->ls->fa = t; //Bug1：别忘了更改此处的父节点信息！
            t->updateH();
            t->fa = t1; //t作为t1的节点准备完毕
            t1->rs = t; //指针旋转
            t1->updateH(); //更新高度
            tt = t1;
            if (isRoot) root = t1;
        }

        inline void RR(Node *&tt) {
            Node *t = tt; //不基于tt直接修改
            bool isRoot = (tt == root);
            Node *t1 = t->rs; //未来的树根
            //先处理t->fa相关
            t1->fa = t->fa;
            if (t->fa) {
                if (t == t->fa->ls) t->fa->ls = t1;
                else t->fa->rs = t1;
            } //
            t->rs = t1->ls;
            if (t->rs) t->rs->fa = t;
            t->updateH();
            t->fa = t1;
            t1->ls = t; //指针旋转
            t1->updateH();
            tt = t1;
            if (isRoot) root = t1;
        }

        inline void LR(Node *&t) {
            RR(t->ls);
            LL(t);
        }

        inline void RL(Node *&t) {
            LL(t->rs);
            RR(t);
        }

        Node *insert(const value_type &value, Node *&t, Node *fa) { //返回insert后节点指针，如键值已存在则不变，不修改size_
            Node *p = t; //用p指向插入节点位置
            if (t == nullptr) {
                t = new Node(value, fa); //t为引用故不用更新fa的子节点信息
                p = t;
            } else if (cmp(value.first, t->data->first)) { //在左子树上插入
                p = insert(value, t->ls, t);
                if (h(t->ls) - h(t->rs) == 2) {
                    if (cmp(value.first, t->ls->data->first)) LL(t);
                    else LR(t);
                }
            } else if (cmp(t->data->first, value.first)) { //在右子树上插入
                p = insert(value, t->rs, t);
                if (h(t->rs) - h(t->ls) == 2) {
                    if (cmp(t->rs->data->first, value.first)) RR(t);
                    else RL(t);
                }
            }
            //else if key = t.key 不变但返回t
            t->updateH();
            return p;
        }

        static inline Node *Max(Node *t) {
            if (t) {
                while (t->rs) t = t->rs;
                return t;
            } else return nullptr;
        }

        static inline Node *Min(Node *t) {
            if (t) {
                while (t->ls) t = t->ls;
                return t;
            } else return nullptr;
        }

        static inline Node *next(const Node *t) {
            if (t == nullptr) return nullptr;
            if (t->rs) return Min(t->rs);
            //没有右儿子节点
            while (t->fa) { //回溯寻找
                if (t->fa->ls == t) return t->fa;
                t = t->fa;
            }
            return nullptr; //最右节点
        }

        static inline Node *pre(const Node *t) {
            if (t == nullptr) return nullptr;
            if (t->ls) return Max(t->ls);
            //没有左儿子节点
            while (t->fa) { //回溯寻找
                if (t->fa->rs == t) return t->fa;
                t = t->fa;
            }
            return nullptr; //最左节点
        }

        bool remove(const Key &key, Node *t) { //返回remove后树高是否更改,不修改size_
            bool isRoot = (t == root);
            if (t == nullptr) return true;
            if (equal(t->data->first, key)) { //remove t
                if (t->ls && t->rs) { //t有两个儿子
                    Node *instead = next(t); //由于t有rs，故instead->ls = nullptr
                    //Bug:需要完全交换t与instead的指针关系，否则iterator将出错
                    //不交换数据/数据与指针绑定！
                    //std::swap(t->data, instead->data);
                    std::swap(t->height, instead->height);
                    //交换指针关系
                    if (t->rs == instead) { //instead->fa = t
                        if (t->fa) {
                            if (t->fa->ls == t) t->fa->ls = instead;
                            else t->fa->rs = instead;
                        }
                        instead->fa = t->fa;
                        t->fa = instead;
                        //先处理t和instead的儿子信息
                        t->ls->fa = instead;
                        if (instead->rs) instead->rs->fa = t;
                        //
                        t->rs = instead->rs;
                        instead->rs = t;
                        std::swap(t->ls, instead->ls);

                    } else { //非父子相连
                        //
                        if (t->fa) {
                            if (t->fa->ls == t) t->fa->ls = instead;
                            else t->fa->rs = instead;
                        }
                        //instead在t的右子树的最左侧
                        instead->fa->ls = t;
                        //
                        t->ls->fa = instead;
                        t->rs->fa = instead;
                        //instead->ls->fa = t; ***instead->ls不存在***
                        if (instead->rs) instead->rs->fa = t;
                        //
                        std::swap(t->fa, instead->fa);
                        std::swap(t->ls, instead->ls);
                        std::swap(t->rs, instead->rs);
                    }
                    //此时节点也变了！
                    if (isRoot) root = instead;
                    if (remove(key, instead->rs)) return true; //递归删除节点，若右子树没有变矮直接返回
                    return adjust(instead, true); //调用adjust检查调整树的平衡性
                } else { //t为叶子节点或只有一个儿子
                    Node *son = (t->ls) ? t->ls : t->rs; //single or null
                    if (t->fa) {
                        if (t->fa->ls == t) t->fa->ls = son;
                        else t->fa->rs = son;
                    }
                    if (son) son->fa = t->fa;
                    delete t;
                    if (isRoot) root = son;
                    return false; //t以下height不变，父节点的高度会在递归过程中由adjust检查更新
                }
            } else if (cmp(key, t->data->first)) { //在t的左子树上删除
                if (remove(key, t->ls)) return true;
                return adjust(t, false);
            } else { //在t的右子树上删除
                if (remove(key, t->rs)) return true;
                return adjust(t, true);
            }
        }

        bool adjust(Node *&t, bool onRight) { //检查t节点的平衡性，onRight表示删除是否发生在右子树上
            int diff_h = h(t->ls) - h(t->rs);
            if (onRight) { //右子树变矮
                if (diff_h == 1) return true;
                if (diff_h == 0) {
                    --t->height;
                    return false; //树高改变
                }
                if (h(t->ls->rs) > h(t->ls->ls)) {
                    LR(t);
                    return false;
                }
                LL(t);
                if (h(t->ls) == h(t->rs)) return false;
                else return true;
            } else { //左子树变矮
                if (diff_h == -1) return true;
                if (diff_h == 0) {
                    --t->height;
                    return false; //树高改变
                }
                if (h(t->rs->ls) > h(t->rs->rs)) {
                    RL(t);
                    return false;
                }
                RR(t);
                if (h(t->ls) == h(t->rs)) return false;
                else return true;
            }
        }

    public:
        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = map.begin(); --it;
         *       or it = map.end(); ++end();
         */
        class const_iterator;

        class iterator;

        friend class const_iterator;

        friend class iterator;

        class iterator {
            friend class map;

        public:

        private:
            /**
             *  add data members
             *   just add whatever you want.
             */
            map *id;
            Node *p; // p = nullptr represent end iterator

        public:
            explicit iterator(map *id = nullptr, Node *p = nullptr) : id(id), p(p) {}

            iterator(const iterator &other) : id(other.id), p(other.p) {}

            /**
             * iter++
             */
            iterator operator++(int) {
                iterator pre_it = *this;
                if (p == nullptr) throw invalid_iterator();
                p = next(p);
                return pre_it;
            }

            /**
             * ++iter
             */
            iterator &operator++() {
                if (p == nullptr) throw invalid_iterator();
                p = next(p);
                return *this;
            }

            /**
             * iter--
             */
            iterator operator--(int) {
                iterator pre_it = *this;
                if (p == nullptr) p = Max(id->root);
                else p = pre(p);
                if (p == nullptr) throw invalid_iterator();
                return pre_it;
            }

            /**
             * --iter
             */
            iterator &operator--() {
                if (p == nullptr) p = Max(id->root);
                else p = pre(p);
                if (p == nullptr) throw invalid_iterator();
                return *this;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type &operator*() const {
                if (p == nullptr) throw invalid_iterator();
                return *(p->data);
            }

            bool operator==(const iterator &rhs) const { return p == rhs.p && id == rhs.id; }

            bool operator==(const const_iterator &rhs) const { return p == rhs.p && id == rhs.id; }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const { return p != rhs.p || id != rhs.id; }

            bool operator!=(const const_iterator &rhs) const { return p != rhs.p || id != rhs.id; }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type *operator->() const noexcept {
                if (p == nullptr) throw invalid_iterator();
                return p->data;
            }
        };

        class const_iterator {
            friend class map;

        public:
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        private:
            // data members.
            const map *id;
            const Node *p;

        public:
            const_iterator(const map *id = nullptr, const Node *p = nullptr) : id(id), p(p) {}

            const_iterator(const const_iterator &other) : id(other.id), p(other.p) {}

            explicit const_iterator(const iterator &other) : id(other.id), p(other.p) {}

            const_iterator &operator=(const iterator &other) {
                id = other.id;
                p = other.p;
                return *this;
            }

            const_iterator operator++(int) {
                const_iterator pre_it = *this;
                if (p == nullptr) throw invalid_iterator();
                p = next(p);
                return pre_it;
            }

            const_iterator &operator++() {
                if (p == nullptr) throw invalid_iterator();
                p = next(p);
                return *this;
            }

            const_iterator operator--(int) {
                const_iterator pre_it = *this;
                if (p == nullptr) p = Max(id->root);
                else p = pre(p);
                if (p == nullptr) throw invalid_iterator();
                return pre_it;
            }

            const_iterator &operator--() {
                if (p == nullptr) p = Max(id->root);
                else p = pre(p);
                if (p == nullptr) throw invalid_iterator();
                return *this;
            }

            const value_type &operator*() const {
                if (p == nullptr) throw invalid_iterator();
                return *(p->data);
            }

            bool operator==(const iterator &rhs) const { return p == rhs.p && id == rhs.id; }

            bool operator==(const const_iterator &rhs) const { return p == rhs.p && id == rhs.id; }

            bool operator!=(const iterator &rhs) const { return p != rhs.p || id != rhs.id; }

            bool operator!=(const const_iterator &rhs) const { return p != rhs.p || id != rhs.id; }

            const value_type *operator->() const noexcept {
                if (p == nullptr) throw invalid_iterator();
                return p->data;
            }

        };

        /**
         * two constructors
         */
        map() = default;

        map(const map &other) : root(copy(other.root, nullptr)), size_(other.size_) {}

        /**
         * assignment operator
         */
        map &operator=(const map &other) {
            if (this != &other) {
                free(root);
                root = copy(other.root, nullptr);
                size_ = other.size_;
            }
            return *this;
        }

        /**
         * Destructors
         */
        ~map() {
            free(root);
            root = nullptr;
        }

        /**
         *
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        T &at(const Key &key) {
            Node *p = find(key, root);
            if (p) return p->data->second;
            else throw index_out_of_bound();
        }

        const T &at(const Key &key) const {
            Node *p = find(key, root);
            if (p) return p->data->second;
            else throw index_out_of_bound();
        }

        /**
         *
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T &operator[](const Key &key) {
            Node *p = find(key, root);
            if (p == nullptr) { //key no found
                p = insert(value_type{key, T()}, root, nullptr);
                size_++;
            }
            return p->data->second;
        }

        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T &operator[](const Key &key) const {
            Node *p = find(key, root);
            if (p) return p->data->second;
            else throw index_out_of_bound();
        }

        /**
         * return a iterator to the beginning
         */
        iterator begin() {
            return iterator(this, Min(root));
        }

        const_iterator cbegin() const {
            return const_iterator(this, Min(root));
        }

        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() {
            return iterator(this, nullptr);
        }

        const_iterator cend() const {
            return const_iterator(this, nullptr);
        }

        /**
         * checks whether the container is empty
         * return true if empty, otherwise false.
         */
        bool empty() const { return size_ == 0; }

        /**
         * returns the number of elements.
         */
        size_t size() const { return size_; }

        /**
         * clears the contents
         */
        void clear() {
            free(root);
            root = nullptr;
            size_ = 0;
        }

        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */

        pair<iterator, bool> insert(const value_type &value) {
            Node *p = find(value.first, root);
            if (p == nullptr) {
                p = insert(value, root, nullptr);
                size_++;
                return pair<iterator, bool>(iterator(this, p), true);
            } else {
                return pair<iterator, bool>(iterator(this, p), false); //key already exist
            }
        }

        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos) {
            if (pos.p == nullptr || pos.id != this || find(pos->first, root) == nullptr) throw invalid_iterator();
            remove(pos->first, root);
            size_--;
        }

        bool erase(const Key &key) {
            iterator iter = find(key);
            if (iter == end()) return false;
            erase(iter);
            return true;
        }

        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         * The default method of check the equivalence is !(a < b || b > a)
         */
        size_t count(const Key &key) const {
            return (find(key, root)) ? 1 : 0;
        }

        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key) {
            return iterator(this, find(key, root));
        }

        const_iterator find(const Key &key) const {
            return const_iterator(this, find(key, root));
        }

    };


}

#endif

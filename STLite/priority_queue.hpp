#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

    template<typename T>
    void swap(T &x, T &y) {
        T tmp = x;
        x = y;
        y = tmp;
    }

/**
 * a container like std::priority_queue which is a heap internal.
 */
    template<typename T, class Compare = std::less<T>>
    class priority_queue {
        //---------------------------------------------
    private:
        struct node {
            T val{};
            node *left = nullptr;
            node *right = nullptr;
            int npl = 0;

            size_t size() {
                if (this) return 1 + left->size() + right->size();
                else return 0;
            }

            node() = default;

            explicit node(T v, node *l = nullptr, node *r = nullptr, int npl = 0) : val(v), left(l), right(r),
                                                                                    npl(npl) {}

        } *root = nullptr;

        size_t size_ = 0;
        Compare cmp{};

        void free(node *r) {
            if (r == nullptr) return;
            free(r->left);
            free(r->right);
            delete r;
        }

        node *copy(node *r) {
            if (r == nullptr) return nullptr;
            else return new node(r->val, copy(r->left), copy(r->right), r->npl);
        }

        node *merge(node *r1, node *r2) {
            //r1 and r2 should be discarded after merge (as they are going to be null or full)
            if (r1 == nullptr) return r2;
            if (r2 == nullptr) return r1;
            if (cmp(r1->val, r2->val)) swap(r1, r2);
            r1->right = merge(r1->right, r2); //r2 not null, so r1->right not null
            //what if r1->left = nullptr?
            if (r1->left == nullptr || r1->right->npl > r1->left->npl) swap(r1->left, r1->right);
            r1->npl = (r1->right) ? r1->right->npl + 1 : 0;
            return r1;
        }

        //---------------------------------------------
    public:

        priority_queue() = default;

        priority_queue(const priority_queue &other) : root(copy(other.root)), size_(other.size_), cmp(other.cmp) {}

        ~priority_queue() { free(root); }

        priority_queue &operator=(const priority_queue &other) {
            if (this == &other) return *this;
            free(root);
            root = copy(other.root);
            size_ = other.size_;
            cmp = other.cmp;
            return *this;
        }

        /**
         * get the top of the queue.
         * @return a reference of the top element.
         * throw container_is_empty if empty() returns true;
         */
        const T &top() const {
            if (root) return root->val;
            else throw container_is_empty();
        }

        /**
         * push new element to the priority queue.
         */
        void push(const T &e) {
            if (empty()) { //first push
                root = new node(e);
                size_ = 1;
                return;
            }
            try { //return if bed pushed
                cmp(root->val, e);
            } catch (exception &ex) {
                return;
            }
            node *r2 = new node(e);
            root = merge(root, r2);
            size_++;
        }

        /**
         * delete the top element.
         * throw container_is_empty if empty() returns true;
         */
        void pop() {
            if (empty()) throw container_is_empty();
            node *pre = root;
            root = merge(root->left, root->right);
            delete pre;
            size_--;
        }

        /**
         * return the number of the elements.
         */
        size_t size() const {
            return size_;
        }

        /**
         * check if the container has at least an element.
         * @return true if it is empty, false if it has at least an element.
         */
        bool empty() const {
            return root == nullptr;
        }

        /**
         * merge two priority_queues with at least O(logn) complexity.
         * clear the other priority_queue.
         */
        void merge(priority_queue &other) {
            if (other.empty()) return;
            root = merge(root, other.root);
            size_ += other.size_;
            other.root = nullptr;
            other.size_ = 0;
            //Be careful not to free other.root as its content maybe important! Using &= would cause bug.
        }

    };

}

#endif

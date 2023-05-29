#ifndef TICKET_SYSTEM_BPT_H
#define TICKET_SYSTEM_BPT_H

#include <iostream>
#include <cstring>
#include <cmath>
#include <fstream>
#include "../STLite/vector.hpp"
#include "../STLite/exceptions.hpp"
#include "data.h"

namespace my {

    using sjtu::error;

    constexpr int halfBlockSize = 50;

    template<class K, class T>
    class BPT {
    public:
        explicit BPT(const std::string &name);

        ~BPT();

        void insert(const K &key, const T &value); //replace if element already exists

        bool erase(const K &key); //return false if element not found

        bool find(const K &key, T &output); //return false if element no find (no change to output)

    private:
        constexpr static int Degree = halfBlockSize << 1 | 1; //odd number required here
        //we keep one empty space for split

        constexpr static int firstNodeAddress = (sizeof(long) << 1) + sizeof(int);
        //we write root_pos, endAddress and size_ at the beginning of file

        std::fstream file;

        long root_pos = 0;
        long endAddress = firstNodeAddress;
        int size_ = 0;

        File<T> data;

        struct Element {
            K key{};
            T value{};

            Element() = default;

            Element(const K &k, const T &v) : key(k), value(v) {}

            explicit Element(const K &k) : key(k) {}

            bool operator<(const Element &e) const {
                if (key == e.key) return value < e.value;
                else return key < e.key;
            }

            bool operator<=(const Element &e) const {
                if (key == e.key) return value <= e.value;
                else return key < e.key;
            }

            bool operator>(const Element &e) const {
                if (key == e.key) return value > e.value;
                else return key > e.key;
            }

            bool operator>=(const Element &e) const {
                if (key == e.key) return value >= e.value;
                else return key > e.key;
            }

            bool operator==(const Element &e) const { return key == e.key && value == e.value; }

            bool operator!=(const Element &e) const { return key != e.key || value != e.value; }
        };

        struct Node {
            bool isLeaf = true; //new created as leaf
            int size = 0;
            long fa = 0;

            K k[Degree]{};

            long ptr[Degree + 1]{0}; // when node is leaf, ptr[Degree] points to next leaf

            Node() = default;

            Node(const Node &n) : size(n.size), fa(n.fa), isLeaf(n.isLeaf) {
                memcpy(k, n.k, sizeof(K) * Degree);
                memcpy(ptr, n.ptr, sizeof(long) * (Degree + 1));
            }

            Node &operator=(const Node &n) { //deep copy
                if (this == &n) return *this;
                size = n.size;
                fa = n.fa;
                isLeaf = n.isLeaf;
                memcpy(k, n.k, sizeof(K) * Degree);
                memcpy(ptr, n.ptr, sizeof(long) * (Degree + 1));
                return *this;
            }

            int lowerBound(const K &key) { //return first e[i] >= key, no find then return size
                int l = 0, r = size - 1, mid;
                while (l <= r) {
                    mid = (l + r) >> 1;
                    if (key > k[mid]) l = mid + 1;
                    else r = mid - 1; //e[r+1] >= key
                }
                return l;
            }

            int upperBound(const K &key) { //return first e[i] > key, no find then return size
                int l = 0, r = size - 1, mid;
                while (l <= r) {
                    mid = (l + r) >> 1;
                    if (key >= k[mid]) l = mid + 1;
                    else r = mid - 1;
                }
                return l;
            }

            bool find(const K &key) {
                int i = lowerBound(key);
                if (i == size) return false;
                return k[i] == key;
            }
        };

        inline void readNode(long address, Node &node) {
            file.seekg(address);
            file.read(reinterpret_cast<char *>(&node), sizeof(Node));
        }

        inline void writeNode(long address, const Node &node) {
            file.seekp(address);
            file.write(reinterpret_cast<const char *>(&node), sizeof(Node));
        }

        inline Node root() {
            Node node;
            readNode(root_pos, node);
            return node;
        }

        long findLeafNode(const K &key, Node &node) { //get required node in node
            if (size_ == 0) {
                node = Node();
                return 0;
            }
            long addr = root_pos; //always points to the address of node
            readNode(addr, node);
            while (!node.isLeaf) {
                addr = node.ptr[node.upperBound(key)];
                readNode(addr, node);
            }
            return addr; //if root is leaf, return root_pos
        }

        void insertInternal(long curAddr, long rightAddr, const K &key);

        //todo

        static inline void removeVal(int index, Node &node) { //remove e[index] from node (no change for ptr)
            if (node.size == 0) return;
            for (int i = index; i < node.size - 1; ++i)
                node.k[i] = node.k[i + 1];
            node.k[--node.size] = K();
        }

        static inline void insertVal(int index, const Element &e, Node &node) { //insert e to node.e[index]
            for (int i = node.size++; i > index; --i) node.e[i] = node.e[i - 1];
            node.e[index] = e;
        }

        static inline void mergeLeafNode(Node &left, const Node &right) { //merge right node to left
            //node right remain unchanged, which is going to be discarded
            for (int j = 0; j < right.size; ++j)
                left.e[left.size + j] = right.e[j];
            left.size += right.size;
            left.ptr[1] = right.ptr[1];
        }

        void eraseAdjust(long address, Node &node);

        void eraseAdjustInternal(long address, Node &node);

    };


    //-----------------------------------core implement--------------------------------------------


    template<class K, class T>
    BPT<K, T>::BPT(const std::string &name):data(name + "_dataFile") { //open file
        file.open(name);
        if (file) {
            file.seekg(0);
            file.read(reinterpret_cast<char *>(&root_pos), sizeof(long));
            file.read(reinterpret_cast<char *>(&endAddress), sizeof(long));
            file.read(reinterpret_cast<char *>(&size_), sizeof(int));
        } else { //create new file, root_pos = 0
            file.open(name, std::ios::out);
            file.seekp(0);
            file.write(reinterpret_cast<char *>(&root_pos), sizeof(long));
            file.write(reinterpret_cast<char *>(&endAddress), sizeof(long));
            file.write(reinterpret_cast<char *>(&size_), sizeof(int));
            file.close();
            file.open(name);
        }
    }

    template<class K, class T>
    BPT<K, T>::~BPT() {
        file.seekp(0);
        file.write(reinterpret_cast<char *>(&root_pos), sizeof(long));
        file.write(reinterpret_cast<char *>(&endAddress), sizeof(long));
        file.write(reinterpret_cast<char *>(&size_), sizeof(int));
        file.close();
    }

    template<class K, class T>
    bool BPT<K, T>::find(const K &key, T &output) {
        if (size_ == 0) return false;
        Node tmp;
        findLeafNode(key, tmp);
        int i = tmp.lowerBound(key);
        if (i == tmp.size) return false;
        data.read(tmp.ptr[i], output);
        return true;
    }

    template<class K, class T>
    void BPT<K, T>::insert(const K &key, const T &value) {
        if (root_pos == 0) { //empty Tree
            Node root;
            root.fa = 0;
            root.size = 1;
            root.k[0] = key;
            root.ptr[0] = data.add(value);
            root_pos = firstNodeAddress;
            writeNode(root_pos, root);
            size_ = 1;
            endAddress += sizeof(Node);
            return;
        }
        Node tmp;
        long tmp_pos = findLeafNode(key, tmp);

        int i = tmp.lowerBound(key);
        if (tmp.k[i] == key && i != tmp.size) return;
        else size_++; //(will) insert successfully

        for (int j = tmp.size; j > i; --j) { //size not updated
            tmp.k[j] = tmp.k[j - 1];
            tmp.ptr[j] = tmp.ptr[j - 1];
        }
        tmp.k[i] = key;
        tmp.ptr[i] = data.add(value);
        tmp.size++;

        if (tmp.size < Degree) {
            writeNode(tmp_pos, tmp);
        } else { //we have to split node now
            Node newLeaf; //at right
            newLeaf.size = tmp.size - halfBlockSize;
            tmp.size = halfBlockSize;
            for (int j = 0; j < newLeaf.size; ++j) {
                newLeaf.k[j] = tmp.k[halfBlockSize + j];
                newLeaf.ptr[j] = tmp.ptr[halfBlockSize + j];
            }
            newLeaf.fa = tmp.fa;
            newLeaf.ptr[Degree] = tmp.ptr[Degree];
            writeNode(endAddress, newLeaf);
            tmp.ptr[Degree] = endAddress;
            writeNode(tmp_pos, tmp);
            endAddress += sizeof(Node);
            insertInternal(tmp.fa, tmp.ptr[Degree], newLeaf.k[0]);
        }
    }

    template<class K, class T>
    void BPT<K, T>::insertInternal(long curAddr, long rightAddr, const K &key) {
        if (curAddr == 0) { //new root
            Node newNode;
            newNode.size = 1;
            newNode.isLeaf = false;
            newNode.k[0] = key;
            newNode.ptr[0] = root_pos;
            newNode.ptr[1] = rightAddr;

            Node rightNode;
            readNode(rightAddr, rightNode);
            rightNode.fa = endAddress;
            writeNode(rightAddr, rightNode);
            //
            Node oldRoot = root();
            oldRoot.fa = endAddress;
            writeNode(root_pos, oldRoot);
            //
            root_pos = endAddress;
            writeNode(root_pos, newNode);
            endAddress += sizeof(Node);
            return;
        }
        Node curNode;
        readNode(curAddr, curNode);
        //
        int i = curNode.lowerBound(key);
        if (curNode.k[i] == key && i != curNode.size) {
            std::cout << "BPT insert internal error: element to insert already exists" << std::endl;
            throw sjtu::bpt_error();
        } //safety check

        for (int j = curNode.size; j > i; --j) {
            curNode.k[j] = curNode.k[j - 1];
            curNode.ptr[j + 1] = curNode.ptr[j];
        }
        curNode.k[i] = key;
        curNode.ptr[i + 1] = rightAddr;
        curNode.size++;

        if (curNode.size < Degree)
            writeNode(curAddr, curNode);
        else { //split interval node
            Node newNode;
            K newKey = curNode.k[halfBlockSize];
            newNode.size = curNode.size - halfBlockSize - 1;
            curNode.size = halfBlockSize;
            newNode.fa = curNode.fa;
            for (int j = 0; j < newNode.size; ++j) {
                newNode.k[j] = curNode.k[halfBlockSize + 1 + j];
                newNode.ptr[j] = curNode.ptr[halfBlockSize + 1 + j];
                curNode.k[halfBlockSize + 1 + j] = K();
                curNode.ptr[halfBlockSize + 1 + j] = 0;
            }
            newNode.ptr[newNode.size] = curNode.ptr[Degree];
            curNode.ptr[Degree] = 0;
            curNode.k[halfBlockSize] = K();

            Node son; //debug: don't forget to change son's father!
            for (int j = 0; j <= newNode.size; ++j) {
                readNode(newNode.ptr[j], son);
                son.fa = endAddress;
                writeNode(newNode.ptr[j], son);
            }

            writeNode(endAddress, newNode);
            endAddress += sizeof(Node);
            writeNode(curAddr, curNode);
            insertInternal(curNode.fa, endAddress - sizeof(Node), newKey);
        }
    }

    /*
    template<class K, class T>
    bool BPT<K, T>::erase(const K &key, const T &value) {
        if (size_ == 0) return false;
        Element ele(key, value);
        Node tmp;
        long tmp_pos = findLeafNode(ele, tmp); //tmp is a leaf node
        int i = tmp.lowerBound(ele);
        if (tmp.e[i] != ele || i == tmp.size) return false; //element no found
        else { //tmp.e[i] = ele
            size_--;
            if (tmp_pos == root_pos) { //root as leaf, only root node
                if (size_ == 0) { //clear tree
                    writeNode(root_pos, Node());
                    root = Node();
                    root_pos = 0;
                    endAddress = firstNodeAddress;
                } else {
                    removeVal(i, root);
                    writeNode(root_pos, root);
                }
                return true;
            }
            //now tmp != root
            removeVal(i, tmp);
            if (tmp.size >= halfBlockSize) {
                writeNode(tmp_pos, tmp);
                return true;
            }
            eraseAdjust(tmp_pos, tmp);
            return true;
        }
    }

    template<class K, class T>
    void BPT<K, T>::eraseAdjust(long address, BPT::Node &node) { //node is a leaf and not root
        Node tmp;
        Node &faNode = (node.fa == root_pos) ? root : tmp;
        if (node.fa != root_pos) readNode(node.fa, faNode);
        //faNode maybe root!

        int i = faNode.upperBound(node.e[0]) - 1; //maybe -1
        long right_pos = 0, left_pos = 0;
        if (i != faNode.size - 1) right_pos = faNode.ptr[i + 2];
        if (i >= 0) left_pos = faNode.ptr[i];
        Node rightNode;
        if (right_pos) { //check if borrow from right available
            readNode(right_pos, rightNode);
            if (rightNode.size > halfBlockSize) { //borrow successfully
                node.e[node.size++] = rightNode.e[0];
                removeVal(0, rightNode);
                faNode.e[i + 1] = rightNode.e[0];
                writeNode(node.fa, faNode);
                writeNode(address, node);
                writeNode(right_pos, rightNode);
                return;
            }
        }
        Node leftNode;
        if (left_pos) { //check if borrow from left available
            readNode(left_pos, leftNode);
            if (leftNode.size > halfBlockSize) { //borrow successfully
                insertVal(0, leftNode.e[--leftNode.size], node);
                leftNode.e[leftNode.size] = Element();
                faNode.e[i] = node.e[0];
                writeNode(node.fa, faNode);
                writeNode(address, node);
                writeNode(left_pos, leftNode);
                return;
            }
        }
        //cannot borrow, we have to merge now
        if (right_pos) {
            //merge rightNode to node
            mergeLeafNode(node, rightNode);
            for (int j = i + 1; j < faNode.size - 1; ++j) {
                faNode.e[j] = faNode.e[j + 1];
                faNode.ptr[j + 1] = faNode.ptr[j + 2];
            }
            --faNode.size;
            faNode.e[faNode.size] = Element();
            faNode.ptr[faNode.size + 1] = 0;
            writeNode(node.fa, faNode);
            writeNode(address, node);
            writeNode(right_pos, Node());
        } else if (left_pos) {
            mergeLeafNode(leftNode, node);
            for (int j = i; j < faNode.size - 1; ++j) {
                faNode.e[j] = faNode.e[j + 1];
                faNode.ptr[j + 1] = faNode.ptr[j + 2];
            }
            --faNode.size;
            faNode.e[faNode.size] = Element();
            faNode.ptr[faNode.size + 1] = 0;
            writeNode(node.fa, faNode);
            writeNode(left_pos, leftNode);
            writeNode(address, Node());
        } else {
            std::cout << "erase adjust error: no siblings" << std::endl;
            throw sjtu::bpt_error();
        } //safety check
        eraseAdjustInternal(node.fa, faNode); //faNode size -1
    }

    template<class K, class T>
    void BPT<K, T>::eraseAdjustInternal(long address, BPT::Node &node) { //node maybe &root
        if (node.fa == 0) { //root node
            if (root_pos != address) {
                std::cout << "erase adjust internal error: root chaos" << std::endl;
                throw sjtu::bpt_error();
            } //safety check
            if (node.size == 0) { //erase empty root
                root_pos = node.ptr[0];
                readNode(root_pos, root);
                root.fa = 0;
                writeNode(root_pos, root);
                writeNode(address, Node());
            } else if (&node != &root) root = node;
            return;
        }
        //now node not root
        if (node.size >= halfBlockSize) return;
        Node tmp;
        Node &faNode = (node.fa == root_pos) ? root : tmp;
        if (node.fa != root_pos) readNode(node.fa, faNode);

        int i = faNode.upperBound(node.e[0]) - 1; //maybe -1
        long right_pos = 0, left_pos = 0;
        if (i != faNode.size - 1) right_pos = faNode.ptr[i + 2];
        if (i >= 0) left_pos = faNode.ptr[i];
        Node rightNode;
        if (right_pos) { //check if borrow from right available
            readNode(right_pos, rightNode);
            if (rightNode.size > halfBlockSize) { //borrow successfully
                Node son;
                readNode(rightNode.ptr[0], son);
                node.e[node.size++] = faNode.e[i + 1]; //not rightNode.e[0]
                node.ptr[node.size] = rightNode.ptr[0];
                son.fa = address;
                faNode.e[i + 1] = rightNode.e[0];
                for (int j = 1; j <= rightNode.size; ++j) {
                    rightNode.e[j - 1] = rightNode.e[j];
                    rightNode.ptr[j - 1] = rightNode.ptr[j];
                }
                rightNode.ptr[rightNode.size--] = 0;

                writeNode(node.fa, faNode);
                writeNode(address, node);
                writeNode(right_pos, rightNode);
                writeNode(node.ptr[node.size], son);
                return;
            }
        }
        Node leftNode;
        if (left_pos) { //check if borrow from left available
            readNode(left_pos, leftNode);
            if (leftNode.size > halfBlockSize) { //borrow successfully
                Node son;
                readNode(leftNode.ptr[leftNode.size], son);
                node.ptr[node.size + 1] = node.ptr[node.size];
                for (int j = node.size; j > 0; --j) {
                    node.e[j] = node.e[j - 1];
                    node.ptr[j] = node.ptr[j - 1];
                }
                node.e[0] = faNode.e[i];
                node.ptr[0] = leftNode.ptr[leftNode.size];
                node.size++;
                son.fa = address;
                faNode.e[i] = leftNode.e[leftNode.size - 1];
                leftNode.size--;
                leftNode.e[leftNode.size] = Element();
                leftNode.ptr[leftNode.size + 1] = 0;

                writeNode(node.fa, faNode);
                writeNode(address, node);
                writeNode(left_pos, leftNode);
                writeNode(node.ptr[0], son);
                return;
            }
        }
        //cannot borrow, we have to merge now
        if (right_pos) {
            //merge rightNode to node
            node.e[node.size] = faNode.e[i + 1];
            node.ptr[node.size + 1] = rightNode.ptr[0];
            Node son;
            readNode(rightNode.ptr[0], son);
            son.fa = address;
            writeNode(rightNode.ptr[0], son);
            for (int j = 0; j < rightNode.size; ++j) {
                node.e[node.size + 1 + j] = rightNode.e[j];
                node.ptr[node.size + 2 + j] = rightNode.ptr[j + 1];
                readNode(rightNode.ptr[j + 1], son);
                son.fa = address;
                writeNode(rightNode.ptr[j + 1], son);
            }
            node.size += rightNode.size + 1;

            for (int j = i + 1; j < faNode.size - 1; ++j) {
                faNode.e[j] = faNode.e[j + 1];
                faNode.ptr[j + 1] = faNode.ptr[j + 2];
            }
            --faNode.size;
            faNode.e[faNode.size] = Element();
            faNode.ptr[faNode.size + 1] = 0;
            writeNode(node.fa, faNode);
            writeNode(address, node);
            writeNode(right_pos, Node());
        } else if (left_pos) {
            leftNode.e[leftNode.size] = faNode.e[i];
            leftNode.ptr[leftNode.size + 1] = node.ptr[0];
            Node son;
            readNode(node.ptr[0], son);
            son.fa = left_pos;
            writeNode(node.ptr[0], son);
            for (int j = 0; j < node.size; ++j) {
                leftNode.e[leftNode.size + 1 + j] = node.e[j];
                leftNode.ptr[leftNode.size + 2 + j] = node.ptr[j + 1];
                readNode(node.ptr[j + 1], son);
                son.fa = left_pos;
                writeNode(node.ptr[j + 1], son);
            }
            leftNode.size += node.size + 1;

            for (int j = i; j < faNode.size - 1; ++j) {
                faNode.e[j] = faNode.e[j + 1];
                faNode.ptr[j + 1] = faNode.ptr[j + 2];
            }
            --faNode.size;
            faNode.e[faNode.size] = Element();
            faNode.ptr[faNode.size + 1] = 0;
            writeNode(node.fa, faNode);
            writeNode(left_pos, leftNode);
            writeNode(address, Node());
        } else {
            std::cout << "erase adjust internal error: no siblings" << std::endl;
            throw sjtu::bpt_error();
        } //safety check
        eraseAdjustInternal(node.fa, faNode);
    }
    */

}

#endif //TICKET_SYSTEM_BPT_H

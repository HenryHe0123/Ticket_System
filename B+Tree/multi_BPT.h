#ifndef TICKET_SYSTEM_MULTI_BPT_H
#define TICKET_SYSTEM_MULTI_BPT_H

#include <iostream>
#include <cstring>
#include <cmath>
#include <fstream>
#include "../STLite/vector.hpp"
#include "../STLite/exceptions.hpp"

/*
 * Class: my::multiBPT
 * ---------------------
 * This class similarly implements the functions of multimap.
 * Typical usage of which looks like this:
 *
 *    multiBPT<key_type,value_type> multimap("file");
 *
 *    multimap.insert(key,value); //do nothing if element already exists
 *
 *    multimap.erase(key,value); //return false if element not found
 *
 *    vector<value_type> output; //receive output in ascending order
 *    multimap.find(key,output);
 *
 *    if(multimap.empty()) {...}
 *
 *    int size = multimap.size();
 *
 */

namespace my {

    template<class K, class T>
    class multiBPT {
    public:
        explicit multiBPT(const std::string &name);

        ~multiBPT();

        void insert(const K &key, const T &value); //do nothing if element already exists

        bool erase(const K &key, const T &value); //return false if element not found

        void find(const K &key, sjtu::vector<T> &output); //return in ascending order,empty if not found

        size_t size() const { return size_; }

        bool empty() const { return size_ == 0; }

    private:
        constexpr static int halfBlockSizeForMulti = 1950 / (sizeof(long) + sizeof(K) + sizeof(T));

        constexpr static int Degree = halfBlockSizeForMulti << 1 | 1; //odd number required here
        //we keep one empty space for split

        constexpr static int firstNodeAddress = (sizeof(long) << 1) + sizeof(int);
        //we write root_pos, endAddress and size_ at the beginning of file

        std::fstream file;

        long root_pos = 0;
        long endAddress = firstNodeAddress;
        int size_ = 0;

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
            int size = 0;
            long fa = 0;

            Element e[Degree]{}; //we store value even in non-leaf node

            long ptr[Degree + 1] = {0}; // when node is leaf, ptr[1] points to next leaf, ptr[0] = 0

            Node() = default;

            Node(const Node &n) : size(n.size), fa(n.fa) {
                memcpy(e, n.e, sizeof(Element) * Degree);
                memcpy(ptr, n.ptr, sizeof(long) * (Degree + 1));
            }

            Node &operator=(const Node &n) { //deep copy
                if (this == &n) return *this;
                size = n.size;
                fa = n.fa;
                memcpy(e, n.e, sizeof(Element) * Degree);
                memcpy(ptr, n.ptr, sizeof(long) * (Degree + 1));
                return *this;
            }

            inline bool isLeaf() { return ptr[0] == 0; }

            int lowerBound(const K &key) { //return first e[i] >= key, no find then return size
                int l = 0, r = size - 1, mid;
                while (l <= r) {
                    mid = (l + r) >> 1;
                    if (key > e[mid].key) l = mid + 1;
                    else r = mid - 1; //e[r+1] >= key
                }
                return l;
            }

            int upperBound(const K &key) { //return first e[i] > key, no find then return size
                int l = 0, r = size - 1, mid;
                while (l <= r) {
                    mid = (l + r) >> 1;
                    if (key >= e[mid].key) l = mid + 1;
                    else r = mid - 1;
                }
                return l;
            }

            int lowerBound(const Element &ele) {
                int l = 0, r = size - 1, mid;
                while (l <= r) {
                    mid = (l + r) >> 1;
                    if (ele > e[mid]) l = mid + 1;
                    else r = mid - 1;
                }
                return l;
            }

            int upperBound(const Element &ele) {
                int l = 0, r = size - 1, mid;
                while (l <= r) {
                    mid = (l + r) >> 1;
                    if (ele >= e[mid]) l = mid + 1;
                    else r = mid - 1;
                }
                return l;
            }

            bool find(const K &key) {
                int i = lowerBound(key);
                if (i == size) return false;
                return e[i].key == key;
            }

            bool find(const Element &ele) {
                int i = lowerBound(ele);
                if (i == size) return false;
                return e[i] == ele;
            }

        } root;

        inline void readNode(long address, Node &node) {
            file.seekg(address);
            file.read(reinterpret_cast<char *>(&node), sizeof(Node));
        }

        inline void writeNode(long address, const Node &node) {
            file.seekp(address);
            file.write(reinterpret_cast<const char *>(&node), sizeof(Node));
        }

        long findLeafNode(const K &key, Node &node) { //get required node in node
            if (size_ == 0) {
                node = Node();
                return 0;
            }
            long addr = root_pos; //always points to the address of node
            node = root;
            while (!node.isLeaf()) {
                addr = node.ptr[node.lowerBound(key)]; //debug: not upperbound! Different with element version.
                readNode(addr, node);
            }
            return addr; //if root is leaf, return root_pos
        }

        long findLeafNode(const Element &ele, Node &node) { //we actually need hole element here, not only key
            if (size_ == 0) {
                node = Node();
                return 0;
            }
            long addr = root_pos; //always points to the address of node
            node = root;
            while (!node.isLeaf()) {
                addr = node.ptr[node.upperBound(ele)];
                readNode(addr, node);
            }
            return addr; //if root is leaf, return root_pos
        }

        void insertInternal(long curAddr, long rightAddr, const Element &ele);

        static inline void removeVal(int index, Node &node) { //remove e[index] from node (no change for ptr)
            if (node.size == 0) return;
            for (int i = index; i < node.size - 1; ++i)
                node.e[i] = node.e[i + 1];
            node.e[--node.size] = Element();
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
    multiBPT<K, T>::multiBPT(const std::string &name) { //open file
        file.open(name);
        if (file) {
            file.seekg(0);
            file.read(reinterpret_cast<char *>(&root_pos), sizeof(long));
            file.read(reinterpret_cast<char *>(&endAddress), sizeof(long));
            file.read(reinterpret_cast<char *>(&size_), sizeof(int));
            if (root_pos) {
                file.seekg(root_pos);
                file.read(reinterpret_cast<char *>(&root), sizeof(Node));
            }
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
    multiBPT<K, T>::~multiBPT() {
        file.seekp(0);
        file.write(reinterpret_cast<char *>(&root_pos), sizeof(long));
        file.write(reinterpret_cast<char *>(&endAddress), sizeof(long));
        file.write(reinterpret_cast<char *>(&size_), sizeof(int));
        file.close();
    }

    template<class K, class T>
    void multiBPT<K, T>::find(const K &key, sjtu::vector<T> &output) {
        output.clear();
        if (size_ == 0) return;
        Node tmp;
        findLeafNode(key, tmp);
        for (int i = tmp.lowerBound(key); i < tmp.size; ++i) {
            if (tmp.e[i].key == key) output.push_back(tmp.e[i].value);
            else return;
        }
        while (tmp.ptr[1]) {
            readNode(tmp.ptr[1], tmp);
            for (int i = 0; i < tmp.size; ++i) {
                if (tmp.e[i].key == key) output.push_back(tmp.e[i].value);
                else return;
            }
        }
    }

    template<class K, class T>
    void multiBPT<K, T>::insert(const K &key, const T &value) {
        Element ele(key, value);
        if (root_pos == 0) { //empty Tree
            if (size_) {
                std::cout << "insert error: size_ != 0 while root_pos = 0" << std::endl;
                throw sjtu::bpt_error();
            } //safety check
            root.fa = 0;
            root.size = 1;
            root.e[0] = ele;
            root.ptr[0] = root.ptr[1] = 0;
            root_pos = firstNodeAddress;
            writeNode(root_pos, root);
            size_ = 1;
            endAddress += sizeof(Node);
            return;
        }
        Node ttmp;
        long tmp_pos = findLeafNode(ele, ttmp);
        Node &tmp = (tmp_pos == root_pos) ? root : ttmp; //be careful about root!

        int i = tmp.lowerBound(ele);
        if (tmp.e[i] == ele && i != tmp.size) return;
        else size_++; //(will) insert successfully

        for (int j = tmp.size; j > i; --j) tmp.e[j] = tmp.e[j - 1]; //size not updated
        tmp.e[i] = ele;
        tmp.size++;

        if (tmp.size < Degree) {
            writeNode(tmp_pos, tmp);
        } else { //we have to split node now
            Node newLeaf; //at right
            newLeaf.size = tmp.size - halfBlockSizeForMulti;
            tmp.size = halfBlockSizeForMulti;
            for (int j = 0; j < newLeaf.size; ++j) {
                newLeaf.e[j] = tmp.e[halfBlockSizeForMulti + j];
                tmp.e[halfBlockSizeForMulti + j] = Element();
            }
            newLeaf.fa = tmp.fa;
            newLeaf.ptr[1] = tmp.ptr[1];
            writeNode(endAddress, newLeaf);
            tmp.ptr[1] = endAddress;
            writeNode(tmp_pos, tmp);
            endAddress += sizeof(Node);
            insertInternal(tmp.fa, tmp.ptr[1], newLeaf.e[0]);
        }
    }

    template<class K, class T>
    void multiBPT<K, T>::insertInternal(long curAddr, long rightAddr, const multiBPT::Element &ele) {
        if (curAddr == 0) { //new root
            Node newNode;
            newNode.size = 1;
            newNode.e[0] = ele;
            newNode.ptr[0] = root_pos;
            newNode.ptr[1] = rightAddr;
            root.fa = endAddress; //still old root
            writeNode(root_pos, root);
            Node rightNode;
            readNode(rightAddr, rightNode);
            rightNode.fa = endAddress;
            writeNode(rightAddr, rightNode);
            //
            root_pos = endAddress;
            root = newNode;
            writeNode(root_pos, newNode);
            endAddress += sizeof(Node);
            return;
        }
        Node tmp;
        Node &curNode = (curAddr == root_pos) ? root : tmp;
        if (curAddr != root_pos) readNode(curAddr, curNode);
        //
        int i = curNode.lowerBound(ele);
        if (curNode.e[i] == ele && i != curNode.size) {
            std::cout << "insert internal error: element to insert already exists" << std::endl;
            throw sjtu::bpt_error();
        } //safety check

        for (int j = curNode.size; j > i; --j) {
            curNode.e[j] = curNode.e[j - 1];
            curNode.ptr[j + 1] = curNode.ptr[j];
        }
        curNode.e[i] = ele;
        curNode.ptr[i + 1] = rightAddr;
        curNode.size++;

        if (curNode.size < Degree)
            writeNode(curAddr, curNode);
        else { //split interval node
            Node newNode;
            Element newEle = curNode.e[halfBlockSizeForMulti];
            newNode.size = curNode.size - halfBlockSizeForMulti - 1;
            curNode.size = halfBlockSizeForMulti;
            newNode.fa = curNode.fa;
            for (int j = 0; j < newNode.size; ++j) {
                newNode.e[j] = curNode.e[halfBlockSizeForMulti + 1 + j];
                newNode.ptr[j] = curNode.ptr[halfBlockSizeForMulti + 1 + j];
                curNode.e[halfBlockSizeForMulti + 1 + j] = Element();
                curNode.ptr[halfBlockSizeForMulti + 1 + j] = 0;
            }
            newNode.ptr[newNode.size] = curNode.ptr[Degree];
            curNode.ptr[Degree] = 0;
            curNode.e[halfBlockSizeForMulti] = Element();

            Node son; //debug: don't forget to change son's father!
            for (int j = 0; j <= newNode.size; ++j) {
                readNode(newNode.ptr[j], son);
                son.fa = endAddress;
                writeNode(newNode.ptr[j], son);
            }

            writeNode(endAddress, newNode);
            endAddress += sizeof(Node);
            writeNode(curAddr, curNode);
            insertInternal(curNode.fa, endAddress - sizeof(Node), newEle);
        }
    }

    template<class K, class T>
    bool multiBPT<K, T>::erase(const K &key, const T &value) {
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
            if (tmp.size >= halfBlockSizeForMulti) {
                writeNode(tmp_pos, tmp);
                return true;
            }
            eraseAdjust(tmp_pos, tmp);
            return true;
        }
    }

    template<class K, class T>
    void multiBPT<K, T>::eraseAdjust(long address, multiBPT::Node &node) { //node is a leaf and not root
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
            if (rightNode.size > halfBlockSizeForMulti) { //borrow successfully
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
            if (leftNode.size > halfBlockSizeForMulti) { //borrow successfully
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
    void multiBPT<K, T>::eraseAdjustInternal(long address, multiBPT::Node &node) { //node maybe &root
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
        if (node.size >= halfBlockSizeForMulti) return;
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
            if (rightNode.size > halfBlockSizeForMulti) { //borrow successfully
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
            if (leftNode.size > halfBlockSizeForMulti) { //borrow successfully
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

}

#endif //TICKET_SYSTEM_MULTI_BPT_H

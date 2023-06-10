#ifndef TICKET_SYSTEM_CACHE_H
#define TICKET_SYSTEM_CACHE_H

#include <cstring>
#include <fstream>
#include "../STLite/exceptions.hpp"

template<size_t N = 599>
struct HashMapL { //hash-map: long->int
private:
    int begin[N]{0}, next[N]{0}, val[N]{0}, waste[N]{0}, cnt = 0, waste_top = 0, size_ = 0;
    long addr[N]{0};
public:
    HashMapL() = default;

    inline void clear() {
        memset(begin, 0, sizeof(begin));
        memset(next, 0, sizeof(next));
        memset(waste, 0, sizeof(waste));
        memset(addr, 0, sizeof(addr));
        //memset(val, 0, sizeof(val));
        cnt = waste_top = size_ = 0;
    }

    inline void insert(long address, int index) {
        if (fulled()) sjtu::error("HashMapL insert error: fulled already");
        int tmp = address % N;
        int now = waste_top ? waste[waste_top--] : ++cnt;
        val[now] = index;
        next[now] = begin[tmp];
        begin[tmp] = now;
        addr[now] = address;
        ++size_;
    }

    void del(long address, int index) {
        int tmp = address % N, pre = 0;
        for (int i = begin[tmp]; i; i = next[i]) {
            if (addr[i] == address) {
                if (val[i] == index) { //delete i
                    if (pre) next[pre] = next[i];
                    else begin[tmp] = next[i];
                    waste[++waste_top] = i;
                    --size_;
                    return;
                } else sjtu::error("HashMapL delete error: index and address not correspond");
            }
            pre = i;
        }
    }

    bool has(long address) {
        for (int i = begin[address % N]; i; i = next[i]) if (addr[i] == address) return true;
        return false;
    }

    int &operator[](long address) {
        for (int i = begin[address % N]; i; i = next[i]) if (addr[i] == address) return val[i];
        sjtu::error("HashMapL[address] error: address no find");
    }

    inline bool fulled() { return size_ == N - 1; }
};

template<class T>
class Editor {
public:
    inline void write(long address, const T &value) {
        file.seekp(address);
        file.write(reinterpret_cast <const char *> (&value), sizeof(T));
    }

    inline void read(long address, T &value) {
        file.seekg(address);
        file.read(reinterpret_cast <char *> (&value), sizeof(T));
    }

    inline void open(const std::string &name) { file.open(name); }

    inline void close() { file.close(); }

private:
    std::fstream file;
};

//Cache for file

template<class T, size_t N = 32>
class Cache { //LRU
private:
    HashMapL<> index;
    T val[N]{};
    int head = -1, tail = -1, size = 0, pre[N]{0}, to[N]{0};
    long pos[N]{0}; //address of val[i]
    Editor<T> f;
public:
    Cache() {
        memset(pre, -1, sizeof(pre));
        memset(to, -1, sizeof(to));
    };

    ~Cache() {
        for (int i = 0; i < size; ++i) if (pos[i]) f.write(pos[i], val[i]);
        f.close();
    }

    inline void init(const std::string &name) { f.open(name); } //need init before use

    bool has(long addr) { return index.has(addr); }

    T &operator[](long addr) { //address must already have stored value
        return index.has(addr) ? get(addr) : getNew(addr);
    }

    T &get(long addr) { //addr already in index
        int i = index[addr];
        if (i == head) return val[i];
        if (~pre[i]) to[pre[i]] = to[i]; //pre[i] != -1
        if (~to[i]) pre[to[i]] = pre[i];
        if (i == tail) tail = pre[i]; //delete i in list
        pre[i] = -1;
        to[i] = head;
        pre[head] = i;
        head = i; //add i to head
        return val[i];
    }

private:
    T &getNew(long addr) { //addr not in index (but exist)
        int tmp; //new index for v
        if (size == N) {
            f.write(pos[tail], val[tail]);
            index.del(pos[tail], tail);
            tmp = tail;
            tail = pre[tail];
            to[tail] = -1; //get tmp out of tail
            pre[head] = tmp;
            pre[tmp] = -1;
            to[tmp] = head;
            head = tmp; //add tmp to head
        } else {
            tmp = size++;
            if (~head) { //not empty
                pre[head] = tmp;
                to[tmp] = head;
            } else tail = tmp;
            head = tmp;
        } //now tmp available for v as head
        pos[tmp] = addr;
        f.read(addr, val[tmp]);
        index.insert(addr, tmp);
        return val[tmp];
    }

};

//Cache for internal

template<class T, size_t N = 64>
class InterCache { //LRU
private:
    HashMapL<1013> index; //hash as address
    T val[N]{};
    int head = -1, tail = -1, size = 0, pre[N]{0}, to[N]{0}, h[N]{0};
public:
    InterCache() {
        memset(pre, -1, sizeof(pre));
        memset(to, -1, sizeof(to));
    };

    bool has(int hash) { return index.has(hash); }

    T &get(int hash) {
        int i = index[hash];
        if (i == head) return val[i];
        if (~pre[i]) to[pre[i]] = to[i]; //pre[i] != -1
        if (~to[i]) pre[to[i]] = pre[i];
        if (i == tail) tail = pre[i]; //delete i in list
        pre[i] = -1;
        to[i] = head;
        pre[head] = i;
        head = i; //add i to head
        return val[i];
    }

    void add(int hash, const T &v) { //add v to InterCache
        int tmp; //new index for v
        if (size == N) {
            index.del(h[tail], tail);
            tmp = tail;
            tail = pre[tail];
            to[tail] = -1; //get tmp out of tail
            pre[head] = tmp;
            pre[tmp] = -1;
            to[tmp] = head;
            head = tmp; //add tmp to head
        } else {
            tmp = size++;
            if (~head) { //not empty
                pre[head] = tmp;
                to[tmp] = head;
            } else tail = tmp;
            head = tmp;
        } //now tmp available for v as head
        h[tmp] = hash;
        val[tmp] = v;
        index.insert(hash, tmp);
    }

};


#endif //TICKET_SYSTEM_CACHE_H

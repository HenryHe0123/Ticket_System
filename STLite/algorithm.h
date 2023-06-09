#ifndef TICKET_SYSTEM_ALGORITHM_H
#define TICKET_SYSTEM_ALGORITHM_H

#include <cstring>
#include "exceptions.hpp"
#include "vector.hpp"

template<class T, size_t N = 2017>
struct HashMap {
    //need extra hash function
    //the original hash_value should not be same
    //some example N for chose: 599,1013,2017,4099,9001
    //provide a fast query of variables
private:
    int begin[N]{0}, next[N]{0}, id[N]{0}, waste[N]{0}, node_cnt = 0, waste_top = 0;
    //id[i] for original hash_value
    //begin[hash] for the index where the first variable store
    T val[N]{};
public:
    HashMap() = default;

    void clear() {
        memset(begin, 0, sizeof(begin));
        memset(next, 0, sizeof(next));
        memset(id, 0, sizeof(id));
        memset(waste, 0, sizeof(waste));
        node_cnt = waste_top = 0;
    }

    void insert(int hash, const T &v) {
        int tmp = hash % N;
        int now = waste_top ? waste[waste_top--] : ++node_cnt;
        val[now] = v;
        next[now] = begin[tmp];
        begin[tmp] = now;
        id[now] = hash;
    }

    void del(int hash, const T &v) {
        int tmp = hash % N, pre = 0;
        for (int i = begin[tmp]; i; i = next[i]) {
            if (id[i] == hash) {
                if (val[i] == v) { //delete i
                    if (pre) next[pre] = next[i];
                    else begin[tmp] = next[i];
                    waste[++waste_top] = i;
                    return;
                } else sjtu::error("HashMap delete error: v and hash not correspond");
            }
            pre = i;
        }
    }

    bool has(int hash) {
        int tmp = hash % N;
        for (int i = begin[tmp]; i; i = next[i]) if (id[i] == hash) return true;
        return false;
    }

    const T *query(int hash) {
        int tmp = hash % N;
        for (int i = begin[tmp]; i; i = next[i]) if (id[i] == hash) return val + i;
        return nullptr;
    }
};

template<class T>
void swap(T &x, T &y) {
    T tmp = x;
    x = y;
    y = tmp;
}

template<class T, class Cmp>
void sort(sjtu::vector<T> &a, int l, int r, const Cmp &cmp) { //for vector
    //if cmp(a,b) return a<=b, then sort from small to big
    if (l >= r)return;
    int i = l, j = r;
    T flag = a[l];
    while (i < j) {
        while (i < j && cmp(flag, a[j])) --j;
        while (i < j && cmp(a[i], flag)) ++i;
        swap(a[i], a[j]);
    } //i=j
    swap(a[i], a[l]);
    sort(a, l, i - 1, cmp);
    sort(a, i + 1, r, cmp);
}

template<class T>
void mergesort(T *a, int begin, int end) { //[begin,end], from big to small
    if (begin >= end) return;
    int mid = (begin + end) >> 1;
    mergesort(a, begin, mid);
    mergesort(a, mid + 1, end);
    auto *tmp = new T[end - begin + 1];
    int lp = begin, rp = mid + 1, i = 0;
    while (lp <= mid && rp <= end) {
        if (a[lp] > a[rp]) tmp[i++] = a[lp++];
        else tmp[i++] = a[rp++];
    }
    while (lp <= mid) tmp[i++] = a[lp++];
    while (rp <= end) tmp[i++] = a[rp++];
    for (int j = begin; j <= end; ++j)
        a[j] = tmp[j - begin];
    delete[] tmp;
}

template<class T>
void quicksort(T a[], int begin, int end) { //[begin,end], from small to big
    int i = begin, j = end;
    if (i >= j) return;
    T flag = a[i];
    while (i < j) {
        while (i < j && a[j] >= flag) --j;
        while (i < j && a[i] <= flag) ++i;
        swap(a[i], a[j]);
    } //i=j
    swap(a[i], a[begin]);
    quicksort(a, begin, i - 1);
    quicksort(a, i + 1, end);
}

#endif //TICKET_SYSTEM_ALGORITHM_H

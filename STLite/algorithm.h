#ifndef TICKET_SYSTEM_ALGORITHM_H
#define TICKET_SYSTEM_ALGORITHM_H

template<class T>
void swap(T &x, T &y) {
    T tmp = x;
    x = y;
    y = tmp;
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

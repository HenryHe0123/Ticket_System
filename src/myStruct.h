#ifndef TICKET_SYSTEM_MY_STRUCT_H
#define TICKET_SYSTEM_MY_STRUCT_H

#include "../STLite/myString.h"

/*
 * this file implements struct Date and Time and TransferMap
 */

const int daysBeforeMonth[14] = {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
const int dayOfMonth[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

struct Date {
    int month = 6, day = 1;

    Date() = default;

    Date(int m, int d) : month(m), day(d) {}

    Date(const std::string &s) {
        month = (s[0] - '0') * 10 + s[1] - '0';
        day = (s[3] - '0') * 10 + s[4] - '0';
    }

    inline bool operator<(const Date &d) const { return (month == d.month) ? (day < d.day) : (month < d.month); }

    inline bool operator>(const Date &d) const { return (month == d.month) ? (day > d.day) : (month > d.month); }

    inline bool operator<=(const Date &d) const { return (month == d.month) ? (day <= d.day) : (month < d.month); }

    inline bool operator>=(const Date &d) const { return (month == d.month) ? (day >= d.day) : (month > d.month); }

    inline bool operator==(const Date &d) const { return month == d.month && day == d.day; }

    inline bool operator!=(const Date &d) const { return month != d.month || day != d.day; }

    inline Date &operator++() {
        ++day;
        if (day > dayOfMonth[month]) {
            day -= dayOfMonth[month];
            ++month;
        }
        return *this;
    }

    inline Date operator++(int) {
        Date pre = *this;
        ++*this;
        return pre;
    }

    inline Date &operator--() {
        --day;
        if (day <= 0) {
            --month;
            day += dayOfMonth[month];
        }
        return *this;
    }

    inline Date operator--(int) {
        Date pre = *this;
        --*this;
        return pre;
    }

    inline int operator-(const Date &d) const {
        return daysBeforeMonth[month] + day - daysBeforeMonth[d.month] - d.day;
    }

    inline Date &operator+=(int i) {
        day += i;
        while (day > dayOfMonth[month])
            day -= dayOfMonth[month++];
        return *this;
    }

    inline Date &operator-=(int i) {
        day -= i;
        while (day <= 0)
            day += dayOfMonth[--month];
        return *this;
    }

    inline Date operator+(int x) const {
        Date tmp = *this;
        tmp += x;
        return tmp;
    }

    inline Date operator-(int x) const {
        Date tmp = *this;
        tmp -= x;
        return tmp;
    }

    friend std::ostream &operator<<(std::ostream &os, const Date &d) {
        os << d.month / 10 << d.month % 10 << '-' << d.day / 10 << d.day % 10;
        return os;
    }
};

struct Time {
    int h = 0, m = 0;

    Time() = default;

    Time(int h, int m) : h(h), m(m) {}

    Time(const std::string &s) {
        h = (s[0] - '0') * 10 + s[1] - '0';
        m = (s[3] - '0') * 10 + s[4] - '0';
    }

    inline bool operator<(const Time &t) const { return (h == t.h) ? (m < t.m) : (h < t.h); }

    inline bool operator>(const Time &t) const { return (h == t.h) ? (m > t.m) : (h > t.h); }

    inline bool operator<=(const Time &t) const { return (h == t.h) ? (m <= t.m) : (h < t.h); }

    inline bool operator>=(const Time &t) const { return (h == t.h) ? (m >= t.m) : (h > t.h); }

    inline bool operator==(const Time &t) const { return h == t.h && m == t.m; }

    inline bool operator!=(const Time &t) const { return h != t.h || m != t.m; }

    inline int operator-(const Time &t) const { return (h - t.h) * 60 + m - t.m; }

    inline Time operator+=(int x) { //h maybe more than 24
        m += x;
        h += m / 60;
        m %= 60;
        if (m < 0) {
            m += 60;
            --h;
        }
        return *this;
    }

    inline Time operator+(int x) {
        Time tmp = *this;
        tmp += x;
        return tmp;
    }

    inline Time operator-=(int x) { return this->operator+=(-x); }

    inline Time operator-(int x) { return *this + (-x); }

    friend std::ostream &operator<<(std::ostream &os, const Time &t) {
        os << t.h / 10 << t.h % 10 << ':' << t.m / 10 << t.m % 10;
        return os;
    }

};

struct Date_Time {
    Date date;
    Time time;

    friend std::ostream &operator<<(std::ostream &os, const Date_Time &dt) {
        os << dt.date << ' ' << dt.time;
        return os;
    }

    inline Date_Time operator+=(int x) {
        time += x;
        date += time.h / 24;
        time.h %= 24;
        if (time.h < 0) {
            --date;
            time.h += 24;
        }
        return *this;
    }

    inline Date_Time operator+(int x) {
        Date_Time tmp = *this;
        tmp += x;
        return tmp;
    }

    inline Date_Time operator-=(int x) { return this->operator+=(-x); }

    inline Date_Time operator-(int x) { return this->operator+(-x); }

    inline int operator-(const Date_Time &dt) const { return (date - dt.date) * 24 * 60 + (time - dt.time); }

    inline bool operator<(const Date_Time &t) const { return (date == t.date) ? (time < t.time) : (date < t.date); }

    inline bool operator>(const Date_Time &t) const { return (date == t.date) ? (time > t.time) : (date > t.date); }

    inline bool operator<=(const Date_Time &t) const { return (date == t.date) ? (time <= t.time) : (date < t.date); }

    inline bool operator>=(const Date_Time &t) const { return (date == t.date) ? (time >= t.time) : (date > t.date); }

    inline bool operator!=(const Date_Time &t) const { return date != t.date || time != t.time; }

    inline bool operator==(const Date_Time &t) const { return date == t.date && time == t.time; }
};

//-------------------------------------------------------------------------------------------

template<class T, size_t N = 2017>
class TransferMap { //special HashMap(multi) for transfer without delete
private:
    int begin[N]{0}, next[N]{0}, originHash[N]{0}, cnt = 0;
    sjtu::vector<T> val[N];
public:
    TransferMap() = default;

    inline void clear() {
        memset(begin, 0, sizeof(begin));
        memset(next, 0, sizeof(next));
        memset(originHash, 0, sizeof(originHash));
        for (int i = 1; i <= cnt; ++i) val[i].clear();
        cnt = 0;
    }

    inline void init(int hash) {
        int tmp = hash % N;
        int now = ++cnt;
        next[now] = begin[tmp];
        begin[tmp] = now;
        originHash[now] = hash;
    }

    bool has(int hash) {
        for (int i = begin[hash % N]; i; i = next[i]) if (originHash[i] == hash) return true;
        return false;
    }

    sjtu::vector<T> *query(int hash) {
        for (int i = begin[hash % N]; i; i = next[i]) if (originHash[i] == hash) return val + i;
        return nullptr;
    }

    void insert(int hash, const T &v) { //include init
        for (int i = begin[hash % N]; i; i = next[i]) {
            if (originHash[i] == hash) {
                val[i].push_back(v);
                return;
            }
        } //hash no found
        init(hash);
        val[cnt].push_back(v);
    }
};


#endif //TICKET_SYSTEM_MY_STRUCT_H

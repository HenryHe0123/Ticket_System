#ifndef TICKET_SYSTEM_MY_STRUCT_H
#define TICKET_SYSTEM_MY_STRUCT_H

#include "../STLite/myString.h"

/*
 * this file implements struct Date and Time
 */

const int daysBeforeMonth[14] = {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

struct Date {
    int month = 6, day = 1; //only for 06-01 ~ 08-31

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
        if (day > 31) { //7.32->8.1
            day -= 31;
            ++month;
        } else if (day == 31 && month == 6) { //6.31->7.1
            day -= 30;
            ++month;
        }
        return *this;
    }

    inline Date operator++(int) {
        Date pre = *this;
        ++day;
        if (day > 31) { //7.32->8.1
            day -= 31;
            ++month;
        } else if (day == 31 && month == 6) { //6.31->7.1
            day -= 30;
            ++month;
        }
        return pre;
    }

    inline Date &operator--() {
        --day;
        if (day == 0) { //8.1->7.31, 7.1->6.30
            --month;
            if (month == 7) day = 31;
            else if (month == 6) day = 30;
            else sjtu::error("Date out of bound");
        }
        return *this;
    }

    inline Date operator--(int) {
        Date pre = *this;
        --day;
        if (day == 0) { //8.1->7.31, 7.1->6.30
            --month;
            if (month == 7) day = 31;
            else if (month == 6) day = 30;
            else sjtu::error("Date out of bound");
        }
        return pre;
    }

    inline int operator-(const Date &d) const {
        return daysBeforeMonth[month] + day - daysBeforeMonth[d.month] - d.day;
    }

    inline void operator+=(int i) {
        if (i < 0) {
            *this -= -i;
            return;
        }
        if (i > 91) sjtu::error("Date out of bound");
        day += i;
        if (day < 31) return;
        //day >= 31
        if (month == 6) { //6.day -> 7.(day-30) -> ??
            ++month;
            day -= 30;
            if (day > 31) {
                ++month;
                day -= 31;
            }
        } else if (month == 7 && day > 31) { //7.day -> 8.(day-31)
            ++month;
            day -= 31;
        } else sjtu::error("Date out of bound");
    }

    inline void operator-=(int i) {
        if (i < 0) {
            *this += -i;
            return;
        }
        if (i > 91) sjtu::error("Date out of bound");
        if (day > i) {
            day -= i;
            return;
        }
        --month;
        day += (month == 6) ? 30 : 31;
        day -= i;
        if (day < 1) { //8.xx -> 6.xx
            --month;
            day += 30;
        }
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
};


#endif //TICKET_SYSTEM_MY_STRUCT_H

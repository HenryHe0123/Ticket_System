#ifndef TICKET_SYSTEM_STRING_H
#define TICKET_SYSTEM_STRING_H

#include <iostream>
#include <cstring>
#include "exceptions.hpp"

namespace my {

    using sjtu::error;

    template<size_t len = 64>
    class string {
    private:
        char str[len + 1] = {0};

    public:
        string() = default;

        string(const char *s) {
            if (strlen(s) > len) error("invalid type conversion from char* to my::string");
            strcpy(str, s);
        }

        template<size_t len2>
        string(const string<len2> &s) {
            if (len2 > len) error("invalid type conversion from my::string to my::string");
            memcpy(str, s.str, sizeof(str));
        }

        string(const std::string &s) {
            if (s.size() > len) error("invalid type conversion from std::string to my::string");
            for (int i = 0; i < s.size(); ++i) str[i] = s[i];
        }

        string<len> &operator=(const string<len> &s) {
            if (this != &s) { memcpy(str, s.str, sizeof(str)); }
            return *this;
        }

        string<len> &operator=(const char *s) {
            if (strlen(s) > len) error("invalid type conversion from char* to my::string");
            memset(str, 0, sizeof(str));
            strcpy(str, s);
            return *this;
        }

        string<len> &operator=(const std::string &s) {
            if (s.size() > len) error("invalid type conversion from std::string to my::string");
            for (int i = 0; i < s.size(); ++i) str[i] = s[i];
        }

        inline char &operator[](const int &i) { return str[i]; }

        inline char operator[](const int &i) const { return str[i]; }

        inline bool operator<(const string<len> &s) const { return strcmp(str, s.str) < 0; }

        inline bool operator>(const string<len> &s) const { return strcmp(str, s.str) > 0; }

        inline bool operator==(const string<len> &s) const { return !strcmp(str, s.str); }

        inline bool operator!=(const string<len> &s) const { return strcmp(str, s.str); }

        inline bool operator<=(const string<len> &s) const { return strcmp(str, s.str) <= 0; }

        inline bool operator>=(const string<len> &s) const { return strcmp(str, s.str) >= 0; }

        friend std::istream &operator>>(std::istream &is, string &tmp) { return is >> tmp.str; }

        friend std::ostream &operator<<(std::ostream &os, const string &tmp) { return os << tmp.str; }

        operator const char *() const { return str; }

        operator std::string() const { return std::string{str}; }

        //prevent ambiguous compare
        inline bool operator==(const char *s) const { return !strcmp(str, s); }

        inline bool operator!=(const char *s) const { return strcmp(str, s); }

        inline bool operator>(const char *s) const { return strcmp(str, s) > 0; }

        inline bool operator<(const char *s) const { return strcmp(str, s) < 0; }

        inline bool operator>=(const char *s) const { return strcmp(str, s) >= 0; }

        inline bool operator<=(const char *s) const { return strcmp(str, s) <= 0; }

        [[nodiscard]] bool empty() const { return !str[0]; }

        void clear() { memset(str, 0, sizeof(str)); }

        inline int size() { return strlen(str); }

    };

}

#endif //TICKET_SYSTEM_STRING_H

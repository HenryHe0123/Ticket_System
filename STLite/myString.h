#ifndef TICKET_SYSTEM_MYSTRING_H
#define TICKET_SYSTEM_MYSTRING_H

#include <cstring>

namespace my {

    template<size_t len = 64>
    class string {
    private:
        char str[len + 1] = {0};

    public:
        string() = default;

        explicit string(const char *s) { strcpy(str, s); }

        template<size_t len2>
        explicit string(const my::string<len2> &s) { strcpy(str, s.str); }

        explicit string(const std::string &s) {
            strcpy(str, s.c_str());
        }

        string<len> &operator=(const string<len> &s) {
            if (this != &s) { strcpy(str, s.str); }
            return *this;
        }

        string<len> &operator=(const char *s) {
            memset(str, 0, sizeof(str));
            strcpy(str, s);
            return *this;
        }

        string<len> &operator=(const std::string &s) {
            memset(str, 0, sizeof(str));
            strcpy(str, s.c_str());
            return *this;
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

        explicit operator const char *() const { return str; }

        explicit operator std::string() const { return std::string{str}; }

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

#endif //TICKET_SYSTEM_MYSTRING_H

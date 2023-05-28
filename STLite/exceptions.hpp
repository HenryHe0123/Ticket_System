#ifndef SJTU_EXCEPTIONS_HPP
#define SJTU_EXCEPTIONS_HPP

#include <cstddef>
#include <cstring>
#include <string>

namespace sjtu {

    class exception {
    protected:
        std::string detail = "";
    public:
        exception() {}

        exception(const exception &e) : detail(e.detail) {}

        exception(const std::string &s) : detail(s) {}

        virtual std::string what() { return detail; }
    };

    class index_out_of_bound : public exception {
        /* __________________________ */
    };

    class runtime_error : public exception {
        /* __________________________ */
    };

    class invalid_iterator : public exception {
        /* __________________________ */
    };

    class container_is_empty : public exception {
        /* __________________________ */
    };

    class bpt_error : public exception {
        /* __________________________ */
    };

    void error(const std::string &message) {
        throw sjtu::exception(message);
    }

}

#endif
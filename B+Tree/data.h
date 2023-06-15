#ifndef TICKET_SYSTEM_DATA_H
#define TICKET_SYSTEM_DATA_H

#include <cstring>
#include <fstream>
#include "../STLite/exceptions.hpp"
#include "cache.h"

using std::fstream;

namespace my {

/*
 * Class: my::File
 * ---------------------
 * This class provides some simple functions for linear data storage.
 *
 */

    template<class value_type>
    class File {
    public:
        explicit File(const std::string &name);

        ~File() {
            file.seekp(0);
            file.write(reinterpret_cast<const char *>(&endAddress), sizeof(long));
            file.close();
        }

        inline long add(const value_type &value); //add a new value at the end of the file and return address

        inline void write(long address, const value_type &value); //must use legal address

        inline void read(long address, value_type &value); //must use legal address

        inline void del(); //delete the last value of the file, do nothing if the file is empty

        inline void clear();

        inline bool empty();

    protected:
        fstream file;
        long endAddress = sizeof(long);
        Cache<value_type, 32> cache;
    };

//----------------------------------------------------------------------------
//implement
//----------------------------------------------------------------------------

    template<class value_type>
    File<value_type>::File(const std::string &name) {
        file.open(name);
        if (file) {
            file.seekg(0);
            file.read(reinterpret_cast<char *>(&endAddress), sizeof(long));
        } else { //create file
            file.open(name, std::ios::out);
            file.write(reinterpret_cast <char *> (&endAddress), sizeof(long));
            file.close();
            file.open(name);
        }
        cache.init(name);
    }


    template<class value_type>
    long File<value_type>::add(const value_type &value) {
        file.seekp(endAddress);
        file.write(reinterpret_cast <const char *> (&value), sizeof(value_type));
        cache.addNew(endAddress, value);
        endAddress += sizeof(value_type);
        return endAddress - sizeof(value_type);
    }

    template<class value_type>
    void File<value_type>::write(long address, const value_type &value) {
        if (cache.has(address)) cache.get(address) = value;
        file.seekp(address);
        file.write(reinterpret_cast <const char *> (&value), sizeof(value_type));
    }

    template<class value_type>
    void File<value_type>::read(long address, value_type &value) {
        if (cache.has(address)) value = cache.get(address);
        else value = cache.getNew(address);
    }

    template<class value_type>
    void File<value_type>::del() {
        if (endAddress != sizeof(long)) endAddress -= sizeof(value_type);
    }

    template<class value_type>
    void File<value_type>::clear() {
        cache.clear();
        endAddress = sizeof(long);
    }

    template<class value_type>
    bool File<value_type>::empty() {
        return endAddress == sizeof(long);
    }

}

#endif //TICKET_SYSTEM_DATA_H

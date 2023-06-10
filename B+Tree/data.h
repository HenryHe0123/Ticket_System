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

        ~File() { file.close(); }

        long add(const value_type &value); //add a new value at the end of the file and return address

        void write(long address, const value_type &value); //must use legal address

        void read(long address, value_type &value); //must use legal address

        void del(); //delete the last value of the file, do nothing if the file is empty

        virtual void clear();

        bool empty();

        long end_address(); //return the endAddress of the file

    protected:
        fstream file;
        //long endAddress = sizeof(long);
        //Cache<value_type, 128> cache;
    };

//----------------------------------------------------------------------------
//implement
//----------------------------------------------------------------------------

    template<class value_type>
    File<value_type>::File(const std::string &name) {
        file.open(name);
        if (!file) { //create file
            file.open(name, std::ios::out);
            long endAddress = sizeof(long);
            file.write(reinterpret_cast <char *> (&endAddress), sizeof(long));
            file.close();
            file.open(name);
        }
    }

    template<class value_type>
    long File<value_type>::end_address() {
        long endAddress = 0;
        file.seekg(0);
        file.read(reinterpret_cast <char *> (&endAddress), sizeof(long));
        return endAddress;
    }

    template<class value_type>
    long File<value_type>::add(const value_type &value) {
        file.seekg(0);
        long endAddress = 0;
        file.read(reinterpret_cast <char *> (&endAddress), sizeof(long));
        file.seekp(endAddress);
        file.write(reinterpret_cast <const char *> (&value), sizeof(value_type));
        endAddress += sizeof(value_type);
        file.seekp(0);
        file.write(reinterpret_cast <char *> (&endAddress), sizeof(long));
        return endAddress - sizeof(value_type);
    }

    template<class value_type>
    void File<value_type>::write(long address, const value_type &value) {
        file.seekp(address);
        file.write(reinterpret_cast <const char *> (&value), sizeof(value_type));
    }

    template<class value_type>
    void File<value_type>::read(long address, value_type &value) {
        file.seekg(address);
        file.read(reinterpret_cast <char *> (&value), sizeof(value_type));
    }

    template<class value_type>
    void File<value_type>::del() {
        file.seekg(0);
        long endAddress = 0;
        file.read(reinterpret_cast <char *> (&endAddress), sizeof(long));
        if (endAddress == sizeof(long)) return;
        endAddress -= sizeof(value_type);
        file.seekp(0);
        file.write(reinterpret_cast <char *> (&endAddress), sizeof(long));
    }

    template<class value_type>
    void File<value_type>::clear() {
        file.seekp(0);
        long endAddress = sizeof(long);
        file.write(reinterpret_cast <char *> (&endAddress), sizeof(long));
    }

    template<class value_type>
    bool File<value_type>::empty() {
        file.seekg(0);
        long endAddress = 0;
        file.read(reinterpret_cast <char *> (&endAddress), sizeof(long));
        return endAddress == sizeof(long);
    }

}

#endif //TICKET_SYSTEM_DATA_H

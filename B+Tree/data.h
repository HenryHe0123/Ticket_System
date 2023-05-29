#ifndef TICKET_SYSTEM_DATA_H
#define TICKET_SYSTEM_DATA_H

#include <cstring>
#include <fstream>
#include "../STLite/exceptions.hpp"

using std::fstream;

/*
 * Class: my::Data
 * ---------------------
 * This class similarly implements the functions of vector.
 * Typical usage of which looks like this:
 *
 *    myData<value_type> data("data");
 *
 *    data.push_back(value);
 *
 *    data.pop_back(); //do nothing if empty
 *
 *    if(data.empty()) {...}
 *
 *    int size = data.size();
 *
 *    tmp_value = data[i];
 *
 *    tmp_value = data.back();
 *
 *    data.assign(i,value);
 *
 *    void func(const value_type &);
 *    map.execute(func,cnt); //execute func for the last cnt value
 *    map.execute(func); //execute func for all value
 *
 */
namespace my {

    template<class value_type>
    class File;

    template<class value_type>
    class Data : public File<value_type> {
    public:
        explicit Data(const std::string &name) : File<value_type>(name) {}

        ~Data() { this->file.close(); }

        void push_back(const value_type &value) { this->add(value); }

        void pop_back() { this->del(); } //do nothing if empty

        inline int size() { return (this->end_address() - sizeof(long)) / sizeof(value_type); }

        value_type operator[](int i);

        value_type back();

        void assign(int i, const value_type &value);

        void execute(void (*func)(const value_type &value), int cnt = -1); //execute all when cnt = -1

    };

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

    template<class value_type>
    value_type Data<value_type>::operator[](int i) {
        if (i >= size() || i < 0) sjtu::error("Data: index out of range");
        long address = sizeof(long) + i * sizeof(value_type);
        this->file.seekg(address);
        value_type value;
        this->file.read(reinterpret_cast <char *> (&value), sizeof(value_type));
        return value;
    }

    template<class value_type>
    value_type Data<value_type>::back() {
        long endAddress = this->end_address();
        if (endAddress == sizeof(long)) sjtu::error("Data: empty data has no back");
        this->file.seekg(endAddress - sizeof(value_type));
        value_type value;
        this->file.read(reinterpret_cast <char *> (&value), sizeof(value_type));
        return value;
    }

    template<class value_type>
    void Data<value_type>::assign(int i, const value_type &value) {
        if (i >= size() || i < 0) sjtu::error("Data: index out of range");
        long address = sizeof(long) + i * sizeof(value_type);
        this->file.seekp(address);
        this->file.write(reinterpret_cast <char *> (&value), sizeof(value_type));
    }

    template<class value_type>
    void Data<value_type>::execute(void (*func)(const value_type &), int cnt) {
        if (!cnt) return;
        if (cnt == -1) cnt = size();
        if (cnt > size() || cnt < 0) sjtu::error("Data: invalid count");
        long address = sizeof(long) + (size() - cnt) * sizeof(value_type);
        this->file.seekg(address);
        value_type value;
        for (int i = 0; i < cnt; ++i) {
            this->file.read(reinterpret_cast <char *> (&value), sizeof(value_type));
            func(value);
        }
    }

}

#endif //TICKET_SYSTEM_DATA_H

#ifndef TICKET_SYSTEM_TRAIN_SYSTEM_H
#define TICKET_SYSTEM_TRAIN_SYSTEM_H

#include "../B+Tree/BPT.h"
#include "../B+Tree/multi_BPT.h"
#include "myStruct.h"
#include <cstring>

class Train {
private:
    static constexpr int N = 100;
public:
    my::string<20> trainID;
    int stationNum = 2; //2 ~ N
    my::string<30> stations[N];
    int seat = 0;
    int prices[N]{0};
    Time startTime;
    int travelTimes[N]{0}, stopoverTimes[N]{0};
    Date beginDate, endDate;
    char type = 0;

    Train() = default;

    explicit Train(const std::string &id) : trainID(id) {}

    explicit Train(const my::string<20> &id) : trainID(id) {}

    explicit Train(const char *id) : trainID(id) {}

    Train(const my::string<20> &i, int n, int m, const my::string<30> *s, const int *p, const Time &x,
          const int *t, const int *o, const Date &d_begin, const Date &d_end, char y) :
            trainID(i), stationNum(n), seat(m), startTime(x), beginDate(d_begin), endDate(d_end), type(y) {
        for (int ii = 0; ii < stationNum; ++ii) stations[ii] = s[ii];
        memcpy(prices, p, sizeof(int) * (stationNum - 1));
        memcpy(travelTimes, t, sizeof(int) * (stationNum - 1));
        memcpy(stopoverTimes, o, sizeof(int) * (stationNum - 2));
    }

    inline int getPrice(int l, int r) {
        int tmp = 0;
        for (int i = l; i <= r; ++i) tmp += prices[i];
        return tmp;
    }
};

class TrainSystem {
    using ustring = my::string<20>;
public:
    TrainSystem() : train_map("train_map") {}

    bool empty() const { return train_map.empty(); }

    int add_train(const Train &train) {
        if (train_map.count(train.trainID)) return -1;
        train_map.assign(train.trainID, train);
        return 0;
    }

    int delete_train(const std::string &i) {
        ustring id(i);
        //todo
        return 0;
    }

private:
    my::BPT<ustring, Train> train_map;

};


#endif //TICKET_SYSTEM_TRAIN_SYSTEM_H

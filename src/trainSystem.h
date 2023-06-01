#ifndef TICKET_SYSTEM_TRAIN_SYSTEM_H
#define TICKET_SYSTEM_TRAIN_SYSTEM_H

#include "../B+Tree/BPT.h"
#include "../B+Tree/multi_BPT.h"
#include "myStruct.h"


class Train {
private:
    static constexpr int N = 100;
public:
    my::string<20> trainID;
    int stationNum; //2 ~ 100
    my::string<30> stations[N];
    int seat;
    int prices[N];
    Time startTime;
    Time travelTimes[N], stopoverTimes[N];
    Date beginDate, endDate;
    char type;
};

class TrainSystem {
    using ustring = my::string<20>;
public:

private:
    my::BPT<ustring, Train> train_map;

};


#endif //TICKET_SYSTEM_TRAIN_SYSTEM_H

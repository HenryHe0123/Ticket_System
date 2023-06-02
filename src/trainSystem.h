#ifndef TICKET_SYSTEM_TRAIN_SYSTEM_H
#define TICKET_SYSTEM_TRAIN_SYSTEM_H

#include "../B+Tree/BPT.h"
#include "../B+Tree/multi_BPT.h"
#include "myStruct.h"
#include <cstring>

constexpr int N = 100;

class Train {
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
    TrainSystem() : train_map("train_map"), released_trains("released_trains"), seats_map("seats_map") {}

    void clean() {
        train_map.clear();
        released_trains.clear();
        seats_map.clear();
    }

    int add_train(const Train &train) {
        if (train_map.count(train.trainID) || released_trains.count(train.trainID)) return -1;
        train_map.assign(train.trainID, train);
        return 0;
    }

    int delete_train(const std::string &i) {
        if (train_map.erase(ustring(i))) return 0;
        return -1;
    }

    int release_train(const std::string &i) {
        ustring id(i);
        if (!train_map.count(id) || released_trains.count(id)) return -1;
        Train train = train_map[id];
        train_map.erase(id);
        released_trains.assign(id, train);
        Seat seat(train);
        Seat_Index index{id, train.beginDate};
        seats_map.assign(index, seat); //add seat to seats_map only after released
        return 0;
    }

    void query_train(const std::string &i, const Date &d) {
        ustring id(i);
        Train train;
        if (released_trains.find(id, train)) { //released train find
            if (train.beginDate != d) {
                std::cout << "-1\n";
                return;
            }
            Seat_Index index{id, d};
            output_query_train(train, seats_map[index]);
        } else {
            if (!train_map.find(id, train)) { //no find
                std::cout << "-1\n";
                return;
            }
            if (train.beginDate != d) {
                std::cout << "-1\n";
                return;
            }
            Seat seat(train);
            output_query_train(train, seat);
        }
    }

private:
    my::BPT<ustring, Train> train_map; //when train released, remove it to released_train
    my::BPT<ustring, Train> released_trains;

    struct Seat_Index {
        ustring id;
        Date date;

        inline bool operator<(const Seat_Index &index) const { return id < index.id; }

        inline bool operator>(const Seat_Index &index) const { return id > index.id; }

        inline bool operator>=(const Seat_Index &index) const { return id >= index.id; }

        inline bool operator<=(const Seat_Index &index) const { return id <= index.id; }

        inline bool operator!=(const Seat_Index &index) const { return id != index.id || date != index.date; }

        inline bool operator==(const Seat_Index &index) const { return id == index.id && date == index.date; }
    };

    struct Seat {
        int remain[N]{0};

        Seat() = default;

        explicit Seat(const Train &train) {
            for (int i = 0; i < train.stationNum; ++i) remain[i] = train.seat;
        }

        inline void modify(int l, int r, int add) {
            for (int i = l; i <= r; ++i) remain[i] += add;
        }

        inline int min(int l, int r) {
            int tmp = 1e5 + 1;
            for (int i = l; i <= r; ++i) tmp = std::min(tmp, remain[i]);
            return tmp;
        }
    };

    my::BPT<Seat_Index, Seat> seats_map; //only for released trains

    static inline void output_query_train(const Train &train, const Seat &seat);

};

void TrainSystem::output_query_train(const Train &train, const TrainSystem::Seat &seat) {
    std::cout << train.trainID << ' ' << train.type << '\n';
    Date_Time t = {train.beginDate, train.startTime};
    std::cout << train.stations[0] << " xx-xx xx:xx -> " << t << ' ' << train.prices[0] << ' ' << seat.remain[0]
              << '\n';
    t += train.travelTimes[0];
    for (int j = 1; j < train.stationNum - 1; ++j) {
        std::cout << train.stations[j] << ' ' << t << " -> ";
        t += train.stopoverTimes[j - 1];
        std::cout << t << ' ' << train.prices[j] << ' ' << seat.remain[j] << '\n';
        t += train.travelTimes[j];
    }
    //j = stationNum - 1
    std::cout << train.stations[train.stationNum - 1] << ' ' << t << " -> xx-xx xx:xx "
              << train.prices[train.stationNum - 1] << " x\n";
}


#endif //TICKET_SYSTEM_TRAIN_SYSTEM_H

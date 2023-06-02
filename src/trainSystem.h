#ifndef TICKET_SYSTEM_TRAIN_SYSTEM_H
#define TICKET_SYSTEM_TRAIN_SYSTEM_H

#include "../B+Tree/BPT.h"
#include "../B+Tree/multi_BPT.h"
#include "../STLite/algorithm.h"
#include "myStruct.h"
#include <cstring>

constexpr int N = 100;

using sjtu::vector;

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
    using sstring = my::string<30>;
public:
    TrainSystem() : train_map("train_map"), released_trains("released_trains"),
                    seats_map("seats_map"), stop_multimap("stop_multimap") {}

    void clean() {
        train_map.clear();
        released_trains.clear();
        seats_map.clear();
        stop_multimap.clear();
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
        //add Stop information
        Date_Time t = {train.beginDate, train.startTime};
        Stop stop(i);
        stop.arrive = stop.leave = t;
        stop_multimap.insert(train.stations[0], stop);
        t += train.travelTimes[0];
        for (int j = 1; j < train.stationNum; ++j) {
            stop.index = j;
            stop.arrive = t;
            if (j != train.stationNum - 1) t += train.stopoverTimes[j - 1];
            stop.leave = t;
            stop_multimap.insert(train.stations[j], stop);
            t += train.travelTimes[j];
        }
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

    void query_ticket(const std::string &s, const std::string &t, const Date &date, bool sortInTime) {
        if (s == t) sjtu::error("query_ticket chaos: from same to same");
        sstring from(s), to(t);
        vector<Stop> stop1, stop2;
        stop_multimap.find(from, stop1);
        stop_multimap.find(to, stop2);
        if (stop1.empty() || stop2.empty()) {
            std::cout << "0\n";
            return;
        }
        vector<Ticket> tickets;
        auto it2 = stop2.begin();
        for (const auto &tmp1: stop1) {
            while (it2 != stop2.end() && *it2 < tmp1) ++it2;
            if (it2 == stop2.end()) break;
            Stop tmp2 = *it2;
            if (tmp1 != tmp2) continue;
            if (tmp2.index < tmp1.index) continue;
            if (date < tmp1.arrive.date || tmp1.leave.date < date) continue;
            //available train
            Train train = released_trains[tmp1.id];
            int price = train.getPrice(tmp1.index, tmp2.index - 1);
            Seat seat = seats_map[Seat_Index{train.trainID, train.beginDate}];
            int seatNum = seat.min(tmp1.index, tmp2.index - 1);
            Ticket ticket(tmp1.id, tmp1.leave, tmp2.arrive, price, seatNum);
            tickets.push_back(ticket);
        }
        sort(tickets, 0, tickets.size() - 1, sortInTime ? cmp_time : cmp_cost);
        std::cout << tickets.size() << '\n';
        for (const auto &ticket: tickets) {
            std::cout << ticket.id << ' ' << s << ' ' << ticket.start << " -> " << t << ' '
                      << ticket.end << ' ' << ticket.price << ' ' << ticket.seat << '\n';
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

    my::BPT<Seat_Index, Seat> seats_map; //only for train released

    struct Stop {
        ustring id;
        int index = 0; //station = train_map[id].stations[index]
        Date_Time arrive, leave;

        Stop() = default;

        explicit Stop(const std::string &s) : id(s) {}

        explicit Stop(const ustring &id) : id(id) {}

        inline bool operator<(const Stop &stop) const { return id < stop.id; }

        inline bool operator>(const Stop &stop) const { return id > stop.id; }

        inline bool operator>=(const Stop &stop) const { return id >= stop.id; }

        inline bool operator<=(const Stop &stop) const { return id <= stop.id; }

        inline bool operator!=(const Stop &stop) const { return id != stop.id; }

        inline bool operator==(const Stop &stop) const { return id == stop.id; }

        friend std::ostream &operator<<(std::ostream &os, const Stop &stop) {
            os << stop.arrive << " -> " << stop.leave;
            return os;
        }
    };

    my::multiBPT<sstring, Stop> stop_multimap; //store all stopping information for train released

    struct Ticket {
        ustring id;
        int price = 0, seat = 0;
        Date_Time start, end;

        Ticket() = default;

        Ticket(const ustring &id, const Date_Time &st, const Date_Time &ed, int p, int seat) :
                id(id), price(p), seat(seat), start(st), end(ed) {}
    };

    static inline bool cmp_cost(const Ticket &a, const Ticket &b) {
        if (a.price == b.price) return a.id <= b.id;
        return a.price < b.price;
    }

    static inline bool cmp_time(const Ticket &a, const Ticket &b) {
        int ta = a.end - a.start, tb = b.end - b.start;
        if (ta == tb) return a.id <= b.id;
        return ta < tb;
    }

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

#ifndef TICKET_SYSTEM_USER_SYSTEM_H
#define TICKET_SYSTEM_USER_SYSTEM_H

#include "../B+Tree/BPT.h"
#include "../STLite/map.hpp"
#include "../STLite/myString.h"

struct User {
    my::string<20> username;
    my::string<30> password;
    my::string<15> name;
    my::string<30> mail;
    int privilege = 10; //0 ~ 10

    User() = default;

    User(const std::string &user_name) : username(user_name) {}

    User(const std::string &u, const std::string &p, const std::string &n, const std::string &m, int g = 10) :
            username(u), password(p), name(n), mail(m), privilege(g) {}

    User(const User &u) = default;

    bool operator==(const User &u) const { return username == u.username; }

    bool operator!=(const User &u) const { return username != u.username; }

    friend std::ostream &operator<<(std::ostream &os, const User &user) {
        return os << user.username << ' ' << user.name << ' ' << user.mail << ' ' << user.privilege;
    }
};

class UserSystem {
    using ustring = my::string<20>;
public:
    UserSystem() : user_map("user_map") {}

    bool empty() const { return user_map.empty(); }

    int add_user(const std::string &c, const User &newUser) {
        ustring cur(c);
        if (!user_login.count(cur) || user_map.count(newUser.username)) return -1;
        if (newUser.privilege >= user_login[cur]) return -1;
        user_map.assign(newUser.username, newUser);
        return 0;
    }

    int add_first_user(const User &user) {
        if (user.privilege != 10) sjtu::error("add first user failed for privilege");
        user_map.assign(user.username, user);
        return 0;
    }

    int login(const std::string &u, const std::string &p) {
        ustring username(u);
        User user;
        if (!user_map.find(username, user)) return -1; //user not exist
        if (user_login.count(username) || user.password != p) return -1;
        user_login[username] = user.privilege;
        return 0;
    }

    int logout(const std::string &u) {
        ustring username(u);
        return user_login.erase(username) ? 0 : -1;
    }

    void query_profile(const std::string &c, const std::string &u) {
        ustring cur(c), username(u);
        User user;
        if (!user_login.count(cur) || !user_map.find(username, user)) {
            std::cout << "-1\n";
            return;
        } //c not login yet or u not exist
        if (cur != username && user_login[cur] <= user.privilege) {
            std::cout << "-1\n";
            return;
        }
        std::cout << user << '\n';
    }

    void modify_profile(const std::string &c, const std::string &u,
                        const char *p, const char *n, const char *m, const int *g) {
        ustring cur(c), username(u);
        User user;
        if (!user_login.count(cur) || !user_map.find(username, user)) {
            std::cout << "-1\n";
            return;
        } //c not login yet or u not exist
        int curp = user_login[cur];
        if (curp <= user.privilege && cur != username) { //only allow c = u when cur.g = user.g
            std::cout << "-1\n";
            return;
        }
        if (g) {
            if (curp <= *g) {
                std::cout << "-1\n";
                return;
            } else {
                user.privilege = *g;
                if (user_login.count(username)) user_login[username] = *g; //update the privilege
            }
        }
        if (p) user.password = p;
        if (n) user.name = n;
        if (m) user.mail = m;
        user_map.assign(username, user);
        std::cout << user << '\n';
    }

    void clean() {
        user_login.clear();
        user_map.clear();
    }

private:
    my::BPT<ustring, User> user_map;
    sjtu::map<ustring, int> user_login; //store privilege in int
};


#endif //TICKET_SYSTEM_USER_SYSTEM_H

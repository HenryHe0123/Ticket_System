#include "src/userSystem.h"
#include "src/simpleScanner.h"
#include "src/trainSystem.h"

using std::cout;

UserSystem userSystem;
TrainSystem trainSystem;
bool quit = false;

void processLine(const std::string &line);

int main() {
    //freopen("../testcases/basic_1/1.in","r",stdin);
    //freopen("test.out","w",stdout);

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    std::string input;
    while (!quit) {
        if (std::cin.eof()) break;
        try {
            getline(std::cin, input);
            if (input.empty()) continue;
            processLine(input);
        } catch (sjtu::exception &ex) {
            cout << "Error: " << ex.what() << std::endl;
            return 19260817;
        }
    }
    return 0;
}

//---------------------------------------------------------------------------

void processLine(const std::string &line) {
    SimpleScanner scanner(line);
    std::string token = scanner.nextToken(); //[time]
    cout << token << ' ';
    int timestamp = stoi(token.substr(1, token.size() - 2));
    token = scanner.nextToken(); //cmd
    if (token == "exit") { //R
        cout << "bye\n";
        quit = true;
    } else if (token == "clean") { //R
        userSystem.clean();
        trainSystem.clean();
        cout << "0\n";
    } else if (token == "add_user") { //N
        std::string c;
        User user;
        while (scanner.hasMoreTokens()) {
            switch (scanner.getKey()) {
                case 'c':
                    c = scanner.nextToken();
                    break;
                case 'u':
                    user.username = scanner.nextToken();
                    break;
                case 'p':
                    user.password = scanner.nextToken();
                    break;
                case 'n':
                    user.name = scanner.nextToken();
                    break;
                case 'm':
                    user.mail = scanner.nextToken();
                    break;
                case 'g':
                    user.privilege = stoi(scanner.nextToken());
                    break;
                default:
                    sjtu::error("add_user failed");
            }
        }
        if (userSystem.empty())
            cout << userSystem.add_first_user(user) << '\n';
        else
            cout << userSystem.add_user(c, user) << '\n';
    } else if (token == "login") { //F
        std::string u, p;
        while (scanner.hasMoreTokens()) {
            switch (scanner.getKey()) {
                case 'u':
                    u = scanner.nextToken();
                    break;
                case 'p':
                    p = scanner.nextToken();
                    break;
                default:
                    sjtu::error("login failed");
            }
        }
        cout << userSystem.login(u, p) << '\n';
    } else if (token == "logout") { //F
        if (scanner.getKey() != 'u') sjtu::error("logout failed");
        std::string u = scanner.nextToken();
        if (scanner.hasMoreTokens()) sjtu::error("logout failed");
        cout << userSystem.logout(u) << '\n';
    } else if (token == "query_profile") { //SF
        std::string c, u;
        while (scanner.hasMoreTokens()) {
            switch (scanner.getKey()) {
                case 'u':
                    u = scanner.nextToken();
                    break;
                case 'c':
                    c = scanner.nextToken();
                    break;
                default:
                    sjtu::error("query_profile failed");
            }
        }
        userSystem.query_profile(c, u);
    } else if (token == "modify_profile") { //F
        std::string c, u, pp, nn, mm;
        const char *p = nullptr, *n = nullptr, *m = nullptr; //*p must not point to temporary value
        int gg;
        int *g = nullptr;
        while (scanner.hasMoreTokens()) {
            switch (scanner.getKey()) {
                case 'c':
                    c = scanner.nextToken();
                    break;
                case 'u':
                    u = scanner.nextToken();
                    break;
                case 'p':
                    pp = scanner.nextToken();
                    p = pp.c_str();
                    break;
                case 'n':
                    nn = scanner.nextToken();
                    n = nn.c_str();
                    break;
                case 'm':
                    mm = scanner.nextToken();
                    m = mm.c_str();
                    break;
                case 'g':
                    gg = stoi(scanner.nextToken());
                    g = &gg;
                    break;
                default:
                    sjtu::error("modify_profile failed");
            }
        }
        userSystem.modify_profile(c, u, p, n, m, g);
    } else if (token == "add_train") { //N
        std::string ii, ss, pp, tt, oo, dd, xx;
        char y;
        int n, m;
        while (scanner.hasMoreTokens()) {
            switch (scanner.getKey()) {
                case 'i':
                    ii = scanner.nextToken();
                    break;
                case 'n':
                    n = stoi(scanner.nextToken());
                    break;
                case 'm':
                    m = stoi(scanner.nextToken());
                    break;
                case 's':
                    ss = scanner.nextToken();
                    break;
                case 'p':
                    pp = scanner.nextToken();
                    break;
                case 'x':
                    xx = scanner.nextToken();
                    break;
                case 't':
                    tt = scanner.nextToken();
                    break;
                case 'o':
                    oo = scanner.nextToken();
                    break;
                case 'd':
                    dd = scanner.nextToken();
                    break;
                case 'y':
                    y = scanner.nextToken()[0];
                    break;
                default:
                    sjtu::error("modify_profile failed");
            }
        }
        my::string<20> i(ii);
        Time x(xx);
        Slicer slicer(dd);
        Date d1(slicer[0]), d2(slicer[1]);
        int p[N]{0}, t[N]{0}, o[N]{0};
        slicer.reset(pp);
        for (int j = 0; j < slicer.size(); ++j) p[j] = stoi(slicer[j]);
        slicer.reset(tt);
        for (int j = 0; j < slicer.size(); ++j) t[j] = stoi(slicer[j]);
        slicer.reset(oo);
        for (int j = 0; j < slicer.size(); ++j) o[j] = stoi(slicer[j]);
        my::string<30> s[N];
        slicer.reset(ss);
        for (int j = 0; j < slicer.size(); ++j) s[j] = slicer[j];
        Train train(i, n, m, s, p, x, t, o, d1, d2, y);
        cout << trainSystem.add_train(train) << '\n';
    } else if (token == "delete_train") { //N
        if (scanner.getKey() != 'i') sjtu::error("delete_train failed");
        auto i = scanner.nextToken();
        cout << trainSystem.delete_train(i) << '\n';
    } else if (token == "release_train") { //N
        if (scanner.getKey() != 'i') sjtu::error("release_train failed");
        auto i = scanner.nextToken();
        cout << trainSystem.release_train(i) << '\n';
    } else if (token == "query_train") { //N
        std::string i, d;
        while (scanner.hasMoreTokens()) {
            switch (scanner.getKey()) {
                case 'i':
                    i = scanner.nextToken();
                    break;
                case 'd':
                    d = scanner.nextToken();
                    break;
                default:
                    sjtu::error("query_train failed");
            }
        }
        Date date(d);
        trainSystem.query_train(i, d);
    } else if (token == "query_ticket") { //SF
        std::string s, t, d, p;
        while (scanner.hasMoreTokens()) {
            switch (scanner.getKey()) {
                case 's':
                    s = scanner.nextToken();
                    break;
                case 't':
                    t = scanner.nextToken();
                    break;
                case 'd':
                    d = scanner.nextToken();
                    break;
                case 'p':
                    p = scanner.nextToken();
                    break;
                default:
                    sjtu::error("query_ticket failed");
            }
        }
        Date date(d);
        trainSystem.query_ticket(s, t, date, p == "time");
    } else if (token == "query_transfer") { //N
        cout << "skip\n";
    } else if (token == "buy_ticket") { //SF
        std::string u, i, d, f, t;
        bool q = false; //pending, initially false
        int n;
        while (scanner.hasMoreTokens()) {
            switch (scanner.getKey()) {
                case 'u':
                    u = scanner.nextToken();
                    break;
                case 'i':
                    i = scanner.nextToken();
                    break;
                case 'd':
                    d = scanner.nextToken();
                    break;
                case 'n':
                    n = stoi(scanner.nextToken());
                    break;
                case 'f':
                    f = scanner.nextToken();
                    break;
                case 't':
                    t = scanner.nextToken();
                    break;
                case 'q':
                    q = (scanner.nextToken() == "true");
                    break;
                default:
                    sjtu::error("buy_ticket failed");
            }
        }
        Date date(d);
        if (userSystem.logged_in(u))
            trainSystem.buy_ticket(timestamp, u, i, date, n, f, t, q);
        else std::cout << "-1\n";
    } else if (token == "query_order") { //F

    } else if (token == "refund_ticket") { //N

    } else sjtu::error("invalid command");
}

#include "src/userSystem.h"
#include "src/simpleScanner.h"

using std::cout;

UserSystem userSystem;
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

void processLine(const std::string &line) {
    SimpleScanner scanner(line);
    std::string token = scanner.nextToken(); //timestamp
    cout << token << ' ';
    token = scanner.nextToken(); //cmd
    if (token == "exit") { //R
        cout << "bye\n";
        quit = true;
    } else if (token == "clean") { //R
        userSystem.clean();
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
        std::string c, u;
        const char *p = nullptr, *n = nullptr, *m = nullptr;
        int tmp;
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
                    p = scanner.nextToken().c_str();
                    break;
                case 'n':
                    n = scanner.nextToken().c_str();
                    break;
                case 'm':
                    m = scanner.nextToken().c_str();
                    break;
                case 'g':
                    tmp = stoi(scanner.nextToken());
                    g = &tmp;
                    break;
                default:
                    sjtu::error("modify_profile failed");
            }
        }
        userSystem.modify_profile(c,u,p,n,m,g);
    } else if (token == "add_train") { //N

    } else if (token == "delete_train") { //N

    } else if (token == "release_train") { //N

    } else if (token == "query_train") { //N

    } else if (token == "query_ticket") { //SF

    } else if (token == "query_transfer") { //N

    } else if (token == "buy_ticket") { //SF

    } else if (token == "query_order") { //F

    } else if (token == "refund_ticket") { //N

    } else sjtu::error("invalid command");
}

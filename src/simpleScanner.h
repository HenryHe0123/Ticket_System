#ifndef TICKET_SYSTEM_SIMPLESCANNER_H
#define TICKET_SYSTEM_SIMPLESCANNER_H

#include <iostream>
#include "../STLite/vector.hpp"
#include "../STLite/exceptions.hpp"

/*
 * Class: SimpleScanner
 * -------------------
 * This class divides a string into individual tokens.
 * Typical usage of which looks like this:
 *
 *    SimpleScanner scanner(input);
 *    while (scanner.hasMoreTokens()) {
 *       std::string token = scanner.nextToken();
 *       ... process the token ...
 *    }
 *
 *    scanner.reset(input);
 *
 *    if(SimpleScanner::isInt(token)) {...}
 *
 *    if(SimpleScanner::isFloat(token)) {...}
 *
 */

class SimpleScanner {
public:
    SimpleScanner() = default;

    SimpleScanner(std::string input);

    [[nodiscard]] bool hasMoreTokens() const;

    std::string nextToken();

    void reset(const std::string &input);

    static bool isInt(const std::string &token);

    static bool isFloat(const std::string &token);

    static bool availableID(const std::string &token);

    static bool checkQuotesMark(const char* str); //return true if " appeared

    sjtu::vector<std::string> tokens;

private:
    std::string src_str;

    int index = 0; //next token's position
};

//----------------------------------------------------------------------------
//implement
//----------------------------------------------------------------------------

SimpleScanner::SimpleScanner(std::string input):src_str(std::move(input)) {
    //slice src_str and stored it in tokens
    std::string tmp;
    for(char ch : src_str) {
        if (ch == ' ') {
            if (!tmp.empty()) {
                tokens.push_back(tmp);
                tmp.clear();
            }
        } else {
            tmp += ch;
        }
    }
    if(!tmp.empty()) tokens.push_back(tmp);
}

bool SimpleScanner::hasMoreTokens() const {
    return index < tokens.size();
}

std::string SimpleScanner::nextToken() {
    if (!hasMoreTokens()) sjtu::error("No nextToken!");
    return tokens[index++];
}

void SimpleScanner::reset(const std::string &input) {
    index = 0;
    src_str = input;
    tokens.clear();
    std::string tmp;
    for(char ch : src_str) {
        if (ch == ' ') {
            if (!tmp.empty()) {
                tokens.push_back(tmp);
                tmp.clear();
            }
        } else {
            tmp += ch;
        }
    }
    if(!tmp.empty()) tokens.push_back(tmp);
}

bool SimpleScanner::isInt(const std::string &token) {
    if (token.size() > 10) return false;
    for (char ch: token)
        if (ch > '9' || ch < '0') return false;
    return true;
}

bool SimpleScanner::isFloat(const std::string &token) {
    if (token.size() > 13) return false;
    bool point = false;
    for (int i = 0; i < token.size(); ++i) {
        if (token[i] == '.') {
            if (!point && i && i != token.size() - 1) point = true;
            else return false;
        } else if (token[i] < '0' || token[i] > '9') return false;
    }
    return true;
}

bool SimpleScanner::availableID(const std::string &token) {
    if (token.size() > 30) return false;
    for (char ch: token) {
        if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') ||
            (ch >= 'a' && ch <= 'z') || ch == '_')
            continue;
        else return false;
    }
    return true;
}

bool SimpleScanner::checkQuotesMark(const char *str) {
    for (int i = 0; str[i]; ++i) {
        if (str[i] == 34) return true;
    }
    return false;
}


#endif //TICKET_SYSTEM_SIMPLESCANNER_H

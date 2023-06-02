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

    explicit SimpleScanner(std::string input);

    [[nodiscard]] bool hasMoreTokens() const;

    std::string nextToken();

    void reset(const std::string &input);

    char getKey(); //check "-x" token and return x

    [[maybe_unused]] static bool isInt(const std::string &token);

    sjtu::vector<std::string> tokens;

private:
    std::string src_str;

    int index = 0; //next token's position
};

//implement

SimpleScanner::SimpleScanner(std::string input) : src_str(std::move(input)) {
    //slice src_str and stored it in tokens
    std::string tmp;
    for (char ch: src_str) {
        if (ch == ' ') {
            if (!tmp.empty()) {
                tokens.push_back(tmp);
                tmp.clear();
            }
        } else {
            tmp += ch;
        }
    }
    if (!tmp.empty()) tokens.push_back(tmp);
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
    for (char ch: src_str) {
        if (ch == ' ') {
            if (!tmp.empty()) {
                tokens.push_back(tmp);
                tmp.clear();
            }
        } else {
            tmp += ch;
        }
    }
    if (!tmp.empty()) tokens.push_back(tmp);
}

[[maybe_unused]] bool SimpleScanner::isInt(const std::string &token) {
    if (token.size() > 10) return false;
    for (char ch: token)
        if (ch > '9' || ch < '0') return false;
    return true;
}

char SimpleScanner::getKey() {
    if (!hasMoreTokens()) sjtu::error("invalid use of getKey");
    auto &token = tokens[index++];
    if (token[0] != '-') sjtu::error("invalid use of getKey");
    return token[1];
}

/*
 * Class: Slicer
 * -------------------
 * This class divides a string into individual tokens by sign char '|'.
 *
 */

class Slicer {
public:
    Slicer() = default;

    explicit Slicer(std::string input);

    void reset(const std::string &input);

    [[nodiscard]] inline size_t size() const { return tokens.size(); }

    inline std::string operator[](int i) const { return tokens[i]; }

private:
    static constexpr char sign = '|';

    std::string src;

    sjtu::vector<std::string> tokens;
};

//implement

Slicer::Slicer(std::string input) : src(std::move(input)) {
    std::string tmp;
    for (char ch: src) {
        if (ch == sign) {
            if (!tmp.empty()) {
                tokens.push_back(tmp);
                tmp.clear();
            }
        } else {
            tmp += ch;
        }
    }
    if (!tmp.empty()) tokens.push_back(tmp);
}

void Slicer::reset(const std::string &input) {
    src = input;
    tokens.clear();
    std::string tmp;
    for (char ch: src) {
        if (ch == sign) {
            if (!tmp.empty()) {
                tokens.push_back(tmp);
                tmp.clear();
            }
        } else {
            tmp += ch;
        }
    }
    if (!tmp.empty()) tokens.push_back(tmp);
}

#endif //TICKET_SYSTEM_SIMPLESCANNER_H

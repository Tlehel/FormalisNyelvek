#ifndef NFA_H
#define NFA_H
#include <map>
#include <set>
#include <string>
#include <utility>

#include "DFA.h"

class NFA {

private:
    std::set<std::string> states;
    std::set<char> alphabet;
    std::string startState;
    std::set<std::string> acceptStates;
    std::map<std::pair<std::string,char>, std::set<std::string>> transitions;

public:
    NFA() = default;

    // false eseten az error tartalmazza a hiba okat
    bool loadFromFile(const std::string& filename, std::string& error);

    // reszhalmaz-konstrukcio
    DFA determinize() const;
};

#endif //NFA_H
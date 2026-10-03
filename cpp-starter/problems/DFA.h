//
// Created by Lehel on 2026-10-03.
//

#ifndef DFA_H
#define DFA_H
#include <map>
#include <string>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <utility>

class DFA {

private:
    std::string startState;
    std::unordered_set<std::string> acceptStates;
    std::map<std::pair<std::string,char>,std::string> transitions; //a kulcs kezdo allapot+ atmenet , a tarolt ertek a fogado

public:

    DFA() = default;

    bool loadFromFile(const std::string& filename);

    bool accepts(const std::string& word) const;
};



#endif //DFA_H

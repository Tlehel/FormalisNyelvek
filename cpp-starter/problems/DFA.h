//
// Created by Lehel on 2026-10-03.
//

#ifndef DFA_H
#define DFA_H
#include <map>
#include <string>
#include <unordered_set>
#include <fstream>
#include <set>
#include <sstream>
#include <utility>
#include <vector>

class DFA {

private:
    std::vector<std::string> states;   // letrehozasi sorrendben (s0, s1, ...)
    std::set<char> alphabet;           // rendezett abece
    std::string startState;
    std::unordered_set<std::string> acceptStates;
    std::map<std::pair<std::string,char>,std::string> transitions; //a kulcs kezdo allapot+ atmenet , a tarolt ertek a fogado

public:

    DFA() = default;

    bool loadFromFile(const std::string& filename);

    bool accepts(const std::string& word) const;


    void addState(const std::string& s);
    void addSymbol(char c);
    void setStartState(const std::string& s);
    void addAcceptState(const std::string& s);
    void addTransition(const std::string& from, char symbol, const std::string& to);

    bool saveToFile(const std::string& filename) const;
};



#endif //DFA_H

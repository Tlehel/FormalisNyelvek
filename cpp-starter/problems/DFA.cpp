//
// Created by Lehel on 2026-10-03.
//

#include "DFA.h"

bool DFA::loadFromFile(const std::string& filename) {

    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string line;

    std::getline(file, line); // nem hasznalom fel mert az atmenetek megadnak mindent ami kell - c

    std::getline(file, line); //ennel is ugyan az

    //kezdoallapot
    if (std::getline(file, line)) {
        std::stringstream ss(line);
        ss >> startState;
    }

    //vegallapotok
    if (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string state;
        while (ss >> state) {
            acceptStates.insert(state);
        }
    }

    // atmenetek feldolgozasa
    std::string fromState, toState;
    char symbol;
    while (file >> fromState >> symbol >> toState) {
        transitions[{fromState, symbol}] = toState;
    }

    return true;//lefutot minden helyesen
}

bool DFA::accepts(const std::string& word) const {

    std::string currentState = startState;

    for (char symbol : word) {

        auto it = transitions.find({currentState, symbol}); //iterator megkeresi hogy a jelenlegi allapotbol tudunk e tovabbmenni a szo eppen figyelt karakterevel

        if (it == transitions.end()) {
            return false;//ha a lista veget kapom vissza, azaz utolso utani karakter, helytelen a szp
        }

        currentState = it->second;
    }

    return acceptStates.count(currentState) > 0; //megszamolja hanyszor van benne a current state a vegallapotok listajaban
}
#include "DFA.h"

bool DFA::loadFromFile(const std::string& filename) {

    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    states.clear();
    alphabet.clear();
    acceptStates.clear();
    transitions.clear();

    std::string line;

    // allapotok
    if (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string s;
        while (ss >> s) states.push_back(s);
    }

    // abece
    if (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string s;
        while (ss >> s) alphabet.insert(s[0]);
    }

    // kezdoallapot
    if (std::getline(file, line)) {
        std::stringstream ss(line);
        ss >> startState;
    }

    // vegallapotok
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

    return true;
}

bool DFA::accepts(const std::string& word) const {

    std::string currentState = startState;

    for (char symbol : word) {
        auto it = transitions.find({currentState, symbol});
        if (it == transitions.end()) {
            return false;
        }
        currentState = it->second;
    }

    return acceptStates.count(currentState) > 0;
}

void DFA::addState(const std::string& s) { states.push_back(s); }
void DFA::addSymbol(char c) { alphabet.insert(c); }
void DFA::setStartState(const std::string& s) { startState = s; }
void DFA::addAcceptState(const std::string& s) { acceptStates.insert(s); }

void DFA::addTransition(const std::string& from, char symbol, const std::string& to) {
    transitions[{from, symbol}] = to;
}

bool DFA::saveToFile(const std::string& filename) const {

    std::ofstream out(filename);
    if (!out) {
        return false;
    }

    // allapotok
    for (size_t i = 0; i < states.size(); ++i) {
        out << (i ? " " : "") << states[i];
    }
    out << '\n';

    // abece
    bool first = true;
    for (char c : alphabet) {
        out << (first ? "" : " ") << c;
        first = false;
    }
    out << '\n';

    // kezdoallapot
    out << startState << '\n';

    // vegallapotok (allapotsorrendben)
    first = true;
    for (const std::string& s : states) {
        if (acceptStates.count(s)) {
            out << (first ? "" : " ") << s;
            first = false;
        }
    }
    out << '\n';

    // atmenetek: allapot szerint, azon belul jel szerint rendezve
    // (DFA-nal egy (allapot, jel) parhoz max. egy cel van)
    for (const std::string& s : states) {
        for (char c : alphabet) {
            auto it = transitions.find({s, c});
            if (it != transitions.end()) {
                out << s << ' ' << c << ' ' << it->second << '\n';
            }
        }
    }

    return true;
}
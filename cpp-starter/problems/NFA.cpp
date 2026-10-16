#include "NFA.h"

#include <fstream>
#include <queue>
#include <sstream>

bool NFA::loadFromFile(const std::string& filename, std::string& error) {

    std::ifstream file(filename);
    if (!file.is_open()) {
        error = "A bemeneti fajl nem nyithato meg: " + filename;
        return false;
    }

    states.clear();
    alphabet.clear();
    acceptStates.clear();
    transitions.clear();
    startState.clear();

    std::string line;

    // 1. sor: allapotok
    if (!std::getline(file, line)) {
        error = "Hianyzik az allapotok sora";
        return false;
    }
    {
        std::stringstream ss(line);
        std::string s;
        while (ss >> s) states.insert(s);
    }

    // 2. sor: abece
    if (!std::getline(file, line)) {
        error = "Hianyzik az abece sora";
        return false;
    }
    {
        std::stringstream ss(line);
        std::string s;
        while (ss >> s) {
            if (s.size() != 1) {
                error = "Ervenytelen jel az abecében (egy karakter kell): " + s;
                return false;
            }
            alphabet.insert(s[0]);
        }
    }

    // 3. sor: kezdoallapot
    if (!std::getline(file, line)) {
        error = "Hianyzik a kezdoallapot sora";
        return false;
    }
    {
        std::stringstream ss(line);
        ss >> startState;
        if (!states.count(startState)) {
            error = "A kezdoallapot nincs az allapotok kozott: " + startState;
            return false;
        }
    }

    // 4. sor: vegallapotok (lehet ures)
    std::getline(file, line);
    {
        std::stringstream ss(line);
        std::string s;
        while (ss >> s) {
            if (!states.count(s)) {
                error = "A vegallapot nincs az allapotok kozott: " + s;
                return false;
            }
            acceptStates.insert(s);
        }
    }

    // atmenetek: forras jel cel
    std::string from, sym, to;
    while (file >> from) {
        if (!(file >> sym >> to)) {
            error = "Hianyos atmenet a(z) " + from + " allapotnal";
            return false;
        }
        if (!states.count(from)) {
            error = "Ismeretlen forrasallapot az atmenetben: " + from;
            return false;
        }
        if (!states.count(to)) {
            error = "Ismeretlen celallapot az atmenetben: " + to;
            return false;
        }
        if (sym.size() != 1 || !alphabet.count(sym[0])) {
            error = "Az atmenetben szereplo jel nincs az abeceben: " + sym;
            return false;
        }
        transitions[{from, sym[0]}].insert(to);
    }

    return true;
}

DFA NFA::determinize() const {

    DFA dfa;
    std::map<std::set<std::string>, std::string> names; // NFA-allapothalmaz -> "sN"
    std::queue<std::set<std::string>> todo;

    for (char c : alphabet) dfa.addSymbol(c);

    // uj DFA-allapot felvetele, ha meg nem letezik
    auto getName = [&](const std::set<std::string>& S) -> const std::string& {
        auto it = names.find(S);
        if (it != names.end()) return it->second;

        std::string name = "s" + std::to_string(names.size());
        dfa.addState(name);
        for (const std::string& q : S) {
            if (acceptStates.count(q)) {   // S es F metszete nem ures
                dfa.addAcceptState(name);
                break;
            }
        }
        todo.push(S);
        return names.emplace(S, name).first->second;
    };

    std::set<std::string> start = {startState};
    dfa.setStartState(getName(start));

    while (!todo.empty()) {
        std::set<std::string> S = todo.front();
        todo.pop();
        std::string from = names[S];

        for (char c : alphabet) {
            std::set<std::string> T;
            for (const std::string& q : S) {
                auto it = transitions.find({q, c});
                if (it != transitions.end()) {
                    T.insert(it->second.begin(), it->second.end());
                }
            }
            if (T.empty()) continue;   // nincs el

            dfa.addTransition(from, c, getName(T));
        }
    }

    return dfa;
}
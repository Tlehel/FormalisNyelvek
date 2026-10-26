#include "DFA.h"
#include <algorithm>
#include <iostream>

bool DFA::loadFromFile(const std::string& filename) {

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Hiba: a bemeneti fajl nem nyithato meg: " << filename << std::endl;
        return false;
    }

    states.clear();
    alphabet.clear();
    acceptStates.clear();
    transitions.clear();
    startState.clear();

    std::set<std::string> known;
    std::string line;

    // 1. sor: allapotok
    if (!std::getline(file, line)) {
        std::cerr << "Hiba: hianyzik az allapotok sora" << std::endl;
        return false;
    }
    {
        std::stringstream ss(line);
        std::string s;
        while (ss >> s) {
            if (known.insert(s).second) states.push_back(s);
        }
    }

    // 2. sor: abece
    if (!std::getline(file, line)) {
        std::cerr << "Hiba: hianyzik az abece sora" << std::endl;
        return false;
    }
    {
        std::stringstream ss(line);
        std::string s;
        while (ss >> s) {
            if (s.size() != 1) {
                std::cerr << "Hiba: ervenytelen jel az abeceben (egy karakter kell): " << s << std::endl;
                return false;
            }
            alphabet.insert(s[0]);
        }
    }

    // 3. sor: kezdoallapot
    if (!std::getline(file, line)) {
        std::cerr << "Hiba: hianyzik a kezdoallapot sora" << std::endl;
        return false;
    }
    {
        std::stringstream ss(line);
        ss >> startState;
        if (!known.count(startState)) {
            std::cerr << "Hiba: a kezdoallapot nincs az allapotok kozott: " << startState << std::endl;
            return false;
        }
    }

    // 4. sor: vegallapotok (lehet ures)
    std::getline(file, line);
    {
        std::stringstream ss(line);
        std::string s;
        while (ss >> s) {
            if (!known.count(s)) {
                std::cerr << "Hiba: a vegallapot nincs az allapotok kozott: " << s << std::endl;
                return false;
            }
            acceptStates.insert(s);
        }
    }

    // atmenetek: forras jel cel
    std::string from, sym, to;
    while (file >> from) {
        if (!(file >> sym >> to)) {
            std::cerr << "Hiba: hianyos atmenet a(z) " << from << " allapotnal" << std::endl;
            return false;
        }
        if (!known.count(from)) {
            std::cerr << "Hiba: ismeretlen forrasallapot az atmenetben: " << from << std::endl;
            return false;
        }
        if (!known.count(to)) {
            std::cerr << "Hiba: ismeretlen celallapot az atmenetben: " << to << std::endl;
            return false;
        }
        if (sym.size() != 1 || !alphabet.count(sym[0])) {
            std::cerr << "Hiba: az atmenetben szereplo jel nincs az abeceben: " << sym << std::endl;
            return false;
        }
        transitions[{from, sym[0]}] = to;
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

namespace {

// "q10" -> value=10, digits="10"; ha nincs szamjegy, false
bool splitNumber(const std::string& name, unsigned long long& value, std::string& digits) {
    size_t pos = name.find_first_of("0123456789");
    if (pos == std::string::npos) return false;
    size_t end = name.find_first_not_of("0123456789", pos);
    digits = name.substr(pos, end == std::string::npos ? std::string::npos : end - pos);
    value = 0;
    for (char ch : digits) value = value * 10 + (ch - '0');
    return true;
}

} // namespace

DFA DFA::minimize() const {

    const int n = static_cast<int>(states.size());
    const int sink = n;   //csapdaallapot a hianyzo atmenetekhez

    std::vector<char> sigma(alphabet.begin(), alphabet.end());
    const int m = static_cast<int>(sigma.size());

    std::map<std::string, int> idx;
    for (int i = 0; i < n; ++i) idx[states[i]] = i;

    // delta[i][k] = cel indexe, hianyzo atmenetnel a csapdaallapot
    std::vector<std::vector<int>> delta(n + 1, std::vector<int>(m, sink));
    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < m; ++k) {
            auto it = transitions.find({states[i], sigma[k]});
            if (it != transitions.end()) {
                delta[i][k] = idx.at(it->second);
            }
        }
    }

    std::vector<char> acc(n + 1, 0);
    for (int i = 0; i < n; ++i) acc[i] = acceptStates.count(states[i]) ? 1 : 0;

    // megkulonboztethetosegi tablazat (a csapdaallapot is resztvesz)
    std::vector<std::vector<char>> dist(n + 1, std::vector<char>(n + 1, 0));
    for (int i = 0; i <= n; ++i)
        for (int j = 0; j <= n; ++j)
            dist[i][j] = (acc[i] != acc[j]);

    bool changed = true;
    while (changed) {
        changed = false;
        for (int i = 0; i <= n; ++i) {
            for (int j = i + 1; j <= n; ++j) {
                if (dist[i][j]) continue;
                for (int k = 0; k < m; ++k) {
                    if (dist[delta[i][k]][delta[j][k]]) {
                        dist[i][j] = dist[j][i] = 1;
                        changed = true;
                        break;
                    }
                }
            }
        }
    }

    // allapotnevek szamerteke, rendezes szam szerint
    std::vector<unsigned long long> num(n, 0);
    std::vector<std::string> digits(n);
    std::vector<char> hasNum(n, 0);
    for (int i = 0; i < n; ++i) hasNum[i] = splitNumber(states[i], num[i], digits[i]);

    std::vector<int> order(n);
    for (int i = 0; i < n; ++i) order[i] = i;
    std::sort(order.begin(), order.end(), [&](int a, int b) {
        if (hasNum[a] != hasNum[b]) return hasNum[a] > hasNum[b];
        if (hasNum[a] && num[a] != num[b]) return num[a] < num[b];
        return states[a] < states[b];
    });

    // kepviselo: a legkisebb nevu egyenertekű allapot
    std::vector<int> rep(n, -1);
    for (int i = 0; i < n; ++i) {
        for (int j : order) {
            if (j == i || !dist[i][j]) {
                rep[i] = j;
                break;
            }
        }
    }

    auto newName = [&](int i) {
        return hasNum[i] ? "s" + digits[i] : "s" + states[i];
    };

    // uj automata felepitese
    DFA result;
    for (char c : sigma) result.addSymbol(c);

    for (int i : order) {
        if (rep[i] != i) continue;
        result.addState(newName(i));
        if (acc[i]) result.addAcceptState(newName(i));
    }

    result.setStartState(newName(rep[idx.at(startState)]));

    for (int i : order) {
        if (rep[i] != i) continue;
        for (int k = 0; k < m; ++k) {
            int t = delta[i][k];
            if (t == sink) continue;   // ures atmenetet nem irunk ki
            result.addTransition(newName(i), sigma[k], newName(rep[t]));
        }
    }

    return result;
}
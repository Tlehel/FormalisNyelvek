#include <iostream>
#include <string>

#include "Determinize.h"
#include "NFA.h"
#include "DFA.h"

void Determinize::initialize_parser(cxxopts::Options &options) {
    options.add_options()
        ("det", "Determinize the automaton in the input file");
}

bool Determinize::is_chosen_problem(const cxxopts::ParseResult &args) {
    return args.count("det") > 0;
}

int Determinize::run(const cxxopts::ParseResult &args) {
    if (!args.count("input") || !args.count("output")) {
        std::cerr << "Hiba: az --input es --output megadasa kotelezo" << std::endl;
        return 1;
    }

    std::string inputFilename = args["input"].as<std::string>();
    std::string outputFilename = args["output"].as<std::string>();

    NFA nfa;
    std::string error;
    if (!nfa.loadFromFile(inputFilename, error)) {
        std::cerr << "Hiba: " << error << std::endl;
        return 1;
    }

    DFA dfa = nfa.determinize();

    if (!dfa.saveToFile(outputFilename)) {
        std::cerr << "Hiba: a kimeneti fajl nem nyithato meg: " << outputFilename << std::endl;
        return 1;
    }

    return 0;
}
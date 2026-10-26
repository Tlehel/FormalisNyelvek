#include <iostream>
#include <string>

#include "Minimize.h"
#include "DFA.h"

void Minimize::initialize_parser(cxxopts::Options &options) {
    options.add_options()
        ("mini", "Minimize the DFA in the input file");
}

bool Minimize::is_chosen_problem(const cxxopts::ParseResult &args) {
    return args.count("mini") > 0;
}

int Minimize::run(const cxxopts::ParseResult &args) {
    if (!args.count("input") || !args.count("output")) {
        std::cerr << "Hiba: --input es --output megadasa kotelezo" << std::endl;
        return 1;
    }

    std::string inputFilename = args["input"].as<std::string>();
    std::string outputFilename = args["output"].as<std::string>();

    DFA dfa;
    if (!dfa.loadFromFile(inputFilename)) {
        return 1;
    }

    DFA minimal = dfa.minimize();

    if (!minimal.saveToFile(outputFilename)) {
        std::cerr << "Hiba: a kimeneti fajl nem irhato: " << outputFilename << std::endl;
        return 1;
    }

    return 0;
}
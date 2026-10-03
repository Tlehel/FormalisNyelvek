//
// Created by Lehel on 2026-10-02.
//

#include <string>
#include <fstream>
#include <iostream>
#include "DFA.h"

#include "Check.h"


void Check::initialize_parser(cxxopts::Options &options) {
    options.add_options()
        ("check", "reads an automat, then checks if the words listed can be generated with it", cxxopts::value<std::string>());
}

bool Check::is_chosen_problem(const cxxopts::ParseResult &args) {
    return args.count("check") > 0;
}

int Check::run(const cxxopts::ParseResult &args) {
    std::string inputFilename = args["input"].as<std::string>();
    std::string outputFilename = args["output"].as<std::string>();

    std::string wordsToCheck = args["check"].as<std::string>(); // check kapcsoloval beolvasott szavak

    std::ofstream outputFile(outputFilename);

    DFA dfa; //automata beolvasasa
    if (!dfa.loadFromFile(inputFilename)) {
        std::cerr << "Error opening input file: " << inputFilename << std::endl;
        return 1;
    }

    if (!outputFile) {
        std::cerr << "Error opening output file: " << outputFilename << std::endl;
        return 1;
    }

    std::stringstream ss(wordsToCheck);
    std::string word;

    while (std::getline(ss, word, ',')) {
        if (dfa.accepts(word)) {
            outputFile << "IGEN\n";
        } else {
            outputFile << "NEM\n";
        }
    }

    return 0;
}
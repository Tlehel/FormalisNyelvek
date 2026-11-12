#ifndef PROBLEMS_REGEX_H
#define PROBLEMS_REGEX_H

#include "../problem.hpp"

class RegexProblem : public Problem {
public:
    void initialize_parser(cxxopts::Options &options) override;
    bool is_chosen_problem(const cxxopts::ParseResult &args) override;
    int run(const cxxopts::ParseResult &args) override;
};

#endif // PROBLEMS_REGEX_H
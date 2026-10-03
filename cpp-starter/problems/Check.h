//
// Created by Lehel on 2026-10-02.
//

#ifndef CHECK_H
#define CHECK_H

#include "../problem.hpp"

class Check : public Problem{
public:
    void initialize_parser(cxxopts::Options &options) override;
    bool is_chosen_problem(const cxxopts::ParseResult &args) override;
    int run(const cxxopts::ParseResult &args) override;

};



#endif //CHECK_H

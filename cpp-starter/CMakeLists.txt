cmake_minimum_required(VERSION 3.10)

project(project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add your own problems here
add_executable(project main.cpp problems/sum.cpp
        problems/Check.cpp
        problems/Check.h
        problems/DFA.cpp
        problems/DFA.h
        problems/NFA.cpp
        problems/NFA.h
        problems/Determinize.cpp
        problems/Determinize.h)
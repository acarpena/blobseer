cmake_minimum_required(VERSION 2.6)

# Some extra paramters to the compiler
add_definitions(-Wall -O3 -DCORE2)

# Target definition
add_library(havege2 SHARED HAVEGE2.0.c)

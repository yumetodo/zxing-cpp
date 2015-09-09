#pragma once
#ifndef INC_CPP11_RAND_H
#define INC_CPP11_RAND_H
#include <random>//std::random_device, std::mt19937, std::seed_seq, std::bernoulli_distribution
std::mt19937 create_random_engine(unsigned int seed);
#endif //INC_CPP11_RAND_H

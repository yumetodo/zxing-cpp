#include "cpp11_rand.h"
#include <vector>
#include <algorithm>//std::generate
std::mt19937 create_random_engine(unsigned int seed) {
	std::random_device rnd;
	std::vector<std::seed_seq::result_type> v(10);
	std::generate(v.begin(), v.end(), std::ref(rnd));//create vector for seed
	if (0 != seed) v.push_back(seed);
	std::seed_seq sedv(v.begin(), v.end());
	return std::mt19937(sedv);//create random engine(algorithm MT)
}

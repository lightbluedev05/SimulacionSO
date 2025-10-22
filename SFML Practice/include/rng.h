#pragma once
#include <random>

namespace random {
	extern std::minstd_rand g_rng;
	extern std::uniform_int_distribution<int> g_dist;



	int randomBetween(int min, int max);
}
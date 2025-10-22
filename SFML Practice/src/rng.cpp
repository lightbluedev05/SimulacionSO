#include <random>
#include "rng.h"

std::uniform_int_distribution<int> random::g_dist(1, 100);
std::minstd_rand random::g_rng{};

int random::randomBetween(int min, int max) {
	std::uniform_int_distribution<int> un_dist(min, max);
	return un_dist(random::g_rng);
}
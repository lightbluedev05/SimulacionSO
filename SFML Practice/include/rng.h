#pragma once
#include <random>
#include <vector>
#include <numeric>
#include <cmath>

namespace random {
	extern std::minstd_rand g_rng;
	extern std::uniform_int_distribution<int> g_dist;

	struct Sequence {
		std::vector<int> times;
		int priority;
		int totalTime;
		float boundness;
		int arrivalTime; 
	};

	struct SequenceDetail {
		int amount{};
		std::pair<int, int> switchesRange{};
		std::pair<int, int> priorityRange{};
		std::pair<int, int> totalTimeRange{};
		float boundnessFactor;
		int arrivalInterval{ 1000 };
		float arrivalFactor{ 0.2f };
	};

	int randomBetween(int min, int max);
	std::vector<int> randomSplit(int total, int parts, std::mt19937& rng);
	std::vector<Sequence> generateSequences(const SequenceDetail& detail);

	extern std::vector<Sequence> g_scenarioSequence1;
	extern std::vector<Sequence> g_scenarioSequence2;
	extern std::vector<Sequence> g_scenarioSequence3;

	extern SequenceDetail g_scenarioDetail1;
	extern SequenceDetail g_scenarioDetail2;
	extern SequenceDetail g_scenarioDetail3;
}
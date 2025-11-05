#include <random>
#include "rng.h"

std::uniform_int_distribution<int> random::g_dist(1, 100);
std::minstd_rand random::g_rng{};

int random::randomBetween(int min, int max) {
	std::uniform_int_distribution<int> un_dist(min, max);
	return un_dist(random::g_rng);
}

using namespace random;
std::vector<int> random::randomSplit(int total, int parts, std::mt19937& rng) {
    std::vector<int> cuts;
    std::uniform_int_distribution<int> dist(1, total - 1);

    for (int i = 0; i < parts - 1; ++i)
        cuts.push_back(dist(rng));

    cuts.push_back(total);
    std::sort(cuts.begin(), cuts.end());

    std::vector<int> result;
    result.reserve(parts);
    int prev = 0;
    for (int c : cuts) {
        result.push_back(c - prev);
        prev = c;
    }
    return result;
}

std::vector<Sequence> random::generateSequences(const SequenceDetail& detail) {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<float> rand01(0.0f, 1.0f);
    std::uniform_int_distribution<int> randSwitch(detail.switchesRange.first, detail.switchesRange.second);
    std::uniform_int_distribution<int> randPriority(detail.priorityRange.first, detail.priorityRange.second);
    std::uniform_int_distribution<int> randTotal(detail.totalTimeRange.first, detail.totalTimeRange.second);

    std::vector<Sequence> result;
    result.reserve(detail.amount);

    int currentArrival = 0;

    for (int i = 0; i < detail.amount; ++i) {
        int nSwitch = randSwitch(rng);
        int totalTime = randTotal(rng);
        int priority = randPriority(rng);

        int numA = nSwitch + 1;
        int numB = nSwitch;

        int timeA = static_cast<int>(std::round(totalTime * detail.boundnessFactor));
        int timeB = totalTime - timeA;

        auto partsA = randomSplit(std::max(timeA, numA), numA, rng);
        auto partsB = nSwitch > 0 ? randomSplit(std::max(timeB, numB), numB, rng) : std::vector<int>{};

        std::vector<int> seq;
        seq.reserve(numA + numB);

        for (int j = 0; j < numA; ++j) {
            seq.push_back(partsA[j]);
            if (j < numB) seq.push_back(partsB[j]);
        }

        Sequence s{ seq, priority, totalTime, detail.boundnessFactor, currentArrival };
        result.push_back(s);

        // Update arrival time
        float randFactor = 1.0f - detail.arrivalFactor + (rand01(rng) * 2 * detail.arrivalFactor);
        currentArrival += static_cast<int>(detail.arrivalInterval * randFactor);
    }

    return result;
}

namespace random {
    SequenceDetail g_scenarioDetail1{
        15,                 // Amount
        { 2, 5 },           // Switches Range
        { 0, 4 },           // Priority Range
        { 200, 5000 },      // Total Time Range
        0.85f,              // Boundness Factor
        250,                // Arrival Interval
        0.3f                // Arrival Factor
    };
    SequenceDetail g_scenarioDetail2{
        15,                 // Amount
        { 5, 9 },           // Switches Range
        { 0, 4 },           // Priority Range
        { 800, 2000 },      // Total Time Range
        0.25f,              // Boundness Factor
        250,                // Arrival Interval
        0.3f                // Arrival Factor
    };
    SequenceDetail g_scenarioDetail3{
        25,                 // Amount
        { 2, 5 },           // Switches Range
        { 0, 4 },           // Priority Range
        { 500, 2000 },      // Total Time Range
        0.75f,              // Boundness Factor
        900,                // Arrival Interval
        0.95f                // Arrival Factor
    };

    std::vector<Sequence> g_scenarioSequence1 = generateSequences(g_scenarioDetail1);
    std::vector<Sequence> g_scenarioSequence2 = generateSequences(g_scenarioDetail2);
    std::vector<Sequence> g_scenarioSequence3 = generateSequences(g_scenarioDetail3);
}
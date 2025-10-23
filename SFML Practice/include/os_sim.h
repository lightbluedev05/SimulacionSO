#pragma once

#include <vector>
#include <list>
#include <memory>
#include <SFML/Graphics.hpp>
#include <queue>

#include "scheduler.h"


namespace os_sim {
	enum class PCBState { New, Ready, Running, Waiting, Terminated };
	enum class Algorithm { FCFS, SJF, RoundRobin };

	struct PCB {
		int pid{};
		PCBState processState{ PCBState::New };
		// -- SJF Related
		float estimatedBurstTime_s{ 5.0f };
		float lastBurstTime_s{};
		float alpha{ 0.5f };

		// -- Round Robin Related


		// Simulation-Related
		int minDuration_s{ 10 };	// Minimum Duration of process (in s)
		int minWaitDuration_s{ 0 };	// Minimum Duration of Simulated Waiting Time (in s)

		sf::Clock processClock{};
		sf::Time totalExecutionTime{};
		sf::Time waitingTime{};

		sf::Clock ioBurstClock{};
		sf::Clock cpuBurstClock{};

		sf::Time totalBurstTime{};
		sf::Time responseTime{};
		sf::Time totalWaitingTime{};

		std::queue<int> cpuBurstSim{};
		std::queue<int> ioBurstSim{};
	};

	namespace global {
		extern bool g_isRunningProcess;
		extern PCB g_PCBCurrentlyRunning;
		extern std::vector<PCB> g_pcbWaitLine;
		extern Algorithm g_currentAlgorithm;

		extern sf::Clock g_CPUBurstClock;
		extern sf::Time g_rrTimeQuantum;

		extern std::unique_ptr<IScheduler> g_scheduler;
		extern int g_finishedProcesses;
	}

	// PCB Control Functions
	void createPCB(std::queue<int> cpuBurst, std::queue<int> ioBurst, std::list<PCB>& pcbQueue);
	void simulate(std::list<PCB>& pcbQueue);
	void simulateIOWaiting(std::list<PCB>& pcbQueue);

	bool ifStartWaiting(PCB& process);
}


#include <vector>
#include <list>

#include "os_sim.h"
#include "rng.h"
#include "scheduler.h"

#ifdef OS_DEBUG
#include <iostream>
#endif // !OS_DEBUG

// -------------------------------- Declarations --------------------------------
namespace os_sim {

	namespace global {
		bool g_isRunningProcess{ false };
		PCB g_PCBCurrentlyRunning{};
		std::vector<PCB> g_pcbWaitLine{};
		Algorithm g_currentAlgorithm{};
		sf::Clock g_CPUBurstClock{};
		sf::Time g_rrTimeQuantum{ sf::milliseconds(50) };

		std::unique_ptr<IScheduler> g_scheduler{ nullptr };
		int g_finishedProcesses{ 0 };

		sf::Time g_agingInterval{ sf::milliseconds(200) };
	}
}

// -------------------------------- Helper Functions --------------------------------
void os_sim::createPCB(std::queue<int> cpuBurst, std::queue<int> ioBurst, int priority, std::list<PCB>& pcbQueue) {
	static unsigned int g_pcbIdAssign{ 0 };
	// Assign Phase
	os_sim::PCB newPCB{};
	newPCB.pid = ++g_pcbIdAssign;
	newPCB.processState = os_sim::PCBState::New;
	newPCB.cpuBurstSim = cpuBurst;
	newPCB.ioBurstSim = ioBurst;
	newPCB.processClock.restart();
	newPCB.ioBurstClock.reset();
	newPCB.cpuBurstClock.reset();
	newPCB.priority = priority;

	// Return Block
	os_sim::global::g_scheduler->returnToQueue(newPCB, pcbQueue);
	//returnToQueue(newPCB, pcbQueue);
#ifdef OS_DEBUG
	std::cout << "\n [Creation]\tProcess '" << newPCB.pid << "' created succesfully.";
#endif // !OS_DEBUG
}

bool os_sim::ifStartWaiting(os_sim::PCB& process) {
	if (process.cpuBurstSim.size() <= 1) return false;
	return process.cpuBurstClock.getElapsedTime().asMilliseconds() > process.cpuBurstSim.front() && 1 == random::g_dist(random::g_rng);
}

bool os_sim::ifEnd(os_sim::PCB& process) {
	return process.cpuBurstSim.size() == 1 && process.cpuBurstClock.getElapsedTime().asMilliseconds() > process.cpuBurstSim.front() && 1 == random::g_dist(random::g_rng);
}

// -------------------------------- Main Simulator Function Block --------------------------------
bool os_sim::simulate(std::list<os_sim::PCB>& pcbQueue) {
	using os_sim::global::g_scheduler;
	using os_sim::global::g_isRunningProcess;
	using os_sim::global::g_PCBCurrentlyRunning;
	using os_sim::global::g_pcbWaitLine;
	using os_sim::global::g_finishedProcesses;
	if (!g_scheduler) return true;

	if (!g_pcbWaitLine.empty()) simulateIOWaiting(pcbQueue);
	// Priority
	if (os_sim::global::g_currentAlgorithm == os_sim::Algorithm::DynamicPriority) simulateAging(pcbQueue);
	// If no Process, select one
	if (!g_isRunningProcess) {
		if (pcbQueue.empty()) return false;
		g_scheduler->selectProcess(pcbQueue);
		g_scheduler->onProcessStart(g_PCBCurrentlyRunning);
	}
	// If Process, simulate Execution
	else g_scheduler->onProcessExecution(g_PCBCurrentlyRunning, pcbQueue);
	if (g_PCBCurrentlyRunning.processState == os_sim::PCBState::Terminated) {
		g_scheduler->outputMetrics(g_PCBCurrentlyRunning);
		++g_finishedProcesses;
		if (g_pcbWaitLine.empty() && pcbQueue.empty()) return true;
	}

	return false;
}

void os_sim::simulateIOWaiting(std::list<os_sim::PCB>& pcbQueue) {
	using os_sim::global::g_pcbWaitLine;
	using os_sim::global::g_scheduler;

	for (auto it = g_pcbWaitLine.begin(); it != g_pcbWaitLine.end(); ) {
		if (it->ioBurstClock.getElapsedTime().asMilliseconds() > it->minWaitDuration_s) {
			it->totalWaitingTime += it->ioBurstClock.restart();

			g_scheduler->returnToQueue(*it, pcbQueue);
#ifdef OS_DEBUG
			std::cout << "\n [PROCESS " << it->pid << "]\tFinished IO, now waiting at queue.";
#endif // !OS_DEBUG
			it = g_pcbWaitLine.erase(it);
		}
		else ++it;
	}
}

bool os_sim::ifPriorityAlgorithm() {
		using os_sim::global::g_currentAlgorithm;
		return (g_currentAlgorithm == os_sim::Algorithm::StaticPriority || g_currentAlgorithm == os_sim::Algorithm::DynamicPriority);
}

void os_sim::simulateAging(std::list<os_sim::PCB>& pcbQueue) {
	using os_sim::global::g_agingInterval;

	for (auto& process : pcbQueue) {
		if (process.processState != os_sim::PCBState::Ready) continue;
		if (process.ioBurstClock.getElapsedTime() >= g_agingInterval) {
			if (process.priority > 0) {
				process.priority--;
#ifdef OS_DEBUG
				std::cout << "\n [AGING]\tProcess " << process.pid << " priority increased to " << process.priority;
#endif
			}
			process.processClock.restart();
		}
	}
}
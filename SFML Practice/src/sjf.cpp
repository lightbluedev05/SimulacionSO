#include "sjf.h"

#ifdef OS_DEBUG
#include <iostream>
#endif // !OS_DEBUG

#include "os_sim.h"
#include "rng.h"

void SJFScheduler::onProcessExecution(os_sim::PCB& process, [[maybe_unused]] std::list<os_sim::PCB>& pcbQueue) {
	using os_sim::global::g_CPUBurstClock;
	using os_sim::global::g_pcbWaitLine;
	using os_sim::global::g_isRunningProcess;
	// -- Simulate process finalization
	if (process.cpuBurstSim.size() == 1 && process.cpuBurstClock.getElapsedTime().asSeconds() > process.cpuBurstSim.front() && 1 == random::g_dist(random::g_rng)) {
		process.processState = os_sim::PCBState::Terminated;
		process.totalExecutionTime = process.processClock.reset() + process.responseTime;								//	<-------- Turnaround
		process.waitingTime = process.totalExecutionTime - process.totalBurstTime - process.totalWaitingTime;			//	<-------- Waiting Time

#ifdef OS_DEBUG
		std::cout << "\n [PROCESS " << process.pid << "]\tTerminated after " << process.totalExecutionTime.asSeconds() << "s.";
#endif // !OS_DEBUG
		g_isRunningProcess = false;
	}
	// -- Simulate process waiting
	else if (os_sim::ifStartWaiting(process)) {
		process.processState = os_sim::PCBState::Waiting;
		sf::Time saveTime{ g_CPUBurstClock.restart() };
		process.totalBurstTime += saveTime;
		process.minWaitDuration_s = process.ioBurstSim.front();
		process.cpuBurstSim.pop();
		process.ioBurstSim.pop();

		process.lastBurstTime_s = saveTime.asSeconds();
		process.estimatedBurstTime_s = process.alpha * process.lastBurstTime_s + (1 - process.alpha) * process.estimatedBurstTime_s;

#ifdef OS_DEBUG
		std::cout << "\n [PROCESS " << process.pid << "]\tHalted, waiting for IO. [Estimated Burst Time of " << process.estimatedBurstTime_s <<  "s.]";
#endif // !OS_DEBUG

		process.ioBurstClock.restart();
		g_pcbWaitLine.push_back(process);
		g_isRunningProcess = false;
	}
}

void SJFScheduler::returnToQueue(os_sim::PCB& process, std::list<os_sim::PCB>& pcbQueue) {
	process.processState = os_sim::PCBState::Ready;
	auto insertPos = std::find_if(pcbQueue.begin(), pcbQueue.end(),
		[&process](const os_sim::PCB& other) {
			return process.estimatedBurstTime_s < other.estimatedBurstTime_s;
		});
	pcbQueue.insert(insertPos, process);			// Insert elements already sorted so selectProcess() can just pick the front.
}

#include "fcfs.h"

#ifdef OS_DEBUG
#include <iostream>
#endif // !OS_DEBUG

#include "os_sim.h"
#include "rng.h"

void FCFSScheduler::onProcessExecution(os_sim::PCB& process, [[maybe_unused]] std::list<os_sim::PCB>& pcbQueue) {
	using os_sim::global::g_CPUBurstClock;
	using os_sim::global::g_pcbWaitLine;
	using os_sim::global::g_isRunningProcess;
	// -- Simulate process finalization
	if (process.cpuBurstSim.size() == 1 && process.cpuBurstClock.getElapsedTime().asSeconds() > process.cpuBurstSim.front() && 1 == random::g_dist(random::g_rng)) {
		process.processState = os_sim::PCBState::Terminated;
		process.totalExecutionTime = process.processClock.reset() + process.responseTime;						//	<-------- Turnaround
		process.waitingTime = process.totalExecutionTime - process.totalBurstTime - process.totalWaitingTime;	//	<-------- Waiting Time

#ifdef OS_DEBUG
		std::cout << "\n [PROCESS " << process.pid << "]\tTerminated after " << process.totalExecutionTime.asSeconds() << "s.";
#endif // !OS_DEBUG
		g_isRunningProcess = false;
	}
	// -- Simulate process waiting
	else if (os_sim::ifStartWaiting(process)) {
		process.processState = os_sim::PCBState::Waiting;
		process.totalBurstTime += g_CPUBurstClock.restart();
		process.minWaitDuration_s = process.ioBurstSim.front();
		process.cpuBurstSim.pop();
		process.ioBurstSim.pop();

#ifdef OS_DEBUG
		std::cout << "\n [PROCESS " << process.pid << "]\tHalted, waiting for IO.";
#endif // !OS_DEBUG
		//returnToQueue(pcbQueue);
		process.ioBurstClock.restart();
		g_pcbWaitLine.push_back(process);
		g_isRunningProcess = false;
	}
}

void FCFSScheduler::returnToQueue(os_sim::PCB& process, std::list<os_sim::PCB>& pcbQueue) {
	process.processState = os_sim::PCBState::Ready;
	pcbQueue.push_back(process);
}
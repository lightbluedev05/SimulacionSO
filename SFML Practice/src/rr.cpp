#include "rr.h"

#ifdef OS_DEBUG
#include <iostream>
#endif // !OS_DEBUG

#include "os_sim.h"
#include "rng.h"

void RRScheduler::onProcessExecution(os_sim::PCB& process, [[maybe_unused]] std::list<os_sim::PCB>& pcbQueue) {
	using os_sim::global::g_CPUBurstClock;
	using os_sim::global::g_pcbWaitLine;
	using os_sim::global::g_isRunningProcess;
	using os_sim::global::g_rrTimeQuantum;
	// -- Simulate process execution
	if (g_CPUBurstClock.getElapsedTime() > g_rrTimeQuantum) {
		this->returnToQueue(process, pcbQueue);
		g_isRunningProcess = false;
		process.totalBurstTime += g_CPUBurstClock.restart();
#ifdef OS_DEBUG
		std::cout << "\t[Exceded time quantum]";
#endif // !OS_DEBUG
		return;
	}

	// -- Simulate process finalization
	if (process.processClock.getElapsedTime().asSeconds() > process.minDuration_s && 1 == random::g_dist(random::g_rng)) {
		process.processState = os_sim::PCBState::Terminated;
		process.totalExecutionTime = process.processClock.reset() + process.responseTime;							//	<-------- Turnaround
		process.waitingTime = process.totalExecutionTime - process.totalBurstTime - -process.totalWaitingTime;		//	<-------- Waiting Time

#ifdef OS_DEBUG
		std::cout << "\n [PROCESS " << process.pid << "]\tTerminated after " << process.totalExecutionTime.asSeconds() << "s.";
#endif // !OS_DEBUG
		g_isRunningProcess = false;
	}
	// -- Simulate process waiting
	else if (1 == random::g_dist(random::g_rng)) {
		process.processState = os_sim::PCBState::Waiting;
		process.totalBurstTime += g_CPUBurstClock.restart();
		process.minWaitDuration_s = random::randomBetween(5, 15);

#ifdef OS_DEBUG
		std::cout << "\n [PROCESS " << process.pid << "]\tHalted, waiting for IO.";
#endif // !OS_DEBUG
		//returnToQueue(pcbQueue);
		process.ioBurstClock.restart();
		g_pcbWaitLine.push_back(process);
		g_isRunningProcess = false;
	}
}

void RRScheduler::returnToQueue(os_sim::PCB& process, std::list<os_sim::PCB>& pcbQueue) {
	process.processState = os_sim::PCBState::Ready;
	pcbQueue.push_back(process);
}
#include <fstream>
#include <filesystem>
#include <iostream>

#include "scheduler.h"
#include "os_sim.h"

// --- Default virtual behaviour ---

void IScheduler::onProcessStart(os_sim::PCB& process) {
    using os_sim::global::g_CPUBurstClock;
    g_CPUBurstClock.restart();
    process.cpuBurstClock.restart();
#ifdef OS_DEBUG
    std::cout << "\n [PROCESS " << process.pid << "]\tStarting execution.";
#endif // !OS_DEBUG
}

void IScheduler::selectProcess(std::list<os_sim::PCB>& pcbQueue) {
    using os_sim::global::g_isRunningProcess;
    using os_sim::global::g_PCBCurrentlyRunning;

    g_PCBCurrentlyRunning = pcbQueue.front();
    pcbQueue.pop_front();
    g_PCBCurrentlyRunning.processState = os_sim::PCBState::Running;
    //g_PCBCurrentlyRunning.cpuBurstClock.restart();

    g_isRunningProcess = true;
    if (g_PCBCurrentlyRunning.responseTime == sf::Time::Zero) g_PCBCurrentlyRunning.responseTime = g_PCBCurrentlyRunning.processClock.restart();	// Response Time
}


// --- Output Metrics to File ---

void IScheduler::outputMetrics([[maybe_unused]] os_sim::PCB& process) {
    std::ofstream outFile("data/results.csv", std::ios::app);
    if (!outFile.is_open()) return;

	float turnaroundTime = process.totalExecutionTime.asSeconds();
	float waitingTime = process.waitingTime.asSeconds();
	float responseTime = process.responseTime.asSeconds();

    outFile
        << process.pid << ","
        << turnaroundTime << ','
        << waitingTime << ','
        << responseTime << ','
        << process.totalWaitingTime.asSeconds() << '\n';

    outFile.close();

	using namespace os_sim::global;
	gr_turnaroundTimeAvg += turnaroundTime;
	gr_waitingTimeAvg += waitingTime;
	gr_responseTimeAvg += responseTime;
}
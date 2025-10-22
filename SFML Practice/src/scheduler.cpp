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
    g_PCBCurrentlyRunning.cpuBurstClock.restart();

    g_isRunningProcess = true;
    if (g_PCBCurrentlyRunning.responseTime == sf::Time::Zero) g_PCBCurrentlyRunning.responseTime = g_PCBCurrentlyRunning.processClock.restart();	// Response Time
}


// --- Output Metrics to File ---

void IScheduler::outputMetrics([[maybe_unused]] os_sim::PCB& process) {
    static const std::string filename = "data/results.csv";

    // If file doesn't exist, write header
    std::ofstream outFile(filename, std::ios::app); // Open in append mode
    if (!outFile.is_open()) return;

    outFile
        << process.pid << ","
        << process.totalExecutionTime.asSeconds() << ','
        << process.waitingTime.asSeconds() << ','
        << process.responseTime.asSeconds() << ','
        << process.totalWaitingTime.asSeconds() << '\n';

    outFile.close();
}
#define OS_GRAPH

// Normal Execution
#ifdef OS_GRAPH
#include <SFML/Graphics.hpp>
#endif // OS_GRAPH
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <list>

#include "os_sim.h"
#include "rng.h"

#include "fcfs.h"
#include "sjf.h"
#include "rr.h"
#include "s_priority.h"
#include "d_priority.h"

std::string generateUniqueFilename(const std::string& prefix, const std::string& extension) {
	namespace fs = std::filesystem;

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::tm tm_struct;
    localtime_s(&tm_struct, &in_time_t);  // thread-safe version

    std::stringstream ss;
    ss << prefix << "_"
        << std::put_time(&tm_struct, "%Y-%m-%d_%H-%M-%S")
        << "-" << std::setw(3) << std::setfill('0') << ms.count()
        << extension;
    return ss.str();
}


void static importFromFile(std::list<os_sim::PCB>& pcbQueue) {
    std::ifstream file("data/input.csv");
    if (!file.is_open()) return;

    
    std::string line;

    while (std::getline(file, line)) {
        std::queue<int> cpuTemp;
        std::queue<int> ioTemp;

        std::stringstream ss(line);
        std::string token;
        int index{ 0 };
		int priority{ 0 };

        while (std::getline(ss, token, ',')) {
            int number = std::stoi(token);

            if (index == 0) priority = number;
            else if (index % 2 == 0) cpuTemp.push(number);
            else ioTemp.push(number);

            ++index;
        }
        os_sim::createPCB(cpuTemp, ioTemp, priority, pcbQueue);
    }
    file.close();
}

int main()
{
    using os_sim::global::g_currentAlgorithm;
    using os_sim::global::g_scheduler;
    g_currentAlgorithm = os_sim::Algorithm::FCFS;
    switch (g_currentAlgorithm) {
    case os_sim::Algorithm::FCFS:
        g_scheduler = std::make_unique<FCFSScheduler>();
        break;
    case os_sim::Algorithm::SJF:
        g_scheduler = std::make_unique<SJFScheduler>();
        break;
    case os_sim::Algorithm::RoundRobin:
        g_scheduler = std::make_unique<RRScheduler>();
        break;
    case os_sim::Algorithm::StaticPriority:
        g_scheduler = std::make_unique<SPriorityScheduler>();
        break;
    case os_sim::Algorithm::DynamicPriority:
        g_scheduler = std::make_unique<DPriorityScheduler>();
        break;
    }
    //bool simEnd{ false };


    std::ofstream clearFile("data/results.csv", std::ios::trunc);
    clearFile << "PID,TurnaroundTime,WaitingTime,ResponseTime,IO Time\n";
    clearFile.close();

    std::list<os_sim::PCB> simulationQueue{};
    importFromFile(simulationQueue);

#ifdef OS_GRAPH
    sf::RenderWindow window(sf::VideoMode({ 800,600 }), "Simulator");
    window.setVerticalSyncEnabled(true);
    //window.setFramerateLimit(10);

    // Clock
	sf::Clock simulationClock{};
	simulationClock.restart();
    while (window.isOpen())
    {
        // Process Planner
        if (os_sim::simulate(simulationQueue)) {
            {
                std::ofstream outFile("data/results.csv", std::ios::app);
                outFile << "ThroughPut," << os_sim::global::g_finishedProcesses / simulationClock.reset().asSeconds() << "/s,,,";
                outFile.close();
            }
            std::ifstream inFile("data/results.csv", std::ios::binary);
            std::ofstream outFile("data/output/temp.csv", std::ios::binary);
            outFile << inFile.rdbuf();
            inFile.close();
            outFile.close();

            std::string name{};
            switch (g_currentAlgorithm)
            {
			case os_sim::Algorithm::FCFS: name = "FCFS";
                break;
			case os_sim::Algorithm::SJF: name = "SJF";
                break;
			case os_sim::Algorithm::RoundRobin: name = "RR";
                break;
			case os_sim::Algorithm::StaticPriority: name = "StaticPriority";
                break;
			case os_sim::Algorithm::DynamicPriority: name = "DynamicPriority";  
                break;
            default:
                break;
            }

            std::filesystem::rename("data/output/temp.csv", "data/output/" + generateUniqueFilename(name, ".csv"));
            std::cout << "\n\nSimulation Finished";
            return 1;
        }

        // Event Handle
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            //if (event->is<sf::Event::KeyPressed>()) os_sim::createPCB(random::randomBetween(5, 20), simulationQueue);
        }



        // Render
        window.clear(sf::Color::Black);
        window.display();
    }
#endif // OS_GRAPH
    
}

#define OS_GRAPH

// Normal Execution
#ifdef OS_GRAPH
#include <SFML/Graphics.hpp>
#endif // OS_GRAPH
#include <fstream>
#include <sstream>
#include <iostream>
#include <list>

#include "os_sim.h"
#include "rng.h"

#include "fcfs.h"
#include "sjf.h"
#include "rr.h"

void static importFromFile(std::list<os_sim::PCB>& pcbQueue) {
    std::ifstream file("data/input.csv");
    if (!file.is_open()) return;

    
    std::string line;

    while (std::getline(file, line)) {
        std::queue<int> cpuTemp;
        std::queue<int> ioTemp;

        std::stringstream ss(line);
        std::string token;
        int index = 0;

        while (std::getline(ss, token, ',')) {
            int number = std::stoi(token);

            if (index % 2 == 0) cpuTemp.push(number);
            else ioTemp.push(number);

            ++index;
        }
        os_sim::createPCB(cpuTemp, ioTemp, pcbQueue);
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
    }

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
    while (window.isOpen())
    {
        // Process Planner
        os_sim::simulate(simulationQueue);

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

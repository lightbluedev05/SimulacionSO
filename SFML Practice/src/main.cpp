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
#include <array>

#include "os_sim.h"
#include "rng.h"

#include "fcfs.h"
#include "sjf.h"
#include "rr.h"
#include "s_priority.h"
#include "d_priority.h"
#include "visualizer.h"
#include "interaction.h"

void loadFromCSV(const std::string& filename);
std::string generateUniqueFilename(const std::string& prefix, const std::string& extension);
void static importFromFile(std::list<os_sim::PCB>& pcbQueue);
void endSimulation(sf::Clock& simClock);
void drawText(sf::RenderTarget& target, const sf::Text& text, const sf::Vector2f& pos, const sf::Color& color = sf::Color::White, bool centered = false);
void setAlgorithm(const os_sim::Algorithm& algorithm);
std::queue<sf::Time> static generateFromSequence(const std::vector<random::Sequence>& source, std::list<os_sim::PCB>& pcbQueue);
void drawDetail(sf::RenderTarget& target, const random::SequenceDetail& detail, const sf::Vector2f& pos);
void clearQueue(std::queue<sf::Time>& q);
void clearFile();

int main()
{
    using os_sim::global::g_scheduler;
    using os_sim::global::g_currentAlgorithm;

    clearFile();

    std::list<os_sim::PCB> simulationQueue{};
    std::list<os_sim::PCB> pullQueue{};
    loadFromCSV("data/config.csv");
    setAlgorithm(g_currentAlgorithm);
    importFromFile(simulationQueue);

#ifdef OS_GRAPH
    sf::RenderWindow window(sf::VideoMode({ 1200,600 }), "Simulator");
    //window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(200);
    Visualizer visual(window);
    int windowState{ 0 };
    sf::Clock simulationClock{};
	std::queue<sf::Time> arrivalTimeQueue{};
    // -- Buttons Definition
    interaction::InteractionManager mainMenuInteractionManager{};
    interaction::InteractionManager simulatorInteractionManager{};
    sf::Vector2f posAux{ 40.f,200.f };
    std::array<interaction::Button, 4> mainMenuButtonSet{
        interaction::Button{
        mainMenuInteractionManager,
        sf::Vector2f{380.f,40.f},
        sf::Text{os_sim::global::g_defaultFont, "Escenario 1: Mezcla de Procesos Cortos y Largos", 14},
        posAux
        },
        interaction::Button{
        mainMenuInteractionManager,
        sf::Vector2f{380.f,40.f},
        sf::Text{os_sim::global::g_defaultFont, "Escenario 2: CPU-Bound vs IO-Bound", 14},
		posAux + sf::Vector2f{0.f,80.f}
        },
        interaction::Button{
        mainMenuInteractionManager,
        sf::Vector2f{380.f,40.f},
        sf::Text{os_sim::global::g_defaultFont, "Escenario 3: Alta vs Baja Concurrencia", 14},
        posAux + sf::Vector2f{0.f,160.f}
        },
        interaction::Button{
        mainMenuInteractionManager,
        sf::Vector2f{300.f,40.f},
        sf::Text{os_sim::global::g_defaultFont, "First Come First Served", 14},
        posAux + sf::Vector2f{40.f,300.f}
        }
    };
    mainMenuButtonSet[0].setOnClick([
        &windowState, &mainMenuInteractionManager, &simulatorInteractionManager, &simulationQueue, &simulationClock, &arrivalTimeQueue, &pullQueue]() {
            mainMenuInteractionManager.setAllActiveTo(false);
            simulatorInteractionManager.setAllActiveTo(true);
			windowState = 1; clearQueue(arrivalTimeQueue); clearFile();
		    arrivalTimeQueue = generateFromSequence(random::g_scenarioSequence1, pullQueue);
            simulationClock.restart(); os_sim::resetAvg(); os_sim::global::g_finishedPCB.clear(); simulationQueue.clear();
        });
    mainMenuButtonSet[1].setOnClick([
        &windowState, &mainMenuInteractionManager, &simulatorInteractionManager, &simulationQueue, &simulationClock, &arrivalTimeQueue, &pullQueue]() {
            mainMenuInteractionManager.setAllActiveTo(false);
            simulatorInteractionManager.setAllActiveTo(true);
            windowState = 2; clearQueue(arrivalTimeQueue); clearFile();
            arrivalTimeQueue = generateFromSequence(random::g_scenarioSequence2, pullQueue);
            simulationClock.restart(); os_sim::resetAvg(); os_sim::global::g_finishedPCB.clear(); simulationQueue.clear();
        });
    mainMenuButtonSet[2].setOnClick([
        &windowState, &mainMenuInteractionManager, &simulatorInteractionManager, &simulationQueue, &simulationClock, &arrivalTimeQueue, &pullQueue]() {
            mainMenuInteractionManager.setAllActiveTo(false);
            simulatorInteractionManager.setAllActiveTo(true);
            windowState = 3; clearQueue(arrivalTimeQueue); clearFile();
            arrivalTimeQueue = generateFromSequence(random::g_scenarioSequence3, pullQueue);
            simulationClock.restart(); os_sim::resetAvg(); os_sim::global::g_finishedPCB.clear(); simulationQueue.clear();
        });
    mainMenuButtonSet[3].setOnClick([&mainMenuButtonSet]() {
		using namespace os_sim;
        std::string newText{};
        switch (g_currentAlgorithm) {
        case Algorithm::FCFS:
            newText = "Shortest Job First";
			g_currentAlgorithm = Algorithm::SJF;
            break;
        case Algorithm::SJF:
            newText = "Round Robin";
            g_currentAlgorithm = Algorithm::RoundRobin;
            break;
        case Algorithm::RoundRobin:
            newText = "Static Priority";
            g_currentAlgorithm = Algorithm::StaticPriority;
            break;
        case Algorithm::StaticPriority:
            newText = "Dynamic Priority";
            g_currentAlgorithm = Algorithm::DynamicPriority;
            break;
        case Algorithm::DynamicPriority:
            newText = "First Come First Served";
            g_currentAlgorithm = Algorithm::FCFS;
            break;
        }
        setAlgorithm(g_currentAlgorithm);
        mainMenuButtonSet[3].setText(newText);
        });
    std::array<interaction::Button, 1> simulatorMenuButtonSet{
        interaction::Button{
        simulatorInteractionManager,
        sf::Vector2f{140.f,40.f},
        sf::Text{os_sim::global::g_defaultFont, "<< Volver", 16},
		sf::Vector2f{40.f,520.f}
        }
    };
    simulatorMenuButtonSet[0].setOnClick([&windowState, &simulatorInteractionManager, &mainMenuInteractionManager]() {
        simulatorInteractionManager.setAllActiveTo(false);
        mainMenuInteractionManager.setAllActiveTo(true);
        windowState = 0;
        });
    // -- Main Loop

    // Clock
	
	
    while (window.isOpen())
    {
        // Prep on every loop
        sf::Vector2i mouseWorld{ window.mapPixelToCoords(sf::Mouse::getPosition(window)) };
        // Tick handle
        if (windowState == 0) mainMenuInteractionManager.update(mouseWorld);
        else simulatorInteractionManager.update(mouseWorld);
		// State Handle
        switch (windowState)
        {
        case 0:
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            // Simulation
            if (!arrivalTimeQueue.empty()) {
                if (simulationClock.getElapsedTime() > arrivalTimeQueue.front()) {
					os_sim::PCB block = pullQueue.front();
                    block.processClock.restart();
                    g_scheduler->returnToQueue(block, simulationQueue);
                    pullQueue.pop_front();
					arrivalTimeQueue.pop();
                }
            }

            

            if (os_sim::simulate(simulationQueue)) endSimulation(simulationClock);
            break;
        default:
            break;
        }

        // Event Handle
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            if (windowState == 0) mainMenuInteractionManager.handleEvent(event.value(), mouseWorld);
            else simulatorInteractionManager.handleEvent(event.value(), mouseWorld);
        }
        
		

        // Render
        window.clear(sf::Color::Black);

        if (windowState == 0) {
            drawText(window, sf::Text{ os_sim::global::g_defaultFont, "Simulador del Planificador de Procesos", 30 }, { 350.f, 80.f });
            drawText(window, sf::Text{ os_sim::global::g_defaultFont, "Algoritmo", 20 }, { 180.f, 450.f }, sf::Color::Yellow);
            for (auto& btn : mainMenuButtonSet) window.draw(btn);
        }
        else {
            visual.draw(simulationQueue);
            for (auto& btn : simulatorMenuButtonSet) window.draw(btn);
            drawText(window, sf::Text{ os_sim::global::g_defaultFont, "Tiempo Total: ", 22 }, { 850.f, 530.f });
            drawText(window, sf::Text{ os_sim::global::g_defaultFont, std::to_string(simulationClock.getElapsedTime().asSeconds()) + "s", 22 }, { 1050.f, 530.f }, sf::Color::Cyan);

            switch (windowState) {
            case 1:
                drawDetail(window, random::g_scenarioDetail1, { 20.f, 100.f });
                break;
            case 2:
                drawDetail(window, random::g_scenarioDetail2, { 20.f, 100.f });
                break;
            case 3:
                drawDetail(window, random::g_scenarioDetail3, { 20.f, 100.f });
                break;
            default:
                break;
            }
        }
        window.display();
    }
#endif // OS_GRAPH
    
}

// ================================================ FUNCTION DEFINITIONS ============================================================================================
void drawDetail(sf::RenderTarget& target, const random::SequenceDetail& detail, const sf::Vector2f& pos) {
    std::stringstream ss{};
    ss << detail.amount << "\n[" << detail.switchesRange.first << "~" << detail.switchesRange.second << "]\n[" << detail.priorityRange.first << "~" << detail.priorityRange.second << "]";
    drawText(target, sf::Text{ os_sim::global::g_defaultFont, "Detalles del Escenario", 20 }, pos + sf::Vector2f{}, sf::Color::Green);
    drawText(target, sf::Text{ os_sim::global::g_defaultFont, "# Procesos:\nRango de Cambios de Burst:\nRango de Prioridad:", 15 }, pos + sf::Vector2f{ 0.f,25.f });
    drawText(target, sf::Text{ os_sim::global::g_defaultFont, ss.str(), 15}, pos + sf::Vector2f{220.f,25.f}, sf::Color::Cyan); ss.str("");
    drawText(target, sf::Text{ os_sim::global::g_defaultFont, "Rango de Prioridad:\nTiempo Total * Proceso:\nFactor de Tiempo de CPU:", 15 }, pos + sf::Vector2f{ 300.f,25.f });
	ss << "[" << detail.priorityRange.first << '~' << detail.priorityRange.second << "]\n[" << detail.totalTimeRange.first << "~" << detail.totalTimeRange.second << "]\n" << detail.boundnessFactor;
    drawText(target, sf::Text{ os_sim::global::g_defaultFont, ss.str(), 15 }, pos + sf::Vector2f{ 500.f,25.f }, sf::Color::Cyan); ss.str("");
    drawText(target, sf::Text{ os_sim::global::g_defaultFont, "Intervalo de Arribo:\nFactor de Arribo:", 15 }, pos + sf::Vector2f{ 600.f,25.f });
    ss << detail.arrivalInterval << "ms\n" << detail.arrivalFactor;
    drawText(target, sf::Text{ os_sim::global::g_defaultFont, ss.str(), 15 }, pos + sf::Vector2f{ 780.f,25.f }, sf::Color::Cyan);
}

void clearQueue(std::queue<sf::Time>& q) {
    std::queue<sf::Time> empty;
    std::swap(q, empty);
}

void setAlgorithm(const os_sim::Algorithm& algorithm) {
	using os_sim::global::g_scheduler;
    switch (algorithm) {
    case os_sim::Algorithm::FCFS:               g_scheduler = std::make_unique<FCFSScheduler>();        break;
    case os_sim::Algorithm::SJF:                g_scheduler = std::make_unique<SJFScheduler>();         break;
    case os_sim::Algorithm::RoundRobin:         g_scheduler = std::make_unique<RRScheduler>();          break;
    case os_sim::Algorithm::StaticPriority:     g_scheduler = std::make_unique<SPriorityScheduler>();   break;
    case os_sim::Algorithm::DynamicPriority:    g_scheduler = std::make_unique<DPriorityScheduler>();   break;
    }
}

void endSimulation(sf::Clock& simClock) {
    using namespace os_sim::global;
    {
        std::ofstream outFile("data/results.csv", std::ios::app);
		outFile << "Avg," << gr_turnaroundTimeAvg / g_finishedProcesses << "," << gr_waitingTimeAvg / g_finishedProcesses << "," << gr_responseTimeAvg / g_finishedProcesses << ",\n";
        outFile << "ThroughPut," << os_sim::global::g_finishedProcesses / simClock.getElapsedTime().asSeconds() << "/s,,,";
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
    simClock.stop();
}

void loadFromCSV(const std::string& filename) {
    using namespace os_sim;

    std::ifstream file(filename);
    if (!file.is_open()) return;

    std::string line;
    // skip header
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string var, val;

        if (!std::getline(ss, var, ',')) continue;
        if (!std::getline(ss, val, ',')) continue;

        // Trim spaces
        var.erase(0, var.find_first_not_of(" \t"));
        var.erase(var.find_last_not_of(" \t") + 1);
        val.erase(0, val.find_first_not_of(" \t"));
        val.erase(val.find_last_not_of(" \t") + 1);

        // Assign values
        if (var == "g_currentAlgorithm") {
            if (val == "FCFS") global::g_currentAlgorithm = Algorithm::FCFS;
            else if (val == "SJF") global::g_currentAlgorithm = Algorithm::SJF;
            else if (val == "RoundRobin") global::g_currentAlgorithm = Algorithm::RoundRobin;
            else if (val == "StaticPriority") global::g_currentAlgorithm = Algorithm::StaticPriority;
            else if (val == "DynamicPriority") global::g_currentAlgorithm = Algorithm::DynamicPriority;
        }
        else if (var == "g_rrTimeQuantum") {
            int ms{ std::stoi(val) };
            global::g_rrTimeQuantum = sf::milliseconds(ms);
        }
        else if (var == "g_agingInterval") {
            int ms{ std::stoi(val) };
            global::g_agingInterval = sf::milliseconds(ms);
        }
    }

    file.close();
}

void clearFile() {
    std::ofstream clearFile("data/results.csv", std::ios::trunc);
    clearFile << "PID,TurnaroundTime,WaitingTime,ResponseTime,IO Time\n";
    clearFile.close();
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

std::queue<sf::Time> generateFromSequence(const std::vector<random::Sequence>& source, std::list<os_sim::PCB>& pcbQueue) {
    pcbQueue.clear();
	std::queue<sf::Time> arrivals{};
    for (auto& sequence : source) {
        std::queue<int> cpuTemp{};
        std::queue<int> ioTemp{};
		arrivals.push(sf::milliseconds(sequence.arrivalTime));
        for (auto& value : sequence.times) {
            if (cpuTemp.size() == ioTemp.size()) cpuTemp.push(value);
            else ioTemp.push(value);
        }

		os_sim::createPCB(cpuTemp, ioTemp, sequence.priority, pcbQueue);
    }
    return arrivals;
}

std::string generateUniqueFilename(const std::string& prefix, const std::string& extension) {
    namespace fs = std::filesystem;

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::tm tm_struct;
    localtime_s(&tm_struct, &in_time_t);

    std::stringstream ss;
    ss << prefix << "_"
        << std::put_time(&tm_struct, "%Y-%m-%d_%H-%M-%S")
        << "-" << std::setw(3) << std::setfill('0') << ms.count()
        << extension;
    return ss.str();
}

void drawText(sf::RenderTarget& target, const sf::Text& text, const sf::Vector2f& pos, const sf::Color& color, bool centered) {
    sf::Text displayText{ text };
	displayText.setFillColor(color);
    if (centered) {
        sf::FloatRect bounds = displayText.getLocalBounds();
        displayText.setOrigin(bounds.getCenter());
    }
    displayText.setPosition(pos);
    target.draw(displayText);
}
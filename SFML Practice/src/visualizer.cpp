#include "visualizer.h"
#include <iomanip>
#include <sstream>

using namespace os_sim;

Visualizer::Visualizer(sf::RenderWindow& window)
    : m_window(window)
{
    if (m_font.openFromFile("resources/ARIAL.TTF")) {

    }

}

void Visualizer::draw(const std::list<PCB>& readyQueue)
{
    // Title
    drawText("Simulador del Planificador de Procesos", 20.f, 20.f, 26, sf::Color::White);
    std::string algorithmName{};
    switch (global::g_currentAlgorithm) {
    case Algorithm::FCFS:
        algorithmName = "First-Come, First-Served (FCFS)"; break;
    case Algorithm::SJF:
        algorithmName = "Shortest Job First (SJF)"; break;
    case Algorithm::RoundRobin:
        algorithmName = "Round Robin (RR)"; break;
    case Algorithm::StaticPriority:
        algorithmName = "Static Priority"; break;
    case Algorithm::DynamicPriority:
        algorithmName = "Dynamic Priority"; break;
    }
    drawText(algorithmName, 20.f, 68.f, 20, sf::Color::Yellow);

    // Ready queue (upper section)
    drawQueue(readyQueue, sf::Vector2f{ 40.f, 240.f }, sf::Color(60, 100, 180), "Cola de Listos");

    // Waiting queue (middle section)
    drawQueue(global::g_pcbWaitLine, sf::Vector2f{ 40.f, 340.f }, sf::Color(160, 100, 40), "Cola de En Espera");

    // Finished PCBs (low section)
    drawQueue(global::g_finishedPCB, sf::Vector2f{ 40.f, 440.f }, sf::Color(255, 38, 0), "Terminados");

    // CPU block (top-right)
    drawCPUBlock();

    // Quantum timer (Round Robin only)
    if (global::g_currentAlgorithm == Algorithm::RoundRobin) {
        drawQuantumTimer();
    }
}

// ------------------- TEXT -------------------
void Visualizer::drawText(const std::string& str, float x, float y, unsigned int size, sf::Color color)
{
    sf::Text text(m_font);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition({ x, y });
    m_window.draw(text);
}

// ------------------- READY QUEUE -------------------
void Visualizer::drawQueue(const std::list<PCB>& queue, sf::Vector2f position, sf::Color color, const std::string& label)
{
    drawText(label, position.x, position.y - 35.f, 18, color);

    float x = position.x;
    unsigned int count = 0;

    for (const auto& pcb : queue) {
        if (count++ >= m_renderLimit)
            break;

        sf::Color fillColor{ color }; fillColor.b -= static_cast<uint8_t>(5 * pcb.pid);
        sf::RectangleShape rect({ m_blockSize, m_blockSize });
        rect.setPosition({ x, position.y });
        rect.setFillColor(fillColor);
        rect.setOutlineColor(sf::Color::White);
        rect.setOutlineThickness(2.f);
        m_window.draw(rect);

        std::string pidText = "P" + std::to_string(pcb.pid);
        drawText(pidText, x + 10.f, position.y + 10.f, 16);
        switch (global::g_currentAlgorithm) {
        case Algorithm::SJF:
        {
            std::string auxText = std::to_string(pcb.estimatedBurstTime_s) + "s";
            drawText(auxText, x + 4.f, position.y + 27.f, 9);
        }
        break;
        case Algorithm::DynamicPriority:
        case Algorithm::StaticPriority:
        {
            std::string auxText = std::to_string(pcb.priority);
            drawText(auxText, x + 10.f, position.y + 27.f, 16);
        }
        break;
        }

        x += m_blockSize + m_blockSpacing;
    }
}

// ------------------- WAIT QUEUE -------------------
void Visualizer::drawQueue(const std::vector<PCB>& queue, sf::Vector2f position, sf::Color color, const std::string& label)
{
    drawText(label, position.x, position.y - 35.f, 18, color);

    float x = position.x;
    unsigned int count = 0;

    for (const auto& pcb : queue) {
        if (count++ >= m_renderLimit)
            break;

        sf::RectangleShape rect({ m_blockSize, m_blockSize });
        rect.setPosition({ x, position.y });
        rect.setFillColor(color);
        rect.setOutlineColor(sf::Color::White);
        rect.setOutlineThickness(2.f);
        m_window.draw(rect);

        std::string pidText = "P" + std::to_string(pcb.pid);
        drawText(pidText, x + 10.f, position.y + 10.f, 16);
        switch (global::g_currentAlgorithm) {
        case Algorithm::SJF:
        {
            std::string auxText = std::to_string(pcb.estimatedBurstTime_s) + "s";
            drawText(auxText, x + 4.f, position.y + 27.f, 9);
        }
            break;
        case Algorithm::DynamicPriority:
        case Algorithm::StaticPriority:
        {
            std::string auxText = std::to_string(pcb.priority);
            drawText(auxText, x + 10.f, position.y + 20.f, 16);
        }
        break;
        }

        x += m_blockSize + m_blockSpacing;
    }
}

// ------------------- CPU BLOCK -------------------
void Visualizer::drawCPUBlock()
{
    using namespace os_sim::global;

    sf::Vector2f pos{ 1020.f, 80.f };
    sf::Vector2f size{ 120.f, 120.f };

    sf::RectangleShape cpuRect(size);
    cpuRect.setPosition(pos);
    cpuRect.setFillColor(sf::Color(30, 80, 30));
    cpuRect.setOutlineColor(sf::Color::White);
    cpuRect.setOutlineThickness(3.f);
    m_window.draw(cpuRect);

    drawText("CPU", pos.x + 35.f, pos.y - 25.f, 18, sf::Color::Green);

    if (g_isRunningProcess) {
        sf::RectangleShape procRect({ 60.f, 60.f });
        procRect.setPosition({ pos.x + 30.f, pos.y + 30.f });
        procRect.setFillColor(sf::Color(100, 180, 100));
        procRect.setOutlineColor(sf::Color::White);
        procRect.setOutlineThickness(2.f);
        m_window.draw(procRect);

        drawText("P" + std::to_string(g_PCBCurrentlyRunning.pid),
            pos.x + 50.f, pos.y + 50.f, 16);
    }
    else {
        drawText("Idle", pos.x + 40.f, pos.y + 50.f, 16, sf::Color(200, 200, 200));
    }
}

// ------------------- QUANTUM TIMER -------------------
void Visualizer::drawQuantumTimer()
{
    using namespace os_sim::global;

    sf::Vector2f pos{ 850.f, 40.f };
    sf::Vector2f size{ 120.f, 40.f };

    sf::RectangleShape timerRect(size);
    timerRect.setPosition(pos);
    timerRect.setFillColor(sf::Color::Black);
    timerRect.setOutlineColor(sf::Color::White);
    timerRect.setOutlineThickness(2.f);
    m_window.draw(timerRect);

    float elapsed = g_CPUBurstClock.getElapsedTime().asSeconds();
    float total = g_rrTimeQuantum.asSeconds();
    float remaining = std::max(0.f, total - elapsed);

    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << remaining << "s";

    drawText(ss.str(), pos.x + 25.f, pos.y + 15.f, 18, sf::Color::Cyan);
    drawText("Quantum", pos.x, pos.y - 25.f, 16, sf::Color::Cyan);
}
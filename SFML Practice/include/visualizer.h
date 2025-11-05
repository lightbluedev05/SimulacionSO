#pragma once
#include <SFML/Graphics.hpp>
#include <list>
#include <vector>
#include <string>
#include "os_sim.h"

class Visualizer {
public:
    explicit Visualizer(sf::RenderWindow& window);

    // Draws the current simulation state
    void draw(const std::list<os_sim::PCB>& readyQueue);

private:
    sf::RenderWindow& m_window;
    sf::Font m_font;

    unsigned int m_renderLimit = 20; // Max number of PCBs to draw per queue
    float m_blockSize = 50.f;        // Size of each process block
    float m_blockSpacing = 6.f;     // Spacing between blocks

    // Internal draw helpers
    void drawQueue(
        const std::list<os_sim::PCB>& queue,
        sf::Vector2f position,
        sf::Color color,
        const std::string& label);

    void drawQueue(
        const std::vector<os_sim::PCB>& queue,
        sf::Vector2f position,
        sf::Color color,
        const std::string& label);

    void drawCPUBlock();
    void drawQuantumTimer();
    void drawText(const std::string& str, float x, float y, unsigned int size = 18, sf::Color color = sf::Color::White);
};

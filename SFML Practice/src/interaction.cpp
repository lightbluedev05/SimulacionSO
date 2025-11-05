#include <functional>
#include <iostream>
#include <SFML/Graphics.hpp>

#include "interaction.h"

namespace interaction {
	// ==================================== Interaction CLASS ====================================
	void Interaction::setOnPress(std::function<void(sf::Mouse::Button)> func) { m_onPress = func; }
	void Interaction::setOnRelease(std::function<void(sf::Mouse::Button)> func) { m_onRelease = func; }
	void Interaction::setOnHeld(std::function<void(sf::Mouse::Button)> func) { m_onHeld = func; }
	void Interaction::setOnHover(std::function<void()> func) { m_onHover = func; }
	void Interaction::setOnHoverExit(std::function<void()> func) { m_onHoverExit = func; }
	void Interaction::setAttached(const sf::Transformable& object) { m_attachedObject = &object; };
	void Interaction::setActive(bool active) { m_isActive = active; }
	bool Interaction::getActive() { return m_isActive; }
	void Interaction::setDetectionBox(const sf::Vector2f& size, const sf::Vector2f& offset) { m_detectionBox = size; m_offset = offset; }
	// ----------------- Private Methods -----------------
	bool Interaction::isMouseInBox(const sf::Vector2i& mousePos) {
		sf::FloatRect boxDetection{ m_attachedObject->getPosition() + m_offset,m_detectionBox };
		return boxDetection.contains(static_cast<sf::Vector2f>(mousePos));
	}
	// ----------------- Public Methods -----------------
	void Interaction::handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) {
		if (!m_attachedObject || !m_isActive) return;
		bool ifInsideInteraction{ isMouseInBox(mousePos) };
		// Behaviour
		if (ifInsideInteraction && event.is<sf::Event::MouseButtonPressed>()) {
			auto button = event.getIf<sf::Event::MouseButtonPressed>()->button;
			if (!m_buttonStates[button]) {
				if (m_onPress) m_onPress(button);
				m_buttonStates[button] = true;
			}
		}
		if (event.is<sf::Event::MouseButtonReleased>()) {
			auto button = event.getIf<sf::Event::MouseButtonReleased>()->button;
			if (m_buttonStates[button]) {
				if (m_onRelease) m_onRelease(button);
				m_buttonStates[button] = false;
			}
		}
	}
	void Interaction::handleUpdate([[maybe_unused]] const sf::Vector2i& mousePos) {
		if (!m_attachedObject || !m_isActive) return;
		bool ifInsideInteraction{ isMouseInBox(mousePos) };
		// Behaviour
		if (ifInsideInteraction) {
			if (m_onHover) m_onHover();
			if (!m_isHovered) m_isHovered = true;
		}
		if (!ifInsideInteraction && m_isHovered) {
			if (m_onHoverExit) m_onHoverExit();
			m_isHovered = false;
		}
		for (auto& [button, isPressed] : m_buttonStates) {
			if (isPressed && m_onHeld) m_onHeld(button);
		}
	}
	// ----------------- Constructor -----------------
	Interaction::Interaction(const sf::Transformable& object, const sf::Vector2f& size, const sf::Vector2f& offset) : m_attachedObject(&object), m_detectionBox(size), m_offset(offset) {}
	// ==================================== InteractionManager CLASS ====================================
    int InteractionManager::createInteraction(const sf::Transformable& attachedObject, const sf::Vector2f& boxSize, const sf::Vector2f& offset)
    {
        int id = ++m_nextId;
        auto inter = std::make_shared<Interaction>(attachedObject, boxSize, offset);
        m_interactions[id] = inter;
        return id;
    }
    std::shared_ptr<Interaction> InteractionManager::getInteraction(int id) {
        auto it = m_interactions.find(id);
        if (it != m_interactions.end()) return it->second;
        return nullptr;
    }
    void InteractionManager::removeInteraction(int id) { m_interactions.erase(id); }
    void InteractionManager::setActive(int id, bool active) {
        if (m_interactions.contains(id)) m_interactions[id].get()->setActive(active);
    }
	void InteractionManager::setAllActiveTo(bool state) {
		for (auto& [id, inter] : m_interactions) {
			inter->setActive(state);
		}
	}
    void InteractionManager::handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) {
        for (auto& [id,inter] : m_interactions) {
            inter->handleEvent(event, mousePos);
        }
    }
    void InteractionManager::update(const sf::Vector2i& mousePos) {
        for (auto& [id, inter] : m_interactions) {
			if (!inter.get()->getActive()) continue;
			inter.get()->handleUpdate(mousePos);
        }
    }
    void InteractionManager::clear() {
        m_interactions.clear();
        m_nextId = 0;
    }
	// ==================================== Button CLASS ====================================
	Button::Button(InteractionManager& manager, const sf::Vector2f& size, const sf::Text& text, const sf::Vector2f& position)
		: m_manager(&manager), m_buttonText(text)
	{
		// Setup shape & text
		m_buttonShape.setSize(size);
		m_buttonShape.setFillColor(m_normalColor);
		setPosition(position);

		m_buttonText.setFillColor(sf::Color::Black);
		centerText();
		m_interactionId = m_manager->createInteraction(*this, size);

		// Setup default behaviors
		auto inter = m_manager->getInteraction(m_interactionId);
		if (inter) {
			inter->setOnHover([this]() {
				m_buttonShape.setFillColor(m_hoverColor);
				});
			inter->setOnHoverExit([this]() {
				m_buttonShape.setFillColor(m_normalColor);
				});
			inter->setOnHeld([this](sf::Mouse::Button) {
				m_buttonShape.setFillColor(m_pressColor);
				});
			inter->setOnRelease([this](sf::Mouse::Button) {
				m_buttonShape.setFillColor(m_hoverColor);
				if (m_onClick) m_onClick();
				});
		}
	}
	void Button::setColors(sf::Color normal, sf::Color hover, sf::Color press) {
		m_normalColor = normal;
		m_hoverColor = hover;
		m_pressColor = press;
		m_buttonShape.setFillColor(normal);
	}
	void Button::setText(const std::string& str) {
		m_buttonText.setString(str);
		centerText();
	}
	void Button::setOnClick(std::function<void()> func) { m_onClick = std::move(func); }
	std::shared_ptr<Interaction> Button::getInteraction() const {
		return m_manager ? m_manager->getInteraction(m_interactionId) : nullptr;
	}
	int Button::getInteractionId() const { return m_interactionId; }
	void Button::centerText() {
		auto bounds = m_buttonText.getLocalBounds();
		auto size = m_buttonShape.getSize();
		m_buttonText.setOrigin(bounds.getCenter());
		m_buttonText.setPosition(size / 2.f);
	}
	void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		states.transform *= getTransform();
		target.draw(m_buttonShape, states);
		target.draw(m_buttonText, states);
	}
}


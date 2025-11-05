#pragma once

#include <functional>
#include <unordered_map>

#include <SFML/Graphics.hpp>

namespace interaction {
    // ------------------------------------------------- Interaction Class -------------------------------------------------
	class Interaction {
		const sf::Transformable* m_attachedObject{};
		sf::Vector2f m_detectionBox{};
		sf::Vector2f m_offset{};
		bool m_isActive{ true };

		bool m_isHovered{ false };
		std::unordered_map<sf::Mouse::Button, bool> m_buttonStates{};

		std::function<void(sf::Mouse::Button)> m_onPress{};
		std::function<void(sf::Mouse::Button)> m_onHeld{};
		std::function<void(sf::Mouse::Button)> m_onRelease{};
		std::function<void()> m_onHover{};
		std::function<void()> m_onHoverExit{};
	private:
		bool isMouseInBox(const sf::Vector2i& mousePos);
	public:
		void handleEvent(const sf::Event& event, const sf::Vector2i& mousePos);
		void handleUpdate(const sf::Vector2i& mousePos);

		void setOnPress(std::function<void(sf::Mouse::Button)>);
		void setOnHeld(std::function<void(sf::Mouse::Button)>);
		void setOnRelease(std::function<void(sf::Mouse::Button)>);
		void setOnHover(std::function<void()>);
		void setOnHoverExit(std::function<void()>);
		void setAttached(const sf::Transformable& object);
		void setActive(bool active);
		bool getActive();
		void setDetectionBox(const sf::Vector2f& size, const sf::Vector2f& offset = { 0.f,0.f });

		Interaction(const sf::Transformable& object, const sf::Vector2f& size, const sf::Vector2f& offset = { 0.f,0.f });
	};
    // ------------------------------------------------- Interaction Manager Class -------------------------------------------------
	class InteractionManager {
		std::unordered_map<int, std::shared_ptr<Interaction>> m_interactions{};
		int m_nextId{ 0 };
	public:
		int createInteraction(const sf::Transformable& attachedObject, const sf::Vector2f& boxSize, const sf::Vector2f& offset = { 0.f, 0.f });
		std::shared_ptr<Interaction> getInteraction(int id);
		void removeInteraction(int id);
		void handleEvent(const sf::Event& event, const sf::Vector2i& mousePos);
		void update(const sf::Vector2i& mousePos);
		void setAllActiveTo(bool state);
		void setActive(int id, bool state);
		void clear();
	};
    // ------------------------------------------------- Button Class -------------------------------------------------
    class Button : public sf::Drawable, public sf::Transformable {
        sf::RectangleShape m_buttonShape{};
        sf::Text m_buttonText;
        int m_interactionId{};
        InteractionManager* m_manager = nullptr;

        sf::Color m_normalColor{ sf::Color(164, 229, 253)};
        sf::Color m_hoverColor{ sf::Color(222, 246, 255) };
        sf::Color m_pressColor{ sf::Color(59, 115, 135) };

    public:
        Button(InteractionManager& manager, const sf::Vector2f& size, const sf::Text& text, const sf::Vector2f& position = { 0.f, 0.f });
		void setColors(sf::Color normal, sf::Color hover, sf::Color press);
		void setText(const std::string& str);
		void setOnClick(std::function<void()> func);
		std::shared_ptr<Interaction> getInteraction() const;
		int getInteractionId() const;

    private:
        std::function<void()> m_onClick{};
		void centerText();
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    };

}


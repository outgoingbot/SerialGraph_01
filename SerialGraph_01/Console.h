#pragma once

#include "UIElement.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

class TextConsole : public UIElement {
public:
	TextConsole(sf::Vector2f size, sf::Vector2f position);

	void handleInput(const char key);

	void draw(sf::RenderWindow& window);

	void updateDisplayText();

	void update(std::string str);

	UI_State_t updateInteractiveState(inputState_t userInput) override;
	bool isMouseOverRect(sf::Vector2f mousePosition);

	sf::Vector2f getSize();
	void setSize(sf::Vector2f size);
	sf::Vector2f getPosition();
	void setPosition(sf::Vector2f pos);

private:
	sf::Font font;
	sf::RectangleShape _dock;
	sf::Text inputText;
	sf::Text displayText;
	std::string input;
	std::vector<std::string> messages;
};

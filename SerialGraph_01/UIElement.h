#pragma once
// Chungus
#include <stdint.h>
#include "Config.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>


typedef enum {
	UI_STATE_READY = 0x00U,
	UI_STATE_HOVER = 0x01U,
	UI_STATE_CLICK_LEFT = 0x02U,
	UI_STATE_CLICK_RIGHT = 0x04U
} UI_Flags_t;


typedef uint8_t UI_State_t;

class UIElement {
public:
	virtual UI_State_t updateInteractiveState(sf::Vector2i mousePosition) = 0;
	virtual void draw(sf::RenderWindow& window) = 0;

	virtual sf::Vector2f getSize() = 0;
	virtual sf::Vector2f getPosition() = 0;
	
	sf::Vector2f getTextBounds(sf::Text t) {
			sf::FloatRect fr = t.getGlobalBounds();
			return sf::Vector2f(fr.left, fr.top);
	}

	// void addElement(UIElement element);
	bool mouseOverElement(sf::Vector2i mousePosition, sf::Vector2i origin);
/*
private:
	std::vector<UIElement*> elements;
*/
};


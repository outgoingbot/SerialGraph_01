#pragma once
// Chungus
#include <stdint.h>
#include "Config.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>


typedef enum {
	UI_STATE_READY = 0x00U,
	UI_STATE_HOVER = 0x01U,
	UI_STATE_CLICK_LEFT = 0x02U,
	UI_STATE_CLICK_RIGHT = 0x04U,
	UI_STATE_KEYBOARD_LOCK = 0x08U
} UI_Flags_t;

typedef struct {
	bool mouseLeftClick = false;
	bool mouseRightClick = false;
	sf::Vector2f mousePosf;
} mouseState_t;

typedef uint8_t UI_State_t;

class UIElement {
public:
	virtual UI_State_t updateInteractiveState(sf::Vector2i mousePosition);
	virtual void draw(sf::RenderWindow& window) = 0;
	void clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	virtual sf::Vector2f getSize() = 0;
	virtual sf::Vector2f getPosition() = 0;
	
	sf::Vector2f getTextBounds(sf::Text t) {
			sf::FloatRect fr = t.getGlobalBounds();
			return sf::Vector2f(fr.left, fr.top);
	}

	// void addElement(UIElement element);
	bool mouseOverElement(sf::Vector2i mousePosition, sf::Vector2i origin);

protected:
	std::vector<UIElement*> _elements;
	std::vector<UIElement*> _keyboardLockedElements;

private:
	UIElement* interactedElement = nullptr;
	UIElement* lastUpdatedElement = nullptr;

};


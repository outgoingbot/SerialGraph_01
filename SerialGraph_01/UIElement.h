#pragma once
// Chungus
#include <stdint.h>
#include "Config.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

typedef uint8_t UI_State_t;

class UIElement {
public:
	virtual UI_State_t getState(sf::Vector2i mousePosition) = 0;
	virtual void draw() = 0;
};


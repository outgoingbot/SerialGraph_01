#pragma once
#include "Config.h"
#include "UIElement.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>




typedef enum {
	BUTTON_STATE_READY = 0x00U,
	BUTTON_STATE_HOVER = 0x01U,
	BUTTON_STATE_CLICK_LEFT = 0x02U,
	BUTTON_STATE_CLICK_RIGHT = 0x04U
} Button_Flags_t;


class Buttons : public UIElement
{
public:

	Buttons(sf::Vector2f size, sf::Vector2f position, sf::Color color, const char* string, uint8_t(*callback)() = nullptr);
	~Buttons();

	UI_State_t getState(sf::Vector2i mousePosition);

	bool isMouseOverRect(sf::Vector2i mousePosition);

	void draw(sf::RenderWindow& window);

	sf::RectangleShape Button; //create button with wideth,height
	sf::Text text;
	sf::Color _color;


private:

	uint8_t(*buttonCallback)() = nullptr;

};


#pragma once
#include "Config.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "UIElement.h"



typedef enum {
	LABEL_STATE_READY = 0x00U,
	LABEL_STATE_HOVER = 0x01U,
	LABEL_STATE_CLICK_LEFT = 0x02U,
	LABEL_STATE_CLICK_RIGHT = 0x04U
} Label_Flags_t;

//typedef uint8_t Label_State_t;

class Label : public UIElement
{
public:
	Label(unsigned int size, sf::Vector2f position, sf::Color color, const char* string);
	~Label();
	void setText(const char* string);
	void setText(const float f);

	void setPos(sf::Vector2f position);

	UI_State_t getState(sf::Vector2i mousePosition);

	bool isMouseOverRect(sf::Vector2i mousePosition);

	void draw();



private:

	sf::Text text;

	sf::Color _color;
};
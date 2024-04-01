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

#define DEFAULT_TEXT_SIZE 40

class Buttons : public UIElement
{
public:

	Buttons(sf::Vector2f size, sf::Vector2f position, sf::Color color, const char* string, uint8_t(*callback)() = nullptr);

	~Buttons();

	UI_State_t updateInteractiveState(sf::Vector2i mousePosition);

	void draw(sf::RenderWindow& window);

	void setTextSize(unsigned int size);
	void setText(const std::string s);
	void setPosition(sf::Vector2f pos);
	void setSize(sf::Vector2f size);

	sf::Vector2f getSize();
	sf::Vector2f getPosition();

	sf::Text text; //move back to private after i solve the menu item button size issue
	
private:
	
	bool isMouseOverRect(sf::Vector2i mousePosition);
	sf::RectangleShape buttonRectangle; //create button with wideth,height
	
	sf::Color _color;
	sf::Font _font;
	uint8_t(*buttonCallback)() = nullptr;
};
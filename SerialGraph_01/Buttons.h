#pragma once
#include "Config.h"
#include "UIElement.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>


typedef bool isDragable_t;

typedef bool isToggle_t;

typedef enum {
	BUTTON_STATE_READY = 0x00U,
	BUTTON_STATE_HOVER = 0x01U,
	BUTTON_STATE_CLICK_LEFT = 0x02U,
	BUTTON_STATE_CLICK_RIGHT = 0x04U
} Button_Flags_t;

#define DEFAULT_TEXT_SIZE 20


class Buttons : public UIElement
{
public:
	//Modified for templating
	Buttons(sf::Vector2f size, sf::Vector2f position, sf::Color color, const char* string, isDragable_t isDraggable = false, isToggle_t isToggle = false, uint8_t(*callback)(uint8_t) = nullptr, uint8_t id = 0);

	~Buttons();

	UI_State_t updateInteractiveState(inputState_t userInput);

	void draw(sf::RenderWindow& window);

	void setTextSize(unsigned int size);
	void setText(const std::string s);
	void setPosition(sf::Vector2f pos);
	void setSize(sf::Vector2f size);

	sf::Vector2f getSize();
	sf::Vector2f getPosition();

	sf::Text text; //move back to private after i solve the menu item button size issue
	
private:
	
	bool isMouseOverRect(sf::Vector2f mousePosition);
	sf::RectangleShape buttonRectangle; //create button with wideth,height
	
	sf::Color _color;
	sf::Font _font;
	uint8_t(*buttonCallback)(uint8_t) = nullptr;
	uint8_t _id; // keep track of multiple buttons in callback within common parent
	bool _mouseLeftHandled = false;
	bool _isDragable = false;
	bool _isToggle = false;
	bool _toggleState = false;
};
#pragma once

#include "Config.h"
#include "UIElement.h"
#include "Buttons.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>

class Menu : public UIElement {

public:
	Menu(sf::Vector2f size, sf::Vector2f position, sf::Color color, const char* string, uint8_t(*callback)(uint8_t)=nullptr);
	~Menu();

	//bool setDockingPosition(sf::Vector2f pos);
	bool setTextOriginPoint(sf::Vector2f pos);
	void setBackgroundColor(sf::Color color);
	void showMenu();
	void hideMenu();
	void toggleMenuShown();
	void showMenuBounds();
	void hideMenuBounds();
	void toggleMenuBounds();
	void showComponentOutlines();
	void hideComponentOutlines();
	void toggleComponentOutlines();
	bool addMenuItem(const std::string text);
	//void setFontSize(int fontSize);

	void draw(sf::RenderWindow& win);
	UI_State_t updateInteractiveState(inputState_t userInput);
	sf::Vector2f getSize();
	sf::Vector2f getPosition();
	void setPosition(sf::Vector2f pos);

private:
	// ------------------------------------------------------
	// Constants
	// formatting
	
	 sf::Font _font;
	const sf::Vector2f DEFAULT_DOCKING_POSITION = { 0,0 };
	const sf::Vector2f DEFAULT_TEXT_ORIGIN_POINT = { 0,0 };
	const sf::Vector2f DEFAULT_PADDING = { 25, 25 };
	const int DEFAULT_COMPONENT_BUFFER = 10;
	const int DEFAULT_CHAR_SIZE_TITLE = 40;
	const int DEFAULT_CHAR_SIZE_ITEM = 20;
	const float DEFAULT_COMP_OUTLINE_PADDING = 1;	//percentage relative to component height


	bool menuShown;	//when enabled, menu is drawn every frame to the window (false by default)
	bool componentOutlinesShown;	//when enabled, draws outlines around UI elements to the screen
	bool menuBoundsShown;	//draws an outline around the whole menu

	// formatting
	sf::Vector2f dockPosition;	//where to start drawing elements relative to the window
	sf::Vector2f _dockSize;
	sf::Vector2f textOriginPoint;	//which corner to start drawing text objects at.
	sf::Vector2f bounds;	//width and height of the menu
	float paddingX;	//amount of space between edge of menu and start of UI
	float paddingY;	//amount of space between edge of menu and start of UI elements
	int componentBuffer;	//space between menu components
	float compOutlinePadding;	//space between menu components and their outline objects
	int numElements;	//number of total menu items
	uint8_t(*menuCallback)(uint8_t) = nullptr;
	
	sf::RectangleShape* _dock = nullptr; //the object for drawing the menu's background
	sf::Color _dockColor;	//set to transparent by default
	sf::Vector2f _dockOpenSize;
	sf::Vector2f _dockClosedSize;
	Buttons* titleItem = nullptr;
	//std::vector <sf::Drawable*> _drawable;

};
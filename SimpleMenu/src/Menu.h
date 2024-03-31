#pragma once

#include "Config.h"
#include "UIElement.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>

class Menu : public UIElement {

public:
	std::vector<const sf::Text*> Text_Array;
	std::vector< sf::RectangleShape*> Text_Array_Outline;
	
	sf::Text text;
	sf::RectangleShape textOutline;

	Menu();

	// Add other constructors? #improve

	~Menu();

	bool setDockingPosition(sf::Vector2f pos);
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
	bool addMenuItem(sf::RenderWindow& win, const std::string text);


	void draw(sf::RenderWindow& win);
	UI_State_t getState(sf::Vector2i mousePosf);

private:
	// ------------------------------------------------------
	// Constants
	// formatting
	
	 sf::Font font;
	const sf::Vector2f DEFAULT_DOCKING_POSITION = { 0,0 };
	const sf::Vector2f DEFAULT_TEXT_ORIGIN_POINT = { 0,0 };
	const sf::Vector2f DEFAULT_PADDING = { 25, 25 };
	const int DEFAULT_COMPONENT_BUFFER = 10;
	const int DEFAULT_CHAR_SIZE = 60;
	const float DEFAULT_COMP_OUTLINE_PADDING = 1;	//percentage relative to component height


	bool menuShown;	//when enabled, menu is drawn every frame to the window (false by default)
	bool componentOutlinesShown;	//when enabled, draws outlines around UI elements to the screen
	bool menuBoundsShown;	//draws an outline around the whole menu

	// formatting
	sf::Vector2f dockingPosition;	//where to start drawing elements relative to the window
	sf::Vector2f textOriginPoint;	//which corner to start drawing text objects at.
	sf::Vector2f bounds;	//width and height of the menu
	float paddingX;	//amount of space between edge of menu and start of UI
	float paddingY;	//amount of space between edge of menu and start of UI elements
	int componentBuffer;	//space between menu components
	float compOutlinePadding;	//space between menu components and their outline objects
	int numElements;	//number of total menu items
	
	sf::RectangleShape background; //the object for drawing the menu's background
	sf::RectangleShape outline;	//the object for drawing the bounds of the menu

	// misc
	sf::Color backgroundColor;	//set to transparent by default
	sf::Color textOutlineColor;	//set to transparent by default
	sf::Color textColor;	//set to transparent by default


	// ------------------------------------------------------
	// Functions

	bool isMouseOverRect(sf::Vector2i mousePosition,  sf::RectangleShape *rect);
};
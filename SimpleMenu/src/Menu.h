#pragma once

#include "Config.h"
#include "UIElement.h"
#include "Buttons.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>

#define MENU_ITEM_SHIFT 20
#define MENU_DEFUALT_COLOR 30,30,30

template<typename T> class Menu : public UIElement {

public:
	using MemberFunctionPtr = uint8_t(T::*)(uint8_t); // Member function pointer type
	//Menu(sf::Vector2f size, sf::Vector2f position, sf::Color color, const char* string, uint8_t(*callback)(uint8_t)=nullptr);
	//Menu::Menu(sf::Vector2f size, sf::Vector2f position, sf::Color color, const char* string, uint8_t(*callback)(uint8_t)) {
	Menu<T>(sf::Vector2f size, sf::Vector2f position, sf::Color color, const char* string, T* instance, MemberFunctionPtr func) : instance(instance), func(func) {
		// setup the dock (large rectangle that borders the entire menu. grows with addItems pushback
		_dock = new sf::RectangleShape();
		_dockColor = color;
		_dockSize = size;
		_dock->setSize(_dockSize);
		_dock->setPosition(position);
		_dock->setFillColor(_dockColor);

		_dockOpenSize.x = _dockSize.x;//setting the dock a bit bigger so I can see it for now
		_dockOpenSize.y = _dockSize.y;
		_dockClosedSize = _dockOpenSize;
		_dock->setSize(_dockClosedSize);

		//menuCallback = callback;
		//title of drop down menu. will activate on state= hover.

		titleItem = new Buttons(sf::Vector2f(_dockSize.x, _dockSize.y), position, sf::Color(MENU_DEFUALT_COLOR), string, false, false, instance, func, 0);
		_elements.push_back(titleItem);
		_menuItems.push_back(titleItem);

		//will change this to false when i get titleItem = hover working
		menuShown = true;
		componentOutlinesShown = false;
	}

	~Menu() {
		//Destruct
	}

	////bool setDockingPosition(sf::Vector2f pos);
	//bool addMenuItem(const std::string text);
	bool addMenuItem(const std::string text) {
	#define SPACING 10
		numElements++;

		//I dont like this constructor. need to think about either a default constructor. will need my call back functions
		Buttons<T>* newItem = new Buttons(sf::Vector2f(20, 20), sf::Vector2f(20, 20), sf::Color(MENU_DEFUALT_COLOR), "Empty", false, true, instance, func, _elements.size());
		_elements.push_back(newItem);
		_menuItems.push_back(newItem);

		newItem->setTextSize(DEFAULT_CHAR_SIZE_ITEM);
		newItem->setText(text);

		//need to get the Button text bounding box (based off the setText) to then set the button size;
		sf::FloatRect fr = newItem->text.getGlobalBounds();
		float newItemHeight = fr.height + SPACING;
		float newItemWidth = _dockSize.x - MENU_ITEM_SHIFT;
		newItem->setSize(sf::Vector2f(newItemWidth, newItemHeight));


		//index element [0] on first pass to po
		newItem->setPosition(sf::Vector2f(_dock->getPosition().x + MENU_ITEM_SHIFT, _elements[_elements.size() - 2]->getPosition().y + _elements[_elements.size() - 2]->getSize().y + SPACING));

		//Some issue here indexing through _elements or its some algrbra
		_dockOpenSize.y = _elements[_elements.size() - 1]->getPosition().y + _elements[_elements.size() - 1]->getSize().y - this->getPosition().y;
		return true;
	}


	//UI_State_t updateInteractiveState(inputState_t userInput);
	UI_State_t updateInteractiveState(inputState_t userInput) {
		UI_State_t returnVal = UI_STATE_READY;

		// Expand menu when mouse hovers
		if (this->mouseOverElement(userInput.m.mousePosf, sf::Vector2f(0, 0))) componentOutlinesShown = true;
		else componentOutlinesShown = false;

		if (componentOutlinesShown) {
			_dock->setSize(_dockOpenSize);
		}
		else {
			_dock->setSize(_dockClosedSize);
		}

		// Call parent updateInteractiveState to evaluate children states
		returnVal |= UIElement::updateInteractiveState(userInput);

		return returnVal;
	}


	//void draw(sf::RenderWindow& win);
	void draw(sf::RenderWindow& win)
	{
		if (menuShown = 1) {
			win.draw(*_dock);
			if (componentOutlinesShown) { // List is exanded
				for (auto Items : _elements) Items->draw(win);
			}
			else {
				titleItem->draw(win); // Draw only the title item if list is collapsed
			}
		}
	}

	//bool setTextOriginPoint(sf::Vector2f pos);
	//void setBackgroundColor(sf::Color color);
	//void showMenu();
	//void hideMenu();
	//void toggleMenuShown();
	//void showMenuBounds();
	//void hideMenuBounds();
	//void toggleMenuBounds();
	//void showComponentOutlines();
	//void hideComponentOutlines();
	//void toggleComponentOutlines();
	////void setFontSize(int fontSize);
	//sf::Vector2f getSize();
	//void setSize(sf::Vector2f size);
	//sf::Vector2f getPosition();
	//void setPosition(sf::Vector2f pos);

	sf::Vector2f getSize() {
		return _dock->getSize();
	}

	void setSize(sf::Vector2f size) {
		//ToDo: implement setting the size
	}

	sf::Vector2f getPosition() {
		return _dock->getPosition();
	}

	void setPosition(sf::Vector2f pos) {
		_dock->setPosition(pos);
		_menuItems[0]->setPosition(pos); //_menuItems does not get swapped with handleinteravtiveUIState()
		for (int i = 1; i < _menuItems.size(); i++) {
			_menuItems[i]->setPosition(sf::Vector2f(_dock->getPosition().x + MENU_ITEM_SHIFT, _menuItems[i - 1]->getPosition().y + _menuItems[i - 1]->getSize().y + SPACING));
		}
	}


	void toggleMenuShown() {
		if (!menuShown) {
			menuShown = true;
		}
		else {
			menuShown = false;
		}
	}

	//bool Menu::setDockingPosition(sf::Vector2f pos)
	//{
	//	_dock->setPosition(pos);
	//
	//	//for (auto element : _elements) element->getPosition;;.setPosition(sf::Vector2f(_dock->getPosition().x + MENU_ITEM_SHIFT, _elements[_elements.size() - 2]->getPosition().y + _elements[_elements.size() - 2]->getSize().y + SPACING));
	//
	//	return true;
	//}

	bool setTextOriginPoint(sf::Vector2f pos)
	{
		//text.setPosition(pos);
		return true;
	}


	void setBackgroundColor(sf::Color color)
	{
		_dockColor = color;
		_dock->setFillColor(_dockColor);
	}

	void showMenu()
	{
		menuShown = true;
	}

	void hideMenu()
	{
		menuShown = false;
	}

	void showMenuBounds()
	{
		menuBoundsShown = true;
	}

	void hideMenuBounds()
	{
		menuBoundsShown = false;
	}

	void toggleMenuBounds()
	{
		if (menuBoundsShown) {
			menuBoundsShown = false;
		}
		else {
			menuBoundsShown = true;
		}
	}

	void showComponentOutlines()
	{
		componentOutlinesShown = true;
	}

	void hideComponentOutlines()
	{
		componentOutlinesShown = false;
	}

	void toggleComponentOutlines()
	{
		if (componentOutlinesShown) {
			componentOutlinesShown = false;
		}
		else {
			componentOutlinesShown = true;
		}
	}


private:
	// ------------------------------------------------------
	// Constants
	// formatting
	
	T* instance;
	MemberFunctionPtr func; // Pointer to a member function of class T

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
	//uint8_t(*menuCallback)(uint8_t) = nullptr;
	
	sf::RectangleShape* _dock = nullptr; //the object for drawing the menu's background
	sf::Color _dockColor;	//set to transparent by default
	sf::Vector2f _dockOpenSize;
	sf::Vector2f _dockClosedSize;
	Buttons<T>* titleItem = nullptr;
	std::vector <UIElement*> _menuItems;

};
#pragma once
#include "Config.h"
#include "UIElement.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <functional> //used for templating


typedef bool isDragable_t;

typedef bool isToggle_t;

typedef enum {
	BUTTON_STATE_READY = 0x00U,
	BUTTON_STATE_HOVER = 0x01U,
	BUTTON_STATE_CLICK_LEFT = 0x02U,
	BUTTON_STATE_CLICK_RIGHT = 0x04U
} Button_Flags_t;

#define DEFAULT_TEXT_SIZE 20

template<typename T>
class Buttons : public UIElement
{
public:
	using MemberFunctionPtr = uint8_t (T::*)(uint8_t); // Member function pointer type

	//void callMemberFunction(T* instance, void (T::*func)(int), int arg) {
	//	(instance->*func)(arg); // Call the member function on the instance
	//}
	//Buttons(sf::Vector2f size, sf::Vector2f position, sf::Color color, const char* string, isDragable_t isDraggable = false, isToggle_t isToggle = false, uint8_t(*callback)(uint8_t) = nullptr, uint8_t id = 0);
	//B(T* instance, MemberFunctionPtr func) : instance(instance), func(func) {}
	//Buttons(sf::Vector2f size, sf::Vector2f position, sf::Color color, const char* string, isDragable_t isDragable, isToggle_t isToggle, T* instance, MemberFunctionPtr func, uint8_t id = 0) : instance(instance), func(func) {}
	Buttons<T>(sf::Vector2f size, sf::Vector2f position, sf::Color color, const char* string, isDragable_t isDragable, isToggle_t isToggle, T* instance, MemberFunctionPtr func, uint8_t id) : instance(instance), func(func) {
		//Load private Font
		if (!_font.loadFromFile("../res/arial.ttf")) {
			printf("Error loading Font");
			system("pause");
		}

		_color = color;
		buttonRectangle.setSize(size);

		//sf::FloatRect btn = buttonRectangle.getLocalBounds();
		//buttonRectangle.setOrigin(btn.width / 2, btn.height / 2);
		buttonRectangle.setPosition(sf::Vector2f(position.x, position.y));
		buttonRectangle.setFillColor(_color);

		text.setFont(_font);
		text.setString(string);
		text.setCharacterSize(DEFAULT_TEXT_SIZE);
		//sf::FloatRect rc = text.getLocalBounds();
		//text.setOrigin(rc.width / 2, rc.height / 2);
		text.setPosition(position);
		text.setFillColor(sf::Color::White);
		
		//buttonCallback = callback;
		_id = id;
		_isDragable = isDragable;
		_isToggle = isToggle;
	}

	~Buttons() {

	}
	//template<typename T>
	//Buttons<T>::~Buttons() {
	//	//delete rectangle shape and text. needed?
	//}

	
	UI_State_t updateInteractiveState(inputState_t userInput) {
		UI_State_t returnVal = BUTTON_STATE_READY;
		buttonRectangle.setFillColor(sf::Color::Yellow);

		returnVal |= BUTTON_STATE_HOVER;
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !_mouseLeftHandled) {
			if (_isToggle) _toggleState = !_toggleState;
			//std::cout << _toggleState << std::endl;

			buttonRectangle.setFillColor(sf::Color(255, 0, 255));
			returnVal |= BUTTON_STATE_CLICK_LEFT;
			//Todo: Handle the button callback
			//if (buttonCallback != nullptr) buttonCallback(_id);
			//std::invoke(func, instance, 0)
			(instance->*func)(10);
			_mouseLeftHandled = true; // Prevents button from being spammed when mouse is held down
		}
		else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) && _mouseLeftHandled == true) {
			_mouseLeftHandled = false;
		}

		//move the object
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && _isDragable) {
			buttonRectangle.setPosition(sf::Vector2f(userInput.m.mousePosf.x - buttonRectangle.getSize().x / 2, userInput.m.mousePosf.y - buttonRectangle.getSize().y / 2));
			text.setPosition(buttonRectangle.getPosition());
			returnVal |= BUTTON_STATE_CLICK_RIGHT;
			//while (sf::Mouse::isButtonPressed(sf::Mouse::Left));
		}
		else {
		}

		if (this->isMouseOverRect(userInput.m.mousePosf)) {
			text.setFillColor(sf::Color::Black);
			buttonRectangle.setFillColor(sf::Color::Yellow); // highlight
		}
		else {
			if (_isToggle && !_toggleState) {
				text.setFillColor(sf::Color::White);
				buttonRectangle.setFillColor(_color); // constructor color
			}
			else if (!_isToggle) {
				text.setFillColor(sf::Color::White);
				buttonRectangle.setFillColor(_color); // constructor color
			}
		}

		// Call parent updateInteractiveState to evaluate children states
		returnVal |= UIElement::updateInteractiveState(userInput);

		return returnVal;
	}

	
	void draw(sf::RenderWindow& window) {
		window.draw(buttonRectangle);
		window.draw(text);
	}

	
	void setTextSize(unsigned int size) {
		text.setCharacterSize(size);
	}

	
	void setPosition(sf::Vector2f pos) {
		text.setPosition(pos);
		buttonRectangle.setPosition(pos);
	}

	
	void setText(const std::string s) {
		text.setString(s);
	}

	
	void setSize(sf::Vector2f size) {
		buttonRectangle.setSize(size);
	}

	
	sf::Vector2f getSize() {
		return buttonRectangle.getSize();
	}

	
	sf::Vector2f getPosition() {
		return buttonRectangle.getPosition();
	}



	sf::Text text; //move back to private after i solve the menu item button size issue

private:
	T* instance;
	MemberFunctionPtr func; // Pointer to a member function of class T

	
	bool isMouseOverRect(sf::Vector2f mousePosition) {
		if (mousePosition.x > buttonRectangle.getPosition().x && mousePosition.x < buttonRectangle.getPosition().x + (buttonRectangle.getSize().x)) {
			if (mousePosition.y > buttonRectangle.getPosition().y
				&& mousePosition.y < buttonRectangle.getPosition().y + (buttonRectangle.getSize().y)) {
				return true;
			}
		}
		return false;
	}

	sf::RectangleShape buttonRectangle; //create button with wideth,height

	sf::Color _color;
	sf::Font _font;
	//uint8_t(*buttonCallback)(uint8_t) = nullptr;
	uint8_t _id; // keep track of multiple buttons in callback within common parent
	bool _mouseLeftHandled = false;
	bool _isDragable = false;
	bool _isToggle = false;
	bool _toggleState = false;
};
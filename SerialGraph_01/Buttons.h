#pragma once
#include "Config.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
extern sf::RenderWindow window;
extern sf::Font font;

typedef enum {
	STATE_READY = 0x00U,
	STATE_HOVER = 0x01U,
	STATE_CLICK_LEFT = 0x02U,
	STATE_CLICK_RIGHT = 0x04U
} Button_Flags_t;

typedef uint8_t Button_State_t;


class Buttons
{
public:

	Button_State_t& operator+(Button_State_t const& orig) {
		Button_State_t rVal;;
		rVal = (Button_State_t)((int)orig + (int)rVal);
		return rVal;
	}

	Button_State_t& operator=(Button_State_t &orig) {
		Button_State_t rVal;
		
		orig = static_cast<Button_State_t>(orig);
		return orig;
	}

	//Button_State_t& operator+=(Button_State_t &orig) {
	//	//Button_State_t rVal = orig;
	//	orig = static_cast<Button_State_t>(orig);
	//	return orig;
	//}


	Buttons(sf::Vector2f size, sf::Vector2f position, const char* string);
	~Buttons();
	
	//bool isPressed(sf::Vector2i mousePosition);

	bool isMouseOverRect(sf::Vector2i mousePosition);

	Button_State_t getState(sf::Vector2i mousePosition);

	void draw();

	sf::RectangleShape Button; //create button with wideth,height
	sf::Text text;


private:
	


};


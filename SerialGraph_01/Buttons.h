#pragma once
#include "Config.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
extern sf::RenderWindow window;
extern sf::Font font;

class Buttons
{
public:
	Buttons(sf::Vector2f size, sf::Vector2f position, const char* string);
	~Buttons();
	
	bool isPressed(sf::Vector2i mousePosition);

	bool isMouseOverRect(sf::Vector2i mousePosition);

	void draw();

	sf::RectangleShape Button; //create button with wideth,height
	sf::Text text;

private:
	


};


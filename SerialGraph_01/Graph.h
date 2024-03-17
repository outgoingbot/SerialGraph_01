#pragma once
#include "Config.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
extern sf::RenderWindow window;
extern sf::Font font;

#define NUMFLOATS 3

class Graph
{
public:
	Graph(sf::Vector2f size, sf::Vector2f position, const char* title, uint8_t numVars);
	~Graph();

	bool isPressed(sf::Vector2i mousePosition);
	bool isMouseOverRect(sf::Vector2i mousePosition);

	void setScale(float scl);
	void autoScale(bool);
	void update(float *dataPoint, uint8_t len);
	void draw(uint8_t len);

	float scaler = 0.1f;

	sf::RectangleShape frame; //create button with wideth,height

	sf::RectangleShape axis_x; //create button with wideth,height
	const sf::Color dotColor[3] = { sf::Color::Red ,sf::Color::Green ,sf::Color::Blue };
	sf::CircleShape* dot[NUMFLOATS];// = nullptr; //create the dots
	sf::Vertex* lineInterpol[NUMFLOATS];// = nullptr;

	sf::Text text; //just holds the graph title
	//will want to to add some color blocks to inidcate the 3 variables color


private:

	uint32_t frameSamples;
	float* dataArray[NUMFLOATS];


};


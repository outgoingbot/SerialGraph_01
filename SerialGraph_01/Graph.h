#pragma once
#include "Config.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
extern sf::RenderWindow window;
extern sf::Font font;

class Graph
{
public:
	Graph(sf::Vector2f size, sf::Vector2f position, const char* title, uint8_t numVars);
	~Graph();

	bool isPressed(sf::Vector2i mousePosition);
	bool isMouseOverRect(sf::Vector2i mousePosition);

	void setScale(float scl);
	void autoScale(bool);
	void update(float dataPoint, uint8_t len);
	void draw();

	float scaler = 0.1f;

	sf::RectangleShape frame; //create button with wideth,height

	sf::RectangleShape axis_x; //create button with wideth,height

	sf::CircleShape* dot = nullptr; //create the dots
	sf::Vertex* lineInterpol = nullptr;

	sf::Text text;


private:

	uint32_t frameSamples;
	float* dataArray;


};


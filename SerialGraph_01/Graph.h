#pragma once
#include "Config.h"
#include "UIElement.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "UtilFuncs.h"
#include <stdio.h>
#include <vector>
#define NUMFLOATS 3

//typedef enum {
//	GRAPH_STATE_READY = 0x00U,
//	GRAPH_STATE_HOVER = 0x01U,
//	GRAPH_STATE_CLICK_LEFT = 0x02U,
//	GRAPH_STATE_CLICK_RIGHT = 0x04U
//} Graph_Flags_t;


class Graph : public UIElement
{
public:
	Graph(sf::Vector2f size, sf::Vector2f position, const char* title, uint8_t numVars);
	~Graph();

	UI_State_t updateInteractiveState(sf::Vector2i mousePosition) override;

	void setScale(float scl);
	void autoScale(bool);
	bool xAxisMode = false; //true = sample,, false = time


	void update(sf::RenderWindow& window, float *dataPoint=nullptr);
	void draw(sf::RenderWindow& window);
	sf::Vector2f getSize();
	sf::Vector2f getPosition();

private:
	bool isMouseOverRect(sf::Vector2i mousePosition);

	float scaler = 1.0f;

	sf::RectangleShape frame; //create button with wideth,height

	sf::RectangleShape axis_x; //create button with wideth,height
	
	sf::Vertex* lineInterpol[NUMFLOATS]; //should make this dynamic at some point
	sf::Color lineColor[NUMFLOATS];

	sf::Text textLabel; //just holds the graph title
	sf::Text textAxis_y; //holds the max value of Y axis scale

	sf::RectangleShape xMouseCross;//x mouse crosshair
	sf::RectangleShape yMouseCross; //y mouse crosshair
	sf::Text textyMouse; //mouse position related to graph scale
	
	

	char textBuff[64];
	//will want to to add some color blocks to inidcate the 3 variables color

	uint32_t frameSamples; //number of samples per frame (based on frame width)
	float* dataArray[NUMFLOATS];
	int _len; //number of floats per graph

	float maxVal=10;
	float minVal=0;
	
	std::vector <sf::Drawable*> _drawables;
	std::vector <sf::Drawable*> _interactive;

	bool drawCrosshair = false;
	


};
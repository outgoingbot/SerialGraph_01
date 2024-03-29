#pragma once
#include "Config.h"
#include "UIElement.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "UtilFuncs.h"

#define NUMFLOATS 3

typedef enum {
	GRAPH_STATE_READY = 0x00U,
	GRAPH_STATE_HOVER = 0x01U,
	GRAPH_STATE_CLICK_LEFT = 0x02U,
	GRAPH_STATE_CLICK_RIGHT = 0x04U
} Graph_Flags_t;


class Graph : public UIElement
{
public:
	Graph(sf::Vector2f size, sf::Vector2f position, const char* title, uint8_t numVars);
	~Graph();

	bool isMouseOverRect(sf::Vector2i mousePosition);
	
	UI_State_t getState(sf::Vector2i mousePosition);

	void setScale(float scl);
	void autoScale(bool);
	void update(float *dataPoint=nullptr);
	void draw(void);

	float scaler = 0.5f;

	sf::RectangleShape frame; //create button with wideth,height

	sf::RectangleShape axis_x; //create button with wideth,height
	sf::Color dotColor[NUMFLOATS];
	sf::Vertex* lineInterpol[NUMFLOATS];// = nullptr;

	sf::Text text; //just holds the graph title
	//will want to to add some color blocks to inidcate the 3 variables color


private:
	uint32_t frameSamples; //number of samples per frame (based on frame width)
	float* dataArray[NUMFLOATS];
	int _len; //number of floats per graph

	float maxVal=10;
	float minVal=0;

};
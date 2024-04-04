#pragma once
#include "Config.h"
#include "UIElement.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "UtilFuncs.h"
#include <stdio.h>
#include <vector>
#include "Label.h"
#include "SimpleMenu/Src/Menu.h" // Fix later
#include "Buttons.h"
#include <iostream>
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

	static uint8_t handleMenu_1(uint8_t val) {
		printf("Graph Menu_1 Button: %i\r\n", val);
		return 0;
	}

	Graph(sf::Vector2f size, sf::Vector2f position, const char* title, uint8_t numVars);
	~Graph();

	UI_State_t updateInteractiveState(inputState_t userInput) override;

	void setScale(float scl);
	void autoScale(bool);
	bool xAxisMode = false; //true = sample,, false = time


	void update(inputState_t userInput, bool withNewData, float *dataPoint=nullptr);
	void draw(sf::RenderWindow& window);
	sf::Vector2f getSize();
	sf::Vector2f getPosition();
	void setPosition(sf::Vector2f pos);

private:
	bool isMouseOverRect(sf::Vector2f mousePosition);
	//void drawFromInteract(bool withShift);

	bool drawCrosshair = false;
	float _Yscaler = 1.0f; //bigger is taller peaks
	uint32_t _Xscaler = 1; //bigger is more zoomed in
	sf::Font _font;

	sf::RectangleShape _dock; //create button with wideth,height
	sf::RectangleShape _axis_x; //create button with wideth,height
	sf::RectangleShape _xMouseCross;//x mouse crosshair
	sf::RectangleShape _yMouseCross; //y mouse crosshair

	sf::Vertex* _lineInterpol[NUMFLOATS]; //should make this dynamic at some point
	sf::Color _lineColor[NUMFLOATS];

	Label *_graphName; //The graph title
	Label *_textYaxisScale;
	
	Label *_textXaxisScale_start;
	Label *_textXaxisScale_stop;

	Label *_textCrossHairData; //mouse position related to graph scale
	
	Menu *_menu;

	char textBuff[64];
	//will want to to add some color blocks to inidcate the 3 variables color

	uint32_t frameSamples; //number of samples per _dock (based on _dock width)
	float* dataArray[NUMFLOATS];
	int _len; //number of floats per graph

	float _size;
	float maxVal=10;
	float minVal=0;
	
	std::vector <sf::Drawable*> _drawables;
	std::vector <sf::Drawable*> _interactive;
	//std::vector<UIElement*> _elements;

	
};
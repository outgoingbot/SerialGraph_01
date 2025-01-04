#pragma once

#include "UIElement.h"
#include "Config.h"
#include "Buttons.h"
#include "Graph.h"
#include "Label.h"
#include "Console.h"

#include "CircularQueue.h"
#include "RS232Comm/RS232Comm.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>

#include <Windows.h>  
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <iostream>
#include <list>
#include <string>
#include <thread>
#include <chrono>
#include <algorithm>
#include "Menu.h" // Fix later


typedef enum {
	NOTDRAGABLE = 0,
	DRAGABLE = 1
} dragableFlag_t;

typedef enum {
	NOTTOGGLE = 0,
	TOGGLE = 1
} toggleFlag_t;

class SerialScope : public UIElement {
public:
	SerialScope(uint16_t rxBufferSz, int bytesReceived);
	~SerialScope();
	void update(inputState_t userInput);
	
	UI_State_t updateInteractiveState(inputState_t userInput);
	void draw(sf::RenderWindow& win);

	// For now, saying Serial Scope takes up whole window. In future,
	// would be wise to define SerialScope width/height
	sf::Vector2f getSize() {
		return sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT);
	}

	void setSize(sf::Vector2f size) {
		// so something?
	}

	sf::Vector2f getPosition() {
		return sf::Vector2f(0, 0);
	}

	void setPosition(sf::Vector2f) {
		//return something?
	}

	//TODO: These may need to be private and/or may not need to be declared here
	uint8_t handleButton_minimize(uint8_t val);
	uint8_t handleButton_maximize(uint8_t val);
	uint8_t handleButton_connect(uint8_t val);
	uint8_t handleButton_disconnect(uint8_t val);
	uint8_t handleMenu_1(uint8_t val);
	uint8_t handleMenu_2(uint8_t val);

private:
	std::thread *serial_thread = nullptr;
	char* rxBuffer;
	Serial* SP;
	sf::Sprite* sprite;
	std::vector<Graph*> Graph_Vector;

	TextConsole* console; //WIP

	Label* serialText;
	int _bytesReceived;
	//uint8_t handleButton_minimize(uint8_t val);
};


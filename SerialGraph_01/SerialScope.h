#pragma once

#include "UIElement.h"
#include "Config.h"
#include "Buttons.h"
#include "Graph.h"
#include "Label.h"
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
#include "SimpleMenu/Src/Menu.h" // Fix later


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

	sf::Vector2f getPosition() {
		return sf::Vector2f(0, 0);
	}

	void setPosition(sf::Vector2f) {
		//return something?
	}

private:
	std::thread *serial_thread = nullptr;
	char* rxBuffer;
	Serial* SP;
	sf::Sprite* sprite;
	std::vector<Graph*> Graph_Vector;
	Label* serialText;
	int _bytesReceived;

};


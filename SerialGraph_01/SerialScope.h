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

class SerialScope : public UIElement {
public:
	SerialScope(uint16_t rxBufferSz, int bytesReceived);
	~SerialScope();
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

private:
	std::thread *serial_thread = nullptr;
	char* rxBuffer;
	Serial* SP;
	sf::Sprite* sprite;
	std::vector<Graph*> Graph_Vector;
	int _bytesReceived;

};


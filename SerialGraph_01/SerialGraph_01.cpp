/*
Serial Oscilloscope

features to add:
configure comport and baud rate in gui (text entry). connect, disconn, pause buttons <- making some good progress on this
transmit text box and send button
Mouse wheel to change scale of data values (zoom)
x and y axis graduated (scale with zoom)
x axis some sort of accurate time interval (system clock?)
graph keeps scrolling with no data (like an oscope (x axis is time)
Buttons to change expexted data type (int, uint32_t, float, doublem etc)
DTR button would be nice to reboot Arduino based boards
allow multiple data varabiables (channels) buttons to enable and textbox to set a special char ('/n' or something) to direct to each channel (4 or 8 channels?)
unique channel colors

possbily make a single shot to trigger on derivative (slope) of certain value or on a hex code

Have a Binary or ASCII mode

Currenly Generating 3 float vlaue with /n terminating ASCXII string
expected data format: "%f,%f,%f\n"
2000000 Baud (set in RS232Comm.cpp line 46)

next change will allow CSV strings with \r\n terminating



*/

#include "SerialGraph_01.h"
#include <iostream>
#include <list>
#include <string>
#include <thread>
#include <chrono>
#include <algorithm>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>
#include <Windows.h>    
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "RS232Comm/RS232Comm.h"
#include <cmath>

#include "Config.h"
#include "Buttons.h"
#include "Graph.h"
#include "Label.h"
#include "CircularQueue.h"
#include "SerialScope.h"
#include "Console.h"

#include "SimpleMenu/Src/Menu.h"


//Debug vars
char charArrayDebug[256] = "Empty";

//Serial
//uint32_t numSamples = 0; //keep track of the total number of received data (WIP)
//char incomingData[256] = { 0 }; //Serial Rx Buffer
//unsigned int dataLength = 1;
//int bytesReceived = 0;

uint8_t gui_ID = 0;

//graphics
sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Serial O-Scope _sweeded", sf::Style::Default, sf::ContextSettings(32));
//sf::RenderWindow windowSettings(sf::VideoMode(800, 600), "Settings", sf::Style::Default, sf::ContextSettings(32));



//Our Callback Handlers
uint8_t handleButton_1(uint8_t val) {
	printf("Button1 Clicked \r\n");
	return 0;
}

uint8_t handleMenu_1(uint8_t val) {
	printf("Menu_1 Button: %i\r\n", val);
	return 0;
}

uint8_t handleMenu_2(uint8_t val) {
	printf("Menu_2 Button: %i\r\n", val);
	return 0;
}


sf::Event event; //Do we only need one event ? (think yes)



int main()
{	

	//set the Program Window Icon
	auto image = sf::Image{};
	if (!image.loadFromFile("../res/icon.png"))
	{
		printf("Error loading Icon");
		system("pause");
	}
	window.setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());
	
	//Load Font
	sf::Font font;
	if (!font.loadFromFile("../res/Pumpkin_Pancakes.ttf")) {
		printf("Error loading Font");
		system("pause");
	}

	//set FPS
	window.setFramerateLimit(60); //seriously reduces the CPU/GPU utilization
	//window.setVerticalSyncEnabled(true);
	window.setActive(true);

	//create our UI Elements
	SerialScope serialScope(255, 0);
	window.clear(sf::Color(55, 55, 55));

	//debug UI Items (non UIelements)
	Graph D_Graph_loopTime(sf::Vector2f(200, 100), sf::Vector2f(WINDOW_WIDTH - 210, 50), "Loop Time", NUMFLOATS);
	Label D_MousePosition(20, sf::Vector2f(100, 500), sf::Color::White, "MousePosition");	
	
	// SerialGraph::updateInteractiveState()
	//-----------------------------------------MAIN LOOP------------------------------------------------------------
	bool running = true;
	UIElement* interactedElement = nullptr;
	UIElement* lastUpdatedElement = nullptr;

	inputState_t userInput;

	while (running)
	{
		auto startTime = std::chrono::high_resolution_clock::now();

		serialScope.updateInteractiveState(userInput);
		//Get Keyboard inputs
		
		//if (sf::Keybord::isKeyPressed(sf::Keyboard::Right)) sprite.move(10, 0);
		//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) sprite.move(-10, 0);
		//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) sprite.move(0, -10);
		//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) sprite.move(0, 10);
		//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) sprite.scale(sf::Vector2f(sprite.getScale().x+1.0f, sprite.getScale().y + 1.0f));

		
		//Update Mouse Position Even after Windows Resizing
		userInput.m.mousePosf = window.mapPixelToCoords(sf::Mouse::getPosition(window));
		userInput.m.mouseLeftClick = sf::Mouse::isButtonPressed(sf::Mouse::Left);
		userInput.m.mouseRightClick = sf::Mouse::isButtonPressed(sf::Mouse::Right);

		serialScope.update(userInput);
		serialScope.clear();
		serialScope.draw(window);

		D_Graph_loopTime.draw(window); //DEBUG OUTSIDE UIELEMENTS
		D_MousePosition.draw(window);
		
		//console.draw(window); //Testing Console

		window.display(); //show drawn objects to the display buffer
		//Set the keyboard input to Unkown to prevent Spamming
		userInput.k.key = sf::Keyboard::Unknown; // set key to -1
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				//window.close();
				serialScope.~SerialScope(); //deconstruct
				running = false;
			}
			else if (event.type == sf::Event::Resized) {

				glViewport(0, 0, event.size.width, event.size.height);
				// update the view to the new size of the window
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
					sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
					window.setView(sf::View(visibleArea));
				}
				//SP.~Serial(); //deconstruct
			}
			else if (event.type == sf::Event::MouseWheelMoved) {
				// display number of ticks mouse wheel has moved
				//if (event.mouseWheel.delta > 0) scaler += 1.0f;
				//if (event.mouseWheel.delta < 0) scaler -= 1.0f;

			}
			else if (event.type == sf::Event::EventType::TextEntered) {
				//event type is keyboard button was press. set the char.
				userInput.k.key = (char)event.text.unicode;
				//printf("\r\n%i - %c", (int)userInput.k.key, userInput.k.key);
			}
			else if (event.type == sf::Event::EventType::KeyReleased){
			}
			else {
				//console.handleInput(event); //Testing Console
				userInput.k.key = sf::Keyboard::Unknown; // set key to -1
			}
		}

		////---------------------------------------------------Loop Timing Info-------------------------------------------------
		auto stopTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stopTime - startTime);
		float loopTimeDuration = (float)duration.count()/1000.f;
		//sprintf_s(loopText, "Loop Time: %f", loopTimeDuration);
		//loopTimeText.setText(loopText);
		char mouseString[64];
		sprintf_s(mouseString, "\t\t%f , %f", userInput.m.mousePosf.x, userInput.m.mousePosf.y);
		D_MousePosition.setPosition(userInput.m.mousePosf);
		D_MousePosition.setText(mouseString);
		//D_Graph_loopTime.update(userInput, true, &loopTimeDuration);	//DEBUG OUTSIDE UIELEMENTS	

	}//end update loop

	window.close();
	//killThread = true;
	//if(serial_thread != nullptr) serial_thread->join();
	return 0;
}
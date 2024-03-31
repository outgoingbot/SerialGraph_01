/*
Serial Oscilloscope

features to add:
configure comport and baud rate in gui (text entry). connect, disconn, pause buttons
transmit text box and send button
Mouse wheel to change scale of data values (zoom)
x and y axis graduated (scale with zoom)
x axis some sort of accurate time interval (system clock?)
graph keeps scrolling with no data (like an oscope)
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

#include "SimpleMenu/Src/Menu.h"

//Debug vars
char charArrayDebug[256] = "Empty";

//Serial
uint32_t numSamples = 0; //keep track of the total number of received data (WIP)
char incomingData[256] = { 0 }; //Serial Rx Buffer
unsigned int dataLength = 1;
int bytesReceived = 0;

//SFML Globals (dont change or remove)

//loop control
std::thread *serial_thread = nullptr;
bool killThread = false; //rename to killSerialThread
uint8_t gui_ID = 0;
//graphics
sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Serial O-Scope _sweeded", sf::Style::Default, sf::ContextSettings(32));
//sf::RenderWindow windowSettings(sf::VideoMode(800, 600), "Settings", sf::Style::Default, sf::ContextSettings(32));
sf::Font font;
sf::Vector2f mousePosf;
//SFML Globals (dont change or remove)

uint8_t handleButton_1() {
	printf("Clicked!!!!");
	return 0;
}


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

	//create the Comm port class object
	
	
	Serial SP;    // adjust in config.h
	SP.ListComPorts();
	
	if (SP.IsConnected()) printf("We're connected\r\n");


	//Load Font
	if (!font.loadFromFile("../res/Pumpkin_Pancakes.ttf")) {
		printf("Error loading Font");
		system("pause");
	}

	//Load Texture
	sf::Texture texture;
	if (!texture.loadFromFile("../res/logo.png")) {
		std::cout << "Could not load enemy texture" << std::endl;
		return 0;
	}
	texture.setSmooth(true);

	sf::Sprite sprite;
	sprite.setTexture(texture);
	sprite.setScale(sf::Vector2f(.1, .1));
	sf::FloatRect rc2 = sprite.getLocalBounds();
	sprite.setOrigin(rc2.width / 2, rc2.height / 2);
	sprite.setPosition(sf::Vector2f(100, WINDOW_HEIGHT - 100));


	//set FPS
	window.setFramerateLimit(60); //seriously reduces the CPU/GPU utilization
	//window.setVerticalSyncEnabled(true);
	window.setActive(true);







	// Hold all UI elements in vector
	std::vector<UIElement*> elements;

	//Create Gui Objects
	//Button( Size, Position, Text)

	Buttons Button_1(sf::Vector2f(200, 100), sf::Vector2f(1340, 150),sf::Color::Green,"Connect", &handleButton_1);
	elements.push_back(&Button_1);

	Buttons Button_2(sf::Vector2f(200, 100), sf::Vector2f(1540, 150), sf::Color::Red, "Disconnect", &handleButton_1);
	elements.push_back(&Button_2);

	std::vector<Graph*> Graph_Vector;
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(750, 1400), "Graph_1", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(750, 1100), "Graph_2", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(750, 800), "Graph_3", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(750, 500), "Graph_4", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(2100, 1400), "Graph_5", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(2100, 1100), "Graph_6", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(2100, 800), "Graph_7", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(2100, 500), "Graph_8", NUMFLOATS));

	// stuff elements vector with Graph_Vector
	for (auto vector : Graph_Vector) elements.push_back(vector);

	Graph Graph_loopTime(sf::Vector2f(600, WINDOW_HEIGHT / 12), sf::Vector2f(2250, 200), "LoopTime", 1);
	elements.push_back(&Graph_loopTime);
	
	char loopText[64]="";
	Label loopTimeText(50, sf::Vector2f(2100, 100), sf::Color::Magenta, loopText);
	elements.push_back(&loopTimeText);
	
	//give me the mouse postion to help with layout
	char charArrayMousePos[256] = "";
	Label mousePosText(25, sf::Vector2f(100, 100), sf::Color::Green, charArrayMousePos);
	elements.push_back(&mousePosText);

	//To Display the serial data received
	char charArraySerialData[256] = "Empty";
	Label serialText(50, sf::Vector2f(100, 300), sf::Color::Yellow, charArraySerialData);
	elements.push_back(&serialText);
	
	// create menu object
	Menu mainMenu;
	// set some of its properties
	mainMenu.setDockingPosition(sf::Vector2f(400, WINDOW_HEIGHT - 400));
	mainMenu.setBackgroundColor(sf::Color::Magenta);
	elements.push_back(&mainMenu);

	// add menu items. list of available com ports
	for (auto ComPortName : SP.ComPortNames) {
		mainMenu.addMenuItem(window, ComPortName);
	}
	

	//this is gonna need to be moved into Connect Button to start the thread
	//getting around 1mS without using threads
	SP.Connect(COMM_PORT);
	if (SP.IsConnected()) {
		serial_thread = new std::thread(&Serial::ReadData, &SP, incomingData, dataLength, &bytesReceived); ///retrieve buffer
	}
																								 //-----------------------------------------MAIN LOOP------------------------------------------------------------
	bool running = true;
	UIElement* movingElement = nullptr;

	while (running)
	{
		auto startTime = std::chrono::high_resolution_clock::now();
		
		//Get Keyboard inputs
		//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) posText.move(10,0);
		//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) posText.move(-10, 0);
		//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) posText.move(0, -10);
		//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) posText.move(0, 10);

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				//window.close();
				SP.~Serial(); //deconstruct
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
		}
		
		//Mouse Position Even after Windows Resizing
		mousePosf = window.mapPixelToCoords(sf::Mouse::getPosition(window));

		//xMouseCross.setPosition(sf::Vector2f(0, mousePosf.y));
		//yMouseCross.setPosition(sf::Vector2f(mousePosf.x,0));
		//
		//sprintf_s(charArrayMousePos, "(%f  %f)", (float)(sf::Mouse::getPosition(window).x), (float)(WINDOW_HEIGHT - sf::Mouse::getPosition(window).y));
		//mousePosText.setText(charArrayMousePos);
		//mousePosText.setPos(sf::Vector2f(mousePosf.x + 50, mousePosf.y + 50)); //set text

		if (movingElement != nullptr) {
			// Keep track of which element we are dragging 
			UI_State_t elementState = movingElement->getState((sf::Vector2i)mousePosf);
			// If the right mouse button is depressed, release movingElement
			if (!(elementState&BUTTON_STATE_CLICK_RIGHT)) movingElement = nullptr;
			
		}
		else 
		{
			// Handle Element Clicked event
			for (auto element : elements) {
				if (element->getState((sf::Vector2i)mousePosf)) {
					// If we are draging an element, point movingElement at it and break
					if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
						movingElement = element;
						std::iter_swap(elements.end() - 1, std::find(elements.begin(), elements.end(), element)); // Move dragged element to front
						break;
					}
				}
			}

		}

		//---------------------------------------------------display-------------------------------------------------
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		//window.clear(sf::Color::Black);
		sprite.rotate(2);
		window.draw(sprite);

		for (auto element : elements) element->draw(window);

		//mainMenu.showComponentOutlines();
		//mainMenu.draw(window, (sf::Vector2i)mousePosf);
	
		window.display(); //show drawn objects to the display buffer


		//---------------------------------------------------Process Serial Packet-------------------------------------------------

		if (SP.payloadComplete) { // ascii to bin
			SP.payloadComplete = false;
			//printf("Data: %f, %f, %f Qs:%i pIdx:%i\r\n", SP.myData[0], SP.myData[1], SP.myData[2], SP.queueSize, SP.payloadIdx);
			sprintf_s(charArraySerialData, "Serial Data: %f, %f, %f", SP.myData[0], SP.myData[1], SP.myData[2]);
			serialText.setText(charArraySerialData);

			if(SP.payloadIdx) Graph_Vector[SP.payloadIdx-1]->update(window, SP.myData);
		}
			


		//---------------------------------------------------Loop Timing Info-------------------------------------------------
		auto stopTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stopTime - startTime);
		float loopTimeDuration = (float)duration.count()/1000.f;
		sprintf_s(loopText, "Loop Time: %f", loopTimeDuration);
		loopTimeText.setText(loopText);
		Graph_loopTime.update(window, &loopTimeDuration);		

	}//end update loop

	window.close();
	killThread = true;
	if(serial_thread != nullptr) serial_thread->join();
	return 0;
}
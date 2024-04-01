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



//SFML Globals (dont change or remove)

typedef struct {
	bool mouseLeftClick = false;
	bool mouseRightClick = false;
	sf::Vector2f mousePosf;
} mouseState_t;

mouseState_t mouseState;
//sf::Vector2f mousePosf;




uint8_t handleButton_1(uint8_t val) {
	printf("Clicked!!!!");
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
	SP.ListBaudRates();
	
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

	Buttons Button_1(sf::Vector2f(200, 100), sf::Vector2f(1400, 1500),sf::Color::Green,"Connect", &handleButton_1);
	elements.push_back(&Button_1);

	Buttons Button_2(sf::Vector2f(200, 100), sf::Vector2f(1650, 1500), sf::Color::Red, "Disconnect", &handleButton_1);
	elements.push_back(&Button_2);

	std::vector<Graph*> Graph_Vector;
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 10), sf::Vector2f(150, 700), "Graph_1", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 10), sf::Vector2f(150, 900), "Graph_2", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 10), sf::Vector2f(150, 1100), "Graph_3", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 10), sf::Vector2f(150, 1300), "Graph_4", NUMFLOATS));

	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 10), sf::Vector2f(1500, 700), "Graph_5", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 10), sf::Vector2f(1500, 900), "Graph_6", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 10), sf::Vector2f(1500, 1100), "Graph_7", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 10), sf::Vector2f(1500, 1300), "Graph_8", NUMFLOATS));

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
	Label serialText(50, sf::Vector2f(300, 100), sf::Color::Yellow, charArraySerialData);
	elements.push_back(&serialText);
	
	// create menu object
	Menu Menu_1(sf::Vector2f(300, 50), sf::Vector2f(0, 0), sf::Color(10, 10, 10), "Comm Port", handleMenu_1);
	elements.push_back(&Menu_1);

	// add menu items. list of available com ports
	for (auto ComPortName : SP.ComPortNames) {
		Menu_1.addMenuItem(window, ComPortName);
	}

	Menu Menu_2(sf::Vector2f(300, 50), sf::Vector2f(310, 0), sf::Color(10, 10, 10), "Baud Rate", handleMenu_2);
	elements.push_back(&Menu_2);

	// add menu items. list of available com ports
	for (auto ComPortBaud : SP.ComPortBauds) {
		Menu_2.addMenuItem(window, ComPortBaud);
	}
	

	//this is gonna need to be moved into Connect Button to start the thread
	SP.Connect(COMM_PORT);
	if (SP.IsConnected()) {
		serial_thread = new std::thread(&Serial::ReadData, &SP, incomingData, dataLength, &bytesReceived); ///retrieve buffer
	}
																								 //-----------------------------------------MAIN LOOP------------------------------------------------------------
	bool running = true;
	UIElement* interactedElement = nullptr;
	UIElement* lastUpdatedElement = nullptr;





	
	while (running)
	{
		// SerialGraph::updateInteractiveState()
		auto startTime = std::chrono::high_resolution_clock::now();

		
		//Get Keyboard inputs
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) sprite.move(10, 0);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) sprite.move(-10, 0);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) sprite.move(0, -10);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) sprite.move(0, 10);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) sprite.scale(sf::Vector2f(sprite.getScale().x+1.0f, sprite.getScale().y + 1.0f));

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

		///////Mouse State Update
		//Mouse Position Even after Windows Resizing
		mouseState.mousePosf = window.mapPixelToCoords(sf::Mouse::getPosition(window));
		mouseState.mouseLeftClick = sf::Mouse::isButtonPressed(sf::Mouse::Left);
		mouseState.mouseRightClick = sf::Mouse::isButtonPressed(sf::Mouse::Right);

		// If we are locked into an interaction...
			// If the mouse is still down, continue interaction
			// Else, mouse is up, release lock (Falling Edge)

		// Else, not locked into interaction
			// If mouse is over element
			// Update Interactive State
			// If mouse is down over element
				// lock interaction

		if (interactedElement != nullptr) { // If we are locked into an interaction...
			// If the mouse is still down, continue interaction
			if (mouseState.mouseLeftClick || mouseState.mouseRightClick) {
				interactedElement->updateInteractiveState((sf::Vector2i)mouseState.mousePosf);
			}
			else { // Else, mouse is up, release lock (Falling Edge)
				interactedElement->updateInteractiveState((sf::Vector2i)mouseState.mousePosf);
				interactedElement = nullptr;
			}
			
		}
		else { // Else, not locked into interaction
			
			// for every element...
			for (auto element : elements) {
				// If mouse is over element
				if (element->mouseOverElement((sf::Vector2i)mouseState.mousePosf, sf::Vector2i(0, 0))) {
					// Update Interactive State
					element->updateInteractiveState((sf::Vector2i)mouseState.mousePosf);

					// If we skip to here from another element that we haven't executed out exit update from,
					// updated the interacted state. If an edge case where mouse immediatly goes down, treat as interacted element.
					if (lastUpdatedElement != nullptr && lastUpdatedElement != element) {
						lastUpdatedElement->updateInteractiveState((sf::Vector2i)mouseState.mousePosf);
						if ((mouseState.mouseLeftClick || mouseState.mouseRightClick) && interactedElement == nullptr) {
							sf::Mouse::setPosition((sf::Vector2i)lastUpdatedElement->getPosition()); // force mouse over lastUpdatedElement
							lastUpdatedElement->updateInteractiveState((sf::Vector2i)mouseState.mousePosf);
							// lock interaction and break
							interactedElement = lastUpdatedElement;
							std::iter_swap(elements.end() - 1, std::find(elements.begin(), elements.end(), lastUpdatedElement)); // Move dragged element to front
							break;
						}
					}

					lastUpdatedElement = element; // update last updated element
					// If mouse is down over element
					if ((mouseState.mouseLeftClick || mouseState.mouseRightClick) && interactedElement == nullptr) {
						// lock interaction and break
						interactedElement = element;
						std::iter_swap(elements.end() - 1, std::find(elements.begin(), elements.end(), element)); // Move dragged element to front
						//break;
					}

				}
				else if (lastUpdatedElement == element) { // update element after mouse leaves
					lastUpdatedElement->updateInteractiveState((sf::Vector2i)mouseState.mousePosf);
					lastUpdatedElement = nullptr;

				}
			}		

		}

		//SerialGraph::draw()
		//---------------------------------------------------display-------------------------------------------------
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		sprite.rotate(2);
		window.draw(sprite);

		for (auto element : elements) element->draw(window);
		
		window.display(); //show drawn objects to the display buffer


		//---------------------------------------------------Process Serial Packet-------------------------------------------------
		//SerialGraph::update()
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
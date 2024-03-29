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

Currenly Generating single float vlaue with /r/n terminating ASCXII string
expected data format: "%f,%f,%f\n"
115200 Baud (set in RS232Comm.cpp line 46)

next change will allow CSV strings with \r\n terminating

*/

#include "SerialGraph_01.h"
#include <iostream>

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


//Debug vars
char charArrayDebug[256] = "Empty";

//Serial
uint32_t numSamples = 0; //keep track of the total number of received data (WIP)
char incomingData[256] = { 0 }; //Serial Rx Buffer
unsigned int dataLength = 1;
int bytesReceived = 0;

//SFML Globals (dont change or remove)

//loop control
bool killThread = false; //rename to killSerialThread
uint8_t gui_ID = 0;
//graphics
sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Serial O-Scope _sweeded", sf::Style::Default, sf::ContextSettings(32));
sf::Font font;
sf::Vector2f mousePosf;
//SFML Globals (dont change or remove)


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
	Serial SP(COMM_PORT);    // adjust in config.h
	if (SP.IsConnected()) printf("We're connected\r\n");


	//Load Font
	if (!font.loadFromFile("../res/Pumpkin_Pancakes.ttf")) {
		printf("Error loading Font");
		system("pause");
	}

	//set FPS
	//window.setFramerateLimit(10000);
	//window.setVerticalSyncEnabled(true);
	window.setActive(true);

	//Create Gui Objects
	//Button( Size, Position, Text)
	Buttons Button_1(sf::Vector2f(200, 100), sf::Vector2f(1340, 150),sf::Color::Green,"Button_1");

	std::vector<Graph> Graph_Vector;
	Graph_Vector.push_back(Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(650, 1500), "Graph_1", NUMFLOATS));
	Graph_Vector.push_back(Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(650, 1200), "Graph_2", NUMFLOATS));
	Graph_Vector.push_back(Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(650, 900), "Graph_3", NUMFLOATS));
	Graph_Vector.push_back(Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(650, 600), "Graph_4", NUMFLOATS));
	Graph_Vector.push_back(Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(2000, 1500), "Graph_5", NUMFLOATS));
	Graph_Vector.push_back(Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(2000, 1200), "Graph_6", NUMFLOATS));
	Graph_Vector.push_back(Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(2000, 900), "Graph_7", NUMFLOATS));
	Graph_Vector.push_back(Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(2000, 600), "Graph_8", NUMFLOATS));

	Graph Graph_loopTime(sf::Vector2f(600, WINDOW_HEIGHT / 12), sf::Vector2f(2250, 200), "LoopTime", NUMFLOATS);
	
	char loopText[64]="";
	Label loopTimeText(50, sf::Vector2f(2250, 300), sf::Color::Magenta, loopText);
	
	//give me the mouse postion to help with layout
	char charArrayMousePos[256] = "Empty";
	Label mousePosText(25, sf::Vector2f(100, 100), sf::Color::Green, charArrayMousePos);

	//To Display the serial data received
	char charArraySerialData[256] = "Empty";
	Label serialText(50, sf::Vector2f(0, WINDOW_HEIGHT-50), sf::Color::Yellow, charArraySerialData);
	
	
	sf::RectangleShape xMouseCross(sf::Vector2f(WINDOW_WIDTH, 2)); //x mouse crosshair
	xMouseCross.setPosition(sf::Vector2f(0, (window.getSize().y / 2) - 1));
	xMouseCross.setFillColor(sf::Color::Magenta);

	sf::RectangleShape yMouseCross(sf::Vector2f(2, WINDOW_HEIGHT)); //y mouse crosshair
	yMouseCross.setPosition(sf::Vector2f((window.getSize().x / 2) - 1, 0));
	yMouseCross.setFillColor(sf::Color::Magenta);
	
	//getting around 1mS without using threads
	std::thread serial_thread(&Serial::ReadData, &SP, incomingData, dataLength, &bytesReceived); ///retrieve buffer
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

		xMouseCross.setPosition(sf::Vector2f(0, mousePosf.y));
		yMouseCross.setPosition(sf::Vector2f(mousePosf.x,0));
		
		sprintf_s(charArrayMousePos, "(%f  %f)", (float)(sf::Mouse::getPosition(window).x), (float)(WINDOW_HEIGHT - sf::Mouse::getPosition(window).y));
		mousePosText.setText(charArrayMousePos);
		mousePosText.setPos(sf::Vector2f(mousePosf.x + 20, mousePosf.y)); //set text

		if (movingElement != nullptr) {
			movingElement->getState((sf::Vector2i)mousePosf);
			// If the right mouse button is depressed, release movingElement
			if (!sf::Mouse::isButtonPressed(sf::Mouse::Right)) movingElement = nullptr;
		}
		else 
		{
			//Handle the Button Pressed Event:
			if (Button_1.getState((sf::Vector2i)mousePosf)) {
				// if the right mouse is pressed on this element, set it to the moving element
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
					movingElement = &Button_1;
					goto HANDLED_UI;
				}

			}

			//Handle the Graph Pressed Event:
			if (Graph_Vector[0].getState((sf::Vector2i)mousePosf)) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
					movingElement = &Graph_Vector[0];
					goto HANDLED_UI;
				}
			}

			//Handle the Graph Pressed Event:
			if (Graph_Vector[1].getState((sf::Vector2i)mousePosf)) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
					movingElement = &Graph_Vector[1];
					goto HANDLED_UI;
				}
			}

			//Handle the Graph Pressed Event:
			if (Graph_Vector[2].getState((sf::Vector2i)mousePosf)) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
					movingElement = &Graph_Vector[2];
					goto HANDLED_UI;
				}
			}

			//Handle the Graph Pressed Event:
			if (Graph_Vector[3].getState((sf::Vector2i)mousePosf)) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
					movingElement = &Graph_Vector[3];
					goto HANDLED_UI;
				}
			}

			//Handle the Graph Pressed Event:
			if (Graph_Vector[4].getState((sf::Vector2i)mousePosf)) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
					movingElement = &Graph_Vector[4];
					goto HANDLED_UI;
				}
			}

			//Handle the Graph Pressed Event:
			if (Graph_Vector[5].getState((sf::Vector2i)mousePosf)) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
					movingElement = &Graph_Vector[5];
					goto HANDLED_UI;
				}
			}

			//Handle the Graph Pressed Event:
			if (Graph_Vector[6].getState((sf::Vector2i)mousePosf)) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
					movingElement = &Graph_Vector[6];
					goto HANDLED_UI;
				}
			}

			//Handle the Graph Pressed Event:
			if (Graph_Vector[7].getState((sf::Vector2i)mousePosf)) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
					movingElement = &Graph_Vector[7];
					goto HANDLED_UI;
				}
			}


		//Handle the Looptime Graph Pressed Event:
		if (Graph_loopTime.getState((sf::Vector2i)mousePosf)) {
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
				movingElement = &Graph_loopTime;
				goto HANDLED_UI;
			}
		}

		//Handle Labels
		if (loopTimeText.getState((sf::Vector2i)mousePosf)) {
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
				movingElement = &loopTimeText;
				goto HANDLED_UI;
			}
		}

		if (serialText.getState((sf::Vector2i)mousePosf)) {
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
				movingElement = &serialText;
				goto HANDLED_UI;
			}
		}
	}

		HANDLED_UI:

		//---------------------------------------------------display-------------------------------------------------
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		//window.clear(sf::Color::Black);
		
		//draw the Gui Objects
		Button_1.draw();
		
		
		//draw all of the graphs
		for (int i = 0; i < NUM_GRAPHS; i++)Graph_Vector[i].draw();
		
		Graph_loopTime.draw();
		loopTimeText.draw();
		mousePosText.draw();
		serialText.draw();

		window.draw(xMouseCross);
		window.draw(yMouseCross);

		window.display();


		//---------------------------------------------------Process Serial Packet-------------------------------------------------

		if (SP.payloadComplete) { // ascii to bin
			SP.payloadComplete = false;
			printf("Data: %f, %f, %f Qs:%i pIdx:%i\r\n", SP.myData[0], SP.myData[1], SP.myData[2], SP.queueSize, SP.payloadIdx);
			sprintf_s(charArraySerialData, "Serial Data: %f, %f, %f", SP.myData[0], SP.myData[1], SP.myData[2]);
			serialText.setText(charArraySerialData);

			if(SP.payloadIdx) Graph_Vector[SP.payloadIdx-1].update(SP.myData, NUMFLOATS);
		}
			


		//---------------------------------------------------Loop Timing Info-------------------------------------------------
		auto stopTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stopTime - startTime);
		float loopTimeDuration = (float)duration.count()/1000.f;
		sprintf_s(loopText, "Loop Time: %f", loopTimeDuration);
		loopTimeText.setText(loopText);
		Graph_loopTime.update(&loopTimeDuration, 1);		

	}//end update loop

	window.close();
	killThread = true;
	serial_thread.join();
	return 0;
}
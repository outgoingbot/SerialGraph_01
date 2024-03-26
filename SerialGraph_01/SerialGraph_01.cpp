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

#include "CircularQueue.h"


//using namespace std::chrono;

#define TOTALFLOATS 3
#define SUBSTRING_LEN 32
//Graphics
float scaler = 1.0f; //needs to rescale all dots. not just new data


char charArray[256] = "Empty"; //wtf this used for again?


							   //Serial
uint32_t numSamples = 0; //keep track of the total number of received data (WIP)
char incomingData[256] = { 0 }; //Serial Rx Buffer
unsigned int dataLength = 1;
int bytesReceived = 0;

bool killThread = false;


//Globals
sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Serial O-Scope _sweeded");
sf::Font font;



int main()
{	

	auto image = sf::Image{};
	if (!image.loadFromFile("../res/icon.png"))
	{
		printf("Error loading Icon");
		system("pause");
	}
	window.setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());

	Serial SP(COMM_PORT);    // adjust in config.h
	if (SP.IsConnected()) printf("We're connected\r\n");


	//Load Font
	if (!font.loadFromFile("../res/Pumpkin_Pancakes.ttf")) {
		printf("Error loading Font");
		system("pause");
	}

	//set FPS
	window.setFramerateLimit(10000);
	
	//Create Gui Objects
	//Button( Size, Position, Text)
	Buttons Button_1(sf::Vector2f(200, 100), sf::Vector2f(1340, 150),"Button_1");

	//Graph( Size, Position, Text, number of floats to display)
	Graph Graph_1(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(20, 1500), "Graph_1", NUMFLOATS);
	Graph Graph_2(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(20, 1200), "Graph_2", NUMFLOATS);
	Graph Graph_3(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(20, 900), "Graph_3", NUMFLOATS);
	Graph Graph_4(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(20, 600), "Graph_4", NUMFLOATS);
	Graph Graph_5(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(1400, 1500), "Graph_5", NUMFLOATS);
	Graph Graph_6(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(1400, 1200), "Graph_6", NUMFLOATS);
	Graph Graph_7(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(1400, 900), "Graph_7", NUMFLOATS);
	Graph Graph_8(sf::Vector2f(1200, WINDOW_HEIGHT / 8), sf::Vector2f(1400, 600), "Graph_8", NUMFLOATS);

	Graph Graph_loopTime(sf::Vector2f(600, WINDOW_HEIGHT / 12), sf::Vector2f(2250, 200), "LoopTime", NUMFLOATS);
	

	//Make shapes
	char loopText[64]="";
	sf::Text loopTimeText;
	loopTimeText.setFont(font);
	loopTimeText.setString(loopText);
	loopTimeText.setCharacterSize(50);
	loopTimeText.setFillColor(sf::Color::Magenta);
	loopTimeText.setPosition(sf::Vector2f(2250, WINDOW_HEIGHT-300));
	
	//give me the mouse postion to help with layout
	sf::Text mousePosText;
	mousePosText.setFont(font);
	mousePosText.setString(charArray); //wrong char array!
	mousePosText.setCharacterSize(25);
	mousePosText.setFillColor(sf::Color::Green);
	mousePosText.setPosition(sf::Vector2f(100, 100));

	//Display the serial data received
	sf::Text serialText;
	serialText.setFont(font);
	serialText.setString(incomingData);
	serialText.setCharacterSize(50);
	serialText.setFillColor(sf::Color::Red);
	serialText.setPosition(sf::Vector2f(0, 50));	//random spot
	
	sf::RectangleShape axis_x(sf::Vector2f(WINDOW_WIDTH, 2)); //x axis
	axis_x.setPosition(sf::Vector2f(0, (window.getSize().y/2)-1));
	axis_x.setFillColor(sf::Color::White);

	sf::RectangleShape xMouseCross(sf::Vector2f(WINDOW_WIDTH, 2)); //x mouse crosshair
	xMouseCross.setPosition(sf::Vector2f(0, (window.getSize().y / 2) - 1));
	xMouseCross.setFillColor(sf::Color::Magenta);

	sf::RectangleShape yMouseCross(sf::Vector2f(2, WINDOW_HEIGHT)); //y mouse crosshair
	yMouseCross.setPosition(sf::Vector2f((window.getSize().x / 2) - 1, 0));
	yMouseCross.setFillColor(sf::Color::Magenta);
	
	
	//the descrete serial data
	sf::CircleShape dot[WINDOW_WIDTH]; //create the dots
	for (int i = 0; i < WINDOW_WIDTH; i++) { //initialize the dots
		dot[i].setRadius(2.0);
		dot[i].setOrigin(2.0, 2.0);
		dot[i].setFillColor(sf::Color::Red);
		dot[i].setPosition(sf::Vector2f(i, window.getSize().y / 2)); //place all dots on the x axis
	}
	
	//connected lines that interpolate the descrete serial data
	sf::Vertex lineInterpol[WINDOW_WIDTH - 1];

	
	//getting around 20mS without using threads
	std::thread serial_thread(&Serial::ReadData, &SP, incomingData, dataLength, &bytesReceived); ///retrieve buffer
//-----------------------------------------MAIN LOOP------------------------------------------------------------
	while (window.isOpen())
	{
		auto startTime = std::chrono::high_resolution_clock::now();
		



		//Get inputs
//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) posText.move(10,0);
//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) posText.move(-10, 0);
//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) posText.move(0, -10);
//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) posText.move(0, 10);


		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
				//SP.~Serial(); //deconstruct
			}
			//else {
			//	if (event.type == sf::Event::MouseWheelMoved) {
			//		// display number of ticks mouse wheel has moved
			//		if (event.mouseWheel.delta > 0) scaler += 1.0f;
			//		if (event.mouseWheel.delta < 0) scaler -= 1.0f;

			//		//this method below doesnt work because it scales the already scaled value
			//		//for (int i = 0; i < WINDOW_WIDTH-1; i++) dot[i].setPosition(sf::Vector2f(i, (float) ( ( (dot[i].getPosition().y - (window.getSize().y / 2)) * scaler ) + (window.getSize().y / 2))) );
			//	}
			//}
		}

		xMouseCross.setPosition(sf::Vector2f(0, sf::Mouse::getPosition(window).y));
		yMouseCross.setPosition(sf::Vector2f(sf::Mouse::getPosition(window).x, 0));

		sprintf_s(charArray, "(%f  %f)", (float)(sf::Mouse::getPosition(window).x), (float)(WINDOW_HEIGHT - sf::Mouse::getPosition(window).y));
		mousePosText.setString(charArray);
		mousePosText.setPosition(sf::Vector2f(sf::Mouse::getPosition(window).x + 20, sf::Mouse::getPosition(window).y)); //set text



		//Handle the Button Pressed Event:
		if (Button_1.isPressed(sf::Mouse::getPosition(window))) {

		}

		//Handle the Graph Pressed Event:
		if (Graph_1.isPressed(sf::Mouse::getPosition(window))) {

		}
		//Handle the Graph Pressed Event:
		if (Graph_2.isPressed(sf::Mouse::getPosition(window))) {

		}
		//Handle the Graph Pressed Event:
		if (Graph_3.isPressed(sf::Mouse::getPosition(window))) {

		}
		//Handle the Graph Pressed Event:
		if (Graph_4.isPressed(sf::Mouse::getPosition(window))) {

		}
		//Handle the Graph Pressed Event:
		if (Graph_5.isPressed(sf::Mouse::getPosition(window))) {

		}
		//Handle the Graph Pressed Event:
		if (Graph_6.isPressed(sf::Mouse::getPosition(window))) {

		}
		//Handle the Graph Pressed Event:
		if (Graph_7.isPressed(sf::Mouse::getPosition(window))) {

		}
		//Handle the Graph Pressed Event:
		if (Graph_8.isPressed(sf::Mouse::getPosition(window))) {

		}

		
		//display
		window.clear(sf::Color::Black);
		
		//draw the Gui Objects
		Button_1.draw();

		Graph_1.draw();
		Graph_2.draw();
		Graph_3.draw();
		Graph_4.draw();
		Graph_5.draw();
		Graph_6.draw();
		Graph_7.draw();
		Graph_8.draw();

		Graph_loopTime.draw();

		window.draw(loopTimeText);
		window.draw(mousePosText);
		window.draw(serialText);
		window.draw(xMouseCross);
		window.draw(yMouseCross);

		window.display();

		if (SP.payloadComplete) { // ascii to bin
			SP.payloadComplete = false;
			printf("myData[0] = %f, myData[1] = %f, myData[2] = %f Qs:%i\r\n", SP.myData[0], SP.myData[1], SP.myData[2], SP.queueSize);

			Graph_1.update(SP.myData, NUMFLOATS); //draw the data 3 floats
			Graph_2.update(SP.myData, NUMFLOATS); //draw the data 3 floats
			Graph_3.update(SP.myData, NUMFLOATS); //draw the data 3 floats
			Graph_4.update(SP.myData, NUMFLOATS); //draw the data 3 floats
			Graph_5.update(SP.myData, NUMFLOATS); //draw the data 3 floats
			Graph_6.update(SP.myData, NUMFLOATS); //draw the data 3 floats
			Graph_7.update(SP.myData, NUMFLOATS); //draw the data 3 floats
			Graph_8.update(SP.myData, NUMFLOATS); //draw the data 3 floats
			sprintf_s(charArray, "Serial Data: %f, %f, %f", SP.myData[0], SP.myData[1], SP.myData[2]);
			serialText.setString(charArray);
		}
			



		auto stopTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stopTime - startTime);
		float loopTimeDuration = (float)duration.count()/1000.f;
		sprintf_s(loopText, "Loop Time: %f", loopTimeDuration);
		loopTimeText.setString(loopText);
		Graph_loopTime.update(&loopTimeDuration, 1);		

	}//end update loop
	killThread = true;
	serial_thread.join();
	return 0;
}
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
expected data format: "%f/r/n"
115200 Baud (set in RS232Comm.cpp line 46)

next change will allow CSV strings with /r/n terminating

*/

#include "SerialGraph_01.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <Windows.h>    
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <RS232Comm.h>
#include <cmath>

#include "Config.h"
#include "Buttons.h"


//Graphics
float scaler = 1; //needs to rescale all dots. not just new data

//Serial
char charArray[256] = "Empty"; //graphics text buffer
uint32_t numSamples = 0; //keep track of the total number of received data (WIP)
float myData;
char incomingData[256] = ""; //Serial Rx Buffer

int dataLength = 255;
int bytesReceived = 0;

//Globals
sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Serial O-Scope _sweeded");
sf::Font font;

int main()
{	
	// ...
	auto image = sf::Image{};
	if (!image.loadFromFile("../res/icon.png"))
	{
		printf("Error loading Icon");
		system("pause");
	}
	window.setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());

	Serial* SP = new Serial(COMM_PORT);    // adjust as needed \\\\.\\COM10
	if (SP->IsConnected()) printf("We're connected");

	
	//Load Font
	if (!font.loadFromFile("../res/Pumpkin_Pancakes.ttf")) {
		printf("Error loading Font");
		system("pause");
	}

	//set FPS
	window.setFramerateLimit(200);


	//Create Gui Objects

	//Button( Size, Position, Text)
	Buttons Button_1(sf::Vector2f(200, 100), sf::Vector2f(1200, 1000),"Button_1");


	

	//Make shapes
	//give me the mouse postion to help with layout
	sf::Text mousePosText;
	mousePosText.setFont(font);
	mousePosText.setString(charArray);
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

	

//-----------------------------------------MAIN LOOP------------------------------------------------------------
	while (window.isOpen())
	{

		//Get inputs
	//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) posText.move(10,0);
	//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) posText.move(-10, 0);
	//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) posText.move(0, -10);
	//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) posText.move(0, 10);


		sf::Event event;
		while (window.pollEvent(event)){
			if (event.type == sf::Event::Closed) {
				window.close();
				SP->~Serial(); //deconstruct
			}
			else {
				if (event.type == sf::Event::MouseWheelMoved) {
					// display number of ticks mouse wheel has moved
					if (event.mouseWheel.delta > 0) scaler+=0.01;
					if (event.mouseWheel.delta < 0) scaler-=0.01;
					
					//this method below doesnt work because it scales the already scaled value
					//for (int i = 0; i < WINDOW_WIDTH-1; i++) dot[i].setPosition(sf::Vector2f(i, (float) ( ( (dot[i].getPosition().y - (window.getSize().y / 2)) * scaler ) + (window.getSize().y / 2))) );
				}
			}
		}



	
		//Handle Serial
		bytesReceived = SP->ReadData(incomingData, dataLength); //number of bytes read
		incomingData[bytesReceived] = 0;
		incomingData[bytesReceived] = '\0';
		if (bytesReceived > 0) {
			bytesReceived = 0;
			//numSamples++; //I could keep track of the sample count
			sscanf_s(incomingData, "%f", &myData); // ascii to bin
			//draw the dots
			dot[WINDOW_WIDTH-1].setPosition(sf::Vector2f(WINDOW_WIDTH-1, (float)(-myData*scaler + (window.getSize().y / 2)))); //keep the dots y position but shift the x values from i+1 to i
			for (int i = 0; i < WINDOW_WIDTH - 1; i++) dot[i].setPosition(sf::Vector2f(i,dot[i+1].getPosition().y) ); //keep the dots y position but shift the x values from i+1 to i
			//draw linear interpolated lines
			for (int i = 0; i < WINDOW_WIDTH - 1; i++) lineInterpol[i] = sf::Vertex(sf::Vector2f(i, dot[i + 1].getPosition().y), sf::Color::Red);
			
			sprintf_s(charArray, "Serial Data: %f", myData);
			serialText.setString(charArray);

			//Console
			printf("\nSerial Data: %s\n\n", incomingData);	// Display ASCII message from port
			//printf("Scaler: %f\n\n", scaler);	// Display message from port

		}


		xMouseCross.setPosition(sf::Vector2f(0, sf::Mouse::getPosition(window).y));
		yMouseCross.setPosition(sf::Vector2f(sf::Mouse::getPosition(window).x, 0));

		sprintf_s(charArray, "(%f  %f)", (float)(sf::Mouse::getPosition(window).x), (float)(WINDOW_HEIGHT - sf::Mouse::getPosition(window).y));
		mousePosText.setString(charArray);
		mousePosText.setPosition(sf::Vector2f(sf::Mouse::getPosition(window).x + 20, sf::Mouse::getPosition(window).y)); //set text



		//Handle the Button Pressed Event:
		if (Button_1.isPressed(sf::Mouse::getPosition(window))) {

		}



		//display
		window.clear(sf::Color::Black);
		
		//draw the Gui Objects
		Button_1.draw();

		window.draw(mousePosText);
		window.draw(serialText);
		window.draw(axis_x);
		window.draw(xMouseCross);
		window.draw(yMouseCross);
		for(int i=0; i< WINDOW_WIDTH; i++) window.draw(dot[i]);
		window.draw(lineInterpol, WINDOW_WIDTH-1, sf::Lines); 
		window.display();
	}//end update loop

	return 0;
}
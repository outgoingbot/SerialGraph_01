#include "Graph.h"


//size, position
Graph::Graph(sf::Vector2f size, sf::Vector2f position, const char* title, uint8_t numVars) {
	frameSamples = (uint32_t)(size.x);

	dataArray = new float[frameSamples];
	frame.setSize(size);

	
	//frame.setOrigin(btn.width / 2, btn.height / 2);
	frame.setPosition(sf::Vector2f(position.x, WINDOW_HEIGHT - position.y));
	frame.setOutlineThickness(2);
	frame.setOutlineColor(sf::Color::White);
	frame.setFillColor(sf::Color::Black);

	//sf::FloatRect lb = frame.getLocalBounds();
	axis_x.setSize(sf::Vector2f(size.x, 2)); //x axis
	axis_x.setPosition(sf::Vector2f(frame.getPosition().x, frame.getPosition().y + (size.y / 2)));
	axis_x.setFillColor(sf::Color::White);
	
	
	

	dot = new sf::CircleShape[frameSamples];
	for (uint32_t i = 0; i < frameSamples; i++) { //initialize the dots
		dot[i].setRadius(2.0);
		dot[i].setOrigin(2.0, 2.0);
		dot[i].setFillColor(sf::Color::Blue);
		dot[i].setPosition(sf::Vector2f(frame.getPosition().x+i, axis_x.getPosition().y)); //place all dots on the x axis
		dataArray[i] = 0.0f;//(float)(axis_x.getPosition().y);
	}

	lineInterpol = new sf::Vertex[frameSamples - 1];
	for (uint32_t i = 0; i < frameSamples - 1; i++) { //initialize the lines
		lineInterpol[i] = sf::Vertex((sf::Vector2f(frame.getPosition().x+i, axis_x.getPosition().y))); //place all dots on the x axis
	}



	text.setFont(font);
	text.setString(title);
	text.setCharacterSize(40);
	text.setPosition(sf::Vector2f(frame.getPosition().x, frame.getPosition().y));
	text.setFillColor(sf::Color::White);

}

Graph::~Graph() {

}

void Graph::setScale(float scl) {
	scaler = scl;
}

void Graph::autoScale(bool) {

}

void Graph::update(float dataPoint, uint8_t len) {

	for (uint32_t i = 0; i < frameSamples - 1; i++) {
		dataArray[i] = dataArray[i + 1];
		dot[i].setPosition(sf::Vector2f(frame.getPosition().x+i, (float)((-dataArray[i+1] * scaler) + frame.getPosition().y + (frame.getSize().y / 2)))); //shift the dots left 1 pixel
	}

	dataArray[frameSamples - 1] = dataPoint;
	dot[frameSamples - 1].setPosition(sf::Vector2f(frame.getPosition().x+frameSamples - 1, (float)(-dataPoint * scaler + frame.getPosition().y +(frame.getSize().y / 2) ))); //keep the dots y position but shift the x values from i+1 to i
	
	//draw linear interpolated lines
	for (uint32_t i = 0; i < frameSamples - 1; i++) lineInterpol[i] = sf::Vertex(sf::Vector2f(frame.getPosition().x+i, (float)(-dataArray[i + 1] * scaler + frame.getPosition().y + (frame.getSize().y / 2))), sf::Color::Red);

}

void Graph::draw() {
	window.draw(frame);
	window.draw(text);
	window.draw(axis_x);
	for (int i = 0; i < frameSamples; i++) window.draw(dot[i]);
	window.draw(lineInterpol, frameSamples - 1, sf::Lines);
}



bool Graph::isPressed(sf::Vector2i mousePosition) {
#define MOUSESHIFT 30
	if (isMouseOverRect(mousePosition)) {
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			frame.setFillColor(sf::Color(255, 0, 255));
			return true;
			//while (sf::Mouse::isButtonPressed(sf::Mouse::Left));
		}

		//move the object
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			frame.setPosition(sf::Vector2f(mousePosition.x- MOUSESHIFT, mousePosition.y - MOUSESHIFT));
			///text.setPosition(sf::Vector2f(mousePosition.x-10, mousePosition.y-10));
			text.setPosition(sf::Vector2f(frame.getPosition().x, frame.getPosition().y));
			axis_x.setPosition(sf::Vector2f(frame.getPosition().x, frame.getPosition().y + (frame.getSize().y / 2)));
			return false;
			//while (sf::Mouse::isButtonPressed(sf::Mouse::Left));
		}
	}
	return false;
}

bool Graph::isMouseOverRect(sf::Vector2i mousePosition) {
	if (mousePosition.x > frame.getPosition().x && mousePosition.x < frame.getPosition().x+frame.getSize().x) {
		if (mousePosition.y > frame.getPosition().y && mousePosition.y < frame.getPosition().y+frame.getSize().y) {
			frame.setFillColor(sf::Color(20, 20, 20));
			return true;
		}
	}
	frame.setFillColor(sf::Color(0, 0, 0));
	return false;
}
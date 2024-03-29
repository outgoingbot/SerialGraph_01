#include "Graph.h"

extern sf::RenderWindow window;
extern sf::Font font;


#define XSCALE 1 //testing 1,10,100

//size, position
Graph::Graph(sf::Vector2f size, sf::Vector2f position, const char* title, uint8_t numVars) {
	_len = (int)numVars;
	frameSamples = (uint32_t)(size.x/ XSCALE);
	for (int j = 0; j < numVars; j++) {
		switch (j) {
		case 0:
			dotColor[j] = sf::Color::Red;
		break;

		case 1:
			dotColor[j] = sf::Color::Green;
		break;

		case 2:
			dotColor[j] = sf::Color::Blue;
		break;
		}
	}
	//const sf::Color dotColor[NUMFLOATS] = { sf::Color::Red ,sf::Color::Green ,sf::Color::Blue };

	for(int j=0; j<numVars; j++) dataArray[j] = new float[frameSamples];
	frame.setSize(size);
	
	sf::FloatRect grph = frame.getLocalBounds();
	frame.setOrigin(grph.width / 2, grph.height / 2);

	frame.setPosition(sf::Vector2f(position.x, WINDOW_HEIGHT - position.y));
	frame.setOutlineThickness(2);
	frame.setOutlineColor(sf::Color::White);
	frame.setFillColor(sf::Color::Black);

	axis_x.setSize(sf::Vector2f(size.x, 2)); //x axis
	axis_x.setPosition(sf::Vector2f(frame.getPosition().x - (size.x/2), frame.getPosition().y));
	axis_x.setFillColor(sf::Color::White);
	

	
	
	//change this to memSet
	for (int j = 0; j < numVars; j++) {
		for (uint32_t i = 0; i < frameSamples; i++) { //initialize the dataArray
			dataArray[j][i] = 0.0f;
		}
	}


	for (int j = 0; j < numVars; j++) {
		lineInterpol[j] = new sf::Vertex[frameSamples - 1];
		for (uint32_t i = 0; i < frameSamples - 1; i++) { //initialize the lineStrip verticies
			lineInterpol[j][i] = sf::Vertex((sf::Vector2f(frame.getPosition().x + (i*XSCALE), axis_x.getPosition().y))); //place all dots on the x axis
		}
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

void Graph::update(float *dataPoint, uint8_t len) {
	maxVal = 0;
	minVal = 100000;

	
	for (int j = 0; j < len; j++) {
		//Search for Max and Min Values (this could be tracked as the serial data comes in maybe?)
		for (uint32_t i = 0; i < frameSamples - 1; i++) {	
			dataArray[j][i] = dataArray[j][i + 1];
			if (dataArray[j][i] > maxVal) maxVal = dataArray[j][i];
			if (dataArray[j][i] < minVal) minVal = dataArray[j][i];
		}
		
		//Set the scaler to multiply the data by. this auto scales the data to fit in the graph window (top edge)
		scaler = frame.getSize().y / (maxVal+1);
		
		//Trying to Set the X axis position to so the Min Value is close graph window edge (bottom edge)		
		axis_x.setPosition(frame.getPosition().x-(frame.getSize().x/2), frame.getPosition().y + (frame.getSize().y/2) - (minVal));
		
		//Store the new Data float value in the Rightmost data point
		dataArray[j][frameSamples - 1] = dataPoint[j];
		
		//draw linear interpolated lines by shifting all the data points Left
		for (uint32_t i = 0; i < frameSamples - 1; i++) {
			lineInterpol[j][i] = sf::Vertex(sf::Vector2f(axis_x.getPosition().x + (i*XSCALE), (float)((-dataArray[j][i + 1] * scaler) + axis_x.getPosition().y)), dotColor[j]);
		}
	}
}


void Graph::draw(void) {
	window.draw(frame);
	window.draw(text);
	window.draw(axis_x);
	for (int j = 0; j < _len; j++) window.draw(lineInterpol[j], frameSamples - 1, sf::LineStrip);
}


UI_State_t Graph::getState(sf::Vector2i mousePosition) {
#define MOUSESHIFT 0 //hack to keep the mouse over the graph box
	UI_State_t returnVal = GRAPH_STATE_READY;
	
		if (isMouseOverRect(mousePosition)) {
			returnVal |= GRAPH_STATE_HOVER;

			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				frame.setFillColor(sf::Color(255, 0, 255));
				returnVal |= GRAPH_STATE_CLICK_LEFT;
				//while (sf::Mouse::isButtonPressed(sf::Mouse::Left));
			}

			//move the object
	
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
				
				frame.setPosition(sf::Vector2f(mousePosition.x - MOUSESHIFT, mousePosition.y - MOUSESHIFT));
				text.setPosition(sf::Vector2f(frame.getPosition().x, frame.getPosition().y));
				axis_x.setPosition(sf::Vector2f(frame.getPosition().x-frame.getSize().x/2, frame.getPosition().y));
					
				returnVal |= GRAPH_STATE_CLICK_RIGHT;
				//while (sf::Mouse::isButtonPressed(sf::Mouse::Left));
			}
			else {
			
			}

			//sf::Event event;
			//while (window.pollEvent(event)) {
			//	if (event.type == sf::Event::MouseWheelMoved) {
			//		// display number of ticks mouse wheel has moved
			//		if (event.mouseWheel.delta > 0) this->scaler += 1.0f;
			//		if (event.mouseWheel.delta < 0) this->scaler -= 1.0f;

			//		//this method below doesnt work because it scales the already scaled value
			//		//for (int i = 0; i < WINDOW_WIDTH-1; i++) dot[i].setPosition(sf::Vector2f(i, (float) ( ( (dot[i].getPosition().y - (window.getSize().y / 2)) * scaler ) + (window.getSize().y / 2))) );
			//	}
			//}
		//}
	}
	return returnVal;
}



bool Graph::isMouseOverRect(sf::Vector2i mousePosition) {
	if (mousePosition.x > frame.getPosition().x - (frame.getSize().x / 2) && mousePosition.x < frame.getPosition().x + (frame.getSize().x / 2)) {
		if (mousePosition.y > frame.getPosition().y - (frame.getSize().y / 2) && mousePosition.y < frame.getPosition().y + (frame.getSize().y / 2)) {
			frame.setFillColor(sf::Color(20, 20, 20));
			return true;
		}
	}
	frame.setFillColor(sf::Color(0, 0, 0));
	return false;
}

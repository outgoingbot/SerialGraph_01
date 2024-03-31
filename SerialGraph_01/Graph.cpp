#include "Graph.h"

//extern sf::RenderWindow window;
extern sf::Font font;


#define XSCALE 1 //testing 1,10,100

//size, position
Graph::Graph(sf::Vector2f size, sf::Vector2f position, const char* title, uint8_t numVars) {
	_len = (int)numVars;
	frameSamples = (uint32_t)(size.x/ XSCALE);
	for (int j = 0; j < numVars; j++) {
		switch (j) {
		case 0:
			lineColor[j] = sf::Color::Red;
		break;

		case 1:
			lineColor[j] = sf::Color::Green;
		break;

		case 2:
			lineColor[j] = sf::Color::Blue;
		break;
		}
	}
	//const sf::Color dotColor[NUMFLOATS] = { sf::Color::Red ,sf::Color::Green ,sf::Color::Blue };

	for(int j=0; j<numVars; j++) dataArray[j] = new float[frameSamples];
	frame.setSize(size);
	
	sf::FloatRect graphRec = frame.getLocalBounds();
	frame.setOrigin(graphRec.width / 2, graphRec.height / 2);

	frame.setPosition(sf::Vector2f(position.x, WINDOW_HEIGHT - position.y));
	frame.setOutlineThickness(2);
	frame.setOutlineColor(sf::Color::White);
	frame.setFillColor(sf::Color::Black);

	axis_x.setSize(sf::Vector2f(frame.getSize().x, 2)); //x axis
	axis_x.setPosition(sf::Vector2f(frame.getPosition().x - (frame.getSize().x / 2), frame.getPosition().y));
	

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
	textLabel.setFont(font);
	textLabel.setCharacterSize(40);
	textLabel.setString(title);
	sf::FloatRect graphtextRec = textLabel.getLocalBounds();
	graphRec = frame.getGlobalBounds();
	textLabel.setPosition(sf::Vector2f(graphRec.left, graphRec.top- graphtextRec.height-5));
	textLabel.setFillColor(sf::Color::White);

	textAxis_y.setFont(font);
	textAxis_y.setCharacterSize(20);
	textAxis_y.setString("100.00");
	graphtextRec = textAxis_y.getLocalBounds();
	
	textAxis_y.setPosition(sf::Vector2f(graphRec.left - graphtextRec.width, graphRec.top+graphtextRec.height));
	textAxis_y.setFillColor(sf::Color::Yellow);


	xMouseCross.setSize(sf::Vector2f(graphRec.width,2));
	yMouseCross.setSize(sf::Vector2f(2, graphRec.height));

	xMouseCross.setPosition(sf::Vector2f(graphRec.left, graphRec.top+(graphRec.height/2)));
	yMouseCross.setPosition(sf::Vector2f(graphRec.left + (graphRec.width/2), graphRec.top));
	
	xMouseCross.setFillColor(sf::Color::Magenta);
	yMouseCross.setFillColor(sf::Color::Magenta);

	
	textyMouse.setFont(font);
	textyMouse.setCharacterSize(20);
	textyMouse.setString("Empty");
	textyMouse.setFillColor(sf::Color::Magenta);
	textyMouse.setPosition(sf::Vector2f(graphRec.left + (graphRec.width / 2), graphRec.top + (graphRec.height / 2)));


}

Graph::~Graph() {

}

void Graph::setScale(float scl) {
	scaler = scl;
}

void Graph::autoScale(bool) {

}

void Graph::update(sf::RenderWindow& window , float *dataPoint) {
	sf::FloatRect graphRec = frame.getGlobalBounds();
	sf::Vector2f mousePosf = window.mapPixelToCoords(sf::Mouse::getPosition(window));
	if (dataPoint == nullptr) return;
	maxVal = 0;
	minVal = 100000;

	
	for (int j = 0; j < _len; j++) {
		//Search for Max and Min Values (this could be tracked as the serial data comes in maybe?)
		for (uint32_t i = 0; i < frameSamples - 1; i++) {	
			dataArray[j][i] = dataArray[j][i + 1];
			if (dataArray[j][i] > maxVal) maxVal = dataArray[j][i];
			if (dataArray[j][i] < minVal) minVal = dataArray[j][i];
		}
		sprintf_s(textBuff, "%.3f", maxVal);
		textAxis_y.setString(textBuff);
		sprintf_s(textBuff, "%.3f", UtilFuncs::mapFloat(mousePosf.y,graphRec.top+graphRec.height,graphRec.top,minVal,maxVal));
		textyMouse.setString(textBuff);
		//Set the scaler to multiply the data by. this auto scales the data to fit in the graph window (top edge)
		scaler = frame.getSize().y / (maxVal*1.1f); //give 10% extra room at top of graph
		
		//Trying to Set the X axis position to so the Min Value is close graph window edge (bottom edge)		
		axis_x.setPosition(frame.getPosition().x-(frame.getSize().x/2), frame.getPosition().y + (frame.getSize().y/2) - (minVal));
		
		//Store the new Data float value in the Rightmost data point
		dataArray[j][frameSamples - 1] = dataPoint[j];
		
		//draw linear interpolated lines by shifting all the data points Left
		for (uint32_t i = 0; i < frameSamples - 1; i++) {
			lineInterpol[j][i] = sf::Vertex(sf::Vector2f(axis_x.getPosition().x + (i*XSCALE), (float)((-dataArray[j][i + 1] * scaler) + axis_x.getPosition().y)), lineColor[j]);
		}
	}
}


void Graph::draw(sf::RenderWindow& window) {
	window.draw(frame);
	window.draw(textLabel);
	window.draw(textAxis_y);
	window.draw(axis_x);
	for (int j = 0; j < _len; j++) window.draw(lineInterpol[j], frameSamples - 1, sf::LineStrip);
	if (drawCrosshair) {
		window.draw(yMouseCross);
		window.draw(xMouseCross);
		window.draw(textyMouse);
	}
}


UI_State_t Graph::getState(sf::Vector2i mousePosition) {
#define MOUSESHIFT 0 //hack to keep the mouse over the graph box
	UI_State_t returnVal = UI_STATE_READY;
	sf::FloatRect graphRec = frame.getGlobalBounds();
	sf::FloatRect graphtextRec = textAxis_y.getGlobalBounds();
	sf::FloatRect graphtextRec2 = textLabel.getGlobalBounds();


	
		if (isMouseOverRect(mousePosition)) 
		{
			returnVal |= UI_STATE_HOVER;
			drawCrosshair = true;
			xMouseCross.setPosition(sf::Vector2f(graphRec.left, mousePosition.y));
			yMouseCross.setPosition(sf::Vector2f(mousePosition.x, graphRec.top));
			textyMouse.setPosition(sf::Vector2f(mousePosition.x+20,mousePosition.y-30));

			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				frame.setFillColor(sf::Color(255, 0, 255));
				returnVal |= UI_STATE_CLICK_LEFT;
				//while (sf::Mouse::isButtonPressed(sf::Mouse::Left));
			}

			//move the object
	
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
				
				frame.setPosition(sf::Vector2f(mousePosition.x - MOUSESHIFT, mousePosition.y - MOUSESHIFT));
				textLabel.setPosition(sf::Vector2f(graphRec.left, graphRec.top - graphtextRec2.height-5));
				textAxis_y.setPosition(sf::Vector2f(graphRec.left - graphtextRec.width, graphRec.top + graphtextRec.height));

				axis_x.setPosition(sf::Vector2f(frame.getPosition().x-frame.getSize().x/2, frame.getPosition().y));
					
				// Update Graph Lines
				//draw linear interpolated lines by shifting all the data points Left
				for (int j = 0; j < _len; j++) {
					for (uint32_t i = 0; i < frameSamples - 1; i++) {
						lineInterpol[j][i] = sf::Vertex(sf::Vector2f(axis_x.getPosition().x + (i*XSCALE), (float)((-dataArray[j][i] * scaler) + axis_x.getPosition().y)), lineColor[j]);
					}
				}

				returnVal |= UI_STATE_CLICK_RIGHT;
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
		else {
			drawCrosshair = false;
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

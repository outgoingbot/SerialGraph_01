#include "Graph.h"

#define XSCALE 10//testing 1,10,100
#define GRAPH_NAME_PADDING 20
#define GRAPH_X_AXIS_PADDING 20
#define GRAPH_Y_AXIS_PADDING 20
//size, position
Graph::Graph(sf::Vector2f size, sf::Vector2f position, const char* title, uint8_t numVars) {
	//load private font
	if (!_font.loadFromFile("../res/arial.ttf")) {
		printf("Error loading Font");
		system("pause");
	}

	_len = (int)numVars;
	frameSamples = (uint32_t)(size.x/ XSCALE); //I dont want to shrink the number of frame samples
	//frameSamples = (uint32_t)(size.x); //I dont want to shrink the number of frame samples

	for (int j = 0; j < numVars; j++) {
		switch (j) {
		case 0:
			_lineColor[j] = sf::Color::Red;
		break;

		case 1:
			_lineColor[j] = sf::Color::Green;
		break;

		case 2:
			_lineColor[j] = sf::Color::Blue;
		break;
		}
	}
	

	for(int j=0; j<numVars; j++) dataArray[j] = new float[frameSamples];

	_dock.setSize(size);
	//sf::FloatRect graphRec = _dock.getLocalBounds();

	_dock.setPosition(sf::Vector2f(position.x, position.y));
	_dock.setOutlineThickness(2);
	_dock.setOutlineColor(sf::Color::White);
	_dock.setFillColor(sf::Color::Black);

	_axis_x.setSize(sf::Vector2f(_dock.getSize().x, 2)); //x axis
	_axis_x.setPosition(sf::Vector2f(_dock.getPosition().x, _dock.getPosition().y + (_dock.getSize().y/2)));
	_axis_x.setFillColor(sf::Color::White);
	
	_graphName = new Label(40, _dock.getPosition(), sf::Color::White, title);
	_graphName->setPosition(sf::Vector2f(_dock.getPosition().x, _dock.getPosition().y - _graphName->getSize().y - GRAPH_NAME_PADDING));
	_elements.push_back(_graphName);

	_textAxis_y = new Label(20, _dock.getPosition(), sf::Color::White, "Empty");
	_textAxis_y->setPosition(sf::Vector2f(_dock.getPosition().x - _textAxis_y->getSize().x-GRAPH_Y_AXIS_PADDING, _dock.getPosition().y + _textAxis_y->getSize().y));
	_elements.push_back(_textAxis_y);
	
	_textyMouse = new Label(20, _dock.getPosition(), sf::Color::Magenta, "Empty");
	//_elements.push_back(_textyMouse); //dont push this oneto the vector


	_xMouseCross.setSize(sf::Vector2f(_dock.getSize().x,2));
	_yMouseCross.setSize(sf::Vector2f(2, _dock.getSize().y));

	_xMouseCross.setPosition(sf::Vector2f(_dock.getSize().x, _dock.getPosition().y+(_dock.getSize().y/2)));
	_yMouseCross.setPosition(sf::Vector2f(_dock.getSize().x + (_dock.getSize().x/2), _dock.getSize().y));
	
	_xMouseCross.setFillColor(sf::Color::Magenta);
	_yMouseCross.setFillColor(sf::Color::Magenta);

	
	_drawables.push_back(&_dock);
	_drawables.push_back(&_axis_x);
	//_drawables.push_back(&_textAxis_y);
	
	_interactive.push_back(&_xMouseCross);
	_interactive.push_back(&_yMouseCross);
	//_interactive.push_back(&_textyMouse);


	//change this to memSet
	for (int j = 0; j < numVars; j++) {
		for (uint32_t i = 0; i < frameSamples; i++) { //initialize the dataArray
			dataArray[j][i] = 0.0f;
		}
	}
	
	for (int j = 0; j < numVars; j++) {
		_lineInterpol[j] = new sf::Vertex[frameSamples - 1];
		for (uint32_t i = 0; i < frameSamples - 1; i++) { //initialize the lineStrip verticies
			_lineInterpol[j][i] = sf::Vertex((sf::Vector2f(_dock.getPosition().x + (i*XSCALE), _axis_x.getPosition().y))); //place all dots on the x axis
		}
	}


}

Graph::~Graph() {

}

void Graph::setScale(float scl) {
	scaler = scl;
}

void Graph::autoScale(bool) {

}

void Graph::update(sf::RenderWindow& window , float *dataPoint) {
	sf::FloatRect graphRec = _dock.getGlobalBounds();
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
		_textAxis_y->setText(textBuff);
		
		sprintf_s(textBuff, "%.3f", UtilFuncs::mapFloat(mousePosf.y,graphRec.top+graphRec.height,graphRec.top,minVal,maxVal));
		_textyMouse->setText(textBuff);
		//Set the scaler to multiply the data by. this auto scales the data to fit in the graph window (top edge)
		scaler = _dock.getSize().y / (maxVal*1.1f); //give 10% extra room at top of graph
		
		//Trying to Set the X axis position to so the Min Value is close graph window edge (bottom edge)		
		_axis_x.setPosition(_dock.getPosition().x, _dock.getPosition().y + (_dock.getSize().y) - (minVal));
		
		//Store the new Data float value in the Rightmost data point
		dataArray[j][frameSamples - 1] = dataPoint[j];
		
		//draw linear interpolated lines by shifting all the data points Left
		for (uint32_t i = 0; i < frameSamples - 1; i++) {
			_lineInterpol[j][i] = sf::Vertex(sf::Vector2f(_axis_x.getPosition().x + (i*XSCALE), (float)((-dataArray[j][i + 1] * scaler) + _axis_x.getPosition().y)), _lineColor[j]);
		}
	}
}


void Graph::draw(sf::RenderWindow& win) {
	// parent.draw();
	//Order Matters. FIFO is Back-to-Front of display
	for (auto Item : _drawables) win.draw(*Item);
	for (int j = 0; j < _len; j++) win.draw(_lineInterpol[j], frameSamples - 1, sf::LineStrip);
	for (auto element : _elements) element->draw(win);

	if (drawCrosshair) {
		_textyMouse->draw(win);
		for (auto Item : _interactive) win.draw(*Item);
	}
	
}


UI_State_t Graph::updateInteractiveState(inputState_t userInput) {
	UI_State_t returnVal = UI_STATE_READY;
	returnVal |= UI_STATE_HOVER;

	_xMouseCross.setPosition(sf::Vector2f(_dock.getPosition().x, userInput.m.mousePosf.y));
	_yMouseCross.setPosition(sf::Vector2f(userInput.m.mousePosf.x, _dock.getPosition().y));
	_textyMouse->setPosition(sf::Vector2f(userInput.m.mousePosf.x+20, userInput.m.mousePosf.y-30));

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		_dock.setSize(sf::Vector2f(_dock.getSize().x + 10, _dock.getSize().y + 10));
		_axis_x.setSize(sf::Vector2f(_dock.getSize().x, 2)); //x axis
		_axis_x.setPosition(sf::Vector2f(_dock.getPosition().x, _dock.getPosition().y + (_dock.getSize().y / 2)));
		_dock.setFillColor(sf::Color(255, 0, 255));
		returnVal |= UI_STATE_CLICK_LEFT;
	}

	//move the object
	if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
		_dock.setPosition(sf::Vector2f(userInput.m.mousePosf.x - (_dock.getSize().x/2), userInput.m.mousePosf.y -(_dock.getSize().y / 2)));
		_graphName->setPosition(sf::Vector2f(_dock.getPosition().x, _dock.getPosition().y - _graphName->getSize().y- GRAPH_NAME_PADDING));
		_textAxis_y->setPosition(sf::Vector2f(_dock.getPosition().x - _textAxis_y->getSize().x- GRAPH_Y_AXIS_PADDING, _dock.getPosition().y + _textAxis_y->getSize().y));
		_axis_x.setPosition(sf::Vector2f(_dock.getPosition().x, _dock.getPosition().y + _dock.getSize().y));
					
		// Update Graph Lines while dragging (hacky)
		for (int j = 0; j < _len; j++) {
			for (uint32_t i = 0; i < frameSamples - 1; i++) {
				_lineInterpol[j][i] = sf::Vertex(sf::Vector2f(_axis_x.getPosition().x + (i*XSCALE), (float)((-dataArray[j][i] * scaler) + _axis_x.getPosition().y)), _lineColor[j]);
			}
		}
		returnVal |= UI_STATE_CLICK_RIGHT;
	}
	

	if (this->isMouseOverRect(userInput.m.mousePosf)) {
		drawCrosshair = true;
		_dock.setFillColor(sf::Color(20, 20, 20)); // highlight gray
	}
	else {
		_dock.setFillColor(sf::Color::Black); // black color
		drawCrosshair = false;
	}

	// Call parent updateInteractiveState to evaluate children states
	returnVal |= UIElement::updateInteractiveState(userInput);
		
	return returnVal;
}



bool Graph::isMouseOverRect(sf::Vector2f mousePosition) {
	if (mousePosition.x > _dock.getPosition().x && mousePosition.x < _dock.getPosition().x + (_dock.getSize().x)) {
		if (mousePosition.y > _dock.getPosition().y && mousePosition.y < _dock.getPosition().y + (_dock.getSize().y)) {
			return true;
		}
	}
	return false;
}


sf::Vector2f Graph::getSize() {
	return _dock.getSize();
}

sf::Vector2f Graph::getPosition() {
	return _dock.getPosition();
}
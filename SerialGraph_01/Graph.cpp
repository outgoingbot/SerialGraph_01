#include "Graph.h"

//#define XSCALE 10//testing 1,10,100
#define GRAPH_NAME_PADDING 20
#define GRAPH_X_AXIS_PADDING 20
#define GRAPH_Y_AXIS_PADDING 20
//size, position
Graph::Graph(sf::Vector2f size, sf::Vector2f position, const char* title, uint8_t numVars) {
	_size = size.x;

	//load private font
	if (!_font.loadFromFile("../res/arial.ttf")) {
		printf("Error loading Font");
		system("pause");
	}

	_len = (int)numVars;
	frameSamples = (uint32_t)(size.x); //Frame Samples is based off the constructed graph Width

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

	_textYaxisScale = new Label(20, _dock.getPosition(), sf::Color::White, "Empty");
	_textYaxisScale->setPosition(sf::Vector2f(_dock.getPosition().x - _textYaxisScale->getSize().x-GRAPH_Y_AXIS_PADDING, _dock.getPosition().y + _textYaxisScale->getSize().y));
	_elements.push_back(_textYaxisScale);
	
	_textXaxisScale_start = new Label(20, _dock.getPosition(), sf::Color::White, "Start");
	_textXaxisScale_start->setPosition(sf::Vector2f(_dock.getPosition().x, _dock.getPosition().y + _dock.getSize().y + _textXaxisScale_start->getSize().y - GRAPH_NAME_PADDING));
	_elements.push_back(_textXaxisScale_start);

	_textXaxisScale_stop = new Label(20, _dock.getPosition(), sf::Color::White, "Stop");
	_textXaxisScale_stop->setPosition(sf::Vector2f(_dock.getPosition().x+ _dock.getSize().x, _dock.getPosition().y + _dock.getSize().y + _textXaxisScale_stop->getSize().y - GRAPH_NAME_PADDING));
	_elements.push_back(_textXaxisScale_stop);

	_textCrossHairData = new Label(20, _dock.getPosition(), sf::Color::Magenta, "Empty");
	//_elements.push_back(_textCrossHairData); //dont push this oneto the vector


	_xMouseCross.setSize(sf::Vector2f(_dock.getSize().x,2));
	_yMouseCross.setSize(sf::Vector2f(2, _dock.getSize().y));

	_xMouseCross.setPosition(sf::Vector2f(_dock.getSize().x, _dock.getPosition().y+(_dock.getSize().y/2)));
	_yMouseCross.setPosition(sf::Vector2f(_dock.getSize().x + (_dock.getSize().x/2), _dock.getSize().y));
	
	_xMouseCross.setFillColor(sf::Color::Magenta);
	_yMouseCross.setFillColor(sf::Color::Magenta);

	
	_drawables.push_back(&_dock);
	_drawables.push_back(&_axis_x);
	
	_interactive.push_back(&_xMouseCross);
	_interactive.push_back(&_yMouseCross);


	_menu = new Menu(sf::Vector2f(300, 50), sf::Vector2f(_dock.getPosition().x, _dock.getPosition().y), sf::Color(10, 10, 10), "Options", handleMenu_1);
	_elements.push_back(_menu);
	// add menu items. list of available com ports	
	_menu->addMenuItem((std::string)"AutoScale");
	_menu->addMenuItem("Pause");
	_menu->addMenuItem("Percent");
	//end UI elements
	
	//these could both be vectors
	for (int j = 0; j < numVars; j++) {
		for (uint32_t i = 0; i < frameSamples; i++) { //initialize the dataArray (//change this to memSet)
			dataArray[j][i] = 0.0f;
		}
	}
	
	for (int j = 0; j < numVars; j++) {
		_lineInterpol[j] = new sf::Vertex[frameSamples];
		for (uint32_t i = 0; i < frameSamples; i++){//initialize the lineStrip verticies
			_lineInterpol[j][i] = sf::Vertex((sf::Vector2f(_dock.getPosition().x+i, _axis_x.getPosition().y))); //place all verticies on the x axis
		}
	}


}

Graph::~Graph() {

}

void Graph::setScale(float scl) {
	_Yscaler = scl;
}

void Graph::autoScale(bool) {

}


//CHEACK THAT WE ARE INDEXING THE DATA ARRAY CORRECTLY
//going to need this to draw from right to left so that graph resizing works
void Graph::update(inputState_t userInput, bool withNewData, float *dataPoint) {
	sf::FloatRect graphRec = _dock.getGlobalBounds();
	//sf::Vector2f mousePosf = window.mapPixelToCoords(sf::Mouse::getPosition(window));
	if (dataPoint == nullptr && withNewData == true) {
		return;
	}

	float xRatio = (_dock.getSize().x / _size);
	
	for (int j = 0; j < _len; j++) {
		//Set the X axis scaling here
		uint32_t startPos = frameSamples - (frameSamples / _Xscaler);
		
		//Search for Max and Min Values
		maxVal = 0; 
		minVal = 100000;  //(this is ugly)
		for (uint32_t i = startPos; i < frameSamples - 1; i++) {
			if (dataArray[j][i] > maxVal) maxVal = dataArray[j][i];
			if (dataArray[j][i] < minVal) minVal = dataArray[j][i];
		}

		sprintf_s(textBuff, "%.3f", maxVal);
		_textYaxisScale->setText(textBuff);
		
		sprintf_s(textBuff, " %.3f", UtilFuncs::mapFloat(userInput.m.mousePosf.y,graphRec.top+graphRec.height,graphRec.top,minVal,maxVal));
		_textCrossHairData->setText(textBuff);
		
		//Set the y axis scaler to multiply the data by. this auto scales the data to fit in the graph window (top edge)
		_Yscaler = _dock.getSize().y / (maxVal*1.1f); //give 10% extra room at top of graph
		
		//Trying to Set the X axis position to so the Min Value is close graph window edge (bottom edge)		
		_axis_x.setPosition(_dock.getPosition().x, _dock.getPosition().y + (_dock.getSize().y) - (minVal));
		

		//draw linear interpolated lines by shifting all the data points Left
		if (withNewData) {
			//shift the all data to the left one place
			for (uint32_t i = 0; i < frameSamples - 1; i++) {	
				dataArray[j][i] = dataArray[j][i + 1];
			}
			//Store the newest Data float value in the Rightmost data point
			dataArray[j][frameSamples - 1] = dataPoint[j];
			
			//stash/ sqush all the unused shit data points to left side of the screen (non "visible" data points that dont fit in the graph back after x scaling (zooming in)
			for (uint32_t i = 0; i < startPos; i++) {
				_lineInterpol[j][i] = sf::Vertex(sf::Vector2f(_axis_x.getPosition().x, (float)_axis_x.getPosition().y), _lineColor[j]);
			}
			//draw the vertex of interest (after x axis scaling)
			for (uint32_t i = startPos; i < frameSamples - 1; i++) {
				_lineInterpol[j][i] = sf::Vertex(sf::Vector2f(_axis_x.getPosition().x + ((i - startPos)*_Xscaler*xRatio), (float)((-dataArray[j][i] * _Yscaler) + _axis_x.getPosition().y)), _lineColor[j]);
			}
		}
		else {
			//we are dragging. no new Data. just redraw all verticies base on new _dock (X axis) Postion
			for (uint32_t i = 0; i < frameSamples - 1; i++) {
				_lineInterpol[j][i] = sf::Vertex(sf::Vector2f(_axis_x.getPosition().x + ((i - startPos)*_Xscaler*xRatio), (float)((-dataArray[j][i] * _Yscaler) + _axis_x.getPosition().y)), _lineColor[j]);
			}
		}
		//make sure to draw the last element on the rights side (makes cool effect if you comment this line)
		_lineInterpol[j][frameSamples - 1] = sf::Vertex(sf::Vector2f(_axis_x.getPosition().x + _axis_x.getSize().x, (float)((-dataArray[j][frameSamples - 1] * _Yscaler) + _axis_x.getPosition().y)), _lineColor[j]);
		
		sprintf_s(textBuff, "%i", startPos);
		_textXaxisScale_start->setText(textBuff);
		sprintf_s(textBuff, "%i", frameSamples);
		_textXaxisScale_stop->setText(textBuff);
		_textXaxisScale_start->setPosition(sf::Vector2f(_dock.getPosition().x, _dock.getPosition().y + _dock.getSize().y + _textXaxisScale_start->getSize().y - GRAPH_NAME_PADDING));
		_textXaxisScale_stop->setPosition(sf::Vector2f(_dock.getPosition().x + _dock.getSize().x, _dock.getPosition().y + _dock.getSize().y + _textXaxisScale_stop->getSize().y - GRAPH_NAME_PADDING));
	}
}


void Graph::draw(sf::RenderWindow& win) {
	// parent.draw();
	//Order Matters. FIFO is Back-to-Front of display
	for (auto Item : _drawables) win.draw(*Item);
	for (int j = 0; j < _len; j++) win.draw(_lineInterpol[j], frameSamples, sf::LineStrip);
	for (auto element : _elements) element->draw(win);

	if (drawCrosshair) {
		_textCrossHairData->draw(win);
		for (auto Item : _interactive) win.draw(*Item);
	}
	
}


UI_State_t Graph::updateInteractiveState(inputState_t userInput) {
#define GRAPH_DATA_TEXT_MOUSE_SHIFT 30
	UI_State_t returnVal = UI_STATE_READY;
	returnVal |= UI_STATE_HOVER;
	//update crosshair size incase _dock is resized
	_xMouseCross.setSize(sf::Vector2f(_dock.getSize().x, _xMouseCross.getSize().y));
	_yMouseCross.setSize(sf::Vector2f(_yMouseCross.getSize().x, _dock.getSize().y));

	_xMouseCross.setPosition(sf::Vector2f(_dock.getPosition().x, userInput.m.mousePosf.y));
	_yMouseCross.setPosition(sf::Vector2f(userInput.m.mousePosf.x, _dock.getPosition().y));
	_textCrossHairData->setPosition(sf::Vector2f(userInput.m.mousePosf.x, userInput.m.mousePosf.y - GRAPH_DATA_TEXT_MOUSE_SHIFT));

	//testing the x axis scaling (zooming)
	static bool keyPressed = true;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && !keyPressed) {
		keyPressed = true;
		if (_Xscaler < 100) {
			_Xscaler += 3;
			std::cout << _Xscaler << std::endl;
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !keyPressed) {
		keyPressed = true;
		if (_Xscaler > 3) {
			_Xscaler -= 3;
			std::cout << _Xscaler << std::endl;
		}
	}
	if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) keyPressed = false;
	//end x axis scaling



	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {	
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
			if (_dock.getSize().y > 100) {
				_dock.setSize(sf::Vector2f(_dock.getSize().x - 10, _dock.getSize().y - 10));
				this->update(userInput, false);
			}
		}
		else {
		_dock.setSize(sf::Vector2f(_dock.getSize().x + 10, _dock.getSize().y + 10));
		this->update(userInput, false);
		}

		_axis_x.setSize(sf::Vector2f(_dock.getSize().x, 2)); //x axis
		_axis_x.setPosition(sf::Vector2f(_dock.getPosition().x, _dock.getPosition().y + (_dock.getSize().y / 2)));
		_dock.setFillColor(sf::Color(255, 0, 255));
		returnVal |= UI_STATE_CLICK_LEFT;
	}



	//move the object
	if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
		_dock.setPosition(sf::Vector2f(userInput.m.mousePosf.x - (_dock.getSize().x/2), userInput.m.mousePosf.y -(_dock.getSize().y / 2)));
		_graphName->setPosition(sf::Vector2f(_dock.getPosition().x, _dock.getPosition().y - _graphName->getSize().y- GRAPH_NAME_PADDING));
		_textYaxisScale->setPosition(sf::Vector2f(_dock.getPosition().x - _textYaxisScale->getSize().x- GRAPH_Y_AXIS_PADDING, _dock.getPosition().y + _textYaxisScale->getSize().y));
		_axis_x.setPosition(sf::Vector2f(_dock.getPosition().x, _dock.getPosition().y + _dock.getSize().y));
		_menu->setPosition(_dock.getPosition());
		this->update(userInput, false); //call this to keep the graph visually updated with user input changes
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

void Graph::setPosition(sf::Vector2f pos) {
	_dock.setPosition(pos);
	for (auto element : _elements) element->setPosition(pos);
	
}
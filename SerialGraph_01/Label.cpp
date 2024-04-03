#include "Label.h"

//set size, position, text
Label::Label(unsigned int size, sf::Vector2f position, sf::Color color, const char* string) {
	//load private font
	if (!_font.loadFromFile("../res/arial.ttf")) {
		printf("Error loading Font");
		system("pause");
	}

	_color = color;
	_text.setFont(_font);
	_text.setCharacterSize(size);
	_text.setString(string);
	
	_text.setPosition(position);
	_text.setFillColor(_color);

	//setColor of the rectangle behind the text
	_dock.setFillColor(sf::Color::Transparent);
	sf::FloatRect rc = _text.getGlobalBounds();
	_dock.setSize(sf::Vector2f(rc.width, rc.height));
	_dock.setPosition(position);	
}


Label::~Label() {
	//delete text
}


void Label::setPos(sf::Vector2f position) {
	_text.setPosition(position);
	_dock.setPosition(position);
}

void Label::setText(const char* string) {
	_text.setString(string);
	sf::FloatRect rc = _text.getGlobalBounds();
	_dock.setSize(sf::Vector2f(rc.width, rc.height));
}

void Label::setText(const float f) {
	char string[64];
	sprintf_s(string, "%f", f);
	_text.setString(string);
	sf::FloatRect rc = _text.getGlobalBounds();
	_dock.setSize(sf::Vector2f(rc.width, rc.height));
}

UI_State_t Label::updateInteractiveState(inputState_t userInput) {
	UI_State_t returnVal = LABEL_STATE_READY;
	_text.setFillColor(sf::Color::Yellow); //mouse over highlight color
		
	returnVal |= LABEL_STATE_HOVER;
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		_text.setFillColor(sf::Color(255, 0, 255));
		returnVal |= LABEL_STATE_CLICK_LEFT;
	}

	//move the object
	if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
		_dock.setPosition(sf::Vector2f(userInput.m.mousePosf.x-(_dock.getSize().x/2), userInput.m.mousePosf.y-(_dock.getSize().y / 2)));
		_text.setPosition(_dock.getPosition());
		returnVal |= LABEL_STATE_CLICK_RIGHT;

	}
			
	if (this->isMouseOverRect(userInput.m.mousePosf)) {
		_text.setFillColor(sf::Color::Yellow);
	}
	else {
		_text.setFillColor(_color);
	}

	// Call parent updateInteractiveState to evaluate children states
	returnVal |= UIElement::updateInteractiveState(userInput);

	return returnVal;
}


bool Label::isMouseOverRect(sf::Vector2f mousePosition) {	
	if (mousePosition.x > _dock.getPosition().x && mousePosition.x < _dock.getPosition().x + _dock.getSize().x) {
		if (mousePosition.y > _dock.getPosition().y && mousePosition.y < _dock.getPosition().y + _dock.getSize().y) {
			return true;
		}
	}
	return false;
}



void Label::draw(sf::RenderWindow& window) {
	window.draw(_dock);
	window.draw(_text);
}



sf::Vector2f Label::getSize() {
	//sf::FloatRect fr = _text.getGlobalBounds();

	return _dock.getSize();
}

sf::Vector2f Label::getPosition() {
	return _dock.getPosition();
}
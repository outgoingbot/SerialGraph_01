#include "Label.h"

//set size, position, text
Label::Label(unsigned int size, sf::Vector2f position, sf::Color color, const char* string) {
	_color = color;
	text.setCharacterSize(size);

	text.setFont(font);
	text.setString(string);
	sf::FloatRect rc = text.getLocalBounds();
	text.setOrigin(rc.width / 2, rc.height / 2);
	text.setPosition(sf::Vector2f(position.x, WINDOW_HEIGHT - position.y));
	text.setFillColor(_color);
}


Label::~Label() {
	//delete text
}


void Label::setPos(sf::Vector2f position) {
	text.setPosition(sf::Vector2f(position.x+20, position.y+20));
}

void Label::setText(const char* string) {

	text.setString(string);
}

void Label::setText(const float f) {
	char string[64];
	sprintf_s(string, "%f", f);
	text.setString(string);
}

Label_State_t Label::getState(sf::Vector2i mousePosition) {
	Label_State_t returnVal = LABEL_STATE_READY;

	if (isMouseOverRect(mousePosition)) {
		returnVal |= LABEL_STATE_HOVER;
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			text.setFillColor(sf::Color(255, 0, 255));
			returnVal |= LABEL_STATE_CLICK_LEFT;
		}

		//move the object
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			text.setPosition(sf::Vector2f(mousePosition.x-20, mousePosition.y-20));
			returnVal |= LABEL_STATE_CLICK_RIGHT;
			//while (sf::Mouse::isButtonPressed(sf::Mouse::Left));
		}
	}
	return returnVal;
}


bool Label::isMouseOverRect(sf::Vector2i mousePosition) {
	sf::FloatRect textRect = text.getGlobalBounds();
	if (mousePosition.x > textRect.left && mousePosition.x < textRect.left + textRect.width) {
		if (mousePosition.y > textRect.top && mousePosition.y < textRect.top+textRect.height) {
			text.setFillColor(sf::Color(0, 0, 255));
			return true;
		}
	}
	text.setFillColor(_color);
	return false;
}



void Label::draw() {
	window.draw(text);
}
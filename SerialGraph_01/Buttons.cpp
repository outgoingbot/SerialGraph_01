#include "Buttons.h"


//set size, position, text
Buttons::Buttons(sf::Vector2f size, sf::Vector2f position, sf::Color color, const char* string, uint8_t (*callback)()) {
	//Load Font
	if (!_font.loadFromFile("../res/arial.ttf")) {
		printf("Error loading Font");
		system("pause");
	}

	_color = color;
	buttonRectangle.setSize(size);

	sf::FloatRect btn = buttonRectangle.getLocalBounds();
	buttonRectangle.setOrigin(btn.width / 2, btn.height / 2);
	buttonRectangle.setPosition(sf::Vector2f(position.x, WINDOW_HEIGHT - position.y));
	buttonRectangle.setFillColor(sf::Color::Blue);

	text.setFont(_font);
	text.setString(string);
	text.setCharacterSize(DEFAULT_TEXT_SIZE);
	sf::FloatRect rc = text.getLocalBounds();
	text.setOrigin(rc.width / 2, rc.height / 2);
	text.setPosition(sf::Vector2f(position.x, WINDOW_HEIGHT - position.y));
	text.setFillColor(sf::Color::White);
	buttonCallback = callback;
}


Buttons::~Buttons() {
	//delete rectangle shape and text. needed?
}



UI_State_t Buttons::getState(sf::Vector2i mousePosition) {
	UI_State_t returnVal = BUTTON_STATE_READY;

		if (isMouseOverRect(mousePosition)) {
			returnVal |= BUTTON_STATE_HOVER;
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				buttonRectangle.setFillColor(sf::Color(255, 0, 255));
				returnVal |= BUTTON_STATE_CLICK_LEFT;
				if(buttonCallback != nullptr) buttonCallback();
			}

		//	//move the object
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
				buttonRectangle.setPosition(sf::Vector2f(mousePosition.x, mousePosition.y));
				text.setPosition(sf::Vector2f(mousePosition.x, mousePosition.y));
				returnVal |= BUTTON_STATE_CLICK_RIGHT;
				//while (sf::Mouse::isButtonPressed(sf::Mouse::Left));
			}
			else {
			}
		}
	return returnVal;
}


bool Buttons::isMouseOverRect(sf::Vector2i mousePosition) {
	if (mousePosition.x > buttonRectangle.getPosition().x - (buttonRectangle.getSize().x / 2) && mousePosition.x < buttonRectangle.getPosition().x + (buttonRectangle.getSize().x / 2)) {
		if (mousePosition.y > buttonRectangle.getPosition().y - (buttonRectangle.getSize().y / 2) && mousePosition.y < buttonRectangle.getPosition().y + (buttonRectangle.getSize().y / 2)) {
			buttonRectangle.setFillColor(sf::Color::Yellow);
			return true;
		}
	}
	buttonRectangle.setFillColor(_color);
	return false;
}



void Buttons::draw(sf::RenderWindow& window) {
	window.draw(buttonRectangle);
	window.draw(text);
}


void Buttons::setTextSize(unsigned int size) {
	text.setCharacterSize(size);
}

void Buttons::setPosition(sf::Vector2f pos) {
	text.setPosition(pos);
	buttonRectangle.setPosition(pos);
}

void Buttons::setText(const std::string s) {
	text.setString(s);
}

void Buttons::setSize(sf::Vector2f size) {
	buttonRectangle.setSize(size);
}

sf::Vector2f Buttons::getSize() {
	return buttonRectangle.getSize();
}

sf::Vector2f Buttons::getPosition() {
	return buttonRectangle.getPosition();
}

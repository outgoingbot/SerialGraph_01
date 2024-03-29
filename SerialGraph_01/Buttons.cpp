#include "Buttons.h"

extern sf::RenderWindow window;
extern sf::Font font;
//extern mutex_t gui_mutex;

//set size, position, text
Buttons::Buttons(sf::Vector2f size, sf::Vector2f position, sf::Color color, const char* string, uint8_t (*callback)()) {
	_color = color;
	Button.setSize(size);

	sf::FloatRect btn = Button.getLocalBounds();
	Button.setOrigin(btn.width / 2, btn.height / 2);
	Button.setPosition(sf::Vector2f(position.x, WINDOW_HEIGHT - position.y));
	Button.setFillColor(sf::Color::Blue);

	text.setFont(font);
	text.setString(string);
	text.setCharacterSize(40);
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
				Button.setFillColor(sf::Color(255, 0, 255));
				returnVal |= BUTTON_STATE_CLICK_LEFT;
				if(buttonCallback != nullptr) buttonCallback();
			}

		//	//move the object
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
				Button.setPosition(sf::Vector2f(mousePosition.x, mousePosition.y));
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
	if (mousePosition.x > Button.getPosition().x - (Button.getSize().x / 2) && mousePosition.x < Button.getPosition().x + (Button.getSize().x / 2)) {
		if (mousePosition.y > Button.getPosition().y - (Button.getSize().y / 2) && mousePosition.y < Button.getPosition().y + (Button.getSize().y / 2)) {
			Button.setFillColor(sf::Color::Yellow);
			return true;
		}
	}
	Button.setFillColor(_color);
	return false;
}



void Buttons::draw() {
	window.draw(Button);
	window.draw(text);
}
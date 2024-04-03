#include "Buttons.h"


//set size, position, text
Buttons::Buttons(sf::Vector2f size, sf::Vector2f position, sf::Color color, const char* string, bool isDragable, uint8_t (*callback)(uint8_t), uint8_t id) {
	//Load Font
	if (!_font.loadFromFile("../res/arial.ttf")) {
		printf("Error loading Font");
		system("pause");
	}

	_color = color;
	buttonRectangle.setSize(size);

	//sf::FloatRect btn = buttonRectangle.getLocalBounds();
	//buttonRectangle.setOrigin(btn.width / 2, btn.height / 2);
	buttonRectangle.setPosition(sf::Vector2f(position.x, position.y));
	buttonRectangle.setFillColor(_color);

	text.setFont(_font);
	text.setString(string);
	text.setCharacterSize(DEFAULT_TEXT_SIZE);
	//sf::FloatRect rc = text.getLocalBounds();
	//text.setOrigin(rc.width / 2, rc.height / 2);
	text.setPosition(position);
	text.setFillColor(sf::Color::White);
	buttonCallback = callback;
	_id = id;
	_isDragable = isDragable;
}


Buttons::~Buttons() {
	//delete rectangle shape and text. needed?
}


UI_State_t Buttons::updateInteractiveState(inputState_t userInput) {
	UI_State_t returnVal = BUTTON_STATE_READY;
	buttonRectangle.setFillColor(sf::Color::Yellow);

	returnVal |= BUTTON_STATE_HOVER;
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !_mouseLeftHandled) {
		buttonRectangle.setFillColor(sf::Color(255, 0, 255));
		returnVal |= BUTTON_STATE_CLICK_LEFT;
		if(buttonCallback != nullptr) buttonCallback(_id);
		_mouseLeftHandled = true; // Prevents button from being spammed when mouse is held down
	}
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) && _mouseLeftHandled == true) {
		_mouseLeftHandled = false;
	}

	//move the object
	if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && _isDragable) {
		buttonRectangle.setPosition(sf::Vector2f(userInput.m.mousePosf.x - buttonRectangle.getSize().x/2, userInput.m.mousePosf.y - buttonRectangle.getSize().y / 2));
		text.setPosition(buttonRectangle.getPosition());
		returnVal |= BUTTON_STATE_CLICK_RIGHT;
		//while (sf::Mouse::isButtonPressed(sf::Mouse::Left));
	}
	else {
	}

	if (this->isMouseOverRect(userInput.m.mousePosf)) {
		text.setFillColor(sf::Color::Black);
		buttonRectangle.setFillColor(sf::Color::Yellow); // highlight
	}
	else {
		text.setFillColor(sf::Color::White);
		buttonRectangle.setFillColor(_color); // constructor color
	}

	// Call parent updateInteractiveState to evaluate children states
	returnVal |= UIElement::updateInteractiveState(userInput);

	return returnVal;
}


bool Buttons::isMouseOverRect(sf::Vector2f mousePosition) {
	if (mousePosition.x > buttonRectangle.getPosition().x && mousePosition.x < buttonRectangle.getPosition().x + (buttonRectangle.getSize().x)) {
		if (mousePosition.y > buttonRectangle.getPosition().y 
			&& mousePosition.y < buttonRectangle.getPosition().y + (buttonRectangle.getSize().y)) {
			return true;
		}
	}
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

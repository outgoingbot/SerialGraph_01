#include "Console.h"

TextConsole::TextConsole(sf::Vector2f size, sf::Vector2f position) {
	if (!font.loadFromFile("../res/arial.ttf")) {
		throw std::runtime_error("Could not load font");
	}

	_dock.setFillColor(sf::Color::Black);
	_dock.setOutlineColor(sf::Color::White);
	_dock.setOutlineThickness(2);
	_dock.setSize(size);
	_dock.setPosition(position);

	inputText.setFont(font);
	inputText.setFillColor(sf::Color::Yellow);
	inputText.setCharacterSize(24);
	inputText.setPosition(sf::Vector2f(position.x + 15, position.y + 15));

	displayText.setFont(font);
	displayText.setFillColor(sf::Color::Green);
	displayText.setCharacterSize(24);
	displayText.setPosition(sf::Vector2f(position.x + 15, position.y + 55));
}

void TextConsole::handleInput(const char key) {
	if (key == -1) return;
	printf_s("\r\nkey entered: %i", key);
	if (key < 128 && key != '\b' && key != '\r') {

		input += static_cast<char>(key);
	}
	else if (key == '\b') {
		if (!input.empty()) {
			input.pop_back();
		}
	}
	else if (key == '\r') {
		if (!input.empty()) {
			messages.push_back(input);
			input.clear();
		}
	}
	updateDisplayText();
}


//void TextConsole::handleInput(const sf::Event& event) {
//	if (event.type == sf::Event::TextEntered) {
//		printf_s("\r\nkey entered: %i", event.text.unicode);
//		if (event.text.unicode < 128 && event.text.unicode != '\b' && event.text.unicode != '\r') {
//			input += static_cast<char>(event.text.unicode);
//		}
//		else if (event.text.unicode == '\b') {
//			if (!input.empty()) {
//				input.pop_back();
//			}
//		}
//		else if (event.text.unicode == '\r') {
//			if (!input.empty()) {
//				messages.push_back(input);
//				input.clear();
//			}
//		}
//	}
//	updateDisplayText();
//}

void TextConsole::draw(sf::RenderWindow& window) {
	window.draw(_dock);
	window.draw(inputText);
	window.draw(displayText);
}

void TextConsole::updateDisplayText() {
	std::string display = "";
	for (const auto& msg : messages) {
		display += msg + "\n";
	}
	display += input;
	displayText.setString(display);
	inputText.setString(input);
}

sf::Vector2f TextConsole::getSize() {
	//sf::FloatRect fr = _text.getGlobalBounds();

	return _dock.getSize();
}

sf::Vector2f TextConsole::getPosition() {
	return _dock.getPosition();
}

bool TextConsole::isMouseOverRect(sf::Vector2f mousePosition) {
	if (mousePosition.x > _dock.getPosition().x && mousePosition.x < _dock.getPosition().x + _dock.getSize().x) {
		if (mousePosition.y > _dock.getPosition().y && mousePosition.y < _dock.getPosition().y + _dock.getSize().y) {
			return true;
		}
	}
	return false;
}

void TextConsole::setPosition(sf::Vector2f position) {
	//_text.setPosition(position);
	//sf::FloatRect rc = _text.getGlobalBounds();
	_dock.setPosition(position);
}


UI_State_t TextConsole::updateInteractiveState(inputState_t userInput) {
	UI_State_t returnVal = UI_STATE_READY;
	returnVal |= UI_STATE_HOVER;
	//_text.setFillColor(sf::Color::Yellow); //mouse over highlight color

	this->handleInput(userInput.k.key);


	//returnVal |= LABEL_STATE_HOVER;
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		//_text.setFillColor(sf::Color(255, 0, 255));
		//returnVal |= LABEL_STATE_CLICK_LEFT;
	}

	//move the object
	if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
		_dock.setPosition(sf::Vector2f(userInput.m.mousePosf.x - (_dock.getSize().x / 2), userInput.m.mousePosf.y - (_dock.getSize().y / 2)));
		//_graphName->setPosition(sf::Vector2f(_dock.getPosition().x+15, _dock.getPosition().y - _graphName->getSize().y - 15));
		inputText.setPosition(sf::Vector2f(_dock.getPosition().x + 15, _dock.getPosition().y + 15));
		displayText.setPosition(sf::Vector2f(_dock.getPosition().x + 15, _dock.getPosition().y + 55));
		returnVal |= UI_STATE_CLICK_RIGHT;
	}

	if (this->isMouseOverRect(userInput.m.mousePosf)) {
		//_text.setFillColor(sf::Color::Yellow);
	}
	else {
		//_text.setFillColor(_color);
	}

	// Call parent updateInteractiveState to evaluate children states
	returnVal |= UIElement::updateInteractiveState(userInput);

	return returnVal;
}
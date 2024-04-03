#include "UIElement.h"

bool UIElement::mouseOverElement(sf::Vector2f mousePosition, sf::Vector2f parentOrigin) {

	// Break up position of mouse into floats
	float mPosX = mousePosition.x;
	float mPosY = mousePosition.y;

	// Calculate global position of element
	float elPosX = this->getPosition().x + parentOrigin.x;
	float elPosY = this->getPosition().y + parentOrigin.y;

	//printf("Mouse Position: %i,%i, ElementPosition: %i,%i\r\n", mPosX, mPosY, elPosX, elPosY);

	// Caluclate if mouse position is within bounds of element
	if (mPosX > elPosX && mPosX < elPosX + (this->getSize().x)) {
		if (mPosY > elPosY && mPosY < elPosY + (this->getSize().y)) {
			return true;
		}
	}

	return false;

}

UI_State_t UIElement::updateInteractiveState(inputState_t userInput) {

	UI_State_t returnVal = UI_STATE_READY;

	///////Mouse State Update
//Mouse Position Even after Windows Resizing
	//userInput.m.mousePosf = (sf::Vector2f)mousePosition;
	//mouseState.mouseLeftClick = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	//mouseState.mouseRightClick = sf::Mouse::isButtonPressed(sf::Mouse::Right);


	if (interactedElement != nullptr) { // If we are locked into an interaction...
	// If the mouse is still down, continue interaction
		if (userInput.m.mouseLeftClick || userInput.m.mouseRightClick) {
			interactedElement->updateInteractiveState(userInput);
		}
		else { // Else, mouse is up, release lock (Falling Edge)
			interactedElement->updateInteractiveState(userInput);
			interactedElement = nullptr;
		}

	}
	else { // Else, not locked into interaction

		// for every element...
		for (auto element : _elements) {
			// If mouse is over element
			if (element->mouseOverElement(userInput.m.mousePosf, sf::Vector2f(0, 0))) {
				// Update Interactive State
				element->updateInteractiveState(userInput);

				// If we skip to here from another element that we haven't executed out exit update from,
				// updated the interacted state. If an edge case where mouse immediatly goes down, treat as interacted element.
				if (lastUpdatedElement != nullptr && lastUpdatedElement != element) {
					lastUpdatedElement->updateInteractiveState(userInput);
					if ((userInput.m.mouseLeftClick || userInput.m.mouseRightClick) && interactedElement == nullptr) {
						//sf::Mouse::setPosition((sf::Vector2i)lastUpdatedElement->getPosition()); // force mouse over lastUpdatedElement
						lastUpdatedElement->updateInteractiveState(userInput);
						// lock interaction and break
						interactedElement = lastUpdatedElement;
						std::iter_swap(_elements.end() - 1, std::find(_elements.begin(), _elements.end(), lastUpdatedElement)); // Move dragged element to front
						break;
					}
				}

				lastUpdatedElement = element; // update last updated element
				// If mouse is down over element
				if ((userInput.m.mouseLeftClick || userInput.m.mouseRightClick) && interactedElement == nullptr) {
					// lock interaction and break
					interactedElement = element;
					std::iter_swap(_elements.end() - 1, std::find(_elements.begin(), _elements.end(), element)); // Move dragged element to front
					break;
				}

			}
			else if (lastUpdatedElement == element) { // update element after mouse leaves
				lastUpdatedElement->updateInteractiveState(userInput);
				lastUpdatedElement = nullptr;

			}
		}

	}

	return returnVal;

}
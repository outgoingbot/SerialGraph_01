#include "UIElement.h"

bool UIElement::mouseOverElement(sf::Vector2i mousePosition, sf::Vector2i parentOrigin) {

	// Break up position of mouse into floats
	int mPosX = mousePosition.x;
	int mPosY = mousePosition.y;

	// Calculate global position of element
	int elPosX = this->getPosition().x + parentOrigin.x;
	int elPosY = this->getPosition().y + parentOrigin.y;

	//printf("Mouse Position: %i,%i, ElementPosition: %i,%i\r\n", mPosX, mPosY, elPosX, elPosY);

	// Caluclate if mouse position is within bounds of element
	if (mPosX > elPosX && mPosX < elPosX + (this->getSize().x)) {
		if (mPosY > elPosY && mPosY < elPosY + (this->getSize().y)) {
			return true;
		}
	}

	return false;

}

UI_State_t UIElement::updateInteractiveState(sf::Vector2i mousePosition) {

	UI_State_t returnVal = UI_STATE_READY;

	mouseState_t mouseState;

	///////Mouse State Update
//Mouse Position Even after Windows Resizing
	mouseState.mousePosf = (sf::Vector2f)mousePosition;
	mouseState.mouseLeftClick = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	mouseState.mouseRightClick = sf::Mouse::isButtonPressed(sf::Mouse::Right);


	if (interactedElement != nullptr) { // If we are locked into an interaction...
	// If the mouse is still down, continue interaction
		if (mouseState.mouseLeftClick || mouseState.mouseRightClick) {
			interactedElement->updateInteractiveState((sf::Vector2i)mouseState.mousePosf);
		}
		else { // Else, mouse is up, release lock (Falling Edge)
			interactedElement->updateInteractiveState((sf::Vector2i)mouseState.mousePosf);
			interactedElement = nullptr;
		}

	}
	else { // Else, not locked into interaction

		// for every element...
		for (auto element : _elements) {
			// If mouse is over element
			if (element->mouseOverElement((sf::Vector2i)mouseState.mousePosf, sf::Vector2i(0, 0))) {
				// Update Interactive State
				element->updateInteractiveState((sf::Vector2i)mouseState.mousePosf);

				// If we skip to here from another element that we haven't executed out exit update from,
				// updated the interacted state. If an edge case where mouse immediatly goes down, treat as interacted element.
				if (lastUpdatedElement != nullptr && lastUpdatedElement != element) {
					lastUpdatedElement->updateInteractiveState((sf::Vector2i)mouseState.mousePosf);
					if ((mouseState.mouseLeftClick || mouseState.mouseRightClick) && interactedElement == nullptr) {
						sf::Mouse::setPosition((sf::Vector2i)lastUpdatedElement->getPosition()); // force mouse over lastUpdatedElement
						lastUpdatedElement->updateInteractiveState((sf::Vector2i)mouseState.mousePosf);
						// lock interaction and break
						interactedElement = lastUpdatedElement;
						std::iter_swap(_elements.end() - 1, std::find(_elements.begin(), _elements.end(), lastUpdatedElement)); // Move dragged element to front
						break;
					}
				}

				lastUpdatedElement = element; // update last updated element
				// If mouse is down over element
				if ((mouseState.mouseLeftClick || mouseState.mouseRightClick) && interactedElement == nullptr) {
					// lock interaction and break
					interactedElement = element;
					std::iter_swap(_elements.end() - 1, std::find(_elements.begin(), _elements.end(), element)); // Move dragged element to front
					//break;
				}

			}
			else if (lastUpdatedElement == element) { // update element after mouse leaves
				lastUpdatedElement->updateInteractiveState((sf::Vector2i)mouseState.mousePosf);
				lastUpdatedElement = nullptr;

			}
		}

	}

	return returnVal;

}
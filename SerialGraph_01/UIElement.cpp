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
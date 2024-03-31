#include <iostream>
#include "uiTools.h"

namespace uiTools {
	//sf::Vector2f cornerTypeToVector(cornerType corner, sf::Vector2f bounds)
	//{
	//	switch (corner) {
	//	case TOP_RIGHT:
	//		return { bounds.x, 0 };
	//		break;
	//	case TOP_LEFT:
	//		return { 0, 0 };
	//		break;
	//	case BOTTOM_LEFT:
	//		return { 0, bounds.y };
	//		break;
	//	case BOTTOM_RIGHT:
	//		return { bounds.x, bounds.y };
	//		break;
	//	default:
	//		std::cout << "ERROR: Passed invalid corner type (myUiTools::cornerTypeToVector())\n";
	//		return sf::Vector2f();
	//		break;
	//	}
	//}


	// ======================================================
	// Text object functions

	void setObjectOrigin(sf::Text& obj, const sf::Vector2f originPoint) {
		sf::FloatRect objBounds = obj.getLocalBounds(); // get object bounds

		// necessary for text objs because of built in padding
		sf::Vector2f offset = { objBounds.left, objBounds.top };

		sf::Vector2f newOrigin = cornerTypeToVector(originPoint, { objBounds.width, static_cast<float>(obj.getCharacterSize()) }) + offset;
		obj.setOrigin(newOrigin);
	}

	void drawOutline(sf::RenderWindow& win, const sf::Text& objToDraw, sf::Color color) {
		sf::FloatRect bounds = objToDraw.getLocalBounds();
		sf::FloatRect globalBounds = objToDraw.getGlobalBounds();

		int height = static_cast<float>(objToDraw.getCharacterSize());
		sf::RectangleShape outline(sf::Vector2f(bounds.width, height));
		outline.setOrigin(0, 0);
		outline.setPosition(sf::Vector2f(globalBounds.left, globalBounds.top));
		outline.setOutlineColor(color);
		outline.setOutlineThickness(1);
		outline.setFillColor(sf::Color::Transparent);

		win.draw(outline);
	}
}
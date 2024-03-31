#include "menu.h"



Menu::Menu() {

	//Load Font
	if (!font.loadFromFile("../res/arial.ttf")) {
		printf("Error loading Font");
		system("pause");
	}
	
	// formatting
	setDockingPosition(DEFAULT_DOCKING_POSITION);
	textOriginPoint = DEFAULT_TEXT_ORIGIN_POINT;
	numElements = 0;

	text.setCharacterSize(DEFAULT_CHAR_SIZE);
	text.setFont(font);

	textColor = sf::Color::Red;

	textOutlineColor = sf::Color::Green;
	textOutline.setFillColor(textOutlineColor); // #check
	
	//background.setFillColor(sf::Color::Transparent);

	outline.setFillColor(sf::Color::Green);
	outline.setOutlineThickness(1);
	outline.setOutlineColor(sf::Color::White);

	// miscellaneous
	menuShown = true;
	componentOutlinesShown = true;
	menuBoundsShown = true;
	setBackgroundColor(sf::Color::Magenta);

}

Menu::~Menu()
{
}


bool Menu::addMenuItem(sf::RenderWindow& win, const std::string text) {	
#define SPACING 20
	// set added item properties
	sf::Text* newItem = new sf::Text;
	newItem->setCharacterSize(DEFAULT_CHAR_SIZE);
	newItem->setFont(font);
	newItem->setFillColor(textColor);
	
	newItem->setString(text);
	int newItemHeight = newItem->getCharacterSize();
	newItem->setOrigin(textOriginPoint);
	
	sf::RectangleShape* newItemOutline = new sf::RectangleShape;
	newItemOutline->setOrigin(textOriginPoint);
	newItemOutline->setFillColor(textOutlineColor);

	
	Text_Array.push_back(newItem);
	Text_Array_Outline.push_back(newItemOutline);

	if (Text_Array.size() >= 2)
	{
		//if there already exists at least on Item them place the newItem below that
		newItem->setPosition( sf::Vector2f(Text_Array[Text_Array.size() - 2]->getPosition().x, Text_Array[Text_Array.size()-2]->getPosition().y+newItemHeight+SPACING));
	}
	else
	{
		//else if this is the first item
		newItem->setPosition(sf::Vector2f(background.getPosition().x, background.getPosition().y + newItemHeight+SPACING));
	}

	sf::FloatRect textRect = newItem->getGlobalBounds();
	newItemOutline->setSize(sf::Vector2f(textRect.width*1.5, textRect.height*1.5));
	newItemOutline->setPosition(sf::Vector2f(newItem->getPosition().x, newItem->getPosition().y));

	//this needs to dynamicaly change based on the text in the vector
	background.setSize(sf::Vector2f(100,100)); 
		return true;

}





bool Menu::isMouseOverRect(sf::Vector2i mousePosition,   sf::RectangleShape *rect) {
	sf::FloatRect textRect = rect->getGlobalBounds();
	if (mousePosition.x > textRect.left && mousePosition.x < textRect.left + textRect.width) {
		if (mousePosition.y > textRect.top && mousePosition.y < textRect.top + textRect.height) {
			rect->setFillColor(sf::Color::Yellow);
			return true;
		}
	}
	rect->setFillColor(textOutlineColor);
	return false;
}

UI_State_t Menu::getState(sf::Vector2i mousePosf){
	UI_State_t returnVal = UI_STATE_READY;
		
	if (menuShown) {
		if (componentOutlinesShown) {
			for(auto Item : Text_Array_Outline) isMouseOverRect(mousePosf, Item);
		}
	
	}
	return returnVal;
}

void Menu::draw(sf::RenderWindow& win)
{

	if (menuShown) {
		//win.draw(background);
		if (componentOutlinesShown) {
			//for (int i = 0; i < Text_Array_Outline.size(); i++) isMouseOverRect(mousePosf, Text_Array_Outline[i]);
			
			//copy.setOrigin(rectOrigin);
			//copy.setPosition(itemPos);

			//draw shape
			//win.draw(copy);
			//win.draw(textOutline);
		}
		for (auto Items : Text_Array_Outline) win.draw(*Items);
		for (auto Item : Text_Array) win.draw(*Item);
	}

}


void Menu::toggleMenuShown() {
	if (!menuShown) {
		menuShown = true;
	} else {
		menuShown = false;
	}
}

bool Menu::setDockingPosition(sf::Vector2f pos)
{
	background.setPosition(pos);
	return true;
}

bool Menu::setTextOriginPoint(sf::Vector2f pos)
{
	text.setPosition(pos);
	return true;
}





void Menu::setBackgroundColor(sf::Color color)
{
	backgroundColor = color;
	background.setFillColor(backgroundColor);
}

void Menu::showMenu()
{
	menuShown = true;
}

void Menu::hideMenu()
{
	menuShown = false;
}

void Menu::showMenuBounds()
{
	menuBoundsShown = true;
}

void Menu::hideMenuBounds()
{
	menuBoundsShown = false;
}

void Menu::toggleMenuBounds()
{
	if (menuBoundsShown) {
		menuBoundsShown = false;
	}
	else {
		menuBoundsShown = true;
	}
}

void Menu::showComponentOutlines()
{
	componentOutlinesShown = true;
}

void Menu::hideComponentOutlines()
{
	componentOutlinesShown = false;
}

void Menu::toggleComponentOutlines()
{
	if (componentOutlinesShown) {
		componentOutlinesShown = false;
	}
	else {
		componentOutlinesShown = true;
	}
}

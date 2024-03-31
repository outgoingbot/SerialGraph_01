#include "menu.h"



Menu::Menu(sf::Vector2f size, sf::Vector2f position, sf::Color color, const char* string, uint8_t(*callback)()){
	// setup the dock (large rectangle that borders the entire menu. grows with addItems pushback
	_dockColor = color;
	_dock.setPosition(position);
	_dock.setFillColor(_dockColor);
	_dock.setSize(sf::Vector2f(size.x + 20, size.y + 20)); //setting the dock a bit bigger so I can see it for now
	
	//title of drop down menu. will activate on state= hover.
	//will set menu show to true
	Buttons* titleItem = new Buttons(size, position, sf::Color::Magenta, string);
	_elements.push_back(titleItem);

	//will change this to false when i get titleItem = hover working
	menuShown = true;
	componentOutlinesShown = true;

}

Menu::~Menu()
{
}


bool Menu::addMenuItem(sf::RenderWindow& win, const std::string text) {	
#define SPACING 20
	
	//I dont like this constructor. need to think about either a default constructor. will need my call back functions
	Buttons* newItem = new Buttons(sf::Vector2f(20, 20), sf::Vector2f(1340, 150), sf::Color::Magenta, "Empty");
	
	_elements.push_back(newItem);

	newItem->setTextSize(DEFAULT_CHAR_SIZE);
	newItem->setText(text);
	//need to get the Button text bounding box (based off the setText) to then set the button size;
	float newItemHeight = 30; //newItem->getTextBounds()
	float newItemWidth = 100;

	newItem->setSize(sf::Vector2f(newItemWidth, newItemHeight));
	//index element [0] on first pass to po
	newItem->setPosition(sf::Vector2f(_elements[_elements.size() - 2]->getPosition().x, _elements[_elements.size() - 2]->getPosition().y + newItemHeight + SPACING));

	//this needs to dynamicaly change based on the text in the vector
	//_dock.setSize(sf::Vector2f(100,100)); 
	
	return true;

}




UI_State_t Menu::getState(sf::Vector2i mousePosf){
	UI_State_t returnVal = UI_STATE_READY;
		
	//Not implemented yet
	if (menuShown) {
		//for(auto Item : _elements) isMouseOverRect(mousePosf, Item);
	}
	
	return returnVal;
}

void Menu::draw(sf::RenderWindow& win)
{

	if (menuShown) {
		//win.draw(_dock);
		if (componentOutlinesShown) {
			for (auto Items : _elements) Items->draw(win);
		}
	}

}


sf::Vector2f Menu::getSize() {
	return _dock.getSize();
}

sf::Vector2f Menu::getPosition() {
	return _dock.getPosition();
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
	_dock.setPosition(pos);
	return true;
}

bool Menu::setTextOriginPoint(sf::Vector2f pos)
{
	//text.setPosition(pos);
	return true;
}





void Menu::setBackgroundColor(sf::Color color)
{
	_dockColor = color;
	_dock.setFillColor(_dockColor);
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

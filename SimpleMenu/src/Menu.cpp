#include "menu.h"
#define MENU_ITEM_SHIFT 20
#define MENU_DEFUALT_COLOR 30,30,30

Menu::Menu(sf::Vector2f size, sf::Vector2f position, sf::Color color, const char* string, uint8_t(*callback)(uint8_t)){
	// setup the dock (large rectangle that borders the entire menu. grows with addItems pushback
	_dock = new sf::RectangleShape();
	_dockColor = color;
	_dockSize = size;
	_dock->setSize(_dockSize);
	_dock->setPosition(position);
	_dock->setFillColor(_dockColor);
	
	_dockOpenSize.x = _dockSize.x;//setting the dock a bit bigger so I can see it for now
	_dockOpenSize.y = _dockSize.y;
	_dockClosedSize = _dockOpenSize;
	_dock->setSize(_dockClosedSize);
	
	menuCallback = callback;
	//title of drop down menu. will activate on state= hover.

	titleItem = new Buttons(sf::Vector2f(_dockSize.x, _dockSize.y), position, sf::Color(MENU_DEFUALT_COLOR), string, false, false, menuCallback, 0);
	_elements.push_back(titleItem);
	_menuItems.push_back(titleItem);

	//will change this to false when i get titleItem = hover working
	menuShown = true;
	componentOutlinesShown = false;
	menuCallback = callback;
}

Menu::~Menu()
{
}


bool Menu::addMenuItem(const std::string text) {	
#define SPACING 10
	numElements++;
	
	//I dont like this constructor. need to think about either a default constructor. will need my call back functions
	Buttons* newItem = new Buttons(sf::Vector2f(20, 20), sf::Vector2f(20, 20), sf::Color(MENU_DEFUALT_COLOR), "Empty", false, true, menuCallback, _elements.size());
	
	_elements.push_back(newItem);
	_menuItems.push_back(newItem);

	newItem->setTextSize(DEFAULT_CHAR_SIZE_ITEM);
	newItem->setText(text);
	
	//need to get the Button text bounding box (based off the setText) to then set the button size;
	sf::FloatRect fr =  newItem->text.getGlobalBounds();
	float newItemHeight = fr.height + SPACING;
	float newItemWidth = _dockSize.x- MENU_ITEM_SHIFT;
	newItem->setSize(sf::Vector2f(newItemWidth, newItemHeight));
	

	//index element [0] on first pass to po
	newItem->setPosition(sf::Vector2f(_dock->getPosition().x + MENU_ITEM_SHIFT, _elements[_elements.size() - 2]->getPosition().y + _elements[_elements.size() - 2]->getSize().y + SPACING));

	//Some issue here indexing through _elements or its some algrbra
	_dockOpenSize.y =  _elements[_elements.size() - 1]->getPosition().y + _elements[_elements.size() - 1]->getSize().y - this->getPosition().y;
	return true;
}



UI_State_t Menu::updateInteractiveState(inputState_t userInput){
	UI_State_t returnVal = UI_STATE_READY;

	// Expand menu when mouse hovers
	if(this->mouseOverElement(userInput.m.mousePosf, sf::Vector2f(0,0))) componentOutlinesShown = true;
	else componentOutlinesShown = false;

	if (componentOutlinesShown) {
		_dock->setSize(_dockOpenSize);
	}
	else {
		_dock->setSize(_dockClosedSize);
	}

	// Call parent updateInteractiveState to evaluate children states
	returnVal |= UIElement::updateInteractiveState(userInput);

	return returnVal;
}


void Menu::draw(sf::RenderWindow& win)
{

	if (menuShown = 1) {
		win.draw(*_dock);
		
		if (componentOutlinesShown) { // List is exanded
			for (auto Items : _elements) Items->draw(win);
		}
		else {
			titleItem->draw(win); // Draw only the title item if list is collapsed
		}
	}

}


sf::Vector2f Menu::getSize() {
	return _dock->getSize();
}

void Menu::setSize(sf::Vector2f size) {
	//ToDo: implement setting the size
}

sf::Vector2f Menu::getPosition() {
	return _dock->getPosition();
}

void Menu::setPosition(sf::Vector2f pos) {
	_dock->setPosition(pos);
	_menuItems[0]->setPosition(pos); //_menuItems does not get swapped with handleinteravtiveUIState()
	for (int i = 1; i < _menuItems.size(); i++) {
		_menuItems[i]->setPosition(sf::Vector2f(_dock->getPosition().x + MENU_ITEM_SHIFT, _menuItems[i-1]->getPosition().y + _menuItems[i-1]->getSize().y + SPACING));
	}
}


void Menu::toggleMenuShown() {
	if (!menuShown) {
		menuShown = true;
	} else {
		menuShown = false;
	}
}

//bool Menu::setDockingPosition(sf::Vector2f pos)
//{
//	_dock->setPosition(pos);
//
//	//for (auto element : _elements) element->getPosition;;.setPosition(sf::Vector2f(_dock->getPosition().x + MENU_ITEM_SHIFT, _elements[_elements.size() - 2]->getPosition().y + _elements[_elements.size() - 2]->getSize().y + SPACING));
//
//	return true;
//}

bool Menu::setTextOriginPoint(sf::Vector2f pos)
{
	//text.setPosition(pos);
	return true;
}





void Menu::setBackgroundColor(sf::Color color)
{
	_dockColor = color;
	_dock->setFillColor(_dockColor);
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

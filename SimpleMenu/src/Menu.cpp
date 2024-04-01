#include "menu.h"
#define MENU_ITEM_SHIFT 20
#define MENU_DEFUALT_COLOR 30,30,30

Menu::Menu(sf::Vector2f size, sf::Vector2f position, sf::Color color, const char* string, uint8_t(*callback)(uint8_t)){
	// setup the dock (large rectangle that borders the entire menu. grows with addItems pushback
	_dockColor = color;
	_dockSize = size;
	_dock.setPosition(position);
	_dock.setFillColor(_dockColor);
	
	_dockOpenSize.x = _dockSize.x;//setting the dock a bit bigger so I can see it for now
	_dockOpenSize.y = _dockSize.y;
	_dockClosedSize = _dockOpenSize;

	_dock.setSize(_dockClosedSize); 
	
	//title of drop down menu. will activate on state= hover.
	//will set menu show to true
	Buttons* titleItem = new Buttons(sf::Vector2f(_dockSize.x, 50), position, sf::Color(MENU_DEFUALT_COLOR), string);
	_elements.push_back(titleItem);


	

	//will change this to false when i get titleItem = hover working
	menuShown = true;
	componentOutlinesShown = false;
	menuCallback = callback;
}

Menu::~Menu()
{
}


bool Menu::addMenuItem(sf::RenderWindow& win, const std::string text) {	
#define SPACING 10
	
	//I dont like this constructor. need to think about either a default constructor. will need my call back functions
	Buttons* newItem = new Buttons(sf::Vector2f(20, 20), sf::Vector2f(1340, 150), sf::Color(MENU_DEFUALT_COLOR), "Empty", menuCallback, _elements.size());
	
	_elements.push_back(newItem);

	newItem->setTextSize(DEFAULT_CHAR_SIZE_ITEM);
	newItem->setText(text);
	
	//sf::FloatRect fr =  newItem->text.getGlobalBounds();
	//need to get the Button text bounding box (based off the setText) to then set the button size;
	float newItemHeight = 30; //newItem->getTextBounds()
	float newItemWidth = _dockSize.x- MENU_ITEM_SHIFT;
	newItem->setSize(sf::Vector2f(newItemWidth, newItemHeight));
	
	
	//index element [0] on first pass to po
	newItem->setPosition(sf::Vector2f(_dock.getPosition().x + MENU_ITEM_SHIFT, _elements[_elements.size() - 2]->getPosition().y + _elements[_elements.size() - 2]->getSize().y + SPACING));

	//_dockOpenSize.x = 
	_dockOpenSize.y =  _elements[_elements.size() - 1]->getPosition().y + _elements[_elements.size() - 1]->getSize().y;
	//_dock.setSize(sf::Vector2f(_dock.getSize().x, _elements[_elements.size() - 1]->getPosition().y + _elements[_elements.size() - 1]->getSize().y));
	return true;

}




UI_State_t Menu::updateInteractiveState(sf::Vector2i mousePosf){
	UI_State_t returnVal = UI_STATE_READY;

	typedef struct {
		bool mouseLeftClick = false;
		bool mouseRightClick = false;
		sf::Vector2f mousePosf;
	} mouseState_t;

	mouseState_t mouseState;

	static UIElement* interactedElement = nullptr;
	static UIElement* lastUpdatedElement = nullptr;

	///////Mouse State Update
//Mouse Position Even after Windows Resizing
	mouseState.mousePosf = (sf::Vector2f)mousePosf;
	mouseState.mouseLeftClick = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	mouseState.mouseRightClick = sf::Mouse::isButtonPressed(sf::Mouse::Right);
	

	if(this->mouseOverElement(mousePosf, sf::Vector2i(0,0))) componentOutlinesShown = true;
	else componentOutlinesShown = false;

	if (componentOutlinesShown) {
		_dock.setSize(_dockOpenSize);
	}
	else {
		_dock.setSize(_dockClosedSize);
	}



	//Not implemented yet
	if (menuShown) {
		//for(auto Item : _elements) isMouseOverRect(mousePosf, Item);
	}





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

void Menu::draw(sf::RenderWindow& win)
{

	if (menuShown) {
		win.draw(_dock);
		_elements[0]->draw(win);
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

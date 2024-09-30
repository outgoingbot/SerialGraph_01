#include "SerialScope.h"

/* Element Handler Callbacks */
uint8_t SerialScope::handleButton_connect(uint8_t val) {
	printf("Clicked Connect %d\r\n", val);
	return 0;
}

uint8_t SerialScope::handleButton_disconnect(uint8_t val) {
	printf("Clicked Disconnect %d\r\n", val);
	return 0;
}


uint8_t SerialScope::handleMenu_1(uint8_t val) {
	printf("Menu_1 Button: %i\r\n", val);
	return 0;
}

uint8_t SerialScope::handleMenu_2(uint8_t val) {
	printf("Menu_2 Button: %i\r\n", val);
	return 0;
}

uint8_t SerialScope::handleButton_minimize(uint8_t val) {
	printf("Minimize all graphs: %i\r\n", val);
	for (int i = 0; i < NUM_GRAPHS; i++) {
		//Graph_Vector[i]->_dock.setSize(sf::Vector2f(1080, 100));
		//Graph_Vector[i]->setPosition(sf::Vector2f(1800, (i * 150) + 150));
	}
	return 0;
}

SerialScope::SerialScope(uint16_t rxBufferSz, int bytesReceived) {
	_bytesReceived = bytesReceived;

	//create the Comm port class object
	SP = new Serial();
	SP->ListComPorts();
	SP->ListBaudRates();

	if (SP->IsConnected()) printf("We're connected\r\n");

	//Load Texture
	sprite = new sf::Sprite();
	sf::Texture* texture = new sf::Texture();
	if (!texture->loadFromFile("../res/logo.png")) {
		std::cout << "Could not load enemy texture" << std::endl;
		return;
	}

	texture->setSmooth(true);
	sprite->setTexture(*texture);
	sprite->setScale(sf::Vector2f(.1, .1));
	sf::FloatRect rc2 = sprite->getLocalBounds();
	sprite->setOrigin(rc2.width / 2, rc2.height / 2);
	sprite->setPosition(sf::Vector2f(100, WINDOW_HEIGHT - 100));

	////set FPS // Should happen in main
	//window.setFramerateLimit(60); //seriously reduces the CPU/GPU utilization
	////window.setVerticalSyncEnabled(true);
	//window.setActive(true);

	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 7), sf::Vector2f(150, 300), "Graph_1", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 7), sf::Vector2f(150, 600), "Graph_2", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 7), sf::Vector2f(150, 900), "Graph_3", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 7), sf::Vector2f(150, 1200), "Graph_4", NUMFLOATS));

	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 7), sf::Vector2f(1500, 300), "Graph_5", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 7), sf::Vector2f(1500, 600), "Graph_6", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 7), sf::Vector2f(1500, 900), "Graph_7", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 7), sf::Vector2f(1500, 1200), "Graph_8", NUMFLOATS));

	// stuff elements vector with Graph_Vector
	for (auto vector : Graph_Vector) _elements.push_back(vector);


	//give me the mouse postion to help with layout
	//char charArrayMousePos[256] = "";
	//Label* mousePosText = new Label(25, sf::Vector2f(100, 700), sf::Color::Green, charArrayMousePos);
	//_elements.push_back(mousePosText);

	/*
	Buttons* Button_minimize = new Buttons(sf::Vector2f(100, 50), sf::Vector2f(2000, 0), sf::Color(10, 10, 10), "Minimize", DRAGABLE, NOTTOGGLE, this, &SerialScope::handleButton_minimize);
	_elements.push_back(Button_minimize);


	Buttons* Button_1 = new Buttons(sf::Vector2f(200, 50), sf::Vector2f(1400, 0), sf::Color(10, 10, 10), "Connect", DRAGABLE, TOGGLE, this, &SerialScope::handleButton_connect);
	_elements.push_back(Button_1);
	
	Button_2 = new Buttons<SerialScope>(sf::Vector2f(200, 50), sf::Vector2f(1650, 0), sf::Color(10, 10, 10), "Disconnect", DRAGABLE, NOTTOGGLE, this, &SerialScope::handleButton_disconnect);
	_elements.push_back(Button_2);
	*/

	//To Display the serial data received
	serialText = new Label(40, sf::Vector2f(900, 1500), sf::Color::White, "Empty");
	_elements.push_back(serialText);
	

	/*
	// create menu object
	Menu* Menu_1 = new Menu(sf::Vector2f(300, 50), sf::Vector2f(0, 0), sf::Color(10, 10, 10), "Comm Port", this, &SerialScope::handleMenu_1);
	_elements.push_back(Menu_1);

	// add menu items. list of available com ports
	for (auto ComPortName : SP->ComPortNames) {
		Menu_1->addMenuItem(ComPortName, this, &SerialScope::handleMenu_1);
	}

	Menu* Menu_2 = new Menu(sf::Vector2f(300, 50), sf::Vector2f(310, 0), sf::Color(10, 10, 10), "Baud Rate", this, &SerialScope::handleMenu_2);
	_elements.push_back(Menu_2);

	console = new TextConsole(sf::Vector2f(780, 550), sf::Vector2f(100, 100)); //Testing Console
	_elements.push_back(console);


	// add menu items. list of available com ports
	for (auto ComPortBaud : SP->ComPortBauds) {
		Menu_2->addMenuItem(ComPortBaud, this, &SerialScope::handleMenu_2);
	}
	*/
	rxBuffer = (char*)calloc(rxBufferSz, 1);
	//this is gonna need to be moved into Connect Button to start the thread
	SP->Connect(COMM_PORT);
	if (SP->IsConnected()) { // Opens thread, need to kill in deconstructor
		serial_thread = new std::thread(&Serial::ReadData, SP, rxBuffer, 1, &_bytesReceived); ///retrieve buffer
	}

}

// Destructor
SerialScope::~SerialScope() {
	SP->killThread();
	if (serial_thread != nullptr) serial_thread->join();
	free(rxBuffer); // Free dynamically allocated buffer
}

UI_State_t SerialScope::updateInteractiveState(inputState_t userInput) {
	UI_State_t returnVal = BUTTON_STATE_READY;

	auto startTime = std::chrono::high_resolution_clock::now();

	//Get Keyboard inputs
	// HANDLE IN MAIN
	//if (userInput.k.key == sf::Keyboard::Right) sprite->move(10, 0);
	//if (userInput.k.key == sf::Keyboard::Left) sprite->move(-10, 0);

	//if (userInput.k.key == sf::Keyboard::Right) sprite->move(10, 0);
	//if (userInput.k.key == sf::Keyboard::Left) sprite->move(-10, 0);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) sprite->move(10, 0);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) sprite->move(-10, 0);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) sprite->move(0, -10);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) sprite->move(0, 10);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) sprite->scale(sf::Vector2f(sprite->getScale().x + 1.0f, sprite->getScale().y + 1.0f));

	//sf::Event event;
	//while (window.pollEvent(event)) {
	//	if (event.type == sf::Event::Closed) {
	//		//window.close();
	//		SP.~Serial(); //deconstruct
	//		running = false;
	//	}
	//	else if (event.type == sf::Event::Resized) {

	//		glViewport(0, 0, event.size.width, event.size.height);
	//		// update the view to the new size of the window
	//		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
	//			sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
	//			window.setView(sf::View(visibleArea));
	//		}
	//		//SP.~Serial(); //deconstruct
	//	}
	//	else if (event.type == sf::Event::MouseWheelMoved) {
	//		// display number of ticks mouse wheel has moved
	//		//if (event.mouseWheel.delta > 0) scaler += 1.0f;
	//		//if (event.mouseWheel.delta < 0) scaler -= 1.0f;

	//	}
	//}

	returnVal |= UIElement::updateInteractiveState(userInput);
	return returnVal;
}

void SerialScope::update(inputState_t userInput) {
	// MOVE TO UPDATE()!!!!
	if (SP->payloadComplete) { // ascii to bin
		SP->payloadComplete = false;
		//printf("Data: %f, %f, %f Qs:%i pIdx:%i\r\n", SP.myData[0], SP.myData[1], SP.myData[2], SP.queueSize, SP.graphIDX);

		//label debug text. convert to terminal-ish side menu item
		char charArraySerialData[256];
		sprintf_s(charArraySerialData, "Serial Data: %f, %f, %f", SP->myData[0], SP->myData[1], SP->myData[2]);
		serialText->setText(charArraySerialData);

		//console->handleInput(userInput.k.key);

		//if (SP->graphIDX) Graph_Vector[SP->graphIDX - 1]->update(userInput, true, SP->myData);

		//for loop all the graphs
		for (int i = 0; i < NUM_GRAPHS; i++) {
			//this is from sending the correct number of floats to each graph
			//that are stored in a 1-d array [24] length
			int x = SP->graphIDX;
			int y = SP->floatsPerGraph[i];
			Graph_Vector[i]->update(userInput, true, (float*)(&(SP->myData[i * 3]))); //this sucks. use some datastructures or some vectors or some objects!!!!!!
			//ToDo: optimize(everything) the way the serialdata is moved around and converted.
			//this was for sending 3 at a time from a small buffer (3)
			//Graph_Vector[i]->update(userInput, true, SP->myData); 
		}
	}

}
void SerialScope::draw(sf::RenderWindow& win) {

	sprite->rotate(2);
	win.draw(*sprite);

	for (auto element : _elements) element->draw(win);

	//win.display(); //show drawn objects to the display buffer
}
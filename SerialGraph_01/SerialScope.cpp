#include "SerialScope.h"

extern sf::Font font;

/* Element Handler Callbacks */
static uint8_t handleButton_1(uint8_t val) {
	printf("Button1 Clicked \r\n");
	return 0;
}

static uint8_t handleMenu_1(uint8_t val) {
	printf("Menu_1 Button: %i\r\n", val);
	return 0;
}

static uint8_t handleMenu_2(uint8_t val) {
	printf("Menu_2 Button: %i\r\n", val);
	return 0;
}

SerialScope::SerialScope(uint16_t rxBufferSz, int bytesReceived) {
	_bytesReceived = bytesReceived;

	//create the Comm port class object
	SP = new Serial();
	SP->ListComPorts();
	SP->ListBaudRates();

	if (SP->IsConnected()) printf("We're connected\r\n");


	//Load Font
	if (!font.loadFromFile("../res/Pumpkin_Pancakes.ttf")) {
		printf("Error loading Font");
		system("pause");
	}

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

	Buttons* Button_1 = new Buttons(sf::Vector2f(200, 100), sf::Vector2f(1400, 1500), sf::Color::Green, "Connect", &handleButton_1);
	_elements.push_back(Button_1);

	Buttons* Button_2 = new Buttons(sf::Vector2f(200, 100), sf::Vector2f(1650, 1500), sf::Color::Red, "Disconnect", &handleButton_1);
	_elements.push_back(Button_2);

	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 10), sf::Vector2f(150, 700), "Graph_1", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 10), sf::Vector2f(150, 900), "Graph_2", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 10), sf::Vector2f(150, 1100), "Graph_3", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 10), sf::Vector2f(150, 1300), "Graph_4", NUMFLOATS));

	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 10), sf::Vector2f(1500, 700), "Graph_5", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 10), sf::Vector2f(1500, 900), "Graph_6", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 10), sf::Vector2f(1500, 1100), "Graph_7", NUMFLOATS));
	Graph_Vector.push_back(new Graph(sf::Vector2f(1200, WINDOW_HEIGHT / 10), sf::Vector2f(1500, 1300), "Graph_8", NUMFLOATS));

	// stuff elements vector with Graph_Vector
	for (auto vector : Graph_Vector) _elements.push_back(vector);

	//Graph* Graph_loopTime = new Graph(sf::Vector2f(600, WINDOW_HEIGHT / 12), sf::Vector2f(2250, 200), "LoopTime", 1);
	//_elements.push_back(Graph_loopTime);

	//char loopText[64] = "";
	//Label* loopTimeText = new Label(50, sf::Vector2f(2100, 100), sf::Color::Magenta, loopText);
	//_elements.push_back(loopTimeText);

	//give me the mouse postion to help with layout
	char charArrayMousePos[256] = "";
	Label* mousePosText = new Label(25, sf::Vector2f(100, 100), sf::Color::Green, charArrayMousePos);
	_elements.push_back(mousePosText);

	//To Display the serial data received
	char charArraySerialData[256] = "Empty";
	Label* serialText = new Label(50, sf::Vector2f(300, 100), sf::Color::Yellow, charArraySerialData);
	_elements.push_back(serialText);

	// create menu object
	Menu* Menu_1 = new Menu(sf::Vector2f(300, 50), sf::Vector2f(0, 0), sf::Color(10, 10, 10), "Comm Port", handleMenu_1);
	_elements.push_back(Menu_1);

	// add menu items. list of available com ports
	for (auto ComPortName : SP->ComPortNames) {
		Menu_1->addMenuItem(ComPortName);
	}

	Menu* Menu_2 = new Menu(sf::Vector2f(300, 50), sf::Vector2f(310, 0), sf::Color(10, 10, 10), "Baud Rate", handleMenu_2);
	_elements.push_back(Menu_2);

	// add menu items. list of available com ports
	for (auto ComPortBaud : SP->ComPortBauds) {
		Menu_2->addMenuItem(ComPortBaud);
	}

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
	//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) sprite.move(10, 0);
	//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) sprite.move(-10, 0);
	//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) sprite.move(0, -10);
	//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) sprite.move(0, 10);
	//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) sprite.scale(sf::Vector2f(sprite.getScale().x + 1.0f, sprite.getScale().y + 1.0f));

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


void SerialScope::draw(sf::RenderWindow& win) {

	sprite->rotate(2);
	win.draw(*sprite);

	for (auto element : _elements) element->draw(win);

	//win.display(); //show drawn objects to the display buffer

	// MOVE TO UPDATE()!!!!
	if (SP->payloadComplete) { // ascii to bin
		SP->payloadComplete = false;
		//printf("Data: %f, %f, %f Qs:%i pIdx:%i\r\n", SP.myData[0], SP.myData[1], SP.myData[2], SP.queueSize, SP.payloadIdx);
		//sprintf_s(charArraySerialData, "Serial Data: %f, %f, %f", SP->myData[0], SP->myData[1], SP->myData[2]);
		//serialText.setText(charArraySerialData);

		if (SP->payloadIdx) Graph_Vector[SP->payloadIdx - 1]->update(win, SP->myData);
	}
}
#ifndef RS232COMM_H_INCLUDED
#define RS232COMM_H_INCLUDED

#define ARDUINO_WAIT_TIME 1

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <iostream>
#include "CircularQueue.h"

class Serial
{
private:
	//Serial comm handler
	HANDLE hSerial;
	//Connection status
	bool connected;
	//Get various information about the connection
	COMSTAT status;
	//Keep track of last error
	DWORD errors;

public:
	//Initialize Serial communication with the given COM port
	Serial(const char *portName);
	//Close the connection
	~Serial();
	//Read data in a buffer, if nbChar is greater than the
	//maximum number of bytes available, it will return only the
	//bytes available. The function return -1 when nothing could
	//be read, the number of bytes actually read.
	void ReadData(char* buffer, unsigned int nbChar, int* returnVal);
	//Writes data from a buffer through the Serial connection
	//return true on success.
	bool WriteData(const char *buffer, unsigned int nbChar);
	//Check if we are actually connected
	bool IsConnected();
	CircularQueue *rxBuffer;

	bool mutexLock = false;
	int queueSize = 0;

	int subStringIdx = 0;
	int subStringCharIdx = 0;
	bool payloadComplete = false;
	bool firstPayload = true;

	#define TOTALFLOATS 3
	#define SUBSTRING_LEN 32
	char asciiDataString[TOTALFLOATS][SUBSTRING_LEN]; //holds the substrings to be processed
	float myData[TOTALFLOATS];


};

#endif // RS232COMM_H_INCLUDED
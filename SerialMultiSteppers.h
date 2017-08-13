/*
	THIS is one of the implements of multi-steppers, which use the serial to communicate with Arduino and let Arduino control steppers for us.

	SO, it better inherit from an abstract class to provide better adaption for future developments. A POSSIBLE ENHANCEMENT.
*/




#ifndef SerialMultiSteppers_H 
#define SerialMultiSteppers_H 

#include <iostream>
#include <vector>
#include <serial/serial.h>

using namespace std;

const char fld_separator = '@';
const char cmd_separator = '#';

#define MOVEDATATYPE double


typedef enum
{
	Acknowledge,
	Error,
	Movement,
	MovementResult,
	HeartBeat,
	HOMING
}CMDTYPE;

namespace SerialMultiSteppers {

class SerialMultiSteppers {
public:

	SerialMultiSteppers() {
		baud = 115200;
	};

	SerialMultiSteppers(uint32_t _baud) {
		baud = _baud;
	};
	~SerialMultiSteppers() {
		if (my_serial != NULL) {
			delete my_serial;
		}		
	};

	
	bool init(uint32_t baud=115200);	
	bool homing();
	std::vector<int> moveRelative(std::vector<MOVEDATATYPE> relativeSteps);

private:
	string get_device_port();
	bool heartBeat();

private:
	serial::Serial* my_serial = NULL;
	uint32_t baud;
};

}

#endif
/***
* Control multiple steppers.
*/

#include "SerialMultiSteppers.h"
#include <iostream>

using namespace std;


SerialMultiSteppers::SerialMultiSteppers my_serialMultiSteppers;


bool testInit() {
	if (my_serialMultiSteppers.init()) {
		cout << "Seems all init job is ok, let's begin our type and run program" << endl;
		return true;
	}
	else
	{		
		throw std::runtime_error("NO available port");
		return false;
	}
}

int test_move()
{
	vector<MOVEDATATYPE> moveSteps{ 20,-1.2,-2.3 };

	
	vector<int> rtn_status = my_serialMultiSteppers.moveRelative(moveSteps);

	if (rtn_status.empty()) {
		cout << "Seems no moving status returned, continue the look" << endl;
	}

	cout << "The movement status are:" << endl;
	for (auto status : rtn_status) {
		cout << status << " ";
	}
	cout << endl;

	cout << "good bye" << endl;

	return 0;
}

int test_homing()
{



	bool rtn_status = my_serialMultiSteppers.homing();

	if (rtn_status) {
		cout << "Seems homing ok" << endl;
	}
	else
	{
		cout << "Seems homing not ok" << endl;
	}

	cout << "good bye" << endl;

	return 0;
}


int main(int argc, char **argv) {

	try {
		testInit();

		test_move();
		test_homing();		
	}
	catch (exception &e) {
		cerr << "Unhandled Exception: " << e.what() << endl;
	}

	system("pause");
	return 0;
}

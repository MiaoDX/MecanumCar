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
	vector<MOVEDATATYPE> relativeDis{ 0, 47.1, 0 };

	
	vector<int> rtn_status = my_serialMultiSteppers.moveRelativeDis ( relativeDis );

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



int main(int argc, char **argv) {

	try {
		testInit();

		test_move();
	}
	catch (exception &e) {
		cerr << "Unhandled Exception: " << e.what() << endl;
	}

	system("pause");
	return 0;
}

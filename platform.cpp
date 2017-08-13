/***
* Control multiple steppers.
*/

#include "SerialMultiSteppers.h"
#include <iostream>

using namespace std;

void type_and_run(SerialMultiSteppers::SerialMultiSteppers& my_serialMultiSteppers) {
	while (true)
	{

		char op;
		MOVEDATATYPE m1, m2, m3;

		cout << "Choose what you want, 'm' move, 'h' homing, 'q' quit:";
		cin >> op;
		cout << endl;

		string cmd_s = "";

		switch (op)
		{
		case 'm': {
			cout << "Enter three relative mms or degrees: \n first one X:";
			cin >> m1;
			cout << "second one Yaw:";
			cin >> m2;
			cout << "last one Z:";
			cin >> m3;

			vector<MOVEDATATYPE> relative_move_steps{ m1, m2, m3 };

			vector<int> rtn_status = my_serialMultiSteppers.moveRelative(relative_move_steps);

			if (rtn_status.empty()) {
				cout << "Seems no moving status returned, continue the look" << endl;
				continue;
			}

			cout << "The movement status are:" << endl;
			for (auto status : rtn_status) {
				cout << status << " ";
			}
			cout << endl;

			break;
		}

		case 'h':
			bool rtn = my_serialMultiSteppers.homing();

			if (rtn == false) {
				cout << "This should not happen at all" << endl;
			}

			cout << "Homing succeeded" << endl;

			break;
		}

		if (op == 'q') break;		
		cout << "ROUND END, continue new one" << endl;
	};
}




int run(int argc, char **argv)
{

	SerialMultiSteppers::SerialMultiSteppers my_serialMultiSteppers;

	if (my_serialMultiSteppers.init()) {
		cout << "Seems all init job is ok, let's begin our type and run program" << endl;
	}
	
	type_and_run(my_serialMultiSteppers); // loop here

	cout << "good bye" << endl;

	return 0;
}




int main(int argc, char **argv) {

	try {
		return run(argc, argv);
	}
	catch (exception &e) {
		cerr << "Unhandled Exception: " << e.what() << endl;
	}
}

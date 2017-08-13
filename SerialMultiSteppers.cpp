#include "SerialMultiSteppers.h"
#include <regex>
#include <thread>


void my_sleep(unsigned long milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

string assemble_no_para_cmd_str(CMDTYPE cmd_id) {

	string cmd_str = std::to_string(cmd_id);
	cmd_str += cmd_separator;
	return cmd_str;
}

string assemble_move_cmd_str(CMDTYPE cmd_id, std::vector<MOVEDATATYPE> relativeSteps) {

	string cmd_str = std::to_string(cmd_id);

	for (auto rs : relativeSteps) {
		cmd_str += fld_separator;
		cmd_str += std::to_string(rs);
	}

	cmd_str += cmd_separator;
	return cmd_str;
}

bool check_for_ack(string recv_str) {
	if (recv_str.find(fld_separator) != string::npos && recv_str.find(cmd_separator) != string::npos) {
		return true;
	}
	return false;
}


// [Split a string using C++11](https://stackoverflow.com/a/9437426/7067150)
std::vector<std::string> split(const string& input, const string& regex) {
	// passing -1 as the sub match index parameter performs splitting
	std::regex re(regex);
	std::sregex_token_iterator
		first{ input.begin(), input.end(), re, -1 },
		last;
	return{ first, last };
}


vector<int> move_rtn_vecstr_to_vecint(vector<string> s_v) {
	vector<int> rtn(s_v.size(), 0);

	for (int i = 0; i < s_v.size(); i++) {
		rtn[i] = std::stoi(s_v[i]);
	}

	return rtn;
}


vector<int> parse_move_rtnstr(string recv_str) {
	// [Single char to string](https://stackoverflow.com/a/34269983/7067150)

	string move_stutus_str = recv_str.substr(2, recv_str.size() - 1); // 4,1,-1,2; The first two and last one should not consider
	vector<string> rtn_s = split(move_stutus_str, string(1, fld_separator));
	return move_rtn_vecstr_to_vecint(rtn_s);
}

string block_send_cmd_and_recv_res_string(serial::Serial* my_serial, string cmd_str) {
	
	cout << "Going to send the command: " << cmd_str << endl;

	my_serial->flush();
	size_t write_byte = my_serial->write(cmd_str);
	cout << write_byte << ";Command write ok" << endl;

	cout << "IDLE WAIT FOR DATA FROM THE ARDUINO" << endl;
	//my_serial.waitReadable(); // THIS IS NOT IMPLEMENTED IN WINDOWS -.-


	/**
	* There can be other Serial data show up, we take care of each line,
	* this is under assumption that the command return data is not messed up with other data
	* For example "Other info, abcdef.. 3,1,-1,0;" won't show up.
	*/
	string recv_line;
	do {		
		recv_line == std::string();
		size_t available_num = my_serial->available();
		if (available_num == 0) {
			cout << "seems no data available, going to wait two seconds" << endl;
			//serial::Timeout::simpleTimeout(2000); // Seems that this can not be called like this.
			my_sleep(2 * 1000);
			continue;
		}
		
		cout << "All available char number: " << available_num  << endl;

		recv_line = my_serial->readline();

		cout << recv_line.length() << ", String line read: " << recv_line << endl;

	} while (check_for_ack(recv_line) == false); // NOTE that, we only get the first response !!!!!!!!!!!!!!!
	

	my_serial->flush();
	return recv_line;
}


string SerialMultiSteppers::SerialMultiSteppers::get_device_port()
{
	vector<serial::PortInfo> devices_found = serial::list_ports();

	for (auto device : devices_found) {
		printf("(%s, %s, %s)\n", device.port.c_str(), device.description.c_str(),
			device.hardware_id.c_str());
	}

	if (devices_found.size() > 1) {
		cout << "Seems that there are more than one suitable available devices" << endl; // !!!!!!!!! EXCEPTION
		return std::string(); // this is empty string
	}

	if (devices_found.size() < 1) {
		cout << "Seems that there is no available devices !!" << endl;
		return std::string(); // !!!!!!!!! EXCEPTION
	}

	// ONLY ONE
	return devices_found.at(0).port.c_str();
}

bool SerialMultiSteppers::SerialMultiSteppers::init(uint32_t baud)
{

	string port = get_device_port();

	if (port.empty()) {
		cout << "seems no available port" << endl;
		return false;
	}

	my_serial = new serial::Serial(port, baud, serial::Timeout::simpleTimeout(1000)); // IT IS MORE TAHN CRUCIAL TO SET THIS TIMEOUT, SINC IT WILL MAKE SURE COMMAND WILL BE SENT, ESPECIALLY IN RASPBERRY, write method.

	cout << "Is the serial port open?";
	if (my_serial->isOpen())
		cout << " Yes." << endl;
	else {
		cout << " No." << endl;
		return false;
	}
		
	// IF open, try heartBeat
	return heartBeat();
}

bool SerialMultiSteppers::SerialMultiSteppers::homing()
{
	string cmd_str = assemble_no_para_cmd_str(CMDTYPE::HOMING);
	string recv_str = block_send_cmd_and_recv_res_string(my_serial, cmd_str);

	if (recv_str.substr(0, 1) == to_string(CMDTYPE::HOMING)) {
		return true;
	}

	return false;
}

std::vector<int> SerialMultiSteppers::SerialMultiSteppers::moveRelative(std::vector<MOVEDATATYPE> relativeSteps)
{

	vector<int> rtn(3, 0);

	string cmd_str = assemble_move_cmd_str(CMDTYPE::Movement, relativeSteps);

	string recv_str = block_send_cmd_and_recv_res_string(my_serial, cmd_str);

	if (recv_str.substr(0, 1) == to_string(CMDTYPE::MovementResult)) {
		rtn = parse_move_rtnstr(recv_str);
	}

	return rtn;
}

bool SerialMultiSteppers::SerialMultiSteppers::heartBeat()
{
	string cmd_str = assemble_no_para_cmd_str(CMDTYPE::HeartBeat);
	string recv_str = block_send_cmd_and_recv_res_string(my_serial, cmd_str);
	
	//if (recv_str.substr(0, 1) == to_string(CMDTYPE::HeartBeat)) {
	//	return true;
	//}

	//return false;
	return true;
}

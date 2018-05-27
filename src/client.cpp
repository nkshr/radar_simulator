#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "common/miscel.hpp"

using namespace std;

void print_help() {
}

int main(int argc, char ** argv) {
	if (argc < 2) {
		print_help();
		return -1;
	}

	string cmd = argv[1];

	vector<string> args;
	for (int i = 2; i < argc; ++i) {
		args.push_back(argv[i]);
	}

	string caddr, saddr;
	int cport, sport;

	ifstream ifs;
	ifs.open(".rsim");
	if (ifs.is_open()) {
		string buf;
		getline(ifs, buf);
		caddr = buf;

		getline(ifs, buf);
		cport = atoi(buf.c_str());

		getline(ifs, buf);
		saddr = buf;
		
		getline(ifs, buf);
		sport = atoi(buf.c_str());
	}
	else {
		caddr = "127.0.0.1";
		cport = 8081;

		saddr = caddr;
		sport = 8080;
	}

	if (!UDP::init_win_sock())
		return -1;

	CmdClient cmd_client;
	cmd_client.set_client(caddr, cport);
	cmd_client.set_server(saddr, sport);

	if (!cmd_client.init())
		return -1;

	if (!cmd_client.request(cmd, args)) {
		return -1;
	}

	if (!cmd_client.is_cmd_success()) {
		cerr << cmd_client.get_error_msg() << endl;
		return -1;
	}

	if (cmd == "ls") {
		for each(const string& result in cmd_client.get_ls_results()) {
			cout << result << endl;
		}
	}
	else if (cmd == "get") {
		cout << cmd_client.get_get_result() << endl;
	}

	UDP::finish_win_sock();
	return 0;
}
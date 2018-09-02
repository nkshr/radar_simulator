#include "module/module.hpp"

#include "board.hpp"

Board::CmdProcess::CmdProcess(Board* board) : board(board) {

}

bool Board::CmdModule::process(vector<string> args) {
	if (args.size() < 2) {
		msg = "Too few arguments.\n";
		msg += "module <type> <name0> <name1>...\n";
		return false;
	}

	if (args[0] == "--help") {
		msg = help;
		return true;
	}

	string& type = args[0];
	string& name = args[1];
	if (!board->create_module(type, name)) {
		msg = "Couldn't create module " + type + " " + name + ".";
		return false;
	}

	for (int i = 0; i < args.size() - 1; ++i) {
		msg = type + " " + args[i + 1] + " created.";
		if (i != args.size() - 2) {
			msg += "\n";
		}
	}
	return true;

}

bool Board::CmdSet::process(vector<string> args) {
	if (args.size() != 3) {
		msg = "Too few argument\n";
		msg += "set <module> <port> <data>";
		return false;
	}

	string& module = args[0];
	string& port = args[1];
	string& data = args[2];
	if (!board->set_data(module, port, data)) {
		msg = "Couldn't set " + data + " to " + port + " in " + module + ".";
		return false;
	}
	msg = data + " -> " + port;
	return true;
}

bool Board::CmdGet::process(vector<string> args) {
	msg = "";

	if (args.size() != 2) {
		msg = "Too few argument\n";
		msg += "get <module> <port>";
		return false;
	}

	string& module = args[0];
	string& port = args[1];
	if (!board->get_data(module, port, msg)) {
		msg = "Couldn't find " + port + " in " + module + ".";
		return false;
	}

	msg += '\0';
	return true;
}

bool Board::CmdLsMod::process(vector<string> args) {
	msg = "";
	if (!board->m_modules.size()) {
		return true;
	}

	for (ModMap::iterator it = board->m_modules.begin(); it != board->m_modules.end(); ++it) {
		msg += it->first + "\n";
	}

	msg[msg.size() - 1] = '\0';
	return true;
}

bool Board::CmdLsPort::process(vector<string> args) {
	msg = "";
	if (args.size() != 1) {
		msg = "Too few arguments.\n";
		msg += "rsim lsport <module>";
		return false;
	}

	string& mname = args[0];
	ModMap::const_iterator mm_it = board->m_modules.find(mname);
	if (mm_it == board->m_modules.end()) {
		msg = "Couldn't find " + mname + ".";
		return false;
	}

	vector<pair<string, string>> names_and_discs;
	mm_it->second->get_port_names_and_discs(names_and_discs);

	for (int i = 0; i < names_and_discs.size(); ++i) {
		string& name = names_and_discs[i].first;
		string& discs = names_and_discs[i].second;
		msg += name + " : " + discs + "\n";
	}
	if (msg.size())
		msg[msg.size() - 1] = '\0';
	return true;
}

bool Board::CmdRun::process(vector<string> args) {
	if (!args.size()) {
		msg = "";

		for (ModMap::iterator it = board->m_modules.begin(); it != board->m_modules.end(); ++it) {
			const string &name = it->first;
			Module *module = it->second;
			module->run();
			msg += name + " is  runninng.";
			if (it != --board->m_modules.end()) {
				msg += "\n";
			}
		}
		return true;
	}
	return false;
}

bool Board::CmdFinish::process(vector<string> args) {
	msg = "";
	if (!args.size()) {
		for (ModMap::iterator it = board->m_modules.begin(); it != board->m_modules.end(); ++it) {
			const string &name = it->first;
			Module *module = it->second;
			module->lock();
			module->stop();
			module->unlock();
		}

		for (ModMap::iterator it = board->m_modules.begin(); it != board->m_modules.end(); ++it) {
			const string &name = it->first;
			Module *module = it->second;
			module->join();
		}

		for (ModMap::iterator it = board->m_modules.begin(); it != board->m_modules.end(); ++it) {
			const string &name = it->first;
			Module *module = it->second;
			if (!module->finish()) {
				msg += name + " finished abnormally.";
			}
			else {
				msg += name + " fineshed.";
			}

			if (it != --board->m_modules.end()) {
				msg += "\n";
			}

		}
	}
	return true;
}

bool Board::CmdShutdown::process(vector<string> args) {
	board->m_brun = false;
	msg = "System is shutdowning.";
	return true;
}

bool Board::CmdPing::process(vector<string> args) {
	msg = "rsim running";
	return true;
}

bool Board::CmdMemory::process(vector<string> args) {
	msg = "";
	if (args.size() < 2) {
		msg = "Too few arguments.\n";
		return false;
	}

	string& type = args[0];
	string& name = args[1];
	if (!board->create_memory(type, name)) {
		msg = "Couldn't create module " + type + " " + name + ".";
		return false;
	}

	for (int i = 0; i < args.size() - 1; ++i) {
		msg = type + " " + args[i + 1] + " created.\n";
	}
	msg[msg.size() - 1] = '\0';
}

bool Board::CmdLsMem::process(vector<string> args) {
	msg = "";
	if (!board->m_memories.size()) {
		return false;
	}

	for (MemMap::iterator it = board->m_memories.begin(); it != board->m_memories.end(); ++it) {
		msg += it->first + '\n';
	}

	msg[msg.size() - 1] = '\0';
	return true;
}

bool Board::CmdConnect::process(vector<string> args) {
	msg = "";
	if (args.size() != 3) {
		msg = "Too few arguments.";
		return false;
	}

	string& mod_name = args[0];
	string& port_name = args[1];
	string& mem_name = args[2];

	ModMap::iterator it = board->m_modules.find(mod_name);
	if (it == board->m_modules.end()) {
		return false;
	}

	if (!it->second->connect_port(port_name, mem_name)) {
		msg = "Couldn't connect " + port_name + " to " + mem_name + ".";
		return false;
	}
	return true;
}
#include "module/module.hpp"

#include "board.hpp"

Board::CmdProcess::CmdProcess(Board* board) : board(board) {

}

bool Board::CmdModule::process(vector<string>& args) {
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

bool Board::CmdSet::process(vector<string>& args) {
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

bool Board::CmdGet::process(vector<string>& args) {
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

bool Board::CmdLsMod::process(vector<string>& args) {
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

bool Board::CmdLsPort::process(vector<string>& args) {
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

bool Board::CmdRun::process(vector<string>& args) {
	if (!args.size()) {
		msg = "";

		for (ModMap::iterator it = board->m_modules.begin(); it != board->m_modules.end(); ++it) {
			const string &name = it->first;
			Module *module = it->second;

			Module::STATUS status = module->get_status();
			if (status == Module::STATUS::CREATED || status == Module::STATUS::FINISHED) {
				if (!module->init()) {
					msg += "Couldn't initialize " + name + ".";
					return false;
				}
				else {
					module->set_status(Module::STATUS::INITIALIZED);
				}
			}

			module->run();
			module->set_status(Module::STATUS::RUNNING);
			msg += name + " is  runninng.";
			if (it != --board->m_modules.end()) {
				msg += "\n";
			}
		}
		return true;
	}
	return false;
}

bool Board::CmdFinish::process(vector<string>& args) {
	msg = "";

	ModMap targets;
	if (args.size()) {
		for (ModMap::iterator it = board->m_modules.begin(); it != board->m_modules.end(); ++it) {
			const string &name = it->first;
			Module *module = it->second;

			for (int i = 0; i < args.size(); ++i) {
				if (args[i] == name)
					targets.insert(*it);
			}
		}
	}
	else {
		targets = board->m_modules;
	}

	for (ModMap::iterator it = targets.begin(); it != targets.end(); ++it) {
		const string &name = it->first;
		Module *module = it->second;
		module->stop();
	}

	for (ModMap::iterator it = targets.begin(); it != targets.end(); ++it) {
		const string &name = it->first;
		Module *module = it->second;
		if(module->get_status() == Module::STATUS::RUNNING)
			module->join();
		module->set_status(Module::STATUS::STOPPED);
	}

	for (ModMap::iterator it = targets.begin(); it != targets.end(); ++it) {
		const string &name = it->first;
		Module *module = it->second;
		if (!module->finish()) {
			msg += name + " finished abnormally.";
		}
		else {
			msg += name + " fineshed.";
		}

		module->set_status(Module::STATUS::FINISHED);

		if (it != --targets.end()) {
			msg += "\n";
		}

	}
	return true;
}

bool Board::CmdShutdown::process(vector<string>& args) {
	board->m_brun = false;
	msg = "System is shutdowning.";
	return true;
}

bool Board::CmdPing::process(vector<string>& args) {
	msg = "rsim running";
	return true;
}

bool Board::CmdMemory::process(vector<string>& args) {
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
	return true;
}

bool Board::CmdLsMem::process(vector<string>& args) {
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

bool Board::CmdConnect::process(vector<string>& args) {
	msg = "";
	if (args.size() != 3) {
		msg = "Too few arguments.";
		return false;
	}

	string& mod_name = args[0];
	string& port_name = args[1];
	string& mem_name = args[2];

	ModMap::iterator mod_it = board->m_modules.find(mod_name);
	if (mod_it == board->m_modules.end()) {
		msg = "Couldn't find " + mod_name + ".";
		return false;
	}
	Module* module = mod_it->second;

	MemMap::iterator mem_it = board->m_memories.find(mem_name);
	if (mem_it == board->m_memories.end()) {
		msg = "Couldn't find " + mem_name + ".";
		return false;
	}
	Memory* memory = mem_it->second;

	if (!module->connect_memory(memory, port_name)) {
		msg = "Couldn't connect " + mem_name + " to " + port_name + " " + mod_name + ".";
		return false;
	}

	msg = mem_name + " connected to " + port_name + " at " + mod_name + ".";
	return true;
}

bool Board::CmdStop::process(vector<string>& args) {
	msg = "";
	ModMap targets;
	if (args.size()) {
		for (ModMap::iterator it = board->m_modules.begin(); it != board->m_modules.end(); ++it) {
			const string &name = it->first;
			Module *module = it->second;

			for (int i = 0; i < args.size(); ++i) {
				if (args[i] == name)
					targets.insert(*it);
			}
		}
	}
	else {
		targets = board->m_modules;
	}

	for (ModMap::iterator it = targets.begin(); it != targets.end(); ++it) {
		const string &name = it->first;
		Module *module = it->second;
		module->stop();
	}

	for (ModMap::iterator it = targets.begin(); it != targets.end(); ++it) {
		const string &name = it->first;
		Module *module = it->second;
		module->join();
		msg += name + " stopped.\n";
		module->set_status(Module::STATUS::STOPPED);
	}

	msg[msg.size() - 1] = '\0';

	return true;
}

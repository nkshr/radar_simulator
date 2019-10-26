#include "module/module.hpp"

#include "board.hpp"

SubProcess::CmdProcess::CmdProcess(SubProcess* sub_proc) : m_sub_proc(sub_proc) {

}

bool SubProcess::CmdModule::process(vector<string>& args) {
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
	if (!m_sub_proc->create_module(type, name)) {
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

bool SubProcess::CmdSet::process(vector<string>& args) {
	if (args.size() != 3) {
		msg = "Too few argument\n";
		msg += "set <module> <port> <data>";
		return false;
	}

	string& module = args[0];
	string& port = args[1];
	string& data = args[2];
	if (!m_sub_proc->set_data(module, port, data)) {
		msg = "Couldn't set " + data + " to " + port + " in " + module + ".";
		return false;
	}
	msg = data + " -> " + port;
	return true;
}

bool SubProcess::CmdGet::process(vector<string>& args) {
	msg = "";

	if (args.size() != 2) {
		msg = "Too few argument\n";
		msg += "get <module> <port>";
		return false;
	}

	string& module = args[0];
	string& port = args[1];
	if (!m_sub_proc->get_data(module, port, msg)) {
		msg = "Couldn't find " + port + " in " + module + ".";
		return false;
	}

	msg += '\0';
	return true;
}

bool SubProcess::CmdLsMod::process(vector<string>& args) {
	msg = "";
	if (!m_sub_proc->m_modules.size()) {
		return true;
	}

	for (ModMap::iterator it = m_sub_proc->m_modules.begin(); it != m_sub_proc->m_modules.end(); ++it) {
		msg += it->first + "\n";
	}

	msg[msg.size() - 1] = '\0';
	return true;
}

bool SubProcess::CmdLsPort::process(vector<string>& args) {
	msg = "";
	if (args.size() != 1) {
		msg = "Too few arguments.\n";
		msg += "rsim lsport <module>";
		return false;
	}

	string& mname = args[0];
	ModMap::const_iterator mm_it = m_sub_proc->m_modules.find(mname);
	if (mm_it == m_sub_proc->m_modules.end()) {
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

bool SubProcess::CmdRun::process(vector<string>& args) {
	bool res = true;
	msg = "";

	auto filter_modules = [&](function<void(const string &name, Module * module)> f) {
		for (ModMap::iterator it = m_sub_proc->m_modules.begin(); it != m_sub_proc->m_modules.end(); ++it) {
			const string &name = it->first;
			Module *module = it->second;

			bool is_target = false;
			if (!args.size())
				is_target = true;
			else {
				for (int i = 0; i < args.size(); ++i) {
					if (args[i] == name) {
						is_target = true;
						break;
					}
				}
			}

			if (is_target) {
				f(name, module);
			}
		}
	};

	auto turn_on_and_init = [&](const string &name, Module * module) {
		if (!module->get_main_sw_status()) {
			module->turn_on();
			msg += name + " was turned on.\n";
		}

		if (!module->get_init_status())
			module->init();
	};

	auto check_init_status = [&](const string &name, Module * module) {
		module->wait_init_reply();
		if (!module->get_init_status()) {
			msg += "Couldn't initialize " + name + "." + "\n";
			res = false;
		}
		else {
			msg += name + " was initialized." + "\n";
		}
	};

	auto run = [&](const string &name, Module * module) {
		module->run();
		msg += name + " is running." + "\n";
	};

	filter_modules(turn_on_and_init);
	filter_modules(check_init_status);
	filter_modules(run);

	//for (ModMap::iterator it = m_sub_proc->m_modules.begin(); it != m_sub_proc->m_modules.end(); ++it) {
	//	const string &name = it->first;
	//	Module *module = it->second;
	//	
	//	bool is_target = false;
	//	if (!args.size())
	//		is_target = true;
	//	else {
	//		for (int i = 0; i < args.size(); ++i) {
	//			if (args[i] == name){
	//				is_target = true;
	//				break;
	//			}
	//		}
	//	}

	//	if (is_target) {
	//		if (!module->get_main_sw_status()) {
	//			module->turn_on();
	//			msg += name + " was turned on.\n";
	//		}

	//		if (!module->get_init_status())
	//			module->init();
	//	}
	//}

	//for (ModMap::iterator it = m_sub_proc->m_modules.begin(); it != m_sub_proc->m_modules.end(); ++it) {
	//	const string &name = it->first;
	//	Module *module = it->second;
	//	module->wait_init_reply();
	//	if (!module->get_init_status()) {
	//		msg += "Couldn't initialize " + name + "." + "\n";
	//		res = false;
	//	}
	//	else {
	//		msg += name + " was initialized." + "\n";
	//	}
	//}

	//for (ModMap::iterator it = m_sub_proc->m_modules.begin(); it != m_sub_proc->m_modules.end(); ++it) {
	//	const string &name = it->first;
	//	Module *module = it->second;
	//	module->run();
	//	msg += name + " is running." + "\n";
	//}

	return res;
}

bool SubProcess::CmdFinish::process(vector<string>& args) {
	msg = "";

	ModMap targets;
	if (args.size()) {
		for (ModMap::iterator it = m_sub_proc->m_modules.begin(); it != m_sub_proc->m_modules.end(); ++it) {
			const string &name = it->first;
			Module *module = it->second;

			for (int i = 0; i < args.size(); ++i) {
				if (args[i] == name)
					targets.insert(*it);
			}
		}
	}
	else {
		targets = m_sub_proc->m_modules;
	}

	for (ModMap::iterator it = targets.begin(); it != targets.end(); ++it) {
		const string &name = it->first;
		Module *module = it->second;
		module->finish();
	}

	for (ModMap::iterator it = targets.begin(); it != targets.end(); ++it) {
		const string &name = it->first;
		Module *module = it->second;
		module->wait_finish_reply();
		if (!module->get_finish_status()) {
			msg += name + " was finished abnormally.";
		}
		else {
			module->turn_off();
			msg += name + "  was fineshed.";
		}
		if (it != --targets.end()) {
			msg += "\n";
		}

	}
	return true;
}

bool SubProcess::CmdShutdown::process(vector<string>& args) {
	m_sub_proc->m_bfinish = true;
	m_sub_proc->m_board->finish_main_proc();
	msg = "System is shutdowning.";
	return true;
}

bool SubProcess::CmdPing::process(vector<string>& args) {
	msg = "rsim running";
	return true;
}

bool SubProcess::CmdMemory::process(vector<string>& args) {
	msg = "";
	if (args.size() < 2) {
		msg = "Too few arguments.\n";
		return false;
	}

	string& type = args[0];
	string& name = args[1];
	if (!m_sub_proc->create_memory(type, name)) {
		msg = "Couldn't create module " + type + " " + name + ".";
		return false;
	}

	for (int i = 0; i < args.size() - 1; ++i) {
		msg = type + " " + args[i + 1] + " created.\n";
	}
	msg[msg.size() - 1] = '\0';
	return true;
}

bool SubProcess::CmdLsMem::process(vector<string>& args) {
	msg = "";
	if (!m_sub_proc->m_memories.size()) {
		return false;
	}

	for (MemMap::iterator it = m_sub_proc->m_memories.begin(); it != m_sub_proc->m_memories.end(); ++it) {
		msg += it->first + '\n';
	}

	msg[msg.size() - 1] = '\0';
	return true;
}

bool SubProcess::CmdConnect::process(vector<string>& args) {
	msg = "";
	if (args.size() != 3) {
		msg = "Too few arguments.";
		return false;
	}

	string& mod_name = args[0];
	string& port_name = args[1];
	string& mem_name = args[2];

	ModMap::iterator mod_it = m_sub_proc->m_modules.find(mod_name);
	if (mod_it == m_sub_proc->m_modules.end()) {
		msg = "Couldn't find " + mod_name + ".";
		return false;
	}
	Module* module = mod_it->second;

	MemMap::iterator mem_it = m_sub_proc->m_memories.find(mem_name);
	if (mem_it == m_sub_proc->m_memories.end()) {
		msg = "Couldn't find " + mem_name + ".";
		return false;
	}
	Memory* memory = mem_it->second;

	if (!module->connect_memory(memory, port_name)) {
		msg = "Couldn't connect " + mem_name + " to " + port_name + " " + mod_name + ".";
		return false;
	}

	msg = mem_name + " was connected to " + port_name + " at " + mod_name + ".";
	return true;
}

bool SubProcess::CmdStop::process(vector<string>& args) {
	msg = "";
	ModMap targets;
	if (args.size()) {
		for (ModMap::iterator it = m_sub_proc->m_modules.begin(); it != m_sub_proc->m_modules.end(); ++it) {
			const string &name = it->first;
			Module *module = it->second;

			for (int i = 0; i < args.size(); ++i) {
				if (args[i] == name)
					targets.insert(*it);
			}
		}
	}
	else {
		targets = m_sub_proc->m_modules;
	}

	for (ModMap::iterator it = targets.begin(); it != targets.end(); ++it) {
		const string &name = it->first;
		Module *module = it->second;
		module->stop();
		msg += name + " was stopped.\n";
	}

	if (msg.size())
		msg[msg.size() - 1] = '\0';
	else
		msg = "No module was stopped.\0";
	return true;
}

#include "module/module.hpp"

#include "command.hpp"

CmdProcess::CmdProcess(Board* board) : m_board(board) {

}

const string& CmdProcess::get_msg() const {
	return m_msg;
}

const string& CmdProcess::get_name() const {
	return m_name;
}

CmdModule::CmdModule(Board* board) : CmdProcess(board) {
}
bool CmdModule::process(vector<string> args) {
	if (args.size() < 2) {
		m_msg = "Too few arguments.\n";
		m_msg += "module <type> <name0> <name1>...";
		return false;
	}

	if (args[0] == "--help") {
		vector<string> types = m_board->get_module_types();

		m_msg = "";
		for (int i = 0; i < types.size(); ++i) {
			m_msg += types[i] + '\n';
		}
		m_msg[m_msg.size() - 1] = '\0';
		return true;
	}

	string& type = args[0];
	string& name = args[1];
	if (!m_board->create_module(type, name)) {
		m_msg = "Couldn't create module " + type + " " + name + ".";
		return false;
	}

	for (int i = 0; i < args.size() - 1; ++i) {
		m_msg = type + " " + args[i + 1] + " created.";
		if (i != args.size() - 2) {
			m_msg += "\n";
		}
	}
	return true;
}

bool CmdSet::process(vector<string> args) {
	if (args.size() != 3) {
		m_msg = "Too few argument\n";
		m_msg += "set <module> <port> <data>";
		return false;
	}

	string& module = args[0];
	string& port = args[1];
	string& data = args[2];
	if (!m_board->set_data(module, port, data)) {
		m_msg = "Couldn't set " + data + " to " + port + " of " + module + ".";
		return false;
	}
	return true;
}

bool CmdGet::process(vector<string> args) {
	if (args.size() != 2) {
		m_msg = "Too few argument\n";
		m_msg += "get <module> <port>";
		return false;
	}

	string& module = args[0];
	string& port = args[1];
	if (m_board->get_data(module, port, m_msg)) {
		m_msg = "Couldn't find " + port + " of " + module + ".";
		return false;
	}
	return true;
}

bool CmdLsMod::process(vector<string> args) {
	m_msg = "";
	vector<string> names = m_board->get_module_names();

	if (names.size() == 0) {
		return true;
	}

	m_msg = "";
	for (int i = 0; i < names.size(); ++i) {
		m_msg += names[i] + '\n';
	}
	m_msg[m_msg.size() - 1] = '\0';
	return true;
}

bool CmdLsPort::process(vector<string> args) {
	m_msg = "";
	if (args.size() != 1) {
		m_msg = "Too few arguments.\n";
		m_msg += "rsim lsport <module>";
		return false;
	}

	string& mname = args[0];
	vector<string> names;
	if (!m_board->get_port_names(mname, names)) {
		m_msg = "Couldn't find " + mname + ".";
		return false;
	}

	for (int i = 0; i < names.size(); ++i) {
		m_msg += names[i] + "\n";
	}
	m_msg[m_msg.size() - 1] = '\0';
	return true;
}

bool CmdRun::process(vector<string> args) {
	if (!args.size()) {
		ModMap modules = m_board->get_modules();
		for (ModMap::iterator it = modules.begin(); it != modules.end(); ++it) {
			const string &name = it->first;
			Module *module = it->second;			
			module->run();
		}
		return true;
	}
	return false;
}

bool CmdFinish::process(vector<string> args) {
	m_board->finish();
	return true;
}

bool CmdPing::process(vector<string> args) {
	m_msg = "rsim running";
	return true;
}

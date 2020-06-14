#pragma once
#include <string>
#include <functional>
#include <vector>
#include <mutex>
#include <map>

using std::map;
using std::function;
using std::string;
using std::vector;
using std::mutex;
using std::pair;

typedef function<bool(const string&)> PortSetCallback;
typedef function<bool(const string&, bool*)> BoolSetCallback;
typedef function<bool(const string&, int*)> IntSetCallback;
typedef function<bool(const string&, double*)> DoubleSetCallback;
typedef function<bool(const string&, string*)> StringSetCallback;
typedef function<bool(const string&, const vector<string>&, void*)> EnumSetCallback;

typedef function<string()> PortGetCallback;
typedef function<string(const bool*)> BoolGetCallback;
typedef function<string(const int*)> IntGetCallback;
typedef function<string(const double*)> DoubleGetCallback;
typedef function<string(const string*)> StringGetCallback;
typedef function<string(const vector<string>& strs, const void*)> EnumGetCallback;

bool smpl_bsc(const string& in, bool* out);
bool smpl_isc(const string& in, int* out);
bool smpl_dsc(const string& in, double* out);
bool smpl_ssc(const string& in, string* out);
bool smpl_esc(const string& in, const vector<string>& strs, void* out);

string smpl_bgc(const bool* in);
string smpl_igc(const int* in);
string smpl_dgc(const double* in);
string smpl_sgc(const string* in);
string smpl_egc(const vector<string>& strs, const void* in);

class Memory;

struct Port {
	enum TYPE {
		BOOL,
		INT,
		DOUBLE,
		STRING,
		ENUM,
		MEMORY,
		CALLBACK_FUNC
	};

	string name;
	string disc;

	vector<string> strs;

	union Data {
		bool* b;
		int* i;
		double* d;
		string* s;
		void* e;
	}data;

	Memory** mem;

	Port::TYPE type;

	PortSetCallback psc;
	BoolSetCallback bsc;
	IntSetCallback isc;
	DoubleSetCallback dsc;
	StringSetCallback ssc;
	EnumSetCallback esc;

	PortGetCallback pgc;
	BoolGetCallback bgc;
	IntGetCallback igc;
	DoubleGetCallback dgc;
	StringGetCallback sgc;
	EnumGetCallback egc;
};

typedef map<const string, Port*> PortMap;

class ModuleBase {
protected:
	mutex m_lock;

	PortMap m_ports;

	virtual bool init_process() {
		return true;
	};
	virtual bool main_process() {
		return true;
	};
	virtual bool finish_process() {
		return true;
	};

	void register_bool(const string& name, const string& disc,
		bool init_status, bool* status, BoolSetCallback sc = smpl_bsc, BoolGetCallback gc = smpl_bgc);
	void register_int(const string& name, const string& disc,
		int init_val, int* val, IntSetCallback sc = smpl_isc, IntGetCallback gc = smpl_igc);
	void register_double(const string& name, const string& disc,
		double init_val, double* val, DoubleSetCallback sc = smpl_dsc, DoubleGetCallback gc = smpl_dgc);
	void register_string(const string& name, const string& disc,
		string init_str, string* str, StringSetCallback sc = smpl_ssc, StringGetCallback gc = smpl_sgc);

	template <typename T>
	void register_enum(const string& name, const string& disc,
		T init_val, T* val, const vector<string>& strs, EnumSetCallback sc = smpl_esc, EnumGetCallback gc = smpl_egc) {
		Port* port = new Port;
		port->name = name;
		port->disc = disc;
		port->data.e = (void*)val;
		*static_cast<T*>(val) = init_val;
		port->type = Port::TYPE::ENUM;
		port->esc = sc;
		port->egc = gc;
		port->strs = strs;
		m_ports.insert(pair<const string, Port*>(port->name, port));
	}

public:
	void lock();
	void unlock();

};
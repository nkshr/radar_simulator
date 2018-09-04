#include "module.hpp"

using std::cout;
using std::endl;

class RsimTest : public Module {
protected:
	bool m_b;
	int m_i;
	double m_d;
	string m_s;
	MemBool* m_pmem_bool;

public:
	RsimTest() : Module(), m_pmem_bool(NULL){
		register_port("b", "boolean value (default y)", true, &m_b);
		register_port("i", "integer value (default 0)", 0, &m_i);
		register_port("d", "double value (default 0)", 0, &m_d);
		register_port("s", "string value (default abc)", "abc", &m_s);
		register_port("mb", "boolean memory",(Memory**)(&m_pmem_bool));
		if (!m_pmem_bool) {
			//return false;
		}

	}

	virtual bool init() {
		if (!m_pmem_bool) {
			return false;
		}

		return true;
	};

	virtual bool process() {
		if (!m_pmem_bool) {
			return false;
		}

		m_pmem_bool->lock();
		m_pmem_bool->set_status(m_b);
		m_pmem_bool->unlock();

		cout << "b : " << bool_to_str(m_b) << endl;
		cout << "i : " << m_i << endl;
		cout << "d : " << m_d << endl;
		cout << "s : " << m_s << endl;

		m_pmem_bool->lock();
		cout << "mb : " << bool_to_str(m_pmem_bool->get_status()) << endl;
		m_pmem_bool->unlock();

		//cout << m_clock.get_cur_time() << endl;

		return true;
	};

	virtual bool finish() {
		return true;
	};

};
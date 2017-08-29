#include <vector>

struct  Object {
	double x, y, z;

};

struct Radar {
	double x, y, z;
};

class Simulator{
private:
	std::vector<Object> m_objects;
	Radar m_radar;

public:
};
#include <iostream>
#include <fstream>

#include  "common/math.hpp"
#include "board.hpp"

int main()
{

	Board board;

	if (!board.init()) {
		cerr << "Board initialization failure." << endl;
		return -1;
	}

	board.run();

	board.finish();

	return 0;
}
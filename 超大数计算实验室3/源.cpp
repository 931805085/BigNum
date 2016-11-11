#include <iostream>
#include "BFigure.h"
#include <time.h>

int main()
{
	BFigure E(1, 1), D(1, 1), C(1, 1);
	
	clock_t t1, t2;
	t1 = clock();
	for (int a = 1; a < 100; a++)
	{
		E = "553155315531553155315531553.0";
		D = "553155315531553155553155555.0";
		C = E + D;
	}
	t2 = clock();
	std::cout << C.toString() << std::endl;
	std::cout << t2 - t1 << std::endl;

	/*
	void _ChunktoStr_Float(int ChunkNum, char *buffer);
	char tt[9];
	_ChunktoStr_Float(25000000, tt);
	std::cout << tt << std::endl;;
*/

	//C.Display();
	//std::cout << C.toString() << std::endl;
	system("pause");
}


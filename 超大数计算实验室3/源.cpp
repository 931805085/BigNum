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
		E = "553155315531553155315531553.3155315531553155315531";
		D = "553155315531553155553155555.1115531515531553";
		C = E + D;
	}
	t2 = clock();
	std::cout << t2 - t1 << std::endl;


	C.Display();
	system("pause");
}


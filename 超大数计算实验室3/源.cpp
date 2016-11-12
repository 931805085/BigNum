#include <iostream>
#include "BFigure.h"
#include <time.h>

int main()
{
	BFigure A(1, 0), B(1,0), C(3, 3);
	A = "-22121";
	B = "22111";

	C = A + B;
	//std::cout << (A + B).toString() << std::endl;
	
	//C.Display();
	std::cout << C.toString() << std::endl;
	system("pause");
}


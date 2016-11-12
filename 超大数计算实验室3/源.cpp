#include <iostream>
#include "BFigure.h"
#include <time.h>

int main()
{
	BFigure A(1, 1), B(1, 1), C(3, 3);
	A = "-22121.0";
	B = "-22111.2";

	//void _Sub_Int(BFigure&Result, const BFigure & OperandA, const BFigure & OperandB, int borrow);
	//int _Sub_Float(BFigure&Result, const BFigure & OperandA, const BFigure & OperandB, int borrow);

	int CompareBFigure(const BFigure &OperandA, const BFigure &OperandB);

	//_Sub_Int(C, A, B, _Sub_Float(C, A, B));

	/*
	clock_t t1, t2;
	t1 = clock();
	for (long long a = 0; a < 100000000; a++)
		CompareBFigure(A, B);
	t2 = clock();
	std::cout << t2 - t1 << std::endl;
	std::cout << CompareBFigure(A, B) << std::endl;
	*/
	//C.Display();
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


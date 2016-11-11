#pragma once
#include <string>
#include <cstdlib>

/*
临时测试
*/
#include <iostream>

#define DEFAULT_MALLOC_INT 20		//1即表示4位,20最多可保存80位
#define EFAULT_MALLOC_FLOAT 8		//1即表示8位,8最多表示64位

/*
const int DEFAULT_MALLOC_INT_SIZE=DEFAULT_MALLOC_INT*4;
const int EFAULT_MALLOC_FLOAT_SIZE=EFAULT_MALLOC_FLOAT*8;
*/

class BFigure
{
public:
	typedef char FLAG;
	BFigure();
	BFigure(int ChunkIntCount);
	BFigure(int ChunkIntCount, int ChunkFloatCount);
	BFigure(std::string NumString, int flag);		//可以用一个数字字符串初始化一个类,但分配的空间只跟string的长度一样大小
	BFigure(const BFigure& Base);					//复制构造函数
	~BFigure(void);

	void StrToFig_Int(std::string String);
	void StrToFig_Float(std::string String);
	std::string toString();
	friend void BFigureCopyer(BFigure &destin, const BFigure &Source, int CompleteCopy = 1);

	friend void Add_Int(BFigure &Result, const BFigure &OperandA, const BFigure &OperandB);
	friend void Add_Float(BFigure &Result, const BFigure &OperandA, const BFigure &OperandB);


	//void Sub(BFigure SperandA, BFigure SperandB);

	/*
	TODO:

	*/

	//*******************临时函数,完工后删除*********************
	void Display();
	//**********************************************************


	//运算符重载函数
	BFigure& operator=(const BFigure& rhs);
	BFigure& BFigure::operator=(std::string NumString);	//将字符串写入到BFigure中
	BFigure operator+(const BFigure& rhs);

	/*
	+ - * / % = <<
	++ --
	*/
protected:
	FLAG *flag;				//标记这个数据的类型 0表示整数   1表示浮点数
	FLAG *Minus;			//标记这个数是否为负数	0表示正数   1表示负数
	int *ChunkInt = NULL;	//整数部分的存储块
	int *ChunkFloat = NULL;	//浮点数部分的存储块
	int *ChunkInt_C;		//Int块中存放数据的元素个数
	int *ChunkFloat_C;		//Float块中存放数据的元素个数
	int *pFigureCount;		//Figure的引用计数
	void _StrToFig_Float(std::string, int start_index, int end_index);
	void _StrToFig_Int(std::string String, int start_index, int end_index);

	friend void _Add_Int(BFigure &Result, const BFigure &OperandA, const BFigure &OperandB, int carry);
	friend int _Add_Float(BFigure &Result, const BFigure &OperandA, const BFigure &OperandB);
public:	//临时的

	friend int _GetRealLength_Int(const BFigure &Figure);
	friend int _GetRealLength_Float(const BFigure &Figure);
};


/*
TODO:
增强健壮性,防止NULL指针
有字符串 初始化时不要先初始化数组			OK

修改=重载,使之在内存足够时,不进行重新分配	OK
重载+号运算符,使之支持BFigure+int

*/
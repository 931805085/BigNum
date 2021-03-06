#include "BFigure.h"

/*
该源文件私有的方法
*/
static int _FindDecimalPoint(const char *NumString, int length);
static int _ItoA(int Num, char *buffer, int size);
static void _ChunktoStr_Float(int ChunkNum, char *buffer);
static void _ChunktoStr_Int(int ChunkNum, char *buffer);
static int _CompareNumString(const char *StringA, const char *StringB);

/*
约定:
整数块中低维到高维分别存储低位到高位
浮点块中低维到高维分别存储低分位到高分位
*/


/*
默认的构造函数
如果没有指定参数,则直接使用头文件中指定的默认数据进行初始化
可存储DEFAULT_MALLOC_INT*4位整数部分
可存储EFAULT_MALLOC_FLOAT*8位整数部分
*/
BFigure::BFigure()
{
	flag = new FLAG(1);								//默认为整数
	ChunkInt = new int[DEFAULT_MALLOC_INT]();		//初始化整数块
	ChunkFloat = new int[EFAULT_MALLOC_FLOAT]();	//初始化浮点块
	ChunkInt_C = new int(DEFAULT_MALLOC_INT);
	ChunkFloat_C = new int(EFAULT_MALLOC_FLOAT);
	pFigureCount = new int(1);						//初始化引用计数
	Minus = new FLAG(0);
}

/*
当如果传入一个int,则用此数据初始化int数组
这个构造函数只能将对象初始化为一个整数
可存储ChunkIntCount*4位整数部分
*/
BFigure::BFigure(int ChunkIntCount)
{
	flag = new FLAG(0);							//设置为整数
	ChunkInt = new int[ChunkIntCount]();		//初始化整数块
	ChunkInt_C = new int(ChunkIntCount);
	ChunkFloat_C = new int(0);
	pFigureCount = new int(1);					//初始化引用计数
	Minus = new FLAG(0);
}

/*
这个函数可以初始化一个数值为0的对象
可存储ChunkIntCount*4位整数部分
可存储ChunkFloatCount*8位整数部分
*/
BFigure::BFigure(int ChunkIntCount, int ChunkFloatCount)
{
	ChunkFloat_C = new int(ChunkFloatCount);
	ChunkInt_C = new int(ChunkIntCount);
	ChunkInt = new int[ChunkIntCount]();			//初始化整数块
	if (ChunkFloatCount) {
		flag = new FLAG(1);							//设置为浮点数
		ChunkFloat = new int[ChunkFloatCount]();	//初始化浮点块
	}
	else
		flag = new FLAG(0);	//当ChunkFloatCount为0时,也将BFigure初始化为一个整数

	pFigureCount = new int(1);						//初始化引用计数				
	Minus = new FLAG(0);
}

/*
这个函数允许用一个字符串
当flag为0时,只会将NumString认为一个整数,忽略小数点
当flag为1时,可以正确读取小数部分
*/
BFigure::BFigure(std::string NumString, int flag = 1)
{
	int P_p;						//小数点的位置
	int Stringlen = (int)NumString.length();
	Minus = new FLAG(0);			//将数字设置为正数

									//std::cout << "构造" << std::endl;

	this->flag = new FLAG(flag);								//默认为浮点数
	if (flag == 0) {
		//传入的字符串中的数字为整形
		ChunkInt_C = new int(0);
		ChunkFloat_C = new int(0);				//虽然不需要用到,但还是需要初始化
		_StrToFig_Int(NumString, 0, Stringlen - 1);
	}
	else if (flag == 1) {
		//传入的字符串中的数字是浮点型
		P_p = _FindDecimalPoint(NumString.c_str(), Stringlen);		//获取小数点的位置
		if (P_p == -1) P_p = Stringlen;
		ChunkInt_C = new int(0);
		_StrToFig_Int(NumString, 0, P_p - 1);
		ChunkFloat_C = new int(0);
		_StrToFig_Float(NumString, P_p + 1, Stringlen - 1);
	}
	pFigureCount = new int(1);
}

/*
复制构造函数
*/
BFigure::BFigure(const BFigure & Source)
{
	//std::cout << "复制构造" << std::endl;
	this->pFigureCount = Source.pFigureCount;
	*this->pFigureCount += 1;
	this->ChunkInt = Source.ChunkInt;
	this->ChunkInt_C = Source.ChunkInt_C;
	if (flag) {
		this->ChunkFloat = Source.ChunkFloat;
		this->ChunkFloat_C = Source.ChunkFloat_C;
	}
	this->flag = Source.flag;
	this->Minus = Source.Minus;
}

/*
BFigure析构函数
*/
BFigure::~BFigure(void)
{
	*pFigureCount -= 1;
	//std::cout << "析构" << std::endl;
	if (!*pFigureCount) {
		//std::cout << "析构 删除" << std::endl;
		//当引用计数为0时,delete所有对象的指针指向的内存
		delete[] ChunkInt;
		delete ChunkInt_C;
		delete Minus;
		if (*flag || *ChunkFloat_C) {
			delete[] ChunkFloat;
			delete ChunkFloat_C;
		}
		delete flag;
		delete pFigureCount;
	}
}

/*
将字符串转换为Figure对象
约定:如果传入的字符串含有多个负号,则只读取最后一个负号后面的内容
*/
void BFigure::StrToFig_Int(std::string String)
{
	*flag = 0;
	_StrToFig_Int(String, 0, (int)(String.length() - 1));
}

/*
将字符串转换为Figure对象
约定:如果传入的字符串含有多个负号,则只读取最后一个负号后面的内容
约定:如果传入的字符串含有多个小数点,只取最后一个小数点,其他小数点忽略
*/
void BFigure::StrToFig_Float(std::string String)
{
	int P_p = _FindDecimalPoint(String.c_str(), (int)String.length());		//获取小数点的位置
	if (P_p == -1) P_p = (int)String.length();
	*flag = 1;

	_StrToFig_Int(String, 0, P_p - 1);
	_StrToFig_Float(String, P_p + 1, (int)String.length() - 1);
}

/*
将对象中的数字转为字符串返回
*/
std::string BFigure::toString()
{
	char ChunkTemp[9];
	char *returnVal = NULL;
	int returnVal_len;

	if (*flag)
		returnVal_len = _GetRealLength_Int(*this) + _GetRealLength_Float(*this) + 3 + *this->Minus;
	else
		returnVal_len = _GetRealLength_Int(*this) + 1+ *this->Minus;
	if (returnVal_len <= 0)
		return NULL;
	returnVal = new char[returnVal_len];

	int a = *ChunkInt_C - 1;
	while (ChunkInt[a] == 0)
		a--;

	int n = _ItoA(ChunkInt[a--], ChunkTemp, 9);
	if (*Minus==1)
	{
		returnVal[0] = '-';
		strcpy(returnVal + 1, &ChunkTemp[n]);	//写入前端
	}
	else
		strcpy(returnVal, &ChunkTemp[n]);	//写入前端

	if (*ChunkInt_C&&*ChunkInt) {		//显示整数部分
		for (; a >= 0; a--) {
			_ChunktoStr_Int(ChunkInt[a], ChunkTemp);
			strcat(returnVal, ChunkTemp);
		}
	}

	if (*flag&&*ChunkFloat_C) {			//是一个小数,显示小数部分
		strcat(returnVal, ".");
		for (int a = 0; a < *ChunkFloat_C; a++) {
			_ChunktoStr_Float(ChunkFloat[a], ChunkTemp);
			strcat(returnVal, ChunkTemp);
		}
	}
	std::string re = std::string(returnVal);
	delete[] returnVal;
	return re;
}

/*
任意整数的加法
只能计算整数,如果传入浮点数将会忽略小数点后的数字
本函数主要用于调配和处理正负号
*/
void Add_Int(BFigure & Result, const BFigure & OperandA, const BFigure & OperandB)
{
	int MinusA = *OperandA.Minus;
	int MinusB = *OperandB.Minus;

	if (MinusA&&MinusB)
	{
		//两个负数
		_Add_Int(Result, OperandA, OperandB, 0);
		*Result.Minus = 1;
	}
	else if ((!MinusA) && MinusB) {
		//A正B负
		if (_CompareBFigure(1,OperandA, OperandB) >= 0)
		{
			//2+(-1)
			_Sub_Int(Result, OperandA, OperandB, 0);
			*Result.Minus = 0;
		}
		else {
			//2+(-3)
			_Sub_Int(Result, OperandB, OperandA, 0);
			*Result.Minus = 1;
		}
	}
	else if (MinusA && (!MinusB)) {
		//A负B正
		if (_CompareBFigure(1, OperandA, OperandB) > 0)
		{
			//(-3)+1
			_Sub_Int(Result, OperandA, OperandB, 0);
			*Result.Minus = 1;
		}
		else
		{
			//(-1)+3
			_Sub_Int(Result, OperandB, OperandA, 0);
			*Result.Minus = 0;
		}
	}
	else {
		//两个正数
		_Add_Int(Result, OperandA, OperandB, 0);
		*Result.Minus = 0;
	}
	*Result.flag = 0;
}

/*
任意浮点数的加法
本函数主要用于调配和处理正负号
*/
void Add_Float(BFigure & Result, const BFigure & OperandA, const BFigure & OperandB)
{
	int MinusA = *OperandA.Minus;
	int MinusB = *OperandB.Minus;

	if (MinusA&&MinusB)
	{
		//两个负数
		_Add_Int(Result, OperandA, OperandB, _Add_Float(Result, OperandA, OperandB));
		*Result.Minus = 1;
	}
	else if ((!MinusA) && MinusB) {
		//A正B负
	}
	else if (MinusA && (!MinusB)) {
		//A负B正
	}
	else {
		//两个正数
		_Add_Int(Result, OperandA, OperandB, _Add_Float(Result, OperandA, OperandB));
		*Result.Minus = 0;
	}
	*Result.flag = 1;
}

/*
整数部分的减法核心(不安全)
不支持负号,且必须满足OperandA>=OperandB
不判断Result容量是否充足,请保证Result容量充足
*/
void _Sub_Int(BFigure & Result, const BFigure & OperandA, const BFigure & OperandB, int borrow)
{
	int Amax_p, Bmax_p, R_p;			//分别是OperandA和OperandB的最大下标及结果的浮标

	Amax_p = *OperandA.ChunkInt_C - 1;
	Bmax_p = *OperandB.ChunkInt_C - 1;

	for (R_p = 0; R_p <= Amax_p&&R_p <= Bmax_p; R_p++) {
		Result.ChunkInt[R_p] = OperandA.ChunkInt[R_p] - OperandB.ChunkInt[R_p] - borrow;
		if (Result.ChunkInt[R_p] < 0) {
			borrow = (-Result.ChunkInt[R_p]) / 10000 + 1;
			Result.ChunkInt[R_p] += 10000;
		}
		else borrow = 0;
	}
	while (R_p <= Amax_p) {
		Result.ChunkInt[R_p] = OperandA.ChunkInt[R_p] - borrow;
		if (Result.ChunkInt[R_p] < 0) {
			borrow = (-Result.ChunkInt[R_p]) / 10000 + 1;
			Result.ChunkInt[R_p] += 10000;
		}
		else borrow = 0;
		R_p++;
	}
	if (borrow&&R_p < *Result.ChunkInt_C)
		Result.ChunkInt[R_p++] = borrow;
	while (R_p < *Result.ChunkInt_C)
		Result.ChunkInt[R_p++] = 0;
	return;
}

/*
浮点数部分减法核心(不安全)
不支持负号
不判断Result容量是否充足,请保证Result容量充足
返回值为向整数块的借位数
*/
int _Sub_Float(BFigure & Result, const BFigure & OperandA, const BFigure & OperandB)
{
	int Amax_p, Bmax_p, R_p;

	Amax_p = *OperandA.ChunkFloat_C - 1;
	Bmax_p = *OperandB.ChunkFloat_C - 1;
	R_p = *Result.ChunkFloat_C - 1;

	Amax_p = Amax_p >= 0 && *OperandA.flag ? Amax_p : -1;//如果是整数,则将该值设置为-1;
	Bmax_p = Bmax_p >= 0 && *OperandB.flag ? Bmax_p : -1;//如果是整数,则将该值设置为-1;

	if (Amax_p == -1 && Bmax_p == -1) return 0;
	for (int a = R_p; a > Amax_p&&a > Bmax_p; a--)		//刷新Result最后的0
		Result.ChunkFloat[a] = 0;

	int borrow = 0;							//借位初始化
	if (Amax_p > Bmax_p) {					//找出没有数可以相减的情况
		//如果OperandA的位数大于OperandB的位数
		while (Amax_p > Bmax_p&&Bmax_p >= -1) {
			Result.ChunkFloat[Amax_p] = OperandA.ChunkFloat[Amax_p];
			Amax_p--;
		}
	}
	else {
		//如果OperandB的位数大于OperandA的位数
		while (Amax_p < Bmax_p&&Amax_p >= -1) {
			Result.ChunkFloat[Bmax_p] = 100000000 - OperandB.ChunkFloat[Bmax_p] - borrow;
			Bmax_p--;
			borrow = 1;
		}
	}

	//运行到这里,已经可以确保Amax_p==Bmax_p了
	while (Amax_p >= 0) {
		Result.ChunkFloat[Amax_p] = OperandA.ChunkFloat[Amax_p] - OperandB.ChunkFloat[Amax_p] - borrow;
		if (Result.ChunkFloat[Amax_p] < 0) {
			Result.ChunkFloat[Amax_p] *= -1;
			borrow = Result.ChunkFloat[Amax_p] / 100000000 + 1;
			Result.ChunkFloat[Amax_p] = 100000000 - Result.ChunkFloat[Amax_p];
		}
		Amax_p--;
	}
	return borrow;
}

/*
整数部分的加法核心(不安全)
不处理小数部分和负号
不判断Result容量是否充足,请保证Result容量充足
*/
void _Add_Int(BFigure &Result, const BFigure &OperandA, const BFigure &OperandB, int carry)
{
	int Amax_p, Bmax_p, R_p;			//分别是OperandA和OperandB的最大下标及结果的浮标

	Amax_p = *OperandA.ChunkInt_C - 1;
	Bmax_p = *OperandB.ChunkInt_C - 1;

	for (R_p = 0; R_p <= Amax_p&&R_p <= Bmax_p; R_p++) {
		Result.ChunkInt[R_p] = OperandA.ChunkInt[R_p] + OperandB.ChunkInt[R_p] + carry;
		if (Result.ChunkInt[R_p] > 9999) {
			carry = Result.ChunkInt[R_p] / 10000;
			Result.ChunkInt[R_p] %= 10000;
		}
		else carry = 0;
	}
	while (R_p <= Amax_p) {
		Result.ChunkInt[R_p] = OperandA.ChunkInt[R_p] + carry;
		if (Result.ChunkInt[R_p] > 9999) {
			carry = Result.ChunkInt[R_p] / 10000;
			Result.ChunkInt[R_p] %= 10000;
		}
		else carry = 0;
		R_p++;
	}
	while (R_p <= Bmax_p)
	{
		Result.ChunkInt[R_p] = OperandB.ChunkInt[R_p] + carry;
		if (Result.ChunkInt[R_p] > 9999)
		{
			carry = Result.ChunkInt[R_p] / 10000;
			Result.ChunkInt[R_p] %= 10000;
		}
		else carry = 0;
		R_p++;
	}
	if (carry&&R_p < *Result.ChunkInt_C)
		Result.ChunkInt[R_p++] = carry;
	while (R_p < *Result.ChunkInt_C)
		Result.ChunkInt[R_p++] = 0;
	return;
}

/*
浮点数部分的加法核心(不安全)
返回值为向整数块的进位数
*/
int _Add_Float(BFigure &Result, const BFigure &OperandA, const BFigure &OperandB)
{
	int Amax_p, Bmax_p, R_p;

	Amax_p = *OperandA.ChunkFloat_C - 1;
	Bmax_p = *OperandB.ChunkFloat_C - 1;
	R_p = *Result.ChunkFloat_C - 1;

	Amax_p = Amax_p >= 0 && *OperandA.flag ? Amax_p : -1;//如果是整数,则将该值设置为-1;
	Bmax_p = Bmax_p >= 0 && *OperandB.flag ? Bmax_p : -1;//如果是整数,则将该值设置为-1;

	if (Amax_p == -1 && Bmax_p == -1) return 0;
	for (int a = R_p; a > Amax_p&&a > Bmax_p; a--)		//刷新Result最后的0
		Result.ChunkFloat[a] = 0;
	if (Amax_p > Bmax_p) {					//找出没有数可以相加的情况
		while (Amax_p > Bmax_p&&Bmax_p >= -1) {
			Result.ChunkFloat[Amax_p] = OperandA.ChunkFloat[Amax_p];
			Amax_p--;
		}
	}
	else {
		while (Amax_p < Bmax_p&&Amax_p >= -1) {
			Result.ChunkFloat[Bmax_p] = OperandB.ChunkFloat[Bmax_p];
			Bmax_p--;
		}
	}
	int carry = 0;			//进位初始化

							//运行到这里,已经可以确保Amax_p==Bmax_p了
	while (Amax_p >= 0)
	{
		Result.ChunkFloat[Amax_p] = OperandA.ChunkFloat[Amax_p] + OperandB.ChunkFloat[Amax_p] + carry;
		if (Result.ChunkFloat[Amax_p] > 99999999)
		{
			carry = Result.ChunkFloat[Amax_p] / 100000000;
			Result.ChunkFloat[Amax_p] %= 100000000;
		}
		Amax_p--;
	}
	return carry;
}

/*
将字符串转换为Figure对象
约定:如果传入的字符串含有多个负号,则只读取最后一个负号后面的内容
只处理整数部分
*/
void BFigure::_StrToFig_Int(std::string String, int start_index, int end_index)
{
	char temp[5];			//临时缓冲区,用于保存从String读取的数字
	temp[4] = 0;			//将字符串的结束符写入
	int temp_p = 3;			//用于指示临时缓冲区的当前写入下标
	int ChunkInt_p = 0;		//用于记录当前写入ChunkInt的位置

	const char *Str = String.c_str();

	if (end_index - start_index + 1 > (*ChunkInt_C) * 4) {
		if (ChunkInt) delete[] ChunkInt;
		int CIC = (end_index - start_index + 1) / 4 + 1;
		if (CIC > 0) {
			ChunkInt = new int[CIC];
			*ChunkInt_C = CIC;
		}
		else {
			ChunkInt = new int[1]();
			*ChunkInt_C = 1;
			return;
		}
	}

	for (int a = end_index; a >= start_index; a--) {
		if (temp_p < 0) {
			//取得四个数之后,将数字进行存储
			ChunkInt[ChunkInt_p++] = atoi(temp);
			temp_p = 3;		//重置下标位置
		}
		if (Str[a] >= '0'&&Str[a] <= '9') {
			temp[temp_p--] = Str[a];
		}
		else if (Str[a] == '-') {
			*this->Minus = 1;				//设置该数字为负数
			break;
		}
	}
	if (temp_p != 3)
	{
		//发现有数据,进行保存
		while (temp_p != -1)				//刷新字符串
			temp[temp_p--] = ' ';
		ChunkInt[ChunkInt_p] = atoi(temp);
		ChunkInt_p++;
	}
	//*ChunkInt_C = ChunkInt_p;				//记录Chunk的大小
	return;
}

/*
将字符串转换为Figure对象
只处理小数部分
*/
void BFigure::_StrToFig_Float(std::string String, int start_index, int end_index)
{
	const char * Str = String.c_str();
	char temp[9];					//临时缓冲区,用于保存从String读取的数字
	int temp_p = 0;					//记录待写入临时缓冲区的位置
	temp[8] = 0;					//写入字符串结束符
	int ChunkFloat_p = 0;			//记录ChunkFloat的写入位置

	if (end_index - start_index > (*ChunkFloat_C) * 8) {
		if (ChunkFloat) delete[] ChunkFloat;
		int CFC = (end_index - start_index + 1) / 8 + 1;
		if (CFC) {
			ChunkFloat = new int[CFC];
			*ChunkFloat_C = CFC;
		}
		else
		{
			ChunkFloat = new int[1]();
			*ChunkFloat_C = 1;
			return;
		}


	}

	for (int a = start_index; a <= end_index; a++) {
		//取得8位数字后,对数字字符串进行处理,使之转为数字
		if (temp_p > 7) {
			ChunkFloat[ChunkFloat_p++] = atoi(temp);	//将缓冲区中的字符串转为数字进行保存
			temp_p = 0;					//重置缓冲区写入位置的下标
		}
		if (Str[a] >= '0'&&Str[a] <= '9') {
			temp[temp_p++] = Str[a];	//发现数字,进行保存
		}
	}
	if (temp_p != 0) {
		//还有已读取的数据没有处理,进行处理
		while (temp_p <= 7)				//刷新字符串
			temp[temp_p++] = '0';
		ChunkFloat[ChunkFloat_p] = atoi(temp);
		ChunkFloat_p++;
	}
	*ChunkFloat_C = ChunkFloat_p;
	return;
}

/*
临时函数的存储区
*/
void BFigure::Display()
{
	int a;
	if (*ChunkInt_C > 0)
	{
		for (a = *ChunkInt_C - 1; a > 0; a--) {
			std::cout.fill('0');             //设置填充方式,不足位补0
			std::cout.width(4);              //设置宽度为2，只对下条输出有用 
			std::cout << ChunkInt[a] << " ";
		}
		if (*flag == 0) {
			std::cout.fill('0');             //设置填充方式,不足位补0
			std::cout.width(4);              //设置宽度为2，只对下条输出有用 
			std::cout << ChunkInt[a] << std::endl;
		}
		else
		{
			std::cout.fill('0');             //设置填充方式,不足位补0
			std::cout.width(4);              //设置宽度为2，只对下条输出有用
			std::cout << ChunkInt[a];
		}

	}
	else std::cout << "0 ";



	if (*flag > 0)
	{
		std::cout << ".";
		for (a = 0; a < *ChunkFloat_C - 1; a++)
		{
			std::cout.fill('0');             //设置填充方式,不足位补0
			std::cout.width(8);              //设置宽度为2，只对下条输出有用 
			std::cout << ChunkFloat[a] << " ";
		}
		std::cout.fill('0');             //设置填充方式,不足位补0
		std::cout.width(8);              //设置宽度为2，只对下条输出有用 
		std::cout << ChunkFloat[a] << std::endl;
	}
	return;
}

/*
当对BFigure对象进行=操作的时候,进行复制操作
在声明BFigure时的赋值无效
*/
BFigure & BFigure::operator=(const BFigure & Source)
{
	BFigureCopyer(*this, Source);
	return *this;
}

/*
直接用一个字符串给BFigure对象赋值
本函数一直将字符串当做是浮点数进行处理
*/
BFigure & BFigure::operator=(std::string NumString)
{
	int Stringlen = NumString.length();
	int P_p = _FindDecimalPoint(NumString.c_str(), Stringlen);		//获取小数点的位置
	if (P_p == -1) P_p = Stringlen, *this->flag = 0;

	if (P_p > *ChunkInt_C * 4) {
		if (ChunkInt) delete[] ChunkInt;
		ChunkInt = NULL;
		*ChunkInt_C = 0;
	}
	_StrToFig_Int(NumString, 0, P_p - 1);
	if (Stringlen - P_p > *ChunkFloat_C * 8) {
		if (ChunkFloat) delete[] ChunkFloat;
		ChunkFloat = NULL;
		*ChunkFloat_C = 0;
	}
	_StrToFig_Float(NumString, P_p + 1, Stringlen - 1);
	return *this;
}

/*
重载加号运算符,使之支持BFigure+Bfigure
*/
BFigure BFigure::operator+(const BFigure & rhs)
{
	int l_len = _GetRealLength_Int(*this);
	int r_len = _GetRealLength_Int(rhs);
	int max_len_i = l_len > r_len ? l_len + 1 : r_len + 1;

	l_len = _GetRealLength_Float(*this);
	r_len = _GetRealLength_Float(rhs);
	int max_len_f = l_len > r_len ? l_len : r_len;	//浮点位不会增加

	//初始化一个可以装得下结果的BFigure对象
	BFigure Result((max_len_i / 4) + 1, (max_len_f / 8) + 1);

	if (*this->flag || *rhs.flag)
		Add_Float(Result, *this, rhs);
	else
		Add_Int(Result, *this, rhs);
	return Result;
}

BFigure BFigure::operator-(const BFigure & rhs)
{
	return BFigure();
}

/*
BFigure对象复制函数
将Source复制到destin,复制后destin完全一样
CompleteCopy为1时,将保留destin原来的内存分配(除非内存不够复制)
CompleteCopy为0时,将无论destin是否足够存放数据,都进行分配内存
*/
void BFigureCopyer(BFigure &destin, const BFigure &Source, int CompleteCopy)
{
	if (CompleteCopy == 0 || (CompleteCopy&&_GetRealLength_Int(Source) > *destin.ChunkInt_C * 4)) {
		if (destin.ChunkInt)delete[] destin.ChunkInt;//删除原来分配的内存,重新进行分配
		destin.ChunkInt = new int[*Source.ChunkInt_C];
		*destin.ChunkInt_C = *Source.ChunkInt_C;
	}

	int temp = *destin.ChunkInt_C > *Source.ChunkInt_C ? *Source.ChunkInt_C : *destin.ChunkInt_C;
	for (int a = 0; a < temp; a++) {
		destin.ChunkInt[a] = Source.ChunkInt[a];
	}
	*destin.flag = *Source.flag;
	*destin.Minus = *Source.Minus;
	*destin.pFigureCount = 1;			//初始化为引用计数为1

	if (*Source.flag) {
		//浮点数部分,特殊处理
		if (CompleteCopy == 0 || (CompleteCopy && (_GetRealLength_Float(Source) > *destin.ChunkFloat_C * 8))) {
			if (destin.ChunkFloat)delete[] destin.ChunkFloat;//删除原来分配的内存,重新进行分配
			destin.ChunkFloat = new int[*Source.ChunkFloat_C];
			*destin.ChunkFloat_C = *Source.ChunkFloat_C;
		}
		temp = *destin.ChunkFloat_C > *Source.ChunkFloat_C ? *Source.ChunkFloat_C : *destin.ChunkFloat_C;
		for (int a = 0; a < temp; a++) {
			destin.ChunkFloat[a] = Source.ChunkFloat[a];
		}
	}
}

//************************内部函数定义区********************************

/*
取得Figure中的整数部分的有效位数
返回值是以位计算的,不是块
*/
static int _GetRealLength_Int(const BFigure &Figure)
{
	bool flg = 1;
	int Count = 0;
	int a = *Figure.ChunkInt_C - 1;

	while (!Figure.ChunkInt[a] && a > 0)		//取得最高位的下标
		a--;

	int ChunkInt_temp = Figure.ChunkInt[a];
	if (ChunkInt_temp > 999)
		Count += 4;
	else if (ChunkInt_temp > 99)
		Count += 3;
	else if (ChunkInt_temp > 9)
		Count += 2;
	else Count += 1;
	a--;

	Count += a * 4 + 4;
	return Count;
}

/*
取得Figure中的浮点部分的有效位数
返回值是以位计算的, 不是块
*/
static int _GetRealLength_Float(const BFigure & Figure)
{
	int A_p = *Figure.ChunkFloat_C - 1;
	if (!*Figure.ChunkFloat_C) return 0;
	while (Figure.ChunkFloat[A_p] == 0)		//找到不为0的块的下标
		A_p--;
	int Temp = Figure.ChunkFloat[A_p--];	//取得最高位的一个有效块
	bool flg = 1;
	int Count = 0;

	for (int a = 0; a < 8; a++) {
		if (flg) {
			if (Temp % 10 == 0)
				Temp /= 10;
			else {
				flg = 0;
				Count += 8 - a;
				break;
			}
		}
	}
	Count += A_p * 8 + 8;
	return Count;
}

/*
约定:找最后一个".",不是最后一个,没找到返回-1
*/
static int _FindDecimalPoint(const char *NumString, int length)
{
	int a;
	for (a = length - 1; a >= 0; a--) {
		if (NumString[a] == '.') {
			return a;
		}
	}
	return a;
}

/*
将整数块进行输出,
传入的buffer应大小大于5

*/
static void _ChunktoStr_Int(int ChunkNum, char *buffer)
{
	if (ChunkNum > 999) {
		_ItoA(ChunkNum, buffer, 5);
	}
	else if (ChunkNum > 99) {
		buffer[0] = '0';
		_ItoA(ChunkNum, buffer + 1, 4);
	}
	else if (ChunkNum > 9) {
		buffer[0] = '0';
		buffer[1] = '0';
		_ItoA(ChunkNum, buffer + 2, 3);
	}
	else if (ChunkNum >= 0) {
		buffer[0] = '0';
		buffer[1] = '0';
		buffer[2] = '0';
		_ItoA(ChunkNum, buffer + 3, 2);
	}
	return;
}

/*
将浮点块转化为字符串
请务必传入一个9位数的
*/
static void _ChunktoStr_Float(int ChunkNum, char *buffer)
{
	int len = 7;
	int a = _ItoA(ChunkNum, buffer, 9) - 1;
	while (a >= 0)
		buffer[a--] = '0';
	while (buffer[len] == '0'&&len > 0)
		len--;
	buffer[len + 1] = 0;
	return;
}

/*
将数字转化为字符串
返回值是一个char*的偏移
请使用buffer+_ItoA()的形式来调用此函数
*/
static int _ItoA(int Num, char *buffer, int size)
{
	int a = size - 2;
	buffer[size - 1] = 0;
	buffer[a--] = Num % 10 + '0';
	Num /= 10;
	while (Num)
	{
		buffer[a--] = Num % 10 + '0';
		Num /= 10;
	}
	return a + 1;
}

/*
字符串数字比较函数
只允许纯数字,否则会出错
返回  1 表示 StringA和StringB相同位数,但StringA > StringB
返回 -1 表示 StringA和StringB相同位数,但StringA < StringB
返回 100表示 StringA>>StringB
返回-100表示 StringA<<StringB
返回  0 表示 StringA==StringB
*/
static int _CompareNumString(const char *StringA, const char *StringB)
{
	int Alen = strlen(StringA);
	int Blen = strlen(StringB);

	if (Alen == Blen)
	{
		//位数相同
		int a = 0;
		while (StringA[a] == StringB[a] && a < Alen)		//执行到不相等,然后进行判断 
			a++;
		if (StringA[a] > StringB[a])
			return 1;
		else if (StringA[a] < StringB[a])
			return -1;
		else return 0;
	}
	else if (Alen > Blen)
		return 100;
	else
		return -100;
}

int CompareBFigure(const BFigure &OperandA, const BFigure &OperandB)
{
	int ReturnPlus;
	if (*OperandA.Minus&&*OperandB.Minus)
		ReturnPlus = -1;
	else if (*OperandA.Minus&&*OperandB.Minus == 0)
		return -1;
	else if (*OperandA.Minus == 0 && *OperandB.Minus)
		return 1;
	else ReturnPlus = 1;

	return _CompareBFigure(ReturnPlus, OperandA, OperandB);
}

/*
比较两个BFigure的大小
OperandA>OperandB
*/
int _CompareBFigure(int ReturnPlus,const BFigure &OperandA, const BFigure &OperandB)
{
	int A_p = *OperandA.ChunkInt_C - 1;
	while (!OperandA.ChunkInt[A_p] && A_p > 0)		//取得A最高位的下标
		A_p--;

	int B_p = *OperandB.ChunkInt_C - 1;
	while (!OperandB.ChunkInt[B_p] && B_p > 0)		//取得B最高位的下标
		B_p--;

	if (A_p > B_p)
		return ReturnPlus;		//A远大于B
	else if (A_p < B_p)
		return ReturnPlus*-1;	//B远大于A
	else {
		//A和B具有相同的有效块数目,需要进一步比较
		//A_p和B_p相等之后,只区A_p来计数
		while (A_p >= 0 && OperandA.ChunkInt[A_p] == OperandB.ChunkInt[A_p])
			A_p--;
		if (OperandA.ChunkInt[A_p] > OperandB.ChunkInt[A_p])
			return ReturnPlus;					//A>B
		else if (OperandA.ChunkInt[A_p] < OperandB.ChunkInt[A_p])
			return ReturnPlus*-1;		//A<B
		else {
			//整数部分完全相等,继续比较浮点数部分
			A_p = 0;		//将计数变量复位,开始比较小数位
			if (*OperandA.flag&&*OperandB.flag&&*OperandA.ChunkFloat_C&&*OperandB.ChunkFloat_C) {
				//两个数都是浮点数
				B_p = *OperandA.ChunkFloat_C < *OperandB.ChunkFloat_C ? *OperandA.ChunkFloat_C : *OperandB.ChunkFloat_C;	//临时借用来存储最小Chunk数
				//B_p = 1;
				while (A_p < B_p - 1 && OperandA.ChunkFloat[A_p] == OperandB.ChunkFloat[A_p])
					A_p++;
				if (OperandA.ChunkFloat[A_p] > OperandB.ChunkFloat[A_p])
					return ReturnPlus;
				else if (OperandA.ChunkFloat[A_p] < OperandB.ChunkFloat[A_p])
					return ReturnPlus*-1;
				else {
					if (B_p == A_p) {
						if (A_p < *OperandA.ChunkFloat_C)
							while (A_p) {
								if (OperandA.ChunkFloat[A_p]) return ReturnPlus;
								A_p++;
							}
						else if (A_p < *OperandB.ChunkFloat_C)
							while (A_p) {
								if (OperandB.ChunkFloat[A_p])return ReturnPlus*-1;
								A_p++;
							}
						else return 0;
					}
				}
			}
			else if (*OperandA.flag && !*OperandB.flag&&*OperandA.ChunkFloat_C) {
				if (OperandA.ChunkFloat[A_p]) return ReturnPlus;
			}
			else if (!*OperandA.flag && *OperandB.flag&&*OperandB.ChunkFloat_C)
				if (OperandB.ChunkFloat[A_p]) return ReturnPlus*-1;
		}
	}
	return 0;
}
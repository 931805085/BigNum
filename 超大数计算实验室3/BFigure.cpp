#include "BFigure.h"

/*
��Դ�ļ�˽�еķ���
*/
static int _FindDecimalPoint(const char *NumString, int length);

static int _ItoA(int Num, char *buffer, int size);
static void _ChunktoStr_Float(int ChunkNum, char *buffer);
static void _ChunktoStr_Int(int ChunkNum, char *buffer);

/*
Լ��:
�������е�ά����ά�ֱ�洢��λ����λ
������е�ά����ά�ֱ�洢�ͷ�λ���߷�λ
*/


/*
Ĭ�ϵĹ��캯��
���û��ָ������,��ֱ��ʹ��ͷ�ļ���ָ����Ĭ�����ݽ��г�ʼ��
�ɴ洢DEFAULT_MALLOC_INT*4λ��������
�ɴ洢EFAULT_MALLOC_FLOAT*8λ��������
*/
BFigure::BFigure()
{
	flag = new FLAG(1);								//Ĭ��Ϊ����
	ChunkInt = new int[DEFAULT_MALLOC_INT]();		//��ʼ��������
	ChunkFloat = new int[EFAULT_MALLOC_FLOAT]();	//��ʼ�������
	ChunkInt_C = new int(DEFAULT_MALLOC_INT);
	ChunkFloat_C = new int(EFAULT_MALLOC_FLOAT);
	pFigureCount = new int(1);						//��ʼ�����ü���
	Minus = new FLAG(0);
}

/*
���������һ��int,���ô����ݳ�ʼ��int����
������캯��ֻ�ܽ������ʼ��Ϊһ������
�ɴ洢ChunkIntCount*4λ��������
*/
BFigure::BFigure(int ChunkIntCount)
{
	flag = new FLAG(0);							//����Ϊ����
	ChunkInt = new int[ChunkIntCount]();		//��ʼ��������
	ChunkInt_C = new int(ChunkIntCount);
	ChunkFloat_C = new int(0);
	pFigureCount = new int(1);					//��ʼ�����ü���
	Minus = new FLAG(0);
}

/*
����������Գ�ʼ��һ����ֵΪ0�Ķ���
�ɴ洢ChunkIntCount*4λ��������
�ɴ洢ChunkFloatCount*8λ��������
*/
BFigure::BFigure(int ChunkIntCount, int ChunkFloatCount)
{
	flag = new FLAG(1);								//����Ϊ������
	ChunkInt = new int[ChunkIntCount]();			//��ʼ��������
	ChunkFloat = new int[ChunkFloatCount]();		//��ʼ�������
	ChunkInt_C = new int(ChunkIntCount);
	ChunkFloat_C = new int(ChunkFloatCount);
	pFigureCount = new int(1);						//��ʼ�����ü���				
	Minus = new FLAG(0);
}

/*
�������������һ���ַ���
��flagΪ0ʱ,ֻ�ὫNumString��Ϊһ������,����С����
��flagΪ1ʱ,������ȷ��ȡС������
*/
BFigure::BFigure(std::string NumString, int flag = 1)
{
	int P_p;						//С�����λ��
	int Stringlen = (int)NumString.length();
	Minus = new FLAG(0);			//����������Ϊ����

									//std::cout << "����" << std::endl;

	this->flag = new FLAG(flag);								//Ĭ��Ϊ������
	if (flag == 0) {
		//������ַ����е�����Ϊ����
		ChunkInt_C = new int(0);
		ChunkFloat_C = new int(0);				//��Ȼ����Ҫ�õ�,��������Ҫ��ʼ��
		_StrToFig_Int(NumString, 0, Stringlen - 1);
	}
	else if (flag == 1) {
		//������ַ����е������Ǹ�����
		P_p = _FindDecimalPoint(NumString.c_str(), Stringlen);		//��ȡС�����λ��
		if (P_p == -1) P_p = Stringlen;
		ChunkInt_C = new int(0);
		_StrToFig_Int(NumString, 0, P_p - 1);
		ChunkFloat_C = new int(0);
		_StrToFig_Float(NumString, P_p + 1, Stringlen - 1);
	}
	pFigureCount = new int(1);
}

/*
���ƹ��캯��
*/
BFigure::BFigure(const BFigure & Source)
{
	//std::cout << "���ƹ���" << std::endl;
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
BFigure��������
*/
BFigure::~BFigure(void)
{
	*pFigureCount -= 1;
	//std::cout << "����" << std::endl;
	if (!*pFigureCount) {
		//std::cout << "���� ɾ��" << std::endl;
		//�����ü���Ϊ0ʱ,delete���ж����ָ��ָ����ڴ�
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
���ַ���ת��ΪFigure����
Լ��:���������ַ������ж������,��ֻ��ȡ���һ�����ź��������
*/
void BFigure::StrToFig_Int(std::string String)
{
	*flag = 0;
	_StrToFig_Int(String, 0, (int)(String.length() - 1));
}


/*
���ַ���ת��ΪFigure����
Լ��:���������ַ������ж������,��ֻ��ȡ���һ�����ź��������
Լ��:���������ַ������ж��С����,ֻȡ���һ��С����,����С�������
*/
void BFigure::StrToFig_Float(std::string String)
{
	int P_p = _FindDecimalPoint(String.c_str(), (int)String.length());		//��ȡС�����λ��
	if (P_p == -1) P_p = (int)String.length();
	*flag = 1;

	_StrToFig_Int(String, 0, P_p - 1);
	_StrToFig_Float(String, P_p + 1, (int)String.length() - 1);
}

/*
�������е�����תΪ�ַ�������
*/
std::string BFigure::toString()
{
	char ChunkTemp[9];
	char *returnVal;
	if (*flag)
		returnVal = new char[_GetRealLength_Int(*this) + _GetRealLength_Float(*this) +3 + *this->Minus]();
	else
		returnVal = new char[_GetRealLength_Int(*this) + 1 + *this->Minus]();

	returnVal[0] = 0;
	if (*ChunkInt_C&&*ChunkInt) {		//��ʾ��������
		for (int a = *ChunkInt_C - 1; a >= 0; a--) {
			_ChunktoStr_Int(ChunkInt[a], ChunkTemp);
			strcat(returnVal, ChunkTemp);
		}
	}

	if (*flag&&*ChunkFloat_C) {			//��һ��С��,��ʾС������
		strcat(returnVal, ".");
		for (int a = 0; a < *ChunkFloat_C; a++) {
			_ChunktoStr_Float(ChunkFloat[a], ChunkTemp);
			strcat(returnVal, ChunkTemp);
		}
	}
	

	std::string re = std::string(returnVal);
	delete returnVal;

	return re;
}

/*
���������ļӷ�
ֻ�ܼ�������,������븡�����������С����������
��������Ҫ���ڵ���ʹ���������
*/
void Add_Int(BFigure & Result, const BFigure & OperandA, const BFigure & OperandB)
{
	int MinusA = *OperandA.Minus;
	int MinusB = *OperandB.Minus;

	if (MinusA&&MinusB)
	{
		//��������
		_Add_Int(Result, OperandA, OperandB, 0);
		*Result.Minus = 1;
	}
	else if ((!MinusA) && MinusB) {
		//A��B��
	}
	else if (MinusA && (!MinusB)) {
		//A��B��
	}
	else {
		//��������
		_Add_Int(Result, OperandA, OperandB, 0);
		*Result.Minus = 0;
	}
	*Result.flag = 0;
}

/*
���⸡�����ļӷ�
��������Ҫ���ڵ���ʹ���������
*/
void Add_Float(BFigure & Result, const BFigure & OperandA, const BFigure & OperandB)
{
	int MinusA = *OperandA.Minus;
	int MinusB = *OperandB.Minus;

	if (MinusA&&MinusB)
	{
		//��������
		_Add_Int(Result, OperandA, OperandB, _Add_Float(Result, OperandA, OperandB));
		*Result.Minus = 1;
	}
	else if ((!MinusA) && MinusB) {
		//A��B��
	}
	else if (MinusA && (!MinusB)) {
		//A��B��
	}
	else {
		//��������
		_Add_Int(Result, OperandA, OperandB, _Add_Float(Result, OperandA, OperandB));
		*Result.Minus = 0;
	}
	*Result.flag = 1;
}

/*
�������ֵļӷ�����(����ȫ)
������С�����ֺ͸���
���ж�Result�����Ƿ����,�뱣֤Result��������
*/
void _Add_Int(BFigure &Result, const BFigure &OperandA, const BFigure &OperandB, int carry)
{
	int Amax_p, Bmax_p, R_p;			//�ֱ���OperandA��OperandB������±꼰����ĸ���

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
	if (carry)
		Result.ChunkInt[R_p++] = carry;
	while (R_p < *Result.ChunkInt_C)
		Result.ChunkInt[R_p++] = 0;
	return;
}

/*
���������ֵļӷ�����(����ȫ)
����ֵΪ��������Ľ�λ��
*/
int _Add_Float(BFigure &Result, const BFigure &OperandA, const BFigure &OperandB)
{
	int Amax_p, Bmax_p, R_p;

	Amax_p = *OperandA.ChunkFloat_C - 1;
	Bmax_p = *OperandB.ChunkFloat_C - 1;
	R_p = *Result.ChunkFloat_C - 1;

	Amax_p = Amax_p >= 0 && *OperandA.flag ? Amax_p : -1;//���������,�򽫸�ֵ����Ϊ-1;
	Bmax_p = Bmax_p >= 0 && *OperandB.flag ? Bmax_p : -1;//���������,�򽫸�ֵ����Ϊ-1;

	if (Amax_p == -1 && Bmax_p == -1) return 0;
	for (int a = R_p; a > Amax_p&&a > Bmax_p; a--)		//ˢ��Result����0
		Result.ChunkFloat[a] = 0;
	if (Amax_p > Bmax_p) {					//�ҳ�û����������ӵ����
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
	int carry = 0;			//��λ��ʼ��

							//���е�����,�Ѿ�����ȷ��Amax_p==Bmax_p��
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
���ַ���ת��ΪFigure����
Լ��:���������ַ������ж������,��ֻ��ȡ���һ�����ź��������
ֻ������������
*/
void BFigure::_StrToFig_Int(std::string String, int start_index, int end_index)
{
	char temp[5];			//��ʱ������,���ڱ����String��ȡ������
	temp[4] = 0;			//���ַ����Ľ�����д��
	int temp_p = 3;			//����ָʾ��ʱ�������ĵ�ǰд���±�
	int ChunkInt_p = 0;		//���ڼ�¼��ǰд��ChunkInt��λ��

	const char *Str = String.c_str();

	if (end_index - start_index + 1 > (*ChunkInt_C) * 4) {
		if (ChunkInt) delete[] ChunkInt;
		int CIC = (end_index - start_index + 1) / 4 + 1;
		if (CIC)
			ChunkInt = new int[CIC];
		else
			ChunkInt = NULL;
		*ChunkInt_C = CIC;
	}

	for (int a = end_index; a >= start_index; a--) {
		if (temp_p < 0) {
			//ȡ���ĸ���֮��,�����ֽ��д洢
			ChunkInt[ChunkInt_p++] = atoi(temp);
			temp_p = 3;		//�����±�λ��
		}
		if (Str[a] >= '0'&&Str[a] <= '9') {
			temp[temp_p--] = Str[a];
		}
		else if (Str[a] == '-') {
			*this->Minus = 1;				//���ø�����Ϊ����
			break;
		}
	}
	if (temp_p != 3)
	{
		//����������,���б���
		while (temp_p != -1)				//ˢ���ַ���
			temp[temp_p--] = ' ';
		ChunkInt[ChunkInt_p++] = atoi(temp);
	}
	*ChunkInt_C = ChunkInt_p;				//��¼Chunk�Ĵ�С
	return;
}

/*
���ַ���ת��ΪFigure����
ֻ����С������
*/
void BFigure::_StrToFig_Float(std::string String, int start_index, int end_index)
{
	const char * Str = String.c_str();
	char temp[9];					//��ʱ������,���ڱ����String��ȡ������
	int temp_p = 0;					//��¼��д����ʱ��������λ��
	temp[8] = 0;					//д���ַ���������
	int ChunkFloat_p = 0;			//��¼ChunkFloat��д��λ��

	if (end_index - start_index > (*ChunkFloat_C) * 8) {
		if (ChunkFloat) delete[] ChunkFloat;
		int CFC = (end_index - start_index + 1) / 8 + 1;
		if (CFC)
			ChunkFloat = new int[CFC];
		else
			ChunkFloat = NULL;
		*ChunkFloat_C = CFC;
	}

	for (int a = start_index; a <= end_index; a++) {
		//ȡ��8λ���ֺ�,�������ַ������д���,ʹ֮תΪ����
		if (temp_p > 7) {
			ChunkFloat[ChunkFloat_p++] = atoi(temp);	//���������е��ַ���תΪ���ֽ��б���
			temp_p = 0;					//���û�����д��λ�õ��±�
		}
		if (Str[a] >= '0'&&Str[a] <= '9') {
			temp[temp_p++] = Str[a];	//��������,���б���
		}
	}
	if (temp_p != 0) {
		//�����Ѷ�ȡ������û�д���,���д���
		while (temp_p <= 7)				//ˢ���ַ���
			temp[temp_p++] = '0';
		ChunkFloat[ChunkFloat_p++] = atoi(temp);
	}
	*ChunkFloat_C = ChunkFloat_p;
	return;
}

/*
��ʱ�����Ĵ洢��
*/
void BFigure::Display()
{
	int a;
	if (*ChunkInt_C > 0)
	{
		for (a = *ChunkInt_C - 1; a > 0; a--) {
			std::cout.fill('0');             //������䷽ʽ,����λ��0
			std::cout.width(4);              //���ÿ��Ϊ2��ֻ������������� 
			std::cout << ChunkInt[a] << " ";
		}
		if (*flag == 0) {
			std::cout.fill('0');             //������䷽ʽ,����λ��0
			std::cout.width(4);              //���ÿ��Ϊ2��ֻ������������� 
			std::cout << ChunkInt[a] << std::endl;
		}
		else
		{
			std::cout.fill('0');             //������䷽ʽ,����λ��0
			std::cout.width(4);              //���ÿ��Ϊ2��ֻ�������������
			std::cout << ChunkInt[a];
		}

	}
	else std::cout << "0 ";



	if (*flag > 0)
	{
		std::cout << ".";
		for (a = 0; a < *ChunkFloat_C - 1; a++)
		{
			std::cout.fill('0');             //������䷽ʽ,����λ��0
			std::cout.width(8);              //���ÿ��Ϊ2��ֻ������������� 
			std::cout << ChunkFloat[a] << " ";
		}
		std::cout.fill('0');             //������䷽ʽ,����λ��0
		std::cout.width(8);              //���ÿ��Ϊ2��ֻ������������� 
		std::cout << ChunkFloat[a] << std::endl;
	}
	return;
}

/*
����BFigure�������=������ʱ��,���и��Ʋ���
������BFigureʱ�ĸ�ֵ��Ч
*/
BFigure & BFigure::operator=(const BFigure & Source)
{
	BFigureCopyer(*this, Source);
	return *this;
}

/*
ֱ����һ���ַ�����BFigure����ֵ
������һֱ���ַ��������Ǹ��������д���
*/
BFigure & BFigure::operator=(std::string NumString)
{
	int Stringlen = NumString.length();
	int P_p = _FindDecimalPoint(NumString.c_str(), Stringlen);		//��ȡС�����λ��
	if (P_p == -1) P_p = Stringlen;

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
���ؼӺ������,ʹ֧֮��BFigure+Bfigure
*/
BFigure BFigure::operator+(const BFigure & rhs)
{
	int l_len = _GetRealLength_Int(*this);
	int r_len = _GetRealLength_Int(rhs);
	int max_len_i = l_len > r_len ? l_len + 1 : r_len + 1;

	l_len = _GetRealLength_Float(*this);
	r_len = _GetRealLength_Float(rhs);
	int max_len_f = l_len > r_len ? l_len : r_len;	//����λ��������

	//��ʼ��һ������װ���½����BFigure����
	BFigure Result((max_len_i / 4), (max_len_f / 8) + 1);

	if (*this->flag || *rhs.flag)
		Add_Float(Result, *this, rhs);
	else
		Add_Int(Result, *this, rhs);
	return Result;
}

/*
BFigure�����ƺ���
��Source���Ƶ�destin,���ƺ�destin��ȫһ��
CompleteCopyΪ1ʱ,������destinԭ�����ڴ����(�����ڴ治������)
CompleteCopyΪ0ʱ,������destin�Ƿ��㹻�������,�����з����ڴ�
*/
void BFigureCopyer(BFigure &destin, const BFigure &Source, int CompleteCopy)
{
	if (CompleteCopy == 0 || (CompleteCopy&&_GetRealLength_Int(Source) > *destin.ChunkInt_C * 4)) {
		if (destin.ChunkInt)delete[] destin.ChunkInt;//ɾ��ԭ��������ڴ�,���½��з���
		destin.ChunkInt = new int[*Source.ChunkInt_C];
		*destin.ChunkInt_C = *Source.ChunkInt_C;
	}

	int temp = *destin.ChunkInt_C > *Source.ChunkInt_C ? *Source.ChunkInt_C : *destin.ChunkInt_C;
	for (int a = 0; a < temp; a++) {
		destin.ChunkInt[a] = Source.ChunkInt[a];
	}
	*destin.flag = *Source.flag;
	*destin.Minus = *Source.Minus;
	*destin.pFigureCount = 1;			//��ʼ��Ϊ���ü���Ϊ1

	if (*Source.flag) {
		//����������,���⴦��
		if (CompleteCopy == 0 || (CompleteCopy&&_GetRealLength_Float(Source) > *destin.ChunkFloat_C * 8)) {
			if (destin.ChunkFloat)delete[] destin.ChunkFloat;//ɾ��ԭ��������ڴ�,���½��з���
			destin.ChunkFloat = new int[*Source.ChunkFloat_C];
			*destin.ChunkFloat_C = *Source.ChunkFloat_C;
		}
		temp = *destin.ChunkFloat_C > *Source.ChunkFloat_C ? *Source.ChunkFloat_C : *destin.ChunkFloat_C;
		for (int a = 0; a < temp; a++) {
			destin.ChunkFloat[a] = Source.ChunkFloat[a];
		}
	}

}

/*
ȡ��Figure�е��������ֵ���Чλ��
����ֵ����λ�����,���ǿ�
*/
int _GetRealLength_Int(const BFigure &Figure)
{
	bool flg = 1;
	int Count = 0;
	int a = *Figure.ChunkInt_C - 1;

	while (!Figure.ChunkInt[a] && a > 0)		//ȡ�����λ���±�
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
ȡ��Figure�еĸ��㲿�ֵ���Чλ��
����ֵ����λ�����, ���ǿ�
*/
int _GetRealLength_Float(const BFigure & Figure)
{
	int A_p = *Figure.ChunkFloat_C - 1;
	if (!*Figure.ChunkFloat_C) return 0;
	while (Figure.ChunkFloat[A_p] == 0)		//�ҵ���Ϊ0�Ŀ���±�
		A_p--;
	int Temp = Figure.ChunkFloat[A_p--];	//ȡ�����λ��һ����Ч��
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
Լ��:�����һ��".",�������һ��,û�ҵ�����-1
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

//************************�ڲ�����������********************************

/*
��������������,
�����bufferӦ��С����5

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
�������ת��Ϊ�ַ���
����ش���һ��9λ����
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
������ת��Ϊ�ַ���
����ֵ��һ��char*��ƫ��
��ʹ��buffer+_ItoA()����ʽ�����ô˺���
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
#pragma once
#include <string>
#include <cstdlib>

/*
��ʱ����
*/
#include <iostream>

#define DEFAULT_MALLOC_INT 20		//1����ʾ4λ,20���ɱ���80λ
#define EFAULT_MALLOC_FLOAT 8		//1����ʾ8λ,8����ʾ64λ

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
	BFigure(std::string NumString, int flag);		//������һ�������ַ�����ʼ��һ����,������Ŀռ�ֻ��string�ĳ���һ����С
	BFigure(const BFigure& Base);					//���ƹ��캯��
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

	//*******************��ʱ����,�깤��ɾ��*********************
	void Display();
	//**********************************************************


	//��������غ���
	BFigure& operator=(const BFigure& rhs);
	BFigure& BFigure::operator=(std::string NumString);	//���ַ���д�뵽BFigure��
	BFigure operator+(const BFigure& rhs);

	/*
	+ - * / % = <<
	++ --
	*/
protected:
	FLAG *flag;				//���������ݵ����� 0��ʾ����   1��ʾ������
	FLAG *Minus;			//���������Ƿ�Ϊ����	0��ʾ����   1��ʾ����
	int *ChunkInt = NULL;	//�������ֵĴ洢��
	int *ChunkFloat = NULL;	//���������ֵĴ洢��
	int *ChunkInt_C;		//Int���д�����ݵ�Ԫ�ظ���
	int *ChunkFloat_C;		//Float���д�����ݵ�Ԫ�ظ���
	int *pFigureCount;		//Figure�����ü���
	void _StrToFig_Float(std::string, int start_index, int end_index);
	void _StrToFig_Int(std::string String, int start_index, int end_index);

	friend void _Add_Int(BFigure &Result, const BFigure &OperandA, const BFigure &OperandB, int carry);
	friend int _Add_Float(BFigure &Result, const BFigure &OperandA, const BFigure &OperandB);
public:	//��ʱ��

	friend int _GetRealLength_Int(const BFigure &Figure);
	friend int _GetRealLength_Float(const BFigure &Figure);
};


/*
TODO:
��ǿ��׳��,��ֹNULLָ��
���ַ��� ��ʼ��ʱ��Ҫ�ȳ�ʼ������			OK

�޸�=����,ʹ֮���ڴ��㹻ʱ,���������·���	OK
����+�������,ʹ֧֮��BFigure+int

*/
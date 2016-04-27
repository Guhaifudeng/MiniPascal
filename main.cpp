#include "StdAfx.h"
//�û��Զ����ļ�
#include "Structure.h"
#include "ErrorProc.h"
#include "CommonLib.h"
#include "State.h"
#include "Lex.h"
#include "Global.h"
#include "SymbolTbl.h"
#include "Optimization.h"
#include "MemoryAlloc.h"
using namespace std;
void Init()
{

	CType::InitTypeSysTbl();

	SymbolTbl.Clear();

	Lex.LexInit();

	Syntax.Init();

	Lex.SetFileName(State.m_szSourceFile);
}

int main(int argc, char** argv)
{
    //���������в���
    if (!State.CommandInit(argc,argv))
    {
        PrintError();
        return 1;
    }

    //ϵͳ��ʼ��
    Init();

    //�ʷ�����
    //extern CLex Lex;
    if (!Lex.GenToken(&TokenList))
    {
    		PrintError();
    		return 1;
    }


	//�﷨���������
	if (!Syntax.SyntaxParse())
	{
	    cout<<"---11"<<endl;
		PrintError();
		return 1;
	}
    //���IR
	SymbolTbl.PrintIR();


	//IR�Ż�
	if (!State.m_bOpti)
	{
		COptimization::OptiPass();
	}
	//����ʱ�̻���
	CMemoryAlloc::MemoryAlloc();
	//Ŀ���������
	target.IRtoASM();

	//���asm�ļ�
	target.WriteFile(State.m_szOutputFile);
}

/*
int main(int argc, char** argv)
{
	//���������в���
	if (!State.CommandInit(argc,argv))
	{
		PrintError();
		return 1;
	}

	//ϵͳ��ʼ��
	Init();

	//�ʷ�����
	if (!Lex.GenToken(&TokenList))
	{
		PrintError();
		return 1;
	}

	//�﷨���������
	if (!Syntax.SyntaxParse())
	{
		PrintError();
		return 1;
	}







}
*/

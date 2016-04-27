#include "StdAfx.h"
//用户自定义文件
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
    //处理命令行参数
    if (!State.CommandInit(argc,argv))
    {
        PrintError();
        return 1;
    }

    //系统初始化
    Init();

    //词法分析
    //extern CLex Lex;
    if (!Lex.GenToken(&TokenList))
    {
    		PrintError();
    		return 1;
    }


	//语法、语义分析
	if (!Syntax.SyntaxParse())
	{
	    cout<<"---11"<<endl;
		PrintError();
		return 1;
	}
    //输出IR
	SymbolTbl.PrintIR();


	//IR优化
	if (!State.m_bOpti)
	{
		COptimization::OptiPass();
	}
	//运行时刻环境
	CMemoryAlloc::MemoryAlloc();
	//目标代码生成
	target.IRtoASM();

	//输出asm文件
	target.WriteFile(State.m_szOutputFile);
}

/*
int main(int argc, char** argv)
{
	//处理命令行参数
	if (!State.CommandInit(argc,argv))
	{
		PrintError();
		return 1;
	}

	//系统初始化
	Init();

	//词法分析
	if (!Lex.GenToken(&TokenList))
	{
		PrintError();
		return 1;
	}

	//语法、语义分析
	if (!Syntax.SyntaxParse())
	{
		PrintError();
		return 1;
	}







}
*/

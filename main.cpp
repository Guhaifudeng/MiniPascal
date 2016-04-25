#include "StdAfx.h"
//用户自定义文件
#include "Structure.h"
#include "ErrorProc.h"
#include "CommonLib.h"
#include "State.h"
#include "Lex.h"
#include "Global.h"
using namespace std;

void Init()//hehe
{
	Lex.LexInit();
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


}

#include "StdAfx.h"
using namespace std;

/*
void Init()
{
	Lex.LexInit();
	Lex.SetFileName(State.m_szSourceFile);
}
*/                      //文件位置
int main(int argc, char** argv)
{
    //处理命令行参数
    if (!State.CommandInit(argc,argv))
    {
        PrintError();
        return 1;
    }

    //系统初始化
    //Init();

    //词法分析
    //extern CLex Lex;
    /*	if (!Lex.GenToken(&TokenList))
    	{
    		PrintError();
    		return 1;
    	}
    */

}

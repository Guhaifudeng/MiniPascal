#include "StdAfx.h"
using namespace std;

/*
void Init()
{
	Lex.LexInit();
	Lex.SetFileName(State.m_szSourceFile);
}
*/                      //�ļ�λ��
int main(int argc, char** argv)
{
    //���������в���
    if (!State.CommandInit(argc,argv))
    {
        PrintError();
        return 1;
    }

    //ϵͳ��ʼ��
    //Init();

    //�ʷ�����
    //extern CLex Lex;
    /*	if (!Lex.GenToken(&TokenList))
    	{
    		PrintError();
    		return 1;
    	}
    */

}

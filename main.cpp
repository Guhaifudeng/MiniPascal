#include "StdAfx.h"
//�û��Զ����ļ�
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


}

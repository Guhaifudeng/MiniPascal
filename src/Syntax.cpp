#include "Syntax.h"
#include "Afx.h"
#include "State.h"
#include <iostream>

using namespace std;

extern CState State;

CSyntax::CSyntax()
{

}

void CSyntax::Init()
{
	int iRow=1;
	int iCol=1;
	string sTmpTxt=FileRead(State.m_szSysDir+"parse.txt");
	string szBuffer("");

	int iCnt=0;

	for(int i=0;i<99;i++)
	{
		for(int j=0;j<100;j++)
		{
			m_iParseTbl[i][j]=atoi(sTmpTxt.substr(iCnt,3).c_str());
			iCnt=iCnt+3;
		}
	}

	SetTbl(FileRead(State.m_szSysDir+"product.txt"),m_szProductList,1);
}

bool CSyntax::SyntaxParse()
{
	int iRow=0;
	int iCol=0;
	int iVal=0;
	iListPos=0;

	ClearStack();

	m_ParseStack.push(100);

	while(!m_ParseStack.empty() && iListPos<TokenList.size())
	{
		if (DeStack(iVal))
		{
			if (iVal==0)//空 推导 候选式
			{
				continue;
			}
			if (iVal<100)//终结符
			{
				if (iVal==TokenList.at(iListPos).m_iKind)
				{
					iListPos++;
				}
				else
				{
					EmitError("语法错误",TokenList.at(iListPos-1));
					break;
				}
			}
			if (iVal>=100 && iVal<300)//非终结符
			{
				iCol=TokenList.at(iListPos).m_iKind;
				iRow=iVal-100;
				int iTmp=m_iParseTbl[iRow][iCol];

				if (iTmp==-1)
				{
					EmitError("语法错误",TokenList.at(iListPos-1));
					break;
				}
				else
				{
					EnStack(m_szProductList[iTmp]);
				}
			}
			if (iVal>=300)//语义子程序标号
			{
				cout<<iVal<<',';

				/*if (!FuncList[iVal-300]())
				{
					break;
				}
				*/
			}
		}
	}

	return (m_ParseStack.empty() && iListPos==TokenList.size());
}

void CSyntax::EnStack(string szProduct)
{
	if (szProduct.length()<=0)
	{
		return;
	}

	for(int i=szProduct.length()-3;i>=0;i=i-3)
	{
		m_ParseStack.push(atoi(szProduct.substr(i,3).c_str()));
	}
}

void  CSyntax::ClearStack()
{
    while(!m_ParseStack.empty())
        m_ParseStack.pop();
}

bool CSyntax::DeStack(int &iTop)
{
	if (m_ParseStack.empty())
	{
		return false;
	}
	else
	{
		iTop=m_ParseStack.top();
		m_ParseStack.pop();
		return true;
	}
}

CSyntax::~CSyntax(void)
{
}

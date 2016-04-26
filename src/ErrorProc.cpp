#include "Afx.h"
#include "ErrorProc.h"

extern vector<ErrorInfo> ErrorProcess;         //错误信息表
extern CState State;
void EmitError (string szErr)
{
    ErrorInfo Tmp;
	Tmp.m_szErrStr = szErr;
	Tmp.m_iRow = 0;
	ErrorProcess.push_back(Tmp);
}
void WriteProcErr(vector<ErrorInfo> ErrorProcess,ofstream &fout)
{
	for(int i=0;i<ErrorProcess.size();i++)
	{
	    	if (ErrorProcess.at(i).m_iRow == 0)
			{
				fout<<"[Error]"<<ErrorProcess.at(i).m_szErrStr.c_str()<<endl;
			}
			else
			{
				fout<<"[Error]"<<ErrorProcess.at(i).m_szFileName.c_str()
					<<"("<<ErrorProcess.at(i).m_iRow<<"):"
					<<ErrorProcess.at(i).m_szErrStr.c_str()<<endl;
			}
	}
}

void PrintError()
{
	cout<<endl;
    string tmp = State.m_szErrFile+"error.txt";
    ofstream fout(tmp.c_str());
	if (!ErrorProcess.empty())
	{
        WriteProcErr(ErrorProcess,fout);
	}
	else
	{
		cout<<'\n'<<"语法出错"<<endl;
	}
	fout.close();
}

void EmitError(string szErr,CToken token)
{
	ErrorInfo Tmp;
	Tmp.m_szErrStr = szErr;
	Tmp.m_iRow = token.m_LineInfo.m_iRow;
	Tmp.m_szFileName = token.m_LineInfo.m_szFileName;
	ErrorProcess.push_back(Tmp);
}
#include "Afx.h"
#include "ErrorProc.h"

extern vector<ErrorInfo> ErrorProcess;         //¥ÌŒÛ–≈œ¢±Ì

void EmitError (string szErr)
{
    ErrorInfo Tmp;
	Tmp.m_szErrStr = szErr;
	Tmp.m_iRow = 0;
	ErrorProcess.push_back(Tmp);
}

void PrintError()
{

}
void EmitError(string szErr,CToken token)
{
	ErrorInfo Tmp;
	Tmp.m_szErrStr = szErr;
	Tmp.m_iRow = token.m_LineInfo.m_iRow;
	Tmp.m_szFileName = token.m_LineInfo.m_szFileName;
	ErrorProcess.push_back(Tmp);
}
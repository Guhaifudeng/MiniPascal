#ifndef STRUCTURE_H
#define STRUCTURE_H
#include "StdAfx.h"
using namespace std;
struct LineInfo
{
    int m_iRow;
    string m_szFileName;
    LineInfo(int iRow,string szFileName);
    LineInfo();
};

struct CToken
{
    CToken();
    int m_iKind;
    string m_szContent;
    LineInfo m_LineInfo;
    CToken(int iKind,string szContent,int iRow,string szFileName);
    ~CToken(void);
};
struct ErrorInfo
{
    string m_szErrStr;
    string m_szFileName;
    int m_iRow;
};

#endif // STRUCTURE_H

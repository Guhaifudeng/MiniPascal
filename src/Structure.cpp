#include "Structure.h"
#include "Afx.h"
/********************词法分析***********************************/
LineInfo::LineInfo()
{

}
LineInfo::LineInfo(int iRow,string szFileName)
{
    m_iRow=iRow;
    m_szFileName=szFileName;
}

CToken::CToken()
{

}
CToken::~CToken()
{

}

CToken::CToken(int iKind,string szContent,int iRow,string szFileName)
{
    m_iKind=iKind;
    m_szContent=szContent;
    m_LineInfo=LineInfo(iRow,szFileName);
    //m_iRow=iRow;
}
/********************语法分析***********************************/
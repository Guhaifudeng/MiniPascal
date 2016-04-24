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
/********************符号表***********************************/
VarInfo::VarInfo()//变量
{
	m_bRef=false;
	m_szReg="";
	m_bUsed=true;
}
ConstInfo::ConstInfo()//常量
{
	m_bUsed=false;
}
ProcInfo::ProcInfo()//过程
{
	m_szName="";
	m_eRank=ProcInfo::Sub;
	m_eType=ProcInfo::Procedure;
	m_ParaTbl.clear();
	m_eFlag=ProcInfo::None;
	m_eCallType=ProcInfo::Stdcall;
	m_iReturnType=-1;
	m_TmpLink=-1;
	m_bUsed=true;
}

TypeInfo::TypeInfo()//类型
{
	m_iLink=-1;
	m_szContent="";
	m_eDataType=T_NONE;
	m_eBaseType=T_NONE;
	m_iState=0;
	m_iSize=0;
}
ArrayInfo::ArrayInfo()//数组
{
	m_iStart=-1;
	m_iEnd=-1;
}

FieldInfo::FieldInfo()//字段
{
	m_iLink=-1;
	m_iSize=0;
	m_szVarFieldFlag="";
	m_szVarFieldConst="";
	m_iState=0;
	m_iOffset=0;
}
/********************中间表示***********************************/

OpInfo::OpInfo()
{
	m_iLink=0;
	while(!m_iDetailType.empty())
        m_iDetailType.pop();
	m_iType=OpInfo::NONE;
	m_bRef=false;
	m_bUninit=false;
}
/***********************声明******************************************/
UseFile::UseFile(string szFileName,bool Flag)
{
	m_szFileName=szFileName;
	m_bFlag=Flag;
}
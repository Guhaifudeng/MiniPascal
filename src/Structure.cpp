#include "Structure.h"
#include "Afx.h"
/********************�ʷ�����***********************************/
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
/********************�﷨����***********************************/
/********************���ű�***********************************/
VarInfo::VarInfo()//����
{
	m_bRef=false;
	m_szReg="";
	m_bUsed=true;
}
ConstInfo::ConstInfo()//����
{
	m_bUsed=false;
}
ProcInfo::ProcInfo()//����
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

TypeInfo::TypeInfo()//����
{
	m_iLink=-1;
	m_szContent="";
	m_eDataType=T_NONE;
	m_eBaseType=T_NONE;
	m_iState=0;
	m_iSize=0;
}
ArrayInfo::ArrayInfo()//����
{
	m_iStart=-1;
	m_iEnd=-1;
}

FieldInfo::FieldInfo()//�ֶ�
{
	m_iLink=-1;
	m_iSize=0;
	m_szVarFieldFlag="";
	m_szVarFieldConst="";
	m_iState=0;
	m_iOffset=0;
}
/********************�м��ʾ***********************************/

OpInfo::OpInfo()
{
	m_iLink=0;
	while(!m_iDetailType.empty())
        m_iDetailType.pop();
	m_iType=OpInfo::NONE;
	m_bRef=false;
	m_bUninit=false;
}
/***********************����******************************************/
UseFile::UseFile(string szFileName,bool Flag)
{
	m_szFileName=szFileName;
	m_bFlag=Flag;
}
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
	m_eDataType=StoreType::T_NONE;
	m_eBaseType=StoreType::T_NONE;
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
/**************************语句**********************************************/

OpInfo Statement::GetLabel(LabelIdx::LabelType TmpLabelType)
{
	int i;
	for (i=0;i<m_Labels.size();i++)
	{
		if (m_Labels.at(i).m_LabelType==TmpLabelType)
			break;
	}
	return m_Labels.at(i).m_Label;
}
/**************************操作数**********************************************/
VarType::VarType(StoreType::CStoreType StoreType,int iLink)
{
	m_StoreType=StoreType;
	m_iLink=iLink;
}
VarType::VarType()
{
}

Var::Var()
{
	m_iVarLink=-1;
	while(!m_VarTypeStack.empty())
        m_VarTypeStack.pop();
	m_eOffsetType=OffsetType::NoneOffset;
	m_iOffsetLink=-1;
	m_bRef=false;
	m_bVarRef=false;
}
/********存储分配**********/
FieldMap::FieldMap(FieldInfo *p,int i)
{
	ptr=p;
	iSize=i;
}
LiveArea::LiveArea(int iDef,int iUse)
{
	m_iDef=iDef;
	m_iUse=iUse;
}

LiveArea::LiveArea()
{
}

TmpInfo::TmpInfo(int iLink,int iDef,int iUse,int iBlock)
{
	m_iLink=iLink;
	m_LiveArea.m_iDef=iDef;
	m_LiveArea.m_iUse=iUse;
	m_iBlock=iBlock;
	//m_bFlag=false;
}

/***********代码生成**************/

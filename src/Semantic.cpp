#include "Semantic.h"
using namespace std;
stack<int> iTypePos;//类型栈
stack<int> iIdListFlag;//标识符列表
stack<int> iTypeFlag;//类型标志
stack<int> iExpListNum;
stack<int> iExpListFlag;
stack<string> szVarFieldFlag;
stack<string> szVarFieldConst;
stack<ProcCall> CurrentProcCall;
stack<Statement> CurrentStatement;
stack<OpInfo> Operand;
stack<Var> CurrentVar;
stack<int> Operation;
stack<Array> ArrayProcess;
stack<WithField> WithStack;
WithField TmpWithField;
bool bWithFlag;
bool bForwardFlag;
bool bArrayFlag;
bool bConstFlag;
bool bLabelFlag;
int iEnumSize;
extern int iListPos;
extern vector<CToken> TokenList;
extern CSymbolTbl SymbolTbl;
extern vector<ErrorInfo> ErrorProcess;
extern CLex Lex;
extern CState State;

void semantic000()
{
	iTypePos.c.clear();
	iIdListFlag.c.clear();
	iTypeFlag.c.clear();
	iExpListNum.c.clear();
	iExpListFlag.c.clear();
	szVarFieldFlag.c.clear();
	szVarFieldConst.c.clear();
	CurrentProcCall.c.clear();
	CurrentStatement.c.clear();
	Operand.c.clear();
	CurrentVar.c.clear();
	Operation.c.clear();
	ArrayProcess.c.clear();
	WithStack.c.clear();
}

//程序头 -> program 标识符 001 ( 标识符列表 ) ;
bool semantic001()
{
	semantic000();
	ProcInfo Tmp;
	Tmp.m_szName=TokenList.at(iListPos-1).m_szContent;
	Tmp.m_eRank=ProcInfo::Rank::Main;
	Tmp.m_eType=ProcInfo::Type::Procedure;
	Tmp.m_ParaTbl.clear();
	SymbolTbl.ProcInfoTbl.push_back(Tmp);
	SymbolTbl.ProcStack.push(SymbolTbl.ProcInfoTbl.size()-1);
	iIdListFlag.push(0);//标识符列表
	iTypeFlag.push(0);
	return true;
}

//程序 -> 程序头 程序块 002 .
bool semantic002()
{
	for (int i=0;i<SymbolTbl.LabelInfoTbl.size();i++)
	{
		if (SymbolTbl.LabelInfoTbl[i].m_szName[0]!='_'
			&& !SymbolTbl.LabelInfoTbl[i].m_bDef
			&& SymbolTbl.LabelInfoTbl[i].m_bUse)
		{
			EmitError(SymbolTbl.LabelInfoTbl[i].m_szName+"标号未定义",TokenList.at(iListPos-1));
			return false;
		}
	}
	return true;
}

//标号声明部分 -> label 002 标识符 003 标号声明列表 ;
//标号声明列表 -> , 标识符 003 标号声明列表
bool semantic003()
{
	LabelInfo Tmp;
	Tmp.m_szName=TokenList.at(iListPos-1).m_szContent;
	Tmp.m_bDef=false;
	Tmp.m_bUse=false;
	Tmp.m_iProcIndex=SymbolTbl.ProcStack.top();

	if (SymbolTbl.SearchLabelInfoTbl(Tmp.m_iProcIndex,Tmp.m_szName)==-1
		&& SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_szName.compare(Tmp.m_szName)!=0)
	{
		SymbolTbl.AddLabel(Tmp);
		return true;
	}
	else
	{
		EmitError(Tmp.m_szName.append("标识符已经存在"),TokenList.at(iListPos-1));
		return false;
	}
}

//常量定义 -> 标识符 102 = 表达式 004
bool semantic004()
{
	string szID=TokenList.at(iListPos-3).m_szContent;

	if (SymbolTbl.SearchLabelInfoTbl(SymbolTbl.ProcStack.top(),szID)==-1
		&& SymbolTbl.SearchConstInfoTbl(SymbolTbl.ProcStack.top(),szID)==-1)
	{
		ConstInfo Tmp;

		//这里，只处量表达式是常数值的情况

		Tmp=SymbolTbl.ConstInfoTbl[atoi(TokenList.at(iListPos-1).m_szContent.c_str())];
		Tmp.m_szName=szID;
		Tmp.m_iProcIndex=SymbolTbl.ProcStack.top();
		SymbolTbl.AddConst(Tmp);
		return true;
	}
	else
	{
		EmitError(szID.append("标识符已经存在"),TokenList.at(iListPos-3));
		return false;
	}

}

//过程调用语句 -> ε 005
bool semantic005()
{
	if (CurrentVar.empty() && !bConstFlag)
	{
		OpInfo Tmp;

		if (CurrentProcCall.empty())
		{
			return false;
		}

		Tmp.m_iType=OpInfo::PROC;
		Tmp.m_iLink=CurrentProcCall.top().m_iProcId;
		Operand.push(Tmp);
		CurrentProcCall.pop();
	}
	return true;
}

//类型定义 -> 标识符 006 = 类型
bool semantic006()
{
	TypeInfo Tmp;
	Tmp.m_szName=TokenList.at(iListPos-1).m_szContent;
	Tmp.m_iProcIndex=SymbolTbl.ProcStack.top();

	if (SymbolTbl.SearchTypeInfoTbl(SymbolTbl.ProcStack.top(),Tmp.m_szName)==-1
		&& SymbolTbl.SearchConstInfoTbl(SymbolTbl.ProcStack.top(),Tmp.m_szName)==-1
		&& SymbolTbl.SearchLabelInfoTbl(SymbolTbl.ProcStack.top(),Tmp.m_szName)==-1
		&& SymbolTbl.SearchEnumInfoTbl(SymbolTbl.ProcStack.top(),Tmp.m_szName)==-1
		&& SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_szName.compare(Tmp.m_szName)!=0)
	{
		SymbolTbl.AddType(Tmp);
		iTypePos.push(SymbolTbl.TypeInfoTbl.size()-1);
		return true;
	}
	else
	{
		EmitError(Tmp.m_szName.append("标识符已经存在"),TokenList.at(iListPos-1));
		return false;
	}
}


/*
  有序类型	-> char 007
            -> boolean 007
  整数类型	-> integer 007
  			-> byte 007
  			-> shortint 007
  			-> smallint 007
  			-> word 007
 			-> longword 007
  			-> cardinal 007
  无序类型	-> real 007
  			-> single 007
*/
bool semantic007()//字符串---
{
	if (iTypeFlag.top()==2)	//集合类型----当前匿名信息未入栈
	{
		SymbolTbl.TypeInfoTbl[iTypePos.top()].m_iLink=SymbolTbl.TypeInfoTbl.size();
		SymbolTbl.TypeInfoTbl[iTypePos.top()].m_iState=1;
		TypeInfo Tmp;
		Tmp.m_iState=1;
		Tmp.m_szName="_noname";
		Tmp.m_szName.append(GetSerialId());
		Tmp.m_iProcIndex=SymbolTbl.ProcStack.top();
		Tmp.m_eDataType=CType::TokenToEnum(TokenList.at(iListPos-1).m_iKind);
		Tmp.m_eBaseType=CType::TokenToEnum(TokenList.at(iListPos-1).m_iKind);
		if (Tmp.m_eBaseType==StoreType::T_BOOLEAN
			|| Tmp.m_eBaseType==StoreType::T_BYTE
			|| Tmp.m_eBaseType==StoreType::T_CHAR
			|| Tmp.m_eBaseType==StoreType::T_SHORTINT )
		{
			SymbolTbl.AddType(Tmp);
			return true;
		}
		else
		{
			EmitError("集合类型的基类型不正确",TokenList.at(iListPos-1));
			return false;
		}
	}

	if (iTypeFlag.top()==5
		|| iTypeFlag.top()==6)	//过程、函数形参列表
	{
		for(int i=SymbolTbl.ProcInfoTbl.back().m_ParaTbl.size()-1;i>=0;i--)
		{
			if (SymbolTbl.ProcInfoTbl.back().m_ParaTbl.at(i).m_iParaType==-1)
			{
				SymbolTbl.ProcInfoTbl.back().m_ParaTbl.at(i).m_iParaType=SymbolTbl.TypeInfoTbl.size();
			}
		}
		TypeInfo Tmp;
		Tmp.m_iState=1;
		Tmp.m_szName="_noname";
		Tmp.m_szName.append(GetSerialId());
		Tmp.m_iProcIndex=SymbolTbl.ProcStack.top();
		Tmp.m_eDataType=CType::TokenToEnum(TokenList.at(iListPos-1).m_iKind);
		Tmp.m_eBaseType=CType::TokenToEnum(TokenList.at(iListPos-1).m_iKind);
		SymbolTbl.AddType(Tmp);
		return true;
	}

	if (iTypeFlag.top()==7)		//函数返回类型
	{
		SymbolTbl.ProcInfoTbl.back().m_iReturnType=SymbolTbl.TypeInfoTbl.size();
		TypeInfo Tmp;
		Tmp.m_iState=1;
		Tmp.m_szName="_noname";
		Tmp.m_szName.append(GetSerialId());
		Tmp.m_iProcIndex=SymbolTbl.ProcStack.top();
		Tmp.m_eDataType=CType::TokenToEnum(TokenList.at(iListPos-1).m_iKind);
		Tmp.m_eBaseType=CType::TokenToEnum(TokenList.at(iListPos-1).m_iKind);
		SymbolTbl.AddType(Tmp);
		return true;
	}

	if (!iTypePos.empty()
		&& iTypePos.top()!=-1
		&& SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eDataType==StoreType::T_NONE)	//标量类型 初始为此
	{
		StoreType TmpStoreType=CType::TokenToEnum(TokenList.at(iListPos-1).m_iKind);
		SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eDataType=TmpStoreType;
		SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eBaseType=TmpStoreType;
		SymbolTbl.TypeInfoTbl[iTypePos.top()].m_iState=1;
	}

	return true;
}

/*
指针类型	-> ^ 简单类型
简单类型	-> 标识符 008
*/
bool semantic008()
{
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eDataType=StoreType::T_POINTER;
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_szContent=TokenList.at(iListPos-1).m_szContent;
    //指针类型的基类型不一定在指针类型前声明
	int i=SymbolTbl.SearchTypeInfoTbl(SymbolTbl.ProcStack.top(),TokenList.at(iListPos-1).m_szContent,false);

	if (i!=-1)
	{
		if (SymbolTbl.TypeInfoTbl[i].m_iState!=1)//避免指针引用自身--指向同一匿名类型
		{
			EmitError("指针指向的类型声明不完整",TokenList.at(iListPos-1));
			return false;
		}
	}
	return true;
}

/*
  指针类型	-> ^ 简单类型
  简单类型	-> 标识符 008
			-> char 009
			-> boolean 009
			-> integer 009
			-> byte 009
			-> shortint 009
			-> smallint 009
			-> word 009
			-> longword 009
			-> cardinal 009
			-> real 009
			-> single 009
*/
bool semantic009()
{
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eDataType=StoreType::T_POINTER;
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_iLink=SymbolTbl.TypeInfoTbl.size();	//设置指针类型的基类型

	TypeInfo Tmp;
	Tmp.m_iState=1;
	Tmp.m_szName="_noname";
	Tmp.m_szName.append(GetSerialId());
	Tmp.m_iProcIndex=SymbolTbl.ProcStack.top();
	Tmp.m_eDataType=CType::TokenToEnum(TokenList.at(iListPos-1).m_iKind);
	Tmp.m_eBaseType=CType::TokenToEnum(TokenList.at(iListPos-1).m_iKind);
	SymbolTbl.AddType(Tmp);
	return true;
}

/*
  类型		-> 015 基本类型 010
			-> 015 函数类型 010
			-> 015 构造类型 010
			-> 015 指针类型 010	^
			-> 015 字符串类型 010
			-> 015 标识符 023 010
*/
bool semantic010()
{
	if (!iTypePos.empty() && iTypePos.top()!=-1)
	{
		SymbolTbl.TypeInfoTbl[iTypePos.top()].m_iState=1;

		iTypePos.pop();
	}
	return true;
}

// 枚举类型		-> ( 011 标识符列表 ) 014
bool semantic011()
{
	iEnumSize=SymbolTbl.EnumInfoTbl.size();

	if (iTypeFlag.top()==2)		//集合类型的基类型可以是枚举类型
	{
		SymbolTbl.TypeInfoTbl[iTypePos.top()].m_iLink=SymbolTbl.TypeInfoTbl.size();	//设置集合类型的基类型
		SymbolTbl.TypeInfoTbl[iTypePos.top()].m_iState=1;

		TypeInfo Tmp;
		Tmp.m_iState=1;
		Tmp.m_szName="_noname";
		Tmp.m_szName.append(GetSerialId());
		Tmp.m_iProcIndex=SymbolTbl.ProcStack.top();
		Tmp.m_eDataType=StoreType::T_ENUM;
		Tmp.m_iLink=SymbolTbl.EnumInfoTbl.size();
		iIdListFlag.push(1);
		SymbolTbl.AddType(Tmp);
		return true;
	}

	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eDataType=StoreType::T_ENUM;
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_iLink=SymbolTbl.EnumInfoTbl.size();
	iIdListFlag.push(1);

	return true;
}

/*
  标识符列表		-> 标识符 012 标识符列表１
  标识符列表１		-> , 标识符 012 标识符列表１
*/
bool semantic012()
{
	if (iIdListFlag.top()==1)	//枚举类型标识符列表
	{
		EnumInfo Tmp;
		Tmp.m_szName=TokenList.at(iListPos-1).m_szContent;
		Tmp.m_iProcIndex=SymbolTbl.ProcStack.top();

		if (SymbolTbl.SearchTypeInfoTbl(SymbolTbl.ProcStack.top(),Tmp.m_szName,false)==-1
			&& SymbolTbl.SearchConstInfoTbl(SymbolTbl.ProcStack.top(),Tmp.m_szName)==-1
			&& SymbolTbl.SearchLabelInfoTbl(SymbolTbl.ProcStack.top(),Tmp.m_szName)==-1
			&& SymbolTbl.SearchEnumInfoTbl(SymbolTbl.ProcStack.top(),Tmp.m_szName)==-1
			&& SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_szName.compare(Tmp.m_szName)!=0)
		{
			if ((SymbolTbl.EnumInfoTbl.size()-iEnumSize)<=256)
			{
				SymbolTbl.EnumInfoTbl.push_back(Tmp);
				return true;
			}
			else
			{
				EmitError("枚举类型的标号个数不得大于256",TokenList.at(iListPos-1));
				return false;
			}
		}
		else
		{
			EmitError("标识符名已经存在，或该标识符已定义",TokenList.at(iListPos-1));
			return false;
		}
	}

	if (iIdListFlag.top()==2)	//记录类型字段名列表中的标识符
	{
		FieldInfo Tmp;
		Tmp.m_szName=TokenList.at(iListPos-1).m_szContent;
		Tmp.m_iState=0;
		Tmp.m_iProcIndex=SymbolTbl.ProcStack.top();

		for(int i=SymbolTbl.TypeInfoTbl[iTypePos.top()].m_FieldInfo.size()-1;i>=0;i--)
		{
			string szTmp=SymbolTbl.TypeInfoTbl[iTypePos.top()].m_FieldInfo.at(i).m_szName;

			if (szTmp.compare(Tmp.m_szName)==0)
			{
				EmitError(Tmp.m_szName.append("域名已经存在"),TokenList.at(iListPos-1));
				return false;
			}
		}

		SymbolTbl.TypeInfoTbl[iTypePos.top()].m_FieldInfo.push_back(Tmp);
	}

	if (iIdListFlag.top()==3)	//记录类型可变字段名列表中的标识符
	{
		FieldInfo Tmp;
		Tmp.m_szName=TokenList.at(iListPos-1).m_szContent;
		Tmp.m_iState=0;
		Tmp.m_iProcIndex=SymbolTbl.ProcStack.top();
		Tmp.m_szVarFieldConst=szVarFieldConst.top();
		Tmp.m_szVarFieldFlag=szVarFieldFlag.top();

		for(int i=SymbolTbl.TypeInfoTbl[iTypePos.top()].m_FieldInfo.size()-1;i>=0;i--)
		{
			FieldInfo *pTmp=&SymbolTbl.TypeInfoTbl[iTypePos.top()].m_FieldInfo.at(i);
			if (pTmp->m_szName.compare(Tmp.m_szName)==0)
			{
				EmitError(Tmp.m_szName.append("域名已经存在"),TokenList.at(iListPos-1));
				return false;
			}
		}

		SymbolTbl.TypeInfoTbl[iTypePos.top()].m_FieldInfo.push_back(Tmp);
	}

	if (iIdListFlag.top()==4 || iIdListFlag.top()==5)	//过程函数形式参数列表的标识符
	{
		ParaInfo Tmp;
		Tmp.m_szName=TokenList.at(iListPos-1).m_szContent;
		Tmp.m_eAssignType=iIdListFlag.top()==4?ParaInfo::VAL:ParaInfo::VAR;
		Tmp.m_iParaType=-1;

		for(int i=SymbolTbl.ProcInfoTbl.back().m_ParaTbl.size()-1;i>=0;i--)
		{
			if (SymbolTbl.ProcInfoTbl.back().m_ParaTbl.at(i).m_szName.compare(Tmp.m_szName)==0)
			{
				EmitError(Tmp.m_szName.append("标识符名已经存在"),TokenList.at(iListPos-1));
				return false;
			}
		}
		SymbolTbl.ProcInfoTbl.back().m_ParaTbl.push_back(Tmp);
		return true;
	}

	if (iIdListFlag.top()==6)	//变量列表的标识符
	{
		VarInfo Tmp;
		Tmp.m_eRank=VarInfo::VAR;
		Tmp.m_iProcIndex=SymbolTbl.ProcStack.top();
		Tmp.m_iTypeLink=SymbolTbl.TypeInfoTbl.size();
		Tmp.m_szName=TokenList.at(iListPos-1).m_szContent;

		if (SymbolTbl.SearchTypeInfoTbl(SymbolTbl.ProcStack.top(),Tmp.m_szName,false)==-1
			&& SymbolTbl.SearchConstInfoTbl(SymbolTbl.ProcStack.top(),Tmp.m_szName)==-1
			&& SymbolTbl.SearchLabelInfoTbl(SymbolTbl.ProcStack.top(),Tmp.m_szName)==-1
			&& SymbolTbl.SearchEnumInfoTbl(SymbolTbl.ProcStack.top(),Tmp.m_szName)==-1
			&& SymbolTbl.SearchVarInfoTbl(SymbolTbl.ProcStack.top(),Tmp.m_szName)==-1
			&& SymbolTbl.SearchProcInfoTbl(Tmp.m_szName)==-1)
		{
			SymbolTbl.AddVar(Tmp);
			return true;
		}
		else
		{
			EmitError(Tmp.m_szName.append("标识符名已经存在"),TokenList.at(iListPos-1));
			return false;
		}
	}

	if (iIdListFlag.top()==7)	//处理USES模块包含声明
	{
		string Tmp=TokenList.at(iListPos-1).m_szContent;
		if (SymbolTbl.SearchUseFileTbl(Tmp)==-1)
		{
			SymbolTbl.UseFileTbl.push_back(UseFile(Tmp,false));
			return true;
		}
		else
		{
			EmitError(Tmp.append("模块已经包含"),TokenList.at(iListPos-1));
			return false;
		}
	}

	return true;
}

//  变量定义	-> 013 标识符列表 040 : 类型
bool semantic013()
{
	iIdListFlag.push(6);
	return true;
}

// 枚举类型		-> ( 011 标识符列表 ) 014
bool semantic014()
{
	EnumInfo Tmp;
	int i=iEnumSize;
	int j=0;
	while (i<SymbolTbl.EnumInfoTbl.size())
	{
		ConstInfo TmpConst;
		TmpConst.m_szName=SymbolTbl.EnumInfoTbl.at(i).m_szName;
		TmpConst.m_iProcIndex=SymbolTbl.ProcStack.top();
		TmpConst.m_iVal=j++;
		TmpConst.m_iEnumIdx=SymbolTbl.TypeInfoTbl.size()-1;
		TmpConst.m_ConstType=ConstType::ENUM;
		TmpConst.m_StoreType=StoreType::T_ENUM;
		SymbolTbl.AddConst(TmpConst);
		i++;
	}
	Tmp.m_szName="-1";
	SymbolTbl.EnumInfoTbl.push_back(Tmp);
	iIdListFlag.pop();
	return true;
}

/*
  类型		-> 015 基本类型 010
 			-> 015 函数类型 010
			-> 015 构造类型 010
			-> 015 指针类型 010
  			-> 015 字符串类型 010
  			-> 015 标识符 023 010
*/
bool semantic015()//需要设置基类型时，先判定传进来的是何种类型
{
	if (iTypeFlag.top()==2			//集合类型
		|| iTypeFlag.top()==5		//过程形参列表
		|| iTypeFlag.top()==6		//函数形参列表
		|| iTypeFlag.top()==7)		//函数返回类型
	{
		iTypePos.push(SymbolTbl.TypeInfoTbl.size()-1);//为了完成统一
		return true;
	}

	if (iTypeFlag.top()==3			//数组类型
		|| iTypeFlag.top()==4)//文件类型
	{
		TypeInfo Tmp;
		Tmp.m_szName="_noname";
		Tmp.m_szName.append(GetSerialId());
		Tmp.m_iProcIndex=SymbolTbl.ProcStack.top();
		Tmp.m_iState=0;
		SymbolTbl.TypeInfoTbl[iTypePos.top()].m_iLink=SymbolTbl.TypeInfoTbl.size();
		SymbolTbl.AddType(Tmp);
		iTypePos.push(SymbolTbl.TypeInfoTbl.size()-1);//----当前匿名信息入栈
		return true;
	}

	if (iTypeFlag.top()==1)			//记录类型
	{
		TypeInfo Tmp;
		Tmp.m_szName="_noname";
		Tmp.m_szName.append(GetSerialId());
		Tmp.m_iProcIndex=SymbolTbl.ProcStack.top();
		Tmp.m_iState=0;
		SymbolTbl.AddType(Tmp);
		int i=SymbolTbl.TypeInfoTbl[iTypePos.top()].m_FieldInfo.size()-1;
		int iTmpTypePos=iTypePos.top();
		iTypePos.push(SymbolTbl.TypeInfoTbl.size()-1);//----当前匿名信息入栈

		while (i>=0)
		{
			if (SymbolTbl.TypeInfoTbl[iTmpTypePos].m_FieldInfo.at(i).m_iState==0)
			{
				SymbolTbl.TypeInfoTbl[iTmpTypePos].m_FieldInfo.at(i).m_iLink=iTypePos.top();
				SymbolTbl.TypeInfoTbl[iTmpTypePos].m_FieldInfo.at(i).m_iState=1;
			}
			i--;
		}

	}

	return true;
}

//数组类型		-> array 016 [ 数组界限 数组界限列表 ] of  类型 025
bool semantic016()
{
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eDataType=StoreType::T_ARRAY;
	iTypeFlag.push(3);	//数组类型
	return true;
}

/*
  数组界限列表		->  , 数组界限 数组界限列表
					-> ε
  数组界限			-> 整数常量 017 .. 整数常量 018
*/
bool semantic017()
{
	ArrayInfo Tmp;
	int i=atoi(TokenList.at(iListPos-1).m_szContent.c_str());
	Tmp.m_iStart=atoi(SymbolTbl.ConstInfoTbl[i].m_szVal.c_str());
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_ArrayInfo.push_back(Tmp);
	return true;
}

/*
数组界限列表		->  , 数组界限 数组界限列表
					-> ε
数组界限			-> 整数常量 017 .. 整数常量 018
*/
bool semantic018()
{
	int i=atoi(TokenList.at(iListPos-1).m_szContent.c_str());
	int iTmp=atoi(SymbolTbl.ConstInfoTbl[i].m_szVal.c_str());

	ArrayInfo* tmp=&(SymbolTbl.TypeInfoTbl[iTypePos.top()].m_ArrayInfo.
		at(SymbolTbl.TypeInfoTbl[iTypePos.top()].m_ArrayInfo.size()-1));

	if (tmp->m_iStart<=iTmp)
	{
		tmp->m_iEnd=iTmp;
		return true;
	}
	else
	{
		EmitError("数组定义上限小于下限",TokenList.at(iListPos-1));
		return false;
	}
}

// 记录类型			-> record 019 字段列表 end 020
bool semantic019()
{
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eDataType=StoreType::T_RECORD;
	iTypeFlag.push(1);	//记录类型
	iIdListFlag.push(2);
	return true;
}

// 记录类型			-> record 019 字段列表 end 020
bool semantic020()
{
	iTypeFlag.pop();
	iIdListFlag.pop();
	return true;
}

// 函数类型			-> procedure 021 形参列表 039 044
bool semantic021()
{
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eDataType=StoreType::T_PROC;
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_iLink=SymbolTbl.ProcInfoTbl.size();
	ProcInfo Tmp;
	Tmp.m_eRank=ProcInfo::Sub;
	Tmp.m_eType=ProcInfo::Type;
	Tmp.m_szName="_noname";
	//Tmp.m_szName=SymbolTbl.TypeInfoTbl[iTypePos.top()].m_szName;
	SymbolTbl.ProcInfoTbl.push_back(Tmp);
	iTypeFlag.push(5);	//过程类型
	return true;
}

//函数类型			-> function 022 形参列表 : 038  类型 039 039 044
bool semantic022()
{
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eDataType=StoreType::T_FUNC;
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_iLink=SymbolTbl.ProcInfoTbl.size();
	ProcInfo Tmp;
	Tmp.m_eRank=ProcInfo::Sub;
	Tmp.m_eType=ProcInfo::Type;
	Tmp.m_szName="_noname";
	//Tmp.m_szName=SymbolTbl.TypeInfoTbl.at(iTypePos.top()).m_szName;
	SymbolTbl.ProcInfoTbl.push_back(Tmp);
	iTypeFlag.push(6);	//函数类型
	return true;
}

//类型				-> 015 标识符 023 010
bool semantic023()
{

	string szTmp=TokenList.at(iListPos-1).m_szContent;

	int i=SymbolTbl.SearchTypeInfoTbl(SymbolTbl.ProcStack.top(),szTmp);	//搜索当前过程或函数

	if (i==-1)
	{
		i=SymbolTbl.SearchTypeInfoTbl(0,szTmp);		//搜索主程序
	}

	if (i!=-1)
	{
		if (iTypeFlag.top()==2)	//集合类型
		{
			int j=i;
			string szTmp1;

			while (SymbolTbl.TypeInfoTbl[j].m_eDataType==StoreType::T_USER)
			{
				if (SymbolTbl.TypeInfoTbl[j].m_iLink==-1)
				{
					EmitError("系统分析出错",TokenList.at(iListPos-1));
					return false;
				}
				szTmp1=SymbolTbl.TypeInfoTbl[SymbolTbl.TypeInfoTbl[j].m_iLink].m_szName;

				j=SymbolTbl.SearchTypeInfoTbl(SymbolTbl.ProcStack.top(),szTmp1);
			}

			if (SymbolTbl.TypeInfoTbl[j].m_eBaseType==StoreType::T_BOOLEAN
				|| SymbolTbl.TypeInfoTbl[j].m_eBaseType==StoreType::T_BYTE
				|| SymbolTbl.TypeInfoTbl[j].m_eBaseType==StoreType::T_CHAR
				|| SymbolTbl.TypeInfoTbl[j].m_eBaseType==StoreType::T_SHORTINT
				|| SymbolTbl.TypeInfoTbl[j].m_eBaseType==StoreType::T_SMALLINT
				|| SymbolTbl.TypeInfoTbl[j].m_eDataType==StoreType::T_ENUM)
			{
				SymbolTbl.TypeInfoTbl[iTypePos.top()].m_iLink=i;
				return true;
			}
			else
			{
				EmitError("集合类型的基类型不正确",TokenList.at(iListPos-1));
				return false;
			}
		}

		if (iTypeFlag.top()==5
			|| iTypeFlag.top()==6)	//过程、函数形参列表
		{
			for(int j=SymbolTbl.ProcInfoTbl.back().m_ParaTbl.size()-1;j>=0;j--)
			{
				if (SymbolTbl.ProcInfoTbl.back().m_ParaTbl.at(j).m_iParaType==-1)
				{
					SymbolTbl.ProcInfoTbl.back().m_ParaTbl.at(j).m_iParaType=i;
				}
			}
			return true;
		}

		if (iTypeFlag.top()==7)	//函数返回类型
		{
			SymbolTbl.ProcInfoTbl.back().m_iReturnType=i;
			return true;
		}


		SymbolTbl.TypeInfoTbl[iTypePos.top()].m_iLink=i;
		SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eDataType=StoreType::T_USER;
		return true;
	}
	else
	{
		EmitError(szTmp.append("类型未定义或定义不完整"),TokenList.at(iListPos-1));
		return false;
	}

	return true;
}

//集合类型			-> set 024 of 类型 025
bool semantic024()
{
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eDataType=StoreType::T_SET;
	iTypeFlag.push(2);		//集合类型

	return true;
}

//集合类型			-> set 024 of 类型 025
bool semantic025()
{
	iTypeFlag.pop();
	return true;
}

//已不用
bool semantic026()
{
	iTypeFlag.pop();
	return true;
}

//文件类型			-> file of 027 类型 028
bool semantic027()
{
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eDataType=StoreType::T_FILE;
	iTypeFlag.push(4);		//文件类型
	return true;
}

//文件类型			-> file of 027 类型 028
bool semantic028()
{
	iTypeFlag.pop();
	return true;
}

/*
 字段列表			-> 字段固定部分 字段列表
					-> 字段可变部分 字段列表
					-> ε
 字段固定部分		-> 标识符列表  : 类型 ;
 字段可变部分		-> case  标识符 029 : 类型  of 033 字段可变部分１ 034 end 030 ;
*/
bool semantic029()
{
	FieldInfo Tmp;

	Tmp.m_szName=TokenList.at(iListPos-1).m_szContent;
	Tmp.m_iState=0;
	Tmp.m_iProcIndex=SymbolTbl.ProcStack.top();

	for(int i=SymbolTbl.TypeInfoTbl[iTypePos.top()].m_FieldInfo.size()-1;i>=0;i--)
	{
		string szTmp=SymbolTbl.TypeInfoTbl[iTypePos.top()].m_FieldInfo.at(i).m_szName;

		if (szTmp.compare(Tmp.m_szName)==0)
		{
			EmitError(Tmp.m_szName.append("域名已经存在"),TokenList.at(iListPos-1));
			return false;
		}
	}

	szVarFieldFlag.push(Tmp.m_szName);
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_FieldInfo.push_back(Tmp);

	return true;
}

/*
 字段列表			-> 字段固定部分 字段列表
					-> 字段可变部分 字段列表
					-> ε
 字段固定部分		-> 标识符列表  : 类型 ;
 字段可变部分		-> case  标识符 029 : 类型  of 033 字段可变部分１ 034 end 030 ;
*/
bool semantic030()
{
	szVarFieldFlag.pop();
	return true;
}


// 字段可变部分１	-> 常量 031 : ( 字段列表 ) 032 ; 字段可变部分１
bool semantic031()
{
	int i=atoi(TokenList.at(iListPos-1).m_szContent.c_str());
	szVarFieldConst.push(SymbolTbl.ConstInfoTbl[i].m_szVal);
	return true;
}

// 字段可变部分１	-> 常量 031 : ( 字段列表 ) 032 ; 字段可变部分１
bool semantic032()
{
	szVarFieldConst.pop();
	return true;
}

// 字段可变部分		-> case  标识符 029 : 类型  of 033 字段可变部分１ 034 end 030 ;
bool semantic033()
{
	iIdListFlag.push(3);	//字段可变部分
	return true;
}

// 字段可变部分		-> case  标识符 029 : 类型  of 033 字段可变部分１ 034 end 030 ;
bool semantic034()
{
	iIdListFlag.pop();
	return true;
}

/*
  形参列表			-> ( 参数说明 参数部分 )
					-> ε
  参数部分			-> ; 参数说明 参数部分
					-> ε
  参数说明			-> 035 标识符列表 036 : 类型
*/
bool semantic035()
{
	iIdListFlag.push(4);	//过程函数形式参数列表（值参方式）
	return true;
}

/*
形参列表			-> ( 参数说明 参数部分 )
					-> ε
参数部分			-> ; 参数说明 参数部分
					-> ε
参数说明			-> 035 标识符列表 036 : 类型
*/
bool semantic036()
{
	iIdListFlag.pop();
	return true;
}


/*
 形参列表			-> ( 参数说明 参数部分 )
					-> ε
 参数部分			-> ; 参数说明 参数部分
					-> ε
 参数说明			-> var 037 标识符列表 036 : 类型
*/
bool semantic037()
{
	iIdListFlag.push(5);	//过程函数形式参数列表（变参方式）
	return true;
}

// 函数类型			-> function 022 形参列表 : 038  类型 039 039 044
bool semantic038()
{
	iTypeFlag.push(7);
	return true;
}

// 函数类型			-> function 022 形参列表 : 038  类型 039 039 044
bool semantic039()
{
	iTypeFlag.pop();
	return true;
}

// 变量定义			-> 013 标识符列表 040 : 类型
bool semantic040()
{
	TypeInfo Tmp;
	Tmp.m_szName="_noname";
	Tmp.m_szName.append(GetSerialId());
	Tmp.m_iProcIndex=SymbolTbl.ProcStack.top();
	Tmp.m_iLink=-1;
	SymbolTbl.AddType(Tmp);
	iTypePos.push(SymbolTbl.TypeInfoTbl.size()-1);
	iIdListFlag.pop();
	return true;
}

// 过程头			-> procedure 标识符 041 形参列表 039
bool semantic041()
{
	ProcInfo Tmp;

	Tmp.m_eRank=ProcInfo::Sub;
	Tmp.m_eType=ProcInfo::Procedure;
	Tmp.m_szName=TokenList.at(iListPos-1).m_szContent;

	if (SymbolTbl.SearchTypeInfoTbl(0,Tmp.m_szName,false)!=-1
		&& SymbolTbl.SearchConstInfoTbl(0,Tmp.m_szName)!=-1
		&& SymbolTbl.SearchLabelInfoTbl(0,Tmp.m_szName)!=-1
		&& SymbolTbl.SearchEnumInfoTbl(0,Tmp.m_szName)!=-1)
	{
		EmitError(Tmp.m_szName.append("标识符已经存在"),TokenList.at(iListPos-1));
		return false;
	}

	int i=SymbolTbl.SearchProcInfoTbl(Tmp.m_szName);

	if (i==-1 || (i!=-1 && SymbolTbl.ProcInfoTbl.at(i).m_eFlag==ProcInfo::Forward))
	{
		if (i==-1 && SymbolTbl.SearchVarInfoTbl(0,Tmp.m_szName)!=-1)
		{
			EmitError(Tmp.m_szName.append("标识符已经存在"),TokenList.at(iListPos-1));
			return false;
		}

		TypeInfo Tmp1;
		Tmp1.m_iLink=SymbolTbl.ProcInfoTbl.size();
		Tmp1.m_eDataType=StoreType::T_PROC;
		Tmp1.m_szName="_noname";
		Tmp1.m_szName.append(GetSerialId());
		SymbolTbl.AddType(Tmp1);
		iTypePos.push(SymbolTbl.TypeInfoTbl.size()-1);
		SymbolTbl.ProcInfoTbl.push_back(Tmp);

		VarInfo TmpVar;
		TmpVar.m_szName=Tmp.m_szName;
		TmpVar.m_iProcIndex=0;
		TmpVar.m_eRank=VarInfo::VAR;
		TmpVar.m_iTypeLink=SymbolTbl.TypeInfoTbl.size()-1;
		SymbolTbl.AddVar(TmpVar);

		iTypeFlag.push(5);	//过程类型

		if (i!=-1 && SymbolTbl.ProcInfoTbl.at(i).m_eFlag==ProcInfo::Forward)
		{
			bForwardFlag=true;
		}

		return true;
	}
	else
	{
		EmitError(Tmp.m_szName.append("标识符已经存在"),TokenList.at(iListPos-1));
		return false;
	}
}

//函数头			-> function 标识符 042 形参列表 : 038 类型 039 039 067
bool semantic042()
{
	ProcInfo Tmp;
	Tmp.m_eRank=ProcInfo::Sub;
	Tmp.m_eType=ProcInfo::Function;
	Tmp.m_szName=TokenList.at(iListPos-1).m_szContent;

	if (SymbolTbl.SearchTypeInfoTbl(0,Tmp.m_szName,false)!=-1
		&& SymbolTbl.SearchConstInfoTbl(0,Tmp.m_szName)!=-1
		&& SymbolTbl.SearchLabelInfoTbl(0,Tmp.m_szName)!=-1
		&& SymbolTbl.SearchEnumInfoTbl(0,Tmp.m_szName)!=-1)
	{
		EmitError(Tmp.m_szName.append("标识符已经存在"),TokenList.at(iListPos-1));
		return false;
	}

	int i=SymbolTbl.SearchProcInfoTbl(Tmp.m_szName);

	if (i==-1 || (i!=-1 && SymbolTbl.ProcInfoTbl.at(i).m_eFlag==ProcInfo::Forward))
	{
		if (i==-1 && SymbolTbl.SearchVarInfoTbl(0,Tmp.m_szName)!=-1)
		{
			EmitError(Tmp.m_szName.append("标识符已经存在"),TokenList.at(iListPos-1));
			return false;
		}

		TypeInfo Tmp1;
		Tmp1.m_iLink=SymbolTbl.ProcInfoTbl.size();
		Tmp1.m_eDataType=StoreType::T_FUNC;
		Tmp1.m_szName="_noname";
		Tmp1.m_szName.append(GetSerialId());
		SymbolTbl.AddType(Tmp1);
		iTypePos.push(SymbolTbl.TypeInfoTbl.size()-1);
		SymbolTbl.ProcInfoTbl.push_back(Tmp);

		VarInfo TmpVar;
		TmpVar.m_szName=Tmp.m_szName;
		TmpVar.m_iProcIndex=0;
		TmpVar.m_eRank=VarInfo::VAR;
		TmpVar.m_iTypeLink=SymbolTbl.TypeInfoTbl.size()-1;
		SymbolTbl.AddVar(TmpVar);

		iTypeFlag.push(6);	//过程类型

		if (i!=-1 && SymbolTbl.ProcInfoTbl.at(i).m_eFlag==ProcInfo::Forward)
		{
			bForwardFlag=true;
		}

		return true;
	}
	else
	{
		EmitError(Tmp.m_szName.append("标识符已经存在"),TokenList.at(iListPos-1));
		return false;
	}
}

//过程函数声明部分１		-> 043 044 程序块 047
bool semantic043()
{
	if (bForwardFlag)
	{
		ProcDefToken Tmp1;

		for (int i=iListPos-1;i>=0;i--)
		{
			if (TokenList.at(i).m_iKind==72
				|| TokenList.at(i).m_iKind==60)
			{
				Tmp1.m_TokenList.push_back(TokenList.at(i));
				Tmp1.m_szName=TokenList.at(i+1).m_szContent;
				break;
			}
			else
			{
				Tmp1.m_TokenList.push_back(TokenList.at(i));
			}
		}

		int i=SymbolTbl.SearchProcInfoTbl(Tmp1.m_szName,false);

		if (i!=-1)
		{
			bForwardFlag=false;

			if (SymbolTbl.ProcDefTokenTblCompare(SymbolTbl.ProcDefTokenTbl.at(
							SymbolTbl.ProcInfoTbl.at(i).m_iProcDefToken).m_TokenList,Tmp1.m_TokenList))
			{
				return true;
			}
			else
			{
				EmitError("函数原型与向前引用原型不一致",TokenList.at(iListPos-1));
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}

//过程函数声明部分１		-> 来源指向 044 047
bool semantic044()
{
	int j=0;

	for(int i=0;i<SymbolTbl.ProcInfoTbl.back().m_ParaTbl.size();i++)
	{
		VarInfo Tmp;

		Tmp.m_iTypeLink=SymbolTbl.ProcInfoTbl.back().m_ParaTbl.at(i).m_iParaType;

		StoreType ParaType=SymbolTbl.TypeInfoTbl[CType::GetRealType(Tmp.m_iTypeLink)].m_eDataType;

		if (SymbolTbl.ProcInfoTbl.back().m_ParaTbl.at(i).m_eAssignType==ParaInfo::VAR
			&& (ParaType==StoreType::T_POINTER || ParaType==StoreType::T_STRING))
		{
			EmitError("过程或函数不允许将指针或STRING类型参数以VAR方式调参");
			return false;
		}

		if (j<3 && CType::GetTypeSize(ParaType)<=4)
		{
			switch(j)
			{
			case 0:Tmp.m_szReg="eax";break;
			case 1:Tmp.m_szReg="edx";break;
			case 2:Tmp.m_szReg="ecx";break;
			}
			j++;
		}

		Tmp.m_szName=SymbolTbl.ProcInfoTbl.back().m_ParaTbl.at(i).m_szName;

		if (Tmp.m_iTypeLink==-1)
		{
			EmitError("过程或函数"+SymbolTbl.ProcInfoTbl.back().m_szName+"参数"+Tmp.m_szName+"类型不正确");
			return false;
		}

		Tmp.m_eRank=VarInfo::PARA;
		Tmp.m_iProcIndex=SymbolTbl.ProcInfoTbl.size()-1;

		if (SymbolTbl.ProcInfoTbl.back().m_ParaTbl.at(i).m_eAssignType==ParaInfo::VAR)
		{
			Tmp.m_bRef=true;
		}
		else
		{
			Tmp.m_bRef=false;
		}

		SymbolTbl.ProcInfoTbl.back().m_ParaTbl.at(i).m_iParaVar=SymbolTbl.VarInfoTbl.size();
		SymbolTbl.AddVar(Tmp);
	}
	if (SymbolTbl.ProcInfoTbl.back().m_iReturnType>=0)
	{
		int i;

		for(i=SymbolTbl.VarInfoTbl.size()-1;i>=0;i--)
		{
			if (SymbolTbl.VarInfoTbl[i].m_szName=="RESULT"
				&& SymbolTbl.VarInfoTbl[i].m_iProcIndex==SymbolTbl.ProcInfoTbl.size()-1)
			{
				SymbolTbl.ProcInfoTbl.back().m_iReturnVar=i;
				break;
			}
		}

		if (i<0)
		{
			VarInfo Tmp;
			Tmp.m_szName="RESULT";
			Tmp.m_eRank=VarInfo::RET;
			Tmp.m_iProcIndex=SymbolTbl.ProcInfoTbl.size()-1;
			Tmp.m_iTypeLink=SymbolTbl.ProcInfoTbl.back().m_iReturnType;
			SymbolTbl.ProcInfoTbl.back().m_iReturnVar=SymbolTbl.VarInfoTbl.size();
			SymbolTbl.AddVar(Tmp);
		}
	}

	if (SymbolTbl.ProcInfoTbl.back().m_eType!=ProcInfo::Type)
	{
		SymbolTbl.ProcStack.push(SymbolTbl.ProcInfoTbl.size()-1);
	}

	return true;
};

// 来源指向					-> 045 forward
bool semantic045()
{
	ProcDefToken Tmp1;
	for (int i=iListPos-1;i>=0;i--)
	{
		if (TokenList.at(i).m_iKind==72
			|| TokenList.at(i).m_iKind==60)
		{
			int j;
			Tmp1.m_TokenList.push_back(TokenList.at(i));
			Tmp1.m_szName=TokenList.at(i+1).m_szContent;
			for (j=SymbolTbl.ProcDefTokenTbl.size()-1;j>=0;j--)
			{
				if (Tmp1.m_szName.compare(SymbolTbl.ProcDefTokenTbl.at(j).m_szName)==0)
					break;
			}
			if (j!=-1)
			{
				EmitError(Tmp1.m_szName.append("过程或函数已经存在向前引用，不可重复引用"),TokenList.at(iListPos-1));
				return false;
			}
			SymbolTbl.ProcInfoTbl.back().m_eFlag=ProcInfo::Forward;
			SymbolTbl.ProcInfoTbl.back().m_iProcDefToken=SymbolTbl.ProcDefTokenTbl.size();
			SymbolTbl.ProcDefTokenTbl.push_back(Tmp1);
			return true;
		}
		else
		{
			Tmp1.m_TokenList.push_back(TokenList.at(i));
		}
	}
	return true;
}

//来源指向					-> 字符串常量 046
bool semantic046()
{
	SymbolTbl.ProcInfoTbl.back().m_eFlag=ProcInfo::Extern;
	SymbolTbl.ProcInfoTbl.back().m_iExternStr=atoi(TokenList.at(iListPos-1).m_szContent.c_str());

	string szTmp=SymbolTbl.ConstInfoTbl[atoi(TokenList.at(iListPos-1).m_szContent.c_str())].m_szName;
	string szCallType;
	string szOther;
	szTmp=szTmp.substr(1,szTmp.length()-2);
	int i=szTmp.find(';');

	if (i!=szTmp.npos)
	{
		szCallType=szTmp.substr(0,i);
		szOther=szTmp.substr(i+1);
		i=szOther.find(';');

		if (0>=i || i>=szOther.length()-1)
		{
			EmitError("外部函数的声明错误，必须包含如下信息：调用方式、函数原型、所属文件",TokenList.at(iListPos-1));
			return false;
		}
	}
	else
	{
		EmitError("外部函数的声明错误，必须包含如下信息：调用方式、函数原型、所属文件",TokenList.at(iListPos-1));
		return false;
	}

	if (UpperCase(szCallType)!="STDCALL" && UpperCase(szCallType)!="FASTCALL")
	{
		EmitError("外部函数的调用方式必须显示声明，可选方式为fastcall、stdcall",TokenList.at(iListPos-1));
		return false;
	}

	if (UpperCase(szCallType)=="STDCALL")
	{
		SymbolTbl.ProcInfoTbl.back().m_eCallType=ProcInfo::Stdcall;
	}
	else
	{
		SymbolTbl.ProcInfoTbl.back().m_eCallType=ProcInfo::Fastcall;
	}

	return true;
}

/*
 过程函数声明部分１			-> 043 044 程序块 047
							-> 来源指向 044 047
*/
bool semantic047()
{
	SymbolTbl.ProcStack.pop();
	return true;
}

// 程序块					-> 说明部分 048 语句部分
bool semantic048()
{
	int i;

	for (i=0;i<SymbolTbl.TypeInfoTbl.size();i++)
	{
		SymbolTbl.CalcTypeSize(i);
	}

	if (!SymbolTbl.PtrCheck(i))
	{
		EmitError(SymbolTbl.TypeInfoTbl[i].m_szContent.append("类型不存在"),TokenList.at(iListPos-1));
		return false;
	}


	if (SymbolTbl.ProcStack.empty())
	{
		SymbolTbl.ProcStack.push(0);
	}

	return true;
}

// 因子	-> 常量 049
bool semantic049()
{
	OpInfo Tmp;
	Tmp.m_iType=OpInfo::CONST;
	Tmp.m_iLink=atoi(TokenList.at(iListPos-1).m_szContent.c_str());
	Tmp.m_bRef=false;
	Operand.push(Tmp);
	return true;
}

/*
 表达式１		-> 关系运算符 050 项 051 表达式１
 项１			-> 低优先级运算符 050 因式 051 项１
 因式１			-> 高优先级运算符 050 因子 051 因式１
 因子			-> + 050 因子 052
				-> - 050 因子 052
				-> not 050 因子 052
*/
bool semantic050()
{
	Operation.push(TokenList.at(iListPos-1).m_iKind);
	return true;
}

/*
 表达式１		-> 关系运算符 050 项 051 表达式１
 项１			-> 低优先级运算符 050 因式 051 项１
 因式１			-> 高优先级运算符 050 因子 051 因式１
*/
bool semantic051()
{
	OpInfo Tmp1,Tmp2;
	int TmpResult;
	int TmpOperation;

	if (!Operand.empty())
	{
		Tmp2=Operand.top();
	}
	else
	{
		EmitError("缺少操作数",TokenList.at(iListPos-1));
		return false;
	}

	Operand.pop();

	if (!Operand.empty())
	{
		Tmp1=Operand.top();
	}
	else
	{
		EmitError("缺少操作数",TokenList.at(iListPos-1));
		return false;
	}

	Operand.pop();

	if (!Operation.empty())
	{
		TmpOperation=Operation.top();
	}
	else
	{
		EmitError("缺少操作符",TokenList.at(iListPos-1));
		return false;
	}

	Operation.pop();

	TmpResult=CType::TypeCompatible(Tmp1,Tmp2,TmpOperation);

	if (TmpResult!=-1)
	{
		if (Tmp1.m_iType==OpInfo::CONST && Tmp2.m_iType==OpInfo::CONST)
		{
			return  OpConstSemantic(Tmp1,Tmp2,SymbolTbl.TypeSysTbl.at(TmpResult).m_iProcessId
								,SymbolTbl.TypeSysTbl.at(TmpResult).m_iRsltType);
		}
		else
		{
			OpInfo TmpRslt;
			TmpRslt.m_iType=OpInfo::VAR;
			TmpRslt.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top()
								,(StoreType)SymbolTbl.TypeSysTbl.at(TmpResult).m_iRsltType);

			if (OpVarSemantic(Tmp1,Tmp2,TmpRslt,SymbolTbl.TypeSysTbl.at(TmpResult))==false)
			{
				return false;
			}

			Operand.push(TmpRslt);
		}
		return true;
	}
	else
	{
		EmitError("操作数类型不兼容",TokenList.at(iListPos-1));
		return false;
	}
	return true;
}

/*
 因子			-> + 050 因子 052
				-> - 050 因子 052
				-> not 050 因子 052
*/
bool semantic052()
{
	OpInfo Tmp1;
	int TmpResult;
	int TmpOperation;
	if (!Operand.empty())
	{
		Tmp1=Operand.top();
	}
	else
	{
		EmitError("缺少操作数",TokenList.at(iListPos-1));
		return false;
	}

	Operand.pop();

	if (!Operation.empty())
	{
		TmpOperation=Operation.top();
	}
	else
	{
		EmitError("缺少操作数",TokenList.at(iListPos-1));
		return false;
	}

	Operation.pop();

	TmpResult=CType::TypeCompatible(Tmp1,TmpOperation);

	if (TmpResult!=-1)
	{
		if (Tmp1.m_iType==OpInfo::CONST)
		{
			return  OpConstSemantic(Tmp1,SymbolTbl.TypeSysTbl.at(TmpResult).m_iProcessId
						,SymbolTbl.TypeSysTbl.at(TmpResult).m_iRsltType);
		}
		else
		{
			OpInfo TmpRslt;
			TmpRslt.m_iType=OpInfo::VAR;

			TmpRslt.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top()
						,(StoreType)SymbolTbl.TypeSysTbl.at(TmpResult).m_iRsltType);

			OpVarSemantic(Tmp1,TmpRslt,SymbolTbl.TypeSysTbl.at(TmpResult));

			Operand.push(TmpRslt);
		}
		return true;
	}
	else
	{
		EmitError("操作数类型不兼容",TokenList.at(iListPos-1));
		return false;
	}
	return true;
}

// 标识符起始语句		-> 变量１ 053 := 表达式 064
bool semantic053()
{
	if (bConstFlag)
	{
		bConstFlag=false;
		return true;
	}

	if (CurrentVar.empty())
	{
		return true;
	}

	if (bWithFlag)
	{
		TmpWithField.m_Var=CurrentVar.top();
		TmpWithField.m_iRestoreIR=SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.size()-1;
	}

	OpInfo Tmp;
	Tmp.m_iType=OpInfo::VAR;
	Tmp.m_iLink=CurrentVar.top().m_iVarLink;
	Tmp.m_iDetailType=CurrentVar.top().m_VarTypeStack;

	if (CurrentVar.top().m_VarTypeStack.top().m_StoreType==StoreType::T_POINTER)
	{
		Tmp.m_bRef=false;
	}
	else
	{
		Tmp.m_bRef=CurrentVar.top().m_bRef;
	}

	if (CType::IsOffsetVar(CurrentVar.top().m_VarTypeStack)
		&& CurrentVar.top().m_eOffsetType!=OffsetType::NoneOffset)
	{
		OpInfo Op1,Rslt;
		int iConstOffset=0;

		for(vector<OffsetStruct>::iterator it=CurrentVar.top().m_OffsetVec.begin();it!=CurrentVar.top().m_OffsetVec.end();it++)
		{
			if (it->m_eOffsetType==OffsetType::ConstOffset)
			{
				iConstOffset+=SymbolTbl.ConstInfoTbl[it->m_iOffsetLink].m_iVal;
			}
		}
		if (CurrentVar.top().m_eOffsetType==OffsetType::ConstOffset)
		{

			Op1.m_iType=OpInfo::CONST;
			iConstOffset+=SymbolTbl.ConstInfoTbl[CurrentVar.top().m_iOffsetLink].m_iVal;
			Op1.m_iLink=SymbolTbl.RecConstTbl(itos(iConstOffset),3);
			iConstOffset=0;
		}
		else
		{
			Op1.m_iType=OpInfo::VAR;
			Op1.m_iLink=CurrentVar.top().m_iOffsetLink;
		}

		Rslt.m_iType=OpInfo::VAR;
		Rslt.m_bRef=false;
		Rslt.m_iDetailType=Tmp.m_iDetailType;
		Rslt.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top(),StoreType::T_POINTER);

		if (!SymbolTbl.IsTmpVar(Tmp.m_iLink))
		{
			if (!SymbolTbl.IsVarPara(SymbolTbl.VarInfoTbl[Tmp.m_iLink].m_szName,SymbolTbl.VarInfoTbl[Tmp.m_iLink].m_iProcIndex)
				&& CurrentVar.top().m_bRef==false)
			{
				OpInfo TmpRslt=Rslt;
				TmpRslt.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top(),StoreType::T_POINTER);
				SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.push_back(EmitIR(OpType::GETADDR,Tmp,TmpRslt));
				Tmp=TmpRslt;
			}
			else
			{
				Tmp.m_bRef=false;
			}

		}
		SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.push_back(EmitIR(OpType::ADD_4,Op1,Tmp,Rslt));
		if (iConstOffset!=0)
		{
			Op1.m_iType=OpInfo::CONST;
			Op1.m_iLink=SymbolTbl.RecConstTbl(itos(iConstOffset),3);
			SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.push_back(EmitIR(OpType::ADD_4,Rslt,Op1,Rslt));
		}

		for(vector<OffsetStruct>::iterator it=CurrentVar.top().m_OffsetVec.begin();it!=CurrentVar.top().m_OffsetVec.end();it++)
		{
			if (it->m_eOffsetType==OffsetType::VarOffset)
			{
				Op1.m_iType=OpInfo::VAR;
				Op1.m_bRef=false;
				Op1.m_iLink=it->m_iOffsetLink;
				SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.push_back(EmitIR(OpType::ADD_4,Rslt,Op1,Rslt));
			}
		}

		Rslt.m_bRef=!(CurrentVar.top().m_VarTypeStack.top().m_StoreType==StoreType::T_POINTER);

		Tmp=Rslt;
	}

	Operand.push(Tmp);
	CurrentVar.pop();
	return true;
}

//变量			-> 标识符 054 变量１
bool semantic054()
{
	Var Tmp;
	string szTmp=TokenList.at(iListPos-1).m_szContent;
	int j=WithStack.size()-1;

	//优先处理with开域
	for (;j>=0;j--)
	{
		int i;

		for(i=0;i<SymbolTbl.TypeInfoTbl[WithStack.c.at(j).m_Var.m_VarTypeStack.top().m_iLink].m_FieldInfo.size();i++)
		{
			if (SymbolTbl.TypeInfoTbl[WithStack.c.at(j).m_Var.m_VarTypeStack.top().m_iLink]
				.m_FieldInfo.at(i).m_szName.compare(szTmp)==0)
			{
				break;
			}
		}

		if (i!=SymbolTbl.TypeInfoTbl[WithStack.c.at(j).m_Var.m_VarTypeStack.top().m_iLink].m_FieldInfo.size())
		{
			break;
		}
	}

	if (j>=0)
	{
		CurrentVar.push(WithStack.c.at(j).m_Var);

		return semantic056();
	}

	int i=SymbolTbl.SearchProcInfoTbl(szTmp,true);

	if (i==-1)
	{
		bool bConstProcess;

		i=SymbolTbl.SearchVarInfoTbl(SymbolTbl.ProcStack.top(),szTmp);

		if (i==-1)
		{
			i=SymbolTbl.SearchConstInfoTbl(SymbolTbl.ProcStack.top(),szTmp);

			if (i==-1)
			{
				i=SymbolTbl.SearchVarInfoTbl(0,szTmp);

				if (i==-1)
				{
					i=SymbolTbl.SearchConstInfoTbl(0,szTmp);

					if (i==-1)
					{
						if (szTmp.compare("SIZEOF")==0)
						{
							ProcCall Tmp;
							Tmp.m_iProcId=-1;
							CurrentProcCall.push(Tmp);
							return true;
						}
						else
						{
							EmitError("标识符未声明或非法引用",TokenList.at(iListPos-1));
							return false;
						}
					}
					else
					{
						bConstProcess=true;
					}
				}
				else
				{
					bConstProcess=false;
				}

			}
			else
			{
				bConstProcess=true;
			}
		}
		else
		{
			bConstProcess=false;
		}

		if (bConstProcess)
		{
			OpInfo Tmp;
			Tmp.m_iType=OpInfo::CONST;
			Tmp.m_iLink=i;
			Operand.push(Tmp);
			bConstFlag=true;
		}
		else
		{
			PushVarType(SymbolTbl.VarInfoTbl[i].m_iTypeLink,Tmp);
			Tmp.m_iVarLink=i;
			Tmp.m_iDim=0;
			Tmp.m_bRef=SymbolTbl.VarInfoTbl[i].m_bRef;
			CurrentVar.push(Tmp);
			bConstFlag=false;
		}
	}
	else
	{
		ProcCall Tmp;
		OpInfo TmpRet;
		TmpRet.m_iType=OpInfo::VAR;
		TmpRet.m_iLink=SymbolTbl.ProcInfoTbl.at(i).m_iReturnVar;
		Tmp.m_Return=TmpRet;
		Tmp.m_iProcId=i;
		Tmp.m_eCallType=ProcCall::Call;
		CurrentProcCall.push(Tmp);
	}

	return true;
}

void PushVarType(int iTypeLink,Var &Tmp)
{
	if (SymbolTbl.TypeInfoTbl[CType::GetRealType(iTypeLink)].m_eDataType==StoreType::T_POINTER
		&& SymbolTbl.TypeInfoTbl[CType::GetRealType(iTypeLink)].m_iLink!=-1)
	{
		PushVarType(SymbolTbl.TypeInfoTbl[CType::GetRealType(iTypeLink)].m_iLink,Tmp);
		Tmp.m_VarTypeStack.push(VarType(SymbolTbl.TypeInfoTbl[CType::GetRealType(iTypeLink)].m_eDataType
								,CType::GetRealType(iTypeLink)));

	}
	else
	{
		Tmp.m_VarTypeStack.push(VarType(SymbolTbl.TypeInfoTbl[CType::GetRealType(iTypeLink)].m_eDataType
								,CType::GetRealType(iTypeLink)));
	}
}

//表达式列表		-> 表达式 055 表达式列表１
bool semantic055()
{
	if (iExpListFlag.top()==1)	//处理数组下标访问
	{
		if (bConstFlag
			|| Operand.empty()
			|| CurrentVar.empty())
		{
			EmitError("语义错误",TokenList.at(iListPos-1));
			return false;
		}

		OpInfo Tmp,Op1,Op2,TmpRslt;
		IRCode TmpIR;
		int iTypeLink;
		Var* TmpVar;
		Tmp=Operand.top();	//获取下标表达式
		Operand.pop();
		TmpVar=&CurrentVar.top();	//获取数组符号
		TmpVar->m_iDim=1;
		iTypeLink=TmpVar->m_VarTypeStack.top().m_iLink;

		if (Tmp.m_iType==OpInfo::CONST)	//处理常量下标偏移
		{
			if (CType::IsInt(CType::GetOpType(Tmp))
				&& SymbolTbl.TypeInfoTbl[iTypeLink].m_ArrayInfo.at(0).m_iStart
					<=SymbolTbl.ConstInfoTbl[Tmp.m_iLink].m_iVal
				&& SymbolTbl.TypeInfoTbl[iTypeLink].m_ArrayInfo.at(0).m_iEnd
					>=SymbolTbl.ConstInfoTbl[Tmp.m_iLink].m_iVal)
			{
				int i;

				if (SymbolTbl.TypeInfoTbl[iTypeLink].m_ArrayInfo.size()>=2)
				{
					i=SymbolTbl.TypeInfoTbl[iTypeLink].m_ArrayInfo.at(1).m_iEnd;
					i=i-SymbolTbl.TypeInfoTbl[iTypeLink].m_ArrayInfo.at(1).m_iStart+1;
					i=SymbolTbl.ConstInfoTbl[Tmp.m_iLink].m_iVal*i;
				}
				else
				{
					i=SymbolTbl.ConstInfoTbl[Tmp.m_iLink].m_iVal;
				}

				Op2.m_iLink=SymbolTbl.RecConstTbl(itos(i),3);

				Op2.m_iType=OpInfo::CONST;

				if (TmpVar->m_eOffsetType==OffsetType::ConstOffset
					|| TmpVar->m_eOffsetType==OffsetType::VarOffset)
				{
					TmpVar->m_OffsetVec.push_back(OffsetStruct(TmpVar->m_eOffsetType,TmpVar->m_iOffsetLink));
				}

				Operand.push(Op2);
			}
			else
			{
				EmitError("越界访问或数组下标类型不正确",TokenList.at(iListPos-1));
				return false;
			}
		}
		else	//处理变量下标偏移
		{
			if (!CType::IsInt(CType::GetOpType(Tmp)))
			{
				EmitError("数组下标类型不正确",TokenList.at(iListPos-1));
				return false;
			}

			if (SymbolTbl.TypeInfoTbl[iTypeLink].m_ArrayInfo.size()>=2)
			{
				Op2.m_iLink=SymbolTbl.RecConstTbl(itos(SymbolTbl.TypeInfoTbl[iTypeLink].m_ArrayInfo.at(1).m_iEnd
								-SymbolTbl.TypeInfoTbl[iTypeLink].m_ArrayInfo.at(1).m_iStart+1),3);

				Op2.m_iType=OpInfo::CONST;
				TmpRslt.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top(),StoreType::T_INTEGER);
				TmpRslt.m_iType=OpInfo::VAR;

				if (!GenIR(Tmp,Op2,TmpRslt,BasicOpType::MUL))
				{
					return false;
				}
				Tmp=TmpRslt;
			}

			if (TmpVar->m_eOffsetType==OffsetType::ConstOffset
				|| TmpVar->m_eOffsetType==OffsetType::VarOffset)
			{
				TmpVar->m_OffsetVec.push_back(OffsetStruct(TmpVar->m_eOffsetType,TmpVar->m_iOffsetLink));
			}

			Operand.push(Tmp);
		}
		return true;
	}
	if (iExpListFlag.top()==2)
	{
		iExpListNum.top()++;
	}
	return true;
}

//变量１	-> . 标识符 056 变量１
bool semantic056()
{
	if (bConstFlag || CurrentVar.empty())
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	Var TmpVar;
	string szTmp=TokenList.at(iListPos-1).m_szContent;
	TmpVar=CurrentVar.top();
	int i;

	for(i=0;i<SymbolTbl.TypeInfoTbl[TmpVar.m_VarTypeStack.top().m_iLink].m_FieldInfo.size();i++)
	{
		if (SymbolTbl.TypeInfoTbl[TmpVar.m_VarTypeStack.top().m_iLink].m_FieldInfo.at(i).m_szName.compare(szTmp)==0)
		{
			break;
		}
	}

	if (i!=SymbolTbl.TypeInfoTbl[TmpVar.m_VarTypeStack.top().m_iLink].m_FieldInfo.size())
	{
		FieldInfo *pFieldInfo=&(SymbolTbl.TypeInfoTbl[TmpVar.m_VarTypeStack.top().m_iLink].m_FieldInfo.at(i));

		int iTmpOffset=pFieldInfo->m_iOffset;

		OffsetType TmpOffsetType=OffsetType::ConstOffset;

		if (CurrentVar.top().m_eOffsetType==OffsetType::NoneOffset)
		{
			iTmpOffset=SymbolTbl.RecConstTbl(itos(iTmpOffset),3);
		}

		if (CurrentVar.top().m_eOffsetType==OffsetType::ConstOffset)
		{
			iTmpOffset=SymbolTbl.RecConstTbl(itos(iTmpOffset+SymbolTbl
						.ConstInfoTbl[CurrentVar.top().m_iOffsetLink].m_iVal),3);
		}

		if (CurrentVar.top().m_eOffsetType==OffsetType::VarOffset)
		{
			TmpOffsetType=OffsetType::VarOffset;
			OpInfo Op1,Op2,Rslt;
			Op1.m_iType=OpInfo::VAR;
			Op1.m_iLink=CurrentVar.top().m_iOffsetLink;
			Op2.m_iType=OpInfo::CONST;
			Op2.m_iLink=SymbolTbl.RecConstTbl(itos(iTmpOffset),3);
			Rslt.m_iType=OpInfo::VAR;
			Rslt.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top(),StoreType::T_INTEGER);
			iTmpOffset=Rslt.m_iLink;

			if (!GenIR(Op1,Op2,Rslt,BasicOpType::ADD))
			{
				return false;
			}
		}

		TmpVar.m_eOffsetType=TmpOffsetType;
		TmpVar.m_iOffsetLink=iTmpOffset;
		TmpVar.m_VarTypeStack.push(VarType(SymbolTbl.TypeInfoTbl[CType::GetRealType(pFieldInfo->m_iLink)]
						.m_eDataType,CType::GetRealType(pFieldInfo->m_iLink)));
		CurrentVar.pop();
		CurrentVar.push(TmpVar);
		return true;
	}
	else
	{
		EmitError("字段未声明",TokenList.at(iListPos-1));
		return false;
	}
}

//表达式列表１	-> , 表达式 057 表达式列表１
bool semantic057()
{
	if (iExpListFlag.top()==1)
	{
		if (bConstFlag || Operand.size()<2 || CurrentVar.empty())
		{
			EmitError("语义错误",TokenList.at(iListPos-1));
			return false;
		}

		OpInfo Tmp1,Tmp2,Op1,Op2,TmpRslt;
		IRCode TmpIR;
		Var* TmpVar;

		Tmp1=Operand.top();
		Operand.pop();

		Tmp2=Operand.top();
		Operand.pop();

		TmpVar=&CurrentVar.top();
		int j=++TmpVar->m_iDim;

		if (SymbolTbl.TypeInfoTbl[TmpVar->m_VarTypeStack.top().m_iLink].m_ArrayInfo.size()<j)
		{
			EmitError("数组访问维数大于声明维数",TokenList.at(iListPos-1));
			return false;
		}

		if (Tmp1.m_iType==OpInfo::CONST && Tmp2.m_iType==OpInfo::CONST)	//两个操作数都是常数，常数折叠
		{
			if (CType::IsInt(CType::GetOpType(Tmp1))
				&& CType::IsInt(CType::GetOpType(Tmp2))
				&& SymbolTbl.TypeInfoTbl[TmpVar->m_VarTypeStack.top().m_iLink].m_ArrayInfo.at(j-1).m_iStart
				   <=SymbolTbl.ConstInfoTbl[Tmp1.m_iLink].m_iVal
				&& SymbolTbl.TypeInfoTbl[TmpVar->m_VarTypeStack.top().m_iLink].m_ArrayInfo.at(j-1).m_iEnd
				   >=SymbolTbl.ConstInfoTbl[Tmp1.m_iLink].m_iVal)
			{
				int i;

				//判断是否需要累计维度积，最末维则不需要处理
				if (SymbolTbl.TypeInfoTbl[TmpVar->m_VarTypeStack.top().m_iLink].m_ArrayInfo.size()>j)
				{
					i=(SymbolTbl.ConstInfoTbl[Tmp2.m_iLink].m_iVal+SymbolTbl.ConstInfoTbl[Tmp1.m_iLink].m_iVal)
						*(SymbolTbl.TypeInfoTbl[TmpVar->m_VarTypeStack.top().m_iLink].m_ArrayInfo.at(j).m_iEnd
						-SymbolTbl.TypeInfoTbl[TmpVar->m_VarTypeStack.top().m_iLink].m_ArrayInfo.at(j).m_iStart+1);
				}
				else
				{
					i=(SymbolTbl.ConstInfoTbl[Tmp2.m_iLink].m_iVal+SymbolTbl.ConstInfoTbl[Tmp1.m_iLink].m_iVal);
				}

				char cBuffer[10];
				itoa(i,cBuffer,10);
				Op2.m_iLink=SymbolTbl.RecConstTbl(cBuffer,3);
				Op2.m_iType=OpInfo::CONST;
				Operand.push(Op2);
			}
			else
			{
				EmitError("数组访问越界或数组下标类型不正确",TokenList.at(iListPos-1));
				return false;
			}
		}
		else
		{
			if (!CType::IsInt(CType::GetOpType(Tmp1)) || !CType::IsInt(CType::GetOpType(Tmp2)))
			{
				EmitError("数组下标类型不正确",TokenList.at(iListPos-1));
				return false;
			}

			TmpRslt.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top(),StoreType::T_INTEGER);
			TmpRslt.m_iType=OpInfo::VAR;

			if (!GenIR(Tmp1,Tmp2,TmpRslt,BasicOpType::ADD))
			{
				return false;
			}

			Operand.push(TmpRslt);

			//判断是否需要累计维度积，最末维则不需要处理
			if (SymbolTbl.TypeInfoTbl[TmpVar->m_VarTypeStack.top().m_iLink].m_ArrayInfo.size()>j)
			{
				Op2.m_iLink=SymbolTbl.RecConstTbl(itos(SymbolTbl.TypeInfoTbl[TmpVar->m_VarTypeStack.top().m_iLink].m_ArrayInfo.at(j).m_iEnd
								-SymbolTbl.TypeInfoTbl[TmpVar->m_VarTypeStack.top().m_iLink].m_ArrayInfo.at(j).m_iStart+1),3);
				Op2.m_iType=OpInfo::CONST;
				Op1=Operand.top();
				Op1.m_bRef=false;
				Operand.pop();
				TmpRslt.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top(),StoreType::T_INTEGER);
				TmpRslt.m_iType=OpInfo::VAR;

				if (!GenIR(Op1,Op2,TmpRslt,BasicOpType::MUL))
				{
					return false;
				}

				Operand.push(TmpRslt);
			}
		}

		return true;
	}

	if (iExpListFlag.top()==2)
	{
		iExpListNum.top()++;
	}

	return true;
}

//变量１	-> ^ 058 变量１
bool semantic058()
{
	if (bConstFlag || CurrentVar.empty())
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	Var TmpVar=CurrentVar.top();

	if (TmpVar.m_VarTypeStack.empty()
		|| TmpVar.m_VarTypeStack.top().m_StoreType!=StoreType::T_POINTER)
	{
		EmitError("^运算符只能运用于指针类型",TokenList.at(iListPos-1));
		return false;
	}

	int iTypeLink=CType::GetRealType(SymbolTbl.TypeInfoTbl[TmpVar.m_VarTypeStack.top().m_iLink].m_iLink);

	TmpVar.m_VarTypeStack.pop();

	TmpVar.m_VarTypeStack.push(VarType(SymbolTbl.TypeInfoTbl[iTypeLink].m_eDataType,iTypeLink));

	semantic053();

	OpInfo TmpOp=Operand.top();

	Operand.pop();

	OpInfo TmpOp1,TmpRslt;

	TmpOp1.m_iType=OpInfo::VAR;
	TmpOp1.m_iLink=TmpOp.m_iLink;

	TmpOp1.m_bRef=SymbolTbl.IsTmpVar(TmpOp1.m_iLink);

	TmpRslt.m_iType=OpInfo::VAR;
	TmpRslt.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top(),StoreType::T_POINTER);
	TmpRslt.m_bRef=false;
	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.push_back(EmitIR(OpType::ASSIGN_4,TmpOp1,TmpRslt));
	TmpOp.m_bRef=false;
	TmpOp.m_iLink=TmpRslt.m_iLink;

	TmpVar.m_iVarLink=TmpOp.m_iLink;

	if (TmpVar.m_VarTypeStack.top().m_StoreType == StoreType::T_ARRAY
		|| TmpVar.m_VarTypeStack.top().m_StoreType == StoreType::T_RECORD)
	{
		TmpVar.m_bRef = false;
	}
	else
	{
		TmpVar.m_bRef=true;
	}

	TmpVar.m_eOffsetType=OffsetType::NoneOffset;
	TmpVar.m_iOffsetLink=-1;
	CurrentVar.push(TmpVar);

	return true;
}

//变量１	-> [ 098 表达式列表 099 ] 059 变量１
bool semantic059()
{
	if (bConstFlag || Operand.empty() || CurrentVar.empty())
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	Var TmpVar=CurrentVar.top();

	CurrentVar.pop();

	if (SymbolTbl.TypeInfoTbl[TmpVar.m_VarTypeStack.top().m_iLink].m_ArrayInfo.size()>TmpVar.m_iDim)
	{
		EmitError("数组访问维数小于声明维数",TokenList.at(iListPos-1));
		return false;
	}

	int iArrayLowC=0;

	int i;

	//计算数组偏移的不变部分

	for (i=0;i<SymbolTbl.TypeInfoTbl[TmpVar.m_VarTypeStack.top().m_iLink].m_ArrayInfo.size();i++)
	{
		iArrayLowC=iArrayLowC+SymbolTbl.TypeInfoTbl[TmpVar.m_VarTypeStack.top().m_iLink].m_ArrayInfo.at(i).m_iStart;

		if (i+1<SymbolTbl.TypeInfoTbl[TmpVar.m_VarTypeStack.top().m_iLink].m_ArrayInfo.size())
		{
			iArrayLowC=iArrayLowC*(SymbolTbl.TypeInfoTbl[TmpVar.m_VarTypeStack.top().m_iLink].m_ArrayInfo.at(i+1).m_iEnd
						-SymbolTbl.TypeInfoTbl[TmpVar.m_VarTypeStack.top().m_iLink].m_ArrayInfo.at(i+1).m_iStart+1);
		}
	}

	int iArrayType=SymbolTbl.CalcTypeSize(SymbolTbl.TypeInfoTbl[TmpVar.m_VarTypeStack.top().m_iLink].m_iLink);

	OpInfo Tmp;

	//累计计算数组偏移的可变部分
	if (Operand.top().m_iType==OpInfo::CONST)
	{
		int j=(SymbolTbl.ConstInfoTbl[Operand.top().m_iLink].m_iVal-iArrayLowC)*iArrayType;

		TmpVar.m_VarTypeStack.push(VarType(SymbolTbl.TypeInfoTbl[CType::GetRealType(
						SymbolTbl.TypeInfoTbl[TmpVar.m_VarTypeStack.top().m_iLink].m_iLink)].m_eDataType
						,CType::GetRealType(SymbolTbl.TypeInfoTbl[TmpVar.m_VarTypeStack.top().m_iLink].m_iLink)));

		TmpVar.m_eOffsetType=OffsetType::ConstOffset;
		TmpVar.m_iOffsetLink=SymbolTbl.RecConstTbl(itos(j),3);
		CurrentVar.push(TmpVar);
		Operand.pop();
	}
	else
	{
		OpInfo TmpRslt,TmpOp1,TmpOp2;
		TmpOp2.m_iType=OpInfo::CONST;
		TmpOp2.m_iLink=SymbolTbl.RecConstTbl(itos(iArrayLowC),3);
		TmpRslt.m_iType=OpInfo::VAR;
		TmpRslt.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top(),StoreType::T_INTEGER);
		TmpOp1=Operand.top();

		if (!GenIR(TmpOp1,TmpOp2,TmpRslt,BasicOpType::SUB))
		{
			return false;
		}

		TmpOp1=TmpRslt;
		TmpOp2.m_iLink=SymbolTbl.RecConstTbl(itos(iArrayType),3);
		TmpRslt.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top(),StoreType::T_INTEGER);

		if (!GenIR(TmpOp1,TmpOp2,TmpRslt,BasicOpType::MUL))
		{
			return false;
		}

		Operand.pop();

		VarType tmp;
		int iTmpLink=TmpVar.m_VarTypeStack.top().m_iLink;

		tmp.m_StoreType=SymbolTbl.TypeInfoTbl[iTmpLink].m_iLink==-1
					?SymbolTbl.TypeInfoTbl[CType::GetRealType(iTmpLink)].m_eDataType
					:SymbolTbl.TypeInfoTbl[CType::GetRealType(SymbolTbl.TypeInfoTbl[iTmpLink].m_iLink)].m_eDataType;

		tmp.m_iLink=SymbolTbl.TypeInfoTbl[iTmpLink].m_iLink==-1
					?iTmpLink
					:CType::GetRealType(SymbolTbl.TypeInfoTbl[iTmpLink].m_iLink);

		TmpVar.m_VarTypeStack.push(tmp);
		TmpVar.m_eOffsetType=OffsetType::VarOffset;
		TmpVar.m_iOffsetLink=TmpRslt.m_iLink;
		CurrentVar.push(TmpVar);

	}
	return true;
}

void PushTypeStack(int iTypeLink,stack<VarType> &TmpStack)
{

	int i=CType::GetRealType(SymbolTbl.TypeInfoTbl[iTypeLink].m_iLink);

	if (i==-1 || SymbolTbl.TypeInfoTbl[i].m_eDataType==StoreType::T_POINTER)
	{
		return ;
	}
	else
	{
		PushTypeStack(i,TmpStack);
		TmpStack.push(VarType(SymbolTbl.TypeInfoTbl[i].m_eDataType,i));
	}
}

//因子		-> @ 变量 053 060
bool semantic060()
{
	OpInfo Tmp,Op1,Rslt;

	if (Operand.empty())
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	Tmp=Operand.top();

	Operand.pop();

	if (Tmp.m_iType==OpInfo::CONST
		|| (Tmp.m_iType==OpInfo::VAR
		&& SymbolTbl.IsTmpVar(Tmp.m_iLink)
		&& Tmp.m_bRef==false))
	{
		EmitError("地址运算的操作数必须为左值",TokenList.at(iListPos-1));
		return false;
	}

	TypeInfo TmpType;

	if (Tmp.m_iType==OpInfo::PROC)
	{

		if (SymbolTbl.ProcInfoTbl.at(Tmp.m_iLink).m_eType==ProcInfo::Function)
		{
			TmpType.m_eDataType=StoreType::T_FUNC;
		}
		else
		{
			TmpType.m_eDataType=StoreType::T_PROC;
		}

		TmpType.m_iLink=Tmp.m_iLink;
		SymbolTbl.AddType(TmpType);

		Rslt.m_iDetailType=Tmp.m_iDetailType;
		Rslt.m_iDetailType.push(VarType(TmpType.m_eDataType,SymbolTbl.TypeInfoTbl.size()-1));

		int i=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top(),StoreType::T_POINTER);
		SymbolTbl.VarInfoTbl[i].m_iTypeLink=SymbolTbl.TypeInfoTbl.size()-1;

		Rslt.m_iType=OpInfo::VAR;
		Rslt.m_iLink=SymbolTbl.VarInfoTbl.size()-1;
		Rslt.m_bRef=false;

		SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.push_back(EmitIR(OpType::GETPROCADDR,Op1,Rslt));
		Operand.push(Rslt);
		return true;
	}

	Rslt=Tmp;

	TmpType.m_eDataType=StoreType::T_POINTER;

	if (Tmp.m_iDetailType.empty())
	{
		Rslt.m_iDetailType.push(VarType(SymbolTbl.TypeInfoTbl[SymbolTbl.VarInfoTbl[Rslt.m_iLink].m_iTypeLink].m_eDataType
					,SymbolTbl.VarInfoTbl[Rslt.m_iLink].m_iTypeLink));

		TmpType.m_eBaseType=SymbolTbl.TypeInfoTbl[SymbolTbl.VarInfoTbl[Rslt.m_iLink].m_iTypeLink].m_eDataType;
		TmpType.m_iLink=SymbolTbl.VarInfoTbl[Rslt.m_iLink].m_iTypeLink;
	}
	else
	{
		TmpType.m_eBaseType=SymbolTbl.TypeInfoTbl[Rslt.m_iDetailType.top().m_iLink].m_eDataType;
		TmpType.m_iLink=Rslt.m_iDetailType.top().m_iLink;
	}

	TmpType.m_szName="_noname";
	TmpType.m_szName.append(GetSerialId());
	SymbolTbl.AddType(TmpType);
	Rslt.m_iDetailType.push(VarType(StoreType::T_POINTER,SymbolTbl.TypeInfoTbl.size()-1));
	Rslt.m_bRef=false;
	Rslt.m_iType=OpInfo::VAR;

	if (!Tmp.m_bRef)
	{
		Op1=Tmp;

		int i=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top(),StoreType::T_POINTER);
		SymbolTbl.VarInfoTbl[i].m_iTypeLink=SymbolTbl.TypeInfoTbl.size()-1;
		Rslt.m_iLink=SymbolTbl.VarInfoTbl.size()-1;

		if (SymbolTbl.IsVarPara(SymbolTbl.VarInfoTbl[Op1.m_iLink].m_szName,SymbolTbl.VarInfoTbl[Op1.m_iLink].m_iProcIndex))
		{
			SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.push_back(EmitIR(OpType::ASSIGN_4,Op1,Rslt));
		}
		else
		{
			SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.push_back(EmitIR(OpType::GETADDR,Op1,Rslt));
		}
	}

	Operand.push(Rslt);

	return true;
}

//语句		-> 标识符 061 标识符起始语句
bool semantic061()
{
	string szTmp=TokenList.at(iListPos-1).m_szContent;

	int i=SymbolTbl.SearchLabelInfoTbl(SymbolTbl.ProcStack.top(),szTmp);

	if (i!=-1)
	{
		if (!SymbolTbl.LabelInfoTbl[i].m_bDef)
		{
			OpInfo Tmp;
			Tmp.m_iType=OpInfo::LABEL;
			Tmp.m_iLink=i;
			SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.push_back(EmitIR(OpType::LABEL,Tmp));
			SymbolTbl.LabelInfoTbl[i].m_bDef=true;
			bLabelFlag=true;
			return true;
		}
		else
		{
			EmitError("标号重复定义",TokenList.at(iListPos-1));
			return false;
		}
	}
	else
	{
		return semantic054();
	}

	return true;
}

//标识符起始语句	-> : 062 语句
bool semantic062()
{
	bLabelFlag=false;
	return true;
}

//其他语句		-> goto 标识符 063
bool semantic063()
{
	string szTmp=TokenList.at(iListPos-1).m_szContent;

	int i=SymbolTbl.SearchLabelInfoTbl(SymbolTbl.ProcStack.top(),szTmp);

	if (i!=-1)
	{
		OpInfo Tmp;
		Tmp.m_iType=OpInfo::LABEL;
		Tmp.m_iLink=i;
		SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.push_back(EmitIR(OpType::JMP,Tmp));
		SymbolTbl.LabelInfoTbl[i].m_bUse=true;
		return true;
	}
	else
	{
		EmitError("标号未声明",TokenList.at(iListPos-1));
		return false;
	}

	return true;
}

//标识符起始语句	-> 变量１ 053 := 表达式 064
bool semantic064()
{
	if (Operand.size()<2)
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	OpInfo Op1=Operand.top();
	Operand.pop();

	OpInfo Rslt=Operand.top();
	Operand.pop();

	if (CType::TypeCompatible(Op1,Rslt,17)!=-1)
	{
		if (!GenIR(Op1,Rslt,BasicOpType::MOV))
			return false;
	}
	else
	{
		EmitError("操作数类型不匹配",TokenList.at(iListPos-1));
		return false;
	}

	return true;
}

/*
 实参列表		-> 表达式 065 实参列表１
				-> ε
 实参列表１		-> , 表达式 065 实参列表１
				-> ε
*/
bool semantic065()
{
	if (Operand.empty() || CurrentProcCall.empty())
	{
		EmitError("语义错误:缺少实参表达式",TokenList.at(iListPos-1));
		return false;
	}

	CurrentProcCall.top().m_Paras.push_back(Operand.top());
	Operand.pop();

	return true;
}

/*
 过程调用语句	-> ( 101 实参列表 ) 066
				-> ε 005
*/
bool semantic066()
{

	ProcCall Tmp=CurrentProcCall.top();
	CurrentProcCall.pop();

	if (Tmp.m_iProcId==-1)		//函数为sizeof
	{
		if (Tmp.m_Paras.size()==1 && Tmp.m_Paras.at(0).m_iType!=OpInfo::CONST)
		{
			OpInfo Rslt;

			Rslt.m_iType=OpInfo::CONST;

			if (!Tmp.m_Paras.at(0).m_iDetailType.empty())
			{
				Rslt.m_iLink=SymbolTbl.RecConstTbl(itos(
						SymbolTbl.TypeInfoTbl[Tmp.m_Paras.at(0).m_iDetailType.top().m_iLink].m_iSize),3);
			}
			else
			{
				Rslt.m_iLink=SymbolTbl.RecConstTbl(itos(
					SymbolTbl.TypeInfoTbl[CType::GetRealType(
							SymbolTbl.VarInfoTbl[Tmp.m_Paras.at(0).m_iLink].m_iTypeLink)].m_iSize),3);
			}

			Operand.push(Rslt);

			return true;
		}
		else
		{
			EmitError("sizeof函数只能存在一个变量实参",TokenList.at(iListPos-1));
			return false;
		}
	}

	if (Tmp.m_iProcId==0)
	{
		EmitError("语义错误:不允许调用主程序",TokenList.at(iListPos-1));
		return false;
	}

	if (SymbolTbl.ProcInfoTbl.at(Tmp.m_iProcId).m_ParaTbl.size()!=Tmp.m_Paras.size())
	{
		EmitError("语义错误:实参与形参个数不匹配",TokenList.at(iListPos-1));
		return false;
	}

	OpInfo Op;

	for (int i=SymbolTbl.ProcInfoTbl.at(Tmp.m_iProcId).m_ParaTbl.size()-1;i>=0;i--)
	{
		OpInfo Op;
		Op.m_iType=OpInfo::VAR;
		Op.m_iLink=SymbolTbl.ProcInfoTbl.at(Tmp.m_iProcId).m_ParaTbl.at(i).m_iParaVar;

		if (CType::TypeCompatible(Tmp.m_Paras.at(i),Op,17)!=-1)
		{
			SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
						.m_Codes.push_back(EmitIR(OpType::PARA,Tmp.m_Paras.at(i),Op));
		}
		else
		{
			EmitError("实参与形参数据类型不匹配",TokenList.at(iListPos-1));
			return false;
		}
	}

	Op.m_iType=OpInfo::PROC;

	Op.m_iLink=Tmp.m_iProcId;

	if (Tmp.m_eCallType==ProcCall::Call)
	{
		SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.push_back(EmitIR(OpType::CALL,Op));
	}
	else
	{
		OpInfo Op=Operand.top();
		Operand.pop();
		SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.push_back(EmitIR(OpType::CALLPTR,Op));
	}

	if (SymbolTbl.ProcInfoTbl.at(Tmp.m_iProcId).m_iReturnType>=0)
	{
		Op.m_iType=OpInfo::VAR;
		Op.m_iLink=SymbolTbl.ProcInfoTbl.at(Tmp.m_iProcId).m_iReturnVar;
		OpInfo Rslt,TmpOp;
		TmpOp.m_iType=OpInfo::NONE;
		Rslt.m_iType=OpInfo::VAR;
		Rslt.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top());

		SymbolTbl.VarInfoTbl[Rslt.m_iLink].m_iTypeLink=SymbolTbl.VarInfoTbl[Op.m_iLink].m_iTypeLink;
		SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.push_back(EmitIR(OpType::RETV,TmpOp,TmpOp,Rslt));
		Var TmpVar;
		TmpVar.m_iDim=0;
		TmpVar.m_iVarLink=Rslt.m_iLink;
		TmpVar.m_VarTypeStack.push(VarType(SymbolTbl.TypeInfoTbl[CType::GetRealType(
						SymbolTbl.VarInfoTbl[Rslt.m_iLink].m_iTypeLink)].m_eDataType
						,CType::GetRealType(SymbolTbl.VarInfoTbl[Rslt.m_iLink].m_iTypeLink)));

		CurrentVar.push(TmpVar);
	}

	return true;
}

//函数头	-> function 标识符 042 形参列表 : 038 类型 039 039 067
bool semantic067()
{
	if (SymbolTbl.ProcInfoTbl.back().m_iReturnType==-1)
	{
		EmitError("函数返回类型只能是基本类型或用户自定义类型",TokenList.at(iListPos-1));
		return false;
	}
	else
	{
		VarInfo Tmp;
		Tmp.m_szName="RESULT";
		Tmp.m_iProcIndex=SymbolTbl.ProcInfoTbl.size()-1;
		Tmp.m_iTypeLink=SymbolTbl.ProcInfoTbl.back().m_iReturnType;
		Tmp.m_eRank=VarInfo::Rank::RET;
		SymbolTbl.AddVar(Tmp);
	}
	return true;
}

//其他语句	-> if 表达式 068 then 语句 if语句后部 070
bool semantic068()
{
	if (Operand.empty())
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	OpInfo Tmp=Operand.top();
	Operand.pop();

	if (CType::GetOpType(Tmp)!=StoreType::T_BOOLEAN)
	{
		EmitError("IF语句判定表达式必须为BOOLEAN类型",TokenList.at(iListPos-1));
		return false;
	}
	else
	{
		Statement TmpS;
		Statement::LabelIdx TmpLblIdx;
		OpInfo TmpLbl;
		TmpS.m_eType=Statement::IF;
		TmpS.m_bIsElse=false;

		TmpLbl.m_iType=OpInfo::LABEL;
		TmpLbl.m_iLink=SymbolTbl.GetTmpLabel(SymbolTbl.ProcStack.top());
		TmpLblIdx.m_Label=TmpLbl;
		TmpLblIdx.m_LabelType=Statement::LabelIdx::FalseLabel;
		TmpLblIdx.m_iIdx=TmpLbl.m_iLink;
		TmpS.m_Labels.push_back(TmpLblIdx);

		TmpLbl.m_iType=OpInfo::LABEL;
		TmpLbl.m_iLink=SymbolTbl.GetTmpLabel(SymbolTbl.ProcStack.top());
		TmpLblIdx.m_Label=TmpLbl;
		TmpLblIdx.m_LabelType=Statement::LabelIdx::ExitLabel;
		TmpLblIdx.m_iIdx=TmpLbl.m_iLink;
		TmpS.m_Labels.push_back(TmpLblIdx);

		CurrentStatement.push(TmpS);

		SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
					.m_Codes.push_back(EmitIR(OpType::JNT,Tmp,TmpS.GetLabel(Statement::LabelIdx::FalseLabel)));
	}

	return true;
}

/*
 if语句后部		-> else 069 语句
 				-> ε
*/
bool semantic069()
{
	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::IF)
	{
		EmitError("ELSE子句必须与IF配合使用",TokenList.at(iListPos-1));
		return false;
	}

	Statement TmpS;
	CurrentStatement.top().m_bIsElse=true;
	TmpS=CurrentStatement.top();

	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
					.m_Codes.push_back(EmitIR(OpType::JMP,TmpS.GetLabel(Statement::LabelIdx::ExitLabel)));
	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
					.m_Codes.push_back(EmitIR(OpType::LABEL,TmpS.GetLabel(Statement::LabelIdx::FalseLabel)));

	return true;
}

//其他语句		-> if 表达式 068 then 语句 if语句后部 070
bool semantic070()
{
	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::IF)
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	Statement TmpS;
	TmpS=CurrentStatement.top();

	if (TmpS.m_bIsElse)
	{
		SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
					.m_Codes.push_back(EmitIR(OpType::LABEL,TmpS.GetLabel(Statement::LabelIdx::ExitLabel)));
	}
	else
	{
		SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
					.m_Codes.push_back(EmitIR(OpType::LABEL,TmpS.GetLabel(Statement::LabelIdx::FalseLabel)));
	}

	CurrentStatement.pop();
	return true;
}

//其他语句		-> while 071 表达式 073 do 语句 072
bool semantic071()
{
	Statement TmpS;
	Statement::LabelIdx TmpLblIdx;
	OpInfo TmpLbl;
	TmpS.m_eType=Statement::WHILE;

	TmpLbl.m_iType=OpInfo::LABEL;
	TmpLbl.m_iLink=SymbolTbl.GetTmpLabel(SymbolTbl.ProcStack.top());
	TmpLblIdx.m_Label=TmpLbl;
	TmpLblIdx.m_LabelType=Statement::LabelIdx::ExitLabel;
	TmpLblIdx.m_iIdx=TmpLbl.m_iLink;
	TmpS.m_Labels.push_back(TmpLblIdx);

	TmpLbl.m_iType=OpInfo::LABEL;
	TmpLbl.m_iLink=SymbolTbl.GetTmpLabel(SymbolTbl.ProcStack.top());
	TmpLblIdx.m_Label=TmpLbl;
	TmpLblIdx.m_LabelType=Statement::LabelIdx::EntryLabel;
	TmpLblIdx.m_iIdx=TmpLbl.m_iLink;
	TmpS.m_Labels.push_back(TmpLblIdx);

	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
					.m_Codes.push_back(EmitIR(OpType::LABEL,TmpS.GetLabel(Statement::LabelIdx::EntryLabel)));

	CurrentStatement.push(TmpS);
	return true;
}

//其他语句		-> while 071 表达式 073 do 语句 072
bool semantic072()
{
	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::WHILE)
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	Statement TmpS;
	TmpS=CurrentStatement.top();

	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
					.m_Codes.push_back(EmitIR(OpType::JMP,TmpS.GetLabel(Statement::LabelIdx::EntryLabel)));

	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
					.m_Codes.push_back(EmitIR(OpType::LABEL,TmpS.GetLabel(Statement::LabelIdx::ExitLabel)));

	CurrentStatement.pop();
	return true;
}

//其他语句		-> while 071 表达式 073 do 语句 072
bool semantic073()
{
	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::WHILE)
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	if (Operand.empty())
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	OpInfo Tmp=Operand.top();
	Operand.pop();

	if (CType::GetOpType(Tmp)!=StoreType::T_BOOLEAN)
	{
		EmitError("WHILE语句判定表达式必须为BOOLEAN类型",TokenList.at(iListPos-1));
		return false;
	}
	else
	{
		Statement TmpS;
		TmpS=CurrentStatement.top();
		SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
					.m_Codes.push_back(EmitIR(OpType::JNT,Tmp,TmpS.GetLabel(Statement::LabelIdx::ExitLabel)));
	}

	return true;
}

//其他语句		-> repeat 074 语句序列 until 表达式 075
bool semantic074()
{
	Statement TmpS;
	Statement::LabelIdx TmpLblIdx;
	OpInfo TmpLbl;
	TmpS.m_eType=Statement::REPEAT;

	TmpLbl.m_iType=OpInfo::LABEL;
	TmpLbl.m_iLink=SymbolTbl.GetTmpLabel(SymbolTbl.ProcStack.top());
	TmpLblIdx.m_Label=TmpLbl;
	TmpLblIdx.m_LabelType=Statement::LabelIdx::ExitLabel;
	TmpLblIdx.m_iIdx=TmpLbl.m_iLink;
	TmpS.m_Labels.push_back(TmpLblIdx);

	TmpLbl.m_iType=OpInfo::LABEL;
	TmpLbl.m_iLink=SymbolTbl.GetTmpLabel(SymbolTbl.ProcStack.top());
	TmpLblIdx.m_Label=TmpLbl;
	TmpLblIdx.m_LabelType=Statement::LabelIdx::EntryLabel;
	TmpLblIdx.m_iIdx=TmpLbl.m_iLink;
	TmpS.m_Labels.push_back(TmpLblIdx);

	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
					.m_Codes.push_back(EmitIR(OpType::LABEL,TmpS.GetLabel(Statement::LabelIdx::EntryLabel)));

	CurrentStatement.push(TmpS);

	return true;
}

//其他语句		-> repeat 074 语句序列 until 表达式 075
bool semantic075()
{

	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::REPEAT)
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	if (Operand.empty())
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	OpInfo Tmp=Operand.top();
	Operand.pop();

	if (CType::GetOpType(Tmp)!=StoreType::T_BOOLEAN)
	{
		EmitError("WHILE语句判定表达式必须为BOOLEAN类型",TokenList.at(iListPos-1));
		return false;
	}
	else
	{
		Statement TmpS;
		TmpS=CurrentStatement.top();

		SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
					.m_Codes.push_back(EmitIR(OpType::JNT,Tmp,TmpS.GetLabel(Statement::LabelIdx::EntryLabel)));

		SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
					.m_Codes.push_back(EmitIR(OpType::LABEL,TmpS.GetLabel(Statement::LabelIdx::ExitLabel)));
	}

	CurrentStatement.pop();
	return true;
}

//其他语句		-> for 标识符 076 := 表达式 077 for语句后部
bool semantic076()
{
	string szTmp=TokenList.at(iListPos-1).m_szContent;
	int i=SymbolTbl.SearchVarInfoTbl(SymbolTbl.ProcStack.top(),szTmp);

	if (i==-1)
	{
		i=SymbolTbl.SearchVarInfoTbl(0,szTmp);
		if (i==-1)
		{
			EmitError("循环变量未定义",TokenList.at(iListPos-1));
			return false;
		}
	}
	Statement TmpS;
	Statement::LabelIdx TmpLblIdx;
	OpInfo TmpLbl;
	TmpS.m_eType=Statement::FOR;
	TmpS.m_iLoopVar=i;

	TmpLbl.m_iType=OpInfo::LABEL;
	TmpLbl.m_iLink=SymbolTbl.GetTmpLabel(SymbolTbl.ProcStack.top());
	TmpLblIdx.m_Label=TmpLbl;
	TmpLblIdx.m_LabelType=Statement::LabelIdx::ExitLabel;
	TmpLblIdx.m_iIdx=TmpLbl.m_iLink;
	TmpS.m_Labels.push_back(TmpLblIdx);

	TmpLbl.m_iType=OpInfo::LABEL;
	TmpLbl.m_iLink=SymbolTbl.GetTmpLabel(SymbolTbl.ProcStack.top());
	TmpLblIdx.m_Label=TmpLbl;
	TmpLblIdx.m_LabelType=Statement::LabelIdx::EntryLabel;
	TmpLblIdx.m_iIdx=TmpLbl.m_iLink;
	TmpS.m_Labels.push_back(TmpLblIdx);

	TmpLbl.m_iType=OpInfo::LABEL;
	TmpLbl.m_iLink=SymbolTbl.GetTmpLabel(SymbolTbl.ProcStack.top());
	TmpLblIdx.m_Label=TmpLbl;
	TmpLblIdx.m_LabelType=Statement::LabelIdx::TrueLabel;
	TmpLblIdx.m_iIdx=TmpLbl.m_iLink;
	TmpS.m_Labels.push_back(TmpLblIdx);

	CurrentStatement.push(TmpS);

	return true;
}

//其他语句		-> for 标识符 076 := 表达式 077 for语句后部
bool semantic077()
{
	if (Operand.empty()
		|| CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::FOR)
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	OpInfo Op1,Rslt;
	Op1=Operand.top();
	Operand.pop();
	Rslt.m_iType=OpInfo::VAR;
	Rslt.m_iLink=CurrentStatement.top().m_iLoopVar;

	if (!CType::IsOrd(Op1)
		|| CType::TypeCompatible(Op1,Rslt,17)==-1)
	{
		EmitError("循环变量的初值不为有序类型或者类型不兼容",TokenList.at(iListPos-1));
		return false;
	}

	if (!GenIR(Op1,Rslt,BasicOpType::MOV))
	{
		return false;
	}

	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
				.m_Codes.push_back(EmitIR(OpType::LABEL,CurrentStatement.top().GetLabel(Statement::LabelIdx::EntryLabel)));

	return true;
}

//for语句后部		-> to 表达式 078 do 语句 079
bool semantic078()
{
	if (Operand.empty()
		|| CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::FOR)
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	OpInfo Op2,Op1;
	Op2=Operand.top();
	Operand.pop();
	Op1.m_iType=OpInfo::VAR;
	Op1.m_iLink=CurrentStatement.top().m_iLoopVar;
	CurrentStatement.top().m_bIsDownto=false;
	int TmpResult=CType::TypeCompatible(Op1,Op2,20);

	if (!CType::IsOrd(Op2) || TmpResult==-1)
	{
		EmitError("循环变量的终值不为有序类型或者类型不兼容",TokenList.at(iListPos-1));
		return false;
	}

	OpInfo TmpRslt;
	TmpRslt.m_iType=OpInfo::VAR;
	TmpRslt.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top(),(StoreType)SymbolTbl.TypeSysTbl.at(TmpResult).m_iRsltType);
	OpVarSemantic(Op1,Op2,TmpRslt,SymbolTbl.TypeSysTbl.at(TmpResult));

	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
				.m_Codes.push_back(EmitIR(OpType::JNT,TmpRslt,CurrentStatement.top().GetLabel(Statement::LabelIdx::ExitLabel)));
	return true;
}

//for语句后部		-> to 表达式 078 do 语句 079
bool semantic079()
{
	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::FOR)
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	OpInfo Op1,Op2;
	Op1.m_iType=OpInfo::VAR;
	Op1.m_iLink=CurrentStatement.top().m_iLoopVar;
	Op2.m_iType=OpInfo::CONST;
	Op2.m_iLink=SymbolTbl.RecConstTbl("1",3);

	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
			.m_Codes.push_back(EmitIR(OpType::LABEL,CurrentStatement.top().GetLabel(Statement::LabelIdx::TrueLabel)));

	if (CurrentStatement.top().m_bIsDownto)
	{
		if (!GenIR(Op1,Op2,Op1,BasicOpType::SUB))
		{
			return false;
		}
	}
	else
	{
		if (!GenIR(Op1,Op2,Op1,BasicOpType::ADD))
		{
			return false;
		}
	}

	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
			.m_Codes.push_back(EmitIR(OpType::JMP,CurrentStatement.top().GetLabel(Statement::LabelIdx::EntryLabel)));

	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
			.m_Codes.push_back(EmitIR(OpType::LABEL,CurrentStatement.top().GetLabel(Statement::LabelIdx::ExitLabel)));

	CurrentStatement.pop();

	return true;
}

//for语句后部		-> downto 表达式 080 do 语句 079
bool semantic080()
{

	if (Operand.empty()
		|| CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::FOR)
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	OpInfo Op2,Op1;
	Op2=Operand.top();
	Operand.pop();
	Op1.m_iType=OpInfo::VAR;
	Op1.m_iLink=CurrentStatement.top().m_iLoopVar;
	CurrentStatement.top().m_bIsDownto=true;
	int TmpResult=CType::TypeCompatible(Op1,Op2,22);

	if (!CType::IsOrd(Op2) || TmpResult==-1)
	{
		EmitError("循环变量的终值不为有序类型或者类型不兼容",TokenList.at(iListPos-1));
		return false;
	}

	OpInfo TmpRslt;
	TmpRslt.m_iType=OpInfo::VAR;
	TmpRslt.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top(),(StoreType)SymbolTbl.TypeSysTbl.at(TmpResult).m_iRsltType);
	OpVarSemantic(Op1,Op2,TmpRslt,SymbolTbl.TypeSysTbl.at(TmpResult));

	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
			.m_Codes.push_back(EmitIR(OpType::JNT,TmpRslt,CurrentStatement.top().GetLabel(Statement::LabelIdx::ExitLabel)));

	return true;
}

//其他语句		-> break 081
bool semantic081()
{
	Statement* pState;
	int i=CurrentStatement.size()-1;
	while (i>=0)
	{
		pState=&CurrentStatement.c.at(i);

		if (pState->m_eType==Statement::FOR
			|| pState->m_eType==Statement::WHILE
			|| pState->m_eType==Statement::REPEAT)
		{
			break;
		}

		i--;
	}

	if (i<0)
	{
		EmitError("BREAK语句只能出现在FOR、WHILE、REPEAT语句块内",TokenList.at(iListPos-1));
		return false;
	}

	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
				.m_Codes.push_back(EmitIR(OpType::JMP,pState->GetLabel(Statement::LabelIdx::ExitLabel)));
	return true;
}

//其他语句		-> continue 082
bool semantic082()
{
	Statement* pState;
	int i=CurrentStatement.size()-1;

	while (i>=0)
	{
		pState=&CurrentStatement.c.at(i);

		if (pState->m_eType==Statement::FOR
			|| pState->m_eType==Statement::WHILE
			|| pState->m_eType==Statement::REPEAT)
		{
			break;
		}

		i--;
	}

	if (i<0)
	{
		EmitError("CONTINUE语句只能出现在FOR、WHILE、REPEAT语句块内",TokenList.at(iListPos-1));
		return false;
	}

	if (pState->m_eType == Statement::FOR)
	{
		SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
				.m_Codes.push_back(EmitIR(OpType::JMP,pState->GetLabel(Statement::LabelIdx::TrueLabel)));
	}
	else
	{
		SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
				.m_Codes.push_back(EmitIR(OpType::JMP,pState->GetLabel(Statement::LabelIdx::EntryLabel)));
	}

	return true;
}

//其他语句		-> asm 字符串常量 , 字符串常量 083 end
bool semantic083()
{
	string szPara=SymbolTbl.ConstInfoTbl[atoi(TokenList.at(iListPos-1).m_szContent.c_str())].m_szName;
	string szAsm=SymbolTbl.ConstInfoTbl[atoi(TokenList.at(iListPos-3).m_szContent.c_str())].m_szName;
	string szTmp="";
	int iAsmParaLink=SymbolTbl.AsmParaTbl.size();
	AsmPara TmpAsmPara;

	for(int i=1;i<szPara.length()-1;i++)
	{
		if (szPara[i]=='@' || szPara[i]==';')
		{
			szTmp=trim(szTmp);

			if (szTmp.empty())
			{
				EmitError("内嵌汇编语句的参数信息不正确",TokenList.at(iListPos-1));
				return false;
			}

			if (szPara[i]=='@')
			{
				TmpAsmPara.flag=AsmPara::N;

				if (szTmp[0]=='w')
					TmpAsmPara.flag=AsmPara::W;
				if (szTmp[0]=='a')
					TmpAsmPara.flag=AsmPara::RW;
				if (szTmp[0]=='r')
					TmpAsmPara.flag=AsmPara::R;

				if (TmpAsmPara.flag==AsmPara::N)
				{
					EmitError("内嵌汇编语句的参数读写属性不正确",TokenList.at(iListPos-1));
					return false;
				}
				szTmp="";
			}

			if (szPara[i]==';')
			{
				szTmp=UpperCase(trim(szTmp));
				TmpAsmPara.m_iLink=SymbolTbl.SearchVarInfoTbl(SymbolTbl.ProcStack.top(),szTmp);
				if (TmpAsmPara.m_iLink==-1)
				{
					TmpAsmPara.m_iLink=SymbolTbl.SearchVarInfoTbl(0,szTmp);
					if (TmpAsmPara.m_iLink==-1)
					{
						EmitError("变量"+szTmp+"不存在",TokenList.at(iListPos-1));
						return false;
					}
				}
				TmpAsmPara.m_szName=szTmp;
				SymbolTbl.AsmParaTbl.push_back(TmpAsmPara);
				szTmp="";
			}
		}
		else
		{
			szTmp=szTmp+szPara[i];
		}
	}

	for (int i=0;i<szAsm.length();i++)
	{
		if (szAsm[i]=='%')
		{
			i++;
			string szNum;

			while (szAsm[i]<='9' && szAsm[i]>='0')
			{
				szNum=szNum+szAsm[i++];
			}

			if (szNum.empty()
				|| (iAsmParaLink+atoi(szNum.c_str())>SymbolTbl.AsmParaTbl.size()-1))
			{
				EmitError("内嵌汇编参数引用不正确",TokenList.at(iListPos-1));
				return false;
			}
		}
	}

	OpInfo Op1,Op2,Op3;
	Op1.m_iType=OpInfo::CONST;
	Op1.m_iLink=atoi(TokenList.at(iListPos-3).m_szContent.c_str());
	Op2.m_iType=OpInfo::NONE;
	Op2.m_iLink=iAsmParaLink;
	Op3.m_iType=OpInfo::NONE;
	Op3.m_iLink=SymbolTbl.AsmParaTbl.size()-1;
	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.push_back(EmitIR(OpType::ASM,Op1,Op2,Op3));

	return true;
}

//其他语句		-> case 表达式 084 of case分支 end 089
bool semantic084()
{
	if (Operand.empty()
		|| !CType::IsOrd(Operand.top()))
	{
		EmitError("CASE的选择子必须为有序类型",TokenList.at(iListPos-1));
		return false;
	}

	OpInfo Tmp,TmpLbl;
	Tmp=Operand.top();
	Operand.pop();
	Statement TmpS;
	TmpS.m_eType=Statement::CASE;
	TmpS.m_CaseExp=Tmp;
	TmpS.m_iLoopVar=SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.size();

	Statement::LabelIdx TmpLblIdx;

	TmpLbl.m_iType=OpInfo::LABEL;
	TmpLbl.m_iLink=SymbolTbl.GetTmpLabel(SymbolTbl.ProcStack.top());
	TmpLblIdx.m_Label=TmpLbl;
	TmpLblIdx.m_LabelType=Statement::LabelIdx::ExitLabel;
	TmpLblIdx.m_iIdx=TmpLbl.m_iLink;
	TmpS.m_Labels.push_back(TmpLblIdx);

	CurrentStatement.push(TmpS);

	return true;
}

//常量列表		-> 常量 085 常量列表１
bool semantic085()
{
	OpInfo Tmp;
	Tmp.m_iType=OpInfo::CONST;
	Tmp.m_iLink=atoi(TokenList.at(iListPos-1).m_szContent.c_str());

	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::CASE)
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	if (CType::TypeCompatible(Tmp,CurrentStatement.top().m_CaseExp,17)==-1)
	{
		EmitError("case子句的常量与选择子的类型不匹配",TokenList.at(iListPos-1));
		return false;
	}

	int iConst1=SymbolTbl.ConstInfoTbl[Tmp.m_iLink].m_iVal;
	int iConst2;

	for (int i=0;i<CurrentStatement.top().m_Labels.size();i++)
	{
		if (CurrentStatement.top().m_Labels.at(i).m_LabelType!=Statement::LabelIdx::CaseLabel)
		{
			continue;
		}

		iConst2=SymbolTbl.ConstInfoTbl[CurrentStatement.top().m_Labels.at(i).m_iConst].m_iVal;

		if (iConst1==iConst2)
		{
			EmitError("case分支常量重复定义",TokenList.at(iListPos-1));
			return false;
		}
	}

	OpInfo TmpLbl;
	Statement::LabelIdx TmpLblIdx;
	TmpLbl.m_iType=OpInfo::LABEL;
	TmpLbl.m_iLink=SymbolTbl.GetTmpLabel(SymbolTbl.ProcStack.top());
	TmpLblIdx.m_Label=TmpLbl;
	TmpLblIdx.m_LabelType=Statement::LabelIdx::CaseLabel;
	TmpLblIdx.m_iIdx=TmpLbl.m_iLink;
	TmpLblIdx.m_iConst=atoi(TokenList.at(iListPos-1).m_szContent.c_str());
	CurrentStatement.top().m_Labels.push_back(TmpLblIdx);

	return true;
}

/*
 常量列表１		-> , 常量 086 常量列表１
				-> ε
*/
bool semantic086()
{
	OpInfo Tmp;
	Tmp.m_iType=OpInfo::CONST;
	Tmp.m_iLink=atoi(TokenList.at(iListPos-1).m_szContent.c_str());

	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::CASE)
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	if (CType::TypeCompatible(Tmp,CurrentStatement.top().m_CaseExp,17)==-1)
	{
		EmitError("case子句的常量与选择子的类型不匹配",TokenList.at(iListPos-1));
		return false;
	}

	int iConst1=SymbolTbl.ConstInfoTbl[Tmp.m_iLink].m_iVal;
	int iConst2;

	for (int i=0;i<CurrentStatement.top().m_Labels.size();i++)
	{
		if (CurrentStatement.top().m_Labels.at(i).m_LabelType!=Statement::LabelIdx::CaseLabel)
		{
			continue;
		}

		iConst2=SymbolTbl.ConstInfoTbl[CurrentStatement.top().m_Labels.at(i).m_iConst].m_iVal;

		if (iConst1==iConst2)
		{
			EmitError("case分支常量重复定义",TokenList.at(iListPos-1));
			return false;
		}
	}

	Statement::LabelIdx TmpLblIdx;
	TmpLblIdx.m_Label=CurrentStatement.top().m_Labels.at(CurrentStatement.top().m_Labels.size()-1).m_Label;
	TmpLblIdx.m_LabelType=Statement::LabelIdx::CaseLabel;
	TmpLblIdx.m_iIdx=CurrentStatement.top().m_Labels.at(CurrentStatement.top().m_Labels.size()-1).m_iIdx;
	TmpLblIdx.m_iConst=atoi(TokenList.at(iListPos-1).m_szContent.c_str());
	CurrentStatement.top().m_Labels.push_back(TmpLblIdx);

	return true;
}

/*
 case分支	-> 常量列表 087 : 语句 088 ; case分支
			-> ε
*/
bool semantic087()
{
	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::CASE)
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.push_back(
				EmitIR(OpType::LABEL,CurrentStatement.top().m_Labels.back().m_Label));

	return true;
}

/*
 case分支	-> 常量列表 087 : 语句 088 ; case分支
			-> ε
*/
bool semantic088()
{
	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::CASE)
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.push_back(
				EmitIR(OpType::JMP,CurrentStatement.top().GetLabel(Statement::LabelIdx::ExitLabel)));

	return true;

}


//其他语句	-> case 表达式 084 of case分支 end 089
bool semantic089()
{
	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::CASE)
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	OpInfo Op1,Op2;
	Op1=CurrentStatement.top().m_CaseExp;
	int iOpSize=0;

	if (!Op1.m_iDetailType.empty())
	{
		iOpSize=SymbolTbl.TypeInfoTbl[Op1.m_iDetailType.top().m_iLink].m_iSize;
	}
	else
	{
		iOpSize=SymbolTbl.TypeInfoTbl[CType::GetRealType(SymbolTbl.VarInfoTbl[Op1.m_iLink].m_iTypeLink)].m_iSize;
	}

	if (iOpSize>4)
	{
		EmitError("CASE语句的选择子必须为有序类型",TokenList.at(iListPos-1));
		return false;
	}

	Op2.m_iType=OpInfo::CONST;
	vector<IRCode> TmpIRList;

	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.push_back(
				EmitIR(OpType::LABEL,CurrentStatement.top().GetLabel(Statement::LabelIdx::ExitLabel)));

	for (int i=CurrentStatement.top().m_Labels.size()-1;i>0;i--)
	{
		Op2.m_iLink=CurrentStatement.top().m_Labels.at(i).m_iConst;
		TmpIRList.push_back(
				EmitIR(OpType(OpType::JE_1+iOpSize/2),Op1,Op2,CurrentStatement.top().m_Labels.at(i).m_Label));
	}

	TmpIRList.push_back(
				EmitIR(OpType::JMP,CurrentStatement.top().GetLabel(Statement::LabelIdx::ExitLabel)));

	vector<IRCode>::iterator it=SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.begin();
	it=it+CurrentStatement.top().m_iLoopVar;
	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.insert(it,TmpIRList.begin(),TmpIRList.end());
	CurrentStatement.pop();

	return true;
}

//其他语句	-> with 090 变量 053 091 do 语句 092
bool semantic090()
{
	bWithFlag=true;
	return true;
}

//其他语句	-> with 090 变量 053 091 do 语句 092
bool semantic091()
{
	bWithFlag=false;

	if (TmpWithField.m_Var.m_VarTypeStack.empty()
		|| TmpWithField.m_Var.m_VarTypeStack.top().m_StoreType!=StoreType::T_RECORD)
	{
		EmitError("With语句只允许开域变量为记录类型",TokenList.at(iListPos-1));
		return false;
	}

	while (SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.size()>TmpWithField.m_iRestoreIR)
	{
		SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.pop_back();
	}

	Operand.pop();
	WithStack.push(TmpWithField);

	return true;
}

//其他语句	-> with 090 变量 053 091 do 语句 092
bool semantic092()
{
	if (WithStack.empty())
	{
		EmitError("语义出错",TokenList.at(iListPos-1));
		return false;
	}
	WithStack.pop();

	return true;
}

//包含文件说明	-> uses 093 标识符列表 094 ;
bool semantic093()
{
	iIdListFlag.push(7);
	return true;
}

//包含文件说明	-> uses 093 标识符列表 094 ;
bool semantic094()
{
	iIdListFlag.pop();
	return true;
}

//声明部分４	-> 095 过程函数声明部分
bool semantic095()
{
	for(int i=0;i<SymbolTbl.UseFileTbl.size();i++)
	{
		if (SymbolTbl.UseFileTbl.at(i).m_bFlag)
			continue;
		SymbolTbl.UseFileTbl.at(i).m_bFlag=true;
		string szFileName=State.m_szUserLib+SymbolTbl.UseFileTbl.at(i).m_szFileName+ _SOURCE_EXT;
		vector<CToken> TmpTokenList;
		FILE *stream  = fopen(szFileName.c_str(), "r" );
		if (stream==NULL)
		{
			szFileName=State.m_szSysLib+SymbolTbl.UseFileTbl.at(i).m_szFileName+ _SOURCE_EXT;
			stream  = fopen(szFileName.c_str(), "r" );
		}
		if (stream!=NULL)
		{
			TmpTokenList.clear();
			fclose(stream);
			Lex.SetFileName(szFileName.c_str());
			if (!Lex.GenToken(&TmpTokenList))
				return false;
			vector<CToken>::iterator it=TokenList.begin();
			it=it+iListPos;
			vector<CToken>::iterator it1,it2;
			it1=TmpTokenList.begin();
			it2=TmpTokenList.end();
			TokenList.insert(it,it1,it2);
		}
		else
		{
			EmitError(szFileName.append("文件不存在").c_str(),TokenList.at(iListPos-1));
			return false;
		}
	}
	return true;
}

//因子			-> [ 096 表达式列表 097 ]
bool semantic096()
{
	OpInfo Tmp;
	iExpListFlag.push(2);	//处理集合
	iExpListNum.push(0);
	return true;
}

//因子			-> [ 096 表达式列表 097 ]
bool semantic097()
{
	iExpListFlag.pop();
	stack<OpInfo> TmpConstItem,TmpVarItem;

	if (Operand.size()<iExpListNum.top())
	{
		EmitError("语义错误",TokenList.at(iListPos-1));
		return false;
	}

	while (iExpListNum.top()--!=0)
	{
		if (Operand.top().m_iType==OpInfo::CONST)
		{
			TmpConstItem.push(Operand.top());
		}
		else
		{
			TmpVarItem.push(Operand.top());
		}

		Operand.pop();
	}

	OpInfo Op1,Op2;
	Op1.m_iType=OpInfo::CONST;
	Op1.m_iLink=SymbolTbl.RecConstSetTbl(GenSetStr(256,"0"));
	string Tmp=SymbolTbl.ConstInfoTbl[Op1.m_iLink].m_szSet;

	while (!TmpConstItem.empty())
	{
		Op2=TmpConstItem.top();
		TmpConstItem.pop();

		if (CType::IsSetItem(Op2))
		{
			if (SymbolTbl.ConstInfoTbl[Op2.m_iLink].m_ConstType==ConstType::INTEGER)
			{
				Tmp=SetAddItem(Tmp,SymbolTbl.ConstInfoTbl[Op2.m_iLink].m_iVal);
			}
			else
			{
				Tmp=SetAddItem(Tmp,SymbolTbl.ConstInfoTbl[Op2.m_iLink].m_szVal[0]);
			}
		}
		else
		{
			EmitError("集合元素不允许为非有序类型数据，并且集合元素值必须介于0..255",TokenList.at(iListPos-1));
			return false;
		}
	}

	if (Tmp.compare(SymbolTbl.ConstInfoTbl[Op1.m_iLink].m_szVal)!=0)
	{
		Op1.m_iLink=SymbolTbl.RecConstSetTbl(Tmp);
	}

	Operand.push(Op1);

	while (!TmpVarItem.empty())
	{
		OpInfo Rslt,Op1,Op2;
		Op1=Operand.top();
		Operand.pop();
		Op2=TmpVarItem.top();
		TmpVarItem.pop();

		if (CType::IsOrd(Op2))
		{
			Rslt.m_iType=OpInfo::VAR;
			Rslt.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top(),StoreType::T_SET);
			SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes
					.push_back(EmitIR(OpType::SET_ADDITEM,Op1,Op2,Rslt));
			Operand.push(Rslt);
		}
		else
		{
			EmitError("集合元素不允许为非有序类型数据，并且集合元素值必须介于0..255",TokenList.at(iListPos-1));
			return false;
		}
	}

	iExpListNum.pop();
	return true;
}

//变量１	-> [ 098 表达式列表 099 ] 059 变量１
bool semantic098()
{
	iExpListFlag.push(1);	//处理数组下标访问
	return true;
}

//变量１	-> [ 098 表达式列表 099 ] 059 变量１
bool semantic099()
{
	iExpListFlag.pop();
	return true;
}

//因子		-> nil 100
bool semantic100()
{
	ConstInfo Tmp;
	Tmp.m_szName="NIL";
	Tmp.m_ConstType=ConstType::PTR;
	Tmp.m_iProcIndex=SymbolTbl.ProcStack.top();
	Tmp.m_iVal=0;
	Tmp.m_StoreType=StoreType::T_POINTER;
	SymbolTbl.AddConst(Tmp);
	OpInfo Op;
	Op.m_iType=OpInfo::CONST;
	Op.m_iLink=SymbolTbl.ConstInfoTbl.size()-1;
	Operand.push(Op);
	return true;
}

//过程调用语句	-> ( 101 实参列表 ) 066
bool semantic101()
{
	if (CurrentVar.empty())
		return true;
	if (CurrentVar.top().m_VarTypeStack.top().m_StoreType==StoreType::T_FUNC ||
		CurrentVar.top().m_VarTypeStack.top().m_StoreType==StoreType::T_PROC)
	{
		Var Tmp=CurrentVar.top();
		semantic053();
		return CallPtrProcess(Tmp);
	}
	return true;
}

//常量定义		-> 标识符 102 = 表达式 004
bool semantic102()
{
	return true;
}

bool GenIR(OpInfo Op1,OpInfo Op2,OpInfo Rslt,BasicOpType Op)
{
	int iTmp=CType::TypeCompatible(Op1,Op2,Op);
	if (iTmp==-1)
	{
		EmitError("操作数类型不匹配",TokenList.at(iListPos-1));
		return false;
	}
	OpVarSemantic(Op1,Op2,Rslt,SymbolTbl.TypeSysTbl.at(iTmp));
	return true;
}

bool GenIR(OpInfo Op1,OpInfo Rslt,BasicOpType Op)
{
	int iTmp=CType::TypeCompatible(Op1,Rslt,Op);
	if (iTmp==-1)
	{
		EmitError("操作数类型不匹配",TokenList.at(iListPos-1));
		return false;
	}
	return OpVarSemantic(Op1,Rslt,SymbolTbl.TypeSysTbl.at(iTmp));
}

bool OpConstSemantic(OpInfo Op1,int iProcessId,int iReturnType)
{
	OpInfo Tmp;
	Tmp.m_iType=OpInfo::NONE;
	return OpConstSemantic(Op1,Tmp,iProcessId,iReturnType);
}

bool OpVarSemantic(OpInfo Op1,OpInfo Rslt,TypeSysInfo Tmp)
{
	vector<IRCode> *P=&SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes;
	switch (Tmp.m_iVarProcessId)
	{
	case 1:
		{
			P->push_back(EmitIR(OpType(Tmp.m_iIR),Op1,Rslt));
		};break;
	case 2:
		{
			OpInfo TmpOp;
			TmpOp.m_iType=OpInfo::VAR;
			TmpOp.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top(),(OpType)Tmp.m_Convert1);
			P->push_back(EmitIR((OpType)Tmp.m_Convert1,Op1,TmpOp));
			P->push_back(EmitIR((OpType)Tmp.m_iIR,TmpOp,Rslt));
		};break;
	case 3:
		{
			int iOp1Type;
			int iRsltType;
			if (!Op1.m_iDetailType.empty())
				iOp1Type=Op1.m_iDetailType.top().m_iLink;
			else
				iOp1Type=SymbolTbl.VarInfoTbl[Op1.m_iLink].m_iTypeLink;
			if (!Rslt.m_iDetailType.empty())
				iRsltType=Rslt.m_iDetailType.top().m_iLink;
			else
				iRsltType=SymbolTbl.VarInfoTbl[Rslt.m_iLink].m_iTypeLink;

			if (CType::GetRealType(iOp1Type)==CType::GetRealType(iRsltType))
			{
				int iSize=CType::GetRealType(iOp1Type);
				if (iSize % 4!=0)
					iSize=(4-iSize)+iSize;
				OpInfo TmpOp2;
				TmpOp2.m_iType=OpInfo::CONST;
				TmpOp2.m_iLink=SymbolTbl.RecConstTbl(itos(iSize),3);
				P->push_back((EmitIR((OpType)Tmp.m_iIR,Op1,TmpOp2,Rslt)));
			}
			else
			{
				EmitError("记录或数组类型不匹配，类型不兼容",TokenList.at(iListPos-1));
				return false;
			}

		};break;
	}
	return true;
}

bool OpVarSemantic(OpInfo Op1,OpInfo Op2,OpInfo Rslt,TypeSysInfo Tmp)
{
	vector<IRCode> *P=&SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes;
	switch (Tmp.m_iVarProcessId)
	{
	case 1:
		{
			P->push_back(EmitIR(OpType(Tmp.m_iIR),Op1,Op2,Rslt));
		};break;
	case 2:
		{
			OpInfo TmpOp;
			TmpOp.m_iType=OpInfo::VAR;
			TmpOp.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top(),(OpType)Tmp.m_Convert1);
			P->push_back(EmitIR((OpType)Tmp.m_Convert1,Op1,TmpOp));
			P->push_back(EmitIR((OpType)Tmp.m_iIR,TmpOp,Op2,Rslt));
		};break;
	case 3:
		{
			OpInfo TmpOp;
			TmpOp.m_iType=OpInfo::VAR;
			TmpOp.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top(),(OpType)Tmp.m_Convert2);
			P->push_back(EmitIR((OpType)Tmp.m_Convert2,Op2,TmpOp));
			P->push_back(EmitIR((OpType)Tmp.m_iIR,Op1,TmpOp,Rslt));
		};break;
	case 4:
		{
			if (CType::GetOpTypeLink(Op1)==CType::GetOpTypeLink(Op2))
				P->push_back(EmitIR(OpType(Tmp.m_iIR),Op1,Op2,Rslt));
			else
			{
				EmitError("枚举类型不匹配",TokenList.at(iListPos-1));
				return false;
			}
		};break;
	case 5:
		{
			OpInfo TmpOp1,TmpOp2;
			TmpOp1.m_iType=OpInfo::VAR;
			TmpOp1.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top(),(OpType)Tmp.m_Convert1);
			TmpOp2.m_iType=OpInfo::VAR;
			TmpOp2.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top(),(OpType)Tmp.m_Convert2);
			P->push_back(EmitIR((OpType)Tmp.m_Convert1,Op1,TmpOp1));
			P->push_back(EmitIR((OpType)Tmp.m_Convert2,Op2,TmpOp2));
			P->push_back(EmitIR((OpType)Tmp.m_iIR,TmpOp1,TmpOp2,Rslt));
		};break;
	}
	return true;
}

bool OpConstFold(OpInfo Op1,OpInfo Op2,int iProcessId,int iReturnType,OpInfo &Rslt)
{
	ConstInfo RsltConst,Op1Const,Op2Const;
	Rslt.m_iType=OpInfo::CONST;
	RsltConst.m_StoreType=(StoreType)iReturnType;
	Op1Const=SymbolTbl.ConstInfoTbl[Op1.m_iLink];
	if (Op2.m_iType!=OpInfo::NONE)
		Op2Const=SymbolTbl.ConstInfoTbl[Op2.m_iLink];
	switch (iProcessId)
	{
	case 1:
		{
			RsltConst.m_bVal=Op1Const.m_szVal.compare(Op2Const.m_szVal)==0;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 2:
		{
			RsltConst.m_bVal=Op1Const.m_fVal==Op2Const.m_fVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 3:
		{
			RsltConst.m_bVal=Op1Const.m_szVal.compare(Op2Const.m_szVal)!=0;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 4:
		{
			RsltConst.m_bVal=Op1Const.m_fVal!=Op2Const.m_fVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 5:
		{
			RsltConst.m_bVal=Op1Const.m_szVal.compare(Op2Const.m_szVal)<0;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 6:
		{
			RsltConst.m_bVal=Op1Const.m_bVal<Op2Const.m_bVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 7:
		{
			RsltConst.m_bVal=Op1Const.m_fVal<Op2Const.m_fVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 8:
		{
			RsltConst.m_bVal=Op1Const.m_szVal.compare(Op2Const.m_szVal)>0;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 9:
		{
			RsltConst.m_bVal=Op1Const.m_bVal>Op2Const.m_bVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 10:
		{
			RsltConst.m_bVal=Op1Const.m_fVal>Op2Const.m_fVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 11:
		{
			RsltConst.m_bVal=Op1Const.m_szVal.compare(Op2Const.m_szVal)<=0;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		}
	case 12:
		{
			RsltConst.m_bVal=Op1Const.m_bVal<=Op2Const.m_bVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 13:
		{
			RsltConst.m_bVal=Op1Const.m_fVal<=Op2Const.m_fVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 14:
		{
			RsltConst.m_bVal=Op1Const.m_szVal.compare(Op2Const.m_szVal)>=0;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 15:
		{
			RsltConst.m_bVal=Op1Const.m_bVal>=Op2Const.m_bVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 16:
		{
			RsltConst.m_bVal=Op1Const.m_fVal>=Op2Const.m_fVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 17:
		{
			RsltConst.m_fVal=Op1Const.m_fVal-Op2Const.m_fVal;
			char cBuffer[20];
			gcvt(RsltConst.m_fVal,10,cBuffer);
			RsltConst.m_szName=cBuffer;
		};break;
	case 18:
		{
			RsltConst.m_szVal.append(Op1Const.m_szVal);
			RsltConst.m_szVal.append(SymbolTbl.ConstInfoTbl[Op2.m_iLink].m_szVal);
			RsltConst.m_szName=RsltConst.m_szVal;
		};break;
	case 19:
		{
			RsltConst.m_fVal=Op1Const.m_fVal+Op2Const.m_fVal;
			char cBuffer[50];
			gcvt(RsltConst.m_fVal,10,cBuffer);
			RsltConst.m_szName=cBuffer;
		};break;
	case 20:
		{
			RsltConst.m_bVal=Op1Const.m_bVal || Op2Const.m_bVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 21:
		{
			RsltConst.m_iVal=Op1Const.m_iVal | Op2Const.m_iVal;
			char cBuffer[50];
			RsltConst.m_fVal=RsltConst.m_iVal;
			itoa(RsltConst.m_iVal,cBuffer,10);
			RsltConst.m_szName=cBuffer;
		};break;
	case 22:
		{
			RsltConst.m_bVal=Op1Const.m_bVal ^ Op2Const.m_bVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 23:
		{
			RsltConst.m_iVal=Op1Const.m_iVal ^ Op2Const.m_iVal;
			char cBuffer[50];
			RsltConst.m_fVal=RsltConst.m_iVal;
			itoa(RsltConst.m_iVal,cBuffer,10);
			RsltConst.m_szName=cBuffer;
		};break;
	case 24:
		{
			RsltConst.m_fVal=Op1Const.m_fVal*Op2Const.m_fVal;
			char cBuffer[50];
			gcvt(RsltConst.m_fVal,10,cBuffer);
			RsltConst.m_szName=cBuffer;
		};break;
	case 25:
		{
			if (Op2Const.m_fVal==0)
			{
				EmitError("除数不能为0",TokenList.at(iListPos-1));
				return false;
			}
			RsltConst.m_fVal=Op1Const.m_fVal/Op2Const.m_fVal;
			char cBuffer[50];
			gcvt(RsltConst.m_fVal,10,cBuffer);
			RsltConst.m_szName=cBuffer;
		};break;
	case 26:
		{
			if (Op2Const.m_iVal==0)
			{
				EmitError("除数不能为0",TokenList.at(iListPos-1));
				return false;
			}
			RsltConst.m_iVal=Op1Const.m_iVal / Op2Const.m_iVal;
			char cBuffer[50];
			RsltConst.m_fVal=RsltConst.m_iVal;
			itoa(RsltConst.m_iVal,cBuffer,10);
			RsltConst.m_szName=cBuffer;
		};break;
	case 27:
		{
			if (Op2Const.m_iVal==0)
			{
				EmitError("除数不能为0",TokenList.at(iListPos-1));
				return false;
			}
			if (Op1Const.m_iVal<0)
			{
				EmitError("被求模数不允许小于0",TokenList.at(iListPos-1));
				return false;
			}
			RsltConst.m_iVal=Op1Const.m_iVal % Op2Const.m_iVal;
			char cBuffer[50];
			RsltConst.m_fVal=RsltConst.m_iVal;
			itoa(RsltConst.m_iVal,cBuffer,10);
			RsltConst.m_szName=cBuffer;
		};break;
	case 28:
		{
			if (Op2Const.m_iVal<0 || Op2Const.m_iVal>255)
			{
				EmitError("移位量只能界于0~255之间",TokenList.at(iListPos-1));
				return false;
			}
			RsltConst.m_iVal=Op1Const.m_iVal >> Op2Const.m_iVal;
			char cBuffer[50];
			RsltConst.m_fVal=RsltConst.m_iVal;
			itoa(RsltConst.m_iVal,cBuffer,10);
			RsltConst.m_szName=cBuffer;
		};break;
	case 29:
		{
			if (Op2Const.m_iVal<0 || Op2Const.m_iVal>255)
			{
				EmitError("移位量只能界于0~255之间",TokenList.at(iListPos-1));
				return false;
			}
			RsltConst.m_iVal=Op1Const.m_iVal << Op2Const.m_iVal;
			char cBuffer[50];
			itoa(RsltConst.m_iVal,cBuffer,10);
			RsltConst.m_fVal=RsltConst.m_iVal;
			RsltConst.m_szName=cBuffer;
		};break;
	case 30:
		{
			RsltConst.m_iVal=Op1Const.m_iVal & Op2Const.m_iVal;
			char cBuffer[50];
			RsltConst.m_fVal=RsltConst.m_iVal;
			itoa(RsltConst.m_iVal,cBuffer,10);
			RsltConst.m_szName=cBuffer;
		};break;
	case 31:
		{
			RsltConst.m_bVal=Op1Const.m_bVal && Op2Const.m_bVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 32:
		{
			RsltConst.m_szSet=SetMul(Op1Const.m_szSet,Op2Const.m_szSet);
			RsltConst.m_szName=RsltConst.m_szSet;
		};break;
	case 33:
		{
			RsltConst.m_fVal=-Op1Const.m_fVal;
			char cBuffer[50];
			gcvt(RsltConst.m_fVal,10,cBuffer);
			RsltConst.m_szName=cBuffer;
		};break;
	case 34:
		{
			RsltConst.m_iVal=~Op1Const.m_iVal;
			char cBuffer[50];
			RsltConst.m_fVal=RsltConst.m_iVal;
			itoa(RsltConst.m_iVal,cBuffer,10);
			RsltConst.m_szName=cBuffer;
		};break;
	case 35:
		{
			RsltConst.m_bVal=!Op1Const.m_bVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 36:
		{
			if (Op1Const.m_iEnumIdx!=Op2Const.m_iEnumIdx)
			{
				EmitError("枚举类型不兼容",TokenList.at(iListPos-1));
				return false;
			}
			RsltConst.m_bVal=Op1Const.m_szName.compare(Op2Const.m_szName)==0;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 37:
		{
			if (Op1Const.m_iEnumIdx!=Op2Const.m_iEnumIdx)
			{
				EmitError("枚举类型不兼容",TokenList.at(iListPos-1));
				return false;
			}
			RsltConst.m_bVal=Op1Const.m_szName.c_str()!=Op2Const.m_szName.c_str();
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 38:
		{
			RsltConst.m_bVal=Op1Const.m_szSet.compare(Op2Const.m_szSet)==0;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 39:
		{
			RsltConst.m_bVal=Op1Const.m_szSet.compare(Op2Const.m_szSet)!=0;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 40:
		{
			if (Op1Const.m_iVal<0 || Op1Const.m_iVal>255)
			{
				EmitError("集合元素只能界于0~255之间",TokenList.at(iListPos-1));
				return false;
			}
			RsltConst.m_bVal=Op2Const.m_szSet.at(Op1Const.m_iVal)=='1';
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 41:
		{
			RsltConst.m_szSet=SetDel(Op1Const.m_szSet,Op2Const.m_szSet);
			RsltConst.m_szName=RsltConst.m_szSet;
		};break;
	case 42:
		{
			RsltConst.m_szSet=SetAdd(Op1Const.m_szSet,Op2Const.m_szSet);
			RsltConst.m_szName=RsltConst.m_szSet;
		};break;
	case 43:
		{
			if (Op1Const.m_szVal[0]<0 || Op1Const.m_szVal[0]>255)
			{
				EmitError("集合元素只能界于0~255之间",TokenList.at(iListPos-1));
				return false;
			}
			RsltConst.m_bVal=Op2Const.m_szSet.at(Op1Const.m_szVal[0])=='1';
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 44:
		{
			if (Op1Const.m_iEnumIdx!=Op2Const.m_iEnumIdx)
			{
				EmitError("枚举类型不兼容",TokenList.at(iListPos-1));
				return false;
			}
			RsltConst.m_bVal=Op1Const.m_iVal>Op2Const.m_iVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 45:
		{
			if (Op1Const.m_iEnumIdx!=Op2Const.m_iEnumIdx)
			{
				EmitError("枚举类型不兼容",TokenList.at(iListPos-1));
				return false;
			}
			RsltConst.m_bVal=Op1Const.m_iVal>=Op2Const.m_iVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 46:
		{
			if (Op1Const.m_iEnumIdx!=Op2Const.m_iEnumIdx)
			{
				EmitError("枚举类型不兼容",TokenList.at(iListPos-1));
				return false;
			}
			RsltConst.m_bVal=Op1Const.m_iVal<Op2Const.m_iVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 47:
		{
			if (Op1Const.m_iEnumIdx!=Op2Const.m_iEnumIdx)
			{
				EmitError("枚举类型不兼容",TokenList.at(iListPos-1));
				return false;
			}
			RsltConst.m_bVal=Op1Const.m_iVal<=Op2Const.m_iVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	}
	switch ((StoreType)iReturnType)
	{
	case StoreType::T_BOOLEAN:RsltConst.m_ConstType=ConstType::BOOLEAN;break;
	case StoreType::T_CHAR:
	case StoreType::T_STRING:RsltConst.m_ConstType=ConstType::STRING;break;
	case StoreType::T_REAL:
	case StoreType::T_SINGLE:RsltConst.m_ConstType=ConstType::REAL;break;
	default:RsltConst.m_ConstType=ConstType::INTEGER;RsltConst.m_iVal=RsltConst.m_fVal;break;
	}
	Rslt.m_iLink=SymbolTbl.RecConstTbl(RsltConst);
	return true;
}

bool OpConstSemantic(OpInfo Op1,OpInfo Op2,int iProcessId,int iReturnType)
{
	OpInfo Rslt;
	if (!OpConstFold(Op1,Op2,iProcessId,iReturnType,Rslt))
		return false;
	Operand.push(Rslt);
	return true;
}

bool CallPtrProcess(Var Tmp)
{
	ProcCall TmpProc;
	OpInfo TmpRet;
	TmpRet.m_iType=OpInfo::VAR;
	TmpRet.m_iLink=SymbolTbl.ProcInfoTbl.at(SymbolTbl.TypeInfoTbl[Tmp.m_VarTypeStack.top().m_iLink].m_iLink).m_iReturnVar;
	TmpProc.m_Return=TmpRet;
	TmpProc.m_eCallType=ProcCall::PtrCall;
	TmpProc.m_iProcId=SymbolTbl.TypeInfoTbl[Tmp.m_VarTypeStack.top().m_iLink].m_iLink;
	TmpProc.m_iProcPtr=Tmp.m_VarTypeStack.top().m_iLink;
	CurrentProcCall.push(TmpProc);
	return true;
}
#include "Semantic.h"
///*****************声明***********************///
stack<int> iTypePos;//类型栈
stack<int> iIdListFlag;//标识符列表
stack<int> iTypeFlag;//类型标志
bool bForwardFlag;//外部文件引用
stack<string> szVarFieldFlag;
stack<string> szVarFieldConst;
int iEnumSize;//枚举类型表指针
extern int iListPos;
extern vector<CToken> TokenList;
extern CSymbolTbl SymbolTbl;
extern vector<ErrorInfo> ErrorProcess;
extern CLex Lex;
extern CState State;

void semantic000()
{
    while(!iTypePos.empty())
        iTypePos.pop() ;
    while(iTypeFlag.empty())
        iTypeFlag.pop();
    while(iIdListFlag.empty())
        iIdListFlag.pop();
    while(szVarFieldFlag.empty())
        szVarFieldFlag.pop();
    while(szVarFieldConst.empty())
        szVarFieldConst.pop();
    /*************************************
    while(iExpListNum.empty())
        iExpListNum.pop();
    while(iExpListFlag.empty())
        iExpListFlag.pop();
    while(CurrentProcCall.empty())
        CurrentProcCall.pop();
    while(CurrentStatement.empty())
        CurrentStatement.pop();
    while(Operand.empty())
        Operand.pop();
    while(CurrentVar.empty())
        CurrentVar.pop();
    while(Operation.empty())
        Operation.pop();
    while(ArrayProcess.empty())
        ArrayProcess.pop();
    while(WithStack.empty())
        WithStack.pop();**/
}
//程序头 -> program 标识符 001 ( 标识符列表 ) ;
bool semantic001()
{
	semantic000();
	ProcInfo Tmp;
	Tmp.m_szName=TokenList.at(iListPos-1).m_szContent;
	Tmp.m_eRank=ProcInfo::Main;
	Tmp.m_eType=ProcInfo::Procedure;
	Tmp.m_ParaTbl.clear();
	SymbolTbl.ProcInfoTbl.push_back(Tmp);
	SymbolTbl.ProcStack.push(SymbolTbl.ProcInfoTbl.size()-1);
	iIdListFlag.push(0);//标识符列表
	iTypeFlag.push(0);
	return true;
}

//程序 -> 程序头 程序块 002 .
bool semantic002()//是否存在未定义却被使用的标号（全局）
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
		StoreType::CStoreType TmpStoreType=CType::TokenToEnum(TokenList.at(iListPos-1).m_iKind);
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
		Tmp.m_iParaType=-1;//形参类型

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
bool semantic041()//设置过程--过程
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

		TypeInfo Tmp1;//过程类型
		Tmp1.m_iLink=SymbolTbl.ProcInfoTbl.size();
		Tmp1.m_eDataType=StoreType::T_PROC;
		Tmp1.m_szName="_noname";
		Tmp1.m_szName.append(GetSerialId());
		SymbolTbl.AddType(Tmp1);
		iTypePos.push(SymbolTbl.TypeInfoTbl.size()-1);
		SymbolTbl.ProcInfoTbl.push_back(Tmp);

		VarInfo TmpVar;//保证命名不冲突
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

		TypeInfo Tmp1;//函数类型
		Tmp1.m_iLink=SymbolTbl.ProcInfoTbl.size();
		Tmp1.m_eDataType=StoreType::T_FUNC;
		Tmp1.m_szName="_noname";
		Tmp1.m_szName.append(GetSerialId());
		SymbolTbl.AddType(Tmp1);
		iTypePos.push(SymbolTbl.TypeInfoTbl.size()-1);
		SymbolTbl.ProcInfoTbl.push_back(Tmp);

		VarInfo TmpVar;//保证命名不冲突
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

		StoreType::CStoreType ParaType=SymbolTbl.TypeInfoTbl[CType::GetRealType(Tmp.m_iTypeLink)].m_eDataType;

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
    /*
	for (i=0;i<SymbolTbl.TypeInfoTbl.size();i++)//计算符号表每个类型所占字节，方便操作数寻址
	{
		SymbolTbl.CalcTypeSize(i);
	}
    */
	if (!SymbolTbl.PtrCheck(i))//类型回填
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


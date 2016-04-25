#include "Semantic.h"
using namespace std;
stack<int> iTypePos;//����ջ
stack<int> iIdListFlag;//��ʶ���б�
stack<int> iTypeFlag;//���ͱ�־
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

//����ͷ -> program ��ʶ�� 001 ( ��ʶ���б� ) ;
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
	iIdListFlag.push(0);//��ʶ���б�
	iTypeFlag.push(0);
	return true;
}

//���� -> ����ͷ ����� 002 .
bool semantic002()
{
	for (int i=0;i<SymbolTbl.LabelInfoTbl.size();i++)
	{
		if (SymbolTbl.LabelInfoTbl[i].m_szName[0]!='_'
			&& !SymbolTbl.LabelInfoTbl[i].m_bDef
			&& SymbolTbl.LabelInfoTbl[i].m_bUse)
		{
			EmitError(SymbolTbl.LabelInfoTbl[i].m_szName+"���δ����",TokenList.at(iListPos-1));
			return false;
		}
	}
	return true;
}

//����������� -> label 002 ��ʶ�� 003 ��������б� ;
//��������б� -> , ��ʶ�� 003 ��������б�
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
		EmitError(Tmp.m_szName.append("��ʶ���Ѿ�����"),TokenList.at(iListPos-1));
		return false;
	}
}

//�������� -> ��ʶ�� 102 = ���ʽ 004
bool semantic004()
{
	string szID=TokenList.at(iListPos-3).m_szContent;

	if (SymbolTbl.SearchLabelInfoTbl(SymbolTbl.ProcStack.top(),szID)==-1
		&& SymbolTbl.SearchConstInfoTbl(SymbolTbl.ProcStack.top(),szID)==-1)
	{
		ConstInfo Tmp;

		//���ֻ�������ʽ�ǳ���ֵ�����

		Tmp=SymbolTbl.ConstInfoTbl[atoi(TokenList.at(iListPos-1).m_szContent.c_str())];
		Tmp.m_szName=szID;
		Tmp.m_iProcIndex=SymbolTbl.ProcStack.top();
		SymbolTbl.AddConst(Tmp);
		return true;
	}
	else
	{
		EmitError(szID.append("��ʶ���Ѿ�����"),TokenList.at(iListPos-3));
		return false;
	}

}

//���̵������ -> �� 005
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

//���Ͷ��� -> ��ʶ�� 006 = ����
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
		EmitError(Tmp.m_szName.append("��ʶ���Ѿ�����"),TokenList.at(iListPos-1));
		return false;
	}
}


/*
  ��������	-> char 007
            -> boolean 007
  ��������	-> integer 007
  			-> byte 007
  			-> shortint 007
  			-> smallint 007
  			-> word 007
 			-> longword 007
  			-> cardinal 007
  ��������	-> real 007
  			-> single 007
*/
bool semantic007()//�ַ���---
{
	if (iTypeFlag.top()==2)	//��������----��ǰ������Ϣδ��ջ
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
			EmitError("�������͵Ļ����Ͳ���ȷ",TokenList.at(iListPos-1));
			return false;
		}
	}

	if (iTypeFlag.top()==5
		|| iTypeFlag.top()==6)	//���̡������β��б�
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

	if (iTypeFlag.top()==7)		//������������
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
		&& SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eDataType==StoreType::T_NONE)	//�������� ��ʼΪ��
	{
		StoreType TmpStoreType=CType::TokenToEnum(TokenList.at(iListPos-1).m_iKind);
		SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eDataType=TmpStoreType;
		SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eBaseType=TmpStoreType;
		SymbolTbl.TypeInfoTbl[iTypePos.top()].m_iState=1;
	}

	return true;
}

/*
ָ������	-> ^ ������
������	-> ��ʶ�� 008
*/
bool semantic008()
{
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eDataType=StoreType::T_POINTER;
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_szContent=TokenList.at(iListPos-1).m_szContent;
    //ָ�����͵Ļ����Ͳ�һ����ָ������ǰ����
	int i=SymbolTbl.SearchTypeInfoTbl(SymbolTbl.ProcStack.top(),TokenList.at(iListPos-1).m_szContent,false);

	if (i!=-1)
	{
		if (SymbolTbl.TypeInfoTbl[i].m_iState!=1)//����ָ����������--ָ��ͬһ��������
		{
			EmitError("ָ��ָ�����������������",TokenList.at(iListPos-1));
			return false;
		}
	}
	return true;
}

/*
  ָ������	-> ^ ������
  ������	-> ��ʶ�� 008
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
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_iLink=SymbolTbl.TypeInfoTbl.size();	//����ָ�����͵Ļ�����

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
  ����		-> 015 �������� 010
			-> 015 �������� 010
			-> 015 �������� 010
			-> 015 ָ������ 010	^
			-> 015 �ַ������� 010
			-> 015 ��ʶ�� 023 010
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

// ö������		-> ( 011 ��ʶ���б� ) 014
bool semantic011()
{
	iEnumSize=SymbolTbl.EnumInfoTbl.size();

	if (iTypeFlag.top()==2)		//�������͵Ļ����Ϳ�����ö������
	{
		SymbolTbl.TypeInfoTbl[iTypePos.top()].m_iLink=SymbolTbl.TypeInfoTbl.size();	//���ü������͵Ļ�����
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
  ��ʶ���б�		-> ��ʶ�� 012 ��ʶ���б�
  ��ʶ���б�		-> , ��ʶ�� 012 ��ʶ���б�
*/
bool semantic012()
{
	if (iIdListFlag.top()==1)	//ö�����ͱ�ʶ���б�
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
				EmitError("ö�����͵ı�Ÿ������ô���256",TokenList.at(iListPos-1));
				return false;
			}
		}
		else
		{
			EmitError("��ʶ�����Ѿ����ڣ���ñ�ʶ���Ѷ���",TokenList.at(iListPos-1));
			return false;
		}
	}

	if (iIdListFlag.top()==2)	//��¼�����ֶ����б��еı�ʶ��
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
				EmitError(Tmp.m_szName.append("�����Ѿ�����"),TokenList.at(iListPos-1));
				return false;
			}
		}

		SymbolTbl.TypeInfoTbl[iTypePos.top()].m_FieldInfo.push_back(Tmp);
	}

	if (iIdListFlag.top()==3)	//��¼���Ϳɱ��ֶ����б��еı�ʶ��
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
				EmitError(Tmp.m_szName.append("�����Ѿ�����"),TokenList.at(iListPos-1));
				return false;
			}
		}

		SymbolTbl.TypeInfoTbl[iTypePos.top()].m_FieldInfo.push_back(Tmp);
	}

	if (iIdListFlag.top()==4 || iIdListFlag.top()==5)	//���̺�����ʽ�����б�ı�ʶ��
	{
		ParaInfo Tmp;
		Tmp.m_szName=TokenList.at(iListPos-1).m_szContent;
		Tmp.m_eAssignType=iIdListFlag.top()==4?ParaInfo::VAL:ParaInfo::VAR;
		Tmp.m_iParaType=-1;

		for(int i=SymbolTbl.ProcInfoTbl.back().m_ParaTbl.size()-1;i>=0;i--)
		{
			if (SymbolTbl.ProcInfoTbl.back().m_ParaTbl.at(i).m_szName.compare(Tmp.m_szName)==0)
			{
				EmitError(Tmp.m_szName.append("��ʶ�����Ѿ�����"),TokenList.at(iListPos-1));
				return false;
			}
		}
		SymbolTbl.ProcInfoTbl.back().m_ParaTbl.push_back(Tmp);
		return true;
	}

	if (iIdListFlag.top()==6)	//�����б�ı�ʶ��
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
			EmitError(Tmp.m_szName.append("��ʶ�����Ѿ�����"),TokenList.at(iListPos-1));
			return false;
		}
	}

	if (iIdListFlag.top()==7)	//����USESģ���������
	{
		string Tmp=TokenList.at(iListPos-1).m_szContent;
		if (SymbolTbl.SearchUseFileTbl(Tmp)==-1)
		{
			SymbolTbl.UseFileTbl.push_back(UseFile(Tmp,false));
			return true;
		}
		else
		{
			EmitError(Tmp.append("ģ���Ѿ�����"),TokenList.at(iListPos-1));
			return false;
		}
	}

	return true;
}

//  ��������	-> 013 ��ʶ���б� 040 : ����
bool semantic013()
{
	iIdListFlag.push(6);
	return true;
}

// ö������		-> ( 011 ��ʶ���б� ) 014
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
  ����		-> 015 �������� 010
 			-> 015 �������� 010
			-> 015 �������� 010
			-> 015 ָ������ 010
  			-> 015 �ַ������� 010
  			-> 015 ��ʶ�� 023 010
*/
bool semantic015()//��Ҫ���û�����ʱ�����ж����������Ǻ�������
{
	if (iTypeFlag.top()==2			//��������
		|| iTypeFlag.top()==5		//�����β��б�
		|| iTypeFlag.top()==6		//�����β��б�
		|| iTypeFlag.top()==7)		//������������
	{
		iTypePos.push(SymbolTbl.TypeInfoTbl.size()-1);//Ϊ�����ͳһ
		return true;
	}

	if (iTypeFlag.top()==3			//��������
		|| iTypeFlag.top()==4)//�ļ�����
	{
		TypeInfo Tmp;
		Tmp.m_szName="_noname";
		Tmp.m_szName.append(GetSerialId());
		Tmp.m_iProcIndex=SymbolTbl.ProcStack.top();
		Tmp.m_iState=0;
		SymbolTbl.TypeInfoTbl[iTypePos.top()].m_iLink=SymbolTbl.TypeInfoTbl.size();
		SymbolTbl.AddType(Tmp);
		iTypePos.push(SymbolTbl.TypeInfoTbl.size()-1);//----��ǰ������Ϣ��ջ
		return true;
	}

	if (iTypeFlag.top()==1)			//��¼����
	{
		TypeInfo Tmp;
		Tmp.m_szName="_noname";
		Tmp.m_szName.append(GetSerialId());
		Tmp.m_iProcIndex=SymbolTbl.ProcStack.top();
		Tmp.m_iState=0;
		SymbolTbl.AddType(Tmp);
		int i=SymbolTbl.TypeInfoTbl[iTypePos.top()].m_FieldInfo.size()-1;
		int iTmpTypePos=iTypePos.top();
		iTypePos.push(SymbolTbl.TypeInfoTbl.size()-1);//----��ǰ������Ϣ��ջ

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

//��������		-> array 016 [ ������� ��������б� ] of  ���� 025
bool semantic016()
{
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eDataType=StoreType::T_ARRAY;
	iTypeFlag.push(3);	//��������
	return true;
}

/*
  ��������б�		->  , ������� ��������б�
					-> ��
  �������			-> �������� 017 .. �������� 018
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
��������б�		->  , ������� ��������б�
					-> ��
�������			-> �������� 017 .. �������� 018
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
		EmitError("���鶨������С������",TokenList.at(iListPos-1));
		return false;
	}
}

// ��¼����			-> record 019 �ֶ��б� end 020
bool semantic019()
{
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eDataType=StoreType::T_RECORD;
	iTypeFlag.push(1);	//��¼����
	iIdListFlag.push(2);
	return true;
}

// ��¼����			-> record 019 �ֶ��б� end 020
bool semantic020()
{
	iTypeFlag.pop();
	iIdListFlag.pop();
	return true;
}

// ��������			-> procedure 021 �β��б� 039 044
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
	iTypeFlag.push(5);	//��������
	return true;
}

//��������			-> function 022 �β��б� : 038  ���� 039 039 044
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
	iTypeFlag.push(6);	//��������
	return true;
}

//����				-> 015 ��ʶ�� 023 010
bool semantic023()
{

	string szTmp=TokenList.at(iListPos-1).m_szContent;

	int i=SymbolTbl.SearchTypeInfoTbl(SymbolTbl.ProcStack.top(),szTmp);	//������ǰ���̻���

	if (i==-1)
	{
		i=SymbolTbl.SearchTypeInfoTbl(0,szTmp);		//����������
	}

	if (i!=-1)
	{
		if (iTypeFlag.top()==2)	//��������
		{
			int j=i;
			string szTmp1;

			while (SymbolTbl.TypeInfoTbl[j].m_eDataType==StoreType::T_USER)
			{
				if (SymbolTbl.TypeInfoTbl[j].m_iLink==-1)
				{
					EmitError("ϵͳ��������",TokenList.at(iListPos-1));
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
				EmitError("�������͵Ļ����Ͳ���ȷ",TokenList.at(iListPos-1));
				return false;
			}
		}

		if (iTypeFlag.top()==5
			|| iTypeFlag.top()==6)	//���̡������β��б�
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

		if (iTypeFlag.top()==7)	//������������
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
		EmitError(szTmp.append("����δ������岻����"),TokenList.at(iListPos-1));
		return false;
	}

	return true;
}

//��������			-> set 024 of ���� 025
bool semantic024()
{
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eDataType=StoreType::T_SET;
	iTypeFlag.push(2);		//��������

	return true;
}

//��������			-> set 024 of ���� 025
bool semantic025()
{
	iTypeFlag.pop();
	return true;
}

//�Ѳ���
bool semantic026()
{
	iTypeFlag.pop();
	return true;
}

//�ļ�����			-> file of 027 ���� 028
bool semantic027()
{
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_eDataType=StoreType::T_FILE;
	iTypeFlag.push(4);		//�ļ�����
	return true;
}

//�ļ�����			-> file of 027 ���� 028
bool semantic028()
{
	iTypeFlag.pop();
	return true;
}

/*
 �ֶ��б�			-> �ֶι̶����� �ֶ��б�
					-> �ֶοɱ䲿�� �ֶ��б�
					-> ��
 �ֶι̶�����		-> ��ʶ���б�  : ���� ;
 �ֶοɱ䲿��		-> case  ��ʶ�� 029 : ����  of 033 �ֶοɱ䲿�֣� 034 end 030 ;
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
			EmitError(Tmp.m_szName.append("�����Ѿ�����"),TokenList.at(iListPos-1));
			return false;
		}
	}

	szVarFieldFlag.push(Tmp.m_szName);
	SymbolTbl.TypeInfoTbl[iTypePos.top()].m_FieldInfo.push_back(Tmp);

	return true;
}

/*
 �ֶ��б�			-> �ֶι̶����� �ֶ��б�
					-> �ֶοɱ䲿�� �ֶ��б�
					-> ��
 �ֶι̶�����		-> ��ʶ���б�  : ���� ;
 �ֶοɱ䲿��		-> case  ��ʶ�� 029 : ����  of 033 �ֶοɱ䲿�֣� 034 end 030 ;
*/
bool semantic030()
{
	szVarFieldFlag.pop();
	return true;
}


// �ֶοɱ䲿�֣�	-> ���� 031 : ( �ֶ��б� ) 032 ; �ֶοɱ䲿�֣�
bool semantic031()
{
	int i=atoi(TokenList.at(iListPos-1).m_szContent.c_str());
	szVarFieldConst.push(SymbolTbl.ConstInfoTbl[i].m_szVal);
	return true;
}

// �ֶοɱ䲿�֣�	-> ���� 031 : ( �ֶ��б� ) 032 ; �ֶοɱ䲿�֣�
bool semantic032()
{
	szVarFieldConst.pop();
	return true;
}

// �ֶοɱ䲿��		-> case  ��ʶ�� 029 : ����  of 033 �ֶοɱ䲿�֣� 034 end 030 ;
bool semantic033()
{
	iIdListFlag.push(3);	//�ֶοɱ䲿��
	return true;
}

// �ֶοɱ䲿��		-> case  ��ʶ�� 029 : ����  of 033 �ֶοɱ䲿�֣� 034 end 030 ;
bool semantic034()
{
	iIdListFlag.pop();
	return true;
}

/*
  �β��б�			-> ( ����˵�� �������� )
					-> ��
  ��������			-> ; ����˵�� ��������
					-> ��
  ����˵��			-> 035 ��ʶ���б� 036 : ����
*/
bool semantic035()
{
	iIdListFlag.push(4);	//���̺�����ʽ�����б�ֵ�η�ʽ��
	return true;
}

/*
�β��б�			-> ( ����˵�� �������� )
					-> ��
��������			-> ; ����˵�� ��������
					-> ��
����˵��			-> 035 ��ʶ���б� 036 : ����
*/
bool semantic036()
{
	iIdListFlag.pop();
	return true;
}


/*
 �β��б�			-> ( ����˵�� �������� )
					-> ��
 ��������			-> ; ����˵�� ��������
					-> ��
 ����˵��			-> var 037 ��ʶ���б� 036 : ����
*/
bool semantic037()
{
	iIdListFlag.push(5);	//���̺�����ʽ�����б���η�ʽ��
	return true;
}

// ��������			-> function 022 �β��б� : 038  ���� 039 039 044
bool semantic038()
{
	iTypeFlag.push(7);
	return true;
}

// ��������			-> function 022 �β��б� : 038  ���� 039 039 044
bool semantic039()
{
	iTypeFlag.pop();
	return true;
}

// ��������			-> 013 ��ʶ���б� 040 : ����
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

// ����ͷ			-> procedure ��ʶ�� 041 �β��б� 039
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
		EmitError(Tmp.m_szName.append("��ʶ���Ѿ�����"),TokenList.at(iListPos-1));
		return false;
	}

	int i=SymbolTbl.SearchProcInfoTbl(Tmp.m_szName);

	if (i==-1 || (i!=-1 && SymbolTbl.ProcInfoTbl.at(i).m_eFlag==ProcInfo::Forward))
	{
		if (i==-1 && SymbolTbl.SearchVarInfoTbl(0,Tmp.m_szName)!=-1)
		{
			EmitError(Tmp.m_szName.append("��ʶ���Ѿ�����"),TokenList.at(iListPos-1));
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

		iTypeFlag.push(5);	//��������

		if (i!=-1 && SymbolTbl.ProcInfoTbl.at(i).m_eFlag==ProcInfo::Forward)
		{
			bForwardFlag=true;
		}

		return true;
	}
	else
	{
		EmitError(Tmp.m_szName.append("��ʶ���Ѿ�����"),TokenList.at(iListPos-1));
		return false;
	}
}

//����ͷ			-> function ��ʶ�� 042 �β��б� : 038 ���� 039 039 067
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
		EmitError(Tmp.m_szName.append("��ʶ���Ѿ�����"),TokenList.at(iListPos-1));
		return false;
	}

	int i=SymbolTbl.SearchProcInfoTbl(Tmp.m_szName);

	if (i==-1 || (i!=-1 && SymbolTbl.ProcInfoTbl.at(i).m_eFlag==ProcInfo::Forward))
	{
		if (i==-1 && SymbolTbl.SearchVarInfoTbl(0,Tmp.m_szName)!=-1)
		{
			EmitError(Tmp.m_szName.append("��ʶ���Ѿ�����"),TokenList.at(iListPos-1));
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

		iTypeFlag.push(6);	//��������

		if (i!=-1 && SymbolTbl.ProcInfoTbl.at(i).m_eFlag==ProcInfo::Forward)
		{
			bForwardFlag=true;
		}

		return true;
	}
	else
	{
		EmitError(Tmp.m_szName.append("��ʶ���Ѿ�����"),TokenList.at(iListPos-1));
		return false;
	}
}

//���̺����������֣�		-> 043 044 ����� 047
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
				EmitError("����ԭ������ǰ����ԭ�Ͳ�һ��",TokenList.at(iListPos-1));
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

//���̺����������֣�		-> ��Դָ�� 044 047
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
			EmitError("���̻���������ָ���STRING���Ͳ�����VAR��ʽ����");
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
			EmitError("���̻���"+SymbolTbl.ProcInfoTbl.back().m_szName+"����"+Tmp.m_szName+"���Ͳ���ȷ");
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

// ��Դָ��					-> 045 forward
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
				EmitError(Tmp1.m_szName.append("���̻����Ѿ�������ǰ���ã������ظ�����"),TokenList.at(iListPos-1));
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

//��Դָ��					-> �ַ������� 046
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
			EmitError("�ⲿ�������������󣬱������������Ϣ�����÷�ʽ������ԭ�͡������ļ�",TokenList.at(iListPos-1));
			return false;
		}
	}
	else
	{
		EmitError("�ⲿ�������������󣬱������������Ϣ�����÷�ʽ������ԭ�͡������ļ�",TokenList.at(iListPos-1));
		return false;
	}

	if (UpperCase(szCallType)!="STDCALL" && UpperCase(szCallType)!="FASTCALL")
	{
		EmitError("�ⲿ�����ĵ��÷�ʽ������ʾ��������ѡ��ʽΪfastcall��stdcall",TokenList.at(iListPos-1));
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
 ���̺����������֣�			-> 043 044 ����� 047
							-> ��Դָ�� 044 047
*/
bool semantic047()
{
	SymbolTbl.ProcStack.pop();
	return true;
}

// �����					-> ˵������ 048 ��䲿��
bool semantic048()
{
	int i;

	for (i=0;i<SymbolTbl.TypeInfoTbl.size();i++)
	{
		SymbolTbl.CalcTypeSize(i);
	}

	if (!SymbolTbl.PtrCheck(i))
	{
		EmitError(SymbolTbl.TypeInfoTbl[i].m_szContent.append("���Ͳ�����"),TokenList.at(iListPos-1));
		return false;
	}


	if (SymbolTbl.ProcStack.empty())
	{
		SymbolTbl.ProcStack.push(0);
	}

	return true;
}

// ����	-> ���� 049
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
 ���ʽ��		-> ��ϵ����� 050 �� 051 ���ʽ��
 �			-> �����ȼ������ 050 ��ʽ 051 �
 ��ʽ��			-> �����ȼ������ 050 ���� 051 ��ʽ��
 ����			-> + 050 ���� 052
				-> - 050 ���� 052
				-> not 050 ���� 052
*/
bool semantic050()
{
	Operation.push(TokenList.at(iListPos-1).m_iKind);
	return true;
}

/*
 ���ʽ��		-> ��ϵ����� 050 �� 051 ���ʽ��
 �			-> �����ȼ������ 050 ��ʽ 051 �
 ��ʽ��			-> �����ȼ������ 050 ���� 051 ��ʽ��
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
		EmitError("ȱ�ٲ�����",TokenList.at(iListPos-1));
		return false;
	}

	Operand.pop();

	if (!Operand.empty())
	{
		Tmp1=Operand.top();
	}
	else
	{
		EmitError("ȱ�ٲ�����",TokenList.at(iListPos-1));
		return false;
	}

	Operand.pop();

	if (!Operation.empty())
	{
		TmpOperation=Operation.top();
	}
	else
	{
		EmitError("ȱ�ٲ�����",TokenList.at(iListPos-1));
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
		EmitError("���������Ͳ�����",TokenList.at(iListPos-1));
		return false;
	}
	return true;
}

/*
 ����			-> + 050 ���� 052
				-> - 050 ���� 052
				-> not 050 ���� 052
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
		EmitError("ȱ�ٲ�����",TokenList.at(iListPos-1));
		return false;
	}

	Operand.pop();

	if (!Operation.empty())
	{
		TmpOperation=Operation.top();
	}
	else
	{
		EmitError("ȱ�ٲ�����",TokenList.at(iListPos-1));
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
		EmitError("���������Ͳ�����",TokenList.at(iListPos-1));
		return false;
	}
	return true;
}

// ��ʶ����ʼ���		-> ������ 053 := ���ʽ 064
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

//����			-> ��ʶ�� 054 ������
bool semantic054()
{
	Var Tmp;
	string szTmp=TokenList.at(iListPos-1).m_szContent;
	int j=WithStack.size()-1;

	//���ȴ���with����
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
							EmitError("��ʶ��δ������Ƿ�����",TokenList.at(iListPos-1));
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

//���ʽ�б�		-> ���ʽ 055 ���ʽ�б�
bool semantic055()
{
	if (iExpListFlag.top()==1)	//���������±����
	{
		if (bConstFlag
			|| Operand.empty()
			|| CurrentVar.empty())
		{
			EmitError("�������",TokenList.at(iListPos-1));
			return false;
		}

		OpInfo Tmp,Op1,Op2,TmpRslt;
		IRCode TmpIR;
		int iTypeLink;
		Var* TmpVar;
		Tmp=Operand.top();	//��ȡ�±���ʽ
		Operand.pop();
		TmpVar=&CurrentVar.top();	//��ȡ�������
		TmpVar->m_iDim=1;
		iTypeLink=TmpVar->m_VarTypeStack.top().m_iLink;

		if (Tmp.m_iType==OpInfo::CONST)	//�������±�ƫ��
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
				EmitError("Խ����ʻ������±����Ͳ���ȷ",TokenList.at(iListPos-1));
				return false;
			}
		}
		else	//��������±�ƫ��
		{
			if (!CType::IsInt(CType::GetOpType(Tmp)))
			{
				EmitError("�����±����Ͳ���ȷ",TokenList.at(iListPos-1));
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

//������	-> . ��ʶ�� 056 ������
bool semantic056()
{
	if (bConstFlag || CurrentVar.empty())
	{
		EmitError("�������",TokenList.at(iListPos-1));
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
		EmitError("�ֶ�δ����",TokenList.at(iListPos-1));
		return false;
	}
}

//���ʽ�б�	-> , ���ʽ 057 ���ʽ�б�
bool semantic057()
{
	if (iExpListFlag.top()==1)
	{
		if (bConstFlag || Operand.size()<2 || CurrentVar.empty())
		{
			EmitError("�������",TokenList.at(iListPos-1));
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
			EmitError("�������ά����������ά��",TokenList.at(iListPos-1));
			return false;
		}

		if (Tmp1.m_iType==OpInfo::CONST && Tmp2.m_iType==OpInfo::CONST)	//�������������ǳ����������۵�
		{
			if (CType::IsInt(CType::GetOpType(Tmp1))
				&& CType::IsInt(CType::GetOpType(Tmp2))
				&& SymbolTbl.TypeInfoTbl[TmpVar->m_VarTypeStack.top().m_iLink].m_ArrayInfo.at(j-1).m_iStart
				   <=SymbolTbl.ConstInfoTbl[Tmp1.m_iLink].m_iVal
				&& SymbolTbl.TypeInfoTbl[TmpVar->m_VarTypeStack.top().m_iLink].m_ArrayInfo.at(j-1).m_iEnd
				   >=SymbolTbl.ConstInfoTbl[Tmp1.m_iLink].m_iVal)
			{
				int i;

				//�ж��Ƿ���Ҫ�ۼ�ά�Ȼ�����ĩά����Ҫ����
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
				EmitError("�������Խ��������±����Ͳ���ȷ",TokenList.at(iListPos-1));
				return false;
			}
		}
		else
		{
			if (!CType::IsInt(CType::GetOpType(Tmp1)) || !CType::IsInt(CType::GetOpType(Tmp2)))
			{
				EmitError("�����±����Ͳ���ȷ",TokenList.at(iListPos-1));
				return false;
			}

			TmpRslt.m_iLink=SymbolTbl.GetTmpVar(SymbolTbl.ProcStack.top(),StoreType::T_INTEGER);
			TmpRslt.m_iType=OpInfo::VAR;

			if (!GenIR(Tmp1,Tmp2,TmpRslt,BasicOpType::ADD))
			{
				return false;
			}

			Operand.push(TmpRslt);

			//�ж��Ƿ���Ҫ�ۼ�ά�Ȼ�����ĩά����Ҫ����
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

//������	-> ^ 058 ������
bool semantic058()
{
	if (bConstFlag || CurrentVar.empty())
	{
		EmitError("�������",TokenList.at(iListPos-1));
		return false;
	}

	Var TmpVar=CurrentVar.top();

	if (TmpVar.m_VarTypeStack.empty()
		|| TmpVar.m_VarTypeStack.top().m_StoreType!=StoreType::T_POINTER)
	{
		EmitError("^�����ֻ��������ָ������",TokenList.at(iListPos-1));
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

//������	-> [ 098 ���ʽ�б� 099 ] 059 ������
bool semantic059()
{
	if (bConstFlag || Operand.empty() || CurrentVar.empty())
	{
		EmitError("�������",TokenList.at(iListPos-1));
		return false;
	}

	Var TmpVar=CurrentVar.top();

	CurrentVar.pop();

	if (SymbolTbl.TypeInfoTbl[TmpVar.m_VarTypeStack.top().m_iLink].m_ArrayInfo.size()>TmpVar.m_iDim)
	{
		EmitError("�������ά��С������ά��",TokenList.at(iListPos-1));
		return false;
	}

	int iArrayLowC=0;

	int i;

	//��������ƫ�ƵĲ��䲿��

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

	//�ۼƼ�������ƫ�ƵĿɱ䲿��
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

//����		-> @ ���� 053 060
bool semantic060()
{
	OpInfo Tmp,Op1,Rslt;

	if (Operand.empty())
	{
		EmitError("�������",TokenList.at(iListPos-1));
		return false;
	}

	Tmp=Operand.top();

	Operand.pop();

	if (Tmp.m_iType==OpInfo::CONST
		|| (Tmp.m_iType==OpInfo::VAR
		&& SymbolTbl.IsTmpVar(Tmp.m_iLink)
		&& Tmp.m_bRef==false))
	{
		EmitError("��ַ����Ĳ���������Ϊ��ֵ",TokenList.at(iListPos-1));
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

//���		-> ��ʶ�� 061 ��ʶ����ʼ���
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
			EmitError("����ظ�����",TokenList.at(iListPos-1));
			return false;
		}
	}
	else
	{
		return semantic054();
	}

	return true;
}

//��ʶ����ʼ���	-> : 062 ���
bool semantic062()
{
	bLabelFlag=false;
	return true;
}

//�������		-> goto ��ʶ�� 063
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
		EmitError("���δ����",TokenList.at(iListPos-1));
		return false;
	}

	return true;
}

//��ʶ����ʼ���	-> ������ 053 := ���ʽ 064
bool semantic064()
{
	if (Operand.size()<2)
	{
		EmitError("�������",TokenList.at(iListPos-1));
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
		EmitError("���������Ͳ�ƥ��",TokenList.at(iListPos-1));
		return false;
	}

	return true;
}

/*
 ʵ���б�		-> ���ʽ 065 ʵ���б�
				-> ��
 ʵ���б�		-> , ���ʽ 065 ʵ���б�
				-> ��
*/
bool semantic065()
{
	if (Operand.empty() || CurrentProcCall.empty())
	{
		EmitError("�������:ȱ��ʵ�α��ʽ",TokenList.at(iListPos-1));
		return false;
	}

	CurrentProcCall.top().m_Paras.push_back(Operand.top());
	Operand.pop();

	return true;
}

/*
 ���̵������	-> ( 101 ʵ���б� ) 066
				-> �� 005
*/
bool semantic066()
{

	ProcCall Tmp=CurrentProcCall.top();
	CurrentProcCall.pop();

	if (Tmp.m_iProcId==-1)		//����Ϊsizeof
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
			EmitError("sizeof����ֻ�ܴ���һ������ʵ��",TokenList.at(iListPos-1));
			return false;
		}
	}

	if (Tmp.m_iProcId==0)
	{
		EmitError("�������:���������������",TokenList.at(iListPos-1));
		return false;
	}

	if (SymbolTbl.ProcInfoTbl.at(Tmp.m_iProcId).m_ParaTbl.size()!=Tmp.m_Paras.size())
	{
		EmitError("�������:ʵ�����βθ�����ƥ��",TokenList.at(iListPos-1));
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
			EmitError("ʵ�����β��������Ͳ�ƥ��",TokenList.at(iListPos-1));
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

//����ͷ	-> function ��ʶ�� 042 �β��б� : 038 ���� 039 039 067
bool semantic067()
{
	if (SymbolTbl.ProcInfoTbl.back().m_iReturnType==-1)
	{
		EmitError("������������ֻ���ǻ������ͻ��û��Զ�������",TokenList.at(iListPos-1));
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

//�������	-> if ���ʽ 068 then ��� if���� 070
bool semantic068()
{
	if (Operand.empty())
	{
		EmitError("�������",TokenList.at(iListPos-1));
		return false;
	}

	OpInfo Tmp=Operand.top();
	Operand.pop();

	if (CType::GetOpType(Tmp)!=StoreType::T_BOOLEAN)
	{
		EmitError("IF����ж����ʽ����ΪBOOLEAN����",TokenList.at(iListPos-1));
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
 if����		-> else 069 ���
 				-> ��
*/
bool semantic069()
{
	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::IF)
	{
		EmitError("ELSE�Ӿ������IF���ʹ��",TokenList.at(iListPos-1));
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

//�������		-> if ���ʽ 068 then ��� if���� 070
bool semantic070()
{
	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::IF)
	{
		EmitError("�������",TokenList.at(iListPos-1));
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

//�������		-> while 071 ���ʽ 073 do ��� 072
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

//�������		-> while 071 ���ʽ 073 do ��� 072
bool semantic072()
{
	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::WHILE)
	{
		EmitError("�������",TokenList.at(iListPos-1));
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

//�������		-> while 071 ���ʽ 073 do ��� 072
bool semantic073()
{
	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::WHILE)
	{
		EmitError("�������",TokenList.at(iListPos-1));
		return false;
	}

	if (Operand.empty())
	{
		EmitError("�������",TokenList.at(iListPos-1));
		return false;
	}

	OpInfo Tmp=Operand.top();
	Operand.pop();

	if (CType::GetOpType(Tmp)!=StoreType::T_BOOLEAN)
	{
		EmitError("WHILE����ж����ʽ����ΪBOOLEAN����",TokenList.at(iListPos-1));
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

//�������		-> repeat 074 ������� until ���ʽ 075
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

//�������		-> repeat 074 ������� until ���ʽ 075
bool semantic075()
{

	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::REPEAT)
	{
		EmitError("�������",TokenList.at(iListPos-1));
		return false;
	}

	if (Operand.empty())
	{
		EmitError("�������",TokenList.at(iListPos-1));
		return false;
	}

	OpInfo Tmp=Operand.top();
	Operand.pop();

	if (CType::GetOpType(Tmp)!=StoreType::T_BOOLEAN)
	{
		EmitError("WHILE����ж����ʽ����ΪBOOLEAN����",TokenList.at(iListPos-1));
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

//�������		-> for ��ʶ�� 076 := ���ʽ 077 for����
bool semantic076()
{
	string szTmp=TokenList.at(iListPos-1).m_szContent;
	int i=SymbolTbl.SearchVarInfoTbl(SymbolTbl.ProcStack.top(),szTmp);

	if (i==-1)
	{
		i=SymbolTbl.SearchVarInfoTbl(0,szTmp);
		if (i==-1)
		{
			EmitError("ѭ������δ����",TokenList.at(iListPos-1));
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

//�������		-> for ��ʶ�� 076 := ���ʽ 077 for����
bool semantic077()
{
	if (Operand.empty()
		|| CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::FOR)
	{
		EmitError("�������",TokenList.at(iListPos-1));
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
		EmitError("ѭ�������ĳ�ֵ��Ϊ�������ͻ������Ͳ�����",TokenList.at(iListPos-1));
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

//for����		-> to ���ʽ 078 do ��� 079
bool semantic078()
{
	if (Operand.empty()
		|| CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::FOR)
	{
		EmitError("�������",TokenList.at(iListPos-1));
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
		EmitError("ѭ����������ֵ��Ϊ�������ͻ������Ͳ�����",TokenList.at(iListPos-1));
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

//for����		-> to ���ʽ 078 do ��� 079
bool semantic079()
{
	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::FOR)
	{
		EmitError("�������",TokenList.at(iListPos-1));
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

//for����		-> downto ���ʽ 080 do ��� 079
bool semantic080()
{

	if (Operand.empty()
		|| CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::FOR)
	{
		EmitError("�������",TokenList.at(iListPos-1));
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
		EmitError("ѭ����������ֵ��Ϊ�������ͻ������Ͳ�����",TokenList.at(iListPos-1));
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

//�������		-> break 081
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
		EmitError("BREAK���ֻ�ܳ�����FOR��WHILE��REPEAT������",TokenList.at(iListPos-1));
		return false;
	}

	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top())
				.m_Codes.push_back(EmitIR(OpType::JMP,pState->GetLabel(Statement::LabelIdx::ExitLabel)));
	return true;
}

//�������		-> continue 082
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
		EmitError("CONTINUE���ֻ�ܳ�����FOR��WHILE��REPEAT������",TokenList.at(iListPos-1));
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

//�������		-> asm �ַ������� , �ַ������� 083 end
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
				EmitError("��Ƕ������Ĳ�����Ϣ����ȷ",TokenList.at(iListPos-1));
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
					EmitError("��Ƕ������Ĳ�����д���Բ���ȷ",TokenList.at(iListPos-1));
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
						EmitError("����"+szTmp+"������",TokenList.at(iListPos-1));
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
				EmitError("��Ƕ���������ò���ȷ",TokenList.at(iListPos-1));
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

//�������		-> case ���ʽ 084 of case��֧ end 089
bool semantic084()
{
	if (Operand.empty()
		|| !CType::IsOrd(Operand.top()))
	{
		EmitError("CASE��ѡ���ӱ���Ϊ��������",TokenList.at(iListPos-1));
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

//�����б�		-> ���� 085 �����б�
bool semantic085()
{
	OpInfo Tmp;
	Tmp.m_iType=OpInfo::CONST;
	Tmp.m_iLink=atoi(TokenList.at(iListPos-1).m_szContent.c_str());

	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::CASE)
	{
		EmitError("�������",TokenList.at(iListPos-1));
		return false;
	}

	if (CType::TypeCompatible(Tmp,CurrentStatement.top().m_CaseExp,17)==-1)
	{
		EmitError("case�Ӿ�ĳ�����ѡ���ӵ����Ͳ�ƥ��",TokenList.at(iListPos-1));
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
			EmitError("case��֧�����ظ�����",TokenList.at(iListPos-1));
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
 �����б�		-> , ���� 086 �����б�
				-> ��
*/
bool semantic086()
{
	OpInfo Tmp;
	Tmp.m_iType=OpInfo::CONST;
	Tmp.m_iLink=atoi(TokenList.at(iListPos-1).m_szContent.c_str());

	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::CASE)
	{
		EmitError("�������",TokenList.at(iListPos-1));
		return false;
	}

	if (CType::TypeCompatible(Tmp,CurrentStatement.top().m_CaseExp,17)==-1)
	{
		EmitError("case�Ӿ�ĳ�����ѡ���ӵ����Ͳ�ƥ��",TokenList.at(iListPos-1));
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
			EmitError("case��֧�����ظ�����",TokenList.at(iListPos-1));
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
 case��֧	-> �����б� 087 : ��� 088 ; case��֧
			-> ��
*/
bool semantic087()
{
	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::CASE)
	{
		EmitError("�������",TokenList.at(iListPos-1));
		return false;
	}

	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.push_back(
				EmitIR(OpType::LABEL,CurrentStatement.top().m_Labels.back().m_Label));

	return true;
}

/*
 case��֧	-> �����б� 087 : ��� 088 ; case��֧
			-> ��
*/
bool semantic088()
{
	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::CASE)
	{
		EmitError("�������",TokenList.at(iListPos-1));
		return false;
	}

	SymbolTbl.ProcInfoTbl.at(SymbolTbl.ProcStack.top()).m_Codes.push_back(
				EmitIR(OpType::JMP,CurrentStatement.top().GetLabel(Statement::LabelIdx::ExitLabel)));

	return true;

}


//�������	-> case ���ʽ 084 of case��֧ end 089
bool semantic089()
{
	if (CurrentStatement.empty()
		|| CurrentStatement.top().m_eType!=Statement::CASE)
	{
		EmitError("�������",TokenList.at(iListPos-1));
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
		EmitError("CASE����ѡ���ӱ���Ϊ��������",TokenList.at(iListPos-1));
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

//�������	-> with 090 ���� 053 091 do ��� 092
bool semantic090()
{
	bWithFlag=true;
	return true;
}

//�������	-> with 090 ���� 053 091 do ��� 092
bool semantic091()
{
	bWithFlag=false;

	if (TmpWithField.m_Var.m_VarTypeStack.empty()
		|| TmpWithField.m_Var.m_VarTypeStack.top().m_StoreType!=StoreType::T_RECORD)
	{
		EmitError("With���ֻ���������Ϊ��¼����",TokenList.at(iListPos-1));
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

//�������	-> with 090 ���� 053 091 do ��� 092
bool semantic092()
{
	if (WithStack.empty())
	{
		EmitError("�������",TokenList.at(iListPos-1));
		return false;
	}
	WithStack.pop();

	return true;
}

//�����ļ�˵��	-> uses 093 ��ʶ���б� 094 ;
bool semantic093()
{
	iIdListFlag.push(7);
	return true;
}

//�����ļ�˵��	-> uses 093 ��ʶ���б� 094 ;
bool semantic094()
{
	iIdListFlag.pop();
	return true;
}

//�������֣�	-> 095 ���̺�����������
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
			EmitError(szFileName.append("�ļ�������").c_str(),TokenList.at(iListPos-1));
			return false;
		}
	}
	return true;
}

//����			-> [ 096 ���ʽ�б� 097 ]
bool semantic096()
{
	OpInfo Tmp;
	iExpListFlag.push(2);	//������
	iExpListNum.push(0);
	return true;
}

//����			-> [ 096 ���ʽ�б� 097 ]
bool semantic097()
{
	iExpListFlag.pop();
	stack<OpInfo> TmpConstItem,TmpVarItem;

	if (Operand.size()<iExpListNum.top())
	{
		EmitError("�������",TokenList.at(iListPos-1));
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
			EmitError("����Ԫ�ز�����Ϊ�������������ݣ����Ҽ���Ԫ��ֵ�������0..255",TokenList.at(iListPos-1));
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
			EmitError("����Ԫ�ز�����Ϊ�������������ݣ����Ҽ���Ԫ��ֵ�������0..255",TokenList.at(iListPos-1));
			return false;
		}
	}

	iExpListNum.pop();
	return true;
}

//������	-> [ 098 ���ʽ�б� 099 ] 059 ������
bool semantic098()
{
	iExpListFlag.push(1);	//���������±����
	return true;
}

//������	-> [ 098 ���ʽ�б� 099 ] 059 ������
bool semantic099()
{
	iExpListFlag.pop();
	return true;
}

//����		-> nil 100
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

//���̵������	-> ( 101 ʵ���б� ) 066
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

//��������		-> ��ʶ�� 102 = ���ʽ 004
bool semantic102()
{
	return true;
}

bool GenIR(OpInfo Op1,OpInfo Op2,OpInfo Rslt,BasicOpType Op)
{
	int iTmp=CType::TypeCompatible(Op1,Op2,Op);
	if (iTmp==-1)
	{
		EmitError("���������Ͳ�ƥ��",TokenList.at(iListPos-1));
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
		EmitError("���������Ͳ�ƥ��",TokenList.at(iListPos-1));
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
				EmitError("��¼���������Ͳ�ƥ�䣬���Ͳ�����",TokenList.at(iListPos-1));
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
				EmitError("ö�����Ͳ�ƥ��",TokenList.at(iListPos-1));
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
				EmitError("��������Ϊ0",TokenList.at(iListPos-1));
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
				EmitError("��������Ϊ0",TokenList.at(iListPos-1));
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
				EmitError("��������Ϊ0",TokenList.at(iListPos-1));
				return false;
			}
			if (Op1Const.m_iVal<0)
			{
				EmitError("����ģ��������С��0",TokenList.at(iListPos-1));
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
				EmitError("��λ��ֻ�ܽ���0~255֮��",TokenList.at(iListPos-1));
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
				EmitError("��λ��ֻ�ܽ���0~255֮��",TokenList.at(iListPos-1));
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
				EmitError("ö�����Ͳ�����",TokenList.at(iListPos-1));
				return false;
			}
			RsltConst.m_bVal=Op1Const.m_szName.compare(Op2Const.m_szName)==0;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 37:
		{
			if (Op1Const.m_iEnumIdx!=Op2Const.m_iEnumIdx)
			{
				EmitError("ö�����Ͳ�����",TokenList.at(iListPos-1));
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
				EmitError("����Ԫ��ֻ�ܽ���0~255֮��",TokenList.at(iListPos-1));
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
				EmitError("����Ԫ��ֻ�ܽ���0~255֮��",TokenList.at(iListPos-1));
				return false;
			}
			RsltConst.m_bVal=Op2Const.m_szSet.at(Op1Const.m_szVal[0])=='1';
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 44:
		{
			if (Op1Const.m_iEnumIdx!=Op2Const.m_iEnumIdx)
			{
				EmitError("ö�����Ͳ�����",TokenList.at(iListPos-1));
				return false;
			}
			RsltConst.m_bVal=Op1Const.m_iVal>Op2Const.m_iVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 45:
		{
			if (Op1Const.m_iEnumIdx!=Op2Const.m_iEnumIdx)
			{
				EmitError("ö�����Ͳ�����",TokenList.at(iListPos-1));
				return false;
			}
			RsltConst.m_bVal=Op1Const.m_iVal>=Op2Const.m_iVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 46:
		{
			if (Op1Const.m_iEnumIdx!=Op2Const.m_iEnumIdx)
			{
				EmitError("ö�����Ͳ�����",TokenList.at(iListPos-1));
				return false;
			}
			RsltConst.m_bVal=Op1Const.m_iVal<Op2Const.m_iVal;
			RsltConst.m_szName=RsltConst.m_bVal?"TRUE":"FALSE";
		};break;
	case 47:
		{
			if (Op1Const.m_iEnumIdx!=Op2Const.m_iEnumIdx)
			{
				EmitError("ö�����Ͳ�����",TokenList.at(iListPos-1));
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
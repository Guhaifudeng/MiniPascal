#include "Semantic.h"
///*****************����***********************///
stack<int> iTypePos;//����ջ
stack<int> iIdListFlag;//��ʶ���б�
stack<int> iTypeFlag;//���ͱ�־
bool bForwardFlag;//�ⲿ�ļ�����
stack<string> szVarFieldFlag;
stack<string> szVarFieldConst;
int iEnumSize;//ö�����ͱ�ָ��
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
//����ͷ -> program ��ʶ�� 001 ( ��ʶ���б� ) ;
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
	iIdListFlag.push(0);//��ʶ���б�
	iTypeFlag.push(0);
	return true;
}

//���� -> ����ͷ ����� 002 .
bool semantic002()//�Ƿ����δ����ȴ��ʹ�õı�ţ�ȫ�֣�
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
		StoreType::CStoreType TmpStoreType=CType::TokenToEnum(TokenList.at(iListPos-1).m_iKind);
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
		Tmp.m_iParaType=-1;//�β�����

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
bool semantic041()//���ù���--����
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

		TypeInfo Tmp1;//��������
		Tmp1.m_iLink=SymbolTbl.ProcInfoTbl.size();
		Tmp1.m_eDataType=StoreType::T_PROC;
		Tmp1.m_szName="_noname";
		Tmp1.m_szName.append(GetSerialId());
		SymbolTbl.AddType(Tmp1);
		iTypePos.push(SymbolTbl.TypeInfoTbl.size()-1);
		SymbolTbl.ProcInfoTbl.push_back(Tmp);

		VarInfo TmpVar;//��֤��������ͻ
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

		TypeInfo Tmp1;//��������
		Tmp1.m_iLink=SymbolTbl.ProcInfoTbl.size();
		Tmp1.m_eDataType=StoreType::T_FUNC;
		Tmp1.m_szName="_noname";
		Tmp1.m_szName.append(GetSerialId());
		SymbolTbl.AddType(Tmp1);
		iTypePos.push(SymbolTbl.TypeInfoTbl.size()-1);
		SymbolTbl.ProcInfoTbl.push_back(Tmp);

		VarInfo TmpVar;//��֤��������ͻ
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

		StoreType::CStoreType ParaType=SymbolTbl.TypeInfoTbl[CType::GetRealType(Tmp.m_iTypeLink)].m_eDataType;

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
    /*
	for (i=0;i<SymbolTbl.TypeInfoTbl.size();i++)//������ű�ÿ��������ռ�ֽڣ����������Ѱַ
	{
		SymbolTbl.CalcTypeSize(i);
	}
    */
	if (!SymbolTbl.PtrCheck(i))//���ͻ���
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


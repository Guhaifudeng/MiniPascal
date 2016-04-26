#include "Type.h"

/********************************************************************************
������   : InitTypeSysTbl
����     : ��ʼ��������Ϣ��
����     :
����ֵ   :
********************************************************************************/
extern CSymbolTbl SymbolTbl;
extern CState State;
void CType::InitTypeSysTbl()
{
	int i;

	string szTypeStr = FileRead(State.m_szSysDir + "TypeSys.txt");

	for(i = 0 ; i < szTypeStr.size() ; i = i + 35)
	{
		string szTmp = szTypeStr.substr(i,35);//7���ֶΣ�����������������1����������2��������͡��������������м�������͡�ת������1��ת������2
		TypeSysInfo Tmp;
		Tmp.m_iOp = atoi(szTmp.substr(1,2).c_str());
		Tmp.m_iOp1Type = atoi(szTmp.substr(4,2).c_str());
		Tmp.m_iOp2Type = atoi(szTmp.substr(7,2).c_str());
		Tmp.m_iRsltType = atoi(szTmp.substr(10,2).c_str());
		Tmp.m_iVarProcessId = atoi(szTmp.substr(13,4).c_str());
		Tmp.m_iProcessId = atoi(szTmp.substr(18,4).c_str());
		Tmp.m_iIR = atoi(szTmp.substr(23,3).c_str());
		Tmp.m_Convert1 = atoi(szTmp.substr(27,3).c_str());
		Tmp.m_Convert2 = atoi(szTmp.substr(31,3).c_str());
		SymbolTbl.TypeSysTbl.push_back(Tmp);
	}
}

/********************************************************************************
������   : TokenToEnum
����     : ���ݵ��ʣ�����õ���Ӧ������ö��ֵ
����     : [in]	 iToken          :���ʵ�ID
����ֵ   : ���͵�ö��ֵ
********************************************************************************/
StoreType::CStoreType CType::TokenToEnum(int iToken)
{
	switch (iToken)
	{
		case 49:return StoreType::T_CHAR;		break;
		case 44:return StoreType::T_BOOLEAN;	break;
		case 64:return StoreType::T_INTEGER;	break;
		case 45:return StoreType::T_BYTE;		break;
		case 79:return StoreType::T_SHORTINT;	break;
		case 82:return StoreType::T_SMALLINT;	break;
		case 92:return StoreType::T_WORD;		break;
		case 66:return StoreType::T_LONGWORD;	break;
		case 47:return StoreType::T_CARDINAL;	break;
		case 74:return StoreType::T_REAL;		break;
		case 81:return StoreType::T_SINGLE;		break;
		case 83:return StoreType::T_STRING;		break;
	}
}

/********************************************************************************
������   : GetTypeSize
����     : �������͵�ö��ֵ������õ���Ӧ�����͵�sizeֵ
����     : [in]	 iToken          :���ʵ�ID
����ֵ   : ���͵�sizeֵ
********************************************************************************/
int CType::GetTypeSize(StoreType::CStoreType eStoreType)
{
	switch (eStoreType)
	{
	case StoreType::T_BYTE:
	case StoreType::T_CHAR:
	case StoreType::T_BOOLEAN:
	case StoreType::T_SHORTINT:	return 1;

	case StoreType::T_SMALLINT:
	case StoreType::T_WORD:
	case StoreType::T_ENUM:		return 2;

	case StoreType::T_CARDINAL:
	case StoreType::T_FILE:
	case StoreType::T_FUNC:
	case StoreType::T_INTEGER:
	case StoreType::T_LONGWORD:
	case StoreType::T_POINTER:
	case StoreType::T_PROC:
	case StoreType::T_STRING:
	case StoreType::T_SINGLE:	return 4;

	case StoreType::T_REAL:
	case StoreType::T_LONG8:	return 8;

	case StoreType::T_SET:		return 32;
	}
}

/********************************************************************************
������   : ProcessConstType
����     : �������泣����ֵ������������ʵ������
����     : [in]	 iPos          :������ID
����ֵ   :
********************************************************************************/
void CType::ProcessConstType(int iPos)
{
	if (SymbolTbl.ConstInfoTbl[iPos].m_StoreType != StoreType::T_NONE)
	{
		return;
	}
	string szTmpVal = SymbolTbl.ConstInfoTbl[iPos].m_szVal;

	switch (SymbolTbl.ConstInfoTbl[iPos].m_ConstType)
	{
		case ConstType::INTEGER:
		{
			int i=atoi(szTmpVal.c_str());

			if (i<0)
			{
				if (i >= -128 && i <= 127)
				{
					SymbolTbl.ConstInfoTbl[iPos].m_StoreType = StoreType::T_SHORTINT;
				}
				else if (i >= -32768 && i <= 32767)
				{
					SymbolTbl.ConstInfoTbl[iPos].m_StoreType = StoreType::T_SMALLINT;
				}
				else if (i >= -2147483648 && i <= 2147483647)
				{
					SymbolTbl.ConstInfoTbl[iPos].m_StoreType = StoreType::T_INTEGER;
				}
			}
			else
			{
				if (i<=255)
					SymbolTbl.ConstInfoTbl[iPos].m_StoreType=StoreType::T_BYTE;
				else
					if (i<=65535)
						SymbolTbl.ConstInfoTbl[iPos].m_StoreType=StoreType::T_WORD;
					else
						if (i<=4294967295)
							SymbolTbl.ConstInfoTbl[iPos].m_StoreType=StoreType::T_LONGWORD;
			}
			SymbolTbl.ConstInfoTbl[iPos].m_iVal=i;
			SymbolTbl.ConstInfoTbl[iPos].m_fVal=SymbolTbl.ConstInfoTbl[iPos].m_iVal;
		};break;
		case ConstType::BOOLEAN:
			 {
				 SymbolTbl.ConstInfoTbl[iPos].m_StoreType=StoreType::T_BOOLEAN;
				 if (SymbolTbl.ConstInfoTbl[iPos].m_szVal.compare("TRUE")==0)
					 SymbolTbl.ConstInfoTbl[iPos].m_bVal=true;
				 else
					 SymbolTbl.ConstInfoTbl[iPos].m_bVal=false;
			 };break;
	 	case ConstType::STRING:
			{
				if (szTmpVal.length()==1)
					SymbolTbl.ConstInfoTbl[iPos].m_StoreType=StoreType::T_CHAR;
				else
					SymbolTbl.ConstInfoTbl[iPos].m_StoreType=StoreType::T_STRING;
			};break;
		case ConstType::EREAL:
		case ConstType::REAL:
			{
				SymbolTbl.ConstInfoTbl[iPos].m_fVal=atof(SymbolTbl.ConstInfoTbl[iPos].m_szVal.c_str());
				if (SymbolTbl.ConstInfoTbl[iPos].m_fVal>1.5e-45 && SymbolTbl.ConstInfoTbl[iPos].m_fVal<3.4e38)
					SymbolTbl.ConstInfoTbl[iPos].m_StoreType=StoreType::T_SINGLE;
				else
					SymbolTbl.ConstInfoTbl[iPos].m_StoreType=StoreType::T_REAL;

			};break;

	}
}

int CType::GetOpTypeLink(OpInfo Op)//�õ�����������ָ��
{
	if (Op.m_iType==OpInfo::CONST)
	{
		if (SymbolTbl.ConstInfoTbl[Op.m_iLink].m_ConstType=ConstType::ENUM)
			return SymbolTbl.ConstInfoTbl[Op.m_iLink].m_iEnumIdx;
		else
			return -1;
	}
	if (Op.m_iType==OpInfo::NONE)
	{
		return -1;
	}
	if (Op.m_iType==OpInfo::VAR)//����������
	{
		int i;
		if (!Op.m_iDetailType.empty())//����m_iDetailTypeջ��Ԫ��
		{
			i=Op.m_iDetailType.top().m_iLink;
			while (SymbolTbl.TypeInfoTbl[i].m_eDataType==StoreType::T_USER)
			{
				i=SymbolTbl.TypeInfoTbl[i].m_iLink;
			}
			return i;
		}
		//����m_iLink��ȡ������Ϣ�����
		if (SymbolTbl.VarInfoTbl[Op.m_iLink].m_eRank==VarInfo::PARA)
			i=SymbolTbl.ProcInfoTbl.at(SymbolTbl.VarInfoTbl[Op.m_iLink].m_iProcIndex).m_ParaTbl.at(SymbolTbl.VarInfoTbl[Op.m_iLink].m_iTypeLink).m_iParaType;
		else
			i=SymbolTbl.VarInfoTbl[Op.m_iLink].m_iTypeLink;
		while (SymbolTbl.TypeInfoTbl[i].m_eDataType==StoreType::T_USER)
		{
			i=SymbolTbl.TypeInfoTbl[i].m_iLink;
		}
		return i;
	}
}
StoreType::CStoreType CType::GetOpType(OpInfo Op)//�õ�������ʵ������
{
	if (Op.m_iType==OpInfo::CONST)
		return SymbolTbl.ConstInfoTbl[Op.m_iLink].m_StoreType;
	if (Op.m_iType==OpInfo::NONE)
		return StoreType::T_NONE;
	if (Op.m_iType==OpInfo::PTR)
		return StoreType::T_POINTER;
	if (Op.m_iType==OpInfo::VAR)
	{
		int i;
		if (!Op.m_iDetailType.empty())
		{
			i=Op.m_iDetailType.top().m_iLink;
			while (SymbolTbl.TypeInfoTbl[i].m_eDataType==StoreType::T_USER)
				i=SymbolTbl.TypeInfoTbl[i].m_iLink;
			return SymbolTbl.TypeInfoTbl[i].m_eDataType;
		}
		i=SymbolTbl.VarInfoTbl[Op.m_iLink].m_iTypeLink;//����m_iLink��ȡ������Ϣ�����
		while (SymbolTbl.TypeInfoTbl[i].m_eDataType==StoreType::T_USER)
			i=SymbolTbl.TypeInfoTbl[i].m_iLink;
		return SymbolTbl.TypeInfoTbl[i].m_eDataType;
	}
}

int CType::TypeCompatible( OpInfo Op1,OpInfo Op2,int Op)
{
	StoreType::CStoreType Op1Type,Op2Type;
	Op1Type=GetOpType(Op1);
	Op2Type=GetOpType(Op2);
	return SymbolTbl.SearchTypeSysTbl(Op,(int)Op1Type,(int)Op2Type);
}

int CType::TypeCompatible( OpInfo Op1,int Op)//������
{
	OpInfo Tmp;
	Tmp.m_iType=OpInfo::NONE;//��ʶ�ñ���δ��ʹ��
	return TypeCompatible(Op1,Tmp,Op);
}

int CType::GetRealType(int iTypeLink)//ʵ����������
{
	int i=iTypeLink;
	while (i>=0 && SymbolTbl.TypeInfoTbl[i].m_eDataType==StoreType::T_USER )
		i=SymbolTbl.TypeInfoTbl[i].m_iLink;
	return i;

}

bool CType::IsOrd(OpInfo Op)//�Ƿ�Ϊ���ͻ��ַ�����
{
	if (Op.m_iType==OpInfo::CONST)//�������͡��ַ�����
	{
		if (IsInt(SymbolTbl.ConstInfoTbl[Op.m_iLink].m_StoreType) || SymbolTbl.ConstInfoTbl[Op.m_iLink].m_StoreType==StoreType::T_CHAR)
			return true;
	}
	if (Op.m_iType==OpInfo::VAR)
	{
		int i;
		if (!Op.m_iDetailType.empty())
			i=Op.m_iDetailType.top().m_iLink;
		else
			i=SymbolTbl.VarInfoTbl[Op.m_iLink].m_iTypeLink;
		if (IsInt(i) || SymbolTbl.TypeInfoTbl[GetRealType(i)].m_eDataType==StoreType::T_CHAR)
			return true;
	}
	return false;
}
bool CType::IsSetItem(OpInfo Op)
{
	if (Op.m_iType==OpInfo::CONST && IsOrd(Op) &&
		(SymbolTbl.ConstInfoTbl[Op.m_iLink].m_iVal>=0 &&//����ֵ 32B-32���ֽڣ�һ���ֽ�8λ
		SymbolTbl.ConstInfoTbl[Op.m_iLink].m_iVal<=255) ||
		SymbolTbl.ConstInfoTbl[Op.m_iLink].m_StoreType==StoreType::T_CHAR)
	{
		return true;
	}
	return Op.m_iType==OpInfo::VAR && IsOrd(Op);

}
bool CType::IsInt(int iTypeLink)
{
	StoreType::CStoreType eStoreType=(StoreType::CStoreType)SymbolTbl.TypeInfoTbl[GetRealType(iTypeLink)].m_eDataType;
	if (eStoreType==StoreType::T_INTEGER || eStoreType==StoreType::T_BYTE || eStoreType==StoreType::T_LONGWORD ||
		eStoreType==StoreType::T_SHORTINT || eStoreType==StoreType::T_SMALLINT || eStoreType==StoreType::T_WORD)
		return true;
	return false;
}


bool CType::IsInt(StoreType::CStoreType eStoreType)//����ʵ�������ж��Ƿ�Ϊ����
{
	if (eStoreType==StoreType::T_INTEGER || eStoreType==StoreType::T_BYTE || eStoreType==StoreType::T_LONGWORD ||
		eStoreType==StoreType::T_SHORTINT || eStoreType==StoreType::T_SMALLINT || eStoreType==StoreType::T_WORD)
		return true;
	return false;
}
VarType CType::getVarTypeStackElement(stack<VarType> VarTypeStack, int position)
{
        VarType result = VarTypeStack.top();
        VarTypeStack.pop();
        if (VarTypeStack.size() == position)
        {
//            stack.push(result);
            return result;
        }else {
            VarType element = getVarTypeStackElement(VarTypeStack, position);
            VarTypeStack.push(element);
            return element;
        }
}
bool CType::IsOffsetVar(stack<VarType> VarTypeStack)//
{
	for(int i=VarTypeStack.size()-1;i < 0;i--){
		if (getVarTypeStackElement(VarTypeStack,i).m_StoreType==StoreType::T_ARRAY ||
			getVarTypeStackElement(VarTypeStack,i).m_StoreType==StoreType::T_RECORD)
			return true;
	}
	return false;
}


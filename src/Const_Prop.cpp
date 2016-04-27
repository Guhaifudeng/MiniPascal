#include "Const_Prop.h"


CConst_Prop::CConst_Prop(void)
{
}

CConst_Prop::~CConst_Prop(void)
{
}

bool CConst_Prop::GenConstOp(OpInfo &Op,int iProcIndex)
{
	bool first=true;
	int ConstOpType=-1;
	OpInfo* TmpOp;
	ConstInfo TmpConstInfo;

	for(int i=0;i<Op.m_udChain.size();i++)
	{
		IRCode* TmpIR=&SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes[Op.m_udChain[i]];

		Opti_Tbl* TmpOpti;

		if (TmpIR->m_eOpType<0)
		{
			TmpOpti=SearchOptiTbl(TmpIR->m_eOpType);
		}
		else if (TmpIR->m_eOpType>>4>=21)
		{
			TmpOpti=SearchOptiTbl(TmpIR->m_eOpType);
		}
		else if (TmpIR->m_eOpType>>4<21)
		{
			TmpOpti=SearchOptiTbl((OpType)(TmpIR->m_eOpType>>4<<4));
		}

		if (TmpOpti!=NULL && TmpOpti->eConstPropType!=Opti_Tbl::None)
		{
			if (TmpIR->m_Op1.m_iType==OpInfo::CONST)
			{
				if (TmpIR->m_Rslt.m_bRef)
					return false;

				if (first)
				{
					TmpConstInfo=SymbolTbl.ConstInfoTbl[TmpIR->m_Op1.m_iLink];
					TmpOp=&TmpIR->m_Op1;
					ConstOpType=TmpOpti->eConstPropType;
					first=false;
				}
				else
				{
					if (TmpConstInfo.m_ConstType!=SymbolTbl.ConstInfoTbl[TmpIR->m_Op1.m_iLink].m_ConstType)
					{
						return false;
					}
					switch (TmpConstInfo.m_ConstType)
					{
					case ConstType::BOOLEAN:
					case ConstType::PTR:
					case ConstType::SET:
					case ConstType::STRING:
						if (TmpConstInfo.m_szName!=SymbolTbl.ConstInfoTbl[TmpIR->m_Op1.m_iLink].m_szName)
						{
							return false;
						}
						break;
					default:
						if (fabs(TmpConstInfo.m_fVal-SymbolTbl.ConstInfoTbl[TmpIR->m_Op1.m_iLink].m_fVal)>10e-30)
						{
							return false;
						}
						break;
					}
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}

	}
	if (ConstOpType==-1)
	{
		return false;
	}

	Op.m_iType=OpInfo::CONST;

	if (ConstOpType==Opti_Tbl::ToAssign)
	{
		Op.m_iLink=TmpOp->m_iLink;
	}
	else
	{
		Op.m_iLink=SymbolTbl.ConstInfoTbl.size();
	}

	Op.m_udChain.clear();

	switch(ConstOpType)
	{
	case Opti_Tbl::ToInt:
		TmpConstInfo.m_ConstType=ConstType::INTEGER;
		TmpConstInfo.m_StoreType=StoreType::T_INTEGER;
		break;
	case Opti_Tbl::ToReal:
		TmpConstInfo.m_ConstType=ConstType::REAL;
		TmpConstInfo.m_StoreType=StoreType::T_REAL;
		break;
	case Opti_Tbl::ToSingle:
		TmpConstInfo.m_ConstType=ConstType::REAL;
		TmpConstInfo.m_StoreType=StoreType::T_SINGLE;
		break;
	case Opti_Tbl::ToSmall:
		TmpConstInfo.m_ConstType=ConstType::INTEGER;
		TmpConstInfo.m_StoreType=StoreType::T_SMALLINT;
		break;
	case Opti_Tbl::ToWord:
		TmpConstInfo.m_ConstType=ConstType::INTEGER;
		TmpConstInfo.m_StoreType=StoreType::T_WORD;
		break;
	case Opti_Tbl::ToLong:
		TmpConstInfo.m_ConstType=ConstType::INTEGER;
		TmpConstInfo.m_StoreType=StoreType::T_LONGWORD;
		break;
	case Opti_Tbl::ToShort:
		TmpConstInfo.m_ConstType=ConstType::INTEGER;
		TmpConstInfo.m_StoreType=StoreType::T_SHORTINT;
		break;
	case Opti_Tbl::ToByte:
		TmpConstInfo.m_ConstType=ConstType::INTEGER;
		TmpConstInfo.m_StoreType=StoreType::T_BYTE;
		break;
	case Opti_Tbl::ToLong8:
		TmpConstInfo.m_ConstType=ConstType::INTEGER;
		TmpConstInfo.m_StoreType=StoreType::T_LONG8;
		break;
	}

	SymbolTbl.AddConst(TmpConstInfo);
	bOptiChanged=true;

	return true;
}

void CConst_Prop::Const_Prop(int iProcIndex)
{
	bool bChange=true;

	while (bChange)
	{
		bChange=false;

		for (int i=0;i<SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes.size();i++)
		{
			IRCode* TmpIR=&SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes[i];
			Opti_Tbl* TmpOpti;

			if (TmpIR->m_eOpType<0)
			{
				TmpOpti=SearchOptiTbl(TypeCast);
			}
			else if (TmpIR->m_eOpType>>4>=21)
			{
				TmpOpti=SearchOptiTbl(TmpIR->m_eOpType);
			}
			else if (TmpIR->m_eOpType>>4<21)
			{
				TmpOpti=SearchOptiTbl((OpType)(TmpIR->m_eOpType>>4<<4));
			}

			if (TmpOpti!=NULL && TmpOpti->eExpType!=Opti_Tbl::None)
			{
				if (TmpIR->m_Op2.m_iType==OpInfo::VAR
					&& !TmpIR->m_Op2.m_bRef
					&& !TmpIR->m_Op2.m_bUninit)
				{
					bChange|=GenConstOp(TmpIR->m_Op2,iProcIndex);
				}

				if (TmpIR->m_Op1.m_iType==OpInfo::VAR
					&& !TmpIR->m_Op1.m_bRef
					&& !TmpIR->m_Op1.m_bUninit)
				{
					bChange|=GenConstOp(TmpIR->m_Op1,iProcIndex);
				}
			}
		}
	}
}

void CConst_Prop::Func_Const_Prop()
{
	for (int i=0;i<SymbolTbl.ProcInfoTbl.size();i++)
	{
		if (SymbolTbl.ProcInfoTbl[i].m_bUsed==false)
		{
			continue;
		}

		if (SymbolTbl.ProcInfoTbl[i].m_Codes.size()>0)
		{
			Const_Prop(i);
		}
	}
}
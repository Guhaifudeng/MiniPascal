#include "Copy_Prop.h"


CCopy_Prop::CCopy_Prop(void)
{
}

CCopy_Prop::~CCopy_Prop(void)
{
}

bool CCopy_Prop::GenCopyOp(OpInfo &Op,int iProcIndex)
{
	bool first=true;
	int iVal=-1;

	for(int i=0;i<Op.m_udChain.size();i++)
	{
		IRCode* TmpIR=&SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes[Op.m_udChain[i]];

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

		if (TmpOpti!=NULL && TmpOpti->eConstPropType!=Opti_Tbl::None)
		{
			if (TmpIR->m_Op1.m_iType==OpInfo::VAR)
			{
				if (TmpIR->m_Op1.m_bRef || TmpIR->m_Rslt.m_bRef)
				{
					return false;
				}

				if (TmpIR->m_Op1.m_iLink == TmpIR->m_Rslt.m_iLink
					&& TmpIR->m_Op1.m_iType == TmpIR->m_Rslt.m_iType
					&& TmpIR->m_Op1.m_iType == OpInfo::VAR)
				{
					return false;
				}

				if (first)
				{
					iVal=TmpIR->m_Op1.m_iLink;

					first=false;
				}
				else if (iVal!=TmpIR->m_Op1.m_iLink)
				{
					return false;
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

	if ( iVal==-1)
	{
		return false;
	}

	Op.m_udChain.clear();
	Op.m_iLink=iVal;

	bOptiChanged=true;

	return true;
}

void CCopy_Prop::Copy_Prop(int iProcIndex)
{
	bool bChange=true;

	while (bChange)
	{
		bChange=false;
		for (int i=0;i<SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes.size();i++)
		{
			IRCode* TmpIR=&SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes[i];
			Opti_Tbl* TmpOpti;

			if (TmpIR->m_eOpType==ASM)
			{
				continue;
			}

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
				if (TmpIR->m_Op2.m_iType==OpInfo::VAR && !TmpIR->m_Op2.m_bRef)
				{
					bChange|=GenCopyOp(TmpIR->m_Op2,iProcIndex);
				}
				if (TmpIR->m_Op1.m_iType==OpInfo::VAR && !TmpIR->m_Op1.m_bRef)
				{
					bChange|=GenCopyOp(TmpIR->m_Op1,iProcIndex);
				}
			}
		}
	}
}
/*
void CCopy_Prop::Tmp_Prop(int iProcIndex)
{
	bool bChange=true;

	while (bChange)
	{
		bChange=false;

		for (int i=0;i<SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes.size();i++)
		{
			IRCode* TmpIR=&SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes[i];

			if (TmpIR->m_eOpType<0)
			{
				continue;
			}

			if (TmpIR->m_eOpType>>4==20
				&& TmpIR->m_Op1.m_bRef==false
				&& TmpIR->m_Op1.m_iType==OpInfo::VAR
				&& SymbolTbl.IsTmpVar(TmpIR->m_Op1.m_iLink)
				&& !SymbolTbl.IsTmpVar(TmpIR->m_Rslt.m_iLink))
			{
				if (VarDef[itos(i)+"$"+itos(TmpIR->m_Op1.m_iLink)].size()!=1
					|| VarAllUse[itos(i)+"$"+itos(TmpIR->m_Op1.m_iLink)].size()!=1
					|| VarAllUse[itos(i)+"$"+itos(TmpIR->m_Op1.m_iLink)].at(0)-1
					!=VarDef[itos(i)+"$"+itos(TmpIR->m_Op1.m_iLink)].at(0))
				{
					continue;
				}

				SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes[i-1].m_Rslt=TmpIR->m_Rslt;
				TmpIR->m_bEnabled=false;
				bChange=true;
			}
		}

		if (bChange)
		{
			CDeadCodeElimination::DeleteCode(iProcIndex);
		}
	}
}
*/
void CCopy_Prop::Func_Copy_Prop()
{
	for (int i=0;i<SymbolTbl.ProcInfoTbl.size();i++)
	{
		if (SymbolTbl.ProcInfoTbl[i].m_bUsed==false)
		{
			continue;
		}

		if (SymbolTbl.ProcInfoTbl[i].m_Codes.size()>0)
		{
			//Tmp_Prop(i);//¡Ÿ ±±‰¡ø

			Copy_Prop(i);
		}
	}
}
#include "Const_Fold.h"


CConst_Fold::CConst_Fold(void)
{
}

CConst_Fold::~CConst_Fold(void)
{
}
void CConst_Fold::Const_Fold(int iProcIndex)
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

			if (TmpOpti!=NULL
				&& (TmpOpti->eExpType==Opti_Tbl::Exp1_2_3
				|| TmpOpti->eExpType==Opti_Tbl::Exp1_3))
			{
				if (TmpIR->m_eOpType==ASM
					|| TmpIR->m_eOpType==CALLPTR)
				{
					continue;
				}

				if (TmpOpti->eExpType==Opti_Tbl::Exp1_2_3)
				{
					if (!(TmpIR->m_Op1.m_iType==OpInfo::CONST
						&& TmpIR->m_Op2.m_iType==OpInfo::CONST))
					{
						continue;
					}
				}

				if (TmpOpti->eExpType==Opti_Tbl::Exp1_3)
				{
					if (!(TmpIR->m_Op1.m_iType==OpInfo::CONST))
					{
						continue;
					}
				}

				if (TmpIR->m_Rslt.m_bRef)
				{
					continue;
				}

				OpInfo Rslt,Op2,Tmp;

				int TmpResult=-1;

				if (TmpOpti->eExpType==Opti_Tbl::Exp1_2_3)
				{
					Op2=TmpIR->m_Op2;
					TmpResult=CType::TypeCompatible(TmpIR->m_Op1,Op2,TmpOpti->eOpToken);
				}

				if (TmpOpti->eExpType==Opti_Tbl::Exp1_3)
				{
					Tmp.m_iType=OpInfo::NONE;
					Op2=Tmp;
					TmpResult=CType::TypeCompatible(TmpIR->m_Op1,TmpOpti->eOpToken);
				}

				if (TmpResult!=-1)
				{
					if (!OpConstFold(TmpIR->m_Op1,Op2,SymbolTbl.TypeSysTbl.at(TmpResult).m_iProcessId
							,SymbolTbl.TypeSysTbl.at(TmpResult).m_iRsltType,Rslt))
					{
						continue;
					}

					TmpResult=CType::TypeCompatible(Rslt,TmpIR->m_Rslt,MOV);

					if (TmpResult!=-1)
					{
						TmpIR->m_eOpType=(OpType)SymbolTbl.TypeSysTbl.at(TmpResult).m_iIR;
						TmpIR->m_Op1=Rslt;
						TmpIR->m_Op2.m_iType=OpInfo::NONE;
						bOptiChanged=true;
						bChange=true;
					}
				}
			}
		}
	}
}
void CConst_Fold::Func_Const_Fold()
{
	for (int i=0;i<SymbolTbl.ProcInfoTbl.size();i++)
	{
		if (SymbolTbl.ProcInfoTbl[i].m_bUsed==false)
		{
			continue;
		}

		Const_Fold(i);
	}
}
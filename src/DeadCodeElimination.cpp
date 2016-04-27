#include "DeadCodeElimination.h"


CDeadCodeElimination::CDeadCodeElimination(void)
{
}

CDeadCodeElimination::~CDeadCodeElimination(void)
{
}
void CDeadCodeElimination::DeadCodeElimination(int iProcIndex)
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

			if (TmpOpti!=NULL && TmpOpti->eDeadCode!=Opti_Tbl::None)
			{
				if (TmpIR->m_Rslt.m_iType==OpInfo::VAR
					&& !TmpIR->m_Rslt.m_bRef
					&& SymbolTbl.VarInfoTbl[TmpIR->m_Rslt.m_iLink].m_eRank==VarInfo::VAR
					&& (SymbolTbl.VarInfoTbl[TmpIR->m_Rslt.m_iLink].m_iProcIndex != 0
					|| SymbolTbl.IsTmpVar(TmpIR->m_Rslt.m_iLink))
					&& TmpIR->m_Rslt.m_duChain.size()==0)
				{
					bChange=true;
					bOptiChanged=true;
					TmpIR->m_bEnabled=false;
				}
			}
		}

		if (bChange)
		{
			DeleteCode(iProcIndex);
		}
	}
}
void CDeadCodeElimination::DeleteCode(int iProcIndex)
{
	vector<IRCode> TmpCode;

	for (int i=0;i<SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes.size();i++)
	{
		if (SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes[i].m_bEnabled)
		{
			TmpCode.push_back(SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes[i]);
		}
	}

	SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes=TmpCode;

	CDataFlowAnalysis::DataFlowAnalysis();
}

void CDeadCodeElimination::DeadJumpElimination(int iProcIndex)
{
	bool bFlag=false;

	for (int i=0;i<SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes.size();i++)
	{
		IRCode* TmpIR=&SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes[i];

		if (TmpIR->m_eOpType==JMP
			|| TmpIR->m_eOpType==JT
			|| TmpIR->m_eOpType==JNT
			|| TmpIR->m_eOpType==JE_1
			|| TmpIR->m_eOpType==JE_2
			|| TmpIR->m_eOpType==JE_4)
		{

			OpInfo *Op = NULL;

			if (TmpIR->m_eOpType == JMP)
			{
				Op=&TmpIR->m_Op1;
			}
			else
			{
				Op=&TmpIR->m_Rslt;
			}

			if (i<SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes.size()-1)
			{
				IRCode* TmpIR1=&SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes[i+1];

				if (TmpIR1->m_eOpType==LABEL
					&& SymbolTbl.LabelInfoTbl[TmpIR1->m_Op1.m_iLink].m_szName
					==SymbolTbl.LabelInfoTbl[Op->m_iLink].m_szName)
				{
					bFlag=true;
					TmpIR->m_bEnabled=false;
					bOptiChanged=true;
				}
			}
		}
	}

	if (bFlag)
	{
		DeleteCode(iProcIndex);
	}
}

set<int> ReachableBlock;
set<int> VisitedBlock;

void ReachPath(vector<CBasicBlock>* pBbs,int iPos)
{
	if (VisitedBlock.count(iPos)!=0)
	{
		return;
	}
	else
	{
		VisitedBlock.insert(iPos);
	}

	vector<int>::iterator it=pBbs->at(iPos).DownFlow.begin();

	for(;it!=pBbs->at(iPos).DownFlow.end();it++)
	{
		ReachableBlock.insert(*it);
		ReachPath(pBbs,*it);
	}
}
void CDeadCodeElimination::DeadBlockElimination(int iProcIndex)
{

	vector<CBasicBlock>* pBbs=&BasicBlock[iProcIndex];
	ReachableBlock.clear();
	VisitedBlock.clear();
	ReachPath(pBbs,0);

	if (ReachableBlock.size()!=pBbs->size())
	{
		for (int i=1;i<pBbs->size();i++)
		{
			if (ReachableBlock.count(i)==0)
			{
				for(int j=pBbs->at(i).iStart;j<=pBbs->at(i).iEnd;j++)
				{
					SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes[j].m_bEnabled=false;
				}

				bOptiChanged=true;
			}
		}
		DeleteCode(iProcIndex);
	}
}

void CDeadCodeElimination::Func_RemoveDeadCode()
{
	for (int i=0;i<SymbolTbl.ProcInfoTbl.size();i++)
	{
		if (SymbolTbl.ProcInfoTbl[i].m_bUsed==false)
		{
			continue;
		}

		if (SymbolTbl.ProcInfoTbl[i].m_Codes.size()>0)
		{
			DeadCodeElimination(i);
		}

		if (SymbolTbl.ProcInfoTbl[i].m_Codes.size()>0)
		{
			DeadBlockElimination(i);
		}

		if (SymbolTbl.ProcInfoTbl[i].m_Codes.size()>0)
		{
			DeadJumpElimination(i);
		}
	}
}

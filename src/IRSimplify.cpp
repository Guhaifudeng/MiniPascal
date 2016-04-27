#include "IRSimplify.h"



CIRSimplify::CIRSimplify(void)
{

}

CIRSimplify::~CIRSimplify(void)
{
}

void CIRSimplify::AlgebraicSimplify(int iProcIndex)
{
	if (Num2Bit.empty())
	{
		int j=1;
		for(int i=1;i<31;i++)
		{
			j=j*2;
			Num2Bit.insert(pair<int,int>(j,i));
		}
	}
	for (int i=0;i<SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes.size();i++)
	{   //获取处理方案
		IRCode* TmpIR=&SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes[i];
		Opti_Tbl* TmpOpti;
		if (TmpIR->m_eOpType<0)
			TmpOpti=SearchOptiTbl(TypeCast);
		else if (TmpIR->m_eOpType>>4>=21)
			TmpOpti=SearchOptiTbl(TmpIR->m_eOpType);
		else if (TmpIR->m_eOpType>>4<21)
			TmpOpti=SearchOptiTbl((OpType)(TmpIR->m_eOpType));
		if (TmpOpti!=NULL && TmpOpti->eAlgebraicProcess!=Opti_Tbl::None)
		{

			if (TmpOpti->eCommutative==Opti_Tbl::Commutative)
			{
				if (TmpIR->m_Op1.m_iType==OpInfo::CONST)
				{
					OpInfo TmpOp=TmpIR->m_Op1;
					TmpIR->m_Op1=TmpIR->m_Op2;
					TmpIR->m_Op2=TmpOp;
				}
			}

			if ((TmpIR->m_eOpType>>4<<4)==ADDI)
			{
				if (TmpIR->m_Op2.m_iType==OpInfo::CONST && SymbolTbl.ConstInfoTbl[TmpIR->m_Op2.m_iLink].m_fVal==0)
				{
					TmpIR->m_eOpType=(OpType)TmpOpti->eAlgebraicProcess;
					TmpIR->m_Op2.m_iType=OpInfo::NONE;
					bOptiChanged=true;
					continue;
				}
			}
			if ((TmpIR->m_eOpType>>4<<4)==SUBI)
			{
				if (TmpIR->m_Op2.m_iType==OpInfo::CONST && SymbolTbl.ConstInfoTbl[TmpIR->m_Op2.m_iLink].m_fVal==0)
				{
					TmpIR->m_eOpType=(OpType)TmpOpti->eAlgebraicProcess;
					TmpIR->m_Op2.m_iType=OpInfo::NONE;
					bOptiChanged=true;
					continue;
				}
			}
			if ((TmpIR->m_eOpType>>4<<4)==DIVI || (TmpIR->m_eOpType>>4<<4)==MOD || (TmpIR->m_eOpType>>4<<4)==MULI )
			{
				if (TmpIR->m_Op2.m_iType==OpInfo::CONST && SymbolTbl.ConstInfoTbl[TmpIR->m_Op2.m_iLink].m_fVal==1)
				{
					TmpIR->m_eOpType=(OpType)TmpOpti->eAlgebraicProcess;
					TmpIR->m_Op2.m_iType=OpInfo::NONE;
					bOptiChanged=true;
					continue;
				}
			}
			if ((TmpIR->m_eOpType==MUL_4 || TmpIR->m_eOpType==DIV_4) && (TmpIR->m_Op2.m_iType==OpInfo::CONST))
			{
				map<int,int>::iterator it=Num2Bit.find(SymbolTbl.ConstInfoTbl[TmpIR->m_Op2.m_iLink].m_iVal);
				if (it!=Num2Bit.end())
				{
					ConstInfo TmpConst=SymbolTbl.ConstInfoTbl[TmpIR->m_Op2.m_iLink];
					TmpConst.m_iVal=it->second;
					TmpConst.m_szName=itos(it->second);
					TmpIR->m_Op2.m_iLink=SymbolTbl.RecConstTbl(TmpConst);
					if (TmpIR->m_eOpType==MUL_4)
						TmpIR->m_eOpType=SHL_4;
					else
						TmpIR->m_eOpType=SHR_4;
					continue;
				}
			}
			if ((TmpIR->m_eOpType>>4<<4)==SHL || (TmpIR->m_eOpType>>4<<4)==SHR)
			{
				if (TmpIR->m_Op2.m_iType==OpInfo::CONST && SymbolTbl.ConstInfoTbl[TmpIR->m_Op2.m_iLink].m_iVal==0)
				{
					TmpIR->m_eOpType=(OpType)TmpOpti->eAlgebraicProcess;
					TmpIR->m_Op2.m_iType=OpInfo::NONE;
					bOptiChanged=true;
					continue;
				}
			}
			if ((TmpIR->m_eOpType>>4<<4)==OR)
			{
				if (TmpIR->m_Op2.m_iType==OpInfo::VAR && TmpIR->m_Op1.m_iType==OpInfo::VAR && TmpIR->m_Op1.m_iLink==TmpIR->m_Op1.m_iLink)
				{
					TmpIR->m_eOpType=(OpType)TmpOpti->eAlgebraicProcess;
					TmpIR->m_Op2.m_iType=OpInfo::NONE;
					bOptiChanged=true;
					continue;
				}
				if (TmpIR->m_Op2.m_iType==OpInfo::CONST && SymbolTbl.ConstInfoTbl[TmpIR->m_Op2.m_iLink].m_bVal==false)
				{
					TmpIR->m_eOpType=(OpType)TmpOpti->eAlgebraicProcess;
					TmpIR->m_Op2.m_iType=OpInfo::NONE;
					bOptiChanged=true;
					continue;
				}
				if ((TmpIR->m_Op2.m_iType==OpInfo::CONST && SymbolTbl.ConstInfoTbl[TmpIR->m_Op2.m_iLink].m_bVal==true))
				{
					TmpIR->m_eOpType=(OpType)TmpOpti->eAlgebraicProcess;
					TmpIR->m_Op1=TmpIR->m_Op2;
					TmpIR->m_Op2.m_iType=OpInfo::NONE;
					bOptiChanged=true;
					continue;
				}
			}
			if ((TmpIR->m_eOpType>>4<<4)==AND)
			{
				if (TmpIR->m_Op2.m_iType==OpInfo::VAR && TmpIR->m_Op1.m_iType==OpInfo::VAR && TmpIR->m_Op1.m_iLink==TmpIR->m_Op1.m_iLink)
				{
					TmpIR->m_eOpType=(OpType)TmpOpti->eAlgebraicProcess;
					TmpIR->m_Op2.m_iType=OpInfo::NONE;
					bOptiChanged=true;
					continue;
				}
				if (TmpIR->m_Op2.m_iType==OpInfo::CONST && SymbolTbl.ConstInfoTbl[TmpIR->m_Op2.m_iLink].m_bVal==true)
				{
					TmpIR->m_eOpType=(OpType)TmpOpti->eAlgebraicProcess;
					TmpIR->m_Op2.m_iType=OpInfo::NONE;
					bOptiChanged=true;
					continue;
				}
				if (TmpIR->m_Op2.m_iType==OpInfo::CONST && SymbolTbl.ConstInfoTbl[TmpIR->m_Op2.m_iLink].m_bVal==false)
				{
					TmpIR->m_eOpType=(OpType)TmpOpti->eAlgebraicProcess;
					TmpIR->m_Op1=TmpIR->m_Op2;
					TmpIR->m_Op2.m_iType=OpInfo::NONE;
					bOptiChanged=true;
					continue;
				}
			}

		}
	}
}
void CIRSimplify::IRSimplify(int iProcIndex)//条件跳转优化
{
	vector<IRCode> TmpCodes;
	bool bFlag=false;
	for (int i=0;i<SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes.size();i++)
	{
		IRCode* TmpIR=&SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes[i];
		if (TmpIR->m_eOpType==JNT)
		{
			if (TmpIR->m_Op1.m_iType==OpInfo::CONST)
			{
				if (SymbolTbl.ConstInfoTbl[TmpIR->m_Op1.m_iLink].m_bVal==false)
				{
					TmpIR->m_eOpType=JMP;
					TmpIR->m_Op1=TmpIR->m_Rslt;
					TmpIR->m_Rslt.m_iType=OpInfo::NONE;
					bOptiChanged=true;
				}
				else
				{
					bFlag=true;
					continue;
				}
			}
		}
		if (TmpIR->m_eOpType==JT)
		{
			if (TmpIR->m_Op1.m_iType==OpInfo::CONST)
			{
				if (SymbolTbl.ConstInfoTbl[TmpIR->m_Op1.m_iLink].m_bVal==true)
				{
					TmpIR->m_eOpType=JMP;
					TmpIR->m_Op1=TmpIR->m_Rslt;
					TmpIR->m_Rslt.m_iType=OpInfo::NONE;
					bOptiChanged=true;
				}
				else
				{
					bFlag=true;
					continue;
				}
			}
		}
		TmpCodes.push_back(*TmpIR);
	}
	if (bFlag)
		SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes=TmpCodes;
}

void CIRSimplify::JumpSimplify(int iProcIndex)//连续跳转优化
{
	vector<IRCode> TmpCodes;
	bool bFlag=false;
	map<int,int> LabelMap;
	set<int> bHasVisit;
	for (int i=0;i<SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes.size();i++)
	{
		IRCode* TmpIR=&SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes[i];
		if (TmpIR->m_eOpType==LABEL)
			LabelMap.insert(pair<int,int>(TmpIR->m_Op1.m_iLink,i));
	}
	for (int i=0;i<SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes.size();i++)
	{
		bHasVisit.clear();
		IRCode* TmpIR=&SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes[i];
		if (TmpIR->m_eOpType>>4!=21)
			continue;
		Opti_Tbl* TmpOpti=SearchOptiTbl(TmpIR->m_eOpType);
		if (TmpOpti->eJmpType==Opti_Tbl::CondJmp || TmpOpti->eJmpType==Opti_Tbl::NonCondJmp)
		{
			int j=TmpOpti->eJmpType==Opti_Tbl::NonCondJmp?TmpIR->m_Op1.m_iLink:TmpIR->m_Rslt.m_iLink;
			while (bHasVisit.find(j)==bHasVisit.end())
			{
				int k=LabelMap.find(j)->second;
				while (SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes[k].m_eOpType==LABEL)
				{
					bHasVisit.insert(SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes[k].m_Op1.m_iLink);
					k++;
					if (k==SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes.size())
					{
						break;
					}
				}

				if (k==SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes.size() ||
					SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes[k].m_eOpType!=JMP)
				{
					if (TmpOpti->eJmpType==Opti_Tbl::NonCondJmp)
						TmpIR->m_Op1=SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes[k-1].m_Op1;
					else
						TmpIR->m_Rslt=SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes[k-1].m_Op1;
				}
				else
					j=SymbolTbl.ProcInfoTbl.at(iProcIndex).m_Codes[k].m_Op1.m_iLink;
			}

		}
	}
}
void CIRSimplify::Func_IRSimplify()
{
	for (int i=0;i<SymbolTbl.ProcInfoTbl.size();i++)
	{
		if (SymbolTbl.ProcInfoTbl[i].m_bUsed==false)
			continue;
		if (SymbolTbl.ProcInfoTbl[i].m_Codes.size()>0)
		{
			IRSimplify(i);
			JumpSimplify(i);
			AlgebraicSimplify(i);
		}
	}
}


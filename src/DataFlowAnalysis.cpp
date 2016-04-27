#include "DataFlowAnalysis.h"

static bool bFirst;

map<int,int> VarMap;//（流图变量序号-变量信息表序号）---（变量信息表序号-流图变量序号）

CDataFlowAnalysis::CDataFlowAnalysis()
{

}
CDataFlowAnalysis::~CDataFlowAnalysis()
{

}

void CDataFlowAnalysis::DataFlowAnalysis()
{
	COptimization::InitOpti();

	for (int i=0;i<SymbolTbl.ProcInfoTbl.size();i++)
	{
		if (SymbolTbl.ProcInfoTbl[i].m_bUsed==false)
		{
			continue;
		}

		VarMap=GetVar2IdMap(i);
		GetBasicBlock(i);
		Def_Use_Analysis(i);
		In_Out_Analysis(i);
		ud_du_Analysis(i);
	}

	for (int i=1;i<SymbolTbl.ProcInfoTbl.size();i++)
	{
		SymbolTbl.ProcInfoTbl[i].m_bUsed=SymbolTbl.ProcInfoTbl[i].m_bUsed || SymbolTbl.ProcInfoTbl[i].m_bTmpUsed;
	}
}

void CDataFlowAnalysis::PrintDefUse()
{

	for (int i=0;i<SymbolTbl.ProcInfoTbl.size();i++)
	{
		if (SymbolTbl.ProcInfoTbl[i].m_bUsed==false)
			continue;

		map<int,int> VarMap=GetVar2IdMap(i);

		vector<CBasicBlock>* pBbs=&BasicBlock[i];

		for(int j=0;j<pBbs->size();j++)
		{
			cout<<"\n第"<<j<<"块：\n";
			cout<<"\n\tDef：";

			for (map<int,int>::iterator it=VarMap.begin();it!=VarMap.end();it++)
			{
				if (pBbs->at(j).Def->value(it->second))
				{
					cout<<SymbolTbl.VarInfoTbl[it->first].m_szName<<",";
				}
			}
			cout<<"\n\tUse：";

			for (map<int,int>::iterator it=VarMap.begin();it!=VarMap.end();it++)
			{
				if (pBbs->at(j).Use->value(it->second))
				{
					cout<<SymbolTbl.VarInfoTbl[it->first].m_szName<<",";
				}
			}
			cout<<"\n\tInSet：";

			for (map<int,int>::iterator it=VarMap.begin();it!=VarMap.end();it++)
			{
				if (pBbs->at(j).InSet->value(it->second))
				{
					cout<<SymbolTbl.VarInfoTbl[it->first].m_szName<<",";
				}
			}
			cout<<"\n\tOutSet：";

			for (map<int,int>::iterator it=VarMap.begin();it!=VarMap.end();it++)
			{
				if (pBbs->at(j).OutSet->value(it->second))
				{
					cout<<SymbolTbl.VarInfoTbl[it->first].m_szName<<",";
				}
			}

		}
	}

	cout<<"\n变量定义点：\n";

	for(map<string,vector<int> >::iterator it=VarDef.begin();it!=VarDef.end();it++)
	{
		cout<<SymbolTbl.VarInfoTbl[GetVarLink(it->first)].m_szName<<":";

		for (int i=0;i<it->second.size();i++)
		{
			cout<<it->second.at(i)<<",";
		}

		cout<<"\n";
	}

	cout<<"\n变量全程引用点：\n";

	for(map<string,vector<int> >::iterator it=VarAllUse.begin();it!=VarAllUse.end();it++)
	{
		cout<<SymbolTbl.VarInfoTbl[GetVarLink(it->first)].m_szName<<":";

		for (int i=0;i<it->second.size();i++)
		{
			cout<<it->second.at(i)<<",";
		}

		cout<<"\n";
	}
}
void CDataFlowAnalysis::In_Out_Analysis(int iProcIndex)
{
	CBits InitBit(VarMap.size());

	vector<CBasicBlock>* pBbs=&BasicBlock[iProcIndex];

	bool bChange=true;

	CBits NewInSet(VarMap.size());

	for(int i=0;i<pBbs->size();i++)
	{
		if (!pBbs->at(i).InSet)
		{
			delete pBbs->at(i).InSet;
		}

		if (!pBbs->at(i).OutSet)
		{
			delete pBbs->at(i).OutSet;
		}

		pBbs->at(i).InSet=new CBits(InitBit);
		pBbs->at(i).OutSet=new CBits(InitBit);
	}

	while (bChange)
	{
		bChange=false;

		for (int i=0;i<pBbs->size();i++)
		{
			CBits Tmp(VarMap.size());

			for (int j=0;j<pBbs->at(i).DownFlow.size();j++)
			{
				Tmp.Or(pBbs->at(pBbs->at(i).DownFlow[j]).InSet);
			}

			pBbs->at(i).OutSet->copy(&Tmp);
			Tmp.Sub(pBbs->at(i).Def);
			Tmp.Or(pBbs->at(i).Use);
			NewInSet.copy(&Tmp);

			if(!(NewInSet==*(pBbs->at(i).InSet)))
			{
				bChange=true;
				pBbs->at(i).InSet->copy(&NewInSet);
			}
		}
	}
}

int CDataFlowAnalysis::ForwardHasDef(int iVar,int iStart,int iEnd,int iProcIndex)
{
	vector<int> tmp;
	map<string,vector<int> >::iterator it=VarDef.find(itos(iProcIndex)+"$"+itos(iVar));
	if (it==VarDef.end() || iStart>iEnd)
		return -1;
	else
	{
		int i;
		for(i=it->second.size()-1;i>=0;i--)
		{
			if (it->second[i]<iStart)
				return -1;
			if (it->second[i]>=iStart && it->second[i]<=iEnd)
				break;
		}
		if (i!=-1)
			return it->second[i];
		else
			return -1;
	}
}
int CDataFlowAnalysis::BackwardHasDef(int iVar,int iStart,int iEnd,int iProcIndex)
{
	vector<int> tmp;
	map<string,vector<int> >::iterator it=VarDef.find(itos(iProcIndex)+"$"+itos(iVar));
	if (it==VarDef.end() || iStart>iEnd)
		return -1;
	else
	{
		int i;
		for(i=0;i<it->second.size();i++)
		{
			if (it->second[i]>iEnd)
				return -1;
			if (it->second[i]>=iStart && it->second[i]<=iEnd)
				break;
		}
		if (i!=it->second.size())
			return it->second[i];
		else
			return -1;
	}
}
vector<int> CDataFlowAnalysis::GetDu(int iVar,int iBb,int iStart,int iProcIndex)
{
	vector<int> tmp;
	int i,iEnd;

	if (bHasVisit.find(iBb)!=bHasVisit.end())
	{
		return tmp;
	}
	else
	{
		if (bFirst)
		{
			bFirst=false;
		}
		else
		{
			bHasVisit.insert(iBb);
		}
	}

	i=BackwardHasDef(iVar,iStart+1,CurrentBasicBlock->at(iBb).iEnd,iProcIndex);

	iEnd=i==-1?CurrentBasicBlock->at(iBb).iEnd:i;

	vector<int> TmpDu=VarAllUse[itos(iProcIndex)+"$"+itos(iVar)];

	for(int j=0;j<TmpDu.size();j++)
	{
		if (iStart<TmpDu[j] && TmpDu[j]<=iEnd)
		{
			tmp.push_back(TmpDu[j]);
		}
	}

	if (i!=-1)
	{
		return tmp;
	}
	else
	{
		for(int j=0;j<CurrentBasicBlock->at(iBb).DownFlow.size();j++)
		{
			int i=CurrentBasicBlock->at(iBb).DownFlow[j];
			vector<int> DownFlowDu=GetDu(iVar,i,CurrentBasicBlock->at(i).iStart-1,iProcIndex);

			if (!DownFlowDu.empty())
			{
				tmp.insert(tmp.end(),DownFlowDu.begin(),DownFlowDu.end());
			}
		}
	}

	return tmp;
}

vector<int> CDataFlowAnalysis::GetUd(int iVar,int iBb,int iEnd,int iProcIndex,bool &bUninit)
{
	vector<int> tmp;
	int i;

	if (bHasVisit.find(iBb)!=bHasVisit.end())
	{
		return tmp;
	}
	else
	{
		if (bFirst)
		{
			bFirst=false;
		}
		else
		{
			bHasVisit.insert(iBb);
		}
	}

	i=ForwardHasDef(iVar,CurrentBasicBlock->at(iBb).iStart,iEnd-1,iProcIndex);

	if (i==-1 && iBb==0)
	{
		bUninit=true;
	}

	if (i!=-1)
	{
		tmp.push_back(i);
		return tmp;
	}
	else
	{
		for(int j=0;j<CurrentBasicBlock->at(iBb).UpFlow.size();j++)
		{
			int i=CurrentBasicBlock->at(iBb).UpFlow[j];

			vector<int> UpFlowUd=GetUd(iVar,i,CurrentBasicBlock->at(i).iEnd+1,iProcIndex,bUninit);

			if (!UpFlowUd.empty())
			{
				tmp.insert(tmp.end(),UpFlowUd.begin(),UpFlowUd.end());
			}
		}
	}

	return tmp;
}

void CDataFlowAnalysis::ud_du_Analysis(int iProcIndex)
{
	vector<CBasicBlock>* pBbs=&BasicBlock[iProcIndex];

	IRCode* TmpIR;

	for(int i=0;i<SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes.size();i++)
	{
		SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes[i].m_Op1.m_udChain.clear();
		SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes[i].m_Op1.m_duChain.clear();
		SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes[i].m_Op2.m_udChain.clear();
		SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes[i].m_Op2.m_duChain.clear();
		SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes[i].m_Rslt.m_udChain.clear();
		SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes[i].m_Rslt.m_duChain.clear();
		SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes[i].m_Op1.m_bUninit=false;
		SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes[i].m_Op2.m_bUninit=false;
		SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes[i].m_Rslt.m_bUninit=false;
	}

	for(int i=0;i<pBbs->size();i++)
	{
		CurrentBasicBlock=pBbs;

		for(int j=pBbs->at(i).iStart;j<=pBbs->at(i).iEnd;j++)
		{
			TmpIR=&SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes[j];
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
				if (TmpIR->m_eOpType==ASM)
				{
					continue;
				}

				if (TmpIR->m_Op2.m_iType==OpInfo::VAR && !TmpIR->m_Op2.m_bRef)
				{
					bHasVisit.clear();
					bFirst=true;
					TmpIR->m_Op2.m_udChain=GetUd(TmpIR->m_Op2.m_iLink,i,j,iProcIndex,TmpIR->m_Op2.m_bUninit);
				}

				if (TmpIR->m_Op1.m_iType==OpInfo::VAR && !TmpIR->m_Op1.m_bRef)
				{
					bHasVisit.clear();
					bFirst=true;
					TmpIR->m_Op1.m_udChain=GetUd(TmpIR->m_Op1.m_iLink,i,j,iProcIndex,TmpIR->m_Op1.m_bUninit);
				}

				if (TmpIR->m_Rslt.m_iType==OpInfo::VAR && !TmpIR->m_Rslt.m_bRef)
				{
					bHasVisit.clear();
					bFirst=true;
					TmpIR->m_Rslt.m_duChain=GetDu(TmpIR->m_Rslt.m_iLink,i,j,iProcIndex);
				}
			}
		}
	}
}


void CDataFlowAnalysis::VarDefUse(map<string,vector<int> > &v,int VarPos,int CodePos,int iProcIndex)
{
	vector<int> tmp;
	map<string,vector<int> >::iterator it=v.find(itos(iProcIndex)+"$"+itos(VarPos));

	if (it!=v.end())
	{
		it->second.push_back(CodePos);
	}
	else
	{
		tmp.push_back(CodePos);
		v.insert(pair<string,vector<int> >(itos(iProcIndex)+"$"+itos(VarPos),tmp));
	}

}
void CDataFlowAnalysis::Def_Use_Analysis(int iProcIndex)
{
	vector<int> RefVar;
	CBits InitBit(VarMap.size());
	vector<CBasicBlock>* pBbs=&BasicBlock[iProcIndex];

	for (int i=0;i<SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes.size();i++)
	{
		if (SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes[i].m_eOpType==GETADDR)
		{
			RefVar.push_back(SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes[i].m_Op1.m_iLink);
		}
	}

	for(int i=0;i<pBbs->size();i++)
	{
		if (!pBbs->at(i).Def)
		{
			delete pBbs->at(i).Def;
		}

		pBbs->at(i).Def=new CBits(VarMap.size());

		if (!pBbs->at(i).Use)
		{
			delete pBbs->at(i).Use;
		}

		pBbs->at(i).Use=new CBits(VarMap.size());
	}

	for(int i=0;i<pBbs->size();i++)
	{
		CBits Def(VarMap.size());
		CBits Use(VarMap.size());

		for(int j=pBbs->at(i).iStart;j<=pBbs->at(i).iEnd;j++)
		{

			IRCode* TmpIR=&SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes[j];

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

			if (TmpIR->m_eOpType==CALL
				|| TmpIR->m_eOpType==GETPROCADDR)
			{
				SymbolTbl.ProcInfoTbl[TmpIR->m_Op1.m_iLink].m_bTmpUsed=true;
			}

			if (TmpIR->m_eOpType==CALL
				|| TmpIR->m_eOpType==CALLPTR)
			{
				if (TmpIR->m_eOpType!=CALL
					|| SymbolTbl.ProcInfoTbl[TmpIR->m_Op1.m_iLink].m_eFlag!=ProcInfo::Extern)
				{
					//函数调用点，对全局变量既是引用又是定值
					map<int,VarInfo>::iterator it=SymbolTbl.VarInfoTbl.begin();

					for(;it!=SymbolTbl.VarInfoTbl.end();it++)
					{
						//这里只处理用户全局变量

						if (it->second.m_iProcIndex!=0)
						{
							continue;
						}

						if (SymbolTbl.IsTmpVar(it->first))
						{
							continue;
						}

						OpInfo TmpOp;
						TmpOp.m_iType=OpInfo::VAR;
						TmpOp.m_iLink=it->first;
						TmpOp.m_bRef=false;

						RecordUse(VarMap,Use,Def,TmpOp,RefVar,j,iProcIndex);	//设置use点

						if (SymbolTbl.TypeInfoTbl[it->second.m_iTypeLink].m_eDataType!=StoreType::T_PROC
							&& SymbolTbl.TypeInfoTbl[it->second.m_iTypeLink].m_eDataType!=StoreType::T_FUNC)
						{
							RecordDef(VarMap,Use,Def,TmpOp,RefVar,j,iProcIndex);//设置def点
						}
					}
				}
			}

			if (TmpOpti!=NULL && TmpOpti->eExpType!=Opti_Tbl::None)
			{
				if (TmpIR->m_eOpType==ASM)
				{
					//内嵌汇编形式
					for(int k=TmpIR->m_Op2.m_iLink;k<=TmpIR->m_Rslt.m_iLink;k++)
					{
						OpInfo TmpOp;
						TmpOp.m_iType=OpInfo::VAR;
						TmpOp.m_iLink=SymbolTbl.AsmParaTbl[k].m_iLink;
						TmpOp.m_bRef=false;
						switch (SymbolTbl.AsmParaTbl[k].flag)
						{
						case AsmPara::RW:
							RecordUse(VarMap,Use,Def,TmpOp,RefVar,j,iProcIndex);
							RecordDef(VarMap,Use,Def,TmpOp,RefVar,j,iProcIndex);
							break;
						case AsmPara::W:
							RecordDef(VarMap,Use,Def,TmpOp,RefVar,j,iProcIndex);
							break;
						case AsmPara::R:
							RecordUse(VarMap,Use,Def,TmpOp,RefVar,j,iProcIndex);
							break;
						}
					}
					continue;
				}

				if (TmpIR->m_Op2.m_iType==OpInfo::VAR )
				{
					//Op2的引用点
					RecordUse(VarMap,Use,Def,TmpIR->m_Op2,RefVar,j,iProcIndex);
				}

				if (TmpIR->m_Op1.m_iType==OpInfo::VAR )
				{
					//Op1的引用点
					RecordUse(VarMap,Use,Def,TmpIR->m_Op1,RefVar,j,iProcIndex);

					//Op1是间接访问的实参，则当前是可能定值点
					if (TmpIR->m_eOpType==PARA
						&& SymbolTbl.VarInfoTbl[TmpIR->m_Rslt.m_iLink].m_bRef)
					{
						RecordDef(VarMap,Use,Def,TmpIR->m_Op1,RefVar,j,iProcIndex);
					}
				}

				if (TmpIR->m_eOpType!=PARA
					&& TmpIR->m_Rslt.m_iType==OpInfo::VAR )
				{
					//Rslt必定是定值点
					RecordDef(VarMap,Use,Def,TmpIR->m_Rslt,RefVar,j,iProcIndex);
				}
			}
		}
		pBbs->at(i).Def->copy(&Def);

		pBbs->at(i).Use->copy(&Use);
	}

	for(map<string,vector<int> >::iterator it=VarDef.begin();it!=VarDef.end();it++)
	{
		sort(it->second.begin(),it->second.end());
	}

	for(map<string,vector<int> >::iterator it=VarAllUse.begin();it!=VarAllUse.end();it++)
	{
		sort(it->second.begin(),it->second.end());
	}
}

void CDataFlowAnalysis::RecordDef(map<int,int>VarMap,CBits &Use,CBits &Def,OpInfo Op,vector<int> RefVar,int iPos,int iProcIndex)
{
	if (!Op.m_bRef)
	{
		//直接访问定值，只需记录其定值点
		Def.set(VarMap[Op.m_iLink]);
		VarDefUse(VarDef,Op.m_iLink,iPos,iProcIndex);
	}
	else
	{
		//间接访问定值，则指针变量是引用点
		VarDefUse(VarAllUse,Op.m_iLink,iPos,iProcIndex);

		if (!Def.value(VarMap[Op.m_iLink]))
		{
			//如果是块内有定值点的引用点是不影响活跃分析的
			Use.set(VarMap[Op.m_iLink]);
		}

		//所有的被引用变量是定值点
		for(int p=0;p<RefVar.size();p++)
		{
			Def.set(VarMap[RefVar.at(p)]);
			VarDefUse(VarDef,RefVar.at(p),iPos,iProcIndex);
		}
	}
}

void CDataFlowAnalysis::RecordUse(map<int,int>VarMap,CBits &Use,CBits &Def,OpInfo Op,vector<int> RefVar,int iPos,int iProcIndex)
{
	VarDefUse(VarAllUse,Op.m_iLink,iPos,iProcIndex);

	if (!Def.value(VarMap[Op.m_iLink]))
	{
		Use.set(VarMap[Op.m_iLink]);
	}

	if (Op.m_bRef)	//如果是间接访问，那么该use点也是所有被引用变量的use点
	{
		for(int p=0;p<RefVar.size();p++)
		{
			Use.set(VarMap[RefVar.at(p)]);
			VarDefUse(VarAllUse,RefVar.at(p),iPos,iProcIndex);
		}
	}
}

void CDataFlowAnalysis::GetBasicBlock(int iProcIndex)
{
	vector<CBasicBlock> TmpBasicBlocks;
	map<int,int> Lbl_Block;
	int iStart=0;
	map<int,int> ReWrite;
	int i;

	for(i=0;i<SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes.size();i++)
	{
		CBasicBlock TmpBb;
		IRCode * TmpIR=&SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes[i];
		Opti_Tbl* TmpOpti=SearchOptiTbl(TmpIR->m_eOpType);

		if (TmpOpti!=NULL && TmpOpti->eJmpType!=Opti_Tbl::None)
		{
			switch(TmpOpti->eJmpType)
			{
			case Opti_Tbl::Lbl:
				{
					if (i!=0 && i!=iStart)
					{
						TmpBb.iStart=iStart;
						TmpBb.iEnd=i-1;

						if (i<SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes.size())
						{
							TmpBb.DownFlow.push_back(TmpBasicBlocks.size()+1);
						}

						TmpBasicBlocks.push_back(TmpBb);

						iStart=i;
					}

					Lbl_Block.insert(pair<int,int>
									(TmpIR->m_Op1.m_iLink,TmpBasicBlocks.size()));
				};
				break;

			case Opti_Tbl::CondJmp:
			case Opti_Tbl::NonCondJmp:
				{
					TmpBb.iStart=iStart;
					TmpBb.iEnd=i;

					int iLbl=(TmpOpti->eJmpType==Opti_Tbl::CondJmp)?
								TmpIR->m_Rslt.m_iLink:TmpIR->m_Op1.m_iLink;

					map<int,int>::iterator it=Lbl_Block.find(iLbl);

					if (it!=Lbl_Block.end())
					{
						TmpBb.DownFlow.push_back(it->second);
					}
					else
					{
						ReWrite.insert(pair<int,int>(TmpBasicBlocks.size(),iLbl));
					}

					if (TmpOpti->eJmpType==Opti_Tbl::CondJmp)
					{
						if (i!=SymbolTbl.ProcInfoTbl[iProcIndex].m_Codes.size()-1)
						{
							TmpBb.DownFlow.push_back(TmpBasicBlocks.size()+1);
						}
					}

					TmpBasicBlocks.push_back(TmpBb);
					iStart=i+1;
				}
				break;
			}
		}
	}

	if (iStart<i)
	{
		CBasicBlock TmpBb;
		TmpBb.iStart=iStart;
		TmpBb.iEnd=i-1;
		TmpBasicBlocks.push_back(TmpBb);
	}

	for(map<int,int>::iterator it=ReWrite.begin();it!=ReWrite.end();it++)
	{
		TmpBasicBlocks.at(it->first).DownFlow.push_back(Lbl_Block[it->second]);
	}

	for (int i=0;i<TmpBasicBlocks.size();i++)
	{
		for (int j=0;j<TmpBasicBlocks.at(i).DownFlow.size();j++)
		{
			TmpBasicBlocks.at(TmpBasicBlocks.at(i).DownFlow[j]).UpFlow.push_back(i);
		}
	}

	BasicBlock.insert(pair<int,vector<CBasicBlock> >(iProcIndex,TmpBasicBlocks));

	CurrentBasicBlock=&BasicBlock[iProcIndex];

}

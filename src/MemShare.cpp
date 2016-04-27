#include "MemShare.h"

CMemShare::CMemShare(void)
{
}

CMemShare::~CMemShare(void)
{
}

int GetBlock(int iProc,int iLine)
{
	vector<CBasicBlock>* pBbs=&BasicBlock[iProc];

	for(int i=0;i<pBbs->size();i++)
	{
		if (pBbs->at(i).iStart<=iLine && pBbs->at(i).iEnd>=iLine)
		{
			return i;
		}
	}
}

bool TmpInfoCmp(TmpInfo p1,TmpInfo p2)
{
	int i=p1.m_LiveArea.m_iUse-p1.m_LiveArea.m_iDef;

	int j=p2.m_LiveArea.m_iUse-p2.m_LiveArea.m_iDef;

	return (i<j);
}

void CMemShare::TmpMemShare()
{
	vector<TmpInfo> TmpVar;

	map<int,vector<LiveArea> > Conflict;//基本块冲突映射表---基本块指针--基本块内已占用的活跃区域

	for (int i=0;i<SymbolTbl.ProcInfoTbl.size();i++)
	{
		map<int,int> VarMap=GetVar2IdMap(i,false);//获取当前过程的变量集合

		SymbolTbl.ProcInfoTbl.at(i).m_TmpMemShare.clear();//描述该过程的共享集合

		TmpVar.clear();

		Conflict.clear();

		for(map<int,int>::iterator it=VarMap.begin();it!=VarMap.end();it++)
		{
			if(!SymbolTbl.IsTmpVar(it->first))
			{
				continue;
			}

			if (VarDef[itos(i)+"$"+itos(it->first)].size()==1
				&& VarAllUse[itos(i)+"$"+itos(it->first)].size()==1)
			{
				int iDef=VarDef[itos(i)+"$"+itos(it->first)].at(0);
				int iUse=VarAllUse[itos(i)+"$"+itos(it->first)].at(0);

				int iDefBlock=GetBlock(i,iDef);
				int iUseBlock=GetBlock(i,iUse);

				if (iDefBlock==iUseBlock && iDef<=iUse)
				{
					TmpVar.push_back(TmpInfo(it->first,iDef,iUse,iDefBlock));
				}
			}
		}

		sort(TmpVar.begin(),TmpVar.end(),TmpInfoCmp);

		for(vector<TmpInfo>::iterator it=TmpVar.begin();it!=TmpVar.end();it++)
		{
			map<int,vector<LiveArea> >::iterator ConflictIt=Conflict.find(it->m_iBlock);//判断Conflict中是否存在该基本块的活跃区域集合

			if (ConflictIt!=Conflict.end())
			{
				vector<LiveArea>::iterator VecIt=ConflictIt->second.begin();

				for(;VecIt!=ConflictIt->second.end();VecIt++)
				{
					 if (it->m_LiveArea.m_iDef < VecIt->m_iUse
						 && it->m_LiveArea.m_iDef > VecIt->m_iDef)
					 {
						 break;
					 }

					 if (it->m_LiveArea.m_iUse <= VecIt->m_iUse
						 && it->m_LiveArea.m_iUse > VecIt->m_iDef)
					 {
						 break;
					 }

					 if (it->m_LiveArea.m_iUse >= VecIt->m_iUse
						 && it->m_LiveArea.m_iDef <= VecIt->m_iDef)
					 {
						 break;
					 }
				}

				if (VecIt==ConflictIt->second.end())
				{
					ConflictIt->second.push_back(LiveArea(it->m_LiveArea.m_iDef,it->m_LiveArea.m_iUse));
					SymbolTbl.ProcInfoTbl.at(i).m_TmpMemShare.insert(it->m_iLink);
				}
			}
			else
			{
				vector<LiveArea> TmpVec;
				SymbolTbl.ProcInfoTbl.at(i).m_TmpMemShare.insert(it->m_iLink);
				TmpVec.push_back(LiveArea(it->m_LiveArea.m_iDef,it->m_LiveArea.m_iUse));
				Conflict.insert(pair<int,vector<LiveArea> >(it->m_iBlock,TmpVec));
			}
		}
	}
}
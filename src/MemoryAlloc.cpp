#include "MemoryAlloc.h"


VarMem::VarMem(int p1, int p2)
{
	m_iLink=p1;
	m_iSize=p2;
	m_bMaxTmp=false;
}

bool VarMemCmp(VarMem v1,VarMem v2)
{
	return (v1.m_iSize<v2.m_iSize)?true:false;
}

void CMemoryAlloc::MemoryAlloc()
{
	for(int i=0;i<SymbolTbl.TypeInfoTbl.size();i++)
	{
		SymbolTbl.CalcTypeSize(i);
	}

	for(int i=0;i<SymbolTbl.VarInfoTbl.size();i++)
	{
		SymbolTbl.VarInfoTbl[i].m_iMemoryAlloc=-1;
	}

	for(vector<ProcInfo>::iterator it=SymbolTbl.ProcInfoTbl.begin();it!=SymbolTbl.ProcInfoTbl.end();it++)
	{
		for(vector<IRCode>::iterator it1=it->m_Codes.begin();it1!=it->m_Codes.end();it1++)
		{
			if (it1->m_Op1.m_iType==OpInfo::CONST)
			{
				SymbolTbl.ConstInfoTbl[it1->m_Op1.m_iLink].m_bUsed|=true;
			}

			if (it1->m_Op2.m_iType==OpInfo::CONST)
			{
				SymbolTbl.ConstInfoTbl[it1->m_Op2.m_iLink].m_bUsed|=true;
			}

			if (it1->m_Rslt.m_iType==OpInfo::CONST)
			{
				SymbolTbl.ConstInfoTbl[it1->m_Rslt.m_iLink].m_bUsed|=true;
			}
		}
	}

	for(int i=0;i<SymbolTbl.ProcInfoTbl.size();i++)
	{
		if (SymbolTbl.ProcInfoTbl.at(i).m_eFlag!=ProcInfo::None)
		{
			continue;
		}

		vector<VarMem> Tmp;

		int iMaxTmp=-1;

		for(int j=0;j<SymbolTbl.VarInfoTbl.size();j++)
		{
			if (i!=SymbolTbl.VarInfoTbl[j].m_iProcIndex
				|| SymbolTbl.VarInfoTbl[j].m_eRank==VarInfo::PARA )
			{
				continue;
			}

			if (i==0 && SymbolTbl.IsProcVar(SymbolTbl.VarInfoTbl[j].m_szName))
			{
				continue;
			}

			if (SymbolTbl.VarInfoTbl[j].m_eRank==VarInfo::VAR )
			{
				map<string,vector<int> >::iterator it=VarDef.find(itos(i)+"$"+itos(j));

				map<string,vector<int> >::iterator it1=VarAllUse.find(itos(i)+"$"+itos(j));

				if ((it==VarDef.end() || it->second.empty())
					&& (it1==VarAllUse.end() || it1->second.empty()))
				{
					continue;
				}
			}

			Tmp.push_back(VarMem(j,SymbolTbl.TypeInfoTbl[SymbolTbl.VarInfoTbl[j].m_iTypeLink].m_iSize));

			if (SymbolTbl.ProcInfoTbl.at(i).m_TmpMemShare.count(j)>0)
			{
				if (iMaxTmp==-1)
				{
					iMaxTmp=Tmp.size()-1;
					Tmp.at(iMaxTmp).m_bMaxTmp=true;
				}
				else
				{
					if (Tmp.at(Tmp.size()-1).m_iSize>Tmp.at(iMaxTmp).m_iSize)
					{
						Tmp.at(iMaxTmp).m_bMaxTmp=false;
						iMaxTmp=Tmp.size()-1;
						Tmp.at(iMaxTmp).m_bMaxTmp=true;
					}
				}
			}

		}

		sort(Tmp.begin(),Tmp.end(),VarMemCmp);

		int iOffset=0;

		int iMaxTmpOffset=-1;

		int iMaxLink=iMaxTmp!=-1?Tmp.at(iMaxTmp).m_iLink:-1;

		cout<<endl<<SymbolTbl.ProcInfoTbl.at(i).m_szName<<"º¯Êý"<<endl;

		vector<int> TmpAlloc;

		for(int j=0;j<Tmp.size();j++)
		{
			if (SymbolTbl.ProcInfoTbl.at(i).m_TmpMemShare.count(Tmp.at(j).m_iLink)==0
				|| Tmp.at(j).m_bMaxTmp)
			{
				int iEmpty;

				switch(Tmp.at(j).m_iSize)
				{
				case 3:
				case 1:iEmpty=1;break;
				case 2:iEmpty=2;break;
				default:iEmpty=4;break;
				}

				if (iOffset%iEmpty!=0)
				{
					iOffset=iOffset+4-iOffset%iEmpty;	//Æ«ÒÆ
				}

				if (Tmp.at(j).m_bMaxTmp)
				{
					iMaxTmpOffset=iOffset;
				}

				SymbolTbl.VarInfoTbl[Tmp.at(j).m_iLink].m_iMemoryAlloc=iOffset;
				cout<<SymbolTbl.VarInfoTbl[Tmp.at(j).m_iLink].m_szName<<":"<<iOffset<<endl;
				iOffset+=Tmp.at(j).m_iSize;
			}
			else
			{
				TmpAlloc.push_back(j);
			}
		}

		for(int j=0;j<TmpAlloc.size() && iMaxTmpOffset!=-1;j++)
		{
			SymbolTbl.VarInfoTbl[Tmp.at(TmpAlloc.at(j)).m_iLink].m_iMemoryAlloc=-1;
			SymbolTbl.ProcInfoTbl.at(i).m_TmpLink=iMaxLink;
		}

		SymbolTbl.ProcInfoTbl.at(i).m_ValSize=iOffset%4==0?iOffset:iOffset+4-iOffset%4;
	}
}
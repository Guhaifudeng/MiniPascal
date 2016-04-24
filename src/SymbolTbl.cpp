#include "SymbolTbl.h"

//extern map<int,vector<CBasicBlock>> BasicBlock;
extern CSymbolTbl SymbolTbl;
CSymbolTbl::CSymbolTbl(void)
{
}

CSymbolTbl::~CSymbolTbl(void)
{
}

int CSymbolTbl::GetTmpVar(int iProcIndex,StoreType::CStoreType eStoreType)
{
	VarInfo Tmp;
	Tmp.m_iProcIndex=iProcIndex;
	Tmp.m_iTypeLink=TypeInfoTbl.size();
	Tmp.m_szName="_T";
	Tmp.m_szName.append(GetVarId());
	Tmp.m_eRank=VarInfo::VAR;
	AddVar(Tmp);
	TypeInfo TmpType;
	TmpType.m_szName="_noname";
	TmpType.m_szName.append(GetSerialId());
	TmpType.m_eDataType=eStoreType;
	TmpType.m_eBaseType=eStoreType;
	AddType(TmpType);
	return VarInfoTbl.size()-1;
}

int CSymbolTbl::GetTmpVar(int iProcIndex,OpType eOpType)
{
	StoreType::CStoreType Tmp;
	switch((int)eOpType)
	{
	case -1:
		Tmp=StoreType::T_STRING;break;
	case -2:case -3:case -4:case -5:case -27:
		Tmp=StoreType::T_INTEGER;break;
	case -6:case -11:case -14:case -16:case -19:case -21:case -23:
		Tmp=StoreType::T_REAL;break;
	case -7:case -12:case-15:case -17:case -20:case -22:case -41:
		Tmp=StoreType::T_SINGLE;break;
	case -8:case -13:case -34:case -39:
		Tmp=StoreType::T_SMALLINT;break;
	case -9:case -25:case -35:case -40:
		Tmp=StoreType::T_WORD;break;
	case -10:case -18:case -26:case -28:
		Tmp=StoreType::T_LONGWORD;break;
	case -24:case -29:case -30:case -31:case -32:
		Tmp=StoreType::T_SHORTINT;break;
	case -33:case -36:case -37:case -38:
		Tmp=StoreType::T_BYTE;break;
	case -42:case -43:case -44:case -45:case -46:case -47:
		Tmp=StoreType::T_LONG8;break;
	case 0:
		Tmp=StoreType::T_NONE;break;
	}
	return GetTmpVar(iProcIndex,Tmp);
}

int CSymbolTbl::GetTmpVar(int iProcIndex)
{
	VarInfo Tmp;
	Tmp.m_iProcIndex=iProcIndex;
	Tmp.m_szName="_T";
	Tmp.m_szName.append(GetVarId());
	Tmp.m_eRank=VarInfo::VAR;
	AddVar(Tmp);
	return VarInfoTbl.size()-1;
}

int CSymbolTbl::CopyTmpVar(int iValIdx)
{
	VarInfo Tmp;
	Tmp=SymbolTbl.VarInfoTbl[iValIdx];
	Tmp.m_szName="_T";
	Tmp.m_szName.append(GetVarId());
	AddVar(Tmp);
	return VarInfoTbl.size()-1;
}

int CSymbolTbl::GetTmpLabel(int iProcIndex)
{
	LabelInfo Tmp;
	Tmp.m_iProcIndex=iProcIndex;
	Tmp.m_szName="__L";
	Tmp.m_szName.append(GetVarId());
	Tmp.m_bDef=true;
	Tmp.m_bUse=true;
	AddLabel(Tmp);
	return LabelInfoTbl.size()-1;
}
/*
int CSymbolTbl::RecConstTbl(const ConstInfo Value)
{
	SymbolTbl.AddConst(Value);
	CType::ProcessConstType(SymbolTbl.ConstInfoTbl.size()-1);
	return SymbolTbl.ConstInfoTbl.size()-1;
}

int CSymbolTbl::RecConstSetTbl(const string szValue)
{
	ConstInfo Tmp;
	Tmp.m_szName=szValue;
	Tmp.m_szVal=szValue;
	Tmp.m_StoreType=StoreType::T_SET;
	Tmp.m_ConstType=ConstType::SET;
	Tmp.m_szSet=szValue;
	SymbolTbl.AddConst(Tmp);

	CType::ProcessConstType(SymbolTbl.ConstInfoTbl.size()-1);
	return SymbolTbl.ConstInfoTbl.size()-1;
}

int CSymbolTbl::RecConstTbl(const string szValue,int iType)
{
	ConstInfo Tmp;
	Tmp.m_szName=szValue;
	Tmp.m_szVal=szValue;
	Tmp.m_StoreType=StoreType::T_NONE;
	switch (iType)
	{
	case 2:
		{
			Tmp.m_ConstType=ConstType::STRING;
			Tmp.m_szVal=Tmp.m_szVal.substr(1,Tmp.m_szVal.length()-2);
			Tmp.m_szVal=StrReplace(Tmp.m_szVal,"''","'");

		};break;
	case 3:		Tmp.m_ConstType=ConstType::INTEGER;		break;
	case 4:		Tmp.m_ConstType=ConstType::REAL;		break;
	case 5:
	case 6:		Tmp.m_ConstType=ConstType::EREAL;		break;
	case 7:		Tmp.m_ConstType=ConstType::BOOLEAN;		break;
	}
	SymbolTbl.AddConst(Tmp);
	CType::ProcessConstType(SymbolTbl.ConstInfoTbl.size()-1);
	return SymbolTbl.ConstInfoTbl.size()-1;
}
*/
int CSymbolTbl::SearchLabelInfoTbl(int iProcIndex,string szName)
{
	int i=LabelInfoTbl.size()-1;
	while (i>=0 && (LabelInfoTbl[i].m_iProcIndex!=iProcIndex || LabelInfoTbl[i].m_szName.compare(szName)!=0))
	{
		i--;
	}
	return i;
}

int CSymbolTbl::SearchConstInfoTbl(int iProcIndex,string szName)
{
	int i=ConstInfoTbl.size()-1;
	while (i>=0 && (ConstInfoTbl[i].m_iProcIndex!=iProcIndex || ConstInfoTbl[i].m_szName.compare(szName)!=0))
		i--;
	return i;
}

int CSymbolTbl::SearchTypeInfoTbl(int iProcIndex,string szName,bool bState)
{
	int i=TypeInfoTbl.size()-1;

	while (i>=0 && (TypeInfoTbl[i].m_iProcIndex!=iProcIndex
		|| TypeInfoTbl[i].m_szName.compare(szName)!=0
		|| (bState ? TypeInfoTbl[i].m_iState==0:false)))
	{
		i--;
	}
	return i;
}

int CSymbolTbl::SearchVarInfoTbl(int iProcIndex,string szName)
{
	int i=VarInfoTbl.size()-1;
	while (i>=0 && (VarInfoTbl[i].m_iProcIndex!=iProcIndex
		|| VarInfoTbl[i].m_szName.compare(szName)!=0 ))
	{
		i--;
	}
	return i;
}

int CSymbolTbl::SearchUseFileTbl(string szName)
{
	int i=UseFileTbl.size()-1;
	while (i>=0 && UseFileTbl.at(i).m_szFileName.compare(szName)!=0)
	{
		i--;
	}
	return i;
}

int CSymbolTbl::SearchEnumInfoTbl(int iProcIndex,string szName)
{
	int i=EnumInfoTbl.size()-1;
	while (i>=0 && (EnumInfoTbl.at(i).m_iProcIndex!=iProcIndex
		|| EnumInfoTbl.at(i).m_szName.compare(szName)!=0))
	{
		i--;
	}
	return i;
}
int CSymbolTbl::SearchProcInfoTbl(string szName,bool bIsForward)
{
	int i=ProcInfoTbl.size()-1;
	while (i>=0 && (ProcInfoTbl.at(i).m_szName.compare(szName)!=0
		|| (bIsForward?false:ProcInfoTbl.at(i).m_eFlag!=ProcInfo::Forward)))
	{
		i--;
	}

	return i;
}

void CSymbolTbl::AddLabel(LabelInfo Tmp)
{
	SymbolTbl.LabelInfoTbl.insert(pair<int,LabelInfo>(SymbolTbl.LabelInfoTbl.size(),Tmp));
}

void CSymbolTbl::AddConst(ConstInfo Tmp)
{
	SymbolTbl.ConstInfoTbl.insert(pair<int,ConstInfo>(SymbolTbl.ConstInfoTbl.size(),Tmp));
}

void CSymbolTbl::AddType(TypeInfo Tmp)
{
	SymbolTbl.TypeInfoTbl.insert(pair<int,TypeInfo>(SymbolTbl.TypeInfoTbl.size(),Tmp));
}

void CSymbolTbl::AddVar(VarInfo Tmp)
{
	SymbolTbl.VarInfoTbl.insert(pair<int,VarInfo>(SymbolTbl.VarInfoTbl.size(),Tmp));
}

void CSymbolTbl::Clear()
{
	ProcInfoTbl.clear();
	LabelInfoTbl.clear();
	ConstInfoTbl.clear();
	EnumInfoTbl.clear();
	TypeInfoTbl.clear();
	VarInfoTbl.clear();
	ProcDefTokenTbl.clear();
	while(!ProcStack.empty())
	{
		ProcStack.pop();
	}
}
/*
bool CSymbolTbl::ProcDefTokenTblCompare(vector<CToken> List1,vector<CToken> List2)
{
	if (List1.size()!=List2.size())
	{
		return false;
	}
	else
	{
		int i;

		for(i=0;i<List1.size();i++)
		{
			if (List1.at(i).m_iKind!=List2.at(i).m_iKind//先比较ID，ID相同的情况下比较名字
				|| List1.at(i).m_szContent.compare(List2.at(i).m_szContent)!=0)
			{
				return false;
			}
		}

		return true;
	}

	return true;
}

bool TypeSizeCmp(FieldMap p1,FieldMap p2)
{
	return p1.iSize<p2.iSize?true:false;
}

int CSymbolTbl::CalcTypeSize(int iPos)
{
	if (iPos==-1)
		return 0;
	else
	{
		if (TypeInfoTbl[iPos].m_iSize>0)
			return TypeInfoTbl[iPos].m_iSize;
		else
		{
			switch (TypeInfoTbl[iPos].m_eDataType)
			{
			case StoreType::T_USER:
				{
					if (TypeInfoTbl[iPos].m_iLink!=-1)
					{
						TypeInfoTbl[iPos].m_iSize=CalcTypeSize(TypeInfoTbl[iPos].m_iLink);
						return TypeInfoTbl[iPos].m_iSize;
					}
					else
					{
						TypeInfoTbl[iPos].m_iSize=CType::GetTypeSize(TypeInfoTbl[iPos].m_eDataType);
						return TypeInfoTbl[iPos].m_iSize;
					}
				}
				break;
			case StoreType::T_ARRAY:
				{
					int iSum=1;
					int i;
					for (i=TypeInfoTbl[iPos].m_ArrayInfo.size()-1;i>=0;i--)
					{
						iSum=iSum*TypeInfoTbl[iPos].m_ArrayInfo.at(i).m_iEnd-TypeInfoTbl[iPos].m_ArrayInfo.at(i).m_iStart+1;
					}
					if (TypeInfoTbl[iPos].m_iLink!=-1)
					{
						i=CalcTypeSize(TypeInfoTbl[iPos].m_iLink);
						if (i>=3 && i%4!=0)
							i=i+4-i%4;
						TypeInfoTbl[iPos].m_iSize=i*iSum;
						return TypeInfoTbl[iPos].m_iSize;
					}
					else
					{
						TypeInfoTbl[iPos].m_iSize=CType::GetTypeSize(TypeInfoTbl[iPos].m_eBaseType)*iSum;
						return TypeInfoTbl[iPos].m_iSize;
					}
				}
				break;
			case StoreType::T_RECORD:
				{
					map<string,set<string>> Tmp;

					vector<FieldMap> TmpFieldList;

					for(int i=TypeInfoTbl[iPos].m_FieldInfo.size()-1;i>=0;i--)
					{
						if (!TypeInfoTbl[iPos].m_FieldInfo.at(i).m_szVarFieldFlag.empty())
						{
							if (Tmp.find(TypeInfoTbl[iPos].m_FieldInfo.at(i).m_szVarFieldFlag)==Tmp.end())
							{
								set<string> TmpSet;
								TmpSet.insert(TypeInfoTbl[iPos].m_FieldInfo.at(i).m_szVarFieldConst);
								Tmp.insert(pair<string,set<string>>(TypeInfoTbl[iPos].m_FieldInfo.at(i).m_szVarFieldFlag,TmpSet));
							}
							else
								Tmp.find(TypeInfoTbl[iPos].m_FieldInfo.at(i).m_szVarFieldFlag)->second.insert(TypeInfoTbl[iPos].m_FieldInfo.at(i).m_szVarFieldConst);
						}
						TypeInfoTbl[iPos].m_FieldInfo.at(i).m_iSize=CalcTypeSize(TypeInfoTbl[iPos].m_FieldInfo.at(i).m_iLink);
					}

					for(int i=0;i<=TypeInfoTbl[iPos].m_FieldInfo.size()-1;i++)
					{
						TmpFieldList.push_back(FieldMap(&TypeInfoTbl[iPos].m_FieldInfo.at(i),TypeInfoTbl[iPos].m_FieldInfo.at(i).m_iSize));
					}

					sort(TmpFieldList.begin(),TmpFieldList.end(),TypeSizeCmp);
					int iOffset=0;
					int iSize=0;
					//计算不变部分size
					for(int i=0;i<=TmpFieldList.size()-1;i++)
					{
						if (TmpFieldList.at(i).ptr->m_szVarFieldFlag.empty())
						{
							int iEmpty;
							switch(TmpFieldList.at(i).iSize)
							{
							case 3:
							case 1:iEmpty=1;break;
							case 2:iEmpty=2;break;
							default:iEmpty=4;break;
							}

							if (iOffset%iEmpty!=0)
							{
								iOffset=iOffset+4-iOffset%iEmpty;	//填补空洞
							}

							TmpFieldList.at(i).ptr->m_iOffset=iOffset;
							iOffset+=TmpFieldList.at(i).iSize;
						}
					}
					//计算可变部分size
					for(map<string,set<string>>::iterator it=Tmp.begin();it!=Tmp.end();it++)
					{
						int iMax=0;
						if (iOffset%4!=0)
						{
							iOffset=iOffset+4-iOffset%4;
						}

						for(set<string>::iterator it1=it->second.begin();it1!=it->second.end();it1++)
						{
							int iSum=0;
							for(int i=0;i<=TmpFieldList.size()-1;i++)
							{
								if (TmpFieldList.at(i).ptr->m_szVarFieldFlag.empty())
								{
									continue;
								}

								if (TmpFieldList.at(i).ptr->m_szVarFieldFlag.compare(it->first)==0
									&&  TmpFieldList.at(i).ptr->m_szVarFieldConst.compare(*it1)==0)
								{
									int iEmpty;
									switch(TmpFieldList.at(i).iSize)
									{
									case 3:
									case 1:iEmpty=1;break;
									case 2:iEmpty=2;break;
									default:iEmpty=4;break;
									}

									if (iSum%iEmpty!=0)
									{
										iSum=iSum+4-iSum%iEmpty;	//填补空洞
									}

									TmpFieldList.at(i).ptr->m_iOffset=iOffset+iSum;
									iSum+=TmpFieldList.at(i).iSize;
								}
							}
							if (iSum>iMax)
							{
								iMax=iSum;
							}
						}
						iOffset+=iMax;
					}
					TypeInfoTbl[iPos].m_iSize=iOffset;
					return iOffset;
				}
				break;
			default:
				{
					TypeInfoTbl[iPos].m_iSize=CType::GetTypeSize(TypeInfoTbl[iPos].m_eDataType);
					return TypeInfoTbl[iPos].m_iSize;
				}
			}
		}
	}
}

bool CSymbolTbl::PtrCheck(int &iPos)
{
	int i=0;
	for (i=0;i<TypeInfoTbl.size();i++)
	{
		if(TypeInfoTbl[i].m_eDataType==StoreType::T_POINTER && !TypeInfoTbl[i].m_szContent.empty())
		{
			int j=SearchTypeInfoTbl(TypeInfoTbl[i].m_iProcIndex,TypeInfoTbl[i].m_szContent,false);
			if (j==-1)
			{
				j=SearchTypeInfoTbl(0,TypeInfoTbl[i].m_szContent,false);
			}
			if (j!=-1)
			{
				TypeInfoTbl[i].m_szContent="";
				TypeInfoTbl[i].m_iLink=j;
			}
			else
			{
				iPos=i;
				return false;
			}
		}
	}
	return true;
}

bool CSymbolTbl::IsVarPara(string szName,int iProcIndex)
{
	for (int i=SymbolTbl.ProcInfoTbl.at(iProcIndex).m_ParaTbl.size()-1;i>=0;i--)
	{
		if (SymbolTbl.ProcInfoTbl.at(iProcIndex).m_ParaTbl.at(i).m_szName.compare(szName)==0)
		{
			if (SymbolTbl.ProcInfoTbl.at(iProcIndex).m_ParaTbl.at(i).m_eAssignType==ParaInfo::VAR)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}
*/
int CSymbolTbl::SearchTypeSysTbl(int iOp,int iOp1Type,int iOp2Type)
{
	int i=TypeSysTbl.size()-1;

	while (i>=0 && (TypeSysTbl.at(i).m_iOp!=iOp
		|| TypeSysTbl.at(i).m_iOp1Type!=iOp1Type
		|| TypeSysTbl.at(i).m_iOp2Type!=iOp2Type))
	{
		i--;
	}

	return i;

}
/*
void CSymbolTbl::PrintIR()
{
	for(int i=0;i<SymbolTbl.ProcInfoTbl.size();i++)
	{
		if (SymbolTbl.ProcInfoTbl[i].m_bUsed==false)
		{
			continue;
		}

		cout<<"\n函数："+SymbolTbl.ProcInfoTbl.at(i).m_szName+"\n";

		for(int j=0;j<SymbolTbl.ProcInfoTbl.at(i).m_Codes.size();j++)
		{
			cout<<j<<":\t";
			cout<<GetIRStr(SymbolTbl.ProcInfoTbl.at(i).m_Codes.at(j))<<'\n';
		}
	}
}
string uddu_str(OpInfo Op)
{
	string str="du:";

	for (int k=0;k<Op.m_duChain.size();k++)
	{
		str=str+itos(Op.m_duChain[k])+",";
	}

	str=str+"\n\tud:";

	for (int k=0;k<Op.m_udChain.size();k++)
	{
		str=str+itos(Op.m_udChain[k])+",";
	}
	return str;
}

bool CSymbolTbl::IsProcVar(string szName)
{
	return SearchProcInfoTbl(szName)==-1?false:true;
}

void CSymbolTbl::PrintBasicBlock()
{
	for(int i=0;i<SymbolTbl.ProcInfoTbl.size();i++)
	{
		cout<<"\n函数："+SymbolTbl.ProcInfoTbl.at(i).m_szName+"\n";
		vector<CBasicBlock> Blocks=BasicBlock[i];

		for(int j=0;j<Blocks.size();j++)
		{
			cout<<"\n第"<<j<<"块\n";

			for (int p=Blocks[j].iStart;p<=Blocks[j].iEnd;p++)
			{
				cout<<GetIRStr(SymbolTbl.ProcInfoTbl.at(i).m_Codes.at(p))<<endl;
				cout<<"\t"<<uddu_str(SymbolTbl.ProcInfoTbl.at(i).m_Codes.at(p).m_Op1)<<endl;
				cout<<"\t"<<uddu_str(SymbolTbl.ProcInfoTbl.at(i).m_Codes.at(p).m_Op2)<<endl;
				cout<<"\t"<<uddu_str(SymbolTbl.ProcInfoTbl.at(i).m_Codes.at(p).m_Rslt)<<endl;
			}
			cout<<"后继块";

			for (int k=0;k<Blocks[j].DownFlow.size();k++)
			{
				cout<<Blocks[j].DownFlow[k]<<",";
			}

			cout<<"\n";
		}
	}
}

bool CSymbolTbl::IsTmpVar(int iPos)
{
	if (SymbolTbl.VarInfoTbl[iPos].m_szName.substr(0,2)=="_T")
	{
		return true;
	}
	else
	{
		return false;
	}
}

*/

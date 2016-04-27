#include "Peep_Hole.h"

extern CState State;

extern map<int,vector<AsmCode>> AsmCodeList;

CPeep_Hole::CPeep_Hole(void)
{
}

CPeep_Hole::~CPeep_Hole(void)
{
}
void CPeep_Hole::Recognition(vector<string> PatternStr)
{
	PeepHolePattern TmpPH;
	Pattern Tmp;
	int i;
	if (PatternStr.size()<4 ||
		PatternStr.front().compare("{")!=0 ||
		PatternStr.back().compare("}")!=0  )
		return;

	TmpPH.iPrior=atoi(PatternStr.at(1).c_str());

	for(i=2;i<PatternStr.size()-1 && PatternStr.at(i)!="***";i++)
	{
		CInstruction Ins(PatternStr.at(i),Tmp);
		Ins.GetIns();
	}
	for(vector<Code>::iterator it=Tmp.CodeList.begin();it!=Tmp.CodeList.end();it++)
	{
		PatternAsm p;
		p.Lbl=it->Label;
		p.Op=it->Op;
		p.Op1=VectorStr(it->Operand1);
		p.Op2=VectorStr(it->Operand2);
		TmpPH.PatternList.push_back(p);
	}
	i++;
	Tmp.CodeList.clear();
	for(;i<PatternStr.size()-1;i++)
	{
		CInstruction Ins(PatternStr.at(i),Tmp);
		Ins.GetIns();
	}
	for(vector<Code>::iterator it=Tmp.CodeList.begin();it!=Tmp.CodeList.end();it++)
	{
		PatternAsm p;
		p.Lbl=trim(it->Label);
		p.Op=trim(it->Op);
		p.Op1=VectorStr(it->Operand1);
		p.Op2=VectorStr(it->Operand2);
		TmpPH.CodeList.push_back(p);
	}

	m_Pattern.push_back(TmpPH);
}
void CPeep_Hole::ReadPattern(const char* pFileName)
{
	string szPatternStr=FileRead(pFileName);
	string szTmp;
	szTmp="";
	vector<string> PatternStr;
	for (string::iterator it=szPatternStr.begin();it!=szPatternStr.end();it++)
	{
		if (*it=='\n')
		{
			if (!szTmp.empty())
				PatternStr.push_back(szTmp);
			if (szTmp.compare("}")==0)
			{
				Recognition(PatternStr);
				PatternStr.clear();
			}
			szTmp="";
		}
		else
			szTmp.push_back(*it);
	}
}
bool CPeep_Hole::MatchStr(string AsmStr,string PatternStr,map<string,string> &ParaMap)
{
	map<string,string>::iterator it;
	it=ParaMap.find(PatternStr);
	if (it==ParaMap.end())
	{
		if (!AsmStr.empty())
			ParaMap.insert(pair<string,string>(PatternStr,AsmStr));
		else
			return false;
	}
	else
	{
		if (PatternStr.length()>2 && PatternStr[0]=='%' && PatternStr[1]=='R')
		{
			ParaMap.insert(pair<string,string>(PatternStr,AsmStr));
		}
		else
			if (AsmStr!=it->second)
				return false;
	}
	return true;
}
bool CPeep_Hole::MatchCode(AsmCode p1,PatternAsm p2,map<string,string> &ParaMap)
{
	map<string,string>::iterator it;
	if (p1.Lbl.empty() && p1.Op.empty())
		return true;

	if (p2.Lbl.length()>2 && p2.Lbl[0]=='%' && p2.Lbl[1]=='V')
	{
		if (!MatchStr(p1.Lbl,p2.Lbl,ParaMap))
			return false;
	}

	if (p2.Op.length()>2 && p2.Op[0]=='%' && p2.Op[1]=='V')
	{
		if (!MatchStr(p1.Op,p2.Op,ParaMap))
			return false;
	}
	else
	{
		if (p2.Op.length()>2 && p2.Op[0]=='%' && p2.Op[1]=='R')
		{
			ParaMap.insert(pair<string,string>(p2.Op,p1.Op));
		}
		else
			if (p2.Op!=p1.Op)
				return false;
	}

	if (p2.Op1.length()>2 && p2.Op1[0]=='%' && p2.Op1[1]=='V')
	{
		if (!MatchStr(p1.Op1,p2.Op1,ParaMap))
			return false;
	}
	else
	{
		if (p2.Op1.length()>2 && p2.Op1[0]=='%' && p2.Op1[1]=='R')
		{
			ParaMap.insert(pair<string,string>(p2.Op1,p1.Op1));
		}
		else
			if (p2.Op1!=p1.Op1)
				return false;
	}

	if (p2.Op2.length()>2 && p2.Op2[0]=='%' && p2.Op2[1]=='V')
	{
		if (!MatchStr(p1.Op2,p2.Op2,ParaMap))
			return false;
	}
	else
	{
		if (p2.Op2.length()>2 && p2.Op2[0]=='%' && p2.Op2[1]=='R')
		{
			ParaMap.insert(pair<string,string>(p2.Op2,p1.Op2));
		}
		else
			if (p2.Op2!=p1.Op2)
				return false;
	}

	return true;
}
bool CPeep_Hole::MatchPattern(PeepHolePattern p,int iStart,int &iEnd,vector<AsmCode> code,map<string,string> &ParaMap)
{

	int i=iStart;
	int k=0;
	ParaMap.clear();
	while (i<code.size() && k<p.PatternList.size() && MatchCode(code[i],p.PatternList[k],ParaMap))
	{
		if (!code[i].Op.empty() || !code[i].Lbl.empty())
			k++;
		i++;
	}
	if (k==p.PatternList.size())
	{
		iEnd=i-1;
		return true;
	}
	iEnd=iStart;
	return false;
}
vector<AsmCode> CPeep_Hole::ReplaceAsm(PeepHolePattern p,map<string,string> ParaMap)
{
	vector<AsmCode> AsmList;

	AsmList.push_back(InsertContent(";PeepHole Replace Start-----"));

	for (int i=0;i<p.CodeList.size();i++)
	{
		AsmCode TmpAsm;
		PatternAsm TmpPattern=p.CodeList.at(i);
		if (!TmpPattern.Lbl.empty())
		{
			if (TmpPattern.Lbl.length() && TmpPattern.Lbl[0]=='%' && TmpPattern.Lbl[1]=='V')
				TmpAsm.Lbl=ParaMap.find(TmpPattern.Lbl)->second;
			else
				TmpAsm.Lbl=TmpPattern.Lbl;
		}
		if (!TmpPattern.Op.empty())
		{
			if (TmpPattern.Op.length() && TmpPattern.Op[0]=='%' && TmpPattern.Op[1]=='V')
				TmpAsm.Op=ParaMap.find(TmpPattern.Op)->second;
			else
				TmpAsm.Op=TmpPattern.Op;
		}
		if (!TmpPattern.Op1.empty())
		{
			if (TmpPattern.Op1.length() && TmpPattern.Op1[0]=='%' && TmpPattern.Op1[1]=='V')
				TmpAsm.Op1=ParaMap.find(TmpPattern.Op1)->second;
			else
				TmpAsm.Op1=TmpPattern.Op1;
		}
		if (!TmpPattern.Op2.empty())
		{
			if (TmpPattern.Op2.length() && TmpPattern.Op2[0]=='%' && TmpPattern.Op2[1]=='V')
				TmpAsm.Op2=ParaMap.find(TmpPattern.Op2)->second;
			else
				TmpAsm.Op2=TmpPattern.Op2;
		}
		AsmList.push_back(TmpAsm);
	}

	AsmList.push_back(InsertContent(";PeepHole Replace End-----"));
	return AsmList;
}

void CPeep_Hole::Peep_Hole(int iProcIndex)
{
	map<int,vector<AsmCode>>::iterator it=AsmCodeList.find(iProcIndex);
	if (it==AsmCodeList.end())
		return;
	vector<AsmCode> tmp;
	map<string,string> ParaMap;
	int i;
	bool bChange=true;
	while (bChange)
	{
		vector<AsmCode> newAsmList;
		bChange=false;
		i=0;
		tmp=it->second;
		while (i<tmp.size())
		{
			int iEnd,j,k;
			for(j=0;j<m_Pattern.size();j++)
			{
				if (MatchPattern(m_Pattern[j],i,k,tmp,ParaMap))
				{
					iEnd=k;
					break;
				}
			}
			if (j!=m_Pattern.size())
			{

				newAsmList.push_back(InsertContent(";Deleted by PeepHole scheme:"+itos(j)));

				for (int t=i;t<=k;t++)
				{
					newAsmList.push_back(InsertContent(";"+AsmtoStr(tmp[t])));
				}
				vector<AsmCode> TmpAsmList=ReplaceAsm(m_Pattern[j],ParaMap);
				newAsmList.insert(newAsmList.end(),TmpAsmList.begin(),TmpAsmList.end());
				i=iEnd+1;
				bChange=true;
			}
			else
				newAsmList.push_back(tmp[i++]);
		}
		it->second=newAsmList;
	}
}
void CPeep_Hole::Func_Peep_Hole()
{
	string szfilename=State.m_szSysDir+"PeepHolescheme.txt";
	ReadPattern(szfilename.c_str());
	for (int i=0;i<SymbolTbl.ProcInfoTbl.size();i++)
	{
		Peep_Hole(i);
	}
}

#ifndef CPEEP_HOLE_H
#define CPEEP_HOLE_H


struct PatternAsm
{
	string Lbl;
	string Op;
	string Op1;
	string Op2;
};
struct PeepHolePattern
{
	int iPrior;
	vector<PatternAsm> PatternList;
	vector<PatternAsm> CodeList;
};

class CPeep_Hole
{
public:
	vector<PeepHolePattern> m_Pattern;
public:
	CPeep_Hole(void);
	~CPeep_Hole(void);
	void Recognition(vector<string> PatternStr);
	void ReadPattern(const char* pFileName);
	void Func_Peep_Hole();
	void Peep_Hole(int iProcIndex);
	bool MatchPattern(PeepHolePattern p,int iStart,int &iEnd,vector<AsmCode> code,map<string,string> &StrMap);
	bool MatchStr(string AsmStr,string PatternStr,map<string,string> &ParaMap);
	bool MatchCode(AsmCode p1,PatternAsm p2,map<string,string> &ParaMap);
	vector<AsmCode> ReplaceAsm(PeepHolePattern p,map<string,string> ParaMap);
};

extern CPeep_Hole PeepHole;
#endif // CPEEP_HOLE_H

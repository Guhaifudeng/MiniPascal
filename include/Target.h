#ifndef CTARGET_H
#define CTARGET_H


#include "Afx.h"
#include "Structure.h"
#include "SymbolTbl.h"
#include "DataFlowAnalysis.h"
using namespace std;
extern map<int,vector<CBasicBlock> > BasicBlock;
enum
{
	EAX=0,
	EBX,
	ECX,
	EDX,
	AX,
	BX,
	CX,
	DX,
	AH,
	BH,
	CH,
	DH,
	AL,
	BL,
	CL,
	DL,
	ESI,
	EDI,
};

static char Size2Reg[3][8]={
	{AL,BL,CL,DL,AH,BH,CH,DH},
	{AX,BX,CX,DX,-1,-1,-1,-1},
	{EAX,EBX,ECX,EDX,EDI,ESI,-1,-1}
};

static char Reg2Size[]={4,4,4,4,2,2,2,2,1,1,1,1,1,1,1,1,4,4};

static string RegName[]=
{
	"eax","ebx","ecx","edx",
	"ax","bx","cx","dx",
	"ah","bh","ch","dh","al","bl","cl","dl",
	"esi","edi"
};


static char RegRela[18][5]={
		{AL,AX,AH,-1,-1},
		{BL,BX,BH,-1,-1},
		{CL,CX,CH,-1,-1},
		{DL,DX,DH,-1,-1},

		{AL,EAX,AH,-1,-1},
		{BL,EBX,BH,-1,-1},
		{CL,ECX,CH,-1,-1},
		{DL,EDX,DH,-1,-1},

		{EAX,AX,-1,-1,-1},
		{EBX,BX,-1,-1,-1},
		{ECX,CX,-1,-1,-1},
		{EDX,DX,-1,-1,-1},

		{EAX,AX,-1,-1,-1},
		{EBX,BX,-1,-1,-1},
		{ECX,CX,-1,-1,-1},
		{EDX,DX,-1,-1,-1},

		{-1,-1,-1,-1,-1},
		{-1,-1,-1,-1,-1}
	};

static map<int,list<OpInfo> > RegVal;

class CCodePattern
{
public:
	map<string,Pattern> PatternList;
public:
	void ReadPattern(const char* pFileName);
	void Recognition(vector<string> PatternStr);
	bool SearchPattern(string szStr,Pattern &Ret);
};

class CInstruction
{
	string szInsStr;
	int iPos;
	Pattern *pattern;
	string szBuffer;
public:
	CInstruction(string szInsStr,Pattern &Tmp);
	void GetIns();
	void Trim();
	void GetContent(string &Content);
	void GetOpData(vector<string> &OpData);
	void GetOperation(string &Op);
	void GetLabel(string &Label);
	void GetCallFunc(string &FuncName);
};

class CTarget
{
public:
	CCodePattern CodePattern;
	Code CurrCode;
	vector<string> ParaStr1,Op1LoadStr,Op2LoadStr,RsltLoadStr;
	int CurrProcId;
	int CurrIR;
	CBasicBlock* pCurrBlock;
	map<string,string> LabelMap;
	map<string,int> Reg2Name;
	map<int,string> Name2Reg;
	vector<AsmCode>* CurrAsmCodeList;
	set<int> Forbid;

public:
	void RegPrint();
	void GenInit(string szFileName);
	bool GetLibAsm();
	bool IsReg(string szOp);
	bool GetIRPattern(IRCode &Code);
	bool IsDeadVar(OpInfo op);
	bool IRtoASM();
	string GetOpDataPattern(OpInfo OpData);
	string GetAsmCode(vector<string> ParaStr);
	string GetTmpLabel(string szPattern);
	bool IRtoAsmList(string szPatternStr,vector<string> ParaStr);
	bool GenParaAsm(IRCode Tmp);
	string GetRegister(int i);
	int GetProcVarSize(int iProcIndex);
	int GetProcParaSize(int iProcIndex);
	bool GenRetAsm(IRCode Tmp);
	bool GenPareAsm(IRCode Tmp);
	void MemoryAlloc();//¥Ê¥¢ø’º‰∑÷≈‰
	string RegDivision(string szReg,int iFlg);
	vector<AsmCode>* GetAsmCodeList(int iProcIndex);
	string GetConstStr(string str);
	string GetOpDataStr(OpInfo OpData);
	void RecOpAsm(string szOp);
	void RecOpAsm(string szOp,OpInfo Op);
	void RecContentAsm(string szContent);
	void WriteFile(string szFileName);
	bool GenAssignNAsm(IRCode Tmp);
	int RegSeek(OpInfo op,int iSize);
	int RegAlloc(int iSize,int iTarget=-1);
	void SaveReg(int iReg,bool bFlg=true);
	void SaveRegs(int iReg,bool bFlg=true);
	void LoadReg(int iReg,OpInfo op);
	int RegSpill(int iReg,OpInfo op);
	int RegSpill(int iReg);
	void RegClearAll();
	void RegForbid(int iReg);
	void RegRefSave();
	int OpRef(OpInfo tmp,int iReg,int iSize);
	void SetVal(int iReg,OpInfo op);
};


#endif // CTARGET_H

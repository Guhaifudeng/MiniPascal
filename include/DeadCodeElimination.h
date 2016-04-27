#ifndef CDEADCODEELIMINATION_H
#define CDEADCODEELIMINATION_H

#include "Afx.h"
#include "Structure.h"
#include "SymbolTbl.h"
#include "DataFlowAnalysis.h"
using namespace std;
extern bool bOptiChanged;
extern CSymbolTbl SymbolTbl;
extern map<OpType,Opti_Tbl*> OptiMap;
extern map<int,vector<CBasicBlock> > BasicBlock;
class CDeadCodeElimination
{
public:
	CDeadCodeElimination(void);
	~CDeadCodeElimination(void);
	static void DeadCodeElimination(int iProcIndex);
	static void DeadJumpElimination(int iProcIndex);
	static void DeleteCode(int iProcIndex);
	static void DeadBlockElimination(int iProcIndex);
	static void Func_RemoveDeadCode();
};

#endif // CDEADCODEELIMINATION_H

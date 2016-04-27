#ifndef COPTIMIZATION_H
#define COPTIMIZATION_H


#include "Afx.h"
#include "SymbolTbl.h"
#include "Structure.h"
#include "DataFlowAnalysis.h"
#include "Const_Prop.h"
#include "Const_Fold.h"
#include "Copy_Prop.h"
using namespace std;
extern CSymbolTbl SymbolTbl;
extern map<OpType,Opti_Tbl*> OptiMap;
extern map<int,vector<CBasicBlock> > BasicBlock;
extern map<string,vector<int> > VarDef;
extern map<string,vector<int> > VarAllUse;
extern set<int> bHasVisit;
extern vector<CBasicBlock>* CurrentBasicBlock;
extern bool bOptiChanged;


class COptimization
{
public:


public:
	static void InitOpti();
	static void OptiPass();
	COptimization(void);
	~COptimization(void);
};
#endif // COPTIMIZATION_H

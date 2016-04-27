#ifndef COPTIMIZATION_H
#define COPTIMIZATION_H


#include "Afx.h"
#include "SymbolTbl.h"
#include "Structure.h"
#include "DataFlowAnalysis.h"//数据流分析-控制流分析
#include "Const_Prop.h"//常量传播 --变量->常量
#include "Const_Fold.h"//常量折叠---常量+常量=常量
#include "Copy_Prop.h"//变量传播----变量+变量-->变量+常量
#include "IRSimplify.h"//条件跳转优化--连续跳转优化---代数优化
#include "DeadCodeElimination.h"//死代码(全局)----不可达代码
#include "MemShare.h"
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

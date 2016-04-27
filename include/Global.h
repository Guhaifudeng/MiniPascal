#ifndef GLOBAL_H
#define GLOBAL_H
#include "Afx.h"
#include "Structure.h"
#include "State.h"
#include "Lex.h"
#include "Syntax.h"
#include "Semantic.h"
#include "Optimization.h"
using namespace std;
extern vector<ErrorInfo> ErrorProcess;         //错误信息表
extern CState State;
extern CLex Lex;
extern CSyntax Syntax;
extern CSymbolTbl SymbolTbl;
extern vector<CToken> TokenList;			   //输出单词流

extern SemanticFunc FuncList[200];

extern map<OpType,Opti_Tbl*> OptiMap;

extern map<int,vector<CBasicBlock> > BasicBlock;
extern map<string,vector<int> > VarDef;
extern map<string,vector<int> > VarAllUse;
extern set<int> bHasVisit;
extern vector<CBasicBlock>* CurrentBasicBlock;

extern COptimization Optimization;
extern int iOptiLevel;
extern bool bOptiChanged;
#endif // GLOBAL_H

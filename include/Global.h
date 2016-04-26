#ifndef GLOBAL_H
#define GLOBAL_H
#include "Afx.h"
#include "Structure.h"
#include "State.h"
#include "Lex.h"
#include "Syntax.h"
#include "Semantic.h"
using namespace std;
extern vector<ErrorInfo> ErrorProcess;         //错误信息表
extern CState State;
extern CLex Lex;
extern CSyntax Syntax;
extern CSymbolTbl SymbolTbl;
extern vector<CToken> TokenList;			   //输出单词流

extern SemanticFunc FuncList[200];
#endif // GLOBAL_H

#ifndef GLOBAL_H
#define GLOBAL_H
#include "Afx.h"
#include "Structure.h"
#include "State.h"
#include "Lex.h"
#include "Syntax.h"
#include "Semantic.h"
using namespace std;
extern vector<ErrorInfo> ErrorProcess;         //������Ϣ��
extern CState State;
extern CLex Lex;
extern CSyntax Syntax;
extern CSymbolTbl SymbolTbl;
extern vector<CToken> TokenList;			   //���������

extern SemanticFunc FuncList[200];
#endif // GLOBAL_H

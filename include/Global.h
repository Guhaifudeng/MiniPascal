#ifndef GLOBAL_H
#define GLOBAL_H
#include "Afx.h"
#include "Structure.h"
#include "State.h"
#include "Lex.h"
#include "Syntax.h"
using namespace std;
extern vector<ErrorInfo> ErrorProcess;         //错误信息表
extern CState State;
extern CLex Lex;
extern CSyntax Syntax;
extern vector<CToken> TokenList;			   //输出单词流
typedef bool (*SemanticFunc)();

#endif // GLOBAL_H

#ifndef GLOBAL_H
#define GLOBAL_H
#include "Afx.h"
#include "Structure.h"
#include "State.h"
#include "Lex.h"
#include "Syntax.h"
using namespace std;
extern vector<ErrorInfo> ErrorProcess;         //������Ϣ��
extern CState State;
extern CLex Lex;
extern CSyntax Syntax;
extern vector<CToken> TokenList;			   //���������
typedef bool (*SemanticFunc)();

#endif // GLOBAL_H

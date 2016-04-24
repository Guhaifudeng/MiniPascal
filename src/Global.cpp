#include "Global.h"
#include "Afx.h"

vector<ErrorInfo> ErrorProcess;         //错误信息表
vector<CToken> TokenList;				//输出单词流
CState State;
CLex Lex;//词法分析
CSyntax Syntax;//语法分析
int iListPos;							//当前语法分析单词下标


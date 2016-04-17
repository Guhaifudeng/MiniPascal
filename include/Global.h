#ifndef GLOBAL_H
#define GLOBAL_H
#include "Afx.h"
#include "Structure.h"
#include "State.h"
using namespace std;
extern vector<ErrorInfo> ErrorProcess;         //错误信息表
extern CState State;
extern vector<CToken> TokenList;			   //输出单词流

#endif // GLOBAL_H

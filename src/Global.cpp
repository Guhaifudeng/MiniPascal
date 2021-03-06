#include "Global.h"
#include "Afx.h"
#define OPTI_TBL_NUM 115
vector<ErrorInfo> ErrorProcess;         //错误信息表
vector<CToken> TokenList;				//输出单词流
CState State;
CLex Lex;//词法分析
CSyntax Syntax;//语法分析
int iListPos;							//当前语法分析单词下标

SemanticFunc FuncList[200]={NULL,semantic001,semantic002,semantic003,semantic004,semantic005,semantic006,semantic007,semantic008,semantic009,semantic010,
								 semantic011,semantic012,semantic013,semantic014,semantic015,semantic016,semantic017,semantic018,semantic019,semantic020,
								 semantic021,semantic022,semantic023,semantic024,semantic025,semantic026,semantic027,semantic028,semantic029,semantic030,
								 semantic031,semantic032,semantic033,semantic034,semantic035,semantic036,semantic037,semantic038,semantic039,semantic040,
								 semantic041,semantic042,semantic043,semantic044,semantic045,semantic046,semantic047,semantic048,semantic049,semantic050,
								 semantic051,semantic052,semantic053,semantic054,semantic055,semantic056,semantic057,semantic058,semantic059,semantic060,
								 semantic061,semantic062,semantic063,semantic064,semantic065,semantic066,semantic067,semantic068,semantic069,semantic070,
								 semantic071,semantic072,semantic073,semantic074,semantic075,semantic076,semantic077,semantic078,semantic079,semantic080,
								 semantic081,semantic082,semantic083,semantic084,semantic085,semantic086,semantic087,semantic088,semantic089,semantic090,
								 semantic091,semantic092,semantic093,semantic094,semantic095,semantic096,semantic097,semantic098,semantic099,semantic100,
								 semantic101,semantic102};

CSymbolTbl SymbolTbl;//符号表
//map<int,vector<CBasicBlock>> BasicBlock;

Opti_Tbl opti_tbl[OPTI_TBL_NUM]=
{
#include "Opti_Tbl.h"
};
/**************DFA**********************/
map<OpType,Opti_Tbl*> OptiMap;

map<int,vector<CBasicBlock> > BasicBlock;
map<string,vector<int> > VarDef;
map<string,vector<int> > VarAllUse;
set<int> bHasVisit;
vector<CBasicBlock>* CurrentBasicBlock;
/****************常量传播-常量折叠-常量优化******/

COptimization Optimization;
int iOptiLevel;
bool bOptiChanged;
/********************代码生成*****************************/
map<int,vector<AsmCode> > AsmCodeList;
CTarget target;

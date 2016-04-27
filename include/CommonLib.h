#ifndef COMMONLIB_H
#define COMMONLIB_H
#include "Afx.h"
#include "Structure.h"
#include "SymbolTbl.h"
using namespace std;
/*
	公共函数库
*/

/********************词法分析***********************************/
string FileRead(string szFileName);
void SetTbl(string szSource,vector<string> &szTbl,int iStart=0);
void SetTbl(string szSource,map<string,int> &szTbl);
string trim(string str);//删去单词里解析出来的不必要的字符
string itos(int i);//将数字用字符表示
string rtos(double i);///将实数用字符表示
string UpperCase(string str);//字符大小写
string StrErase(string str,char c);//从字符串中剔除字符
string StrReplace(string str,string src,string des);//将str中字符串src替换成des
/********************语法分析***********************************/

/*********************语义分析*************************************/


string SetAdd(string szSet1,string szSet2);//集合并
string SetMul(string szSet1,string szSet2);//集合交
string SetDel(string szSet1,string szSet2);//集合减
string SetAddItem(string szSet,int i);//向集合中添加元素
string GenSetStr(int iNum,string ch);


IRCode EmitIR(OpType eOpType,OpInfo Op1,OpInfo Op2,OpInfo Rslt);//生成中间代码
IRCode EmitIR(OpType eOpType,OpInfo Op1,OpInfo Rslt);
IRCode EmitIR(OpType eOpType,OpInfo Op1);

string GetVarId();
void ClrSerialId();
string GetSerialId();
string SetValue(string szSetValue);
map<int,int> GetVar2IdMap(int iProcIndex,bool flag=true);
map<int,int> GetId2VarMap(int iProcIndex,bool flag=true);
string VectorStr(vector<string> vec);
void ClsVarId();
string GetOpStr(int i);
string GetOp(OpInfo Op);
string GetIRStr(IRCode tmp);
int GetVarLink(string szTmp);

/*****************代码生成***********************************/
string AsmtoStr(AsmCode p);
AsmCode InsertContent(string szContent);
void WriteProcAsm(vector<AsmCode> procAsm,ofstream &fout);
#endif // COMMONLIB_H

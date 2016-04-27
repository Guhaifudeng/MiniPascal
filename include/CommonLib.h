#ifndef COMMONLIB_H
#define COMMONLIB_H
#include "Afx.h"
#include "Structure.h"
#include "SymbolTbl.h"
using namespace std;
/*
	����������
*/

/********************�ʷ�����***********************************/
string FileRead(string szFileName);
void SetTbl(string szSource,vector<string> &szTbl,int iStart=0);
void SetTbl(string szSource,map<string,int> &szTbl);
string trim(string str);//ɾȥ��������������Ĳ���Ҫ���ַ�
string itos(int i);//���������ַ���ʾ
string rtos(double i);///��ʵ�����ַ���ʾ
string UpperCase(string str);//�ַ���Сд
string StrErase(string str,char c);//���ַ������޳��ַ�
string StrReplace(string str,string src,string des);//��str���ַ���src�滻��des
/********************�﷨����***********************************/

/*********************�������*************************************/


string SetAdd(string szSet1,string szSet2);//���ϲ�
string SetMul(string szSet1,string szSet2);//���Ͻ�
string SetDel(string szSet1,string szSet2);//���ϼ�
string SetAddItem(string szSet,int i);//�򼯺������Ԫ��
string GenSetStr(int iNum,string ch);


IRCode EmitIR(OpType eOpType,OpInfo Op1,OpInfo Op2,OpInfo Rslt);//�����м����
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

/*****************��������***********************************/
string AsmtoStr(AsmCode p);
AsmCode InsertContent(string szContent);
void WriteProcAsm(vector<AsmCode> procAsm,ofstream &fout);
#endif // COMMONLIB_H

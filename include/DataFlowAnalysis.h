#ifndef CDATAFLOWANALYSIS_H
#define CDATAFLOWANALYSIS_H
#include "Afx.h"
#include "Structure.h"
#include "SymbolTbl.h"
#include "Optimization.h"
using namespace std;
extern map<OpType,Opti_Tbl*> OptiMap;
extern map<int,vector<CBasicBlock> > BasicBlock;//流图
extern set<int> bHasVisit;//基本块是否已被访问
extern vector<CBasicBlock>* CurrentBasicBlock;//当前基本流图
extern map<string,vector<int> > VarDef;//变量定值集合--流图+变量序数--定值点
extern map<string,vector<int> > VarAllUse;//变量引用集合--流图+变量序数--引用点
extern CSymbolTbl SymbolTbl;
//CBits --- 流图内变量----序号----位
//
class CDataFlowAnalysis
{

public:
	CDataFlowAnalysis();
	~CDataFlowAnalysis();
	static void DataFlowAnalysis();
	static void GetBasicBlock(int iProcIndex);
	static void Def_Use_Analysis(int iProcIndex);
	static void In_Out_Analysis(int iProcIndex);
	static void ud_du_Analysis(int iProcIndex);
	static int ForwardHasDef(int iVar,int iStart,int iEnd,int iProcIndex);
	static int BackwardHasDef(int iVar,int iStart,int iEnd,int iProcIndex);
	static vector<int> GetUd(int iVar,int iBb,int iEnd,int iProcIndex,bool &bUninit);
	static vector<int> GetDu(int iVar,int iBb,int iPos,int iProcIndex);
	static void PrintDefUse();
	static void VarDefUse(map<string,vector<int> > &v,int VarPos,int CodePos,int iProcIndex);
	static void RecordDef(map<int,int>VarMap,CBits &Use,CBits &Def,OpInfo Op,vector<int> RefVar,int iPos,int iProcIndex);
	static void RecordUse(map<int,int>VarMap,CBits &Use,CBits &Def,OpInfo Op,vector<int> RefVar,int iPos,int iProcIndex);
};
#endif // CDATAFLOWANALYSIS_H

#ifndef CDATAFLOWANALYSIS_H
#define CDATAFLOWANALYSIS_H
#include "Afx.h"
#include "Structure.h"
#include "SymbolTbl.h"
#include "Optimization.h"
using namespace std;
extern map<OpType,Opti_Tbl*> OptiMap;
extern map<int,vector<CBasicBlock> > BasicBlock;//��ͼ
extern set<int> bHasVisit;//�������Ƿ��ѱ�����
extern vector<CBasicBlock>* CurrentBasicBlock;//��ǰ������ͼ
extern map<string,vector<int> > VarDef;//������ֵ����--��ͼ+��������--��ֵ��
extern map<string,vector<int> > VarAllUse;//�������ü���--��ͼ+��������--���õ�
extern CSymbolTbl SymbolTbl;
//CBits --- ��ͼ�ڱ���----���----λ
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

#ifndef CCOPY_PROP_H
#define CCOPY_PROP_H
#include "Afx.h"
#include "Structure.h"
#include "SymbolTbl.h"
using namespace std;
extern bool bOptiChanged;
extern CSymbolTbl SymbolTbl;
extern map<OpType,Opti_Tbl*> OptiMap;
extern map<string,vector<int> > VarDef;//变量定值集合--流图+变量序数--定值点
extern map<string,vector<int> > VarAllUse;//变量引用集合--流图+变量序数--引用点
class CCopy_Prop
{
public:
	CCopy_Prop(void);
	~CCopy_Prop(void);
	static bool GenCopyOp(OpInfo &Op,int iProcIndex);
	static void Copy_Prop(int iProcIndex);
	//static void Tmp_Prop(int iProcIndex);
	static void Func_Copy_Prop();
};

#endif // CCOPY_PROP_H

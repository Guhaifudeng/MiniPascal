#ifndef CCONST_PROP_H
#define CCONST_PROP_H
#include "Afx.h"
#include "Structure.h"
#include "SymbolTbl.h"
using namespace std;
extern bool bOptiChanged;
extern CSymbolTbl SymbolTbl;
extern map<OpType,Opti_Tbl*> OptiMap;
class CConst_Prop
{
public:
	CConst_Prop(void);
	~CConst_Prop(void);
	static void Const_Prop(int iProcIndex);
	static void Func_Const_Prop();
	static bool GenConstOp(OpInfo &Op,int iProcIndex);
};

#endif // CCONST_PROP_H

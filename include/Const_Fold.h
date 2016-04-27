#ifndef CCONST_FOLD_H
#define CCONST_FOLD_H

#include "Afx.h"
#include "Structure.h"
#include "SymbolTbl.h"
#include "Semantic.h"
using namespace std;
extern bool bOptiChanged;
extern CSymbolTbl SymbolTbl;
extern map<OpType,Opti_Tbl*> OptiMap;

class CConst_Fold
{
public:
	CConst_Fold(void);
	~CConst_Fold(void);
	static bool GenConstOp(OpInfo &Op,int iProcIndex);
	static void Const_Fold(int iProcIndex);
	static void Func_Const_Fold();
};


#endif // CCONST_FOLD_H

#ifndef CIRSIMPLIFY_H
#define CIRSIMPLIFY_H
#include "Structure.h"
#include "Afx.h"
#include "SymbolTbl.h"s
using namespace std;
extern bool bOptiChanged;
extern CSymbolTbl SymbolTbl;
extern map<OpType,Opti_Tbl*> OptiMap;
static map<int,int> Num2Bit;

class CIRSimplify
{
public:
	CIRSimplify(void);
	~CIRSimplify(void);
	static void AlgebraicSimplify(int iProcIndex);
	static void IRSimplify(int iProcIndex);
	static void JumpSimplify(int iProcIndex);
	static void Func_IRSimplify();

};


#endif // CIRSIMPLIFY_H

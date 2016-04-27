#ifndef CMEMORYALLOC_H
#define CMEMORYALLOC_H
#include "Afx.h"
#include "SymbolTbl.h"
#include "CommonLib.h"
#include "Structure.h"
using namespace std;
extern CSymbolTbl SymbolTbl;
extern map<string,vector<int> > VarDef;//变量定值集合--流图+变量序数--定值点
extern map<string,vector<int> > VarAllUse;//变量引用集合--流图+变量序数--引用点
struct VarMem
{
	int m_iLink;
	int m_iSize;
	bool m_bMaxTmp;
	VarMem(int p1,int p2);
};

class CMemoryAlloc
{

public:

	CMemoryAlloc(void);

	void static MemoryAlloc();

public:
	~CMemoryAlloc(void);
};

#endif // CMEMORYALLOC_H

#ifndef CMEMSHARE_H
#define CMEMSHARE_H
#include "Afx.h"
#include "Structure.h"
#include "SymbolTbl.h"
using namespace std;
extern CSymbolTbl SymbolTbl;
extern bool bOptiChanged;
extern map<int,vector<CBasicBlock> > BasicBlock;//流图
extern map<string,vector<int> > VarDef;//变量定值集合--流图+变量序数--定值点
extern map<string,vector<int> > VarAllUse;//变量引用集合--流图+变量序数--引用点
struct RefArea
{
	int m_iDef;
	int m_iUse;
};

class CMemShare
{

public:
	CMemShare(void);
	static void TmpMemShare();
	static void AddTmp(int iProcIndex,int iTmpIndex,int iDef,int iUse);
public:
	~CMemShare(void);
};


#endif // CMEMSHARE_H

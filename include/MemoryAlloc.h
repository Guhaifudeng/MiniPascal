#ifndef CMEMORYALLOC_H
#define CMEMORYALLOC_H
#include "Afx.h"
#include "SymbolTbl.h"
#include "CommonLib.h"
#include "Structure.h"
using namespace std;
extern CSymbolTbl SymbolTbl;
extern map<string,vector<int> > VarDef;//������ֵ����--��ͼ+��������--��ֵ��
extern map<string,vector<int> > VarAllUse;//�������ü���--��ͼ+��������--���õ�
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

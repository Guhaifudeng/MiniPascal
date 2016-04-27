#ifndef CMEMSHARE_H
#define CMEMSHARE_H
#include "Afx.h"
#include "Structure.h"
#include "SymbolTbl.h"
using namespace std;
extern CSymbolTbl SymbolTbl;
extern bool bOptiChanged;
extern map<int,vector<CBasicBlock> > BasicBlock;//��ͼ
extern map<string,vector<int> > VarDef;//������ֵ����--��ͼ+��������--��ֵ��
extern map<string,vector<int> > VarAllUse;//�������ü���--��ͼ+��������--���õ�
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

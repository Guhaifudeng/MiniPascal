#ifndef CSYNTAX_H
#define CSYNTAX_H
#include "Afx.h"
#include "Structure.h"
#include "ErrorProc.h"
#include "CommonLib.h"
using namespace std;
extern int iListPos;
extern vector<CToken> TokenList;
extern SemanticFunc FuncList[200];
class CSyntax
{
private:
	int m_iParseTbl[120][120];//LL(1)Ԥ�������
	vector<string> m_szProductList;//����ʽ
	stack<int> m_ParseStack;//����ջ
	void EnStack(string szProduct);//��ջ-����ʽ
	bool DeStack(int &iTop);//��ջ-�ս������ս�� ����ջ�����ַ��ţ��ս��-<100;���ս�� 100-300�������ӳ������ ->300
	void ClearStack();
public:
	CSyntax(void);
	bool SyntaxParse();//�������
	void Init();
	~CSyntax(void);
};
#endif // CSYNTAX_H

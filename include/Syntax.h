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
	int m_iParseTbl[120][120];//LL(1)预测分析表
	vector<string> m_szProductList;//产生式
	stack<int> m_ParseStack;//分析栈
	void EnStack(string szProduct);//入栈-产生式
	bool DeStack(int &iTop);//出栈-终结符或非终结符 分析栈中三种符号：终结符-<100;非终结符 100-300；语义子程序序号 ->300
	void ClearStack();
public:
	CSyntax(void);
	bool SyntaxParse();//语义分析
	void Init();
	~CSyntax(void);
};
#endif // CSYNTAX_H

#include "Global.h"
#include "Afx.h"

vector<ErrorInfo> ErrorProcess;         //������Ϣ��
vector<CToken> TokenList;				//���������
CState State;
CLex Lex;//�ʷ�����
CSyntax Syntax;//�﷨����
int iListPos;							//��ǰ�﷨���������±�


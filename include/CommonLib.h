#ifndef COMMONLIB_H
#define COMMONLIB_H
#include "Afx.h"
using namespace std;
/*
	����������
*/


/*��ȡָ���ļ������ı��ļ�����
  ����: pFileName	�ļ���
  ����: iStart		���ַ�����ʽ�����ı��ļ�����
*/
void SetTbl(string szSource,vector<string> &szTbl,int iStart=0);
void SetTbl(string szSource,map<string,int> &szTbl);
string FileRead(string szFileName);

string trim(string str);
string itos(int i);
string StrErase(string str,char c);

string StrReplace(string str,string src,string des);

string UpperCase(string str);


#endif // COMMONLIB_H

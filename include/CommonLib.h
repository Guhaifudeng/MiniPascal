#ifndef COMMONLIB_H
#define COMMONLIB_H
#include "Afx.h"
using namespace std;
/*
	����������
*/

/********************�ʷ�����***********************************/
string FileRead(string szFileName);
void SetTbl(string szSource,vector<string> &szTbl,int iStart=0);
void SetTbl(string szSource,map<string,int> &szTbl);

string trim(string str);//ɾȥ��������������Ĳ���Ҫ���ַ�
string itos(int i);//���������ַ���ʾ
string UpperCase(string str);//�ַ���Сд
/********************�﷨����***********************************/
string StrErase(string str,char c);//���ַ������޳��ַ�
string StrReplace(string str,string src,string des);
#endif // COMMONLIB_H

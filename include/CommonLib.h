#ifndef COMMONLIB_H
#define COMMONLIB_H
#include "Afx.h"
using namespace std;
/*
	公共函数库
*/

/********************词法分析***********************************/
string FileRead(string szFileName);
void SetTbl(string szSource,vector<string> &szTbl,int iStart=0);
void SetTbl(string szSource,map<string,int> &szTbl);

string trim(string str);//删去单词里解析出来的不必要的字符
string itos(int i);//将数字用字符表示
string UpperCase(string str);//字符大小写
/********************语法分析***********************************/
string StrErase(string str,char c);//从字符串中剔除字符
string StrReplace(string str,string src,string des);
#endif // COMMONLIB_H

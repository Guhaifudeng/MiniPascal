#ifndef COMMONLIB_H
#define COMMONLIB_H
#include "Afx.h"
using namespace std;
/*
	公共函数库
*/


/*读取指定文件名的文本文件函数
  参数: pFileName	文件名
  返回: iStart		以字符串形式返回文本文件内容
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

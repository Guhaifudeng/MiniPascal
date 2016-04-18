#include "CommonLib.h"
#include "Afx.h"
/********************词法分析***********************************/
void SetTbl(string szSource,map<string,int> &szTbl)
{
    string szTmp="";
    int cnt=0;
    for(int i=0; i<szSource.length(); i++)
    {
        if (szSource[i]!='\n')
        {
            szTmp.append(1,szSource[i]);
        }
        else
        {
            if (szTmp.compare("")!=0)
            {
                szTbl.insert(pair<string,int>(szTmp,cnt));
                szTmp="";
                cnt++;
            }
        }
    }
    if (szTmp.compare("")!=0)
        szTbl.insert(pair<string,int>(szTmp,cnt));
}

void SetTbl(string szSource,vector<string> &szTbl,int iStart)
{
    string szTmp="";
    int cnt=iStart;

    for (int i=0; i<cnt; i++)
    {
        szTbl.push_back("");
    }

    for(int i=0; i<szSource.length(); i++)
    {
        if (szSource[i]!='\n')
        {
            szTmp.append(1,szSource[i]);
        }
        else
        {
            if (szTmp.compare("")!=0)
            {
                szTbl.push_back(szTmp);
                szTmp="";
                cnt++;
            }
        }
    }

    if (szTmp.compare("")!=0)
    {
        szTbl[cnt]=szTmp;
    }

}


string FileRead(string szFileName)
{
    string szFileContent="";
    FILE* fp=fopen(szFileName.c_str(),"rt");

    if (fp==NULL)
    {
        return "";
    }

    int i=1;
    char Buffer[1];

    while (i==1)
    {
        i=fread(Buffer,1,1,fp);

        if (i==1)
        {
            szFileContent.push_back((char)Buffer[0]);
        }
    }

    fclose(fp);
    return szFileContent;
}

string ltrim(string str)
{
    if (str.empty())
    {
        return str;
    }

    string szTmp="";
    int i;

    for (i=0; str[i]<=32&&i<str.length(); i++);

    for (; i<str.length(); i++)
    {
        szTmp.append(1,str[i]);
    }

    return szTmp;
}

string rtrim(string str)
{
    if (str.empty())
    {
        return str;
    }

    string szTmp="";
    int i;
    for (i=str.length(); str[i]<=32&&i>=0; i--);

    for(int j=0; j<=i; j++)
    {
        szTmp.append(1,str[j]);
    }

    return szTmp;
}
string trim(string str)
{
    return rtrim(ltrim(str));
}
string itos(int i)
{
    char cBuffer[10];
    int iPos,iSign;
    itoa(i,cBuffer,10);
    return cBuffer;
}

string UpperCase(string str)
{
    string szTmp=str;

    for (int i=0; i<szTmp.length(); i++)
    {
        if (szTmp[i]>='a' && szTmp[i]<='z')
        {
            szTmp[i]=szTmp[i]-('a'-'A');
        }
    }
    return szTmp;
}
/********************语法分析***********************************/
string StrErase(string str,char c)
{
    string szTmp="";
    int i;

    for (i=0; i<str.length(); ++i)
    {
        if (str[i]!=c)
        {
            szTmp.append(1,str[i]);
        }
    }

    return  szTmp;
}


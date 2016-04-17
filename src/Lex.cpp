#include "Lex.h"
#include "StdAfx.h"
extern CState State;
CLex::CLex()
{
}

void CLex::SetFileName(string szStr)
{
    m_szFileName=szStr;
}

void CLex::BufferBack()
{
    m_szBuffer.erase(m_szBuffer.end()-1);
    m_iPos--;
}

bool CLex::SearchKeyword(string szKeyWord,int &iPosition)
{
    map<string,int>::iterator It=m_KeywordTbl.find(szKeyWord);
    if (It!=m_KeywordTbl.end())
    {
        iPosition=It->second;
        return true;
    }
    return false;
}

bool CLex::Process(int iTag)
{
    int iTmp=0;
    if (iTag==-99)
        return false;

    if (iTag<0)
    {
        BufferBack();
        m_szBuffer=trim(m_szBuffer);

        if (iTag==-1)
        {
            m_szBuffer=UpperCase(m_szBuffer);
            if (SearchKeyword(m_szBuffer,iTmp)==true)
            {
                EmitToken(iTmp+40,NULL,m_iRow);
            }
            else
            {
                if (m_szBuffer.compare("TRUE")==0 || m_szBuffer.compare("FALSE")==0 )
                {
                    //EmitToken(3,SymbolTbl.RecConstTbl(m_szBuffer,7),m_iRow);// 符号表-存储常量
                }
                else
                {
                    EmitToken(1,m_szBuffer,m_iRow);
                }
            }
        }

        if (iTag>=-6 && iTag<=-2)
        {
            //EmitToken(-iTag,SymbolTbl.RecConstTbl(m_szBuffer,-iTag),m_iRow);// 符号表-存储常量
        }
        if (iTag>=-15 && iTag<=-7)
        {
            EmitToken(-iTag,NULL,m_iRow);
        }

        if (iTag>=-28 && iTag<=-16)
        {
            EmitToken(-iTag,NULL,m_iRow);
        }

        if (iTag==-42)
        {
            BufferBack();
            m_szBuffer=trim(m_szBuffer);
            //EmitToken(3,SymbolTbl.RecConstTbl(m_szBuffer,3),m_iRow); //符号表-存储常量
        }

        m_szBuffer="";
        m_iNonTerminal=0;
    }
    else
    {
        m_iNonTerminal=iTag;
    }

    return true;
}

void CLex::EmitToken(int iKind,string sContent,int iRow)
{
    m_pTokenList->push_back(CToken(iKind,sContent,iRow,m_szFileName));
}

void CLex::EmitToken(const int iKind,const int iContent,const int iRow)
{
    string szTmp;
    char cBuffer[5];
    itoa(iContent,cBuffer,10);
    szTmp=cBuffer;
    EmitToken(iKind,szTmp,iRow);
}

void CLex::SetKeywords(string szStr)
{
    SetTbl(szStr,m_KeywordTbl);
}

void CLex::SetLexTbl(string szStr)
{
    int iRow=0;
    int iCol=0;
    int iTmp=0;
    szStr=StrErase(szStr,'\n');

    for(iRow=0; iRow<=36; iRow++)
    {
        for(iCol=0; iCol<=128; iCol++)
        {
            m_szLexTbl[iRow][iCol]=atoi(szStr.substr(iTmp,3).c_str());
            iTmp=iTmp+3;
        }
    }
}

bool CLex::GenToken(vector<CToken> *pTokenList)
{


    bool bTag=true;
    m_iPos=0;
    m_pTokenList=pTokenList;
    m_szSource=FileRead(m_szFileName.c_str());
    m_szSource.push_back(' ');
    m_iNonTerminal=0;
    m_szBuffer="";
    m_iRow=1;
    int TmpPos=0;
    m_pTokenList->clear();

    while (m_iPos<m_szSource.length() && bTag)
    {
        if (m_szSource[m_iPos]=='\n'&& TmpPos!=m_iPos)
        {
            m_iRow++;

            TmpPos=m_iPos;
        }

        m_szBuffer.push_back(m_szSource.at(m_iPos));
        bTag=Process(m_szLexTbl[m_iNonTerminal][(unsigned char)(m_szSource[m_iPos]<128?m_szSource[m_iPos]:128)]);

        if (!bTag)
        {
            EmitError(m_szFileName+"("+itos(m_iRow)+"): lex Error");

            return false;
        }

        m_iPos++;
    }
    return bTag;
}



void CLex::LexInit()
{
    m_szSource="";
    SetLexTbl(FileRead(State.m_szSysDir+"lex.txt"));
    SetKeywords(FileRead(State.m_szSysDir+"keywords.txt"));
}
CLex::~CLex()
{
}

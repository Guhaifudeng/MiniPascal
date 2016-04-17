#ifndef CSTATE_H
#define CSTATE_H
#include "Afx.h"
#include "ErrorProc.h"
using namespace std;
class CState
{
public:
    string m_szSourceFile;
    string m_szSysDir;
    string m_szOutputFile;
    string m_szSysLib;
    string m_szUserLib;
    string m_szErrFile;
    bool m_bOpti;

public:
    CState()
    {
        m_szSourceFile = "";
        m_szSysDir = "";
        m_szOutputFile = "";
        m_szSysLib = "";
        m_szUserLib = "";
        m_szErrFile = "";
        m_bOpti = false;
    };
    bool CommandInit(int argc, char** argv);
    ~CState() {};



};
#endif // CSTATE_H

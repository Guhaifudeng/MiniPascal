#include "State.h"
#include "Afx.h"

bool CState::CommandInit(int argc, char** argv)
{
    char* cmd;
m_szSourceFile = "test\\1.mlp";
    for (int i = 0 ; i < argc ; i ++)
    {
        cmd = argv[i];

        if (cmd[0] == '/'
                || cmd[0] == '-')
        {
            if (cmd[1] == 's')
            {
                m_szSourceFile = &cmd[2];
            }

            else if (cmd[1] == 'o'
                     && cmd[2] == 'p'
                     && cmd[3] == 't')
            {
                m_bOpti = true;
            }
        }
    }

m_szSysDir="sys\\";
m_szErrFile="err\\";
m_szOutputFile="out\\";
/*m_szSysLib="syslib\\";
m_szUserLib="userlib\\";
    if (-1 == access(m_szSysLib.c_str(),2))
    {
        EmitError("系统库路径未知22");

        return false;
    }

    if (-1 == access(m_szUserLib.c_str(),2))
    {
        EmitError("用户库路径未知");

        return false;
    }
*/
    string szTmp = m_szSysDir + "lex.txt";

    if (-1 == access(szTmp.c_str(),2))
    {
        EmitError("无法找到文件lex.txt");

        return false;
    }

    szTmp = m_szSysDir + "keywords.txt";

    if (-1 == access(szTmp.c_str(),2))
    {
        EmitError("无法找到文件keywords.txt");

        return false;
    }

    szTmp = m_szSysDir + "TypeSys.txt";

    if (-1 == access(szTmp.c_str(),2))
    {
        EmitError("无法找到文件TypeSys.txt");

        return false;
    }

    szTmp = m_szSysDir + "AsmScheme.txt";

    if (-1 == access(szTmp.c_str(),2))
    {
        EmitError("无法找到文件AsmScheme.txt");

        return false;
    }

    szTmp = m_szSysDir + "PeepHoleScheme.txt";

    if (-1 == access(szTmp.c_str(),2))
    {
        EmitError("无法找到文件PeepHolescheme.txt");

        return false;
    }

    if (-1 == access(m_szSourceFile.c_str(),2))
    {
        EmitError("无法找到文件"+m_szSourceFile);

        return false;
    }

    if (m_szOutputFile.empty())
    {
        EmitError("缺少输出文件名");

        return false;
    }

    return true;
}

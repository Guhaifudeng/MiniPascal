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
        EmitError("ϵͳ��·��δ֪22");

        return false;
    }

    if (-1 == access(m_szUserLib.c_str(),2))
    {
        EmitError("�û���·��δ֪");

        return false;
    }
*/
    string szTmp = m_szSysDir + "lex.txt";

    if (-1 == access(szTmp.c_str(),2))
    {
        EmitError("�޷��ҵ��ļ�lex.txt");

        return false;
    }

    szTmp = m_szSysDir + "keywords.txt";

    if (-1 == access(szTmp.c_str(),2))
    {
        EmitError("�޷��ҵ��ļ�keywords.txt");

        return false;
    }

    szTmp = m_szSysDir + "TypeSys.txt";

    if (-1 == access(szTmp.c_str(),2))
    {
        EmitError("�޷��ҵ��ļ�TypeSys.txt");

        return false;
    }

    szTmp = m_szSysDir + "AsmScheme.txt";

    if (-1 == access(szTmp.c_str(),2))
    {
        EmitError("�޷��ҵ��ļ�AsmScheme.txt");

        return false;
    }

    szTmp = m_szSysDir + "PeepHoleScheme.txt";

    if (-1 == access(szTmp.c_str(),2))
    {
        EmitError("�޷��ҵ��ļ�PeepHolescheme.txt");

        return false;
    }

    if (-1 == access(m_szSourceFile.c_str(),2))
    {
        EmitError("�޷��ҵ��ļ�"+m_szSourceFile);

        return false;
    }

    if (m_szOutputFile.empty())
    {
        EmitError("ȱ������ļ���");

        return false;
    }

    return true;
}

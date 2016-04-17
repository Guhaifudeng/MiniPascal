#include "State.h"
#include "StdAfx.h"

bool CState::CommandInit(int argc, char** argv)
{
    char* cmd;

    for (int i = 0 ; i < argc ; i ++)
    {
        cmd = argv[i];

        if (cmd[0] == '/'
                || cmd[0] == '-')
        {
            if (cmd[1] == 'l'
                    && cmd[2] == 's')
            {
                m_szSysLib = &cmd[3];

                if (m_szSysLib.length() >= 1
                        && m_szSysLib[m_szSysLib.length()-1] != '\\')
                {
                    m_szSysLib = m_szSysLib + '\\';
                }
            }
            else if (cmd[1] == 'l'
                     && cmd[2] == 'u')
            {
                m_szUserLib = &cmd[3];

                if (m_szUserLib.length() >= 1
                        && m_szUserLib[m_szUserLib.length()-1] != '\\')
                {
                    m_szUserLib = m_szUserLib + '\\';
                }
            }
            else if (cmd[1] == 's')
            {
                m_szSourceFile = &cmd[2];
            }
            else if (cmd[1] == 'd')
            {
                m_szSysDir = &cmd[2];

                if (m_szSysDir.length() >= 1
                        && m_szSysDir[m_szSysDir.length()-1] != '\\')
                {
                    m_szSysDir=m_szSysDir+'\\';
                }
            }
            else if (cmd[1] == 'e'
                     && cmd[2] == 'r'
                     && cmd[3] == 'r')
            {
                m_szErrFile = &cmd[4];
            }
            else if (cmd[1] == 'o'
                     && cmd[2] == 'u'
                     && cmd[3] == 't')
            {
                m_szOutputFile = &cmd[4];
            }
            else if (cmd[1] == 'o'
                     && cmd[2] == 'p'
                     && cmd[3] == 't')
            {
                m_bOpti = true;
            }
        }
    }

    if (-1 == access(m_szSysLib.c_str(),2))
    {
        EmitError("ϵͳ��·��δ֪");

        return false;
    }

    if (-1 == access(m_szUserLib.c_str(),2))
    {
        EmitError("�û���·��δ֪");

        return false;
    }

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

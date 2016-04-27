#include "State.h"
#include "Afx.h"
#include <string.h>
void replace_all(string& str,char* oldValue,char* newValue)
{
    string::size_type pos(0);

    while(true){
        pos=str.find(oldValue,pos);
        if (pos!=(string::npos))
        {
            str.replace(pos,strlen(oldValue),newValue);
            pos+=2;//注意是加2，为了跳到下一个反斜杠
        }
        else
            break;
    }
}
bool CState::CommandInit(int argc, char** argv)
{
    char* cmd;
    /*char buf[1000];
    int i=1000;
    GetCurrentDirectory(1000,buf);  //得到当前工作路径*/
    char buf[80];
    getcwd(buf, sizeof(buf));
    string a;
    a.assign(buf);
    replace_all(a,"\\","\\\\");
    a.append("\\\\");
    cout<<a<<endl;
    m_szSourceFile = a+"test\\1.mlp";
    for (int i = 0 ; i < argc ; i ++)
    {
        cmd = argv[i];

        if (cmd[0] == '/'
                || cmd[0] == '-')
        {
            if(cmd[1]=='d'){//设置绝对路径
                char *p;
                p = &cmd[2];
                a.assign(p);
                replace_all(a,"\\","\\\\");
                a.append("\\\\");
                cout<<a<<endl;
            }else if (cmd[1] == 's')
            {
                m_szSourceFile = &cmd[2];
                m_szSourceFile=a+"\\test\\"+m_szSourceFile;
            }

            else if (cmd[1] == 'o'
                     && cmd[2] == 'p'
                     && cmd[3] == 't')
            {
                m_bOpti = true;
            }
        }
    }

m_szSysDir=a+"sys\\";
m_szErrFile=a+"err\\";
m_szOutputFile=a+"out\\";
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

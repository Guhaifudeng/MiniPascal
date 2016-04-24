#include "CommonLib.h"
#include "Afx.h"
/********************´Ê·¨·ÖÎö***********************************/
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

string StrErase(string str,char c)//ÌÞ³ýÄ³×Ö·û
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
/********************Óï·¨·ÖÎö***********************************/


/*********************ÓïÒå·ÖÎö*************************************/
extern Opti_Tbl opti_tbl[OPTI_TBL_NUM];

extern CSymbolTbl SymbolTbl;

map<int,int> GetVar2IdMap(int iProcIndex,bool flag)
{
	map<int,int> Tmp;
	int j=0;

	for(int i=0;i<SymbolTbl.VarInfoTbl.size();i++)
	{
		if ((SymbolTbl.VarInfoTbl[i].m_iProcIndex==0 && flag)
			|| SymbolTbl.VarInfoTbl[i].m_iProcIndex==iProcIndex)
		{
			Tmp.insert(pair<int,int>(i,j++));
		}
	}

	return Tmp;
}

map<int,int> GetId2VarMap(int iProcIndex,bool flag)
{
	map<int,int> Tmp;
	int j=0;

	for(int i=0;i<SymbolTbl.VarInfoTbl.size();i++)
	{
		if ((SymbolTbl.VarInfoTbl[i].m_iProcIndex==0 && flag)
			|| SymbolTbl.VarInfoTbl[i].m_iProcIndex==iProcIndex)
		{
			Tmp.insert(pair<int,int>(j++,i));
		}
	}
	return Tmp;
}

void SetTbl(string szSource,map<string,int> &szTbl)
{
	string szTmp="";
	int cnt=0;
	for(int i=0;i<szSource.length();i++)
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

	for (int i=0;i<cnt;i++)
	{
		szTbl.push_back("");
	}

	for(int i=0;i<szSource.length();i++)
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

	for (i=0;str[i]<=32&&i<str.length();i++);

	for (;i<str.length();i++)
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
	for (i=str.length();str[i]<=32&&i>=0;i--);

	for(int j=0;j<=i;j++)
	{
		szTmp.append(1,str[j]);
	}

	return szTmp;
}
string trim(string str)
{
	return rtrim(ltrim(str));
}
string VectorStr(vector<string> vec)
{
	string tmp="";

	for(vector<string>::iterator it=vec.begin();it!=vec.end();it++)
	{
		tmp+=*it;
	}

	return trim(tmp);
}

string UpperCase(string str)
{
	string szTmp=str;

	for (int i=0;i<szTmp.length();i++)
	{
		if (szTmp[i]>='a' && szTmp[i]<='z')
		{
			szTmp[i]=szTmp[i]-('a'-'A');
		}
	}
	return szTmp;
}
string StrErase(string str,char c)
{
	string szTmp="";
	int i;

	for (i=0;i<str.length();++i)
	{
		if (str[i]!=c)
		{
			szTmp.append(1,str[i]);
		}
	}

	return  szTmp;
}
string GenSetStr(int iNum,string ch)
{
	int i;
	string szTmp;

	for(i=0;i<iNum;i++)
	{
		szTmp.append(ch);
	}

	return szTmp;
}

string SetAdd(string szSet1,string szSet2)
{
	string szTmp;

	for (int i=0;i<256;i++)
	{
		szTmp.append(szSet1.at(i)=='1' || szSet2.at(i)=='1' ?"1":"0" );
	}
	return szTmp;
}

string SetMul(string szSet1,string szSet2)
{
	string szTmp;

	for (int i=0;i<256;i++)
	{
		szTmp.append(szSet1.at(i)=='1' && szSet2.at(i)=='1' ?"1":"0" );
	}
	return szTmp;
}

string SetDel(string szSet1,string szSet2)
{
	string szTmp=GenSetStr(256,"0");

	for (int i=0;i<256;i++)
	{
		if (szSet2.at(i)=='1')
		{
			szTmp.at(i)='0';
		}
		else
		{
			szTmp.at(i)=szSet1.at(i);
		}
	}
	return szTmp;
}

string SetAddItem(string szSet,int i)
{
	if (i>=szSet.length())
	{
		return szSet;
	}
	else
	{
		szSet.at(i)='1';
		return szSet;
	}
}

string StrReplace(string str,string src,string des)
{
	string szTmp=str;
	int i=szTmp.find(src);

	while (i!=string::npos)
	{
		szTmp.replace(i,src.length(),des);
		i=szTmp.find(src);
	}

	return szTmp;
}

static int SerialId;

static int VarId;

static int LabelId;

void ClsVarId()
{
	VarId=0;
}

string GetVarId()
{
	return itos(VarId++);
}

void ClsLabelId()
{
	LabelId=0;
}

string GetLabelId()
{
	return itos(LabelId++);
}


void ClrSerialId()
{
	SerialId=0;
}
string GetSerialId()
{
	return itos(SerialId++);
}

string itos(int i)
{
	char cBuffer[10];
	int iPos,iSign;
	itoa(i,cBuffer,10);
	return cBuffer;
}

string rtos(double i)
{
	char* cBuffer;
	int iPos,iSign;
	cBuffer=ecvt(i,10,&iPos,&iSign);
	string szTmp=cBuffer;
	if (iPos>0)
	{
		szTmp.insert(szTmp.begin()+iPos,1,'.');
	}
	if (iSign==1)
	{
		szTmp='-'+szTmp;
	}
	return szTmp;
}

IRCode EmitIR(OpType eOpType,OpInfo Op1,OpInfo Op2,OpInfo Rslt)
{
	IRCode Tmp;
	Tmp.m_eOpType=eOpType;
	Tmp.m_Op1=Op1;
	Tmp.m_Op2=Op2;
	Tmp.m_Rslt=Rslt;
	Tmp.m_bEnabled=true;
	return Tmp;
}

IRCode EmitIR(OpType eOpType,OpInfo Op1,OpInfo Rslt)
{
	IRCode Tmp;
	Tmp.m_eOpType=eOpType;
	Tmp.m_Op1=Op1;
	Tmp.m_Rslt=Rslt;
	Tmp.m_bEnabled=true;
	return Tmp;
}

IRCode EmitIR(OpType eOpType,OpInfo Op1)
{
	IRCode Tmp;
	Tmp.m_eOpType=eOpType;
	Tmp.m_Op1=Op1;
	Tmp.m_bEnabled=true;
	return Tmp;
}

string GetIRStr(IRCode tmp)
{
	string s="("+GetOpStr(tmp.m_eOpType)+"              ";
	s=s.substr(0,15)+",";
	s=s+GetOp(tmp.m_Op1);
	s=s+",";
	s=s+GetOp(tmp.m_Op2);
	s=s+",";
	s=s+GetOp(tmp.m_Rslt);
	s=s+")";
	replace(s.begin(),s.end(),'\n',';');
	return s;
}
string GetOp(OpInfo Op)
{
	if (Op.m_iType==OpInfo::NONE)
	{
		return "null";
	}
	if (Op.m_iType==OpInfo::CONST)
	{
		if (SymbolTbl.ConstInfoTbl[Op.m_iLink].m_ConstType==ConstType::SET)
			return SetValue(SymbolTbl.ConstInfoTbl[Op.m_iLink].m_szName);
		else
			return SymbolTbl.ConstInfoTbl[Op.m_iLink].m_szName.c_str();
	}
	if (Op.m_iType==OpInfo::VAR || Op.m_iType==OpInfo::PTR)
	{
		string Tmp="";

		if (Op.m_bRef==true)
			Tmp="@";

		return Tmp+SymbolTbl.VarInfoTbl[Op.m_iLink].m_szName;

		//if (Op.m_eOffsetType==OffsetType::ConstOffset)
		//{

		//	return Tmp+SymbolTbl.VarInfoTbl.at(Op.m_iLink).m_szName+"."+SymbolTbl.ConstInfoTbl.at(Op.m_iOffsetLink).m_szName;
		//}
		//if (Op.m_eOffsetType==OffsetType::VarOffset)
		//{

		//	return Tmp+SymbolTbl.VarInfoTbl.at(Op.m_iLink).m_szName+"."+SymbolTbl.VarInfoTbl.at(Op.m_iOffsetLink).m_szName;
		//}

	}
	if (Op.m_iType==OpInfo::LABEL)
	{
		return SymbolTbl.LabelInfoTbl[Op.m_iLink].m_szName.c_str();
	}
	if (Op.m_iType==OpInfo::PROC)
	{
		return SymbolTbl.ProcInfoTbl.at(Op.m_iLink).m_szName.c_str();
	}
}

string SetValue(string szSetValue)
{
	string szHex;
	string szTmp;
	char c;

	for (int i=0;i<szSetValue.length()/2;i++)
	{
		c=szSetValue[i];
		szSetValue[i]=szSetValue[szSetValue.length()-i-1];
		szSetValue[szSetValue.length()-i-1]=c;
	}


	for (int i=0;i<=256;i++)
	{
		if (i!=256)
			szTmp+=szSetValue[i];
		if (szTmp.length()==4)
		{
			if (szTmp=="0000") szHex+="0";
			if (szTmp=="0001") szHex+="1";
			if (szTmp=="0010") szHex+="2";
			if (szTmp=="0011") szHex+="3";
			if (szTmp=="0100") szHex+="4";
			if (szTmp=="0101") szHex+="5";
			if (szTmp=="0110") szHex+="6";
			if (szTmp=="0111") szHex+="7";
			if (szTmp=="1000") szHex+="8";
			if (szTmp=="1001") szHex+="9";
			if (szTmp=="1010") szHex+="A";
			if (szTmp=="1011") szHex+="B";
			if (szTmp=="1100") szHex+="C";
			if (szTmp=="1101") szHex+="D";
			if (szTmp=="1110") szHex+="E";
			if (szTmp=="1111") szHex+="F";
			szTmp="";
		}
	}

	szTmp="";

	for (int i=0;i<32;i++)
	{
		string s=szHex.substr(i*2,2);
		if (s=="00")
			s="0";
		else
			if (s[0]>='A' && s[0]<='F')
				s="0"+s;
		szTmp=s+"h,"+szTmp;
	}
	szTmp=szTmp.substr(0,szTmp.length()-1);

	return szTmp;
}

string GetOpStr(int i)
{
	switch(i)
	{
	case EQU_1:	return "EQU_1";
	case EQU_S:	return "EQU_S";
	case EQU_4:	return "EQU_4";
	case EQU_8:	return "EQU_8";
	case EQU_8F:return "EQU_8F";
	case EQU_4F:return "EQU_4F";
	case EQU_2:	return "EQU_2";
	case EQU_A:	return "EQU_A";
	case EQU_32:return "EQU_32";

	case NEQU_1:return "NEQU_1";
	case NEQU_S:return "NEQU_S";
	case NEQU_4:return "NEQU_4";
	case NEQU_8:return "NEQU_8";
	case NEQU_8F:return "NEQU_8F";
	case NEQU_4F:return "NEQU_4F";
	case NEQU_2:return "NEQU_2";
	case NEQU_A:return "NEQU_A";
	case NEQU_32:return "NEQU_32";

	case LS_1:return "LS_1";
	case LS_S:return "LS_S";
	case LS_4:return "LS_4";
	case LS_8:return "LS_8";
	case LS_8F:return "LS_8F";
	case LS_4F:return "LS_4F";
	case LS_2:return "LS_2";
	case LS_1U:return "LS_1U";
	case LS_2U:return "LS_2U";
	case LS_4U:return "LS_2U";

	case MR_1:return "MR_1";
	case MR_S:return "MR_S";
	case MR_4:return "MR_4";
	case MR_8:return "MR_8";
	case MR_8F:return "MR_8F";
	case MR_4F:return "MR_4F";
	case MR_2:return "MR_2";
	case MR_1U:return "MR_1U";
	case MR_2U:return "MR_2U";
	case MR_4U:return "MR_2U";


	case LE_1:return "LE_1";
	case LE_S:return "LE_S";
	case LE_4:return "LE_4";
	case LE_8:return "LE_8";
	case LE_8F:return "LE_8F";
	case LE_4F:return "LE_4F";
	case LE_2:return "LE_2";
	case LE_1U:return "LE_1U";
	case LE_2U:return "LE_2U";
	case LE_4U:return "LE_2U";

	case ME_1:return "ME_1";
	case ME_S:return "ME_S";
	case ME_4:return "ME_4";
	case ME_8:return "ME_8";
	case ME_8F:return "ME_8F";
	case ME_4F:return "ME_4F";
	case ME_2:return "ME_2";
	case ME_1U:return "ME_1U";
	case ME_2U:return "ME_2U";
	case ME_4U:return "ME_2U";

	case IN_C:return "IN_C";
	case IN_I:return "IN_I";
	case IN_B:return "IN_B";
	case IN_SH:return "IN_SH";
	case IN_SM:return "IN_SM";
	case IN_W:return "IN_W";
	case IN_LW:return "IN_LW";
	case IN_E:return "IN_E";

	case SUB_4:return "SUB_4";
	case SUB_8F:return "SUB_8F";
	case SUB_8:return "SUB_8";
	case SUB_SET:return "SUB_SET";
	case SUB_2:return "SUB_2";


	case ADD_S:return "ADD_S";
	case ADD_4:return "ADD_4";
	case ADD_8F:return "ADD_8F";
	case ADD_SET:return "ADD_SET";
	case ADD_2:return "ADD_2";


	case OR_0:return "OR_0";
	case OR_4:return "OR_4";
	case OR_1:return "OR_1";
	case OR_2:return "OR_2";

	case XOR_0:return "XOR_0";
	case XOR_4:return "XOR_4";
	case XOR_1:return "XOR_1";
	case XOR_2:return "XOR_2";

	case MUL_4:return "MUL_4";
	case MUL_8F:return "MUL_8F";
	case MUL_SET:return "MUL_SET";

	case DIV_8F:return "DIV_8F";
	case DIV_4:return "DIV_4";

	case MOD_4:return "MOD_4";

	case SHR_4:return "SHR_4";
	case SHR_1U:return "SHR_1U";
	case SHR_1:return "SHR_1";
	case SHR_2:return "SHR_2";
	case SHR_2U:return "SHR_2U";
	case SHR_4U:return "SHR_4U";
	case SHL_4:return "SHL_4";
	case SHL_1U:return "SHL_1U";
	case SHL_1:return "SHL_1";
	case SHL_2:return "SHL_2";
	case SHL_2U:return "SHL_2U";
	case SHL_4U:return "SHL_4U";

	case AND_0:return "AND_0";
	case AND_4:return "AND_4";
	case AND_1:return "AND_1";
	case AND_2:return "AND_2";

	case NEG_4:return "NEG_4";
	case NEG_1:return "NEG_1";
	case NEG_2:return "NEG_2";
	case NEG_8F:return "NEG_8F";
	case NEG_4F:return "NEG_4F";

	case NOT_0:return "NOT_0";
	case NOT_4:return "NOT_4";
	case NOT_1:return "NOT_1";
	case NOT_2:return "NOT_2";

	case ASSIGN_1:return "ASSIGN_1";
	case ASSIGN_S:return "ASSIGN_S";
	case ASSIGN_4:return "ASSIGN_4";
	case ASSIGN_2:return "ASSIGN_2";
	case ASSIGN_8:return "ASSIGN_8";
	case ASSIGN_32:return "ASSIGN_32";
	case ASSIGN_N:return "ASSIGN_N";

	case JMP:return "JMP";
	case LABEL:return "LABEL";
	case PARA:return "PARA";
	case RETV:return "RETV";
	case CALL:return "CALL";
	case JT:return "JT";
	case JNT:return "JNT";
	case ASM:return "ASM";
	case GETADDR:return "GETADDR";
	case JE_1:return "JE_1";
	case JE_2:return "JE_2";
	case JE_4:return "JE_4";
	case GETPROCADDR:return "GETPROCADDR";

	case SET_ADDITEM:return "SET_ADDITEM";
	case CALLPTR:return  "CALLPTR";

	case None:return "None";
	case ChrToStr:return "ChrToStr";

	case ByteToInt:return "ByteToInt";
	case ShortToInt:return "ShortToInt";
	case SmallToInt:return "SmallToInt";
	case WordToInt:return "WordToInt";
	case LongToInt:return "LongToInt";

	case IntToReal:return "IntToReal";
	case ByteToReal:return "ByteToReal";
	case ShortToReal:return "ShortToReal";
	case SmallToReal:return "SmallToReal";
	case WordToReal:return "WordToReal";
	case LongToReal:return "LongToReal";
	case SingleToReal:return "SingleToReal";

	case IntToSingle:return "IntToSingle";
	case ByteToSingle:return "ByteToSingle";
	case ShortToSingle:return "ShortToSingle";
	case SmallToSingle:return "SmallToSingle";
	case WordToSingle:return "WordToSingle";
	case LongToSingle:return "LongToSingle";
	case RealToSingle:return "RealToSingle";

	case ByteToSmall:return "ByteToSmall";
	case ShortToSmall:return "ShortToSmall";
	case IntToSmall:return "IntToSmall";
	case LongToSmall:return "LongToSmall";

	case ByteToWord:return "ByteToWord";
	case ShortToWord:return "ShortToWord";
	case IntToWord:return "IntToWord";
	case LongToWord:return "LongToWord";

	case ByteToLong:return "ByteToLong";
	case WordToLong:return "WordToLong";
	case ShortToLong:return "ShortToLong";
	case SmallToLong:return "SmallToLong";

	case ByteToShort:return "ByteToShort";
	case IntToShort:return "IntToShort";
	case SmallToShort:return "SmallToShort";
	case WordToShort:return "WordToShort";
	case LongToShort:return "LongToShort";

	case IntToByte:return "IntToByte";
	case SmallToByte:return "SmallToByte";
	case WordToByte:return "WordToByte";
	case LongToByte:return "LongToByte";

	case ByteToLong8:return "ByteToLong8";
	case ShortToLong8:return "ShortToLong8";
	case SmallToLong8:return "SmallToLong8";
	case IntToLong8:return "IntToLong8";
	case WordToLong8:return "WordToLong8";
	case LongToLong8:return "LongToLong8";

	}
}



int GetVarLink(string szTmp)
{


	return atoi(szTmp.substr(szTmp.find('$')+1,szTmp.length()).c_str());
}

string AsmtoStr(AsmCode p)
{
	string szTmp="";
	if (!p.Lbl.empty())
		szTmp=p.Lbl+": "+szTmp;
	szTmp=szTmp+p.Op;
	szTmp=szTmp+" "+p.Op1;
	if (!p.Op2.empty())
		szTmp=szTmp+","+p.Op2;
	szTmp=szTmp+" "+p.Content;

	return szTmp;
}

void WriteProcAsm(vector<AsmCode> procAsm,ofstream &fout)
{
	for(int i=0;i<procAsm.size();i++)
	{
		fout<<AsmtoStr(procAsm[i])<<'\n';
	}
}
AsmCode InsertContent(string szContent)
{
	AsmCode tmp;
	tmp.Content=szContent;
	return tmp;
}

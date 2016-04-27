#include "Target.h"

extern CState State;

extern map<int,vector<AsmCode>> AsmCodeList;

extern CSymbolTbl SymbolTbl;

CTarget target;

CInstruction::CInstruction(string szInsStr,Pattern &Tmp)
{
	this->szInsStr=szInsStr;
	iPos=0;
	this->pattern=&Tmp;
}
void CInstruction::Trim()
{
	while (iPos<szInsStr.length() && szInsStr.at(iPos)<=0x20)
	{
		iPos++;
	}
}

void CInstruction::GetLabel(string &Label)
{
	int iTmp=iPos;
	string szTmp;
	while (iPos<szInsStr.length()
			&& szInsStr.at(iPos)!='\n'
			&& szInsStr.at(iPos)!=';'
			&& szInsStr.at(iPos)!=':'
			&& szInsStr.at(iPos)!=',')
	{
		szTmp.push_back(szInsStr.at(iPos));
		iPos++;
	}

	if (iPos<szInsStr.length() && szInsStr.at(iPos)==':')
	{
		if (!szTmp.empty())
		{
			Label=szTmp;
			iPos++;
		}
	}
	else
	{
		iPos=iTmp;
	}
}
void CInstruction::GetCallFunc(string &FuncName)
{
	string szTmp;
	szTmp="";
	if (iPos>=szInsStr.length())
	{
		return;
	}

	if (szInsStr.at(iPos)!='/')
	{
		return;
	}

	while (iPos<szInsStr.length() && szInsStr.at(iPos)!='\n')
	{
		szTmp.push_back(szInsStr.at(iPos));
		iPos++;
	}

	if (!szTmp.empty())
	{
		FuncName=szTmp;
	}
}

void CInstruction::GetContent(string &Content)
{
	string szTmp;
	szTmp="";

	if (iPos>=szInsStr.length())
	{
		return;
	}

	if (szInsStr.at(iPos)!=';')
	{
		return;
	}

	while (iPos<szInsStr.length() && szInsStr.at(iPos)!='\n')
	{
		szTmp.push_back(szInsStr.at(iPos));
		iPos++;
	}

	if (!szTmp.empty())
	{
		Content=szTmp;
	}
}
void CInstruction::GetOperation(string &Op)
{
	string szTmp;
	szTmp="";

	while (iPos<szInsStr.length() && szInsStr.at(iPos)>0x20)
	{
		szTmp.push_back(szInsStr.at(iPos));
		iPos++;
	}
	if (!szTmp.empty())
	{
		Op=szTmp;
	}
}

void CInstruction::GetOpData(vector<string> &OpData)
{
	string szTmp;
	szTmp="";
	if (iPos>=szInsStr.length())
		return;
	if (szInsStr.at(iPos)==',')
		iPos++;
	while (iPos<szInsStr.length()
		&& szInsStr.at(iPos)!=','
		&& szInsStr.at(iPos)!='\n'
		&& szInsStr.at(iPos)!=';')
	{
		if (szInsStr.at(iPos)=='%')
		{
			if (!szTmp.empty())
			{
				if (szTmp.at(0)=='%')
				{
					szTmp.push_back(szInsStr.at(iPos++));
				}

				OpData.push_back(szTmp);
			}

			Trim();
			szTmp="";

			if (iPos>=szInsStr.length()
				|| szInsStr.at(iPos)==','
				|| szInsStr.at(iPos)=='\n'
				|| szInsStr.at(iPos)==';')
			{
				break;
			}
		}
		szTmp.push_back(szInsStr.at(iPos++));
	}

	if (!szTmp.empty())
	{
		OpData.push_back(szTmp);
	}
}

void CInstruction::GetIns()
{
	Code TmpCode;

	Trim();

	if (iPos>=szInsStr.length())
	{
		return;
	}

	if (szInsStr.at(iPos)==';')
	{
		GetContent(TmpCode.Comment);
	}
	else if (szInsStr.at(iPos)=='/')
	{
		GetCallFunc(TmpCode.Op);
	}
	else
	{
		GetLabel(TmpCode.Label);
		GetOperation(TmpCode.Op);
		GetOpData(TmpCode.Operand1);
		GetOpData(TmpCode.Operand2);
		GetOpData(TmpCode.Operand3);
		GetContent(TmpCode.Comment);
	}

	pattern->CodeList.push_back(TmpCode);
}

void CCodePattern::Recognition(vector<string> PatternStr)
{
	Pattern Tmp;

	if (PatternStr.size()<4
		|| PatternStr.front().compare("{")!=0
		|| PatternStr.back().compare("}")!=0)
	{
		return;
	}

	Tmp.szPattern=PatternStr.at(1);
	Tmp.iPrior=atoi(PatternStr.at(2).c_str());

	int i=3;

	for(;i<PatternStr.size()-1 && !PatternStr.at(i).empty();i++)
	{
		if (PatternStr.at(i)[0]=='@')
		{
			string szTmp=PatternStr.at(i);
			RegFlg TmpFlg;

			TmpFlg.cFlg=-1;
			TmpFlg.cReg=-1;
			TmpFlg.iSize=-1;

			if (szTmp[1]<='9' && szTmp[1]>='0')
			{
				TmpFlg.cFlg=szTmp[1]-'0';
			}
			else
			{
				continue;
			}

			szTmp=szTmp.substr(2);
			int iPos = szTmp.find(',');

			TmpFlg.iSize = atoi(szTmp.substr(iPos+1).c_str());
			szTmp = szTmp.substr(0,iPos);

			if (szTmp=="div")
			{
				TmpFlg.cFlg=1;
				TmpFlg.cReg=-2;
			}
			else
			{
				for(int j=0;j<18;j++)
				{
					if (RegName[j]==szTmp)
					{
						TmpFlg.cFlg=1;
						TmpFlg.cReg=j;
						break;
					}
				}
			}

			Tmp.Flg.push_back(TmpFlg);
		}
		else if (PatternStr.at(i)[0]=='#')
		{
			string szConflict=PatternStr.at(i).substr(1,PatternStr.at(i).length()-1);
			string szTmp="";

			for(int j=0;j<szConflict.length();j++)
			{
				if (szConflict[j]==',')
				{
					for(int k=0;k<18;k++)
					{
						if (RegName[k]==szTmp)
						{
							Tmp.SaveReg.push_back(k);
							break;
						}
					}
					szTmp="";
				}
				else
				{
					szTmp=szTmp+szConflict[j];
				}
			}
		}
		else
		{
			break;
		}
	}

	for(;i<PatternStr.size()-1;i++)
	{
		CInstruction Ins(PatternStr.at(i),Tmp);
		Ins.GetIns();
	}

	PatternList.insert(pair<string,Pattern>(Tmp.szPattern,Tmp));
}

void CCodePattern::ReadPattern(const char* pFileName)
{
	//��ȡָ��ģ��

	string szPatternStr=FileRead(pFileName);
	string szTmp="";
	vector<string> PatternStr;

	for (string::iterator it=szPatternStr.begin();it!=szPatternStr.end();it++)
	{
		if (*it=='\n')
		{
			if (!szTmp.empty())
			{
				PatternStr.push_back(szTmp);
			}
			if (szTmp.compare("}")==0)
			{
				Recognition(PatternStr);
				PatternStr.clear();
			}

			szTmp="";
		}
		else
		{
			szTmp.push_back(*it);
		}
	}
}


bool CCodePattern::SearchPattern(string szStr,Pattern &Ret)
{
	if (PatternList.count(szStr)!=0)
	{
		Ret=PatternList[szStr];
		return true;
	}
	else
	{
		return false;
	}

}
int CTarget::RegSeek(OpInfo op,int iSize)
{
	//�����Ĵ�������ر�����
	for(map<int,list<OpInfo>>::iterator it=RegVal.begin();it!=RegVal.end();it++)
	{
		for(list<OpInfo>::iterator it1=it->second.begin();it1!=it->second.end();it1++)
		{
			if (it1->m_iType==OpInfo::VAR
				&&  it1->m_iLink==op.m_iLink)
			{
				if (Reg2Size[it->first]==iSize)
				{
					return it->first;
				}
				//else
				//{
				//	if (Reg2Size[it->first]>iSize && it->first<8)
				//	{
				//		return RegRela[it->first][iSize-1];
				//	}

				//	SaveReg(it->first);
				//}
			}
		}
	}

	return -1;
}

void CTarget::RegForbid(int iReg)
{
	for (int i=0;i<5;i++)
	{
		if (RegRela[iReg][i]==-1)
		{
			break;
		}
		Forbid.insert(RegRela[iReg][i]);
	}
	Forbid.insert(iReg);
}

int CTarget::RegAlloc(int iSize,int iTarget)
{
	if (iTarget!=-1
		&& iTarget!=-2)
	{
		return iTarget;
	}

	int j=iSize/2;

	int iReg=-1;

	set<int> TmpSet;

	static int AllocCursor[3]={0,0,0};

	int iTmpCursor=AllocCursor[j];

	int iTmpReg=-1;

	for (;TmpSet.count(iTmpReg)==0;iTmpCursor=((iTmpCursor+1)%8))
	{
		if(iTmpReg!=-1)
		{
			TmpSet.insert(iTmpReg);
		}

		if (Size2Reg[j][iTmpCursor] == -1)
		{
			continue;
		}

		if (iTarget==-2
			&& (Size2Reg[j][iTmpCursor]==ESI
			|| Size2Reg[j][iTmpCursor]==EDI))
		{
			continue;
		}

		iTmpReg=Size2Reg[j][iTmpCursor];

		if (Forbid.count(iTmpReg)==0
		&& (RegVal.count(iTmpReg)==0
		|| RegVal[iTmpReg].empty()))
		{
			iReg=iTmpReg;
			break;
		}

	}

	TmpSet.clear();
	iTmpReg=-1;

	if (iReg==-1)
	{
		for (;TmpSet.count(iTmpReg)==0;iTmpCursor=((iTmpCursor+1)%8))
		{
			if(iTmpReg!=-1)
			{
				TmpSet.insert(iTmpReg);
			}

			if (Size2Reg[j][iTmpCursor] == -1)
			{
				continue;
			}

			if (iTarget==-2
				&& (Size2Reg[j][iTmpCursor]==ESI
				|| Size2Reg[j][iTmpCursor]==EDI))
			{
				continue;
			}

			iTmpReg = Size2Reg[j][iTmpCursor];

			if (Forbid.count(iTmpCursor)==0)
			{
				iReg=iTmpReg;
				break;
			}
		}
	}

	AllocCursor[j]=(iTmpCursor+1)%18;

	if (iReg==-1)
	{
		EmitError("out of register");
	}

	return iReg;
}
void CTarget::LoadReg(int iReg,OpInfo op)
{
	int iSize=Reg2Size[iReg];
	vector<string> ParaStr;
	ParaStr.push_back(GetOpDataStr(op));
	ParaStr.push_back("");
	ParaStr.push_back(RegName[iReg]);
	IRtoAsmList("#MOV_"+itos(iSize)+" %V.1% %V.3%",ParaStr);

	if (op.m_iType != OpInfo::CONST)
	{
		SetVal(iReg,op);
	}
}

bool CTarget::IsDeadVar(OpInfo op)
{
	if (!pCurrBlock
		|| (SymbolTbl.VarInfoTbl[op.m_iLink].m_iProcIndex == 0
		&&	!SymbolTbl.IsTmpVar(op.m_iLink))
		|| SymbolTbl.VarInfoTbl[op.m_iLink].m_eRank == VarInfo::RET)
	{
		return false;
	}

	map<int,int> VarMap=GetVar2IdMap(CurrProcId);

	if (VarMap.count(op.m_iLink) != 0)
	{
		int iTmp = VarMap[op.m_iLink];

		if (pCurrBlock->OutSet->value(iTmp))
		{
			return false;
		}
		else
		{
			for(vector<int>::iterator it=op.m_duChain.begin();it!=op.m_duChain.end();it++)
			{
				if ((*it)>= CurrIR && (*it) <= pCurrBlock->iEnd)
				{
					return false;
				}
			}
		}
	}
	return true;
}

void CTarget::SaveReg(int iReg,bool bFlg)
{
	set<int> TmpSet;

	for(list<OpInfo>::iterator it=RegVal[iReg].begin();it!=RegVal[iReg].end();)
	{
		//����������Ҫ�ش�
		if (it->m_iType!=OpInfo::VAR
			|| (bFlg && IsDeadVar(*it)))
		{
			RegVal[iReg].erase(it++);

			continue;
		}

		//�������ݲ������ظ��ش�
		if (TmpSet.count(it->m_iLink)==0)
		{
			TmpSet.insert(it->m_iLink);
			int iSize=Reg2Size[iReg];
			vector<string> ParaStr;
			ParaStr.push_back(RegName[iReg]);
			ParaStr.push_back("");
			ParaStr.push_back(GetOpDataStr(*it));
			IRtoAsmList("#MOV_"+itos(iSize)+" %V.1% %V.3%",ParaStr);
		}

		RegVal[iReg].erase(it++);
	}
}

void CTarget::SaveRegs(int iReg,bool bFlg)
{
	for(int i=0;i<5 && RegRela[iReg][i]!=-1;i++)
	{
		//�ش���ؼĴ���������
		SaveReg(RegRela[iReg][i],bFlg);

		//�����ؼĴ����ı�����
		RegVal[RegRela[iReg][i]].clear();
	}

	//�ش浱ǰ�Ĵ���������
	SaveReg(iReg,bFlg);

	//��յ�ǰ�Ĵ���������
	RegVal[iReg].clear();
}

int CTarget::RegSpill(int iReg)
{
	for(int i=0;i<5 && RegRela[iReg][i]!=-1;i++)
	{
		SaveReg(RegRela[iReg][i]);
		RegVal[RegRela[iReg][i]].clear();
	}
	return iReg;
}

int CTarget::RegSpill(int iReg,OpInfo op)
{
	for(int i=0;i<5 && RegRela[iReg][i]!=-1;i++)
	{
		SaveReg(RegRela[iReg][i]);
		RegVal[RegRela[iReg][i]].clear();
	}
	return iReg;
}
void CTarget::RegRefSave()
{
	for(map<int,list<OpInfo>>::iterator it=RegVal.begin();it!=RegVal.end();it++)
	{
		SaveReg(it->first);
		it->second.clear();
	}
}
void CTarget::RegClearAll()
{
	for(map<int,list<OpInfo>>::iterator it=RegVal.begin();it!=RegVal.end();it++)
	{
		it->second.clear();
	}
}

int CTarget::OpRef(OpInfo tmp,int iReg,int iSize)
{
	vector<string> TmpPara;
	int iTmpReg;

	//�����ݴ浱ǰ�������ļĴ���
	iTmpReg=RegSeek(tmp,4);

	//��ǰ�������������ڼĴ����У�������һ���Ĵ�����������������
	if (iTmpReg==-1)
	{
		iTmpReg=RegAlloc(4);
		SaveRegs(iTmpReg);
		TmpPara.push_back(GetOpDataStr(tmp));
		TmpPara.push_back("");
		TmpPara.push_back(RegName[iTmpReg]);
		IRtoAsmList("#MOV_4 %V.1% %V.3%",TmpPara);
		TmpPara.clear();
	}

	RegRefSave();	//��Ҫ�ش����мĴ�������

	TmpPara.push_back("["+RegName[iTmpReg]+"]");
	TmpPara.push_back("");

	//����һ���洢��ӷ��ʽ���ļĴ���
	iTmpReg=RegAlloc(iSize,iReg);
	TmpPara.push_back(RegName[iTmpReg]);

	//���ɼ�ӷ���ָ��
	IRtoAsmList("#MOV_"+itos(iSize)+" %V.1% %V.3%",TmpPara);

	return iTmpReg;
}

void CTarget::SetVal(int iReg,OpInfo op)
{
	if (op.m_iType!=OpInfo::VAR)
	{
		return;
	}

	for (map<int,list<OpInfo>>::iterator it=RegVal.begin();it!=RegVal.end();it++)
	{
		for (list<OpInfo>::iterator it1=it->second.begin();it1!=it->second.end();)
		{
			if (it1->m_iType==OpInfo::VAR && it1->m_iLink==op.m_iLink)
				it->second.erase(it1++);
			else
				it1++;

		}
	}
	RegVal[iReg].push_back(op);
}


void CTarget::RegPrint()
{
	//����Ĵ����������
	for(map<int,list<OpInfo>>::iterator it=RegVal.begin();it!=RegVal.end();it++)
	{
		string szTmp=";";

		if (!it->second.empty())
		{
			szTmp=szTmp+RegName[it->first]+":";

			for(list<OpInfo>::iterator it1=it->second.begin();it1!=it->second.end();it1++)
			{
				szTmp=szTmp+GetOp(*it1)+",";
			}

			RecContentAsm(szTmp);
		}
	}

}

bool CTarget::IRtoASM()
{
	//����������
	CDataFlowAnalysis::DataFlowAnalysis();

	pCurrBlock = NULL;

	//��ȡָ��ģ��
	GenInit(State.m_szSysDir+"AsmSchm.txt");

	CurrProcId=-1;

	//���asm�б��ָ�룬���µ�asm���뽫����CurrAsmCodeListָ����б���
	CurrAsmCodeList=GetAsmCodeList(CurrProcId);

	//����asm�ļ��ײ�����
	IRtoAsmList("FILESTART",ParaStr1);

	//���ɰ���ϵͳ������
	GetLibAsm();

	//���ɷ�������
	MemoryAlloc();

	//�������ʼ
	RecOpAsm(".code");

	for (int i=0;i<SymbolTbl.ProcInfoTbl.size();i++)
	{
		//���ù��̡���������Ҫ����asm
		if (SymbolTbl.ProcInfoTbl[i].m_bUsed==false)
		{
			continue;
		}

		//�ⲿ���̡���������Ҫ����asm
		if (SymbolTbl.ProcInfoTbl.at(i).m_eFlag==ProcInfo::Extern)
		{
			continue;
		}

		//�������̡���������Ҫ����asm
		if (SymbolTbl.ProcInfoTbl.at(i).m_szName=="_noname")
		{
			continue;
		}

		CurrProcId=i;

		//���asm�б��ָ�룬���µ�asm���뽫����CurrAsmCodeListָ����б���
		CurrAsmCodeList=GetAsmCodeList(CurrProcId);

		RecContentAsm(";function:"+SymbolTbl.ProcInfoTbl.at(i).m_szName);
		RecContentAsm(";process of start");

		if (i==0)
		{
			//�������������
			IRtoAsmList("MAINSTART",ParaStr1);
		}

		ParaStr1.clear();
		ParaStr1.push_back(itos(GetProcVarSize(i)));	//��øú����ľֲ������Ĵ�С
		ParaStr1.push_back("_"+SymbolTbl.ProcInfoTbl.at(i).m_szName);

		//���ɺ�������ֳ�����
		IRtoAsmList("FUNCSTART %V.1% %V.2%",ParaStr1);

		//��ȡ��ǰ��������ͼ
		vector<CBasicBlock> Blocks=BasicBlock[i];

		//������ͼ�Ļ����飬�Ի�����Ϊ��λ����asm
		for(int j=0;j<Blocks.size();j++,RegClearAll())
		{
			pCurrBlock = &(Blocks[j]);

			//�����������ڵ�����IR
			for (int p=Blocks[j].iStart;p<=Blocks[j].iEnd;p++)
			{
				int iSize,iReg1,iReg2,iReg3;

				vector<int> ExtReg;

				CurrIR = p;

				IRCode * tmp=&(SymbolTbl.ProcInfoTbl[i].m_Codes[p]);

				//����ע��
				RecContentAsm(";"+GetIRStr(*tmp));

				if (tmp->m_eOpType==PARA)
				{
					GenParaAsm(*tmp);
					continue;
				}
				else if (tmp->m_eOpType==ASSIGN_N)
				{
					GenAssignNAsm(*tmp);
					continue;
				}
				else if (tmp->m_eOpType==ASM)
				{
					string szIncludeAsm=SymbolTbl.ConstInfoTbl[tmp->m_Op1.m_iLink].m_szName;
					RecOpAsm(szIncludeAsm.substr(1,szIncludeAsm.length()-2),tmp->m_Op2);
					continue;
				}
				else if (tmp->m_eOpType==RETV)
				{
					GenRetAsm(*tmp);
					continue;
				}

				//����IR������ģʽ��������ƥ����Ӧ��ָ��ģʽ
				string szPatternStr=GetOpStr(tmp->m_eOpType);

				if (tmp->m_eOpType>>4<<4 == OpType::IN)
				{
					szPatternStr="IN";
				}

				string szTmp=GetOpDataPattern(tmp->m_Op1);

				if (szTmp!="")
				{
					szPatternStr=szPatternStr+" %"+szTmp+".1%";
				}

				szTmp=GetOpDataPattern(tmp->m_Op2);

				if (szTmp!="")
				{
					szPatternStr=szPatternStr+" %"+szTmp+".2%";
				}

				szTmp=GetOpDataPattern(tmp->m_Rslt);

				if (szTmp!="")
				{
					szPatternStr=szPatternStr+" %"+szTmp+".3%";
				}

				Pattern AsmPattern;

				//����ģʽ����ƥ����Ӧ��ָ��ģʽ
				if (CodePattern.SearchPattern(szPatternStr,AsmPattern))
				{
					ParaStr1.clear();
					ExtReg.clear();
					Forbid.clear();

					//��ȡ��ģʽ�ı����Ĵ�����������Щָ��ԼĴ��������ض������
					for(int i=0;i<AsmPattern.SaveReg.size();i++)
					{
						SaveRegs(i);	//�ش汣���Ĵ���������
						RegForbid(i);	//�����Ĵ����ǲ������������
					}

					//����ģʽ���������Ҫ�ļĴ�������Щ�Ĵ����Ǹ�ģʽ���еģ��������ٷ����
					for(int i=3;i<AsmPattern.Flg.size();i++)
					{
						int j=RegAlloc(AsmPattern.Flg[i].iSize,AsmPattern.Flg[i].cReg);
						SaveRegs(j);	//�ش汣���Ĵ���������
						RegForbid(j);	//�����Ĵ����ǲ������������
						ExtReg.push_back(j);
					}

					//���������1
					if (tmp->m_Op1.m_iType==OpInfo::VAR)
					{
						//��ȡ������1��size
						iSize = AsmPattern.Flg[0].iSize;

						if (tmp->m_Op1.m_bRef)
						{
							//����Ǽ�ӷ��ʣ������ӷ��ʵĽ��������
							iReg1=OpRef(tmp->m_Op1,AsmPattern.Flg[0].cReg,iSize);

							//��ֹ������Ĵ����ٷ���
							RegForbid(iReg1);

							ParaStr1.push_back(RegName[iReg1]);
						}
						else
						{
							if (AsmPattern.Flg[0].cFlg==0)	//ֱ��Ѱַ������Ҫ����Ĵ���
							{
								//���������Ҫ����Ӧ�ļĴ���ֵ�ش棬�ٴ��ڴ��ȡ

								iReg1=RegSeek(tmp->m_Op1,iSize);

								//����ò����������мĴ�������ش�����
								if (iReg1!=-1)
								{
									SaveReg(iReg1);
								}

								ParaStr1.push_back(GetOpDataStr(tmp->m_Op1));

								iReg1=-1;
							}

							if (AsmPattern.Flg[0].cFlg==1)	//�Ĵ���Ѱַ
							{
								//����������뱣֤�������ڼĴ���

								iReg1=RegSeek(tmp->m_Op1,iSize);

								//���������δ�����мĴ����������Ĵ���������
								if (iReg1==-1)
								{
									//���ݲ�������size���������һ���Ĵ���
									iReg1=RegAlloc(iSize,AsmPattern.Flg[0].cReg);

									//������õ��ļĴ����ڵ����ݻش�
									SaveRegs(iReg1);

									//���ڴ���������Ĵ���
									LoadReg(iReg1,tmp->m_Op1);
								}

								//��ֹ�Ĵ����ٷ���
								RegForbid(iReg1);

								ParaStr1.push_back(RegName[iReg1]);
							}

							if (AsmPattern.Flg[0].cFlg==2)	//��ǿ�ƼĴ���Ѱַ
							{
								//��������£����ݿ����������ڼĴ������ڴ棬��ǿ������

								iReg1=RegSeek(tmp->m_Op1,iSize);

								//���������δ�����мĴ���������ڴ���ʣ�����ӼĴ�������
								if (iReg1==-1)
								{
									ParaStr1.push_back(GetOpDataStr(tmp->m_Op1));
								}
								else
								{
									ParaStr1.push_back(RegName[iReg1]);
								}
							}
						}
					}
					else
					{
						if (tmp->m_Op1.m_iType == OpInfo::CONST
							&& AsmPattern.Flg[0].cFlg!=0)		//����������Ĵ���Ѱַ
						{
							iSize = AsmPattern.Flg[0].iSize;
							vector<string> TmpPara;

							//���ݲ�������size���������һ���Ĵ���
							iReg1=RegAlloc(iSize);

							//������õ��ļĴ����ڵ����ݻش�
							SaveRegs(iReg1);

							//��ֹ�Ĵ����ٷ���
							RegForbid(iReg1);

							//���ڴ���������Ĵ���
							LoadReg(iReg1,tmp->m_Op1);

							ParaStr1.push_back(RegName[iReg1]);
						}
						else
						{
							//������ֱ��Ѱַ

							if (tmp->m_Op1.m_iType == OpInfo::NONE)
							{
								ParaStr1.push_back("");
							}
							else
							{
								ParaStr1.push_back(GetOpDataStr(tmp->m_Op1));
							}
						}
					}

					//���������2
					if (tmp->m_Op2.m_iType==OpInfo::VAR)
					{
						//��ȡ������2��size
						iSize = AsmPattern.Flg[1].iSize;

						if (tmp->m_Op2.m_bRef)
						{
							//����Ǽ�ӷ��ʣ������ӷ��ʵĽ��������
							iReg2=OpRef(tmp->m_Op2,AsmPattern.Flg[1].cReg,iSize);

							//��ֹ������Ĵ����ٷ���
							RegForbid(iReg2);

							ParaStr1.push_back(RegName[iReg2]);
						}
						else
						{
							if (AsmPattern.Flg[1].cFlg==0)	//ֱ��Ѱַ������Ҫ����Ĵ���
							{
								//���������Ҫ����Ӧ�ļĴ���ֵ�ش棬�ٴ��ڴ��ȡ
								iReg2=RegSeek(tmp->m_Op2,iSize);

								//����ò����������мĴ�������ش�����
								if (iReg2!=-1)
								{
									SaveReg(iReg2);
								}

								ParaStr1.push_back(GetOpDataStr(tmp->m_Op2));

								iReg2=-1;
							}
							if (AsmPattern.Flg[1].cFlg==1)	//�Ĵ���Ѱַ
							{
								//����������뱣֤�������ڼĴ���

								iReg2=RegSeek(tmp->m_Op2,iSize);

								//���������δ�����мĴ����������Ĵ���������
								if (iReg2==-1)
								{
									//���ݲ�������size���������һ���Ĵ���
									iReg2=RegAlloc(iSize,AsmPattern.Flg[1].cReg);

									//������õ��ļĴ����ڵ����ݻش�
									SaveRegs(iReg2);

									//���ڴ���������Ĵ���
									LoadReg(iReg2,tmp->m_Op2);
								}

								//��ֹ�Ĵ����ٷ���
								RegForbid(iReg2);

								ParaStr1.push_back(RegName[iReg2]);
							}
							if (AsmPattern.Flg[1].cFlg==2)	//��ǿ�ƼĴ���Ѱַ
							{
								//��������£����ݿ����������ڼĴ������ڴ棬��ǿ������

								iReg2=RegSeek(tmp->m_Op2,iSize);

								//���������δ�����мĴ���������ڴ���ʣ�����ӼĴ�������
								if (iReg2==-1)
								{
									ParaStr1.push_back(GetOpDataStr(tmp->m_Op2));
								}
								else
								{
									ParaStr1.push_back(RegName[iReg2]);
								}
							}
						}
					}
					else
					{
						if (tmp->m_Op2.m_iType==OpInfo::CONST
							&& AsmPattern.Flg[1].cFlg!=0)		//����������Ĵ���Ѱַ
						{
							iSize = AsmPattern.Flg[1].iSize;
							vector<string> TmpPara;

							//���ݲ�������size���������һ���Ĵ���
							iReg2=RegAlloc(iSize);

							//������õ��ļĴ����ڵ����ݻش�
							SaveRegs(iReg2);

							//��ֹ�Ĵ����ٷ���
							RegForbid(iReg2);

							//���ڴ���������Ĵ���
							LoadReg(iReg2,tmp->m_Op2);

							ParaStr1.push_back(RegName[iReg2]);
						}
						else
						{
							//������ֱ��Ѱַ

							if (tmp->m_Op2.m_iType == OpInfo::NONE)
							{
								ParaStr1.push_back("");
							}
							else
							{
								ParaStr1.push_back(GetOpDataStr(tmp->m_Op2));
							}
						}
					}

					//������������
					if (tmp->m_Rslt.m_iType==OpInfo::VAR)
					{
						iSize = AsmPattern.Flg[2].iSize;

						vector<string> TmpPara;

						string szTmp = GetOpDataStr(tmp->m_Rslt);

						if (tmp->m_Rslt.m_bRef)
						{
							//�������һ���Ĵ���
							iReg3=RegAlloc(4);

							//������õ��ļĴ����ڵ����ݻش�
							SaveRegs(iReg3);

							//��ֹ�Ĵ����ٷ���
							RegForbid(iReg3);

							//�󶨼Ĵ���
							SetVal(iReg3,tmp->m_Rslt);

							TmpPara.push_back(GetOpDataStr(tmp->m_Rslt));
							TmpPara.push_back("");
							TmpPara.push_back(RegName[iReg3]);

							//�ش����мĴ���������
							RegRefSave();

							IRtoAsmList("#MOV_4 %V.1% %V.3%",TmpPara);

							ParaStr1.push_back("["+RegName[iReg3]+"]");

							if (szPatternStr == "ASSIGN_4 %V.1% %V.3%")
							{
								szPatternStr = "#MOV_4 %V.1% %V.3%";
							}

							if (szPatternStr == "ASSIGN_2 %V.1% %V.3%")
							{
								szPatternStr = "#MOV_2 %V.1% %V.3%";
							}

							if (szPatternStr == "ASSIGN_1 %V.1% %V.3%")
							{
								szPatternStr = "#MOV_1 %V.1% %V.3%";
							}
						}
						else
						{
							if (AsmPattern.Flg[2].cFlg==0)	//ֱ��Ѱַ������Ҫ����Ĵ���
							{
								iReg3=-1;
							}

							if (AsmPattern.Flg[2].cFlg==1)	//������һ���Ĵ��������ڴ洢������
							{
								//�������һ���Ĵ���
								iReg3=RegAlloc(iSize,AsmPattern.Flg[2].cReg);

								//������õ��ļĴ����ڵ����ݻش�
								SaveRegs(iReg3);

								//��ֹ�Ĵ����ٷ���
								RegForbid(iReg3);

								//�󶨼Ĵ���
								SetVal(iReg3,tmp->m_Rslt);

								szTmp=RegName[iReg3];
							}

							if (AsmPattern.Flg[2].cFlg==2)	//���������ڲ�����2�������Ĵ�����
							{
								//���Ĵ����ڵ����ݻش�
								SaveRegs(iReg2);

								//�󶨼Ĵ���
								SetVal(iReg2,tmp->m_Rslt);
							}

							if (AsmPattern.Flg[2].cFlg==3)	//���������ڲ�����1�������Ĵ����У�Ӱ��ԭֵ
							{
								//���Ĵ����ڵ����ݻش�
								SaveRegs(iReg1);

								//�󶨼Ĵ���
								SetVal(iReg1,tmp->m_Rslt);
							}

							if (AsmPattern.Flg[2].cFlg==4)	//���������ڲ�����1�������Ĵ����У���Ӱ��ԭֵ
							{
								//�󶨼Ĵ���
								SetVal(iReg1,tmp->m_Rslt);
							}

							ParaStr1.push_back(szTmp);
						}
					}
					else
					{
						ParaStr1.push_back(GetOpDataStr(tmp->m_Rslt));
					}

					//��ģʽ��Ҫ�Ķ���Ĵ���
					for(vector<int>::iterator it=ExtReg.begin();it!=ExtReg.end();it++)
					{
						ParaStr1.push_back(Name2Reg[*it]);
					}

					//����ָ��
					IRtoAsmList(szPatternStr,ParaStr1);

					LabelMap.clear();

					//����Ĵ����������
					RegPrint();
				}

				//�ش����мĴ���������
				RegRefSave();
			}
		}
		RecContentAsm(";process of end");

		if (SymbolTbl.ProcInfoTbl.at(i).m_eType==ProcInfo::Type::Function)
		{
			int iRetSize=SymbolTbl.TypeInfoTbl[SymbolTbl.ProcInfoTbl.at(i).m_iReturnType].m_iSize;
			ParaStr1.clear();
			ParaStr1.push_back("ebp+_"+SymbolTbl.ProcInfoTbl.at(i).m_szName+"$RESULT");
			if (iRetSize<=4)
				IRtoAsmList("FUNCRET4 %V.1%",ParaStr1);
			else if (iRetSize<=8)
				IRtoAsmList("FUNCRET8 %V.1%",ParaStr1);
			else
				IRtoAsmList("FUNCRETA %V.1%",ParaStr1);
		}
		ParaStr1.clear();
		ParaStr1.push_back(itos(GetProcParaSize(i)));
		ParaStr1.push_back("_"+SymbolTbl.ProcInfoTbl.at(i).m_szName);
		IRtoAsmList("FUNCEND %V.1% %V.2%",ParaStr1);
	}

	CurrProcId=-2;

	CurrAsmCodeList=GetAsmCodeList(CurrProcId);

	IRtoAsmList("FILEEND",ParaStr1);

	return true;
}

void CTarget::GenInit(string szFileName)
{
	CodePattern.ReadPattern(szFileName.c_str());

	for(int i=0;i<18;i++)
	{
		Reg2Name.insert(pair<string,int>(RegName[i],i));
		Name2Reg.insert(pair<int,string>(i,RegName[i]));
	}
}
string CTarget::GetTmpLabel(string szPattern)
{
	map<string,string>::iterator it=LabelMap.find(szPattern);
	if (it==LabelMap.end())
	{
		string szTmp=SymbolTbl.LabelInfoTbl[SymbolTbl.GetTmpLabel(CurrProcId)].m_szName;
		LabelMap.insert(pair<string,string>(szPattern,szTmp));
		return szTmp;
	}
	else
	{
		return it->second;
	}
}
bool CTarget::IsReg(string szOp)
{
	if (szOp=="eax" || szOp=="ebx" || szOp=="ecx" || szOp=="edx" ||
		szOp=="ax" || szOp=="bx" || szOp=="cx" || szOp=="dx")
	{
		return true;
	}
	else
	{
		return false;
	}
}

string CTarget::RegDivision(string szReg,int iFlg)
{
	if (iFlg==-1)
	{
		return szReg;
	}

	if (IsReg(szReg))
	{
		int iReg=Reg2Name[szReg];

		switch(iFlg)
		{
		case 1:
			{
				switch(iReg)
				{
				case EAX:case AX:case AH:case AL:	return Name2Reg[AL];
				case EBX:case BX:case BH:case BL:	return Name2Reg[BL];
				case ECX:case CX:case CH:case CL:	return Name2Reg[CL];
				case EDX:case DX:case DH:case DL:	return Name2Reg[DL];
				default:return szReg;
				}
				break;
			}
		case 2:
			{
				switch(iReg)
				{
				case EAX:case AX:case AH:case AL:	return Name2Reg[AH];
				case EBX:case BX:case BH:case BL:	return Name2Reg[BH];
				case ECX:case CX:case CH:case CL:	return Name2Reg[CH];
				case EDX:case DX:case DH:case DL:	return Name2Reg[DH];
				default:return szReg;
				}
				break;
			}
		case 3:
			{
				switch(iReg)
				{
				case EAX:case AX:case AH:case AL:	return Name2Reg[AX];
				case EBX:case BX:case BH:case BL:	return Name2Reg[BX];
				case ECX:case CX:case CH:case CL:	return Name2Reg[CX];
				case EDX:case DX:case DH:case DL:	return Name2Reg[DX];
				default:return szReg;
				}
				break;
			}
		case 4:
			{
				switch(iReg)
				{
				case EAX:case AX:case AH:case AL:	return Name2Reg[EAX];
				case EBX:case BX:case BH:case BL:	return Name2Reg[EBX];
				case ECX:case CX:case CH:case CL:	return Name2Reg[ECX];
				case EDX:case DX:case DH:case DL:	return Name2Reg[EDX];
				default:return szReg;
				}
				break;
			}
		}
	}
	else
	{
		return szReg;
	}
}

string CTarget::GetAsmCode(vector<string> ParaStr)
{
	string szTmp;

	AsmCode Tmp;

	szTmp="";

	if (!CurrCode.Label.empty() && CurrCode.Label.at(0)=='%')
	{
		if (CurrCode.Label.at(3)=='-')
		{
			szTmp=szTmp+GetTmpLabel(CurrCode.Label);
		}
		else
		{
			szTmp=szTmp+ParaStr[CurrCode.Label.at(3)-'1'];
		}
	}
	else
	{
		szTmp=szTmp+CurrCode.Label;
	}

	Tmp.Lbl=trim(szTmp);

	szTmp="";

	if (!CurrCode.Op.empty() && CurrCode.Op.at(0)=='%')
	{
		szTmp=szTmp+ParaStr[CurrCode.Op.at(3)-'1'];
	}
	else
	{
		szTmp=szTmp+" "+CurrCode.Op;
	}

	Tmp.Op=trim(szTmp);

	szTmp="";

	for(int i=0;i<CurrCode.Operand1.size();i++)
	{
		if (!CurrCode.Operand1.at(i).empty() && CurrCode.Operand1.at(i).at(0)=='%')
		{
			int iBits = -1;

			if (CurrCode.Operand1.at(i).length()>=7)
			{
				iBits = CurrCode.Operand1.at(i).at(5)-'0';
			}

			if (CurrCode.Operand1.at(i).at(3)=='-')
			{
				szTmp=szTmp+GetTmpLabel(CurrCode.Operand1.at(i));
			}
			else
			{
				szTmp=szTmp+RegDivision(ParaStr[CurrCode.Operand1.at(i).at(3)-'1'],iBits);
			}
		}
		else
		{
			szTmp=szTmp+" "+CurrCode.Operand1.at(i);
		}
	}

	Tmp.Op1=trim(szTmp);

	szTmp="";

	for(int i=0;i<CurrCode.Operand2.size();i++)
	{
		if (!CurrCode.Operand2.at(i).empty() && CurrCode.Operand2.at(i).at(0)=='%')
		{
			int iBits = -1;

			if (CurrCode.Operand2.at(i).length()>=7)
			{
				iBits = CurrCode.Operand2.at(i).at(5)-'0';
			}

			if (CurrCode.Operand2.at(i).at(3)=='-')
			{
				szTmp=szTmp+GetTmpLabel(CurrCode.Operand2.at(i));
			}
			else
			{
				szTmp=szTmp+RegDivision(ParaStr[CurrCode.Operand2.at(i).at(3)-'1'],iBits);
			}
		}
		else
		{
			szTmp=szTmp+" "+CurrCode.Operand2.at(i);
		}
	}

	Tmp.Op2=trim(szTmp);

	szTmp="";

	for(int i=0;i<CurrCode.Operand3.size();i++)
	{
		if (!CurrCode.Operand3.at(i).empty() && CurrCode.Operand3.at(i).at(0)=='%')
		{
			int iBits = -1;

			if (CurrCode.Operand3.at(i).length()>=7)
			{
				iBits = CurrCode.Operand3.at(i).at(5)-'0';
			}

			if (CurrCode.Operand3.at(i).at(3)=='-')
			{
				szTmp=szTmp+GetTmpLabel(CurrCode.Operand3.at(i));
			}
			else
			{
				szTmp=szTmp+RegDivision(ParaStr[CurrCode.Operand3.at(i).at(3)-'1'],iBits);
			}
		}
		else
		{
			szTmp=szTmp+" "+CurrCode.Operand3.at(i);
		}
	}

	Tmp.Content=trim(szTmp);

	CurrAsmCodeList->push_back(Tmp);

	return AsmtoStr(Tmp);

}
string CTarget::GetConstStr(string str)
{
	string szTmp="\"";
	for (int i=0;i<str.length();i++)
	{
		if (str[i]==10)
		{
			if (i==str.length()-1)
			{
				szTmp=szTmp+"\",10";
				return szTmp;
			}
			szTmp=szTmp+"\",10,\"";
		}
		else
		{
			szTmp=szTmp+str[i];
		}
	}
	return szTmp+"\"";
}

void CTarget::MemoryAlloc()
{
	int iStackOffset=8;
	int iTmp=0;

	for(int i=0;i<SymbolTbl.TypeInfoTbl.size();i++)
	{
		SymbolTbl.CalcTypeSize(i);
	}

	RecOpAsm(".data");

	RecContentAsm(";ConstSymbolList:");

	for(int i=0;i<SymbolTbl.ConstInfoTbl.size();i++)
	{
		if (!SymbolTbl.ConstInfoTbl[i].m_bUsed)
			continue;
		switch (SymbolTbl.ConstInfoTbl[i].m_ConstType)
		{
		case ConstType::SET:
			RecOpAsm("__Const"+itos(i)+" db "+SetValue(SymbolTbl.ConstInfoTbl[i].m_szName));
			break;
		case ConstType::BOOLEAN:
			if (SymbolTbl.ConstInfoTbl[i].m_szName=="TRUE")
			{
				RecOpAsm("__Const"+itos(i)+"=1");
			}
			else
			{
				RecOpAsm("__Const"+itos(i)+"=0");
			}
			break;
		case ConstType::PTR:
			RecOpAsm("__Const"+itos(i)+"=0");
			break;
		case ConstType::STRING:
			{
				string szTmp=GetConstStr(SymbolTbl.ConstInfoTbl[i].m_szName
						.substr(1,SymbolTbl.ConstInfoTbl[i].m_szName.length()-2));

				if (szTmp=="\"\"")
				{
					RecOpAsm("__Text"+itos(i)+" db 0");
				}
				else
				{
					RecOpAsm("__Text"+itos(i)+" db "+szTmp+",0");
				}

				RecOpAsm("__Const"+itos(i)+" dword 0");
				break;
			}
		case ConstType::ENUM:
		case ConstType::INTEGER:
			RecOpAsm("__Const"+itos(i)+"="+itos(SymbolTbl.ConstInfoTbl[i].m_iVal));
			break;
		case ConstType::EREAL:
		case ConstType::REAL:
			RecOpAsm("__Const"+itos(i)+" dword "+rtos(SymbolTbl.ConstInfoTbl[i].m_fVal));
			break;
		default:
			RecOpAsm("__Const"+itos(i)+"="+SymbolTbl.ConstInfoTbl[i].m_szName);
		}
	}

	RecContentAsm(";VarSymbolList");

	RecOpAsm("__cvt DWORD 1 DUP(0)");

	for(int i=0;i<SymbolTbl.ProcInfoTbl.size();i++)
	{
		if (SymbolTbl.ProcInfoTbl[i].m_bUsed==false)
		{
			continue;
		}

		iTmp=iStackOffset;

		if (SymbolTbl.ProcInfoTbl.at(i).m_szName=="_noname")
		{
			continue;
		}
        //ʵ��
		for(int j=0;j<SymbolTbl.VarInfoTbl.size();j++)
		{
			if (SymbolTbl.VarInfoTbl[j].m_iProcIndex==i && SymbolTbl.VarInfoTbl[j].m_eRank==VarInfo::PARA)//�β�
			{
				SymbolTbl.VarInfoTbl[j].m_iMemoryAlloc=iTmp;
				int iSize=SymbolTbl.TypeInfoTbl[SymbolTbl.VarInfoTbl[j].m_iTypeLink].m_iSize;

				if (iSize % 4!=0)
				{
					iSize=(4-iSize % 4)+iSize;
				}
				if (i!=0)
				{
					RecOpAsm("_"+SymbolTbl.ProcInfoTbl.at(i).m_szName
						+"$"+SymbolTbl.VarInfoTbl[j].m_szName+"="+itos(iTmp));
				}
				else
				{
					RecOpAsm("_"+SymbolTbl.ProcInfoTbl.at(i).m_szName
						+"$"+SymbolTbl.VarInfoTbl[j].m_szName+" DWORD "+itos(iSize/4)+" DUP(0)");
				}

				iTmp=iTmp+iSize;

			}
		}
	}
    //�ֲ�����
	for(map<int,VarInfo>::iterator it=SymbolTbl.VarInfoTbl.begin();it!=SymbolTbl.VarInfoTbl.end();it++)
	{
		if (SymbolTbl.ProcInfoTbl[it->second.m_iProcIndex].m_bUsed==false)
		{
			continue;
		}

		if (it->second.m_iProcIndex!=0 && it->second.m_eRank!=VarInfo::PARA)
		{
			if (it->second.m_iMemoryAlloc!=-1)//�����Ż�
			{
				iTmp=-SymbolTbl.ProcInfoTbl.at(it->second.m_iProcIndex).m_ValSize;
				RecOpAsm("_"+SymbolTbl.ProcInfoTbl.at(it->second.m_iProcIndex).m_szName
						+"$"+it->second.m_szName+"="+itos(iTmp+it->second.m_iMemoryAlloc));
			}
			else
			{//���Ż�
				if (SymbolTbl.ProcInfoTbl.at(it->second.m_iProcIndex).m_TmpLink != -1)
				{
					RecOpAsm("_"+SymbolTbl.ProcInfoTbl.at(it->second.m_iProcIndex).m_szName
						+"$"+it->second.m_szName+"="+
						"_"+SymbolTbl.ProcInfoTbl.at(it->second.m_iProcIndex).m_szName
						+"$"+SymbolTbl.VarInfoTbl[SymbolTbl.ProcInfoTbl
						.at(it->second.m_iProcIndex).m_TmpLink].m_szName);
				}
			}
		}
	}

	for(map<int,VarInfo>::iterator it=SymbolTbl.VarInfoTbl.begin();it!=SymbolTbl.VarInfoTbl.end();it++)
	{
		if (SymbolTbl.ProcInfoTbl[it->second.m_iProcIndex].m_bUsed==false || it->second.m_iProcIndex!=0)
		{
			continue;//�����Ϊ������
		}

		if (it->second.m_iMemoryAlloc!=-1//�����Ż�
			|| (SymbolTbl.ProcInfoTbl.at(it->second.m_iProcIndex).m_TmpLink !=-1 &&
			&(it->second) == &SymbolTbl.VarInfoTbl[SymbolTbl.ProcInfoTbl.at(it->second.m_iProcIndex).m_TmpLink]))
		{
			iTmp=SymbolTbl.TypeInfoTbl[it->second.m_iTypeLink].m_iSize;
			string szTmp="";
			if (iTmp==1)
			{
				szTmp="BYTE";
			}
			if (iTmp==2)
			{
				szTmp="WORD";
			}
			if (iTmp>=3)
			{
				szTmp="DWORD";
			}
			if (iTmp % 4!=0)
			{
				iTmp=(4-iTmp % 4)+iTmp;
			}
			RecOpAsm("_"+SymbolTbl.ProcInfoTbl.at(it->second.m_iProcIndex).m_szName
					+"$"+it->second.m_szName+" "+szTmp+" "+itos(iTmp/4)+" DUP(0)");
		}
		else
		{
			if (SymbolTbl.ProcInfoTbl.at(it->second.m_iProcIndex).m_TmpLink != -1)
			{
				RecOpAsm("_"+SymbolTbl.ProcInfoTbl.at(it->second.m_iProcIndex).m_szName
					+"$"+it->second.m_szName+"="+
					"_"+SymbolTbl.ProcInfoTbl.at(it->second.m_iProcIndex).m_szName
					+"$"+SymbolTbl.VarInfoTbl[SymbolTbl.ProcInfoTbl
					.at(it->second.m_iProcIndex).m_TmpLink].m_szName);
			}
		}
	}
}
int CTarget::GetProcParaSize(int iProcIndex)
{
	int iSum=0;
	for (int i=0;i<SymbolTbl.VarInfoTbl.size();i++)
	{
		if (SymbolTbl.VarInfoTbl[i].m_iProcIndex==iProcIndex && SymbolTbl.VarInfoTbl[i].m_eRank==VarInfo::PARA)
		{
			int iSize=SymbolTbl.TypeInfoTbl[SymbolTbl.VarInfoTbl[i].m_iTypeLink].m_iSize;

			if (iSize % 4!=0)
			{
				iSize=(4-iSize % 4)+iSize;
			}
			iSum=iSum+iSize;
		}
	}
	return iSum;
}

int CTarget::GetProcVarSize(int iProcIndex)
{
	if (iProcIndex==0)
	{
		return 0;
	}
	else
	{
		return SymbolTbl.ProcInfoTbl[iProcIndex].m_ValSize;
	}
}

string CTarget::GetOpDataStr(OpInfo OpData)
{
	switch (OpData.m_iType)
	{
	case OpInfo::LABEL:
		{
			return SymbolTbl.LabelInfoTbl[OpData.m_iLink].m_szName;
		}
		break;
	case OpInfo::VAR:
	case OpInfo::PTR:
		{
			string szTmp;

			szTmp=SymbolTbl.VarInfoTbl[OpData.m_iLink].m_szName;

			if (SymbolTbl.VarInfoTbl[OpData.m_iLink].m_iProcIndex==0)
			{
				return "[_"+SymbolTbl.ProcInfoTbl.at(
							SymbolTbl.VarInfoTbl[OpData.m_iLink].m_iProcIndex).m_szName+"$"+szTmp+"]";
			}
			else
			{
				return "[ebp+_"+SymbolTbl.ProcInfoTbl.at(
							SymbolTbl.VarInfoTbl[OpData.m_iLink].m_iProcIndex).m_szName+"$"+szTmp+"]";
			}
		}
		break;
	case OpInfo::CONST:
		{
			return "[__Const"+itos(OpData.m_iLink)+"]";
		}
	case OpInfo::PROC:
		{
			return "_"+SymbolTbl.ProcInfoTbl.at(OpData.m_iLink).m_szName;
		}
	default:
		return "";
	}
}
string CTarget::GetOpDataPattern(OpInfo OpData)
{
	switch (OpData.m_iType)
	{
	case OpInfo::LABEL:		return "L";
	case OpInfo::CONST:		return "V";
	case OpInfo::VAR:		return "V";
	case OpInfo::PTR:		return "P";
	case OpInfo::PROC:		return "F";
	default:				return "";
	}
}

bool CTarget::GetIRPattern(IRCode &Code)
{
	Code.m_OpStr=GetOpStr(Code.m_eOpType);
	Code.m_Op1Str=GetOpDataStr(Code.m_Op1);
	Code.m_Op2Str=GetOpDataStr(Code.m_Op2);
	Code.m_RsltStr=GetOpDataStr(Code.m_Rslt);
	return true;

}
string CTarget::GetRegister(int i)
{
	if (i==1)
		return "edi";
	if (i==2)
		return "esi";
	if (i==3)
		return "ebx";
	if (i==4)
		return "eax";
}

bool CTarget::GenParaAsm(IRCode Tmp)
{
	int iSize=4;
	string szReg="";

	RegRefSave();

	if (SymbolTbl.ProcInfoTbl.at(SymbolTbl.VarInfoTbl[Tmp.m_Rslt.m_iLink].m_iProcIndex).m_eCallType==ProcInfo::Fastcall)
	{
		szReg=SymbolTbl.VarInfoTbl[Tmp.m_Rslt.m_iLink].m_szReg;
	}

	vector<string> ParaStr;

	if (Tmp.m_Op1.m_iType==OpInfo::CONST)
	{
		iSize=CType::GetTypeSize(SymbolTbl.ConstInfoTbl[Tmp.m_Op1.m_iLink].m_StoreType);
	}

	if (Tmp.m_Op1.m_iType==OpInfo::VAR)
	{
		iSize=SymbolTbl.TypeInfoTbl[SymbolTbl.VarInfoTbl[Tmp.m_Op1.m_iLink].m_iTypeLink].m_iSize;
	}

	if (iSize % 4!=0)
	{
		iSize=(4-iSize % 4)+iSize;
	}

	for (int i=0;i<iSize;i=i+4)
	{
		ParaStr.clear();

		if ((iSize-i-4)==0)
		{
			ParaStr.push_back(GetOpDataStr(Tmp.m_Op1));
		}
		else
		{
			ParaStr.push_back(GetOpDataStr(Tmp.m_Op1)+"+"+itos(iSize-i-4));  //����4byte�Ĳ�������
		}

		if (SymbolTbl.VarInfoTbl[Tmp.m_Rslt.m_iLink].m_bRef)
		{
			if (Tmp.m_Op1.m_bRef==false)
			{
				Op1LoadStr.push_back(GetOpDataStr(Tmp.m_Op1));
				Op1LoadStr.push_back("");
				Op1LoadStr.push_back("edi");
				IRtoAsmList("GETADDR %V.1% %V.3%",Op1LoadStr);
				Op1LoadStr.clear();
				ParaStr.clear();
				ParaStr.push_back("edi");

				if (!szReg.empty())
				{
					ParaStr.push_back("");
					ParaStr.push_back(szReg);
					IRtoAsmList("#MOV_4 %V.1% %V.3%",ParaStr);
				}
				else
				{
					IRtoAsmList("PUSH %V.1%",ParaStr);
				}

				return true;
			}
		}
		else
		{
			if (Tmp.m_Op1.m_bRef==true)
			{
				Op1LoadStr.push_back(GetOpDataStr(Tmp.m_Op1));
				Op1LoadStr.push_back("");
				Op1LoadStr.push_back("edi");
				IRtoAsmList("#MOV_4 %V.1% %V.3%",Op1LoadStr);
				Op1LoadStr.clear();
				ParaStr.clear();
				ParaStr.push_back("[edi]");
				ParaStr.push_back("");
				ParaStr.push_back("edi");
				IRtoAsmList("#MOV_4 %V.1% %V.3%",ParaStr);
				ParaStr.clear();
				ParaStr.push_back("edi");
			}
			if (Tmp.m_Op1.m_iType==OpInfo::CONST
				&& SymbolTbl.ConstInfoTbl[Tmp.m_Op1.m_iLink].m_ConstType==ConstType::STRING
				&& SymbolTbl.TypeInfoTbl[CType::GetRealType(SymbolTbl
							.VarInfoTbl[Tmp.m_Rslt.m_iLink].m_iTypeLink)].m_eDataType==StoreType::T_CHAR)
			{
				Op1LoadStr.push_back("dword ptr[__Text"+itos(Tmp.m_Op1.m_iLink)+" ]");
				Op1LoadStr.push_back("");
				Op1LoadStr.push_back("edi");
				IRtoAsmList("MOV_RR %V.1% %V.3%",Op1LoadStr);
				Op1LoadStr.clear();
				ParaStr.clear();
				ParaStr.push_back("edi");
			}
		}
		if (!szReg.empty())
		{
			ParaStr.push_back("");
			ParaStr.push_back(szReg);
			IRtoAsmList("#MOV_4 %V.1% %V.3%",ParaStr);
		}
		else
		{
			IRtoAsmList("PUSH %V.1%",ParaStr);
		}
	}
	return true;
}

bool CTarget::IRtoAsmList(string szPatternStr,vector<string> ParaStr)
{
	Pattern Tmp;
	string szTmp;

	if (CodePattern.SearchPattern(szPatternStr,Tmp))
	{
		for(int k=0;k<Tmp.CodeList.size();k++)
		{
			if (Tmp.CodeList.at(k).Op.compare("/loadS")==0)
			{
				if (!Op1LoadStr.empty())
				{
					IRtoAsmList("LD %V.1% %V.3%",Op1LoadStr);
				}
				if (!Op2LoadStr.empty())
				{
					IRtoAsmList("LD %V.1% %V.3%",Op2LoadStr);
				}
				continue;
			}
			if (Tmp.CodeList.at(k).Op.compare("/saveS")==0)
			{
				if (!RsltLoadStr.empty())
				{
					IRtoAsmList("SD %V.1% %V.3%",RsltLoadStr);
				}
				continue;
			}
			if (Tmp.CodeList.at(k).Op.compare("/strInit")==0
				|| Tmp.CodeList.at(k).Op.compare("/strDestory")==0)
			{
				set<int> TmpSet;

				for (int i=0;i<SymbolTbl.VarInfoTbl.size();i++)
				{

					if (SymbolTbl.VarInfoTbl[i].m_iProcIndex!=CurrProcId
						|| SymbolTbl.TypeInfoTbl[CType::GetRealType(
							SymbolTbl.VarInfoTbl[i].m_iTypeLink)].m_eDataType!=StoreType::T_STRING
						|| SymbolTbl.VarInfoTbl[i].m_iMemoryAlloc==-1
						|| TmpSet.count(SymbolTbl.VarInfoTbl[i].m_iMemoryAlloc)>0)
					{
						continue;
					}

					vector<string> VarName;

					TmpSet.insert(SymbolTbl.VarInfoTbl[i].m_iMemoryAlloc);

					if (CurrProcId==0)
					{
						VarName.push_back("_"+SymbolTbl.ProcInfoTbl.at(CurrProcId).m_szName
							+"$"+SymbolTbl.VarInfoTbl[i].m_szName);
					}
					else
					{
						VarName.push_back("ebp+_"+SymbolTbl.ProcInfoTbl.at(CurrProcId).m_szName
							+"$"+SymbolTbl.VarInfoTbl[i].m_szName);
					}

					if (Tmp.CodeList.at(k).Op.compare("/strInit")==0)
					{
						IRtoAsmList("STR_INIT %V.1%",VarName);
					}
					else
					{
						IRtoAsmList("STR_DESTORY %V.1%",VarName);
					}
				}
				if (CurrProcId==0)
				{
					for (int i=0;i<SymbolTbl.ConstInfoTbl.size();i++)
					{
						if (!SymbolTbl.ConstInfoTbl[i].m_bUsed
							|| SymbolTbl.ConstInfoTbl[i].m_ConstType!=ConstType::STRING)
						{
							continue;
						}

						vector<string> VarName;

						string szTmp=GetConstStr(SymbolTbl.ConstInfoTbl[i].m_szName
								.substr(1,SymbolTbl.ConstInfoTbl[i].m_szName.length()-2));

						VarName.push_back("__Text"+itos(i));
						VarName.push_back(itos(szTmp.length()-1));
						VarName.push_back("__Const"+itos(i));

						if (Tmp.CodeList.at(k).Op.compare("/strInit")==0)
						{
							IRtoAsmList("CONSTSTR_INIT %V.1% %V.2% %V.3%",VarName);
						}
						else
						{
							IRtoAsmList("CONSTSTR_DESTORY %V.3%",VarName);
						}
					}
				}

				continue;
			}
			CurrCode=Tmp.CodeList[k];

			GetAsmCode(ParaStr);
		}
		return true;
	}
	else
	{
		cout<<szPatternStr<<"δƥ�䵽"<<'\n';
		return false;
	}
}

bool CTarget::GenRetAsm(IRCode Tmp)
{
	int iSize=SymbolTbl.TypeInfoTbl[SymbolTbl.VarInfoTbl[Tmp.m_Rslt.m_iLink].m_iTypeLink].m_iSize;
	vector<string> ParaStr;
	ParaStr.clear();
	ParaStr.push_back(GetOpDataStr(Tmp.m_Rslt));

	if (iSize<=4)
	{
		IRtoAsmList("RETV4 %V.1%",ParaStr);
	}
	else if (iSize<=8)
	{
		IRtoAsmList("RETV8 %V.1%",ParaStr);
	}
	else
	{
		if (iSize % 4!=0)
		{
			iSize=(4-iSize % 4)+iSize;
		}
		iSize=iSize / 4;
		ParaStr.push_back(itos(iSize));
		IRtoAsmList("RETVN %V.1% %V.2%",ParaStr);
	}

	return true;
}

bool CTarget::GetLibAsm()
{
	set<string> FileList;
	FileList.clear();

	for (int i=0;i<SymbolTbl.ProcInfoTbl.size();i++)
	{
		if (SymbolTbl.ProcInfoTbl[i].m_bUsed==false)
		{
			continue;
		}

		if (SymbolTbl.ProcInfoTbl.at(i).m_eFlag==ProcInfo::Extern)
		{
			string szTmp=SymbolTbl.ConstInfoTbl[SymbolTbl.ProcInfoTbl.at(i).m_iExternStr].m_szName;
			szTmp=szTmp.substr(1,szTmp.length()-2);
			int pos=szTmp.find(';');
			szTmp=szTmp.substr(pos+1);
			pos=szTmp.find(';');
			if (pos==szTmp.npos)
			{
				return false;
			}
			string szFuncName=szTmp.substr(0,pos);
			string szFileName=szTmp.substr(pos+1);
			FileList.insert(szFileName);
			RecOpAsm("_"+SymbolTbl.ProcInfoTbl.at(i).m_szName+" EQU "+szFuncName);
		}
	}
	for(set<string>::iterator it=FileList.begin();it!=FileList.end();it++)
	{
		string szTmp=*it;
		int pos=szTmp.rfind('.');
		szTmp=szTmp.substr(pos+1);
		szTmp=UpperCase(szTmp);
		if (szTmp=="DLL" || szTmp=="LIB")
		{
			RecOpAsm("INCLUDELIB "+*it);
		}
		else
		{
			RecOpAsm("INCLUDE "+*it);
		}
	}
	return true;
}

vector<AsmCode>* CTarget::GetAsmCodeList(int iProcIndex)
{
	map<int,vector<AsmCode>>::iterator it=AsmCodeList.find(CurrProcId);

	if (it==AsmCodeList.end())
	{
		vector<AsmCode> tmp;
		AsmCodeList.insert(pair<int,vector<AsmCode>>(CurrProcId,tmp));
	}

	it=AsmCodeList.find(CurrProcId);

	return &(it->second);
}

bool  CTarget::GenAssignNAsm(IRCode Tmp)
{
	int iSize=SymbolTbl.TypeInfoTbl[Tmp.m_Op1.m_iDetailType.top().m_iLink].m_iSize;
	iSize=(iSize+iSize % 4) /4;
	vector<string>  ParaStr;
	string szRegOp1,szRegRslt;

	if (Tmp.m_Op1.m_bRef)
	{
		szRegOp1=GetRegister(3);
		ParaStr.push_back(GetOpDataStr(Tmp.m_Op1));
		ParaStr.push_back("");
		ParaStr.push_back(szRegOp1);
		IRtoAsmList("LD %V.1% %V.3%",ParaStr);
		ParaStr.clear();
	}
	if (Tmp.m_Rslt.m_bRef)
	{
		szRegRslt=GetRegister(4);
		ParaStr.push_back(GetOpDataStr(Tmp.m_Rslt));
		ParaStr.push_back("");
		ParaStr.push_back(szRegRslt);
		IRtoAsmList("LD %V.1% %V.3%",ParaStr);
		ParaStr.clear();
	}

	ParaStr.clear();
	ParaStr.push_back(szRegOp1.empty()?GetOpDataStr(Tmp.m_Op1):szRegOp1);
	ParaStr.push_back(itos(iSize));
	ParaStr.push_back(szRegRslt.empty()?GetOpDataStr(Tmp.m_Rslt):szRegRslt);
	IRtoAsmList("ASSIGN_N %V.1% %V.2% %V.3%",ParaStr);

	return true;
}
void CTarget::RecOpAsm(string szOp,OpInfo Op)
{
	AsmCode tmp;
	string szTmp;

	RegClearAll();

	for (int i=0;i<szOp.length();i++)
	{
		if (szOp[i]=='%')
		{
			i++;
			string szNum;

			while (szOp[i]<='9' && szOp[i]>='0')
			{
				szNum=szNum+szOp[i++];
			}

			OpInfo TmpOp;
			TmpOp.m_iType=OpInfo::VAR;
			TmpOp.m_iLink=SymbolTbl.AsmParaTbl.at(Op.m_iLink+atoi(szNum.c_str())).m_iLink;
			szTmp=szTmp+GetOpDataStr(TmpOp)+szOp[i];
		}
		else
		{
			szTmp=szTmp+szOp[i];
		}
	}

	tmp.Op=szTmp;

	CurrAsmCodeList->push_back(tmp);
}
void CTarget::RecOpAsm(string szOp)
{
	AsmCode tmp;
	RegClearAll();
	tmp.Op=szOp;
	CurrAsmCodeList->push_back(tmp);
}

void CTarget::RecContentAsm(string szContent)
{
	AsmCode tmp;
	tmp.Content=szContent;
	CurrAsmCodeList->push_back(tmp);
}

void CTarget::WriteFile(string szFileName)
{
	extern void WriteProcAsm(vector<AsmCode> procAsm,ofstream &fout);
	ofstream fout(szFileName.c_str());
	map<int,vector<AsmCode>>::iterator it=AsmCodeList.find(-1);
	WriteProcAsm(it->second,fout);
	it=AsmCodeList.begin();
	for(;it!=AsmCodeList.end();it++)
	{
		if (it->first==-1 || it->first==-2)
		{
			continue;
		}
		WriteProcAsm(it->second,fout);
	}
	it=AsmCodeList.find(-2);
	WriteProcAsm(it->second,fout);
	fout.close();
}
#ifndef CSYMBOLTBL_H
#define CSYMBOLTBL_H
#include "Afx.h"
#include "Structure.h"
#include "CommonLib.h"
using namespace std;
class CSymbolTbl
{

public:
	vector<ProcInfo> ProcInfoTbl;			//������Ϣ��
	stack<int> ProcStack;					//���̷���ջ
	map<int,LabelInfo> LabelInfoTbl;		//�����Ϣ��
	map<int,ConstInfo> ConstInfoTbl;		//������Ϣ��
	map<int,TypeInfo> TypeInfoTbl;			//������Ϣ��
	vector<EnumInfo> EnumInfoTbl;			//ö��������Ϣ��
	map<int,VarInfo> VarInfoTbl;			//������Ϣ��

	vector<ProcDefToken> ProcDefTokenTbl;	//���̺��������ַ����б�
	vector<TypeSysInfo> TypeSysTbl;			//����ϵͳ��
	vector<UseFile> UseFileTbl;			    //�����ļ���Ϣ��
//	vector<AsmPara> AsmParaTbl;             //��������Ϣ��

public:
	CSymbolTbl(void);
	int SearchLabelInfoTbl(int iProcIndex,string sName);
	int SearchConstInfoTbl(int iProcIndex,string sName);
	int SearchTypeInfoTbl(int iProcIndex,string sName,bool bState=true);
	int SearchEnumInfoTbl(int iProcIndex,string sName);
	int SearchProcInfoTbl(string szName,bool bIsForward=true);
    int SearchVarInfoTbl(int iProcIndex,string szName);
   /**************�����׶�***********************************/
    int SearchUseFileTbl(string szName);//�ļ���Ϣ��
    /***************���******************************************/
    /***************���ʽ******************************************/
    int SearchTypeSysTbl(int iOp,int iOp1Type,int iOp2Type=0);//����ϵͳ��
    /***************������******************************************

	bool ProcDefTokenTblCompare(vector<CToken> List1,vector<CToken> List2);//��������ԭ�͵������Ƿ�һ��
	int RecConstTbl(const string szValue,int iType);//ע�᳣������(������ʽ)
	int RecConstTbl(const ConstInfo Value);//ע�᳣������
	int RecConstSetTbl(const string szValue);//ע�Ἧ�ϳ�������
	int CalcTypeSize(int iPos);//��������ռ�ô洢�ռ��С
	bool PtrCheck(int &iPos);//���ָ�������Ƿ�Ϸ�
	*/
	int GetTmpVar(int iProcIndex,StoreType::CStoreType eStoreType);//����һ������ΪeStoreType����ʱ����
	int GetTmpVar(int iProcIndex);//����һ����ʱ����
	int GetTmpVar(int iProcIndex,OpType eOpType);//����һ���̶����������͵���ʱ����
	int CopyTmpVar(int iValIdx);//��ֵһ������Ϊ��ʱ����
    int GetTmpLabel(int iProcIndex);//����һ����ʱ���
    int GetRealType(int iTypeLink);//��ȡ����������ʵ�����ͣ�����ȥ�������е�T_USER  �Ƿ��ظ�
	/*


	bool IsVarPara(string szName,int iProcIndex);//�жϱ����Ƿ�Ϊ�βα���
	bool IsTmpVar(int iPose);//�жϱ����Ƿ�Ϊ��ʱ����
	void PrintIR();//��ӡIR�б�
	bool IsProcVar(string szName);//�жϱ��������Ƿ�Ϊ����
	void PrintBasicBlock();//��ӡ��������Ϣ*/
	void Clear();//��շ��ű�

	void AddLabel(LabelInfo Tmp);//��ӱ��
	void AddConst(ConstInfo Tmp);//��ӳ���
	void AddType(TypeInfo Tmp);//�������
	void AddVar(VarInfo Tmp);//��ӱ���
	~CSymbolTbl(void);
};


#endif // CSYMBOLTBL_H

#ifndef CSYMBOLTBL_H
#define CSYMBOLTBL_H
#include "Afx.h"
#include "Structure.h"
#include "CommonLib.h"
using namespace std;
class CSymbolTbl
{

public:
	vector<ProcInfo> ProcInfoTbl;			//过程信息表
	stack<int> ProcStack;					//过程分析栈
	map<int,LabelInfo> LabelInfoTbl;		//标号信息表
	map<int,ConstInfo> ConstInfoTbl;		//常量信息表
	map<int,TypeInfo> TypeInfoTbl;			//类型信息表
	vector<EnumInfo> EnumInfoTbl;			//枚举类型信息表
	map<int,VarInfo> VarInfoTbl;			//变量信息表

	vector<ProcDefToken> ProcDefTokenTbl;	//过程函数声明字符串列表
	vector<TypeSysInfo> TypeSysTbl;			//类型系统表
	vector<UseFile> UseFileTbl;			    //包含文件信息表
//	vector<AsmPara> AsmParaTbl;             //汇编参数信息表

public:
	CSymbolTbl(void);
	int SearchLabelInfoTbl(int iProcIndex,string sName);
	int SearchConstInfoTbl(int iProcIndex,string sName);
	int SearchTypeInfoTbl(int iProcIndex,string sName,bool bState=true);
	int SearchEnumInfoTbl(int iProcIndex,string sName);
	int SearchProcInfoTbl(string szName,bool bIsForward=true);
    int SearchVarInfoTbl(int iProcIndex,string szName);
   /**************声明阶段***********************************/
    int SearchUseFileTbl(string szName);//文件信息表
    /***************语句******************************************/
    /***************表达式******************************************/
    int SearchTypeSysTbl(int iOp,int iOp1Type,int iOp2Type=0);//类型系统表
    /***************操作数******************************************

	bool ProcDefTokenTblCompare(vector<CToken> List1,vector<CToken> List2);//两个过程原型单词流是否一致
	int RecConstTbl(const string szValue,int iType);//注册常量函数(重载形式)
	int RecConstTbl(const ConstInfo Value);//注册常量函数
	int RecConstSetTbl(const string szValue);//注册集合常量函数
	int CalcTypeSize(int iPos);//计算类型占用存储空间大小
	bool PtrCheck(int &iPos);//检查指针声明是否合法
	*/
	int GetTmpVar(int iProcIndex,StoreType::CStoreType eStoreType);//申请一个类型为eStoreType的临时变量
	int GetTmpVar(int iProcIndex);//申请一个临时变量
	int GetTmpVar(int iProcIndex,OpType eOpType);//申请一个固定操作符类型的临时变量
	int CopyTmpVar(int iValIdx);//赋值一个变量为临时变量
    int GetTmpLabel(int iProcIndex);//申请一个临时标号
    int GetRealType(int iTypeLink);//获取类型描述的实际类型，即略去类型链中的T_USER  是否重复
	/*


	bool IsVarPara(string szName,int iProcIndex);//判断变量是否为形参变量
	bool IsTmpVar(int iPose);//判断变量是否为临时变量
	void PrintIR();//打印IR列表
	bool IsProcVar(string szName);//判断变量类型是否为过程
	void PrintBasicBlock();//打印基本块信息*/
	void Clear();//清空符号表

	void AddLabel(LabelInfo Tmp);//添加标号
	void AddConst(ConstInfo Tmp);//添加常量
	void AddType(TypeInfo Tmp);//添加类型
	void AddVar(VarInfo Tmp);//添加变量
	~CSymbolTbl(void);
};


#endif // CSYMBOLTBL_H

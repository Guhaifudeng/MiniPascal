#ifndef STRUCTURE_H
#define STRUCTURE_H
#include "Afx.h"
using namespace std;
/********************词法分析***********************************/
struct LineInfo
{
    int m_iRow;
    string m_szFileName;
    LineInfo(int iRow,string szFileName);
    LineInfo();
};

struct CToken
{
    CToken();
    int m_iKind;
    string m_szContent;
    LineInfo m_LineInfo;
    CToken(int iKind,string szContent,int iRow,string szFileName);
    ~CToken(void);
};
struct ErrorInfo
{
    string m_szErrStr;
    string m_szFileName;
    int m_iRow;
};
/********************语法分析***********************************/

/********************符号表系统***********************************/
struct ObjectInfo//对象符号-名字+作用域
{
	string m_szName;
	int m_iProcIndex;
};

struct LabelInfo:ObjectInfo//标号信息表项
{
	bool m_bDef;
	bool m_bUse;
};
struct ConstType{
    enum CConstType{STRING,REAL,INTEGER,EREAL,BOOLEAN,ENUM,SET,PTR};//常量基本类型-语法
};
struct StoreType {
    enum CStoreType{			T_NONE=0,//实际数据类型
						T_CHAR=1,
						T_BOOLEAN=2,
						T_INTEGER=3,
						T_BYTE=4,
						T_SHORTINT=5,
						T_SMALLINT=6,
						T_WORD=7,
						T_LONGWORD=8,
						T_CARDINAL=9,
						T_REAL=10,
						T_SINGLE=11,
						T_ENUM=12,
						T_ARRAY=13,
						T_SET=14,
						T_RECORD=15,
						T_STRING=16,
						T_FILE=17,
						T_POINTER=18,
						T_LONG8=19,	//隐式类型，8字节的整型数据
						T_FUNC=20,
						T_PROC=21,
						T_USER=22
};
};
struct ConstInfo:ObjectInfo//常量信息表项
{
	string m_szVal;
	ConstType::CConstType m_ConstType;//当前数据类型
	StoreType::CStoreType m_StoreType;
	long long m_iVal;
	bool m_bVal;
	double m_fVal;
	int m_iEnumIdx;	//枚举类型指针
	string m_szSet;	//集合常量
	bool m_bUsed;
	ConstInfo();
};

struct VarInfo:ObjectInfo//变量信息表项
{
	int m_iTypeLink;
	enum Rank {PARA,VAR,RET} m_eRank;//普通变量、形参变量、返回值
	bool m_bRef;
	int m_iMemoryAlloc;
	string m_szReg;
	bool m_bUsed;
	VarInfo();

};

struct ArrayInfo//数组类型的维度域
{
	int m_iStart;
	int m_iEnd;
	ArrayInfo();
};
struct FieldInfo:ObjectInfo//记录类型字段域
{
	int m_iLink;				//类型指针
	int m_iSize;				//占用字节数
	string m_szVarFieldFlag;	//可变字段的标识量-标识字段的名字
	string m_szVarFieldConst;	//可变字段的分类常量-字段标号-如果相同，则表示一个字段组
	int m_iState;				//处理状态
	int m_iOffset;				//字段偏移量
	FieldInfo();
};
struct TypeInfo:ObjectInfo//类型信息表项
{
	StoreType::CStoreType m_eDataType;				//数据类型
	int m_iLink;						//复杂类型指针
	vector<FieldInfo> m_FieldInfo;		//记录类型字段域
	vector<ArrayInfo> m_ArrayInfo;		//数组类型的维度域
	StoreType::CStoreType m_eBaseType;				//基本类型
	string m_szContent;					//备注
	int m_iState;						//处理状态-该类型是否构造完毕
	int m_iSize;
	TypeInfo();
};
struct EnumInfo:ObjectInfo//枚举类型信息表项(用来存储枚举常量值)
{

};

struct ParaInfo
{
	string m_szName;
	int m_iParaType;
	int m_iParaVar;
	enum  AssignType {VAL,VAR} m_eAssignType;
};






/********************中间表示***********************************/
struct VarType
{
	StoreType::CStoreType m_StoreType;
	int m_iLink;
	VarType(StoreType StoreType,int iLink);
	VarType();
};
struct OpInfo//操作数
{
	enum  {CONST,VAR,PTR,LABEL,NONE,PROC} m_iType;				//操作数类型
	int m_iLink;												//操作数指针
	stack<VarType> m_iDetailType;								//操作数详细类型指针，仅在结构、数组等复杂类型时有效
	bool m_bRef;												//操作数是否为间接寻址
	vector<int> m_udChain;
	vector<int> m_duChain;
	bool m_bUninit;
	OpInfo();
};

enum OpType//操作符-操作类型
{

	EQU		=	(1<<4),
	EQU_1	=	(1<<4)+1,
	EQU_S	=	(1<<4)+2,
	EQU_A	=	(1<<4)+3,
	EQU_4	=	(1<<4)+4,
	EQU_8	=	(1<<4)+5,
	EQU_8F	=	(1<<4)+6,
	EQU_4F	=	(1<<4)+7,
	EQU_2	=	(1<<4)+8,
	EQU_32	=	(1<<4)+9,

	NEQU    =	(2<<4),
	NEQU_1	=	(2<<4)+1,
	NEQU_S	=	(2<<4)+2,
	NEQU_4	=	(2<<4)+3,
	NEQU_8	=	(2<<4)+4,
	NEQU_8F	=	(2<<4)+5,
	NEQU_4F	=	(2<<4)+6,
	NEQU_2	=	(2<<4)+7,
	NEQU_32	=	(2<<4)+8,
	NEQU_A	=	(2<<4)+9,

	LS		=	(3<<4),
	LS_1	=	(3<<4)+1,
	LS_S	=	(3<<4)+2,
	LS_4	=	(3<<4)+3,
	LS_8	=	(3<<4)+4,
	LS_8F	=	(3<<4)+5,
	LS_4F	=	(3<<4)+6,
	LS_2	=	(3<<4)+7,
	LS_1U	=	(3<<4)+8,
	LS_2U	=	(3<<4)+9,
	LS_4U	=	(3<<4)+10,

	MR		=	(4<<4),
	MR_1	=	(4<<4)+1,
	MR_S	=	(4<<4)+2,
	MR_4	=	(4<<4)+3,
	MR_8	=	(4<<4)+4,
	MR_8F	=	(4<<4)+5,
	MR_4F	=	(4<<4)+6,
	MR_2	=	(4<<4)+7,
	MR_1U	=	(4<<4)+8,
	MR_2U	=	(4<<4)+9,
	MR_4U	=	(4<<4)+10,

	LE		=	(5<<4),
	LE_1	=	(5<<4)+1,
	LE_S	=	(5<<4)+2,
	LE_4	=	(5<<4)+3,
	LE_8	=	(5<<4)+4,
	LE_8F	=	(5<<4)+5,
	LE_4F	=	(5<<4)+6,
	LE_2	=	(5<<4)+7,
	LE_1U	=	(5<<4)+8,
	LE_2U	=	(5<<4)+9,
	LE_4U	=	(5<<4)+10,

	ME		=	(6<<4),
	ME_1	=	(6<<4)+1,
	ME_S	=	(6<<4)+2,
	ME_4	=	(6<<4)+3,
	ME_8	=	(6<<4)+4,
	ME_8F	=	(6<<4)+5,
	ME_4F	=	(6<<4)+6,
	ME_2	=	(6<<4)+7,
	ME_1U	=	(6<<4)+8,
	ME_2U	=	(6<<4)+9,
	ME_4U	=	(6<<4)+10,

	IN		=	(7<<4),
	IN_C	=	(7<<4)+1,
	IN_I	=	(7<<4)+2,
	IN_B	=	(7<<4)+3,
	IN_SH	=	(7<<4)+4,
	IN_SM	=	(7<<4)+5,
	IN_W	=	(7<<4)+6,
	IN_LW	=	(7<<4)+7,
	IN_E	=	(7<<4)+8,

	SUBI	=	(8<<4),
	SUB_4	=	(8<<4)+1,
	SUB_8F	=	(8<<4)+2,
	SUB_8	=	(8<<4)+3,
	SUB_SET	=	(8<<4)+4,
	SUB_2	=	(8<<4)+5,

	ADDI	=	(9<<4),
	ADD_S	=	(9<<4)+1,
	ADD_4	=	(9<<4)+2,
	ADD_8F	=	(9<<4)+3,
	ADD_SET	=	(9<<4)+4,
	ADD_2	=	(9<<4)+5,

	OR		=	(10<4),
	OR_0	=	(10<<4)+1,
	OR_4	=	(10<<4)+2,
	OR_1	=	(10<<4)+3,
	OR_2	=	(10<<4)+4,

	XOR		=	(11<<4),
	XOR_0	=	(11<<4)+1,
	XOR_4	=	(11<<4)+2,
	XOR_1	=	(11<<4)+3,
	XOR_2	=	(11<<4)+4,

	MULI	=	(12<<4),
	MUL_4	=	(12<<4)+1,
	MUL_8F	=	(12<<4)+2,
	MUL_SET	=	(12<<4)+3,

	DIVI	=	(13<<4),
	DIV_8F	=	(13<<4)+1,
	DIV_4	=	(13<<4)+2,

	MOD		=	(14<<4),
	MOD_4	=	(14<<4)+1,

	SHR		=	(15<<4),
	SHR_4	=	(15<<4)+1,
	SHR_1U	=	(15<<4)+2,
	SHR_1	=	(15<<4)+3,
	SHR_2	=	(15<<4)+4,
	SHR_2U	=	(15<<4)+5,
	SHR_4U	=	(15<<4)+6,

	AND		=	(16<<4),
	AND_0	=	(16<<4)+1,
	AND_4	=	(16<<4)+2,
	AND_1	=	(16<<4)+3,
	AND_2	=	(16<<4)+4,

	SHL		=	(17<<4),
	SHL_4	=	(17<<4)+1,
	SHL_1U	=	(17<<4)+2,
	SHL_1	=	(17<<4)+3,
	SHL_2	=	(17<<4)+4,
	SHL_2U	=	(17<<4)+5,
	SHL_4U	=	(17<<4)+6,

	NEG		=	(18<<4),
	NEG_4	=	(18<<4)+1,
	NEG_1	=	(18<<4)+2,
	NEG_2	=	(18<<4)+3,
	NEG_8F	=	(18<<4)+4,
	NEG_4F	=	(18<<4)+5,

	NOT		=	(19<<4),
	NOT_0	=	(19<<4)+1,
	NOT_4	=	(19<<4)+2,
	NOT_1	=	(19<<4)+3,
	NOT_2	=	(19<<4)+4,

	ASSIGN		=	(20<<4),
	ASSIGN_1	=	(20<<4)+1,
	ASSIGN_S	=	(20<<4)+2,
	ASSIGN_4	=	(20<<4)+3,
	ASSIGN_2	=	(20<<4)+4,
	ASSIGN_8	=	(20<<4)+5,
	ASSIGN_N	=	(20<<4)+6,
	ASSIGN_32	=	(20<<4)+7,

	JMP			=	(21<<4)+1,
	LABEL		=	(21<<4)+2,
	PARA		=	(21<<4)+3,
	RETV		=	(21<<4)+4,
	CALL		=	(21<<4)+5,
	JT			=	(21<<4)+6,
	JNT			=	(21<<4)+7,
	JE_1		=	(21<<4)+8,
	JE_2		=	(21<<4)+9,
	JE_4		=	(21<<4)+10,
	CALLPTR		=	(21<<4)+11,
	ASM			=	(21<<4)+12,
	GETADDR		=	(21<<4)+13,

	SET_ADDITEM	=	(22<<4)+1,
	GETPROCADDR	=	(22<<4)+2,

	None		=	 0,
	ChrToStr	=	-1,

	ByteToInt	=	-2,
	ShortToInt	=	-3,
	SmallToInt	=	-4,
	WordToInt	=	-5,
	LongToInt	=	-27,

	IntToReal	=	-6,
	ByteToReal	=	-11,
	ShortToReal	=	-14,
	SmallToReal	=	-16,
	WordToReal	=	-19,
	LongToReal	=	-21,
	SingleToReal=	-23,

	IntToSingle	=	-7,
	ByteToSingle=	-12,
	ShortToSingle=	-15,
	SmallToSingle=	-17,
	WordToSingle=	-20,
	LongToSingle=	-22,
	RealToSingle=	-41,

	ByteToSmall	=	-8,
	ShortToSmall=	-13,
	IntToSmall	=	-34,
	LongToSmall	=	-39,

	ByteToWord	=	-9,
	ShortToWord	=	-25,
	IntToWord	=	-35,
	LongToWord	=	-40,

	ByteToLong	=	-10,
	WordToLong	=	-18,
	ShortToLong	=	-26,
	SmallToLong	=	-28,

	ByteToShort	=	-24,
	IntToShort	=	-29,
	SmallToShort=	-30,
	WordToShort	=	-31,
	LongToShort	=	-32,

	IntToByte	=	-33,
	SmallToByte	=	-36,
	WordToByte	=	-37,
	LongToByte	=	-38,

	ByteToLong8	=	-42,
	ShortToLong8=	-43,
	SmallToLong8=	-44,
	IntToLong8	=	-45,
	WordToLong8	=	-46,
	LongToLong8	=	-47,

	TypeCast	=	-50

};

struct IRCode
{
	OpType m_eOpType;
	OpInfo m_Op1,m_Op2,m_Rslt;
	bool m_bEnabled;
	string m_OpStr,m_Op1Str,m_Op2Str,m_RsltStr; //目标代码生成时使用

};



struct ProcInfo//过程信息表项
{
	string m_szName;
	enum  Rank {Main,Sub} m_eRank;//MLP不支持嵌套声明
	enum  Type {Procedure,Function,Type} m_eType;//表项类型，标识该表项是过程表项、函数表项还是函数指针类型表项（内嵌函数）
	vector<ParaInfo> m_ParaTbl;//形参列表信息
	int m_iReturnType;//返回类型指针描述
	int m_iReturnVar;//返回变量指针
    enum Flag {Forward,Extern,None} m_eFlag;//Extern 外部函数头部声明，Forward 文件稍后位置或其他源文件
	int m_iExternStr;//外部 修饰说明
	int m_iProcDefToken;//原型单词流
	enum  CallType {Stdcall,Fastcall} m_eCallType;
	vector<IRCode> m_Codes;//中间代码序列
	set<int> m_TmpMemShare;//可共享存储区域的临时变量集合
	//_________________________ 运行时刻管理
	int m_ValSize;//相关局部变量的空间大小
	int m_TmpLink;//临时变量的指针
	bool m_bUsed;//函数或过程是否使用
	bool m_bTmpUsed;//函数或过程中是否使用了临时变量
	ProcInfo();
};
/**********************类型系统*********************************************************/
struct TypeSysInfo
{
	int m_iOp;
	int m_iOp1Type;
	int m_iOp2Type;
	int m_iRsltType;
	int m_iProcessId;
	int m_iVarProcessId;
	int m_iIR;
	int m_Convert1;
	int m_Convert2;
};

/*************声明阶段*************************************************************************/
struct ProcDefToken
{
	string m_szName;
	int m_iLink;
	vector<CToken> m_TokenList;
};
struct UseFile
{
	string m_szFileName;
	bool m_bFlag;
	UseFile(string szFileName,bool Flag);
};
#endif // STRUCTURE_H

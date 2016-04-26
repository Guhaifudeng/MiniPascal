#ifndef CTYPE_H
#define CTYPE_H
#include "Structure.h"
#include "CommonLib.h"
#include "State.h"
#include "Afx.h"
using namespace std;
class CType
{
public:
    static VarType getVarTypeStackElement(stack<VarType> VarTypeStack, int position);
	static StoreType::CStoreType TokenToEnum(int iToken);//根据单词，计算得到相应的类型枚举值
	static int GetTypeSize(StoreType::CStoreType eStoreType);//根据类型的枚举值，计算得到相应的类型的size值
	static StoreType::CStoreType GetOpType(OpInfo Op);//获取操作数实际类型
	static int  TypeCompatible(OpInfo Op1,OpInfo Op2,int iOp);//类型相容
	static int  TypeCompatible( OpInfo Op1,int Op);//
	static void ProcessConstType(int iPos);//根据字面常量的值，评估常量的实际类型
	static int GetRealType(int iTypeLink);//根据类型表项指针获取实际类型
	static int GetOpTypeLink(OpInfo Op);//
	static bool IsInt(int iTypeLink);//是否为整型
	static bool IsInt(StoreType::CStoreType eStoreType);//是传入的实际类型是否为整型
	static bool IsOrd(OpInfo Op);//
	static bool IsSetItem(OpInfo Op);//
	static bool IsOffsetVar(stack<VarType> VarTypeStack);//
	static void InitTypeSysTbl();//初始化符号表

};

#endif // CTYPE_H

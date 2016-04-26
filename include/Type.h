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
	static StoreType::CStoreType TokenToEnum(int iToken);//���ݵ��ʣ�����õ���Ӧ������ö��ֵ
	static int GetTypeSize(StoreType::CStoreType eStoreType);//�������͵�ö��ֵ������õ���Ӧ�����͵�sizeֵ
	static StoreType::CStoreType GetOpType(OpInfo Op);//��ȡ������ʵ������
	static int  TypeCompatible(OpInfo Op1,OpInfo Op2,int iOp);//��������
	static int  TypeCompatible( OpInfo Op1,int Op);//
	static void ProcessConstType(int iPos);//�������泣����ֵ������������ʵ������
	static int GetRealType(int iTypeLink);//�������ͱ���ָ���ȡʵ������
	static int GetOpTypeLink(OpInfo Op);//
	static bool IsInt(int iTypeLink);//�Ƿ�Ϊ����
	static bool IsInt(StoreType::CStoreType eStoreType);//�Ǵ����ʵ�������Ƿ�Ϊ����
	static bool IsOrd(OpInfo Op);//
	static bool IsSetItem(OpInfo Op);//
	static bool IsOffsetVar(stack<VarType> VarTypeStack);//
	static void InitTypeSysTbl();//��ʼ�����ű�

};

#endif // CTYPE_H

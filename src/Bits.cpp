#include "Bits.h"
/*****************************************************************
	函数名:					CBits::CBits
	功能描述:				构造函数，分配用于存储流值的合适长度的整型数组
	输入参数及其说明:		nn 表示流值的长度
	输出参数及其说明:		无
	影响全局或局部静态变量:	m_pInts, m_nNum, m_nInts
	有密切关系的函数、类:	CFlowVal, CLivenessAnalysis
	其它必要的说明信息：	在构造函数中完成所有流值位置“0”
****************************************************************/
CBits::CBits (int nn)
{
	m_nNum = nn;
	m_nInts = nn / BITS_PER_INT;
	if (nn % BITS_PER_INT)
		m_nInts++;
	m_pInts = new unsigned int[m_nInts];
	//初始化所有位为0
	reset_all ();
}

/*****************************************************************
	函数名:					CBits::CBits
	功能描述:				构造函数，将other的内容存于当前CBits中
	输入参数及其说明:		other 拷贝的对象
	输出参数及其说明:		无
	影响全局或局部静态变量:	m_pInts, m_nNum, m_nInts
	有密切关系的函数、类:	CFlowVal, CLivenessAnalysis, CBits::copy
	其它必要的说明信息：	无
****************************************************************/
CBits::CBits (CBits & other)
{
	m_nNum = other.m_nNum;
	m_nInts = other.m_nInts;
	m_pInts = new unsigned int[m_nInts];
	copy(&other);
}

/*****************************************************************
	函数名:					CBits::~CBits
	功能描述:				析构函数，释放当前CBits占用的空间
	输入参数及其说明:		无
	输出参数及其说明:		无
	影响全局或局部静态变量:	m_pInts
	有密切关系的函数、类:	CFlowVal, CLivenessAnalysis
	其它必要的说明信息：	无
****************************************************************/
CBits::~CBits ()
{
	delete [] m_pInts;
}

/*****************************************************************
	函数名:					CBits::reset_all
	功能描述:				成员函数，设置流值的所有位为0
	输入参数及其说明:		无
	输出参数及其说明:		无
	影响全局或局部静态变量:	m_pInts
	有密切关系的函数、类:	CFlowVal, CLivenessAnalysis
	其它必要的说明信息：	无
****************************************************************/
void CBits::reset_all ()
{
	for (int i=0; i<m_nInts; i++)
		m_pInts[i] = 0;
}

/*****************************************************************
	函数名:					CBits::set_all
	功能描述:				成员函数，设置流值的所有位为1
	输入参数及其说明:		无
	输出参数及其说明:		无
	影响全局或局部静态变量:	m_pInts
	有密切关系的函数、类:	CFlowVal, CLivenessAnalysis, CBits::set
	其它必要的说明信息：	无
****************************************************************/
void CBits::set_all ()
{
	for (int i=0; i < m_nNum; i++)
		set(i);
}

/*****************************************************************
	函数名:					CBits::operator=
	功能描述:				成员函数，重载赋值语句,将rhs中的内容赋值到当前CBits中
	输入参数及其说明:		rhs, 赋值的对象
	输出参数及其说明:		赋值完成的对象
	影响全局或局部静态变量:	m_pInts, m_nNum, m_nInts
	有密切关系的函数、类:	CFlowVal, CLivenessAnalysis, CBits::copy
	其它必要的说明信息：	无
****************************************************************/
CBits & CBits::operator=(CBits & rhs)
{
	if ( this != &rhs )
		{
			//释放该CBits所占用的资源
			delete [] m_pInts;
			m_nNum = rhs.m_nNum;
			m_nInts = rhs.m_nInts;
			m_pInts = new unsigned int[m_nInts];
			copy(&rhs);
		}
	return *this;
}

/*****************************************************************
	函数名:					CBits::size
	功能描述:				成员函数，取数据流值中使用的向量位数
	输入参数及其说明:		无
	输出参数及其说明:		数据流值中使用的向量位数
	影响全局或局部静态变量:	无
	有密切关系的函数、类:	CFlowVal, CLivenessAnalysis
	其它必要的说明信息：	无
****************************************************************/
int CBits::size()
{
	return m_nNum;
}

/*****************************************************************
	函数名:					CBits::set
	功能描述:				成员函数，按位设置流值
	输入参数及其说明:		pos, 设置pos位的流值
							val, 为true则将pos位置为1,否则置为0
	输出参数及其说明:		无
	影响全局或局部静态变量:	m_pInts
	有密切关系的函数、类:	CFlowVal, CLivenessAnalysis
	其它必要的说明信息：	无
****************************************************************/
void CBits::set (int pos, bool val)
{
	int i = pos / BITS_PER_INT;
	int j = pos % BITS_PER_INT;
	if (val)
		{
			m_pInts[i] |= (1<<j);
		}
	else
		{
			m_pInts[i] &= ~(1<<j);
		}
}

/*****************************************************************
	函数名:					CBits::set
	功能描述:				成员函数，按位设置其流值为1
	输入参数及其说明:		pos, 设置pos位的流值
	输出参数及其说明:		无
	影响全局或局部静态变量:	m_pInts
	有密切关系的函数、类:	CFlowVal, CLivenessAnalysis
	其它必要的说明信息：	将pos位设为“1”
****************************************************************/
void CBits::set (int pos)
{
	set (pos, true);
}

/*****************************************************************
	函数名:					CBits::copy
	功能描述:				成员函数，复制流值
	输入参数及其说明:		b, 复制的对象
	输出参数及其说明:		无
	影响全局或局部静态变量:	m_pInts
	有密切关系的函数、类:	CFlowVal, CLivenessAnalysis
	其它必要的说明信息：	无
****************************************************************/
void CBits::copy (CBits * b)
{
	for (int i=0; i<m_nInts; i++)
		m_pInts[i] = b->m_pInts[i];
}

/*****************************************************************
	函数名:					CBits::reset
	功能描述:				成员函数，按位设置其流值为0
	输入参数及其说明:		pos, 设置pos位的流值
	输出参数及其说明:		无
	影响全局或局部静态变量:	m_pInts
	有密切关系的函数、类:	CFlowVal, CLivenessAnalysis, CBits::set
	其它必要的说明信息：	将pos位设为“0”
****************************************************************/
void CBits::reset (int pos)
{
	set (pos, false);
}

/*****************************************************************
	函数名:					CBits::value
	功能描述:				成员函数，判断流值位是0还是1
	输入参数及其说明:		pos, 判断pos的流值位
	输出参数及其说明:		pos位为1,返回true,否则返回false
	影响全局或局部静态变量:	无
	有密切关系的函数、类:	CFlowVal, CLivenessAnalysis
	其它必要的说明信息：	无
****************************************************************/
bool CBits::value (int pos)
{
	int i = pos / BITS_PER_INT;
	int j = pos % BITS_PER_INT;
	if (m_pInts[i] & (1<<j))
		return true;
	return false;
}

/*****************************************************************
	函数名:					CBits::And
	功能描述:				成员函数，求other和当前CBits进行与
	输入参数及其说明:		other, 与操作的对象
	输出参数及其说明:		无
	影响全局或局部静态变量:	m_pInts
	有密切关系的函数、类:	CFlowVal, CLivenessAnalysis
	其它必要的说明信息：	无
****************************************************************/
void CBits::And (CBits *other)
{
	for (int i=0; i<m_nInts; i++)
		m_pInts[i] &= other->m_pInts[i];
}

/*****************************************************************
	函数名:					CBits::Or
	功能描述:				成员函数，求other和当前CBits进行或
	输入参数及其说明:		other, 或操作的对象
	输出参数及其说明:		无
	影响全局或局部静态变量:	m_pInts
	有密切关系的函数、类:	CFlowVal, CLivenessAnalysis
	其它必要的说明信息：	无
****************************************************************/
void CBits::Or (CBits *other)
{
	for (int i=0; i<m_nInts; i++)
		m_pInts[i] |= other->m_pInts[i];
}

/*****************************************************************
	函数名:					CBits::Not
	功能描述:				成员函数，求当前CBits的非
	输入参数及其说明:		无
	输出参数及其说明:		无
	影响全局或局部静态变量:	m_pInts
	有密切关系的函数、类:	CFlowVal, CLivenessAnalysis
	其它必要的说明信息：	无
****************************************************************/
void CBits::Not ()
{
	for (int i=0; i<m_nInts; i++)
		m_pInts[i] = ~m_pInts[i];
}

/*****************************************************************
	函数名:					CBits::Difference
	功能描述:				成员函数，求当前CBits和other的差
	输入参数及其说明:		other, 差操作的对象
	输出参数及其说明:		无
	影响全局或局部静态变量:	m_pInts
	有密切关系的函数、类:	CFlowVal, CLivenessAnalysis
	其它必要的说明信息：	无
****************************************************************/
void CBits::Difference(CBits * other)
{
	CBits comp(m_nNum);
	comp.copy(other);
	comp.Not();
	And(&comp);
}

/*****************************************************************
	函数名:					operator==
	功能描述:				友员函数，判断两个CBits是否相等
	输入参数及其说明:		a,b 两个比较的流值
	输出参数及其说明:		相等返回true, 不等返回false
	影响全局或局部静态变量:	无
	有密切关系的函数、类:	CBits, CFlowVal, CLivenessAnalysis
	其它必要的说明信息：	无
****************************************************************/
bool operator==(CBits & a, CBits & b)
{
	if ( a.m_nInts != b.m_nInts )
		return false;
	for ( int i = 0; i < a.m_nInts; i++ )
		if ( a.m_pInts[i] != b.m_pInts[i] )
			return false;
	return true;
}

/*****************************************************************
	函数名:					operator!=
	功能描述:				友员函数，判断两个CBits是否不相等
	输入参数及其说明:		a,b 两个比较的流值
	输出参数及其说明:		不相等返回true, 等返回false
	影响全局或局部静态变量:	无
	有密切关系的函数、类:	CBits, CFlowVal, CLivenessAnalysis, operator==
	其它必要的说明信息：	无
****************************************************************/
bool operator!=(CBits & a, CBits & b)
{
	return ! (a == b);
}

/*****************************************************************
	函数名:					CBits::clone
	功能描述:				成员函数，新建一个CBits对象，为自身对象的拷贝
	输入参数及其说明:		无
	输出参数及其说明:		新建拷贝自身后的流值
	影响全局或局部静态变量:	m_pInts, m_nNum, m_nInts
	有密切关系的函数、类:	CFlowVal, CLivenessAnalysis, CBits::CBits
	其它必要的说明信息：	无
****************************************************************/
CBits * CBits::clone ()
{
	CBits *b = new CBits (m_nNum);
	for (int i=0; i<m_nInts; i++)
		b->m_pInts[i] = m_pInts[i];
	return b;
}

/*****************************************************************
	函数名:					operator!=
	功能描述:				友员函数，输出当前CBits的流值
	输入参数及其说明:		out,输出流
							a, 待输出的流值
	输出参数及其说明:		输出流
	影响全局或局部静态变量:	无
	有密切关系的函数、类:	CBits
	其它必要的说明信息：	无
****************************************************************/
//ostream & operator<<(ostream & out, CBits & a)
//{
//	out << "(" << a.size() << ")" << " ( ";
//	for ( int i = 0; i < a.size(); i++ )
//		out << a.value(i);
//	out << " )";
//	return out;
//}

void CBits::Sub(CBits *other)
{
	int iTmp;
	for (int i=0; i<m_nInts; i++)
	{
		iTmp=m_pInts[i] & other->m_pInts[i];
		iTmp=~iTmp;
		m_pInts[i] &= iTmp;
	}

}

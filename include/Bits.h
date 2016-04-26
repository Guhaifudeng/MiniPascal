#ifndef CBITS_H
#define CBITS_H


#include "Afx.h"

using namespace std;

#define BITS_PER_INT (8*sizeof(int)) //一个整型所占用的bit数

class CBits
{
private:
	//存储实际的流值,用整型来存储
	unsigned int * m_pInts;
	//n表示数据流值中使用的向量位数
	int	m_nNum;
	//nints表示用int类型来存储该流值需要用多少个整型变量来保存
	int m_nInts;
public:
	//constructor:分配用于存储流值的合适长度的整型数组
	//nn表示流值的长度
	CBits (int nn);


	//constructor:将other的内容存于当前Bits中
	CBits (CBits & other);

	//destructor:释放当前Bits占用的空间
	~CBits ();

	//设置流值的所有位为0
	void reset_all ();

	//设置流值所有位为1
	void set_all ();

	//重载赋值语句,将rhs中的内容赋值到当前Bits中
	CBits & operator=(CBits & rhs);


	//取bits位数
	int size(void);

	//按位设置流值,设置pos位的流值,val为true则将pos位置为1,否则置为0
	void set (int pos, bool val);

	//设置pos位为1
	void set (int pos);

	//复制流值
	void copy (CBits * b);

	//设置pos位为0
	void reset (int pos);

	//pos位为1,返回true,否则返回false
	bool value (int pos);

	//求other和当前Bits进行与
	void And (CBits *other);



	//求other和当前Bits进行或
	void Or (CBits *other);


	//求当前CBits的非
	void Not ();

	void Sub(CBits *other) ;


	//当前Bits-other
	void Difference(CBits * other);


	friend bool operator==(CBits & a, CBits & b) ;

	friend bool operator!=(CBits & a, CBits & b) ;

	//新建一个CBits对象，为自身对象的拷贝
	CBits *clone ();


	//输出当前Bits的流值
	//friend ostream & operator<<(ostream & out, CBits & a);


};
#endif // CBITS_H

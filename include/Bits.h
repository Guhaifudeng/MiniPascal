#ifndef CBITS_H
#define CBITS_H


#include "Afx.h"

using namespace std;

#define BITS_PER_INT (8*sizeof(int)) //һ��������ռ�õ�bit��

class CBits
{
private:
	//�洢ʵ�ʵ���ֵ,���������洢
	unsigned int * m_pInts;
	//n��ʾ������ֵ��ʹ�õ�����λ��
	int	m_nNum;
	//nints��ʾ��int�������洢����ֵ��Ҫ�ö��ٸ����ͱ���������
	int m_nInts;
public:
	//constructor:�������ڴ洢��ֵ�ĺ��ʳ��ȵ���������
	//nn��ʾ��ֵ�ĳ���
	CBits (int nn);


	//constructor:��other�����ݴ��ڵ�ǰBits��
	CBits (CBits & other);

	//destructor:�ͷŵ�ǰBitsռ�õĿռ�
	~CBits ();

	//������ֵ������λΪ0
	void reset_all ();

	//������ֵ����λΪ1
	void set_all ();

	//���ظ�ֵ���,��rhs�е����ݸ�ֵ����ǰBits��
	CBits & operator=(CBits & rhs);


	//ȡbitsλ��
	int size(void);

	//��λ������ֵ,����posλ����ֵ,valΪtrue��posλ��Ϊ1,������Ϊ0
	void set (int pos, bool val);

	//����posλΪ1
	void set (int pos);

	//������ֵ
	void copy (CBits * b);

	//����posλΪ0
	void reset (int pos);

	//posλΪ1,����true,���򷵻�false
	bool value (int pos);

	//��other�͵�ǰBits������
	void And (CBits *other);



	//��other�͵�ǰBits���л�
	void Or (CBits *other);


	//��ǰCBits�ķ�
	void Not ();

	void Sub(CBits *other) ;


	//��ǰBits-other
	void Difference(CBits * other);


	friend bool operator==(CBits & a, CBits & b) ;

	friend bool operator!=(CBits & a, CBits & b) ;

	//�½�һ��CBits����Ϊ�������Ŀ���
	CBits *clone ();


	//�����ǰBits����ֵ
	//friend ostream & operator<<(ostream & out, CBits & a);


};
#endif // CBITS_H

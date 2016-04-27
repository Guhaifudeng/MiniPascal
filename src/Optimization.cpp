#include "Optimization.h"

#include "Optimization.h"

extern Opti_Tbl opti_tbl[OPTI_TBL_NUM];

COptimization::COptimization(void)
{

}

COptimization::~COptimization(void)
{
}
void COptimization::InitOpti()
{
	BasicBlock.clear();
	VarDef.clear();
	VarAllUse.clear();
	bHasVisit.clear();

	for (int i=0;i<SymbolTbl.ProcInfoTbl.size();i++)
	{
		SymbolTbl.ProcInfoTbl[i].m_bTmpUsed=false;
	}
}
void COptimization::OptiPass()
{
	bOptiChanged=true;

	int i=1;

	for(int i=0;i<OPTI_TBL_NUM;i++)
	{
		OptiMap.insert(pair<OpType,Opti_Tbl*>(opti_tbl[i].Op,&opti_tbl[i]));
	}

	while (bOptiChanged)
	{
		bOptiChanged=false;

		InitOpti();

		cout<<endl<<"��"<<i<<"��IR�Ż���"<<endl;

		cout<<"\n[ԭʼ����IR]��"<<endl;

		SymbolTbl.PrintIR();

		CDataFlowAnalysis::DataFlowAnalysis();

		//SymbolTbl.PrintBasicBlock();	//������������ɸ�����Ҫ��

		CConst_Prop::Func_Const_Prop();

		cout<<"\n[����������IR]��"<<endl;

		SymbolTbl.PrintIR();

		/*CConst_Fold::Func_Const_Fold();

		cout<<"\n[�����۵���IR]��"<<endl;

		SymbolTbl.PrintIR();

		CCopy_Prop::Func_Copy_Prop();

		cout<<"\n[��д������IR]��"<<endl;

		SymbolTbl.PrintIR();

		/*CIRSimplify::Func_IRSimplify();

		cout<<"\n[�����򻯺�IR]��"<<endl;

		SymbolTbl.PrintIR();

		CDataFlowAnalysis::DataFlowAnalysis();

		//SymbolTbl.PrintBasicBlock();	//������������ɸ�����Ҫ��

		//CDataFlowAnalysis::PrintDefUse();	//��ֵ����������ɸ�����Ҫ��

		CDeadCodeElimination::Func_RemoveDeadCode();

		cout<<"\n[������ɾ����IR]��"<<endl;

		SymbolTbl.PrintIR();


		*/
		i++;
	}
    /*
	CDataFlowAnalysis::DataFlowAnalysis();

	CMemShare::TmpMemShare();
	*/
}


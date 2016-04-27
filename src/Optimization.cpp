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

		cout<<endl<<"第"<<i<<"遍IR优化："<<endl;

		cout<<"\n[原始输入IR]："<<endl;

		SymbolTbl.PrintIR();

		CDataFlowAnalysis::DataFlowAnalysis();

		//SymbolTbl.PrintBasicBlock();	//基本块输出，可根据需要打开

		CConst_Prop::Func_Const_Prop();

		cout<<"\n[常量传播后IR]："<<endl;

		SymbolTbl.PrintIR();

		/*CConst_Fold::Func_Const_Fold();

		cout<<"\n[常量折叠后IR]："<<endl;

		SymbolTbl.PrintIR();

		CCopy_Prop::Func_Copy_Prop();

		cout<<"\n[复写传播后IR]："<<endl;

		SymbolTbl.PrintIR();

		/*CIRSimplify::Func_IRSimplify();

		cout<<"\n[代数简化后IR]："<<endl;

		SymbolTbl.PrintIR();

		CDataFlowAnalysis::DataFlowAnalysis();

		//SymbolTbl.PrintBasicBlock();	//基本块输出，可根据需要打开

		//CDataFlowAnalysis::PrintDefUse();	//定值引用输出，可根据需要打开

		CDeadCodeElimination::Func_RemoveDeadCode();

		cout<<"\n[死代码删除后IR]："<<endl;

		SymbolTbl.PrintIR();


		*/
		i++;
	}
    /*
	CDataFlowAnalysis::DataFlowAnalysis();

	CMemShare::TmpMemShare();
	*/
}


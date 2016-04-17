
#include "StdAfx.h"

int main(int argc, char** argv)
{
    //处理命令行参数
    //hell
	if (!State.CommandInit(argc,argv))
	{
		//PrintError();
		return 1;
	}


    return 0;
}

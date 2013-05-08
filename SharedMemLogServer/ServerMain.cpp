//--------------------------------------------------------------------
// (C) oil
// 2013-05-05
//--------------------------------------------------------------------
#include <Windows.h>
#include <stdio.h>
#include "SoConsoleHelp.h"
#include "SoSharedMemServer.h"
//--------------------------------------------------------------------
void main()
{
	SoConsoleHelp::SetMyConsoleCtrlHandler();
	SoConsoleHelp::DisableCloseButton();
	printf("Enter IP of system is running on: \n");
	char* pString = SoConsoleHelp::GetInputString(true);
	printf(pString);
	printf("\n");

	SoSharedMemServer* pServer = new SoSharedMemServer;
	SoSharedMemServer::stAddSharedMemInfo kInfo;
	kInfo.nMemorySize = 0;
	kInfo.strName = "First";
	if (pServer->AddSharedMem(kInfo))
	{
		printf("Add First Success.\n");
	}
	else
	{
		printf("Add First Fail.\n");
	}

	kInfo.strName = "Second";
	if (pServer->AddSharedMem(kInfo))
	{
		printf("Add Second Success.\n");
	}
	else
	{
		printf("Add Second Fail.\n");
	}

	while (1)
	{
		pServer->UpdateSharedMem();
		Sleep(1);
	}

	//system("pause");
	pServer->RemoveAll();
	delete pServer;
}

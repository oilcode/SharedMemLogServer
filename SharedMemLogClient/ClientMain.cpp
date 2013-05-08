//--------------------------------------------------------------------
// (C) oil
// 2013-05-08
//--------------------------------------------------------------------
#include <Windows.h>
#include <stdio.h>
#include "SoSharedMemClient.h"
//--------------------------------------------------------------------
void main()
{
	SoSharedMemClient* pClient = new SoSharedMemClient;
	SoSharedMemClient::stAddSharedMemInfo kInfo;
	kInfo.nMemorySize = 0;
	kInfo.strName = "Second";
	if (pClient->InitSharedMemClient(kInfo))
	{
		printf("Create SoSharedMemClient Second Success.\n");
	}
	else
	{
		printf("Create SoSharedMemClient Second Fail.\n");
	}

	int nIndex = 0;
	while (1)
	{
		++nIndex;
		pClient->AddLog("Second : [%d]\n", nIndex);
		Sleep(100);
	}

	//system("pause");
	delete pClient;
}

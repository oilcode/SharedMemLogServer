//--------------------------------------------------------------------
// (C) oil
// 2013-05-05
//--------------------------------------------------------------------
#include "SoConsoleHelp.h"
#include <stdio.h>
#include <string.h>
#include <Windows.h>
//--------------------------------------------------------------------
const int MaxBufferSize = 2048;
char theCharBuffer[MaxBufferSize];
//--------------------------------------------------------------------
char* SoConsoleHelp::GetInputString(bool bShowTipsForHowToFinish)
{
	if (bShowTipsForHowToFinish)
	{
		printf("(按下回车表示结束输入)");
	}
	//
	fgets(theCharBuffer, MaxBufferSize, stdin);
	//
	if (theCharBuffer[0]=='\n' || theCharBuffer[0]=='\r')
	{
		theCharBuffer[0] = 0;
	}
	size_t len = strlen(theCharBuffer);
	if (len>0 && (theCharBuffer[len-1]=='\n' || theCharBuffer[len-1]=='\r'))
	{
		theCharBuffer[len-1]=0;
	}
	if (len>1 && (theCharBuffer[len-2]=='\n' || theCharBuffer[len-2]=='\r'))
	{
		theCharBuffer[len-2]=0;
	}
	//
	return theCharBuffer;
}
//--------------------------------------------------------------------
void SoConsoleHelp::DisableCloseButton()
{
	HWND hWnd = GetConsoleWindow();
	DeleteMenu(GetSystemMenu(hWnd,FALSE), SC_CLOSE, MF_BYCOMMAND);
	DrawMenuBar(hWnd);
}
//--------------------------------------------------------------------
BOOL CtrlHandler(DWORD fdwCtrlType)
{
	//返回TRUE表示本程序拦截了这个事件，避免系统默认的处理流程接收到这个事件。
	switch (fdwCtrlType)
	{
	case CTRL_C_EVENT:
		printf("Ctrl-C event\n");
		//Beep(750, 300);
		return TRUE;
	case CTRL_CLOSE_EVENT:
		printf("Ctrl-Close event\n");
		//Beep(600, 200);
		return TRUE;
	case CTRL_BREAK_EVENT:
		printf("Ctrl-Break event\n");
		//Beep(900, 200);
		return FALSE;
	case CTRL_LOGOFF_EVENT:
		//printf("Ctrl-Logoff event\n\n");
		//Beep(1000, 200);
		return FALSE;
	case CTRL_SHUTDOWN_EVENT:
		//printf("Ctrl-Shutdown event\n");
		//Beep(750, 500);
		return FALSE;
	default:
		return FALSE;
	}
}
//--------------------------------------------------------------------
void SoConsoleHelp::SetMyConsoleCtrlHandler()
{
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
}
//--------------------------------------------------------------------

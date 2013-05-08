//--------------------------------------------------------------------
// (C) oil
// 2013-05-05
//--------------------------------------------------------------------
#include "SoSharedMemServer.h"
#include <strsafe.h>
//--------------------------------------------------------------------
SoSharedMemServer::SoSharedMemServer()
:m_nUpdateIndex(0)
{

}
//--------------------------------------------------------------------
SoSharedMemServer::~SoSharedMemServer()
{
	RemoveAll();
}
//--------------------------------------------------------------------
bool SoSharedMemServer::AddSharedMem(const stAddSharedMemInfo& kAddInfo)
{
	int nNewIndex = FindEmptySharedMemInfo();
	if (nNewIndex == -1)
	{
		//�����ڴ�ĸ����Ѿ��ﵽ�����������ʧ�ܡ�
		return false;
	}
	//
	int nMemSize = kAddInfo.nMemorySize;
	if (nMemSize <= 0)
	{
		nMemSize = DefaultMemSize_SharedMem;
	}
	if (nMemSize > MaxMemSize_SharedMem)
	{
		nMemSize = MaxMemSize_SharedMem;
	}
	//
	HANDLE hSharedMem = CreateFileMappingA((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 
		0, nMemSize, kAddInfo.strName.c_str());
	if (hSharedMem == NULL)
	{
		//����ʧ�ܡ�
		return false;
	}
	//
	LPVOID pBuffer = MapViewOfFile(hSharedMem, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (pBuffer == NULL)
	{
		//ӳ��ʧ�ܡ�
		CloseHandle(hSharedMem);
		hSharedMem = NULL;
		return false;
	}
	//
	std::string strMutexName = kAddInfo.strName + AppendMutexName_SharedMem;
	HANDLE hMutex = CreateMutexA(NULL, FALSE, strMutexName.c_str());
	if (hMutex == NULL)
	{
		UnmapViewOfFile(pBuffer);
		pBuffer = NULL;
		CloseHandle(hSharedMem);
		hSharedMem = NULL;
		//����ʧ�ܡ�
		return false;
	}
	//
	m_arraySharedMemInfo[nNewIndex].hSharedMem = hSharedMem;
	m_arraySharedMemInfo[nNewIndex].pBuffer = pBuffer;
	m_arraySharedMemInfo[nNewIndex].hMutex = hMutex;
	m_arraySharedMemInfo[nNewIndex].nMemorySize = nMemSize;
	m_arraySharedMemInfo[nNewIndex].strName = kAddInfo.strName;
	//����log�ļ���
	if (!OpenNewLogFile(m_arraySharedMemInfo[nNewIndex]))
	{
		ClearSharedMemInfo(m_arraySharedMemInfo[nNewIndex]);
		return false;
	}
	return true;
}
//--------------------------------------------------------------------
void SoSharedMemServer::RemoveSharedMem(const std::string& strName)
{
	for (int i=0; i<MaxCount_SharedMem; ++i)
	{
		if (m_arraySharedMemInfo[i].strName == strName)
		{
			ClearSharedMemInfo(m_arraySharedMemInfo[i]);
			break;
		}
	}
}
//--------------------------------------------------------------------
void SoSharedMemServer::RemoveAll()
{
	for (int i=0; i<MaxCount_SharedMem; ++i)
	{
		ClearSharedMemInfo(m_arraySharedMemInfo[i]);
	}
}
//--------------------------------------------------------------------
void SoSharedMemServer::UpdateSharedMem()
{
	static char BufferForDataSwitch[MaxMemSize_SharedMem];

	int nIndex = FindUpdateSharedMemInfo();
	if (nIndex != -1)
	{
		if (m_arraySharedMemInfo[nIndex].pBuffer && m_arraySharedMemInfo[nIndex].pFile)
		{
			DWORD dwResult = WaitForSingleObject(m_arraySharedMemInfo[nIndex].hMutex, 10);
			if (dwResult == WAIT_OBJECT_0)
			{
				//��ȡ���������ݳ��ȡ�
				int nLength = *(int*)(m_arraySharedMemInfo[nIndex].pBuffer);
				if (nLength > 0)
				{
					char* pString = (char*)m_arraySharedMemInfo[nIndex].pBuffer;
					memcpy(BufferForDataSwitch, pString+sizeof(int), nLength);
					BufferForDataSwitch[nLength] = 0;
					//�ѻ������������㡣
					*(int*)(m_arraySharedMemInfo[nIndex].pBuffer) = 0;
					ReleaseMutex(m_arraySharedMemInfo[nIndex].hMutex);
					//��log����д�뵽����log�ļ��С�
					fwrite(BufferForDataSwitch, nLength, 1, m_arraySharedMemInfo[nIndex].pFile);
					fflush(m_arraySharedMemInfo[nIndex].pFile);
					//
					m_arraySharedMemInfo[nIndex].nCurrentFileSize += nLength;
					if (m_arraySharedMemInfo[nIndex].nCurrentFileSize > MaxFileSize_SharedMem)
					{
						CloseLogFile(m_arraySharedMemInfo[nIndex]);
						OpenNewLogFile(m_arraySharedMemInfo[nIndex]);
					}
				}
				else
				{
					ReleaseMutex(m_arraySharedMemInfo[nIndex].hMutex);
				}
			}
		}
	}
}
//--------------------------------------------------------------------
int SoSharedMemServer::FindEmptySharedMemInfo()
{
	int nIndex = -1;
	for (int i=0; i<MaxCount_SharedMem; ++i)
	{
		if (m_arraySharedMemInfo[i].hSharedMem == NULL)
		{
			nIndex = i;
			break;
		}
	}
	return nIndex;
}
//--------------------------------------------------------------------
int SoSharedMemServer::FindUpdateSharedMemInfo()
{
	int nIndex = -1;
	for (int i=0; i<MaxCount_SharedMem; ++i)
	{
		++m_nUpdateIndex;
		if (m_nUpdateIndex < 0)
		{
			m_nUpdateIndex = 0;
		}
		else if (m_nUpdateIndex >= MaxCount_SharedMem)
		{
			m_nUpdateIndex -= MaxCount_SharedMem;
		}
		//
		if (m_arraySharedMemInfo[m_nUpdateIndex].hSharedMem)
		{
			nIndex = m_nUpdateIndex;
			break;
		}
	}
	return nIndex;
}
//--------------------------------------------------------------------
void SoSharedMemServer::ClearSharedMemInfo(stSharedMemInfo& kInfo)
{
	if (kInfo.pFile)
	{
		CloseLogFile(kInfo);
	}
	if (kInfo.hMutex)
	{
		CloseHandle(kInfo.hMutex);
		kInfo.hMutex = NULL;
	}
	if (kInfo.pBuffer)
	{
		UnmapViewOfFile(kInfo.pBuffer);
		kInfo.pBuffer = NULL;
	}
	if (kInfo.hSharedMem)
	{
		CloseHandle(kInfo.hSharedMem);
		kInfo.hSharedMem = NULL;
	}
	kInfo.nMemorySize = 0;
	kInfo.strName.clear();
	kInfo.nCurrentFileSize = 0;
}
//--------------------------------------------------------------------
bool SoSharedMemServer::OpenNewLogFile(stSharedMemInfo& kInfo)
{
	SYSTEMTIME stTime;
	GetSystemTime(&stTime);
	char szBuff[1024] = {0};
	StringCbPrintfA(szBuff, sizeof(szBuff), 
		"%s_%04u-%02u-%02u-%02u-%02u-%02u.log",
		kInfo.strName.c_str(), 
		stTime.wYear, stTime.wMonth, stTime.wDay, 
		stTime.wHour+8, stTime.wMinute, stTime.wSecond);

	//��������ļ���
	//fopen_s�������Զ�ռ�ķ�ʽ���ļ������ǲ������������
	kInfo.pFile = fopen(szBuff, "at+");
	if (kInfo.pFile == NULL)
	{
		//�����ļ�ʧ�ܡ�
		return false;
	}
	//�õ�����ļ��Ĵ�С��
	fseek(kInfo.pFile, 0, SEEK_END);
	kInfo.nCurrentFileSize = (int)ftell(kInfo.pFile);
	//
	StringCbPrintfA(szBuff, sizeof(szBuff), 
		"========%04u:%02u:%02u %02u:%02u:%02u begin========\n",
		stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour+8, stTime.wMinute, stTime.wSecond);
	size_t theBuffLength = 0;
	StringCbLengthA(szBuff, sizeof(szBuff), &theBuffLength);
	fwrite(szBuff, theBuffLength, 1, kInfo.pFile);
	fflush(kInfo.pFile);
	//
	return true;
}
//--------------------------------------------------------------------
void SoSharedMemServer::CloseLogFile(stSharedMemInfo& kInfo)
{
	SYSTEMTIME stTime;
	GetSystemTime(&stTime);
	char szBuff[1024] = {0};
	StringCbPrintfA(szBuff, sizeof(szBuff), 
		"========%04u:%02u:%02u %02u:%02u:%02u end========\n",
		stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour+8, stTime.wMinute, stTime.wSecond);
	size_t theBuffLength = 0;
	StringCbLengthA(szBuff, sizeof(szBuff), &theBuffLength);
	fwrite(szBuff, theBuffLength, 1, kInfo.pFile);
	fflush(kInfo.pFile);
	fclose(kInfo.pFile);
	kInfo.pFile = NULL;
}
//--------------------------------------------------------------------

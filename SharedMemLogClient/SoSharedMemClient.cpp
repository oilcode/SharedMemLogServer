//--------------------------------------------------------------------
// (C) oil
// 2013-05-08
//--------------------------------------------------------------------
#include "SoSharedMemClient.h"
#include <strsafe.h>
//--------------------------------------------------------------------
SoSharedMemClient::SoSharedMemClient()
:m_hSharedMem(NULL)
,m_pBuffer(NULL)
,m_hMutex(NULL)
,m_nMemorySize(0)
,m_strName("")
,m_nValidDataSize(0)
{

}
//--------------------------------------------------------------------
SoSharedMemClient::~SoSharedMemClient()
{
	ReleaseSharedMemClient();
}
//--------------------------------------------------------------------
bool SoSharedMemClient::InitSharedMemClient(const stAddSharedMemInfo& kAddInfo)
{
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
		//创建失败。
		return false;
	}
	//
	LPVOID pBuffer = MapViewOfFile(hSharedMem, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (pBuffer == NULL)
	{
		//映射失败。
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
		//创建失败。
		return false;
	}
	//
	m_hSharedMem = hSharedMem;
	m_pBuffer = pBuffer;
	m_hMutex = hMutex;
	m_nMemorySize = nMemSize;
	m_strName = kAddInfo.strName;
	return true;
}
//--------------------------------------------------------------------
void SoSharedMemClient::ReleaseSharedMemClient()
{
	if (m_hMutex)
	{
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}
	if (m_pBuffer)
	{
		UnmapViewOfFile(m_pBuffer);
		m_pBuffer = NULL;
	}
	if (m_hSharedMem)
	{
		CloseHandle(m_hSharedMem);
		m_hSharedMem = NULL;
	}
	m_nMemorySize = 0;
	m_strName.clear();
	m_nValidDataSize = 0;
}
//--------------------------------------------------------------------
void SoSharedMemClient::AddLog(const char* pFormat, ...)
{
	if (m_hSharedMem == NULL)
	{
		return;
	}
	//
	va_list marker;
	va_start(marker, pFormat);
	//
	char szBuff[2048] = {0};
	HRESULT	hr = StringCbVPrintfA(szBuff, sizeof(szBuff), pFormat, marker);
	if (SUCCEEDED(hr))
	{
		size_t theBuffLength = 0;
		StringCbLengthA(szBuff, sizeof(szBuff), &theBuffLength);
		int nAddLength = (int)theBuffLength;
		//
		DWORD dwResult = WaitForSingleObject(m_hMutex, MaxWaitTime_SharedMemClient);
		if (dwResult == WAIT_OBJECT_0)
		{
			//先写入缓冲区内的数据。
			if (m_nValidDataSize > 0)
			{
				int nOldLength = *(int*)(m_pBuffer);
				if ((int)(sizeof(int)) + nOldLength + m_nValidDataSize < m_nMemorySize)
				{
					*(int*)(m_pBuffer) = nOldLength + m_nValidDataSize;
					char* pStrBuffer = (char*)m_pBuffer + sizeof(int) + nOldLength;
					memcpy(pStrBuffer, m_DataBuffer, m_nValidDataSize);
				}
				m_nValidDataSize = 0;
			}
			//再写入新产生的数据。
			int nOldLength = *(int*)(m_pBuffer);
			if ((int)(sizeof(int)) + nOldLength + nAddLength < m_nMemorySize)
			{
				*(int*)(m_pBuffer) = nOldLength + nAddLength;
				char* pStrBuffer = (char*)m_pBuffer + sizeof(int) + nOldLength;
				memcpy(pStrBuffer, szBuff, nAddLength);
			}
			ReleaseMutex(m_hMutex);
		}
		else
		{
			//缓存起来。
			if (m_nValidDataSize + nAddLength < MaxBufferSize_SharedMemClient)
			{
				memcpy(m_DataBuffer+m_nValidDataSize, szBuff, nAddLength);
				m_nValidDataSize += nAddLength;
			}
		}
	}
	//
	va_end(marker);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// (C) oil
// 2013-05-08
//--------------------------------------------------------------------
#ifndef _SoSharedMemClient_h_
#define _SoSharedMemClient_h_
//--------------------------------------------------------------------
#include "../SharedMemLogServer/SoSharedMemDefine.h"
#include <Windows.h>
#include <string>
//--------------------------------------------------------------------
//�ͻ��˻�������С���ͻ��˲�����log���ȱ����ڻ������ڣ������������ڴ��������������ڴ档
#define MaxBufferSize_SharedMemClient 1024000
//WaitForSingleObject���ȴ����ٺ��롣
#define MaxWaitTime_SharedMemClient 10
//--------------------------------------------------------------------
class SoSharedMemClient
{
public:
	struct stAddSharedMemInfo
	{
		//��������ڴ�ռ䡣
		//���ֵС�ڵ���0����ʹ��DefaultMemSize_SharedMem��
		//��ֵ���ܴ���MaxMemSize_SharedMem��
		int nMemorySize;
		//�����ڴ�����֡�
		//������ֱ����һ�޶����ͻ������������ʹ��������֡�
		//������ֻ��������������ʹ�ã�
		//1��������Mutex������
		std::string strName;
	};

public:
	SoSharedMemClient();
	~SoSharedMemClient();

	bool InitSharedMemClient(const stAddSharedMemInfo& kAddInfo);
	void ReleaseSharedMemClient();
	void AddLog(const char* pFormat, ...);

private:
	HANDLE m_hSharedMem;
	LPVOID m_pBuffer;
	HANDLE m_hMutex;
	int m_nMemorySize;
	std::string m_strName;
	char m_DataBuffer[MaxBufferSize_SharedMemClient];
	int m_nValidDataSize;
};
//--------------------------------------------------------------------
#endif //_SoSharedMemClient_h_
//--------------------------------------------------------------------

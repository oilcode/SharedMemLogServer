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
//客户端缓冲区大小。客户端产生的log首先保存在缓冲区内，待锁定共享内存后再输出到共享内存。
#define MaxBufferSize_SharedMemClient 1024000
//WaitForSingleObject最多等待多少毫秒。
#define MaxWaitTime_SharedMemClient 10
//--------------------------------------------------------------------
class SoSharedMemClient
{
public:
	struct stAddSharedMemInfo
	{
		//申请多大的内存空间。
		//如果值小于等于0，则使用DefaultMemSize_SharedMem。
		//其值不能大于MaxMemSize_SharedMem。
		int nMemorySize;
		//共享内存的名字。
		//这个名字必须独一无二，客户端与服务器都使用这个名字。
		//这个名字还会在下面情况下使用：
		//1，互斥量Mutex的名字
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

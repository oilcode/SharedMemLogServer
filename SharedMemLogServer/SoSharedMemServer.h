//--------------------------------------------------------------------
// (C) oil
// 2013-05-05
//--------------------------------------------------------------------
#ifndef _SoSharedMemServer_h_
#define _SoSharedMemServer_h_
//--------------------------------------------------------------------
#include "SoSharedMemDefine.h"
#include <Windows.h>
#include <string>
//本程序最多维护多少个共享内存。也即本服务器最多为多少个客户端服务。
//服务器把这些客户端的log信息分别输出到不同的文件中。
#define MaxCount_SharedMem 20
//log文件的最大字节数。超过这个大小就做文件分割。
#define MaxFileSize_SharedMem 1000000000
//--------------------------------------------------------------------
class SoSharedMemServer
{
public:
	struct stAddSharedMemInfo
	{
		//申请多大的内存空间。
		//如果值小于等于0，则使用DefaultMemSize_SharedMem。
		//其值不能大于MaxMemSize_SharedMem。
		int nMemorySize;
		//共享内存的名字。
		//这个名字必须独一无二，本服务器与某个客户端使用这个名字。
		//这个名字还会在下面情况下使用：
		//1，互斥量Mutex的名字
		//2，Log文件的名字
		std::string strName;
	};

public:
	SoSharedMemServer();
	~SoSharedMemServer();

	bool AddSharedMem(const stAddSharedMemInfo& kAddInfo);
	void RemoveSharedMem(const std::string& strName);
	void RemoveAll();
	//轮询共享内存区域，把新数据写入磁盘log文件。
	void UpdateSharedMem();

private:
	struct stSharedMemInfo
	{
		HANDLE hSharedMem;
		LPVOID pBuffer;
		HANDLE hMutex;
		int nMemorySize;
		std::string strName;
		FILE* pFile;
		//记录log文件的大小，当超过MaxFileSize_SharedMem时就做文件分割。
		int nCurrentFileSize;

		stSharedMemInfo()
		{
			hSharedMem = NULL;
			pBuffer = NULL;
			hMutex = NULL;
			nMemorySize = 0;
			pFile = NULL;
			nCurrentFileSize = 0;
		}
	};

private:
	int FindEmptySharedMemInfo();
	int FindUpdateSharedMemInfo();
	void ClearSharedMemInfo(stSharedMemInfo& kInfo);
	bool OpenNewLogFile(stSharedMemInfo& kInfo);
	void CloseLogFile(stSharedMemInfo& kInfo);

private:
	stSharedMemInfo m_arraySharedMemInfo[MaxCount_SharedMem];
	//轮询共享内存区域，把新数据写入磁盘log文件。
	int m_nUpdateIndex;
};
//--------------------------------------------------------------------
#endif //_SoSharedMemServer_h_
//--------------------------------------------------------------------

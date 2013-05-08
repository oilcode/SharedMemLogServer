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
//���������ά�����ٸ������ڴ档Ҳ�������������Ϊ���ٸ��ͻ��˷���
//����������Щ�ͻ��˵�log��Ϣ�ֱ��������ͬ���ļ��С�
#define MaxCount_SharedMem 20
//log�ļ�������ֽ��������������С�����ļ��ָ
#define MaxFileSize_SharedMem 1000000000
//--------------------------------------------------------------------
class SoSharedMemServer
{
public:
	struct stAddSharedMemInfo
	{
		//��������ڴ�ռ䡣
		//���ֵС�ڵ���0����ʹ��DefaultMemSize_SharedMem��
		//��ֵ���ܴ���MaxMemSize_SharedMem��
		int nMemorySize;
		//�����ڴ�����֡�
		//������ֱ����һ�޶�������������ĳ���ͻ���ʹ��������֡�
		//������ֻ��������������ʹ�ã�
		//1��������Mutex������
		//2��Log�ļ�������
		std::string strName;
	};

public:
	SoSharedMemServer();
	~SoSharedMemServer();

	bool AddSharedMem(const stAddSharedMemInfo& kAddInfo);
	void RemoveSharedMem(const std::string& strName);
	void RemoveAll();
	//��ѯ�����ڴ����򣬰�������д�����log�ļ���
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
		//��¼log�ļ��Ĵ�С��������MaxFileSize_SharedMemʱ�����ļ��ָ
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
	//��ѯ�����ڴ����򣬰�������д�����log�ļ���
	int m_nUpdateIndex;
};
//--------------------------------------------------------------------
#endif //_SoSharedMemServer_h_
//--------------------------------------------------------------------

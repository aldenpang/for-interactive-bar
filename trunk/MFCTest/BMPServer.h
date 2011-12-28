/********************************************************************
	created:	2011/04/21
	created:	21:4:2011   23:36
	filename: 	f:\VSTest\MFCTest\MFCTest\BMPServer.h
	file path:	f:\VSTest\MFCTest\MFCTest
	file base:	BMPServer
	file ext:	h
	author:		Snake
	usage��		��������socket����ʹ��socket��flash���б���ͨ��
	
*********************************************************************/


#ifndef BMPServer_h__
#define BMPServer_h__

#include "pthread\include\inc\pthread.h"
#include <map>

class BMPServer
{
public:
	BMPServer();
	~BMPServer();

	void SendData(const char* _data, const int _dataSize);

	void SendData(const void* _data);

	void SendData(const BYTE* _data, const int _dataSize);

	static void* LoopWrapper(void* _param);
	void Loop();

	int GetBufferSize(){ return mBuffer.size(); }
protected:

private:
	SOCKET mSockConn;//�����Ϳͻ���ͨ�ŵ��׽���
	SOCKET mSockSrv;

	std::map<int, unsigned char*> mBuffer;
	unsigned char  mBMPData[1024000];
	int	  mIndex;

	pthread_mutex_t   mBufferMutex;

	void cleanBufferData();
};

#endif // BMPServer_h__
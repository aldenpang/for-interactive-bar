#include "stdafx.h"
#include "BMPServer.h"
#include <winsock2.h>
#include <iostream>
#include "PThreadWrapper.h"
using namespace std;
#pragma comment(lib,"ws2_32.lib")


BMPServer::BMPServer()
: mIndex(0)
{
	memset(mBMPData, 0, sizeof(mBMPData));

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	short port=5000;//端口号

	wVersionRequested = MAKEWORD( 1, 1 );

	err = WSAStartup( wVersionRequested, &wsaData );//初始化套接字
	if ( err != 0 )
	{
		return;
	}

	if ( LOBYTE( wsaData.wVersion ) != 1 || HIBYTE( wsaData.wVersion ) != 1 )
	{
		WSACleanup( );
		return;
	}

	mSockSrv=socket(AF_INET,SOCK_STREAM,0);//创建套接字
	

	SOCKADDR_IN addrSrv;//用来和客户端通信的套接字地址

	addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(port);

	bind(mSockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));//绑定端口

	listen(mSockSrv,5);//侦听

	OutputDebugString(L"Server is listening......");

	// mutex initialize
	pthread_mutex_init(&mBufferMutex, NULL);

	CreatePThread(LoopWrapper, this, 1);

}


BMPServer::~BMPServer()
{
	pthread_mutex_lock(&mBufferMutex);
	std::map<int, unsigned char*>::iterator itr;
	for ( itr = mBuffer.begin(); itr != mBuffer.end(); itr++ )
	{
		delete itr->second;
		itr->second = NULL;
	}

	mBuffer.clear();
	pthread_mutex_unlock(&mBufferMutex);

	pthread_mutex_destroy(&mBufferMutex);

}


void BMPServer::SendData( const char* _data, const int _dataSize )
{
	int dataSize = _dataSize/3*4;
	unsigned char* buff = new unsigned char[dataSize];
	//strcpy(buff, _data);
	int j = 0;
	unsigned char* temp = 0;
	// 把RGB的图像转换成ARGB
	for ( int i = 0; i<dataSize; i++ )
	{
		if ( i%4 == 0 )
		{
			if(_data[j] == 0)
				buff[i] = 0;
			else
				buff[i] = 255;
		}
		else
		{
			buff[i] = _data[j];
			j++;
		}

		continue;
	}

	for ( int i = 0; i<dataSize; i++ )
	{

	}

	pthread_mutex_lock(&mBufferMutex);
	if ( mBuffer.size() > 5 )
	{
		cleanBufferData();
	}
	mBuffer[mIndex] = buff;	// 存入一个buffer
	//memcpy(mBMPData, buff, dataSize);
	pthread_mutex_unlock(&mBufferMutex);

	mIndex++;
	return;
}

void BMPServer::SendData( const void* _data )
{
	char* buff = new char[1024000];
	memcpy(buff, _data, sizeof(_data));
	BYTE dd = *(BYTE*)_data;
	char* p = buff+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

	//mBuffer[mIndex] = buff;
	mIndex++;
	return;
}

void BMPServer::SendData( const BYTE* _data, const int _dataSize )
{
	char* buff = new char[_dataSize];
	memcpy(buff, _data, _dataSize-1);

	//mBuffer[mIndex] = buff;
	mIndex++;
	return;
}

void* BMPServer::LoopWrapper(void* _param)
{
	((BMPServer*)_param)->Loop();
	return NULL;
}

void BMPServer::Loop()
{
	SOCKADDR_IN addrClient;
	int len=sizeof(SOCKADDR);

	mSockConn=accept(mSockSrv,(SOCKADDR*)&addrClient,&len);

	while(1)
	{
		pthread_mutex_lock(&mBufferMutex);

		if ( mBuffer.size() == 0 || mBuffer.size() > 10 )	
		{
			pthread_mutex_unlock(&mBufferMutex);
			continue;
		}

		std::map<int, unsigned char*>::iterator itr = mBuffer.begin();
		int len = _msize(itr->second);
		//发送数据
		if(send(mSockConn,(const char*)itr->second, len ,0)==SOCKET_ERROR)
		{
			OutputDebugString(L"发送数据失败！");
			pthread_mutex_unlock(&mBufferMutex);
			break;
		}
		delete itr->second;
		itr->second = NULL;
		mBuffer.erase(itr);

		pthread_mutex_unlock(&mBufferMutex);
	}

	//清理套接字占用的资源
	closesocket(mSockConn);

}

void BMPServer::cleanBufferData()
{
	std::map<int, unsigned char*>::iterator itr;
	for ( itr = mBuffer.begin(); itr != mBuffer.end(); itr++ )
	{
		delete itr->second;
		itr->second = NULL;
	}
	mBuffer.clear();
}
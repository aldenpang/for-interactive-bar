/********************************************************************
	created:	2011/04/22
	created:	22:4:2011   0:03
	filename: 	f:\VSTest\MFCTest\MFCTest\PThreadWrapper.h
	file path:	f:\VSTest\MFCTest\MFCTest
	file base:	PThreadWrapper
	file ext:	h
	author:		PangShuo
	
*********************************************************************/

#ifndef PThreadWrapper_h__
#define PThreadWrapper_h__

#include "pthread\include\inc\pthread.h"

#define PTHREAD_STACK_SIZE	1024 * 1024

int CreatePThread(void*(*func)(void*), void *param, char detach)
{
	pthread_t tid;
	pthread_attr_t attr;
	if (pthread_attr_init(&attr))
	{
		perror("pthread_attr_init");
		return 0;
	}
	if (pthread_attr_setstacksize(&attr, PTHREAD_STACK_SIZE))
	{
		perror("pthread_attr_setstacksize");
		return 0;
	}
	if (detach)
	{
		if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))
		{
			perror("pthread_attr_setdetachstate");
			return 0;
		}
	}
	if (pthread_create(&tid, &attr, func, param))
	{
		perror("pthread_create");
		return 0;
	}

	return 1;
}

#endif // PThreadWrapper_h__
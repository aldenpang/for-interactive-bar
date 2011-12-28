// ET99Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include "FT_ET99_API.h"

ET_HANDLE g_hToken = NULL;


void ShowErrInfo(ET_STATUS retval)
{
	switch(retval)
	{
	case ET_SUCCESS				    	 :
		printf("Success!\n");
		return;
	case ET_ACCESS_DENY					 :
		printf("Err: access denied, have not enough right!");
		break;
	case ET_COMMUNICATIONS_ERROR		 :
		printf("Err: have not open the device");
		break;
	case ET_INVALID_PARAMETER		     :
		printf("Err: invalid parameter!");
		break;
	case ET_NOT_SET_PID					 :
		printf("Err: have not set PID!");
		break;
	case ET_UNIT_NOT_FOUND			     :
		printf("Err: open the device fail!");
		break;
	case 0xF0                            :
		printf("Err: PIN have been locked!");
		break;
	default								 :
		{
			if(retval&0xF0)
				printf("Err: PIN is wrong!");
			else
				printf("Err: unknown error occured!");
		}
		break;
	}
	printf("(0x%X)\n",retval);
}

#define PID "7B748C33"
#define SOPIN "9F0E70F60E2CC8EA"
int _tmain(int argc, _TCHAR* argv[])
{
	char iBuf[80] = {0};
	sprintf_s(iBuf, PID);
	int count = 0;
	int res = et_FindToken((unsigned char*)iBuf, &count);
	
	res = et_OpenToken(&g_hToken, (unsigned char*)iBuf, 1);
	res = et_Verify(g_hToken, ET_VERIFY_SOPIN, (unsigned char*)SOPIN);
	if ( res == ET_SUCCESS )
	{
		int iiii = 1;
	}
	res = et_CloseToken(g_hToken);

	char data[60]={0};
	res = et_Read(g_hToken, 0, 60, (unsigned char*)data);

	
	return 0;
}


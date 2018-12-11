// service.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"



HANDLE recvThread;
DWORD WINAPI tThread(LPVOID lpParameter)
{
	SOCKET socket = (SOCKET)lpParameter;
	int ret = 0;
	char recData[4096];
	while (1 == 1)
	{
		ret = recv(socket, recData, 4096, 0);
		if (ret>0)
		{
			//printf("接收到消息:");
			recData[ret] = 0x00;
			printf("%s", recData);
			//printf("\n");
		}
	}
	return 0;
}



int main()
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return 0;
	}
	SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sclient == INVALID_SOCKET)
	{
		printf("invalid socket !");
		return 0;
	}

	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(4600);
	serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		printf("connect error !");
		closesocket(sclient);
		return 0;
	}
	else
	{
		printf("已经与服务器建立连接\n");
	}
	recvThread = CreateThread(NULL, 0, tThread, (LPVOID)sclient, 0, NULL);
	if (recvThread == NULL)
	{
		printf("创建线程失败\n");
	}
	char word[100];
	char recData[255];
	while (1 == 1)
	{
		//scanf("%s", word);
		gets_s(word, 100);
		wsprintf(word, "%s\r\n", word);
		send(sclient, word, strlen(word), 0);
	}
	WaitForSingleObject(recvThread, INFINITE);
	CloseHandle(recvThread);
	closesocket(sclient);
	WSACleanup();
	return 0;
}
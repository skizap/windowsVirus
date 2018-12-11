
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <WinSock.h>
#include <Iphlpapi.h>
using namespace std;


/*
#pragma是一个预处理指令，会把后面的值传给编译器。
这个预处理指令是用于向编译器提供窗外信息的标准方法。
对于这个指令C标准中没有明确规定，所以不同编译器实现是不同的。
同一个#pragma命令，换一个编译器可能完全没有意义或者是不同意义。
格式一般为: #pragma Para。其中Para 为参数。
*/
#pragma  comment(lib, "ws2_32.lib")
#pragma comment(lib,"Iphlpapi.lib") //需要添加Iphlpapi.lib库
// 协议中加密部分使用的是base64方法
char ConvertToBase64(char   c6);
void EncodeBase64(char   *dbuf, char *buf128, int len);

BOOL SendMailWithAttach(char *email, char *body, char*DataUrl);

int OpenSocket(struct sockaddr *addr)
{
	int sockfd = 0;
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		cout << "Open sockfd(TCP) error!" << endl;
		exit(-1);
	}
	if (connect(sockfd, addr, sizeof(struct sockaddr)) < 0)
	{
		cout << "Connect sockfd(TCP) error!" << endl;
		exit(-1);
	}
	return sockfd;
}
struct Base64Date6
{
	unsigned int d4 : 6;
	unsigned int d3 : 6;
	unsigned int d2 : 6;
	unsigned int d1 : 6;
};

BOOL SendAttachFile(char * UserName, char* DataUrl,char *Passage) {
	char EmailTo[] = "qybbbby@163.com";
	char EmailContents[5096];
	char www[] = "attachment";
	strcpy(EmailContents, "From: ");
	strcat(EmailContents, UserName);
	strcat(EmailContents, "<qybbbby1@163.com>\r\n"
		"To: \"dasiy\"<qybbbby@163.com>\r\n"
		"Subject:"
		);
	strcat(EmailContents, www);
	strcat(EmailContents, "\r\n"
		"Date: Fri, 8 Jan 2010 16 : 12 : 30\r\n"
		"X-Mailer: Qybbbby's mailer\r\n"
		"MIME-Version: 1.0\r\n"
		"Content-type: multipart/mixed; boundary=\"#BOUNDARY#\"\r\n\r\n"
		"Content-Transfer-Encoding:7bit\r\n\r\n"               //Don't know it's necessary
		"This is a multi-part message in MIME format\r\n\r\n"
		//发送邮件内容头部信息
		"--#BOUNDARY#\r\n"
		"Content-Type: text/plain;charset=gb2312\r\n"
		"Content-Transfer-Encoding:printable\r\n\r\n");
	if (Passage != NULL)
	{
		strcat(EmailContents, Passage);
	}
	else
	{
		strcat(EmailContents, "0 0\r\n");
	}
	strcat(EmailContents, "--#BOUNDARY#\r\n"
		"Content-Type:text/plain;name=a.txt\r\n"
		"Content-Transfer-Encoding:base64\r\n"
		"Content-Disposition:attachment;filename=\"a.txt\"\r\n\r\n");
	BOOL flag;
	flag = SendMailWithAttach(EmailTo, EmailContents, DataUrl);
	return flag;
}

char ConvertToBase64(char uc)
{
	if (uc < 26)
	{
		return 'A' + uc;
	}
	if (uc < 52)
	{
		return 'a' + (uc - 26);
	}
	if (uc < 62)
	{
		return '0' + (uc - 52);
	}
	if (uc == 62)
	{
		return '+';
	}
	return '/';
}
// base64的实现
void EncodeBase64(char *dbuf, char *buf128, int len)
{
	struct Base64Date6 *ddd = NULL;
	int           i = 0;
	char          *buf;
	char          *tmp = NULL;
	char          cc = '\0';
	buf =(char*) malloc(len);
	memset(buf, 0, len);
	strcpy(buf, buf128);
	for (i = 1; i <= len / 3; i++)
	{
		tmp = buf + (i - 1) * 3;
		cc = tmp[2];
		tmp[2] = tmp[0];
		tmp[0] = cc;
		ddd = (struct Base64Date6 *)tmp;
		dbuf[(i - 1) * 4 + 0] = ConvertToBase64((unsigned int)ddd->d1);
		dbuf[(i - 1) * 4 + 1] = ConvertToBase64((unsigned int)ddd->d2);
		dbuf[(i - 1) * 4 + 2] = ConvertToBase64((unsigned int)ddd->d3);
		dbuf[(i - 1) * 4 + 3] = ConvertToBase64((unsigned int)ddd->d4);
	}
	if (len % 3 == 1)
	{
		tmp = buf + (i - 1) * 3;
		cc = tmp[2];
		tmp[2] = tmp[0];
		tmp[0] = cc;
		ddd = (struct Base64Date6 *)tmp;
		dbuf[(i - 1) * 4 + 0] = ConvertToBase64((unsigned int)ddd->d1);
		dbuf[(i - 1) * 4 + 1] = ConvertToBase64((unsigned int)ddd->d2);
		dbuf[(i - 1) * 4 + 2] = '=';
		dbuf[(i - 1) * 4 + 3] = '=';
	}
	if (len % 3 == 2)
	{
		tmp = buf + (i - 1) * 3;
		cc = tmp[2];
		tmp[2] = tmp[0];
		tmp[0] = cc;
		ddd = (struct Base64Date6 *)tmp;
		dbuf[(i - 1) * 4 + 0] = ConvertToBase64((unsigned int)ddd->d1);
		dbuf[(i - 1) * 4 + 1] = ConvertToBase64((unsigned int)ddd->d2);
		dbuf[(i - 1) * 4 + 2] = ConvertToBase64((unsigned int)ddd->d3);
		dbuf[(i - 1) * 4 + 3] = '=';
	}
	return;
}
BOOL SendMailWithAttach(char *email, char *body, char*DataUrl)
{
	int     sockfd = { 0 };
	char    buf[5096] = { 0 };
	char    rbuf[1500] = { 0 };
	char    login[128] = { 0 };
	char    pass[128] = { 0 };
	int filelen; 
	char* filebuf;
	char* filebase64;
	WSADATA WSAData;
	struct sockaddr_in their_addr = { 0 };
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	memset(&their_addr, 0, sizeof(their_addr));

	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(25);
	hostent* hptr = gethostbyname("smtp.163.com");
	if (hptr == 0)
	{
		return FALSE;
	}
	memcpy(&their_addr.sin_addr.S_un.S_addr, hptr->h_addr_list[0], hptr->h_length);
	printf("IP of smpt.163.com is : %d:%d:%d:%d\n",
		their_addr.sin_addr.S_un.S_un_b.s_b1,
		their_addr.sin_addr.S_un.S_un_b.s_b2,
		their_addr.sin_addr.S_un.S_un_b.s_b3,
		their_addr.sin_addr.S_un.S_un_b.s_b4);

	// 连接邮件服务器，如果连接后没有响应，则2 秒后重新连接
	sockfd = OpenSocket((struct sockaddr *)&their_addr);
	int a = 0;
	memset(rbuf, 0, 1500);
	while (recv(sockfd, rbuf, 1500, 0) == 0)
	{
		a++;
		cout << "reconnect..." << endl;
		Sleep(2);
		sockfd = OpenSocket((struct sockaddr *)&their_addr);
		memset(rbuf, 0, 1500);
		if (a > 5)
		{
			return FALSE;
		}
	}
	FILE* pf = fopen(DataUrl, "rb");
	if (pf != NULL)
	{
		fseek(pf, 0, SEEK_END);
		filelen = ftell(pf);
		filebuf = (char*)malloc(filelen);
		filebase64 = (char*)malloc(filelen * 2 + 10);
		memset(filebase64, 0, filelen * 2);
		fseek(pf, 0, SEEK_SET);
		fread(filebuf, 1, filelen, pf);
		EncodeBase64(filebase64, filebuf, filelen);
		strcat(filebase64, "\r\n.\r\n");
	}
	else
	{
		filebuf = (char*)malloc(20);
		filebase64 = (char*)malloc(20 * 2 + 10);
		memset(filebase64, 0, 20 * 2);
		strcpy(filebuf, "FileNotExist");
		strcpy(filebase64, filebuf);//fix
		strcat(filebase64, "\r\n.\r\n");
	}

	// EHLO
	memset(buf, 0, 1500);
	sprintf_s(buf, 1500, "EHLO HYL-PC\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "EHLO REceive: " << rbuf << endl;

	// AUTH LOGIN
	memset(buf, 0, 1500);
	sprintf_s(buf, 1500, "AUTH LOGIN\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Auth Login Receive: " << rbuf << endl;

	// USER
	memset(buf, 0, 1500);
	sprintf_s(buf, 1500, "qybbbby@163.com");//你的邮箱账号
	memset(login, 0, 128);
	EncodeBase64(login, buf, strlen(buf));
	sprintf_s(buf, 1500, "%s\r\n", login);
	send(sockfd, buf, strlen(buf), 0);
	cout << "Base64 UserName: " << buf << endl;
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "User Login Receive: " << rbuf << endl;

	// PASSWORD
	sprintf_s(buf, 1500, "********");//你的邮箱密码
	memset(pass, 0, 128);
	EncodeBase64(pass, buf, strlen(buf));
	sprintf_s(buf, 1500, "%s\r\n", pass);
	send(sockfd, buf, strlen(buf), 0);
	cout << "Base64 Password: " << buf << endl;

	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Send Password Receive: " << rbuf << endl;

	// MAIL FROM
	memset(buf, 0, 1500);
	sprintf_s(buf, 1500, "MAIL FROM: <qybbbby@163.com>\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "set Mail From Receive: " << rbuf << endl;

	// RCPT TO 第一个收件人
	sprintf_s(buf, 1500, "RCPT TO:<%s>\r\n", email);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Tell Sendto Receive: " << rbuf << endl;

	// DATA 准备开始发送邮件内容
	sprintf_s(buf, 1500, "DATA\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Send Mail Prepare Receive: " << rbuf << endl;

	// 发送邮件内容，\r\n.\r\n内容结束标记
	sprintf_s(buf, 5096, "%s\r\n", body);
	send(sockfd, buf, strlen(buf), 0);
	//memset(rbuf, 0, 1500);
	//recv(sockfd, rbuf, 1500, 0);
	//cout << "Send Mail Receive: " << rbuf << endl;


	send(sockfd, filebase64, strlen(filebase64), 0);//发送邮件的内容
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Send Mail Receive: " << rbuf << endl;

	// QUIT
	sprintf_s(buf, 1500, "QUIT\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Quit Receive: " << rbuf << endl;

	//清理工作
	closesocket(sockfd);
	WSACleanup();
	return TRUE;
}

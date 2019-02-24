#include<stdio.h>
#include<stdlib.h>
#include<Winsock2.h>
#include<string.h>
#pragma comment(lib,"Ws2_32.lib")
#define PORT 2000
#define HOST "49.140.77.144"   //�����ڱ��ص�ַ
const int len = 10;   //�������г���
extern int errno;
void client(const char *host, const unsigned short port)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))   //�ɹ��򷵻�0
	{
		printf("Socket��ʼ��ʧ�ܣ�\n");
		return;
	}
	SOCKET st = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serverAddr;

	memset(&serverAddr, 0, sizeof(serverAddr));
	if (inet_addr(host) == INADDR_NONE)
	{
		struct hostent *pht;
		pht = gethostbyname(host);
		memcpy(&serverAddr.sin_addr, pht->h_addr_list[0], pht->h_length);
		serverAddr.sin_family = pht->h_addrtype;
		serverAddr.sin_port = htons(port);
	}
	else
	{
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.S_un.S_addr = inet_addr(host);
		serverAddr.sin_port = htons(port);
	}
	if (connect(st, (struct sockaddr*)&serverAddr, sizeof(serverAddr)))
	{
		printf("Socket����ʧ�ܣ�\n");
		return;
	}
	char readBuf[1024] = { 0 };
	char writeBuf[1024] = { 0 };
	sprintf(writeBuf, "Hello,Server! Tell me time.");
	if (send(st, writeBuf, strlen(writeBuf), 0))
	{
		if (recv(st, readBuf, sizeof(readBuf), 0)>0)
		{
			printf("%s", readBuf);
		}
	}
	closesocket(st);
	WSACleanup();
	return;
}
int main(int argc, char **argv)
{
	printf("** ʵ��1. Windowsƽ̨���׽��ֱ�� **\n");
	printf("** --by PZC LC LY***\n");
	printf("**************************************\n");
	if (argc >= 3)
	{
		int port = atoi(argv[2]);
		printf("HOST:%s\tPORT:%d\n", argv[1], port);
		client(argv[1], port);
	}
	else
	{
		printf("\n");
		printf("����:%s\t\n�˿ں�:%d\n", HOST, PORT);
		client(HOST, PORT);
	}
	system("pause");
	return 0;
}

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<Winsock2.h>
#include<string.h>
#pragma comment(lib,"Ws2_32.lib")  //Ԥ����ı�ʶ��,Ϊlib�ṩ��Ϣ���ַ���
#define PORT 2000
const int len = 10;   //�������г���
void server(const unsigned short port)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))  //�ɹ��򷵻�0
	{
		printf("Socket��ʼ��ʧ�ܣ�\n");
		return;
	}
	SOCKET st = socket(AF_INET, SOCK_STREAM, 0);   //����Socket
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(st, (struct sockaddr*)&serverAddr, sizeof(serverAddr)))  //�ж�bind
	{
		printf("Step1:\n");
		printf("״̬��Socket��ʧ�ܣ�\n");
		return;
	}
	printf("\n");
	printf("Step1:\n");
	printf("״̬��Socket�󶨳ɹ���\n");
	if (listen(st, len))    //�ж�����
	{
		printf("Step2:\n");
		printf("״̬��Socket����ʧ�ܣ�\n");
		return;
	}
	printf("\n");
	printf("**************************************\n");
	printf("Step2:\n");
	printf("״̬��Socket�����ɹ���\n");
	printf("\n");
	printf("�ӿͻ��˷�������Ϣ:\n");
	while (1)
	{
		static char readBuf[1024];
		static char writeBuf[1024];
		SOCKET clientSock = accept(st, 0, 0);
		memset(readBuf, 0, sizeof(readBuf));
		if (recv(clientSock, readBuf, sizeof(readBuf), 0)>0)
		{
			printf("%s\n", readBuf);
			memset(writeBuf, 0, sizeof(writeBuf));
			time_t t;
			struct tm *tblock;
			t = time(NULL);
			tblock = localtime(&t);
            printf("%s\n", asctime(tblock));
			sprintf(writeBuf, "%s\n", asctime(tblock));
			send(clientSock, writeBuf, strlen(writeBuf), 0);
		}
		closesocket(clientSock);
	}
	WSACleanup();
	return;
}

int main(int argc, char **argv)
{
	printf("**************************************\n");
	printf("** ʵ��1. Windowsƽ̨���׽��ֱ�� **\n");
	printf("**--By PZC LC LY***\n");
	printf("**************************************\n");
	if (argc == 1)
	{
		server(PORT);
	}
	else
	{
		server(atoi(argv[1]));
	}
	system("pause");
	return 0;
}




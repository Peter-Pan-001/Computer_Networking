#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<Winsock2.h>
#include<string.h>
#pragma comment(lib,"Ws2_32.lib")  //预定义的标识符,为lib提供信息的字符串
#define PORT 2000
const int len = 10;   //监听队列长度
void server(const unsigned short port)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))  //成功则返回0
	{
		printf("Socket初始化失败！\n");
		return;
	}
	SOCKET st = socket(AF_INET, SOCK_STREAM, 0);   //定义Socket
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(st, (struct sockaddr*)&serverAddr, sizeof(serverAddr)))  //判定bind
	{
		printf("Step1:\n");
		printf("状态：Socket绑定失败！\n");
		return;
	}
	printf("\n");
	printf("Step1:\n");
	printf("状态：Socket绑定成功！\n");
	if (listen(st, len))    //判定监听
	{
		printf("Step2:\n");
		printf("状态：Socket监听失败！\n");
		return;
	}
	printf("\n");
	printf("**************************************\n");
	printf("Step2:\n");
	printf("状态：Socket监听成功！\n");
	printf("\n");
	printf("从客户端发来的信息:\n");
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
	printf("** 实验1. Windows平台简单套接字编程 **\n");
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




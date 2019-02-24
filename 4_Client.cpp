#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <WinSock2.h> 
#include <direct.h>

#define PORT 2000 
#define SERVER_IP "49.140.77.143" 
#define BUFFER_MAX 1024 
#define FILE_NAME_MAX 512 
#pragma comment(lib, "WS2_32") 

int main()
{
	printf("初始化客户端...\n");
	// 初始化WSA
	WSADATA wsaData;
	WORD socketVersion = MAKEWORD(2, 0);
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		printf("初始化WinSock服务失败!\n");
		system("pause");
		exit(-1);
	}

	//创建socket 
	SOCKET client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == client_fd)
	{
		printf("初始化SOCKET失败!\n");
		system("pause");
		exit(-1);
	}

	//配置服务器地址 
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	server_addr.sin_port = htons(PORT);

	printf("连接服务器...\n");
	if (SOCKET_ERROR == connect(client_fd, (LPSOCKADDR)&server_addr, sizeof(server_addr)))//尝试连接服务器
	{
		printf("无法连接指定IP的服务器!\n");
		system("pause");
		exit(1);
	}
	printf("已连接!\n");

	//输入文件名 
	char file_name[FILE_NAME_MAX + 1];
	memset(file_name, 0, FILE_NAME_MAX + 1);
	printf("\n**************************************\n");
	printf("请输入要请求的文件名: ");
	scanf("%s", &file_name);

	char buffer[BUFFER_MAX];
	memset(buffer, 0, BUFFER_MAX);
	strncpy(buffer, file_name, strlen(file_name) > BUFFER_MAX ? BUFFER_MAX : strlen(file_name));

	printf("正在向服务器发送请求...\n");
	if (send(client_fd, buffer, BUFFER_MAX, 0) < 0)//向服务器发送文件名 
	{
		printf("文件名发送失败！\n");
		system("pause");
		exit(1);
	}

	printf("正在接收...\n");
	FILE * fp = fopen(file_name, "wb"); //新建一个文件指针，以二进制写的形式打开该文件 
	if (NULL == fp)
	{
		printf("文件 %s 无法被写入本地!\n", file_name);
		system("pause");
		exit(1);
	}
	else
	{
		memset(buffer, 0, BUFFER_MAX);
		int file_length = 0;
		int file_recv = 0;
		while ((file_length = recv(client_fd, buffer, BUFFER_MAX, 0)) > 0)
		{
			if (fwrite(buffer, sizeof(char), file_length, fp) < file_length)
			{
				printf("文件 %s 写入失败!\n", file_name);
				break;
			}
			memset(buffer, 0, BUFFER_MAX);
			file_recv += file_length;
		}

		printf("文件 %s 接收完毕!\n", file_name);
		printf("共接收 %d 字节!\n", file_recv);
		char path[_MAX_PATH];
		_getcwd(path, _MAX_PATH);
		printf("文件保存在:\n	%s\n", path);

	}

	fclose(fp);//关闭文件指针
	closesocket(client_fd);

	//卸载WSA
	WSACleanup();

	system("pause");
	return 0;
}
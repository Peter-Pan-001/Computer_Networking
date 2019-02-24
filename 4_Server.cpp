#include<time.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <direct.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#define PORT 9988 
#define SERVER_IP "127.0.0.1" 
#define BUFFER_MAX 1024
#define FILE_NAME_MAX 512 
int main()
{
	SOCKET listen_fd, connect_fd, max_fd;
	fd_set rdfs, tempfs;
	int num, length;
	char  LocalName[16];
	char buf[BUFFER_MAX];
	struct hostent *pHost;
	struct sockaddr_in serveraddr, clientaddr;

	printf("部署服务器...\n");
	//初始化WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		printf("初始化WinSock服务失败!\n");
		system("pause");
		exit(-1);
	}
	//创建套接字
	listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_fd == INVALID_SOCKET)
	{
		printf("初始化监听SOCKET失败!\n");
		system("pause");
		exit(-1);
	}
	// 获取本机名 
	if (gethostname((char*)LocalName, sizeof(LocalName)) == SOCKET_ERROR)
	{
		printf("gethostname ERROR: %d\n", WSAGetLastError());
		system("pause");
		exit(-1);
	}
	if ((pHost = gethostbyname((char*)LocalName)) == NULL)//获取本地 IP 地址 
	{
		printf("gethostbyname ERROR: %d\n", WSAGetLastError());
		system("pause");
		exit(-1);
	}
	//配置服务器socket地址
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT);
	serveraddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	//serveraddr.sin_addr.S_un.S_addr = INADDR_ANY;
	//serveraddr.sin_addr = *(in_addr *)pHost->h_addr_list[0]; // IP 

	if (bind(listen_fd, (SOCKADDR *)&serveraddr, sizeof(serveraddr)) == -1)//绑定服务器地址
	{
		printf("bind ERROR: %d\n", WSAGetLastError());
		system("pause");
		exit(-1);
	}

	if (listen(listen_fd, 10) == -1)//启动监听描述符
	{
		printf("listen ERROR: %d\n", WSAGetLastError());
		system("pause");
		exit(-1);
	}

	printf("部署完毕...\n");
	//printf("服务器主机名:%s\n", LocalName);
	printf("服务器IP地址:%s\n", inet_ntoa(serveraddr.sin_addr));

	char path[_MAX_PATH];
	_getcwd(path, _MAX_PATH);
	printf("服务器当前目录:\n	%s\n", path);

	printf("等待连接请求...\n");


	memset(buf, '\0', BUFFER_MAX);//初始化缓存区
	FD_ZERO(&rdfs);//初始化描述符集合
	FD_SET(listen_fd, &rdfs);//将监听Socket描述符添加到描述符集合中
	max_fd = listen_fd;//初始化最大描述符记录，实际在Windows系统中没有意义，仅为兼容操作
	int i;//文件描述符
	//int count = 0;//遍历计数

	while (1)
	{
		//定义选择超时间隔
		timeval tv;
		tv.tv_sec = 3;
		tv.tv_usec = 0;

		tempfs = rdfs;//临时描述符集合

		//多路复用I/O实现多客户端功能
		if ((num = select(max_fd + 1, &tempfs, NULL, NULL, &tv)) == -1)
		{
			printf("Select Failed!选择失败:\n", WSAGetLastError());
			system("pause");
			exit(-1);
		}
	

		for (i = 0; i < max_fd + 1; i++)//遍历选择过滤后的文件描述符集合
		{
			if (FD_ISSET(i, &tempfs))//连接已就绪（socket描述符未被清零）
			{

				if (i == listen_fd)//倾听套接字就绪，说明一个新的连接请求建立 
				{
					length = sizeof(clientaddr);
					if ((connect_fd = accept(listen_fd, (SOCKADDR *)&clientaddr, &length)) == INVALID_SOCKET)
					{
						printf("accept ERROR: %d\n", WSAGetLastError());
						system("pause");
						exit(-1);
					}
					printf("\n*************************************\n");
					printf("新的Socket连接建立中...\n");
					printf("\n*************************************\n");
					printf("新的Socket已连接!\n");
					printf("来自IP: %s port: %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

					FD_SET(connect_fd, &rdfs);//将新建立的socket的描述符添加到描述符集合中

					if (max_fd < connect_fd)//更新最大描述符记录，实际在Windows系统中没有意义，仅为兼容操作
					{
						max_fd = connect_fd;
					}

				}
				else
				{
					memset(buf, 0, BUFFER_MAX);//清空缓存区
					length = sizeof(clientaddr);
					int temp = recvfrom(i, buf, BUFFER_MAX, 0, (SOCKADDR *)&clientaddr, &length);//获取recv操作结果（客户端请求的文件名）
					if (temp > 0){//recv成功，将客户所请求的文件名缓存到buf中 （短时间内多次send可能会发生粘包）

						char file_name[FILE_NAME_MAX + 1];//保存文件名的字符串
						memset(file_name, 0, FILE_NAME_MAX + 1);//初始化文件名字符串
						strncpy(file_name, buf, strlen(buf) > FILE_NAME_MAX ? FILE_NAME_MAX : strlen(buf));//从缓存中读取文件名

						printf("\n*************************************\n");
						printf("来自IP: %s port: %d；\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
						printf("\n*************************************\n");
						printf("请求文件:%s；\n", file_name);

						char path[_MAX_PATH];
						if ((_chdir("d:\\testftp")) != 0)
						{
							printf("打开指定目录失败!\n");
							_getcwd(path, _MAX_PATH);
							printf("当前目录路径为:\n	%s\n", path);
						}

						printf("检索文件中...\n");
						FILE * fp = fopen(file_name, "rb"); //新建一个文件指针，以二进制读的形式打开该文件 
						if (NULL == fp)
						{
							printf("没有在当前目录下找到文件名为 %s 的文件！\n", file_name);
							closesocket(i);
						}
						else
						{
							printf("正在传输...\n");
							memset(buf, 0, BUFFER_MAX);//清空缓存区
							int file_length = 0;
							int file_send = 0;
							while ((file_length = fread(buf, sizeof(char), BUFFER_MAX, fp)) > 0)
							{
								if (send(i, buf, file_length, 0) < 0)
								{
									printf("传输出错，名为 %s 的文件发送失败!\n", file_name);
									break;
								}
								memset(buf, 0, BUFFER_MAX);
								file_send += file_length;
							}

							fclose(fp);//关闭文件指针
							printf("\n文件 %s 传输完毕!\n", file_name);
							printf("\n共传输 %d 字节!\n", file_send);
							closesocket(i);//关闭该socket
							FD_CLR(i, &rdfs);//将socket对应描述符从描述符集合中移除
							printf("\nSocket连接已断开!\n");

						}

					}
					else
					{//recv无法接受数据则关闭该socket连接
						closesocket(i);//关闭该socket
						FD_CLR(i, &rdfs);//将socket对应描述符从描述符集合中移除
						printf("\n*************************************\n");
						printf("来自IP: %s port : %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
						printf("\n*************************************\n");
						printf("Socket连接已断开\n");

						if (max_fd == i)//更新最大描述符记录，实际在Windows系统中没有意义，仅为兼容操作
						{
							max_fd--;
						}
					}

				}

			}
		}
	}

	//卸载WSA 
	WSACleanup();

	system("pause");
	return 0;
}
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
	printf("��ʼ���ͻ���...\n");
	// ��ʼ��WSA
	WSADATA wsaData;
	WORD socketVersion = MAKEWORD(2, 0);
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		printf("��ʼ��WinSock����ʧ��!\n");
		system("pause");
		exit(-1);
	}

	//����socket 
	SOCKET client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == client_fd)
	{
		printf("��ʼ��SOCKETʧ��!\n");
		system("pause");
		exit(-1);
	}

	//���÷�������ַ 
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	server_addr.sin_port = htons(PORT);

	printf("���ӷ�����...\n");
	if (SOCKET_ERROR == connect(client_fd, (LPSOCKADDR)&server_addr, sizeof(server_addr)))//�������ӷ�����
	{
		printf("�޷�����ָ��IP�ķ�����!\n");
		system("pause");
		exit(1);
	}
	printf("������!\n");

	//�����ļ��� 
	char file_name[FILE_NAME_MAX + 1];
	memset(file_name, 0, FILE_NAME_MAX + 1);
	printf("\n**************************************\n");
	printf("������Ҫ������ļ���: ");
	scanf("%s", &file_name);

	char buffer[BUFFER_MAX];
	memset(buffer, 0, BUFFER_MAX);
	strncpy(buffer, file_name, strlen(file_name) > BUFFER_MAX ? BUFFER_MAX : strlen(file_name));

	printf("�������������������...\n");
	if (send(client_fd, buffer, BUFFER_MAX, 0) < 0)//������������ļ��� 
	{
		printf("�ļ�������ʧ�ܣ�\n");
		system("pause");
		exit(1);
	}

	printf("���ڽ���...\n");
	FILE * fp = fopen(file_name, "wb"); //�½�һ���ļ�ָ�룬�Զ�����д����ʽ�򿪸��ļ� 
	if (NULL == fp)
	{
		printf("�ļ� %s �޷���д�뱾��!\n", file_name);
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
				printf("�ļ� %s д��ʧ��!\n", file_name);
				break;
			}
			memset(buffer, 0, BUFFER_MAX);
			file_recv += file_length;
		}

		printf("�ļ� %s �������!\n", file_name);
		printf("������ %d �ֽ�!\n", file_recv);
		char path[_MAX_PATH];
		_getcwd(path, _MAX_PATH);
		printf("�ļ�������:\n	%s\n", path);

	}

	fclose(fp);//�ر��ļ�ָ��
	closesocket(client_fd);

	//ж��WSA
	WSACleanup();

	system("pause");
	return 0;
}
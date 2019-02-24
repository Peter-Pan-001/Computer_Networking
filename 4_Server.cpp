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

	printf("���������...\n");
	//��ʼ��WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		printf("��ʼ��WinSock����ʧ��!\n");
		system("pause");
		exit(-1);
	}
	//�����׽���
	listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_fd == INVALID_SOCKET)
	{
		printf("��ʼ������SOCKETʧ��!\n");
		system("pause");
		exit(-1);
	}
	// ��ȡ������ 
	if (gethostname((char*)LocalName, sizeof(LocalName)) == SOCKET_ERROR)
	{
		printf("gethostname ERROR: %d\n", WSAGetLastError());
		system("pause");
		exit(-1);
	}
	if ((pHost = gethostbyname((char*)LocalName)) == NULL)//��ȡ���� IP ��ַ 
	{
		printf("gethostbyname ERROR: %d\n", WSAGetLastError());
		system("pause");
		exit(-1);
	}
	//���÷�����socket��ַ
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT);
	serveraddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	//serveraddr.sin_addr.S_un.S_addr = INADDR_ANY;
	//serveraddr.sin_addr = *(in_addr *)pHost->h_addr_list[0]; // IP 

	if (bind(listen_fd, (SOCKADDR *)&serveraddr, sizeof(serveraddr)) == -1)//�󶨷�������ַ
	{
		printf("bind ERROR: %d\n", WSAGetLastError());
		system("pause");
		exit(-1);
	}

	if (listen(listen_fd, 10) == -1)//��������������
	{
		printf("listen ERROR: %d\n", WSAGetLastError());
		system("pause");
		exit(-1);
	}

	printf("�������...\n");
	//printf("������������:%s\n", LocalName);
	printf("������IP��ַ:%s\n", inet_ntoa(serveraddr.sin_addr));

	char path[_MAX_PATH];
	_getcwd(path, _MAX_PATH);
	printf("��������ǰĿ¼:\n	%s\n", path);

	printf("�ȴ���������...\n");


	memset(buf, '\0', BUFFER_MAX);//��ʼ��������
	FD_ZERO(&rdfs);//��ʼ������������
	FD_SET(listen_fd, &rdfs);//������Socket��������ӵ�������������
	max_fd = listen_fd;//��ʼ�������������¼��ʵ����Windowsϵͳ��û�����壬��Ϊ���ݲ���
	int i;//�ļ�������
	//int count = 0;//��������

	while (1)
	{
		//����ѡ��ʱ���
		timeval tv;
		tv.tv_sec = 3;
		tv.tv_usec = 0;

		tempfs = rdfs;//��ʱ����������

		//��·����I/Oʵ�ֶ�ͻ��˹���
		if ((num = select(max_fd + 1, &tempfs, NULL, NULL, &tv)) == -1)
		{
			printf("Select Failed!ѡ��ʧ��:\n", WSAGetLastError());
			system("pause");
			exit(-1);
		}
	

		for (i = 0; i < max_fd + 1; i++)//����ѡ����˺���ļ�����������
		{
			if (FD_ISSET(i, &tempfs))//�����Ѿ�����socket������δ�����㣩
			{

				if (i == listen_fd)//�����׽��־�����˵��һ���µ����������� 
				{
					length = sizeof(clientaddr);
					if ((connect_fd = accept(listen_fd, (SOCKADDR *)&clientaddr, &length)) == INVALID_SOCKET)
					{
						printf("accept ERROR: %d\n", WSAGetLastError());
						system("pause");
						exit(-1);
					}
					printf("\n*************************************\n");
					printf("�µ�Socket���ӽ�����...\n");
					printf("\n*************************************\n");
					printf("�µ�Socket������!\n");
					printf("����IP: %s port: %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

					FD_SET(connect_fd, &rdfs);//���½�����socket����������ӵ�������������

					if (max_fd < connect_fd)//���������������¼��ʵ����Windowsϵͳ��û�����壬��Ϊ���ݲ���
					{
						max_fd = connect_fd;
					}

				}
				else
				{
					memset(buf, 0, BUFFER_MAX);//��ջ�����
					length = sizeof(clientaddr);
					int temp = recvfrom(i, buf, BUFFER_MAX, 0, (SOCKADDR *)&clientaddr, &length);//��ȡrecv����������ͻ���������ļ�����
					if (temp > 0){//recv�ɹ������ͻ���������ļ������浽buf�� ����ʱ���ڶ��send���ܻᷢ��ճ����

						char file_name[FILE_NAME_MAX + 1];//�����ļ������ַ���
						memset(file_name, 0, FILE_NAME_MAX + 1);//��ʼ���ļ����ַ���
						strncpy(file_name, buf, strlen(buf) > FILE_NAME_MAX ? FILE_NAME_MAX : strlen(buf));//�ӻ����ж�ȡ�ļ���

						printf("\n*************************************\n");
						printf("����IP: %s port: %d��\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
						printf("\n*************************************\n");
						printf("�����ļ�:%s��\n", file_name);

						char path[_MAX_PATH];
						if ((_chdir("d:\\testftp")) != 0)
						{
							printf("��ָ��Ŀ¼ʧ��!\n");
							_getcwd(path, _MAX_PATH);
							printf("��ǰĿ¼·��Ϊ:\n	%s\n", path);
						}

						printf("�����ļ���...\n");
						FILE * fp = fopen(file_name, "rb"); //�½�һ���ļ�ָ�룬�Զ����ƶ�����ʽ�򿪸��ļ� 
						if (NULL == fp)
						{
							printf("û���ڵ�ǰĿ¼���ҵ��ļ���Ϊ %s ���ļ���\n", file_name);
							closesocket(i);
						}
						else
						{
							printf("���ڴ���...\n");
							memset(buf, 0, BUFFER_MAX);//��ջ�����
							int file_length = 0;
							int file_send = 0;
							while ((file_length = fread(buf, sizeof(char), BUFFER_MAX, fp)) > 0)
							{
								if (send(i, buf, file_length, 0) < 0)
								{
									printf("���������Ϊ %s ���ļ�����ʧ��!\n", file_name);
									break;
								}
								memset(buf, 0, BUFFER_MAX);
								file_send += file_length;
							}

							fclose(fp);//�ر��ļ�ָ��
							printf("\n�ļ� %s �������!\n", file_name);
							printf("\n������ %d �ֽ�!\n", file_send);
							closesocket(i);//�رո�socket
							FD_CLR(i, &rdfs);//��socket��Ӧ���������������������Ƴ�
							printf("\nSocket�����ѶϿ�!\n");

						}

					}
					else
					{//recv�޷�����������رո�socket����
						closesocket(i);//�رո�socket
						FD_CLR(i, &rdfs);//��socket��Ӧ���������������������Ƴ�
						printf("\n*************************************\n");
						printf("����IP: %s port : %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
						printf("\n*************************************\n");
						printf("Socket�����ѶϿ�\n");

						if (max_fd == i)//���������������¼��ʵ����Windowsϵͳ��û�����壬��Ϊ���ݲ���
						{
							max_fd--;
						}
					}

				}

			}
		}
	}

	//ж��WSA 
	WSACleanup();

	system("pause");
	return 0;
}
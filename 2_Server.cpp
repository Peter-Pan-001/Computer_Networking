#include "stdafx.h"
#pragma comment(lib,"Ws2_32.lib")
#include <iostream>
using namespace std;
#include <Winsock2.h>
#include <Windows.h>
#include <time.h>
#define  PORT 2000
#define  IP_ADDRESS "49.140.77.144"

DWORD WINAPI ClientThread(LPVOID lpParameter)       //����ÿ���ͻ������ӵĴ�����
{
	SOCKET CientSocket = (SOCKET)lpParameter;
	int Ret = 0;
	char RecvBuffer[MAX_PATH];  //��VC++6.0�У� MAX_PATH��ֵΪ260��
	time_t t = time( 0 ); 
	char tmp[64]; 
	strftime( tmp, sizeof(tmp), "%Y/%m/%d %X %A �����%j�� %z",localtime(&t) );
	Ret=send(CientSocket, tmp, (int)strlen(tmp), 0);
	if ( Ret == SOCKET_ERROR )
	{
		cout<<"Send Info Error"<<endl;
		
	}
	while ( true )
	{
		memset(RecvBuffer, 0x00, sizeof(RecvBuffer));  //Ϊ��������ڴ�����ʼ�������� �䷵��ֵΪָ��RecvBuffer��ָ�롣
		Ret = recv(CientSocket, RecvBuffer, MAX_PATH, 0);
		if ( Ret == 0 || Ret == SOCKET_ERROR ) 
		{
			cout<<"�ͻ����˳�!"<<endl;
			closesocket(CientSocket);//�رտͻ����׽��� 
			break;
		}
		cout<<"���յ��ͻ���ϢΪ:"<<RecvBuffer<<endl;
	}
	
	return 0;
}

int main(int argc, char* argv[])
{
	WSADATA  Ws;
	SOCKET ServerSocket, CientSocket;
	struct sockaddr_in LocalAddr, ClientAddr;
	int Ret = 0;
	int AddrLen = 0;
	HANDLE hThread = NULL;
	
	
	//Init Windows Socket
	if ( WSAStartup(MAKEWORD(2,2), &Ws) != 0 )
	{
		cout<<"Init Windows Socket Failed::"<<GetLastError()<<endl;
		return -1;
	}
	
	//Create Socket
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if ( ServerSocket == INVALID_SOCKET )
	{
		cout<<"Create Socket Failed::"<<GetLastError()<<endl;
		return -1;
	}
	
	LocalAddr.sin_family = AF_INET;
	LocalAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
	LocalAddr.sin_port = htons(PORT);
	memset(LocalAddr.sin_zero, 0x00, 8);
	
	//Bind Socket
	Ret = bind(ServerSocket, (struct sockaddr*)&LocalAddr, sizeof(LocalAddr));
	if ( Ret != 0 )
	{
		cout<<"Bind Socket Failed::"<<GetLastError()<<endl;
		return -1;
	}
	
	Ret = listen(ServerSocket, 10);
	if ( Ret != 0 )
	{
		cout<<"listen Socket Failed::"<<GetLastError()<<endl;
		return -1;
	}
	
	cout<<"������Ѿ�����"<<endl;
	
	while ( true )
	{
		AddrLen = sizeof(ClientAddr);
		CientSocket = accept(ServerSocket, (struct sockaddr*)&ClientAddr, &AddrLen);
		if ( CientSocket == INVALID_SOCKET )
		{
			cout<<"Accept Failed::"<<GetLastError()<<endl;
			break;
		}
		
		cout<<"�ͻ�������::"<<inet_ntoa(ClientAddr.sin_addr)<<":"<<ClientAddr.sin_port<<endl;
		
		hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)CientSocket, 0, NULL);
		/*lpsa���߳̾���İ�ȫ���ԣ������ӽ����Ƿ���Լ̳�����߳̾����һ���������ΪNULL
		cbStack���߳�ջ��С��һ��ȡ0��ʾĬ�ϴ�С
		lpStartAddr���߳���ں���   
		lpvThreadParam�������߳���ں����Ĳ���������ThreadProc( void* lpParamete) �Ĳ���
		fdwCreate�������̴߳����ı�־һ��Ϊ0����ʾ�߳�����������������봴��֮����̹߳��������Դ���CREATE_SUSPENDED �����������������Ҫ���ʵ��ĵط�����ResumeThread �����߳�
		lpIDThread�����߳�ID����ֵ��������������̷߳��ص�ID*/
				if ( hThread == NULL )
		{
			cout<<"Create Thread Failed!"<<endl;
			break;
		}
		
		CloseHandle(hThread);
	}
	
	closesocket(ServerSocket);
	closesocket(CientSocket);
	WSACleanup();
	
	return 0;
}

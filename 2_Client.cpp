#include "stdafx.h"
#pragma comment(lib,"Ws2_32.lib")
#include <iostream>
using namespace std;
#include <Winsock2.h>
#include <Windows.h>
#include <time.h>
#define  PORT 2000
#define  IP_ADDRESS "49.140.77.144"


int main(int argc, char* argv[])
{
	WSADATA  Ws;     //存储被WSAStartup函数调用后返回值
	SOCKET CientSocket;
	struct sockaddr_in ServerAddr;
	int Ret = 0;
	int AddrLen = 0;
	HANDLE hThread = NULL;
	char SendBuffer[MAX_PATH];
	
	//Init Windows Socket
	if ( WSAStartup(MAKEWORD(2,2), &Ws) != 0 )
	{
        cout<<"Init Windows Socket Failed::"<<GetLastError()<<endl;
		return -1;
	}
	
	//Create Socket
	CientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if ( CientSocket == INVALID_SOCKET )
	{
		cout<<"Create Socket Failed::"<<GetLastError()<<endl;
		return -1;
	}
	
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
	ServerAddr.sin_port = htons(PORT);
	memset(ServerAddr.sin_zero, 0x00, 8);
	
	Ret = connect(CientSocket,(struct sockaddr*)&ServerAddr, sizeof(ServerAddr));
	if ( Ret == SOCKET_ERROR )
	{
		cout<<"Connect Error::"<<GetLastError()<<endl;
		return -1;
	}
	else
	{
		cout<<"连接成功!"<<endl;
	}
	char recvbuf[100];
	memset(recvbuf, 0x00, sizeof(recvbuf));
	 Ret = recv(CientSocket, recvbuf, 100, 0);
          if ( Ret == 0 || Ret == SOCKET_ERROR ) 
          {
              cout<<"接受失败!"<<endl;
			  
              
          }
		  cout<<" 接收到的服务器端时间:   "<<recvbuf<<endl;
	while ( true )
	{
		cin.getline(SendBuffer, sizeof(SendBuffer));  //接受一个字符串，可以接收空格并输出
		Ret = send(CientSocket, SendBuffer, (int)strlen(SendBuffer), 0);
		if ( Ret == SOCKET_ERROR )
		{
			cout<<"Send Info Error::"<<GetLastError()<<endl;
			break;
		}
	}
	
	closesocket(CientSocket);
	WSACleanup();
	
	return 0;

}

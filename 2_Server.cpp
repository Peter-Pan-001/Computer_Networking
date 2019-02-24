#include "stdafx.h"
#pragma comment(lib,"Ws2_32.lib")
#include <iostream>
using namespace std;
#include <Winsock2.h>
#include <Windows.h>
#include <time.h>
#define  PORT 2000
#define  IP_ADDRESS "49.140.77.144"

DWORD WINAPI ClientThread(LPVOID lpParameter)       //接收每个客户端连接的处理函数
{
	SOCKET CientSocket = (SOCKET)lpParameter;
	int Ret = 0;
	char RecvBuffer[MAX_PATH];  //在VC++6.0中， MAX_PATH的值为260。
	time_t t = time( 0 ); 
	char tmp[64]; 
	strftime( tmp, sizeof(tmp), "%Y/%m/%d %X %A 本年第%j天 %z",localtime(&t) );
	Ret=send(CientSocket, tmp, (int)strlen(tmp), 0);
	if ( Ret == SOCKET_ERROR )
	{
		cout<<"Send Info Error"<<endl;
		
	}
	while ( true )
	{
		memset(RecvBuffer, 0x00, sizeof(RecvBuffer));  //为新申请的内存做初始化工作， 其返回值为指向RecvBuffer的指针。
		Ret = recv(CientSocket, RecvBuffer, MAX_PATH, 0);
		if ( Ret == 0 || Ret == SOCKET_ERROR ) 
		{
			cout<<"客户端退出!"<<endl;
			closesocket(CientSocket);//关闭客户端套接字 
			break;
		}
		cout<<"接收到客户信息为:"<<RecvBuffer<<endl;
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
	
	cout<<"服务端已经启动"<<endl;
	
	while ( true )
	{
		AddrLen = sizeof(ClientAddr);
		CientSocket = accept(ServerSocket, (struct sockaddr*)&ClientAddr, &AddrLen);
		if ( CientSocket == INVALID_SOCKET )
		{
			cout<<"Accept Failed::"<<GetLastError()<<endl;
			break;
		}
		
		cout<<"客户端连接::"<<inet_ntoa(ClientAddr.sin_addr)<<":"<<ClientAddr.sin_port<<endl;
		
		hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)CientSocket, 0, NULL);
		/*lpsa：线程句柄的安全属性，比如子进程是否可以继承这个线程句柄，一般情况设置为NULL
		cbStack：线程栈大小，一般取0表示默认大小
		lpStartAddr：线程入口函数   
		lpvThreadParam：就是线程入口函数的参数，就是ThreadProc( void* lpParamete) 的参数
		fdwCreate：控制线程创建的标志一般为0，表示线程立即启动。如果你想创建之后把线程挂起来可以传入CREATE_SUSPENDED ，传入这个参数你需要再适当的地方调用ResumeThread 启动线程
		lpIDThread：是线程ID返回值，这个用来接收线程返回的ID*/
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

#include<stdio.h>
#include<Winsock2.h>
#pragma comment(lib,"ws2_32")
#define SIO_RCVALL _WSAIOW(IOC_VENDOR,1)
struct ip // 定义IP首部 
{ 
	 unsigned char h_verlen;   // 4位首部长度,4位IP版本号 
	 unsigned char tos;    // 8位服务类型TOS 
	 unsigned short ip_length;       // 16位总长度（字节） 
	 unsigned short ident;      // 16位标识 
	 unsigned short frag_and_flags;  // 3位标志位 
	 unsigned char ttl;       // 8位生存时间 TTL 
	 unsigned char proto;      // 8位协议 (TCP, UDP 或其他) 
	 unsigned short checksum;  // 16位IP首部校验和 
	 unsigned int sourceIP;   // 32位源IP地址 
	 unsigned int destIP;   // 32位目的IP地址 
};


// 定义TCP首部 
 struct tcp
{ 
	 USHORT th_sport;     // 16位源端口 
	 USHORT th_dport;     // 16位目的端口 
	 unsigned int th_seq;    // 32位序列号 
	 unsigned int th_ack;    // 32位确认号 
	 unsigned char th_lenres;   // 4位首部长度/6位保留字 
	 unsigned char th_flag;    // 6位标志位 
	 USHORT th_win;      // 16位窗口大小 
	 USHORT th_sum;      // 16位校验和 
	 USHORT th_urp;      // 16位紧急数据偏移量 
};

void main()
{
	int sock,bytes_recieved,fromlen;
	char buffer[65535];
	struct sockaddr_in from;
	struct ip *ip;
	struct tcp *tcp;
	WORD wVersionRequested;             //版本号
	WSADATA wsaData;                       //启动SOCKET的
	int err;
	wVersionRequested = MAKEWORD( 2, 2 );//建立版本
	err = WSAStartup( wVersionRequested,&wsaData );//启用socket
	if ( err != 0 ) //如果返回值不等于0，那么表示出错，直截退出程序
	{
		return;
	}
	sock=socket(AF_INET,SOCK_RAW,IPPROTO_IP);
	bool flag=true;
	setsockopt(sock, IPPROTO_IP, 2 , (char*)&flag, sizeof(flag));
	sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");     //IP设置
	addr.sin_port=htons(0);
	if(SOCKET_ERROR==bind(sock,(sockaddr *)&addr,sizeof(addr)))       //绑定
    {
         closesocket(sock);
         printf("绑定失败！");
         exit(0);
    }
	DWORD dwBytesRet;
	DWORD dwVal=1;
	ioctlsocket(sock, SIO_RCVALL, &dwVal);           //设置网卡为混听模式
	int i=0; 
	while(true)
	{
		fromlen=sizeof(from);
		bytes_recieved=recvfrom(sock,buffer,sizeof(buffer),0,(struct sockaddr*)&from,&fromlen); //接收数据
		i++;
		ip=(struct ip *)buffer;       //得到ip头
		
		if(ip->proto==6)                                            //过滤其他协议，只留下TCP协议
		{
			tcp=(struct tcp *)(buffer+(4*ip->h_verlen&0xf0>>4));        //得到tcp头
			printf("Ip包字节数：%d\n",bytes_recieved);               //打印ip数据包长度
			printf("源IP：%s\n", inet_ntoa(*(in_addr*)&ip->sourceIP));         //打印源IP
			printf("目的IP：%s\n", inet_ntoa(*(in_addr*)&ip->destIP));         //打印目的IP
			printf("源端口：%d\n",ntohs(tcp->th_sport));                       //打印源端口
			printf("目的端口：%d\n",ntohs(tcp->th_dport));                    //打印目的端口
			printf("TCP的数据内容：");
			char* ptr=buffer+5+ 4*((tcp->th_lenres&0xf0)>>4|0);              //计算数据头指针
			int cpysize=bytes_recieved-5- 4*((tcp->th_lenres&0xf0)>>4|0);    //计算数据长度
			memcpy(buffer, ptr, cpysize);                                   //取出数据
			for(int i = 0; i < cpysize ; i++)                               //打印数据 
			{
				if(buffer[i] >= 32 && buffer[i] < 255)
				{
					printf("%c", (unsigned char)buffer[i]); 
				}
				else
				{
				 printf("."); 
				}
			}
			printf("\n");
		}
	}
}

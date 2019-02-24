#include<stdio.h>
#include<Winsock2.h>
#pragma comment(lib,"ws2_32")
#define SIO_RCVALL _WSAIOW(IOC_VENDOR,1)
struct ip // ����IP�ײ� 
{ 
	 unsigned char h_verlen;   // 4λ�ײ�����,4λIP�汾�� 
	 unsigned char tos;    // 8λ��������TOS 
	 unsigned short ip_length;       // 16λ�ܳ��ȣ��ֽڣ� 
	 unsigned short ident;      // 16λ��ʶ 
	 unsigned short frag_and_flags;  // 3λ��־λ 
	 unsigned char ttl;       // 8λ����ʱ�� TTL 
	 unsigned char proto;      // 8λЭ�� (TCP, UDP ������) 
	 unsigned short checksum;  // 16λIP�ײ�У��� 
	 unsigned int sourceIP;   // 32λԴIP��ַ 
	 unsigned int destIP;   // 32λĿ��IP��ַ 
};


// ����TCP�ײ� 
 struct tcp
{ 
	 USHORT th_sport;     // 16λԴ�˿� 
	 USHORT th_dport;     // 16λĿ�Ķ˿� 
	 unsigned int th_seq;    // 32λ���к� 
	 unsigned int th_ack;    // 32λȷ�Ϻ� 
	 unsigned char th_lenres;   // 4λ�ײ�����/6λ������ 
	 unsigned char th_flag;    // 6λ��־λ 
	 USHORT th_win;      // 16λ���ڴ�С 
	 USHORT th_sum;      // 16λУ��� 
	 USHORT th_urp;      // 16λ��������ƫ���� 
};

void main()
{
	int sock,bytes_recieved,fromlen;
	char buffer[65535];
	struct sockaddr_in from;
	struct ip *ip;
	struct tcp *tcp;
	WORD wVersionRequested;             //�汾��
	WSADATA wsaData;                       //����SOCKET��
	int err;
	wVersionRequested = MAKEWORD( 2, 2 );//�����汾
	err = WSAStartup( wVersionRequested,&wsaData );//����socket
	if ( err != 0 ) //�������ֵ������0����ô��ʾ����ֱ���˳�����
	{
		return;
	}
	sock=socket(AF_INET,SOCK_RAW,IPPROTO_IP);
	bool flag=true;
	setsockopt(sock, IPPROTO_IP, 2 , (char*)&flag, sizeof(flag));
	sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");     //IP����
	addr.sin_port=htons(0);
	if(SOCKET_ERROR==bind(sock,(sockaddr *)&addr,sizeof(addr)))       //��
    {
         closesocket(sock);
         printf("��ʧ�ܣ�");
         exit(0);
    }
	DWORD dwBytesRet;
	DWORD dwVal=1;
	ioctlsocket(sock, SIO_RCVALL, &dwVal);           //��������Ϊ����ģʽ
	int i=0; 
	while(true)
	{
		fromlen=sizeof(from);
		bytes_recieved=recvfrom(sock,buffer,sizeof(buffer),0,(struct sockaddr*)&from,&fromlen); //��������
		i++;
		ip=(struct ip *)buffer;       //�õ�ipͷ
		
		if(ip->proto==6)                                            //��������Э�飬ֻ����TCPЭ��
		{
			tcp=(struct tcp *)(buffer+(4*ip->h_verlen&0xf0>>4));        //�õ�tcpͷ
			printf("Ip���ֽ�����%d\n",bytes_recieved);               //��ӡip���ݰ�����
			printf("ԴIP��%s\n", inet_ntoa(*(in_addr*)&ip->sourceIP));         //��ӡԴIP
			printf("Ŀ��IP��%s\n", inet_ntoa(*(in_addr*)&ip->destIP));         //��ӡĿ��IP
			printf("Դ�˿ڣ�%d\n",ntohs(tcp->th_sport));                       //��ӡԴ�˿�
			printf("Ŀ�Ķ˿ڣ�%d\n",ntohs(tcp->th_dport));                    //��ӡĿ�Ķ˿�
			printf("TCP���������ݣ�");
			char* ptr=buffer+5+ 4*((tcp->th_lenres&0xf0)>>4|0);              //��������ͷָ��
			int cpysize=bytes_recieved-5- 4*((tcp->th_lenres&0xf0)>>4|0);    //�������ݳ���
			memcpy(buffer, ptr, cpysize);                                   //ȡ������
			for(int i = 0; i < cpysize ; i++)                               //��ӡ���� 
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

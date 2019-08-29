#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "struct.h"

int main()
{
	printf("服务器创建socket...\n");
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(0 > sockfd)
	{
		perror("socket");
		return -1;
	}

	printf("准备地址...\n");
	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(7777);
	addr.sin_addr.s_addr = inet_addr("192.168.43.183");
	socklen_t len = sizeof(addr);

	printf("绑定socket与地址...\n");
	if(bind(sockfd,(struct sockaddr*)&addr,len))
	{
		perror("bind");
		return -1;
	}

	printf("设置监听...\n");
	if(listen(sockfd,5))
	{
		perror("listen");
		return -1;
	}

	printf("等待客户端连接...\n");
	struct sockaddr_in addrcli = {};
	int clifd = accept(sockfd,(struct sockaddr*)&addrcli,&len);
	if(0 > clifd)
	{
		perror("accept");
		return -1;
	}




	struct sendto_server serverbuf;
	char recv1[1024]={};
	//int ret = recv(sockfd,&serverbuf,sizeof(sendto_server),0);
	int ret = recv(sockfd,&recv1,1024,0);
	printf("%d\n",ret);
	//printf("%s",serverbuf.name);
}



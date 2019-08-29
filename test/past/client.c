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

void send_file(int fd,int sockfd,int num)
{
	for(int i=0;i<=num;i++)
	{
		char buf[1024]={};
		int ret = read(fd,&buf,1000);
		printf("%d\n",ret);
		if (i == num)
		{
			ret = send(sockfd,&buf,strlen(buf)+1,0);
			break;
		}
		ret = send(sockfd,&buf,strlen(buf),0);
	//	printf("%d\n",ret);
	}
}



int main()
{
	struct sendto_server serverbuf;
	char buf[1024]={};
	char path[255]={};
	char name[20]={};
	struct stat fdbuf;
	printf("请输入要发送的文件的路径");
	gets(path);
	printf("请输入要发送的文件的名称");
	gets(name);
	sprintf(path,"%s/%s",path,name);
	int fd = open(path,O_RDONLY);
	if (0 > fd)
	{
		perror("open");
		return -1;
	}
	fstat(fd,&fdbuf);
	int num = (fdbuf.st_size/1000);
	strcpy(serverbuf.name,name);
	serverbuf.atime = fdbuf.st_atime;
	serverbuf.mtime = fdbuf.st_mtime;
	serverbuf.ctime = fdbuf.st_ctime;
	serverbuf.size = fdbuf.st_size;
	serverbuf.num = num;
	//memset(buf,0,4096);
	printf("%d\n",sizeof(serverbuf));
	//memcpy(buf,&serverbuf,sizeof(sendto_server));
	printf("%d\n",serverbuf.atime);	
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

	printf("绑定连接服务器...\n");
	if(connect(sockfd,(struct sockaddr*)&addr,len))
	{
		perror("bind");
		return -1;
	}

	int len_write =send(sockfd,&serverbuf,sizeof(sendto_server),0);
	read(sockfd,&buf,sizeof(buf));
	puts(buf);
	if (0 == strcmp("error",buf))
	{
		send_file(fd,sockfd,num);		
	}
	close(sockfd);
}

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

int sockfd;

void upload_file(char* name)
{
	stdin->_IO_read_ptr=stdin->_IO_read_end;
	char buf[1024]={};
	int fd = open(name,O_RDONLY);
	if (0 > fd)
	{
		perror("open");
		return ;
	}
	usleep(100);
	int ret=0;
	while(ret = read(fd,&buf,1000))
	{
		printf("%d\n",write(sockfd,&buf,ret));
	}
	usleep(100);
	strcpy(buf,"发送结束");
	write(sockfd,&buf,strlen(buf));
}

void download_file(char* name)
{
	stdin->_IO_read_ptr=stdin->_IO_read_end;
	char buf[1024]={};
	int ret=0;
	int fd =open(name,O_CREAT|O_EXCL|O_WRONLY,0644);
	while(ret =read(sockfd,&buf,1000))
	{
		if (0 == strcmp(buf,"发送结束"))
		{
			break;
		}
		printf("%d\n",ret);
		write(fd,&buf,strlen(buf));
		bzero(buf,sizeof(buf));
	}
	close(fd);
}

int main()
{
	
	printf("服务器创建socket...\n");
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(0 > sockfd)
	{
		perror("socket");
		return -1;
	}

	printf("准备地址...\n");
	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888);
	addr.sin_addr.s_addr = inet_addr("192.168.43.183");
	socklen_t len = sizeof(addr);

	printf("绑定连接服务器...\n");
	if(connect(sockfd,(struct sockaddr*)&addr,len))
	{
		perror("bind");
		return -1;
	}


	char buf[1024]={};
	char name[1024]={};
	for(;;)
	{
		printf("请输入您要输入的命令");
		gets(buf);
		send(sockfd,&buf,strlen(buf)+1,0);
		if (0 == strcmp("bye",buf))
		{
			break;
		}
		else if ('g'==buf[0] &&'e' == buf[1] && 't' == buf[2])
		{
			strcpy(name,buf+4);
			download_file(name);
		}
		else if ('p'==buf[0] &&'u' == buf[1] && 't' == buf[2])
		{
			strcpy(name,buf+4);
			upload_file(name);
		}
		read(sockfd,&buf,sizeof(buf));
		puts(buf);
		printf("--------------------------\n");
	}

	close(sockfd);
}

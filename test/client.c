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

struct sendto_server serverbuf;
int sockfd;

void command_file()
{
	stdin->_IO_read_ptr=stdin->_IO_read_end;
	char buf[1024]={};
	for(;;)
	{
		printf("请输入您要输入的命令");
		gets(buf);
		send(sockfd,&buf,strlen(buf)+1,0);
		if (0 == strcmp("exit",buf))
		{
			break;
		}
		read(sockfd,&buf,sizeof(buf));
		puts(buf);
		printf("--------------------------\n");
	}
}


void upload_file()
{
	stdin->_IO_read_ptr=stdin->_IO_read_end;
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
		return ;
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
	//printf("%d\n",sizeof(serverbuf));
	//memcpy(buf,&serverbuf,sizeof(sendto_server));
	//printf("%d\n",serverbuf.atime);	
	
	int len_write =send(sockfd,&serverbuf,sizeof(sendto_server),0);
	read(sockfd,&buf,sizeof(buf));
	puts(buf);
	if (0 == strcmp("error",buf))
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
}

void download_file()
{
	stdin->_IO_read_ptr=stdin->_IO_read_end;
	char path[1024]={};
	char name[1024]={};
	char buf[1024]={};
	printf("请输入要下载的文件的服务器路径");
	gets(path);
	printf("请输入要下载的文件的名称");
	gets(name);
	sprintf(path,"%s/%s",path,name);
	write(sockfd,&path,strlen(path)+1);
	printf("%d\n",read(sockfd,&buf,sizeof(buf)));
	puts(buf);
	if (0 == strcmp(buf,"文件不存在"))
	{
		return;
	}
	int fd = open(name,O_CREAT|O_EXCL|O_WRONLY,0644);
	if (0 > fd)
	{
		printf("文件已存在");
	}
	int ret=0;
	read(sockfd,&serverbuf,sizeof(sendto_server));
	//printf("serverbuf:%d",ret);
	printf("%d\n",serverbuf.num);
	for(int i=0;i<=serverbuf.num;i++)
	{
		ret =read(sockfd,&buf,1000);
		printf("%d\n",ret);
		if (0 == strlen(buf))
		{
			break;
		}
		write(fd,&buf,strlen(buf));
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
	addr.sin_port = htons(7777);
	addr.sin_addr.s_addr = inet_addr("192.168.43.183");
	socklen_t len = sizeof(addr);

	printf("绑定连接服务器...\n");
	if(connect(sockfd,(struct sockaddr*)&addr,len))
	{
		perror("bind");
		return -1;
	}
	printf("1、输入命令\n");
	printf("2、上传文件\n");
	printf("3、下载文件\n");
	int n=0;
	scanf("%d",&n);
	char buf[1024]={};
	if (1==n) 
	{
		strcpy(buf,"1");
		send(sockfd,&buf,strlen(buf)+1,0);
		command_file();
	}
	if (2==n) 
	{
		strcpy(buf,"2");
		send(sockfd,&buf,strlen(buf)+1,0);
		upload_file();
	}
	if (3==n) 
	{
		strcpy(buf,"3");
		send(sockfd,&buf,strlen(buf)+1,0);
		download_file();
	}
	
	close(sockfd);
}

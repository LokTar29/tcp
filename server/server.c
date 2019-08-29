#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "struct.h"
#include <pthread.h>
#include <dirent.h>
void* command_file(void* arg)
{
	int clifd = *(int*)arg;
	char buf[1024]={};
	char path[1024]={};
	strcpy(path,".");
	for(;;)
	{
		read(clifd,&buf,sizeof(buf));
		puts(buf);
		if (0 == strcmp("exit",buf))
		{
			break;
			write(clifd,&buf,strlen(buf)+1);
		}
		else if (0 == strcmp("ls",buf))
		{
			DIR *dp = opendir(path);
			int ret = chdir(path);
			printf("%d\n",ret);
			for(struct dirent* de = readdir(dp);NULL != de;de =readdir(dp))
			{
				strcat(buf,de->d_name);
				strcat(buf,"\n");
			}
			write(clifd,&buf,strlen(buf)+1);
		}
		else if ('c' == buf[0] && 'd' == buf[1])
		{
			//sprintf(path,"%s/%s",path,buf+3);
			puts(path);
			strcpy(path,buf+3);
			sprintf(buf,"进入:%s",path);
			write(clifd,&buf,strlen(buf)+1);
		}
		else if ('m' == buf[0] && 'k' == buf[1] && 'd' == buf[2] && 'i' ==buf[3]&& 'r' == buf[4])
		{
			char name[20]={};
			strcpy(name,buf+6);
			int ret = mkdir(name,0777);
			if (0 == ret)
			{
				strcpy(buf,"创建目录成功");
			}
			else
			{
				strcpy(buf,"创建目录失败");
			}
			write(clifd,&buf,strlen(buf)+1);
		}
		else
		{
			strcpy(buf,"无此命令");
			write(clifd,&buf,strlen(buf)+1);
		}
	}
}	
	

void* upload_file(void* arg)
{
	int clifd = *(int*)arg;
	char buf[1024] = {};
	struct stat fdbuf;
	struct sendto_server serverbuf;
	int ret = read(clifd,&serverbuf,sizeof(serverbuf));
	//printf("%d %d\n",ret,serverbuf.atime);
	int fd = open(serverbuf.name,O_RDONLY);
	if (0 >fd)
	{
		strcpy(buf,"error");
		fd = open(serverbuf.name,O_CREAT|O_EXCL|O_WRONLY,0644);
	}
	else 
	{
		strcpy(buf,"yes");
	}
	ret = write(clifd,&buf,strlen(buf)+1);
	//printf("%d\n",ret);
	if (0 == strcmp("error",buf))
	{
		for(int i=0;i<=serverbuf.num;i++)
		{
			ret = read(clifd,&buf,1000);
			if (0 == strlen(buf))
			{
				break;
			}
			printf("read:%d\n",ret);
			ret = write(fd,&buf,strlen(buf));
			printf("write:%d\n",ret);
		}
		close(fd);
	}
}

void* download_file(void* arg)
{
	struct sendto_server serverbuf;
	int clifd = *(int*)arg;
	char path[1024]={};
	char buf[1024]={};
	struct stat fdbuf;
	read(clifd,&path,sizeof(path));
	puts(path);
	int fd = open(path,O_RDONLY);
	if (0 >fd)
	{
		strcpy(buf,"文件不存在");
		write(clifd,&buf,strlen(buf)+1);	
		return NULL;
	}
	else
	{
		strcpy(buf,"文件已打开，准备传输");
		write(clifd,&buf,strlen(buf)+1);
	}
	usleep(100);	
	fstat(fd,&fdbuf);
	int num =(fdbuf.st_size/1000);
	serverbuf.num = num;
	int ret=0;
	write(clifd,&serverbuf,sizeof(sendto_server));
	//printf("serverbuf:%d\n",ret);
	while(ret = read(fd,&buf,1000))
	{
		printf("%d\n",write(clifd,&buf,ret));
	}
}



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
	for(;;)
	{
		struct sockaddr_in addrcli = {};
		int clifd = accept(sockfd,(struct sockaddr*)&addrcli,&len);
		if(0 > clifd)
		{
			perror("accept");
			continue;
		}
		
		char buf[1024]={};
		void *str;
		read(clifd,&buf,sizeof(buf));
		if (0 == strcmp("1",buf))
		{
			pthread_t pid1;
			pthread_create(&pid1,NULL,command_file,&clifd);
			pthread_join(pid1,&str);
		}
		if (0 == strcmp("2",buf))
		{
			pthread_t pid2;
			pthread_create(&pid2,NULL,upload_file,&clifd);
			pthread_join(pid2,&str);
		}
		if (0 == strcmp("3",buf))
		{
			pthread_t pid3;
			pthread_create(&pid3,NULL,download_file,&clifd);
			pthread_join(pid3,&str);
		}
	}
}

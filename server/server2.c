#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>


int upload_file(int clifd,char* name)
{
	char buf[1024] = {};
	struct stat fdbuf;
	int fd = open(name,O_CREAT|O_EXCL|O_WRONLY,0644);
	usleep(100);
	int ret=0;
	while(ret = read(clifd,&buf,1000))
	{
		printf("read:%d\n",ret);
		if (0 == strcmp(buf,"发送结束"))
		{
			break;
		}
		ret = write(fd,&buf,strlen(buf));
		printf("write:%d\n",ret);
		bzero(buf,sizeof(buf));
	}
	close(fd);
	return 1;	
}

int  download_file(int clifd,char* name)
{
	char buf[1024]={};
	int fd = open(name,O_RDONLY);
	if (0 >fd)
	{
		return 0;
	}
	else
	{
	}
	usleep(100);	
	int ret=0;
	while(ret = read(fd,&buf,1000))
	{
		printf("%d\n",write(clifd,&buf,ret));
	}
	usleep(100);
	strcpy(buf,"发送结束");
	ret =write(clifd,&buf,strlen(buf));
	printf("%d\n",ret);
	return 1;
}


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
		if (0 == strcmp("bye",buf))
		{
			break;
			write(clifd,&buf,strlen(buf)+1);
		}
		else if (0 == strcmp("ls",buf))
		{
			DIR *dp = opendir(path);
			int ret = chdir(path);
			printf("%d\n",ret);
			bzero(buf,sizeof(buf));
			for(struct dirent* de = readdir(dp);NULL != de;de =readdir(dp))
			{
				strcat(buf,de->d_name);
				strcat(buf,"\n");
			}
			write(clifd,&buf,strlen(buf)+1);
		}
		else if ('c' == buf[0] && 'd' == buf[1])
		{
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
		else if('g' ==buf[0] && 'e' == buf[1] && 't' == buf[2])
		{
			char name[20]={};
			strcpy(name,buf+4);
			int ret = download_file(clifd,name);
			if (1 == ret)
			{
				strcpy(buf,"下载成功");
			}
			else 
			{
				strcpy(buf,"下载失败");
			}
			write(clifd,&buf,strlen(buf)+1);
		}
		else if('p' ==buf[0] && 'u' == buf[1] && 't' == buf[2])
		{
			char name[20]={};
			strcpy(name,buf+4);
			int ret = upload_file(clifd,name);
			if (1 == ret)
			{
				strcpy(buf,"上传成功");
			}
			else 
			{
				strcpy(buf,"上传失败");
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
	addr.sin_port = htons(8888);
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
			pthread_t pid1;
			pthread_create(&pid1,NULL,command_file,&clifd);
			pthread_join(pid1,&str);
	}
}

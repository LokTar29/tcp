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

		if(0 == fork())
		{
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
	}
}

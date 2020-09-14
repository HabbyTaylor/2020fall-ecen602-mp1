#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<strings.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<errno.h>


ssize_t written(int sockfd, char sendline[]){//把从stdin读取的内容写入sockfd所指的文件内，并返回实际写入内容的大小
	ssize_t actualsize;//
	int i;
	int readsize=strlen(sendline);//传入数组的大小
	printf("%s", "length of message received: ");
	printf("%i\n", readsize-1);

	actualsize=write(sockfd, sendline, strlen(sendline));//write()返回实际写入的字节数，有错误发生时返回-1，错误代码存入errno中
	printf("%s", "length of message written: ");
	printf("%zu\n", actualsize-1);

	if(actualsize<readsize){//如果实际写入字数<接收到的字数，则把没写入的字继续写入，这里相当于双重保险
		for(i=actualsize; i<=readsize; i++){
			write(sockfd, (void*)&sendline[i], 1);
		}
	}
	return actualsize;

}

#if 0

ssize_t read_line(int newfd, void *buffer, size_t n){
	ssize_t read_bytes=0;
	size_t total_read=0;
	char *buf=NULL;
	char ch;
	buf=buffer;//buffer是空的

	while(1){
		memset(&ch, '\0', 1);//初始化ch为‘ ’	

		read_bytes=read(newfd, &ch, 1);//read()返回实际读入的字数，把newfd所指的文件传送1个字节到ch中
		
		if(ch=='\n')break;

		printf("n=%zu\n",n);
		if(read_bytes>0){
			if(total_read<n-1){
				*buf=ch;
				buf++;
				total_read++;
				printf("buf1=%s\n ", buf-1);
			}
		}

		if(read_bytes==-1){
			if(errno=EINTR)
				continue;
			else
				return -1;
		}

		if(read_bytes==0){//已经读到了文件尾端或者是无可读取的数据
			exit(1);//end of file
		}

		
	}

	*buf='\0';
	printf("buf=%s\n", buf-total_read);
	printf("total_read is : %d\n",total_read );
	return total_read+1;
}
#endif
int main(int argc, char *argv[]){
	int sockfd;
	char bufSend[100];//回声客户端
	char buffer[100];//读取服务器回传的数据？？
	struct sockaddr_in sadr;//向特定的IP和端口发起请求
	int port;
	port=atoi(argv[2]);//把port由字符串转换成数字
	sockfd=socket(PF_INET, SOCK_STREAM, 0);//创建套接字socket

	if(sockfd==-1){//sockfd=-1说明有问题，创建失败
		perror("socket");
		exit(-1);
	}
	else{//其他情况说明socket创建成功
		fputs("socket created\n", stdout);
	} 

	memset(&sadr, 0, sizeof sadr);//service address每个字节都用0填充

	sadr.sin_family=AF_INET;//使用ipv4地址
	sadr.sin_port=htons(port);//端口
	inet_pton(AF_INET, argv[2], &(sadr.sin_addr));

	if(connect(sockfd, (struct sockaddr*)&sadr, sizeof(sadr))==-1){//connet=1说明连接失败
		close(sockfd);//关闭套接字
		perror("connection failed");
		return 1;
	}
	else{//其他情况说明连接成功
		fputs("connected\n", stdout);
	}

	while(1){//让代码进入死循环，一直监听客户端的请求
		memset(bufSend, 0, 100);//数组writeline先用0填充
		memset(buffer, 0, 100);//数组read[]先用0填充
		printf("Input: ");
		fgets(bufSend, 100, stdin);//从std读取内容存入writeline数组

		if(written(sockfd, bufSend)<0){//将writeline内容写入sockfd，如果写入字数<0则返回error
			printf("%s", "write error");
		}
		printf("客户端写入：%s\n",bufSend);
		
		read(sockfd, buffer, sizeof(buffer)-1);//把socket内容存入read数组
		printf("服务器返回：%s\n", buffer);
		fputs(buffer, stdout);//输出read数组的内容
	}

	close(sockfd);


}

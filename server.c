#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<stdio.h>
#include<strings.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<errno.h>
#include<time.h>
#include<sys/wait.h>

void handler(int s){
	int saved_errno=errno;
	while(waitpid(-1, NULL, WNOHANG)>0);
	errno=saved_errno;
	

}

int written(int sockfd, char sendline[]){
	ssize_t actualsize;
	int i;
	int readsize=strlen(sendline);
	printf("%s", "length of message received ");
	printf("%i\n", readsize-1);
	actualsize=write(sockfd, sendline, strlen(sendline));
	printf("%s", "length of message written ");
	printf("%zu\n", actualsize-1);

	if(actualsize<readsize){
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
	buf=buffer;

	while(1){
		memset(&ch, ' ', 1);
	

		read_bytes=read(newfd, &ch, 1);

		if(read_bytes>0){
			if(total_read<n-1){
				*buf=ch;
				buf++;
				total_read++;
				//printf("buf1=%s", buf-1);
			}	
		}
	
		if(read_bytes==-1){
			if(errno==EINTR)
				continue;
			else
				return -1;
		}	

		if(read_bytes==0){
			exit(1);//end of file
		}	

		if(ch=='\n')
			break;
	}

	*buf='\0';
	//printf("buf=%s", buf-total_read);
	return total_read+1;
}
#endif

int main(int arg, char *argv[]){
	char str[100];
	int listenfd;//套接字socket
	int newfd;//新的套接字
	pid_t childpid;
	int n;
	int port;
	struct sockaddr_in sadr;//向特定的IP和端口发起请求, 即service_address
	struct sigaction s;
	time_t time1,time2;

	port=atoi(argv[1]);//把port由字符串转换成数字
	if((listenfd=socket(PF_INET, SOCK_STREAM, 0))<0){//创建套接字listenfd
		perror("socket");
		exit(1);
	}
	else{
		fputs("socket created\n", stdout);
	}

	s.sa_flags=SA_RESTART;//如果信号中断了进程中的某个系统调用，则系统自动启动该系统调用
	s.sa_handler=handler;//代表新的信号处理函数
	sigemptyset(&s.sa_mask);//设置在处理该信号时将sa_mask指定的信号集暂时搁置

	if(sigaction(SIGCHLD, &s, NULL)==-1){
		perror("sigaction error");
		exit(1);
	}

	memset(&sadr, 0, sizeof sadr);//service_address初始化为0

	sadr.sin_family=AF_INET;//使用ipv4地址
	sadr.sin_port=htons(port);//端口
	sadr.sin_addr.s_addr=htons(INADDR_ANY);//泛指本机所有IP

	if(bind(listenfd, (struct sockaddr*)&sadr, sizeof sadr)<0){//绑定socket，如果出错，则关闭socket
		close(listenfd);
		perror("server bind error");
		exit(1);
	}

	//listen
	if(listen(listenfd, 20)<0){
		perror("listen error");
		exit(1);
	}

	for(;;){
		newfd=accept(listenfd, (struct sockaddr*)NULL, NULL);//accepting new connections from listen
		fputs("new connection accepted", stdout);
		fputs("\n", stdout);

		if((childpid=fork())==0){//child process id=0 when fork is called
			fputs("Child created\n", stdout);
			//fputs("\n", stdout);
			close(listenfd);//closing the listening socket of the child
			memset(str, 0, 100);
		}
		while((n=read(newfd, str, 100))>0&&(time1=time((time_t *)NULL))!=0){
			fputs("Echoing back the message received from client: ", stdout);
			fputs(str, stdout);
			printf("time1=%zu\n", time1);
			time2=time((time_t *)NULL);
			printf("time2=%zu\n", time2);
			printf("timestamp: %f\n", difftime(time1, time2));
			written(newfd, str);
			memset(str, 0, 100);
		}

		if(n<0){

			fputs("Read error\n", stdout);
			//exit(0);
		}
		exit(0);
	}

	close(newfd);
	
}

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

int main(int arg, char *argv[]){
	char str[100];
	int listenfd;//socket named listenfd
	int newfd;//a new socket
	pid_t childpid;
	int n;
	int port;
	struct sockaddr_in sadr;//service_address
	struct sigaction s;
	time_t tm;

	port=atoi(argv[1]);//transfer port from string to integer
	if((listenfd=socket(PF_INET, SOCK_STREAM, 0))<0){//create socket listenfd
		perror("socket");
		exit(1);
	}
	else{
		fputs("server socket created\n", stdout);
	}

	//Config the sigaction
	s.sa_flags=SA_RESTART;//If the signal interrupts a system call in the process, the system automatically initiates the system call
	s.sa_handler=handler;//A new signal handler
	sigemptyset(&s.sa_mask);//Set the signals of "sa_mask" on hold while processing the current signal
	if(sigaction(SIGCHLD, &s, NULL)==-1){
		perror("sigaction error");
		exit(1);
	}

	//Create service_address
	memset(&sadr, 0, sizeof sadr);//service_address initialed as 0
	sadr.sin_family=AF_INET;//Use IPv4
	sadr.sin_port=htons(port);//port
	sadr.sin_addr.s_addr=htons(INADDR_ANY);//all the local IP address

	//Bind the socket to service_address
	if(bind(listenfd, (struct sockaddr*)&sadr, sizeof sadr)<0){//bind the socket to service_address
		close(listenfd);
		perror("server bind error");
		exit(1);
	}

	//listen
	if(listen(listenfd, 20)<0){//20 is the max number of requests
		perror("listen error");
		exit(1);
	}


	//Keep accepting new connections from listen
	for(;;){
		newfd=accept(listenfd, (struct sockaddr*)NULL, NULL);//accepting new connections from listen
		if(newfd<0)exit(1);
		fputs("new connection accepted\n", stdout);

		if((childpid=fork())==0){//child process id=0 when fork is called
			fputs("Child created\n", stdout);
			printf("Child pid=%d\n",getpid());
			fputs("\n", stdout);
			close(listenfd);//closing the listening socket of the child
			memset(str, 0, 100);
			while((n=read(newfd, str, 100))>0){
				if(strcmp(str, ":exit")==0){
					printf("Disconnected from %d\n", getpid());
					break;
				}
				else{
					time(&tm);
					printf("Echoing back the message received from client: %s, and the time stamp is: %s\n",str, ctime(&tm));
					//time(&tm);
					//printf("Date and time: %s\n", ctime(&tm));
					write(newfd, str,100);
					bzero(str, sizeof(str));
				}
			}
		}

	}

	close(newfd);
	
}

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

int main(int argc, char *argv[]){
	int sockfd;
	char bufSend[100];//content in client
	char buffer[100];//content echoing back from server
	struct sockaddr_in sadr;
	int port;
	port=atoi(argv[2]);//get port 
	
	//Create socket
	sockfd=socket(PF_INET, SOCK_STREAM, 0);
	if(sockfd==-1){//sockfd=-1
		perror("socket");
		exit(-1);
	}
	else{
		fputs("socket created\n", stdout);
	} 
	
	//Initialize service_address
	memset(&sadr, 0, sizeof sadr);
	sadr.sin_family=AF_INET;//Use IPv4 
	sadr.sin_port=htons(port);
	inet_pton(AF_INET, argv[2], &(sadr.sin_addr));

	//Connect sockfd and service_address
	if(connect(sockfd, (struct sockaddr*)&sadr, sizeof(sadr))==-1){
		close(sockfd);
		perror("connection failed");
		return 1;
	}
	else{
		fputs("connected\n", stdout);
	}

	//Listen to the request from client
	while(1){
		memset(bufSend, 0, 100);//Initialize bufSend array
		memset(buffer, 0, 100);//Initialize buffer array
		printf("Input: ");
		fgets(bufSend, 100, stdin);//Save the content from stdin to bufSend array

		if(written(sockfd, bufSend)<0){//Write the content from bufSend array to sockfd
			printf("%s", "write error");
		}
		printf("client written：%s\n",bufSend);
		
		read(sockfd, buffer, sizeof(buffer)-1);//Save the content from socket to buffer array
		printf("server echos back：%s\n", buffer);
	}

	close(sockfd);


}

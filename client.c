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
#include<time.h>

#if 0
ssize_t written(int sockfd, char sendline[]){
	ssize_t actualsize;//
	int i;
	int readsize=strlen(sendline);
	printf("%s", "length of message received: ");
	printf("%i\n", readsize-1);

	actualsize=write(sockfd, sendline, strlen(sendline));
	printf("%s", "length of message written: ");
	printf("%zu\n", actualsize-1);

	if(actualsize<readsize){
		for(i=actualsize; i<=readsize; i++){
			write(sockfd, (void*)&sendline[i], 1);
		}
	}
	return actualsize;

}
#endif

int main(int argc, char *argv[]){
	int clientSocket;
	char bufSend[100];//content in client
	char buffer[100];//content echoing back from server
	struct sockaddr_in sadr;//service_address
	int port;
	time_t tm;
	port=atoi(argv[2]);//get port 
	
	//Create socket
	clientSocket=socket(PF_INET, SOCK_STREAM, 0);
	if(clientSocket==-1){//sockfd=-1
		perror("socket");
		exit(-1);
	}
	else{
		fputs("client socket created\n", stdout);
	} 
	
	//Initialize service_address
	memset(&sadr, 0, sizeof sadr);
	sadr.sin_family=AF_INET;//Use IPv4 
	sadr.sin_port=htons(port);
	inet_pton(AF_INET, argv[2], &(sadr.sin_addr));

	//Connect socket and service_address
	if(connect(clientSocket, (struct sockaddr*)&sadr, sizeof(sadr))==-1){
		close(clientSocket);
		perror("connection failed");
		return 1;
	}
	else{
		fputs("connected to server.\n", stdout);
	}

	//Listen to the request from client
	while(1){
		memset(bufSend, 0, 100);//Initialize bufSend array
		memset(buffer, 0, 100);//Initialize buffer array
		printf("Input: ");
		fgets(bufSend, 100, stdin);//Save the content from stdin to bufSend array

		if(write(clientSocket, bufSend,strlen(bufSend))<0){//Write the content from bufSend array to socket
			printf("%s", "write error");
		}
		
		printf("client written：%s\n",bufSend);
		
		read(clientSocket, buffer, sizeof(buffer)-1);//Save the content from socket to buffer array
		time(&tm);
		printf("server echos back：%s, and the time stamp is: %s\n", buffer, ctime(&tm));
	}

	close(clientSocket);


}

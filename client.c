/***********************************************************************************************
*	Date: April 8, 2019								       *
*	Description: The code act as a client and checks its own and server's incoming score to*
*			display the result of the game.					       *	
* 	Reference: Chapter 9, example 3							       *
************************************************************************************************/


#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>


int main(int argc, char *argv[]){
	char message[2];
	int server, portNumber, pid, n;
	struct sockaddr_in servAdd;
	int serverTotal = 0, clientTotal = 0, score;
	long int ss = 0;

	if(argc != 3){
		printf("Required argument(s): %s <IP Address> <Port Number>\n", argv[0]);
		exit(0);
	}

	if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr, "Unable to create socket\n");
		exit(1);
	}

	servAdd.sin_family = AF_INET;
	sscanf(argv[2], "%d", &portNumber);
	servAdd.sin_port = htons((uint16_t)portNumber);

	if(inet_pton(AF_INET, argv[1], &servAdd.sin_addr) < 0){
		fprintf(stderr, "inet_pton() has failed\n");
		exit(2);
	}

	if(connect(server, (struct sockaddr *) &servAdd, sizeof(servAdd))<0){
		fprintf(stderr, "connect() has failed,\nexiting...\n");
		exit(3);
	}

	read(server, message, 255);
	printf("Refree/Server: %s\n", message);
	printf("Hit enter after recieving server's score.\n");

	pid=fork();

	if(pid)
		while(1)                         
		if(n=read(server, &score, sizeof(int))){
			serverTotal += score;
			if(serverTotal >= 100){
				printf("Game over: You lost!\nExiting...\n");
				kill(getppid(), SIGTERM);
				exit(0);
			}
			printf("Server's total so far: %d\n", serverTotal);
		 }

	if(!pid)                           
		while(1)
		if(n=read(0, message, sizeof(char))){
			score = (int) time(&ss) % 10 + 1;
			clientTotal += score;
			write(server, &score, sizeof(int));
			if (clientTotal >= 100){
				fprintf(stderr, "Game over: You won!\nExiting...\n");
				kill(getppid(), SIGTERM);
				exit(0);
		 	}
		}
}



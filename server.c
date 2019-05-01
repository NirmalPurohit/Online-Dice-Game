/******************************************************************************************************
*	Author: Nirmal Purohit									      *
*	ID: 104944091										      *
*	Group member: Oluwajoba Hassan								      *
*	Date: April 8, 2019									      *
*	Description: The code below act as a server and listens on a provided port for a client. Once *
*			the connection is accepted, the server plays a dice game with the player, and *
*			also act as a refree to judge the incoming as well as its own score.	      *
*	Reference: Chapter 9, example 3								      *
*******************************************************************************************************/


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <wait.h>


void serviceClient(int);

int main(int argc, char *argv[]){  
	int sd, client, portNumber, status;
	struct sockaddr_in servAdd;

	if(argc != 2){
		printf("Required argument(s): %s <Port Number>\n", argv[0]);
		exit(0);
	}
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr, "Unable to create socket...\nTry runnig the server again.");
		exit(1);
	}
	sd = socket(AF_INET, SOCK_STREAM, 0);
	servAdd.sin_family = AF_INET;
	servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
	sscanf(argv[1], "%d", &portNumber);
	servAdd.sin_port = htons((uint16_t)portNumber);

	bind(sd, (struct sockaddr *) &servAdd, sizeof(servAdd));
	listen(sd, 5);

	while(1){
		printf("Waiting for a client to play the dice\n");
		client = accept(sd, NULL, NULL);
		printf("Got a client, starting the game...\n");

		if(!fork())
			serviceClient(client);
		close(client);
		waitpid(0, &status, WNOHANG);
	}
}

void serviceClient(int sd){
	char message[4];
	int n, pid, temp = sd;
	int clientTotal = 0, serverTotal = 0, score, flag = 0;
	long int ss = 0;

	write(sd, "Game on: you can now play your dice\n", 255);

	pid=fork();
	if(pid)          
		while(1)
			if(n=read(sd, &score, sizeof(int))){
				clientTotal += score;
				if(clientTotal >= 100){
					printf("Game over: You lost!\nWaiting for new player...\n");
					exit(0);
				}
				printf("Player's total so far: %d\n", clientTotal);
			}
	if(!pid)
		while(1)
			if(n=read(0, message, sizeof(char))){
				score = (int) time(&ss) % 10 + 1;
				serverTotal += score;
				write(sd, &score, sizeof(char));
				if (serverTotal >= 100){
					fprintf(stderr, "Game over: You won!\nWaiting for new player...\n");
					exit(0);
				}
			}

}

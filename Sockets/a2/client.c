/* 
	Name: Jeffin Biju
	Roll: EE19B085
	To run: 
			gcc -o client client.c
			./client X.X.X.X   where X.X.X.X is the ip address like 192.168.0.101 or 127.0.0.1
*/


#include <unistd.h>
#include <poll.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 1000
#define PORT 8080
#define SA struct sockaddr
void func(int sockfd)
{
	char buff[MAX];

	while(1){
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		printf("%s", buff);
		// enter row, col
		if(!strcmp(buff, "Enter (ROW, COL) for placing your mark: \n") || !strcmp(buff, "Invalid entry. Out of range. Enter (ROW, COL) again for placing your mark: \n") || !strcmp(buff, "Invalid entry. Already marked. Enter (ROW, COL) again for placing your mark: \n")){
			int row, col;
			struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };
			if( poll(&mypoll, 1, 15000) )
			{
				scanf("%d %d", &row, &col);
				bzero(buff, MAX);
				sprintf(buff, "%d %d\n", row, col);
				write(sockfd, buff, MAX);
			}
			else{
				// timeout
				bzero(buff, sizeof(buff));
				sprintf(buff, "Timeout\n");
				write(sockfd, buff, sizeof(buff));
			}
		}
		// replay
		if(!strcmp(buff, "Do you want to replay?[Y, N]\n") || !strcmp(buff, "Timeout. Do you want to replay?[Y, N]\n")){
			bzero(buff, MAX);
			scanf("%s", buff);
			write(sockfd, buff, MAX);
		}
		if(!strcmp(buff, "Sorry, replay unavailable since one of you did not agree\n")){
			break;
		}
		// quit
		if(!strcmp(buff, "Opponent has left\n")){
			break;
		}

	}
	
}

int main(int argc, char*argv[])
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));
	struct hostent *he;
	if (argc != 2)
	{
		perror("Incomplete arguments!");
		return 1;
	}
	he = gethostbyname(argv[1]);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);

	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

	// connect the client socket to server socket
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else{
		func(sockfd);
		close(sockfd);
	}
	
}


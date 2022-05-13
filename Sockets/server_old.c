// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>  
#include <unistd.h>
#define PORT 8080
#define MAX 1000

pthread_mutex_t lock;
FILE* fp;

struct players{
    int p1;
    int p2;
	int id1;
	int id2;
};
typedef struct players players;

void add(char* s1, char* s2, char*s3){
	int i = 0;
	while(s1[i]!='\0'){
		s3[i]=s1[i];
		i++;
	}
	int j = 0;
	while(s2[j]!='\0'){
		s3[i]=s2[j];
		i++;
		j++;
	}
}

void render(char* board, char* buff){
	for(int i=0; i<3; i++){
		char temp[100];
		sprintf(temp, "%c|%c|%c\n", board[3*i], board[3*i+1], board[3*i+2]);
		strncat(buff, temp, MAX);
	}
}

int check_status(char* board){   //-1 not over, 0 draw, 1 won, 2 won
	for(int i=0;i<3;i++){
		if((board[3*i]==board[3*i+1] && board[3*i]==board[3*i+2] && board[3*i]!='_') || (board[i]==board[i+3] && board[i]==board[i+6] && board[i]!='_')){
			if(board[3*i+i]=='O')
			return 1;
			return 2;
		}
	}
	if((board[0]==board[4] && board[0]==board[8] && board[0]!='_')|| (board[2]==board[4] && board[2]==board[6] && board[6]!='_')){
		if(board[4]=='O')
		return 1;
		return 2;
	}
	for(int i=0;i<9;i++){
		if(board[i]=='_')
		return -1;
	}
	return 0;
}
int update(char* board, char* buff, char c){
	int row, col;
	row = buff[0] - '1';
	col = buff[2] - '1';
	if(row<0 || row>2 || col<0 || col>2){
		return -3;
	}
	if(board[3*(row)+col]!='_'){
		return -2;
	}
	board[3*(row)+col] = c;
	return check_status(board);
}

void reset(char* board){
	for(int i=0;i<9;i++)
	board[i] = '_';
}

void* game(void* data){
    int sock1 = ((players*)data)->p1;
    int sock2 = ((players*)data)->p2;
	int id1 = ((players*)data)->id1;
	int id2 = ((players*)data)->id2;
	char board [9];
	for(int i=0; i<9; i++){
		board[i] = '_';
	}
	char buff1[MAX];
	char buff2[MAX];
	bzero(buff1, MAX);
	sprintf(buff1, "Starting the game...\n");
	write(sock1, buff1, sizeof(buff1));
	bzero(buff2, MAX);
	sprintf(buff2, "Starting the game...\n");
	write(sock2, buff2, sizeof(buff2));
	int winner = 0;
	int over = 0;
	int turn = 1;
	while(!over){
		bzero(buff1, MAX);
		render(board, buff1);
		write(sock1, buff1, sizeof(buff1));
		bzero(buff2, MAX);
		render(board, buff2);
		write(sock2, buff2, sizeof(buff2));
		int status = -4;
		if(turn&1){
			bzero(buff2, MAX);
			sprintf(buff2, "Waiting for opponent to move\n");
			write(sock2, buff2, sizeof(buff2));

			while(status < -1){
				bzero(buff1, MAX);
				if(status == -4){
					sprintf(buff1, "Enter (ROW, COL) for placing your mark: \n");
				}
				if(status == -3){
					sprintf(buff1, "Invalid entry. Out of range. Enter (ROW, COL) again for placing your mark: \n");
				}
				if(status == -2){
					sprintf(buff1, "Invalid entry. Already marked. Enter (ROW, COL) again for placing your mark: \n");
				}
				write(sock1, buff1, sizeof(buff1));
				bzero(buff1, MAX);
				if(!read(sock1, buff1, sizeof(buff1))){
					bzero(buff2, MAX);
					sprintf(buff2, "Opponent has left\n");
					write(sock2, buff2, sizeof(buff2));
					break;
				}
				if(!strcmp(buff1, "Timeout\n")){
					bzero(buff1, MAX);
					sprintf(buff1, "Timeout. Do you want to replay?[Y, N]\n");
					write(sock1, buff1, sizeof(buff1));
					bzero(buff2, MAX);
					sprintf(buff2, "Timeout. Do you want to replay?[Y, N]\n");
					write(sock2, buff2, sizeof(buff2));
					bzero(buff1, MAX);
					read(sock1, buff1, sizeof(buff1));
					bzero(buff2, MAX);
					read(sock2, buff2, sizeof(buff2));
					if(!strcmp(buff1, "Y") && !strcmp(buff2, "Y")){
						bzero(buff1, MAX);
						sprintf(buff1, "Playing again\n");
						write(sock1, buff1, sizeof(buff1));
						bzero(buff2, MAX);
						sprintf(buff2, "Playing again\n");
						write(sock2, buff2, sizeof(buff2));
						reset(board);
						turn = 0;
						break;
					}
					else{
						bzero(buff1, MAX);
						sprintf(buff1, "Sorry, replay unavailable since one of you did not agree\n");
						write(sock1, buff1, sizeof(buff1));
						bzero(buff2, MAX);
						sprintf(buff2, "Sorry, replay unavailable since one of you did not agree\n");
						write(sock2, buff2, sizeof(buff2));
						over = 1;
						break;
					}
				}
				status = update(board, buff1, 'O');
			}

		}
		else{
			bzero(buff1, MAX);
			sprintf(buff1, "Waiting for opponent to move\n");
			write(sock1, buff1, sizeof(buff1));
			while(status < -1){
				bzero(buff2, MAX);
				if(status == -4){
					sprintf(buff2, "Enter (ROW, COL) for placing your mark: \n");
				}
				if(status == -3){
					sprintf(buff2, "Invalid entry. Out of range. Enter (ROW, COL) again for placing your mark: \n");
				}
				if(status == -2){
					sprintf(buff2, "Invalid entry. Already marked. Enter (ROW, COL) again for placing your mark: \n");
				}
				write(sock2, buff2, sizeof(buff2));
				bzero(buff2, MAX);
				if(!read(sock2, buff2, sizeof(buff2))){
					bzero(buff1, MAX);
					sprintf(buff1, "Opponent has left\n");
					write(sock1, buff1, sizeof(buff1));
					break;
				}
				if(!strcmp(buff2, "Timeout\n")){
					bzero(buff1, MAX);
					sprintf(buff1, "Timeout. Do you want to replay?[Y, N]\n");
					write(sock1, buff1, sizeof(buff1));
					bzero(buff2, MAX);
					sprintf(buff2, "Timeout. Do you want to replay?[Y, N]\n");
					write(sock2, buff2, sizeof(buff2));
					bzero(buff1, MAX);
					read(sock1, buff1, sizeof(buff1));
					bzero(buff2, MAX);
					read(sock2, buff2, sizeof(buff2));
					if(!strcmp(buff1, "Y") && !strcmp(buff2, "Y")){
						bzero(buff1, MAX);
						sprintf(buff1, "Playing again\n");
						write(sock1, buff1, sizeof(buff1));
						bzero(buff2, MAX);
						sprintf(buff2, "Playing again\n");
						write(sock2, buff2, sizeof(buff2));
						reset(board);
						turn = 0;
						break;
					}
					else{
						bzero(buff1, MAX);
						sprintf(buff1, "Sorry, replay unavailable since one of you did not agree\n");
						write(sock1, buff1, sizeof(buff1));
						bzero(buff2, MAX);
						sprintf(buff2, "Sorry, replay unavailable since one of you did not agree\n");
						write(sock2, buff2, sizeof(buff2));
						over = 1;
						break;
					}
				}
				status = update(board, buff2, 'X');
			}
		}
		if(status == 0 || status == 1 || status == 2){
			bzero(buff1, MAX);
			render(board, buff1);
			write(sock1, buff1, sizeof(buff1));
			bzero(buff2, MAX);
			render(board, buff2);
			write(sock2, buff2, sizeof(buff2));
		}
		if(status == 2){

			bzero(buff1, MAX);
			sprintf(buff1, "Game over. Player %d won \n", id2);
			write(sock1, buff1, sizeof(buff1));
			bzero(buff2, MAX);
			sprintf(buff2, "Game over. Player %d won \n", id2);
			write(sock2, buff2, sizeof(buff2));
		}
		if(status == 1){
			bzero(buff1, MAX);
			sprintf(buff1, "Game over. Player %d won \n", id1);
			write(sock1, buff1, sizeof(buff1));
			bzero(buff2, MAX);
			sprintf(buff2, "Game over. Player %d won \n", id1);
			write(sock2, buff2, sizeof(buff2));
		}
		if(status == 0){
			bzero(buff1, MAX);
			sprintf(buff1, "Game over. Draw\n");
			write(sock1, buff1, sizeof(buff1));
			bzero(buff2, MAX);
			sprintf(buff2, "Game over. Draw\n");
			write(sock2, buff2, sizeof(buff2));
		}
		if(status == 0 || status == 1 || status == 2){
			bzero(buff1, MAX);
			sprintf(buff1, "Do you want to replay?[Y, N]\n");
			write(sock1, buff1, sizeof(buff1));
			bzero(buff2, MAX);
			sprintf(buff2, "Do you want to replay?[Y, N]\n");
			write(sock2, buff2, sizeof(buff2));
			bzero(buff1, MAX);
			read(sock1, buff1, sizeof(buff1));
			bzero(buff2, MAX);
			read(sock2, buff2, sizeof(buff2));
			if(!strcmp(buff1, "Y") && !strcmp(buff2, "Y")){
				bzero(buff1, MAX);
				sprintf(buff1, "Playing again\n");
				write(sock1, buff1, sizeof(buff1));
				bzero(buff2, MAX);
				sprintf(buff2, "Playing again\n");
				write(sock2, buff2, sizeof(buff2));
				reset(board);
				turn = 1;
				continue;
			}
			else{
				bzero(buff1, MAX);
				sprintf(buff1, "Sorry, replay unavailable since one of you did not agree\n");
				write(sock1, buff1, sizeof(buff1));
				bzero(buff2, MAX);
				sprintf(buff2, "Sorry, replay unavailable since one of you did not agree\n");
				write(sock2, buff2, sizeof(buff2));
				break;
			}
		}
		turn++;
	}


	// int cnt = 1;
	// char buff1[MAX];
	// char buff2[MAX];
	// char buff3[2*MAX];
	// printf("Hi %u and %u \n", sock1, sock2);
	// while(1){
	// 	if(cnt&1){
	// 		bzero(buff1, sizeof(buff1));
	// 		read(sock1, buff1, sizeof(buff1));
	// 		if ((strncmp(buff1, "exit", 4)) == 0) {
	// 			printf("server Exit...\n");
	// 			break;
    //     	}
	// 	}
	// 	else{
	// 		bzero(buff2, sizeof(buff2));
	// 		read(sock2, buff2, sizeof(buff2));
	// 		if ((strncmp(buff2, "exit", 4)) == 0) {
	// 			printf("server Exit...\n");
	// 			break;
    //     	}
	// 		bzero(buff3, sizeof(buff3));
	// 		add(buff1, buff2, buff3);
	// 		write(sock1, buff3, sizeof(buff3));
	// 		write(sock2, buff3, sizeof(buff3));
	// 	}
	// 	cnt += 1;
	// }
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{


	fp = fopen("file.txt", "w");
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }
    int server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("Server socket failed");
		exit(EXIT_FAILURE);
	}
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_NOSIGPIPE,
												&opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
	// struct timeval tv;
	// tv.tv_sec = 1;
	// tv.tv_usec = 0;
	// if (setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv))
	// {
	// 	perror("setsockopt");
	// 	exit(EXIT_FAILURE);
	// }
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );
    if (bind(server_fd, (struct sockaddr *)&address,
								sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
    }
	char buff[MAX];
	printf("Game server started. Waiting for players.\n");
    int cnt = 1;
    int new_socket1, new_socket2;

    while(1){
        if (listen(server_fd, 10) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        if(cnt&1){
            if ((new_socket1 = accept(server_fd, (struct sockaddr *)&address,
					(socklen_t*)&addrlen))<0)
            {
                perror("accept1");
                exit(EXIT_FAILURE);
            }
			bzero(buff, MAX);
			sprintf(buff, "Connected to the game server. Your player ID is %d. Waiting for a partner to join . . .\n", cnt);
			//strncpy(buff, "Connected to the game server. Your player ID is %d. Waiting for a partner to join . . .\n", , MAX);
			//buff = "Connected to the game server. Your player ID is 1. Waiting for a partner to join . . .\n"
			write(new_socket1, buff, sizeof(buff));
        }
        else{
            if ((new_socket2 = accept(server_fd, (struct sockaddr *)&address,
					(socklen_t*)&addrlen))<0)
            {
                perror("accept2");
                exit(EXIT_FAILURE);
            }
			bzero(buff, MAX);
			sprintf(buff, "Connected to the game server. Your player ID is %d. Your partner’s ID is %d. Your symbol is ‘X’.\n", cnt, cnt-1);
			//strncpy(buff, "Connected to the game server. Your player ID is 2. Your partner’s ID is 1. Your symbol is ‘X’.\n", MAX);
			//buff = "Connected to the game server. Your player ID is 2. Your partner’s ID is 1. Your symbol is ‘X’.\n"
			write(new_socket2, buff, sizeof(buff));
			bzero(buff, MAX);
			sprintf(buff, "Your partner's ID is %d. Your symbol is ‘O’.\n", cnt);
			//strncpy(buff, "Your partner's ID is 2. Your symbol is ‘O’.\n", MAX);
			//buff = "Your partner's ID is 2. Your symbol is ‘O’.\n"
			write(new_socket1, buff, sizeof(buff));
            pthread_t  thread_id;
            players data;
            data.p1 = new_socket1;
            data.p2 = new_socket2;
			data.id1 = cnt-1;
			data.id2 = cnt;
            pthread_create(&thread_id, NULL, game, (void*) &data);
			printf("Game %d created.\n", cnt/2);
        }
        cnt += 1;
    }
	pthread_mutex_destroy(&lock);
    fclose(fp);
	printf("hi");
	return 0;
}


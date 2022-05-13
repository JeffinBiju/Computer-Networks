/* 
	Name: Jeffin Biju
	Roll: EE19B085
	To run: 
			gcc -o server server.c
			./server
*/


// Implements 1 a), b), c) and d).

// Tic Tac Toe using server and client. Supports multiple clients on multiple systems, replay, timeouts and log file entries. 
// Rows and Columns are 1 indexed. Handles invalid inputs.

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

// log file
FILE* fp;

int gameid = 1;
// lock for gameid
pthread_mutex_t lock1;
// lock for file
pthread_mutex_t lock2;

struct players{
    int p1;
    int p2;
	int id1;
	int id2;
    int gid;
};
typedef struct players players;


// get game id
void getgame(void* data){
    pthread_mutex_lock(&lock2);
    ((players*)data)->gid = gameid;
    gameid = gameid + 1;
    pthread_mutex_unlock(&lock2);
}

// log file
void logfun(void* data, int result, int* moves, int sock1, int sock2){
    char gamedata[MAX];
    sprintf(gamedata, "\n\n\n\nGame: %d\n Player1 id: %d\n Player2 id: %d\n Result: %d \nMoves:\n", ((players*)data)->gid, ((players*)data)->id1, ((players*)data)->id2, result);
    for(int i=0;i<MAX;i++){
        if(moves[i]==0)
        break;
		moves[i] -= 1;
		if(i&1){
			char movedata[MAX];
			sprintf(movedata, "Player2: (%d, %d)\n", moves[i]/3+1, moves[i]%3+1);
			strcat(gamedata, movedata);
		}
		else{
			char movedata[MAX];
			sprintf(movedata, "Player1: (%d, %d)\n", moves[i]/3+1, moves[i]%3+1);
			strcat(gamedata, movedata);
		}
    }
	pthread_mutex_lock(&lock1);
    fp = fopen("file.txt", "a");
    fputs(gamedata, fp);
    fclose(fp);
    pthread_mutex_unlock(&lock1);
}

// render tic tac toe board
void render(char* board, char* buff){
	for(int i=0; i<3; i++){
		char temp[100];
		sprintf(temp, "%c|%c|%c\n", board[3*i], board[3*i+1], board[3*i+2]);
		strncat(buff, temp, MAX);
	}
}

// check status of game. returns -1: game not over, 0: draw, 1: player 1 won, 2: player 2 won
int check_status(char* board){   
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

// update board. // returns -3: row, col out of range, -2: already marked, -1: game not over, 0: draw, 1: player 1 won, 2: player 2 won
int update(char* board, char* buff, char c, int* moves, int* pos){
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
    moves[*pos] = 3*(row)+col+1;
    (*pos)++;
	return check_status(board);
}

// reset tic tac toe board
void reset(char* board){
	for(int i=0;i<9;i++)
	board[i] = '_';
}

// game function
void* game(void* data){
    int moves[MAX];
    int pos = 0;
    int sock1 = ((players*)data)->p1;
    int sock2 = ((players*)data)->p2;
	int id1 = ((players*)data)->id1;
	int id2 = ((players*)data)->id2;
    int gid = ((players*)data)->gid;
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
		// to record game status. -1: not over 0: draw, 1: player 1 won, 2: player 2 won
		int status = -4;
		// player 1 turn
		if(turn&1){
			bzero(buff2, MAX);
			sprintf(buff2, "Waiting for opponent to move\n");
			write(sock2, buff2, sizeof(buff2));
			// Enter row, col. Loop until you receive valid entry.
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
                    logfun(data, -2, moves, sock1, sock2);
                    over = 1;
					break;
				}
				// handle timeout with replay
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
                        logfun(data, -1, moves, sock1, sock2);
                        getgame(data);
                        bzero(moves, sizeof(moves));
                        pos = 0;
						break;
					}
					else{
						bzero(buff1, MAX);
						sprintf(buff1, "Sorry, replay unavailable since one of you did not agree\n");
						write(sock1, buff1, sizeof(buff1));
						bzero(buff2, MAX);
						sprintf(buff2, "Sorry, replay unavailable since one of you did not agree\n");
						write(sock2, buff2, sizeof(buff2));
                        logfun(data, -1, moves, sock1, sock2);
						over = 1;
						break;
					}
				}
				status = update(board, buff1, 'O', moves, &pos);
			}

		}
		// player 2 turn
		else{  
			bzero(buff1, MAX);
			sprintf(buff1, "Waiting for opponent to move\n");
			write(sock1, buff1, sizeof(buff1));
			// Loop until you receive valid entry.
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
                    logfun(data, -2, moves, sock1, sock2);
                    over = 1;
					break;
				}
				// handle timeout with replay
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
                        logfun(data, -1, moves, sock1, sock2);
                        getgame(data);
                        bzero(moves, sizeof(moves));
                        pos = 0;
						break;
					}
					else{
						bzero(buff1, MAX);
						sprintf(buff1, "Sorry, replay unavailable since one of you did not agree\n");
						write(sock1, buff1, sizeof(buff1));
						bzero(buff2, MAX);
						sprintf(buff2, "Sorry, replay unavailable since one of you did not agree\n");
						write(sock2, buff2, sizeof(buff2));
                        logfun(data, -1, moves, sock1, sock2);
						over = 1;
						break;
					}
				}
				status = update(board, buff2, 'X', moves, &pos);
			}
		}
		// Game over
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
		// handle replay
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
                logfun(data, status, moves, sock1, sock2);
                getgame(data);
                bzero(moves, sizeof(moves));
                pos = 0;
				continue;
			}
			else{
				bzero(buff1, MAX);
				sprintf(buff1, "Sorry, replay unavailable since one of you did not agree\n");
				write(sock1, buff1, sizeof(buff1));
				bzero(buff2, MAX);
				sprintf(buff2, "Sorry, replay unavailable since one of you did not agree\n");
				write(sock2, buff2, sizeof(buff2));
                logfun(data, status, moves, sock1, sock2);
                over = 1;
				break;
			}
		}
		turn++;
	}
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{


	fp = fopen("file.txt", "w");
	fputs("Shows Game id, Player 1 and Player 2 id, Sequence of moves in (row, col) format indexed by 1 and Result.\nResult:\n-2: player disconnected\n-1: timeout\n0: draw\n1: Player O (odd id) won\n2: Player X (even id) won\n", fp);
    fclose(fp);

	//locks
	if (pthread_mutex_init(&lock1, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }
    if (pthread_mutex_init(&lock2, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }
    int server_fd;
	// create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("Server socket failed");
		exit(EXIT_FAILURE);
	}
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int opt = 1;
	// set options
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
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );
	// bind
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
	// keep listening. 
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
			write(new_socket2, buff, sizeof(buff));
			bzero(buff, MAX);
			sprintf(buff, "Your partner's ID is %d. Your symbol is ‘O’.\n", cnt);
			write(new_socket1, buff, sizeof(buff));
            pthread_t  thread_id;
			// create new game
            players* data = malloc(sizeof(players)) ;
            data->p1 = new_socket1;
            data->p2 = new_socket2;
			data->id1 = cnt-1;
			data->id2 = cnt;
            getgame(data);
            pthread_create(&thread_id, NULL, game, (void*) data);
			printf("Game %d created.\n", cnt/2);
        }
        cnt += 1;
    }
	pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);
    fclose(fp);
	return 0;
}


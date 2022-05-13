// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>  
#define PORT 8080

struct players{
    int p1;
    int p2;
};
typedef struct players players;
void* game(void* data){
    int sock1 = ((players*)data)->p1;
    int sock2 = ((players*)data)->p2;
    printf("Hi %u and %u \n", sock1, sock2);
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{

    int server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("Server socket failed");
		exit(EXIT_FAILURE);
	}
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
												&opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );
    if (bind(server_fd, (struct sockaddr *)&address,
								sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
    }

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
        }
        else{
            if ((new_socket2 = accept(server_fd, (struct sockaddr *)&address,
					(socklen_t*)&addrlen))<0)
            {
                perror("accept2");
                exit(EXIT_FAILURE);
            }
            pthread_t  thread_id;
            players data;
            data.p1 = new_socket1;
            data.p2 = new_socket2;
            pthread_create(&thread_id, NULL, game, (void*) &data);
        }
        cnt += 1;
    }
	return 0;
}


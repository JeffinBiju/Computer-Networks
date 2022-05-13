#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
 
int main()
{
    int network_socket;
    network_socket=socket(AF_INET, SOCK_STREAM, 0);
    //socket created
 
    struct sockaddr_in server_address;
    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(9002);
    server_address.sin_addr.s_addr="192.168.1.103";
 
    int connect_socket=connect(network_socket,(struct sockaddr*)&server_address,sizeof(server_address));
    if(connect_socket==-1)
    {
        printf("Error connecting socket");
        exit(1);
    }
    //socket connected to the server
 
    char ch[1024];
     
    //recieve data from server
    while(1){
    recv(network_socket, &ch, sizeof(ch), 0);
    printf("The recieved data from the server is:\n%s",ch);
    gets(ch);
    send(network_socket,ch,strlen(ch),0);}
     
    //close(sock);
return 0;
}   
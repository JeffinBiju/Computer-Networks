/*

	Name: Jeffin Biju
	Roll: EE19B085
	Ping program in C. Sends 1 ping request. Handles invalid ip addresses, unreachable hosts and timeout. Returns RTT in ms.
	To run:
		gcc -o yapp yapp.c
		sudo ./yapp 8.8.8.8

*/

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define PKT_SIZE 64
#define PORT_NO 0

//timeout
#define TIMEOUT 3


// ping packet
struct ping_pkt
{
	struct icmphdr hdr;
	char msg[PKT_SIZE-sizeof(struct icmphdr)];
};

unsigned short helper(void *b, int len)
{ 	
	unsigned short *buf = b;
	unsigned int sum=0;
	unsigned short result;
	for ( sum = 0; len > 1; len -= 2 )
		sum += *buf++;
	if ( len == 1 )
	sum += *(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}

// make a ping request
void ping_request(int ping_sockfd, struct sockaddr_in *ping_addr, char *ping_ip) 
{
	int addr_len, i;
	struct ping_pkt pckt;
	struct sockaddr_in r_addr;
	// to find RTT
	struct timespec time_start, time_end;
	long double rtt_msec=0;
	// to handle timeout
	struct timeval tv_out;
	tv_out.tv_sec = TIMEOUT;
	tv_out.tv_usec = 0;

	// setting timeout of recv 
	setsockopt(ping_sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_out, sizeof tv_out);
	
	
	bzero(&pckt, sizeof(pckt));
	pckt.hdr.type = ICMP_ECHO;
	pckt.hdr.un.echo.id = getpid();
	pckt.hdr.checksum = helper(&pckt, sizeof(pckt));

	//send packet
	clock_gettime(CLOCK_MONOTONIC, &time_start);
	if ( sendto(ping_sockfd, &pckt, sizeof(pckt), 0, (struct sockaddr*) ping_addr, sizeof(*ping_addr)) <= 0)
	{
	
		printf("\nHost unreachable!\n");    // host unreachbale.
		return;
	}

	//receive packet
	addr_len=sizeof(r_addr);

	if ( recvfrom(ping_sockfd, &pckt, sizeof(pckt), 0, (struct sockaddr*)&r_addr, &addr_len) <= 0)
	{
		printf("\nRequest timed out\n");    // timeout message.
		return;
	}
	
	else
	{	//calculate RTT
		clock_gettime(CLOCK_MONOTONIC, &time_end);
		double timeElapsed = ((double)(time_end.tv_nsec - time_start.tv_nsec))/1000000.0;
		rtt_msec = (time_end.tv_sec- time_start.tv_sec) * 1000.0 + timeElapsed;
		printf("\nReply from %s. RTT = %Lf ms.\n", ping_ip, rtt_msec);
	}	
}

int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in addr_con;
	int addrlen = sizeof(addr_con);
	if(argc!=2)
	{
		printf("\nFormat should be : %s <address>\n", argv[0]);
		return 0;
	}
	printf("\nResolving DNS..\n");
	struct hostent *host_entity;
	char *ip_addr=(char*)malloc(NI_MAXHOST*sizeof(char));
	int i;
	// check if IP address is valid.
	if ((host_entity = gethostbyname(argv[1])) == NULL)
	{
		printf("\nBad Host. Invalid IP address\n");
		return 0;
	}
	strcpy(ip_addr, inet_ntoa(*(struct in_addr *)host_entity->h_addr));
	(addr_con).sin_family = host_entity->h_addrtype;
	(addr_con).sin_port = htons (PORT_NO);
	(addr_con).sin_addr.s_addr = *(long*)host_entity->h_addr;
	printf("\nTrying to connect to '%s' IP: %s\n", argv[1], ip_addr);
	// create socket
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(sockfd<0)
	{
		printf("\nSocket creation failed\n");
		return 0;
	}
	// ping
	ping_request(sockfd, &addr_con, ip_addr);
	return 0;
}


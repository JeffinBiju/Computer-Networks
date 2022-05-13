// C program to Implement Ping

// compile as -o ping
// run as sudo ./ping <hostname>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>


/* Copyright (C) 1991, 92, 93, 95, 96, 97, 99 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifndef __NETINET_IP_ICMP_H
#define __NETINET_IP_ICMP_H    1

#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

struct icmphdr
{
  u_int8_t type;                /* message type */
  u_int8_t code;                /* type sub-code */
  u_int16_t checksum;
  union
  {
    struct
    {
      u_int16_t        id;
      u_int16_t        sequence;
    } echo;                        /* echo datagram */
    u_int32_t        gateway;        /* gateway address */
    struct
    {
      u_int16_t        __unused;
      u_int16_t        mtu;
    } frag;                        /* path mtu discovery */
  } un;
};

#define ICMP_ECHOREPLY                0        /* Echo Reply                        */
#define ICMP_DEST_UNREACH        3        /* Destination Unreachable        */
#define ICMP_SOURCE_QUENCH        4        /* Source Quench                */
#define ICMP_REDIRECT                5        /* Redirect (change route)        */
#define ICMP_ECHO                8        /* Echo Request                        */
#define ICMP_TIME_EXCEEDED        11        /* Time Exceeded                */
#define ICMP_PARAMETERPROB        12        /* Parameter Problem                */
#define ICMP_TIMESTAMP                13        /* Timestamp Request                */
#define ICMP_TIMESTAMPREPLY        14        /* Timestamp Reply                */
#define ICMP_INFO_REQUEST        15        /* Information Request                */
#define ICMP_INFO_REPLY                16        /* Information Reply                */
#define ICMP_ADDRESS                17        /* Address Mask Request                */
#define ICMP_ADDRESSREPLY        18        /* Address Mask Reply                */
#define NR_ICMP_TYPES                18


/* Codes for UNREACH. */
#define ICMP_NET_UNREACH        0        /* Network Unreachable                */
#define ICMP_HOST_UNREACH        1        /* Host Unreachable                */
#define ICMP_PROT_UNREACH        2        /* Protocol Unreachable                */
#define ICMP_PORT_UNREACH        3        /* Port Unreachable                */
#define ICMP_FRAG_NEEDED        4        /* Fragmentation Needed/DF set        */
#define ICMP_SR_FAILED                5        /* Source Route failed                */
#define ICMP_NET_UNKNOWN        6
#define ICMP_HOST_UNKNOWN        7
#define ICMP_HOST_ISOLATED        8
#define ICMP_NET_ANO                9
#define ICMP_HOST_ANO                10
#define ICMP_NET_UNR_TOS        11
#define ICMP_HOST_UNR_TOS        12
#define ICMP_PKT_FILTERED        13        /* Packet filtered */
#define ICMP_PREC_VIOLATION        14        /* Precedence violation */
#define ICMP_PREC_CUTOFF        15        /* Precedence cut off */
#define NR_ICMP_UNREACH                15        /* instead of hardcoding immediate value */

/* Codes for REDIRECT. */
#define ICMP_REDIR_NET                0        /* Redirect Net                        */
#define ICMP_REDIR_HOST                1        /* Redirect Host                */
#define ICMP_REDIR_NETTOS        2        /* Redirect Net for TOS                */
#define ICMP_REDIR_HOSTTOS        3        /* Redirect Host for TOS        */

/* Codes for TIME_EXCEEDED. */
#define ICMP_EXC_TTL                0        /* TTL count exceeded                */
#define ICMP_EXC_FRAGTIME        1        /* Fragment Reass time exceeded        */


#ifdef __USE_BSD
/*
 * Copyright (c) 1982, 1986, 1993
 *        The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *        @(#)ip_icmp.h        8.1 (Berkeley) 6/10/93
 */

#include <netinet/in.h>
#include <netinet/ip.h>

/*
 * Internal of an ICMP Router Advertisement
 */
struct icmp_ra_addr
{
  u_int32_t ira_addr;
  u_int32_t ira_preference;
};

struct icmp
{
  u_int8_t  icmp_type;        /* type of message, see below */
  u_int8_t  icmp_code;        /* type sub code */
  u_int16_t icmp_cksum;        /* ones complement checksum of struct */
  union
  {
    u_char ih_pptr;                /* ICMP_PARAMPROB */
    struct in_addr ih_gwaddr;        /* gateway address */
    struct ih_idseq                /* echo datagram */
    {
      u_int16_t icd_id;
      u_int16_t icd_seq;
    } ih_idseq;
    u_int32_t ih_void;

    /* ICMP_UNREACH_NEEDFRAG -- Path MTU Discovery (RFC1191) */
    struct ih_pmtu
    {
      u_int16_t ipm_void;
      u_int16_t ipm_nextmtu;
    } ih_pmtu;

    struct ih_rtradv
    {
      u_int8_t irt_num_addrs;
      u_int8_t irt_wpa;
      u_int16_t irt_lifetime;
    } ih_rtradv;
  } icmp_hun;
#define        icmp_pptr        icmp_hun.ih_pptr
#define        icmp_gwaddr        icmp_hun.ih_gwaddr
#define        icmp_id                icmp_hun.ih_idseq.icd_id
#define        icmp_seq        icmp_hun.ih_idseq.icd_seq
#define        icmp_void        icmp_hun.ih_void
#define        icmp_pmvoid        icmp_hun.ih_pmtu.ipm_void
#define        icmp_nextmtu        icmp_hun.ih_pmtu.ipm_nextmtu
#define        icmp_num_addrs        icmp_hun.ih_rtradv.irt_num_addrs
#define        icmp_wpa        icmp_hun.ih_rtradv.irt_wpa
#define        icmp_lifetime        icmp_hun.ih_rtradv.irt_lifetime
  union
  {
    struct
    {
      u_int32_t its_otime;
      u_int32_t its_rtime;
      u_int32_t its_ttime;
    } id_ts;
    struct
    {
      struct ip idi_ip;
      /* options and then 64 bits of data */
    } id_ip;
    struct icmp_ra_addr id_radv;
    u_int32_t   id_mask;
    u_int8_t    id_data[1];
  } icmp_dun;
#define        icmp_otime        icmp_dun.id_ts.its_otime
#define        icmp_rtime        icmp_dun.id_ts.its_rtime
#define        icmp_ttime        icmp_dun.id_ts.its_ttime
#define        icmp_ip                icmp_dun.id_ip.idi_ip
#define        icmp_radv        icmp_dun.id_radv
#define        icmp_mask        icmp_dun.id_mask
#define        icmp_data        icmp_dun.id_data
};

/*
 * Lower bounds on packet lengths for various types.
 * For the error advice packets must first insure that the
 * packet is large enough to contain the returned ip header.
 * Only then can we do the check to see if 64 bits of packet
 * data have been returned, since we need to check the returned
 * ip header length.
 */
#define        ICMP_MINLEN        8                                /* abs minimum */
#define        ICMP_TSLEN        (8 + 3 * sizeof (n_time))        /* timestamp */
#define        ICMP_MASKLEN        12                                /* address mask */
#define        ICMP_ADVLENMIN        (8 + sizeof (struct ip) + 8)        /* min */
#ifndef _IP_VHL
#define        ICMP_ADVLEN(p)        (8 + ((p)->icmp_ip.ip_hl << 2) + 8)
        /* N.B.: must separately check that ip_hl >= 5 */
#else
#define        ICMP_ADVLEN(p)        (8 + (IP_VHL_HL((p)->icmp_ip.ip_vhl) << 2) + 8)
        /* N.B.: must separately check that header length >= 5 */
#endif

/* Definition of type and code fields. */
/* defined above: ICMP_ECHOREPLY, ICMP_REDIRECT, ICMP_ECHO */
#define        ICMP_UNREACH                3                /* dest unreachable, codes: */
#define        ICMP_SOURCEQUENCH        4                /* packet lost, slow down */
#define        ICMP_ROUTERADVERT        9                /* router advertisement */
#define        ICMP_ROUTERSOLICIT        10                /* router solicitation */
#define        ICMP_TIMXCEED                11                /* time exceeded, code: */
#define        ICMP_PARAMPROB                12                /* ip header bad */
#define        ICMP_TSTAMP                13                /* timestamp request */
#define        ICMP_TSTAMPREPLY        14                /* timestamp reply */
#define        ICMP_IREQ                15                /* information request */
#define        ICMP_IREQREPLY                16                /* information reply */
#define        ICMP_MASKREQ                17                /* address mask request */
#define        ICMP_MASKREPLY                18                /* address mask reply */

#define        ICMP_MAXTYPE                18

/* UNREACH codes */
#define        ICMP_UNREACH_NET                0        /* bad net */
#define        ICMP_UNREACH_HOST                1        /* bad host */
#define        ICMP_UNREACH_PROTOCOL                2        /* bad protocol */
#define        ICMP_UNREACH_PORT                3        /* bad port */
#define        ICMP_UNREACH_NEEDFRAG                4        /* IP_DF caused drop */
#define        ICMP_UNREACH_SRCFAIL                5        /* src route failed */
#define        ICMP_UNREACH_NET_UNKNOWN        6        /* unknown net */
#define        ICMP_UNREACH_HOST_UNKNOWN       7        /* unknown host */
#define        ICMP_UNREACH_ISOLATED                8        /* src host isolated */
#define        ICMP_UNREACH_NET_PROHIB                9        /* net denied */
#define        ICMP_UNREACH_HOST_PROHIB        10        /* host denied */
#define        ICMP_UNREACH_TOSNET                11        /* bad tos for net */
#define        ICMP_UNREACH_TOSHOST                12        /* bad tos for host */
#define        ICMP_UNREACH_FILTER_PROHIB      13        /* admin prohib */
#define        ICMP_UNREACH_HOST_PRECEDENCE    14        /* host prec vio. */
#define        ICMP_UNREACH_PRECEDENCE_CUTOFF  15        /* prec cutoff */

/* REDIRECT codes */
#define        ICMP_REDIRECT_NET        0                /* for network */
#define        ICMP_REDIRECT_HOST        1                /* for host */
#define        ICMP_REDIRECT_TOSNET        2                /* for tos and net */
#define        ICMP_REDIRECT_TOSHOST        3                /* for tos and host */

/* TIMEXCEED codes */
#define        ICMP_TIMXCEED_INTRANS        0                /* ttl==0 in transit */
#define        ICMP_TIMXCEED_REASS        1                /* ttl==0 in reass */

/* PARAMPROB code */
#define        ICMP_PARAMPROB_OPTABSENT 1                /* req. opt. absent */

#define        ICMP_INFOTYPE(type) \
        ((type) == ICMP_ECHOREPLY || (type) == ICMP_ECHO || \
        (type) == ICMP_ROUTERADVERT || (type) == ICMP_ROUTERSOLICIT || \
        (type) == ICMP_TSTAMP || (type) == ICMP_TSTAMPREPLY || \
        (type) == ICMP_IREQ || (type) == ICMP_IREQREPLY || \
        (type) == ICMP_MASKREQ || (type) == ICMP_MASKREPLY)

#endif /* __USE_BSD */

__END_DECLS

#endif /* netinet/ip_icmp.h */

// Define the Packet Constants
// ping packet size
#define PING_PKT_S 64

// Automatic port number
#define PORT_NO 0

// Automatic port number
#define PING_SLEEP_RATE 1000000 x

// Gives the timeout delay for receiving packets
// in seconds
#define RECV_TIMEOUT 1

// Define the Ping Loop
int pingloop=1;


// ping packet structure
struct ping_pkt
{
	struct icmphdr hdr;
	char msg[PING_PKT_S-sizeof(struct icmphdr)];
};

// Calculating the Check Sum
unsigned short checksum(void *b, int len)
{ unsigned short *buf = b;
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


// Interrupt handler
void intHandler(int dummy)
{
	pingloop=0;
}

// Performs a DNS lookup
char *dns_lookup(char *addr_host, struct sockaddr_in *addr_con)
{
	printf("\nResolving DNS..\n");
	struct hostent *host_entity;
	char *ip=(char*)malloc(NI_MAXHOST*sizeof(char));
	int i;

	if ((host_entity = gethostbyname(addr_host)) == NULL)
	{
		// No ip found for hostname
		return NULL;
	}
	
	//filling up address structure
	strcpy(ip, inet_ntoa(*(struct in_addr *)
						host_entity->h_addr));

	(*addr_con).sin_family = host_entity->h_addrtype;
	(*addr_con).sin_port = htons (PORT_NO);
	(*addr_con).sin_addr.s_addr = *(long*)host_entity->h_addr;

	return ip;
	
}

// Resolves the reverse lookup of the hostname
char* reverse_dns_lookup(char *ip_addr)
{
	struct sockaddr_in temp_addr;	
	socklen_t len;
	char buf[NI_MAXHOST], *ret_buf;

	temp_addr.sin_family = AF_INET;
	temp_addr.sin_addr.s_addr = inet_addr(ip_addr);
	len = sizeof(struct sockaddr_in);

	if (getnameinfo((struct sockaddr *) &temp_addr, len, buf,
					sizeof(buf), NULL, 0, NI_NAMEREQD))
	{
		printf("Could not resolve reverse lookup of hostname\n");
		return NULL;
	}
	ret_buf = (char*)malloc((strlen(buf) +1)*sizeof(char) );
	strcpy(ret_buf, buf);
	return ret_buf;
}

// make a ping request
void send_ping(int ping_sockfd, struct sockaddr_in *ping_addr,
				char *ping_dom, char *ping_ip, char *rev_host)
{
	int ttl_val=64, msg_count=0, i, addr_len, flag=1,
			msg_received_count=0;
	
	struct ping_pkt pckt;
	struct sockaddr_in r_addr;
	struct timespec time_start, time_end, tfs, tfe;
	long double rtt_msec=0, total_msec=0;
	struct timeval tv_out;
	tv_out.tv_sec = RECV_TIMEOUT;
	tv_out.tv_usec = 0;

	clock_gettime(CLOCK_MONOTONIC, &tfs);

	
	// set socket options at ip to TTL and value to 64,
	// change to what you want by setting ttl_val
	if (setsockopt(ping_sockfd, SOL_IP, IP_TTL,
			&ttl_val, sizeof(ttl_val)) != 0)
	{
		printf("\nSetting socket options
				to TTL failed!\n");
		return;
	}

	else
	{
		printf("\nSocket set to TTL..\n");
	}

	// setting timeout of recv setting
	setsockopt(ping_sockfd, SOL_SOCKET, SO_RCVTIMEO,
				(const char*)&tv_out, sizeof tv_out);

	// send icmp packet in an infinite loop
	while(pingloop)
	{
		// flag is whether packet was sent or not
		flag=1;
	
		//filling packet
		bzero(&pckt, sizeof(pckt));
		
		pckt.hdr.type = ICMP_ECHO;
		pckt.hdr.un.echo.id = getpid();
		
		for ( i = 0; i < sizeof(pckt.msg)-1; i++ )
			pckt.msg[i] = i+'0';
		
		pckt.msg[i] = 0;
		pckt.hdr.un.echo.sequence = msg_count++;
		pckt.hdr.checksum = checksum(&pckt, sizeof(pckt));


		usleep(PING_SLEEP_RATE);

		//send packet
		clock_gettime(CLOCK_MONOTONIC, &time_start);
		if ( sendto(ping_sockfd, &pckt, sizeof(pckt), 0,
		(struct sockaddr*) ping_addr,
			sizeof(*ping_addr)) <= 0)
		{
			printf("\nPacket Sending Failed!\n");
			flag=0;
		}

		//receive packet
		addr_len=sizeof(r_addr);

		if ( recvfrom(ping_sockfd, &pckt, sizeof(pckt), 0,
			(struct sockaddr*)&r_addr, &addr_len) <= 0
			&& msg_count>1)
		{
			printf("\nPacket receive failed!\n");
		}

		else
		{
			clock_gettime(CLOCK_MONOTONIC, &time_end);
			
			double timeElapsed = ((double)(time_end.tv_nsec -
								time_start.tv_nsec))/1000000.0;
			rtt_msec = (time_end.tv_sec-
						time_start.tv_sec) * 1000.0
						+ timeElapsed;
			
			// if packet was not sent, don't receive
			if(flag)
			{
				if(!(pckt.hdr.type ==69 && pckt.hdr.code==0))
				{
					printf("Error..Packet received with ICMP
						type %d code %d\n",
						pckt.hdr.type, pckt.hdr.code);
				}
				else
				{
					printf("%d bytes from %s (h: %s)
						(%s) msg_seq=%d ttl=%d
						rtt = %Lf ms.\n",
						PING_PKT_S, ping_dom, rev_host,
						ping_ip, msg_count,
						ttl_val, rtt_msec);

					msg_received_count++;
				}
			}
		}	
	}
	clock_gettime(CLOCK_MONOTONIC, &tfe);
	double timeElapsed = ((double)(tfe.tv_nsec -
						tfs.tv_nsec))/1000000.0;
	
	total_msec = (tfe.tv_sec-tfs.tv_sec)*1000.0+
						timeElapsed;
					
	printf("\n===%s ping statistics===\n", ping_ip);
	printf("\n%d packets sent, %d packets received, %f percent
		packet loss. Total time: %Lf ms.\n\n",
		msg_count, msg_received_count,
		((msg_count - msg_received_count)/msg_count) * 100.0,
		total_msec);
}

// Driver Code
int main(int argc, char *argv[])
{
	int sockfd;
	char *ip_addr, *reverse_hostname;
	struct sockaddr_in addr_con;
	int addrlen = sizeof(addr_con);
	char net_buf[NI_MAXHOST];

	if(argc!=2)
	{
		printf("\nFormat %s <address>\n", argv[0]);
		return 0;
	}

	ip_addr = dns_lookup(argv[1], &addr_con);
	if(ip_addr==NULL)
	{
		printf("\nDNS lookup failed! Could
				not resolve hostname!\n");
		return 0;
	}

	reverse_hostname = reverse_dns_lookup(ip_addr);
	printf("\nTrying to connect to '%s' IP: %s\n",
									argv[1], ip_addr);
	printf("\nReverse Lookup domain: %s",
						reverse_hostname);

	//socket()
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(sockfd<0)
	{
		printf("\nSocket file descriptor not received!!\n");
		return 0;
	}
	else
		printf("\nSocket file descriptor %d received\n", sockfd);

	signal(SIGINT, intHandler);//catching interrupt

	//send pings continuously
	send_ping(sockfd, &addr_con, reverse_hostname,
								ip_addr, argv[1]);
	
	return 0;
}


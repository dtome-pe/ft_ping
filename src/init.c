#define _GNU_SOURCE

#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>

#include "../inc/ft_ping.h"

void    init(t_data *data)
{

    /*RESOLVE AND COPY DST ADDRESS*/

    struct addrinfo     *result;
    struct addrinfo     *res;
    int                 error;
    char                ip_str[INET_ADDRSTRLEN];

    /* resolve the domain name into a list of addresses */
    error = getaddrinfo(data->hostname, NULL, NULL, &result);
    if (error != 0) {   
        if (error == EAI_SYSTEM) {
            perror("getaddrinfo");
        } else {
            fprintf(stderr, "./ft_ping: unknown host\n");
        }   
        exit(EXIT_FAILURE);
    }

    /* loop over all returned results and do inverse lookup */
    for (res = result; res != NULL; res = res->ai_next) {   
        if (res->ai_family == AF_INET) 
        {
            /*ALLOCATING AND COPYING SOCKADDR_IN IN DATA STRUCTURE*/
            struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;           

            addr->sin_family = AF_INET;
            addr->sin_port = 0;
            data->dest_addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));  // Ensure you allocate memory
            if (!data->dest_addr) {
                perror("Memory allocation for destination address failed");
                exit(EXIT_FAILURE);
            }
            memcpy(data->dest_addr, addr, sizeof(struct sockaddr_in));

            /*COPYING DEST IP FOR PACKET*/

            data->ip_hdr.daddr = addr->sin_addr.s_addr;
            
            break ;
        }
    }
    freeaddrinfo(result);

	/*-----------------------------------*/

    /*COPY ITS IP STRING REPRESENTATION*/

    inet_ntop(AF_INET, &data->dest_addr->sin_addr, data->hostname_ip_str, sizeof(data->hostname_ip_str));

    /*------------------------*/

    /*RAW SOCKET CREATION*/
    int s;

    s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (s < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    data->ping_fd = s;

    /*-----------------------------------*/

    /*TELL THE KERNEL NOT TO INCLUDE IP HEADER*/

    int optval = 1;
    
    if (setsockopt(data->ping_fd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval)) < 0)
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    /*--------------------------------------*/

    /*GET IP ADDRESS OF OWN COMPUTER*/
    struct ifreq ifr;
    char array[] = "nordlynx";

    //Type of address to retrieve - IPv4 IP address
    ifr.ifr_addr.sa_family = AF_INET;
    //Copy the interface name in the ifreq structure
    strncpy(ifr.ifr_name , array , IFNAMSIZ - 1);
    if (ioctl(data->ping_fd, SIOCGIFADDR, &ifr) < 0)
    {
        perror("ioctl failed to get IP address");
        exit(EXIT_FAILURE);
    }
    //display result
    //printf("IP Address is %s - %s\n" , array , inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) );

    /*------------------------------------------*/

    /*BIND SOCKET TO ADDRESS*/

    data->from_addr = (struct sockaddr_in *)&ifr.ifr_addr;

    if (bind(data->ping_fd, (const struct sockaddr*)data->from_addr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    /*----------------------------------------------*/

    /*STORE SRC ADDR INTO IP_HEADER STRUCTURE AND ITS STRING IP REPRESENTATION*/

    struct sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_addr;
    data->ip_hdr.saddr = sin->sin_addr.s_addr;

    /*-----------------------------------------------*/

    /*SET PING INTERVAL*/

    data->interval.tv_sec = 1;
    data->interval.tv_usec = 0;

    /*---------------------------*/

    /*INITIALIZE STATS VALUES*/

    data->stats.seq = 0;
    data->stats.packets_sent = 0;
    data->stats.packets_received = 0;
    data->stats.min_rtt = 0;
    data->stats.max_rtt = 0;
    data->stats.total_rtt = 0;
    data->stats.rtt_list_head = NULL;
    data->stats.rtt_count = 0;

    /*-------------------------*/
}
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

void    get_own_IP()
{

}

void    init(t_data *data, char *dst)
{
    /*RAW SOCKET CREATION*/
    int s;

    s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (s < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    /*-----------------------------------*/

    /*TELL THE KERNEL NOT TO INCLUDE IP HEADER*/

    int optval = 1;
    
    if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval)) < 0)
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    /*--------------------------------------*/

    /*GET IP ADDRESS OF OWN COMPUTER*/
    struct ifreq ifr;
    char array[] = INET;

    //Type of address to retrieve - IPv4 IP address
    ifr.ifr_addr.sa_family = AF_INET;
    //Copy the interface name in the ifreq structure
    strncpy(ifr.ifr_name , array , IFNAMSIZ - 1);
    if (ioctl(s, SIOCGIFADDR, &ifr) < 0)
    {
        perror("ioctl failed to get IP address");
        exit(EXIT_FAILURE);
    }
    //display result
    //printf("IP Address is %s - %s\n" , array , inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) );

    /*------------------------------------------*/

    /*BIND SOCKET TO ADDRESS*/

    struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;

    if (bind(s, (const struct sockaddr*)addr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    /*----------------------------------------------*/

    /*STORE SRC ADDR INTO IP_HEADER STRUCTURE*/

    struct sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_addr;
    data->ip_header.src_ip = ntohl(sin->sin_addr.s_addr);

    /*-----------------------------------------------*/

    /*GET AND COPY DST ADDRESS*/

    struct addrinfo     *result;
    struct addrinfo     *res;
    int                 error;
    char                ip_str[INET_ADDRSTRLEN];

    /* resolve the domain name into a list of addresses */
    error = getaddrinfo(dst, NULL, NULL, &result);
    if (error != 0) {   
        if (error == EAI_SYSTEM) {
            perror("getaddrinfo");
        } else {
            fprintf(stderr, "error in getaddrinfo: %s\n", gai_strerror(error));
        }   
        exit(EXIT_FAILURE);
    }

    /* loop over all returned results and do inverse lookup */
    for (res = result; res != NULL; res = res->ai_next) {   
        if (res->ai_family == AF_INET) 
        {
            struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
            //memcpy(addr_buffer, &addr->sin_addr, sizeof(addr->sin_addr));
            data->ip_header.dest_ip = ntohl(addr->sin_addr.s_addr);

            // Print for verification
            //print_addr(addr_buffer, *addr_len);
        }
    }
    freeaddrinfo(result);

    /*------------------------*/
}
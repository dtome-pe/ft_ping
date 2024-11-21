#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>  // For struct sockaddr_in
#include <arpa/inet.h>   // For socket-related functions
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include "../inc/ft_ping.h"

int volatile stop = 0;

void
sig_int (int signal)
{
    stop = 1;
    exit(1);
}

void    echo(t_data *data)
{
    if (sendto(data->ping_fd, data->packet, data->packet_size, 0, (const struct sockaddr *)data->dest_addr, sizeof(struct sockaddr_in)) < 0) 
    {
        perror("Packet send failed");
        free(data->packet);
        exit(EXIT_FAILURE);
    }

    printf("Packet sent successfully\n");
    
    free(data->packet);
}

int     receive(t_data *data)
{
    int                 n;
    socklen_t len = sizeof(*(data->dest_addr));
    n = recvfrom(data->ping_fd, data->ping_buffer, 10000, 0, (struct sockaddr *)data->dest_addr, &len);
    if (n < 0)
    {
        perror("recv failed");
        exit(EXIT_FAILURE);
    }
    if (n > 0) 
    {
        printf("ICMP packet received from %s\n", inet_ntoa(data->dest_addr->sin_addr));
        struct icmphdr *icmp = (struct icmphdr *) (data->ping_buffer + sizeof(struct iphdr));
        switch (icmp->type) 
        {
            case ICMP_ECHOREPLY:      printf("Type: ICMP_ECHOREPLY\n"); break;
            case ICMP_DEST_UNREACH:   printf("Type: ICMP_DEST_UNREACH\n"); break;
            case ICMP_SOURCE_QUENCH:  printf("Type: ICMP_SOURCE_QUENCH\n"); break;
            case ICMP_REDIRECT:       printf("Type: ICMP_REDIRECT\n"); break;
            case ICMP_ECHO:           printf("Type: ICMP_ECHO\n"); break;
            case ICMP_TIME_EXCEEDED:  printf("Type: ICMP_TIME_EXCEEDED\n"); break;
            case ICMP_PARAMETERPROB:  printf("Type: ICMP_PARAMETERPROB\n"); break;
            case ICMP_TIMESTAMP:      printf("Type: ICMP_TIMESTAMP\n"); break;
            case ICMP_TIMESTAMPREPLY: printf("Type: ICMP_TIMESTAMPREPLY\n"); break;
            case ICMP_INFO_REQUEST:   printf("Type: ICMP_INFO_REQUEST\n"); break;
            case ICMP_INFO_REPLY:     printf("Type: ICMP_INFO_REPLY\n"); break;
            case ICMP_ADDRESS:        printf("Type: ICMP_ADDRESS\n"); break;
            case ICMP_ADDRESSREPLY:   printf("Type: ICMP_ADDRESSREPLY\n"); break;
            default:                  printf("Type: <0x%02x>\n", icmp->type); break;
        }
    }
}

void    run(t_data *data)
{

    /*PING LOOP*/

    int             n;
    int             fdmax = data->ping_fd + 1;
    fd_set          fdset;
    struct timeval  resp_time, last, now;

    memset(&resp_time, 0, sizeof(resp_time));
    memset(&last, 0, sizeof(last));
    memset(&now, 0, sizeof(now));

    signal (SIGINT, sig_int);
    gettimeofday(&last, NULL);
    echo(data);

    while (!stop)
    {
        FD_ZERO(&fdset);
        FD_SET(data->ping_fd, &fdset);
        gettimeofday (&now, NULL);


        n = select(fdmax, &fdset, NULL, NULL, &resp_time);
        if (n < 0)
        {
            if (errno != EINTR)
            {
                perror("select failed");
                exit(EXIT_FAILURE);
            }
            continue;
        }
        else
        {
            receive(data);
        }
    }
}
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>  // For struct sockaddr_in
#include <arpa/inet.h>   // For socket-related functions
#include <signal.h>

#include "../inc/ft_ping.h"

int volatile stop = 0;

void
sig_int (int signal)
{
    stop = 1;
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

void run(t_data *data)
{

    /*PING LOOP*/

    int             n;
    int             fdmax = data->ping_fd + 1;
    fd_set          fdset;
    struct timeval  resp_time, last;

    signal (SIGINT, sig_int);
    gettimeofday(&last, NULL);
    echo(data);

    while (!stop)
    {
        FD_ZERO(&fdset);
        FD_SET(data->ping_fd, &fdset);
        n = select(data->ping_fd, &fdset, NULL, NULL, &resp_time);
        if (n < 0)
        {
            perror("select failed\n");
            exit(EXIT_FAILURE);
        }
    }

}
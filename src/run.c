#define _GNU_SOURCE

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
#include <sys/time.h>

#include "../inc/ft_ping.h"

int volatile stop = 0;

// Calculate the checksum for the ICMP packet
unsigned short checksum(void *b, int len)
{
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void sig_int (int signal)
{
    stop = 1;
}

void    run(t_data *data)
{

    /*PING LOOP*/

    int             n;
    int             fdmax = data->ping_fd + 1;
    fd_set          fdset;
    struct timeval  resp_time, last, now, start, elapsed;

    memset(&resp_time, 0, sizeof(resp_time));
    memset(&last, 0, sizeof(last));
    memset(&now, 0, sizeof(now));
    memset(&start, 0, sizeof(start));
    memset(&elapsed, 0, sizeof(elapsed));

    signal (SIGINT, sig_int);
    echo(data);
    gettimeofday(&start, NULL);
    gettimeofday(&last, NULL);

    while (!stop)
    {
        FD_ZERO(&fdset);
        FD_SET(data->ping_fd, &fdset);
        gettimeofday (&now, NULL);

        if (data->opts.w > 0)
            if (check_timeout(now, start, elapsed, data) == 1)
                break;

        set_resp_time(&resp_time, last, now, data);

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
        else if (n == 1)
        {
            receive(data, &last);
        }
        else
        {
            echo(data);
            gettimeofday(&last, NULL);
        }
    }
}
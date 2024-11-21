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

void    echo(t_data *data)
{   
    generate_headers(data);
    
    struct timeval tv;
    memset (&tv, 0, sizeof(tv));
    printf("size of timeval: %zu\n", sizeof(tv));
    gettimeofday(&tv, NULL);

    // Calculate total packet size (ICMP header + payload)
    size_t packet_size = sizeof(data->ip_hdr) + sizeof(data->icmp_hdr) + sizeof(tv) + 40;

    char *packet = malloc(packet_size);
    if (!packet)
    {
        perror("Failed to allocate memory for packet");
        exit(EXIT_FAILURE);
    }

    /* Copy IP header, ICMP header, and payload (struct timeval) into the packet */
    memcpy(packet, &data->ip_hdr, sizeof(data->ip_hdr));
    memcpy(packet + sizeof(data->ip_hdr), &data->icmp_hdr, sizeof(data->icmp_hdr));
    memcpy(packet + sizeof(data->ip_hdr) + sizeof(data->icmp_hdr), &tv, sizeof(tv));
    memset(packet + sizeof(data->ip_hdr) + sizeof(data->icmp_hdr) + sizeof(tv), 0, 40);

    /*-------------------------------------*/

    data->icmp_hdr.checksum = calculate_checksum(packet + sizeof(data->ip_hdr), sizeof(data->icmp_hdr) + sizeof(tv) + 40);
    
    /* Now that the checksum is calculated, copy the updated ICMP header into the packet */
    memcpy(packet + sizeof(data->ip_hdr), &data->icmp_hdr, sizeof(data->icmp_hdr));

    data->ip_hdr.tot_len = sizeof(data->ip_hdr) + sizeof(data->icmp_hdr) + sizeof(tv) + 40;
    printf("iphdr tot len: %d\n", data->ip_hdr.tot_len);

    memcpy(packet, &data->ip_hdr, sizeof(data->ip_hdr));

    if (sendto(data->ping_fd, packet, packet_size, 0, (const struct sockaddr *)data->dest_addr, sizeof(struct sockaddr_in)) < 0) 
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
        data->last_reply = (struct icmphdr *) (data->ping_buffer + sizeof(struct iphdr));
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
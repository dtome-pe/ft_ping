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

void    echo(t_data *data)
{   
    generate_headers(data);
    
    struct timeval tv;
    memset (&tv, 0, sizeof(tv));
    gettimeofday(&tv, NULL);

    // Calculate total packet size (ICMP header + payload)
    size_t packet_size = sizeof(data->ip_hdr) + sizeof(data->icmp_hdr) + sizeof(tv) + 40;
    data->packet_size = packet_size;
    data->payload_size = sizeof(tv) + 40;

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
    
    memcpy(packet + sizeof(data->ip_hdr), &data->icmp_hdr, sizeof(data->icmp_hdr));

    data->ip_hdr.tot_len = sizeof(data->ip_hdr) + sizeof(data->icmp_hdr) + sizeof(tv) + 40;

    memcpy(packet, &data->ip_hdr, sizeof(data->ip_hdr));

    if (sendto(data->ping_fd, packet, packet_size, 0, (const struct sockaddr *)data->dest_addr, sizeof(struct sockaddr_in)) < 0) 
    {
        perror("Packet send failed");
        free(data->packet);
        exit(EXIT_FAILURE);
    }

    if (data->stats.packets_sent == 0)
	{
		printf("PING %s (%s): %zu data bytes", data->hostname, data->hostname_ip_str, data->payload_size);
		if (data->opts.verbose)
			printf(", id 0x%x = %d\n", ntohs(data->icmp_hdr.un.echo.id), ntohs(data->icmp_hdr.un.echo.id));
		printf("\n");
	}

    data->stats.packets_sent++;
    data->stats.seq++;    
    free(packet);
}

int     receive(t_data *data, struct timeval *last)
{
    size_t                 n;
    socklen_t len = sizeof(*(data->dest_addr));
    struct timeval      reply, rtt;

    n = recvfrom(data->ping_fd, data->ping_buffer, 10000, 0, (struct sockaddr *)data->dest_addr, &len);
    if (n < 0)
    {
        perror("recv failed");
        exit(EXIT_FAILURE);
    }

    gettimeofday(&reply, NULL);

    // Parse the IP header from the received packet
    struct iphdr *ip_header = (struct iphdr *)data->ping_buffer;

    struct icmphdr *icmp_header = (struct icmphdr *)(data->ping_buffer + (ip_header->ihl * 4));
	char dest_ip[INET_ADDRSTRLEN];

	printf("%s\n", get_icmp_message_type(icmp_header->type));

    if (icmp_header->type == ICMP_TIME_EXCEEDED)
    {
		data->code = 1;
        printf("%lu bytes from %s: Time to live exceeded", n - sizeof(*ip_header), inet_ntop(AF_INET, &ip_header->saddr, dest_ip, INET_ADDRSTRLEN));
		if (data->opts.verbose)
			print_headers(&data->ip_hdr, &data->icmp_hdr);
		printf("\n");

    }
	else if (icmp_header->type == ICMP_DEST_UNREACH)
	{
		data->code = 1;
		printf("%lu bytes from %s: Destination Host Unreachable\n", n - sizeof(*ip_header), inet_ntop(AF_INET, &ip_header->saddr, dest_ip, INET_ADDRSTRLEN));
		if (data->opts.verbose)
			print_headers(&data->ip_hdr, &data->icmp_hdr);
		printf("\n");
	}
	else
	{
		data->stats.packets_received++;

		// Extract the sender's IP address
		inet_ntop(AF_INET, &data->dest_addr->sin_addr, data->hostname_ip_str, sizeof(data->hostname_ip_str));

		/*CALCULATE RTT AND ADD TO LIST*/
		timersub(&reply, last, &rtt);
		double rtt_ms = rtt.tv_sec * 1000.0 + rtt.tv_usec / 1000.0;
		
		handle_rtt(data, rtt_ms);

		/*------------------------------*/

		// Display information
		printf("%lu bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
		n - (ip_header->ihl * 4),  // Data size (without IP header)
		data->hostname_ip_str,
		ntohs(icmp_header->un.echo.sequence),
		ip_header->ttl,
		rtt_ms);
	}


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
    struct timeval  resp_time, last, now;

    memset(&resp_time, 0, sizeof(resp_time));
    memset(&last, 0, sizeof(last));
    memset(&now, 0, sizeof(now));

    signal (SIGINT, sig_int);
    echo(data);
    gettimeofday(&last, NULL);

    while (!stop)
    {
        FD_ZERO(&fdset);
        FD_SET(data->ping_fd, &fdset);
        gettimeofday (&now, NULL);

        resp_time.tv_sec = last.tv_sec + data->interval.tv_sec - now.tv_sec;
        resp_time.tv_usec = last.tv_usec + data->interval.tv_usec - now.tv_usec;

        while (resp_time.tv_usec < 0)
        {
            resp_time.tv_usec += 1000000;
            resp_time.tv_sec--;
        }
        while (resp_time.tv_usec >= 1000000)
        {
            resp_time.tv_usec -= 1000000;
            resp_time.tv_sec++;
        }

        if (resp_time.tv_sec < 0)
            resp_time.tv_sec = resp_time.tv_usec = 0;

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
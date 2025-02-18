#include "../inc/ft_ping.h"
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>  // For struct sockaddr_in
#include <arpa/inet.h>   // For socket-related functions

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

	//printf("%s\n", get_icmp_message_type(icmp_header->type));

    if (icmp_header->type == ICMP_TIME_EXCEEDED)
    {
		data->code = 1;
        printf("%lu bytes from %s: Time to live exceeded\n", n - sizeof(*ip_header), inet_ntop(AF_INET, &ip_header->saddr, dest_ip, INET_ADDRSTRLEN));
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
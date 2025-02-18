#include "../inc/ft_ping.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

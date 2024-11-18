#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>  // For struct sockaddr_in
#include <arpa/inet.h>   // For socket-related functions

#include "../inc/ft_ping.h"

void allocate_and_send(t_data *data)
{

    /*ALLOCATE MEMORY FOR PACKET*/
    size_t packet_size = sizeof(data->ip_header) + sizeof(data->icmp_header);
    printf("packet size: %ld\n", packet_size);
    data->packet = malloc(packet_size);
    if (!data->packet) {
        perror("Packet allocation failed");
        exit(EXIT_FAILURE);
    }
    memset(data->packet, 0, packet_size); // Initialize all bytes to zero

    /*--------------------------*/

    /*COPY BOTH HEADERS INTO PACKET*/

    // Copy the IP header into the packet
    memcpy(data->packet, &data->ip_header, sizeof(data->ip_header));

    // Copy the ICMP header into the packet, right after the IP header
    memcpy(data->packet + sizeof(data->ip_header), &data->icmp_header, sizeof(data->icmp_header));

    /*---------------------------*/

    /*SEND THE PACKET*/

    // printf("socket: %d\npacket: %s\n", data->socket, data->packet);
    // print_binary(data->packet, packet_size);
    // printf("dest addr family:%d\n", data->dest_addr->sin_family);
    // printf("dest family port: %d\n", data->dest_addr->sin_port);

    // printf("ip header size: %ld\n", sizeof(data->ip_header));

    // for (int i = 0; i < packet_size; i++) {
    //     printf("Byte %d: 0x%02x\n", i, ((uint8_t *)data->packet)[i]);
    // }

    if (sendto(data->socket, data->packet, packet_size, 0, (const struct sockaddr *)data->dest_addr, sizeof(struct sockaddr_in)) < 0) {
        perror("Packet send failed");
        free(data->packet);
        exit(EXIT_FAILURE);
    }

    printf("Packet sent successfully\n");
    
    free(data->packet);

    /*-----------------------------*/

}